// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Vendor, Inc. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/freezer.h>
#include <linux/reboot.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/namei.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/reboot.h>
#include <linux/watchdog.h>
#include <soc/mediatek/mtk-pm.h>

#define  DEVICE_NAME "swupdev"
#define  CLASS_NAME  "swup"


extern int mtdchar_erase(int start, int length, struct mtd_info *mtd);
extern struct mtd_info * get_mtd_info(int minor);
extern int mtd_block_isbad(struct mtd_info *mtd, loff_t ofs);
extern int mtd_block_markbad(struct mtd_info *mtd, loff_t ofs);
extern void roku_wdt_keepalive(void);

MODULE_LICENSE("GPL v2");            ///< The license type -- this affects available functionality

static struct proc_dir_entry *swupdev_entry;
static int    majorNumber;
static char*  kbuf;
static struct class*  swupClass;
static struct device* swupDevice;
static uint32_t	swupd_length_written;
static struct mtd_info *mtd;

static uint8_t* bbtTable;
static int bbtLen;
static int all_blocks_written;
static int commit_to_flash;
static int write_swup_magic;
// The prototype functions for the character driver -- must come before the struct definition
static int     swupd_dev_open(struct inode *, struct file *);
static int     swupd_dev_release(struct inode *, struct file *);
static ssize_t swupd_dev_write(struct file *, const char *, size_t, loff_t *);
static loff_t  swupd_dev_llseek(struct file *, loff_t, int);
static ssize_t swupd_dev_read(struct file *, char *, size_t, loff_t *);

typedef struct {
    unsigned int _d0[2];
    unsigned int magic;
    unsigned int magic2;
    unsigned int _d1[2];
    unsigned int type;
    unsigned int length;
    unsigned int data_length;
    unsigned int _d2[3];
    unsigned int flags;
    unsigned int build_time;
    unsigned int host_offset;
    unsigned int signature[32];
    unsigned int hash[5];
    unsigned int boot_version;
    unsigned int reserved[7];
    unsigned int iv[4];
} aimage_v1_header_t;

#define MSU_SWUP_MAGIC_NEW_FIRMWARE     0x6e657731
#define MSU_SWUP_MAGIC_TRIAL_FIRMWARE   0x74727932

/*
 * Data structure to hold the pointer to the mtd device as well
 * as mode information of various use cases.
 */
struct mtd_file_info {
  struct mtd_info *mtd;
  enum mtd_file_modes mode;
};

/* BBT offset start from the last block */
#define SWUPBBT_BLK_OFF 0x1000

/* BBT table checksum field size */
#define SWUPBBT_CSUM_SIZE sizeof(uint32_t)

/* Number of blocks covered by each BBT bytes */
#define SWUPBBT_N_BLK_PER_BYTE 4

/*
 * Mark all block as good in the BBT buffer
 * bbt - the start of BBT buffer
 * len - the lengh of BBT buffer include 4 byte checksum field in the end
 *
 * All following function parameters have the same meaning
 */

static inline void swupbbt_clear_bbt(uint8_t *bbt, size_t len)
{
	memset(bbt, 0xff, len);
}

/* Check if a block index in BBT buffer has bad block status */
static inline int swupbbt_block_isbad(uint8_t *bbt, unsigned blk)
{
	return ((bbt[blk >> 2] >> ((blk & 0x03) << 1)) & 0x03) != 0x03;
}

/* Mark one block index in BBT buffer as bad block status */
static inline void swupbbt_block_markbad(uint8_t *bbt, unsigned blk)
{
	bbt[blk >> 2] &= ~(0x01 << ((blk & 0x03) << 1));
}

/* Verify if BBT is valid by verifing the checksum in its end */
static inline int swupbbt_verify_checksum(uint8_t *bbt, size_t len)
{
	uint32_t csum = 0;
	size_t i;
	for (i = 0; i < len - SWUPBBT_CSUM_SIZE; i++)
		csum += (uint8_t)~bbt[i];
	csum = ~csum;
	return memcmp(&bbt[len-SWUPBBT_CSUM_SIZE], &csum, SWUPBBT_CSUM_SIZE);
}

/* Generate checksum at the end of the BBT buffer */
static inline uint32_t swupbbt_gen_checksum(uint8_t *bbt, size_t len)
{
	uint32_t csum = 0;
	size_t i;
	for (i = 0; i < len; i++)
	{
		csum += (uint8_t)~bbt[i];
	}
	csum = ~csum;
	return csum;
}

static void boot_rescue(void)
{
	pm_set_boot_reason(PM_BR_REBOOT_UPDATE);
}

static struct mtd_info *get_mtd(char *path) {
	struct kstat fw_stat;
	struct path fw_path;
	struct mtd_info *mtd;

	if (kern_path(path, LOOKUP_FOLLOW, &fw_path)) {
		printk(KERN_ALERT "Couldn't find firmware device\n");
		return NULL;
	}
	if (vfs_getattr(&fw_path, &fw_stat, STATX_ALL, KSTAT_QUERY_FLAGS)) {
		printk(KERN_ALERT "Couldn't stat firmware device\n");
		return NULL;
	}
	mtd = get_mtd_info(MINOR(fw_stat.rdev));
	if (!mtd) {
		printk(KERN_ALERT "Couldn't get mtd of firmware device\n");
		return NULL;
	}

	return mtd;
}

static struct mtd_info *get_firmware_mtd(void)
{
    return get_mtd("/dev/mtd_Firmware");
}

static long swupd_ioctl(struct file *file, u_int cmd, u_long arg)
{
	void __user *argp = (void __user *)arg;
	int ret = 0;
	u_long size;
	struct mtd_info_user info;

	size = (cmd & IOCSIZE_MASK) >> IOCSIZE_SHIFT;
	if (cmd & IOC_IN) {
	if (!access_ok(argp, size))
		return -EFAULT;
	}
	if (cmd & IOC_OUT) {
	if (!access_ok(argp, size))
		return -EFAULT;
	}

	switch (cmd) {
	case MEMGETREGIONCOUNT:
	mtd = get_firmware_mtd();

	if (copy_to_user(argp, &(mtd->numeraseregions), sizeof(int)))
		return -EFAULT;
	break;

	case MEMGETREGIONINFO:
	{
	uint32_t ur_idx;
	struct mtd_erase_region_info *kr;
	struct region_info_user __user *ur = argp;

	if (get_user(ur_idx, &(ur->regionindex)))
		return -EFAULT;

	mtd = get_firmware_mtd();

	if (ur_idx >= mtd->numeraseregions)
		return -EINVAL;

	kr = &(mtd->eraseregions[ur_idx]);

	if (put_user(kr->offset, &(ur->offset))
		|| put_user(kr->erasesize, &(ur->erasesize))
		|| put_user(kr->numblocks, &(ur->numblocks)))
		return -EFAULT;
	}

	case MEMGETINFO:
	{
	memset(&info, 0, sizeof(info));
	mtd = get_firmware_mtd();
	info.type = mtd->type;
	info.flags  = mtd->flags;
	info.size = mtd->size;
	info.erasesize  = mtd->erasesize;
	info.writesize  = mtd->writesize;
	info.oobsize  = mtd->oobsize;
	/* The below field is obsolete*/
	info.padding  = 0;
	if (copy_to_user(argp, &info, sizeof(struct mtd_info_user)))
	  return -EFAULT;
	break;
	}
	case MEMWRITEOOB:
	case MEMGETBADBLOCK:
	case MEMERASE:
	case MEMERASE64:
	{
	return 0;
	}

	case MTDFILEMODE:
	{
	ret = 0;
	break;
	}

	// We overload the MEMLOCK ioctl to indicate that all blocks have been written
	// to the RAM buffer.
	case MEMLOCK:
	{
	ret = 0;
	all_blocks_written = (swupd_length_written != 0);
	break;
	}

	default:
	ret = -ENOTTY;
	}

	return ret;
} /* memory_ioctl */

static struct file_operations fops =
{
	.open = swupd_dev_open,
	.write = swupd_dev_write,
	.read = swupd_dev_read,
	.llseek = swupd_dev_llseek,
	.release = swupd_dev_release,
	.unlocked_ioctl = swupd_ioctl,
};

#define SWUPBBT_ENABLE 0

#if SWUPBBT_ENABLE
static void flush_bbt(void)
{
//MESON doesn't need to update the BBT at the end of partition
#if !defined(CONFIG_ARCH_MESON)
	size_t retlen;
	uint32_t bbtPos = mtd->size - mtd->erasesize;
	uint32_t bbtWriteSize = mtd->writesize;
	uint32_t checkSum = swupbbt_gen_checksum(bbtTable, bbtLen);
	char* tempBuf = (char*)vmalloc(bbtWriteSize * sizeof(char));
	int eraseRet;
	int writeRet;

	// This code assumes that BBT fits in a single write block and that the
	// SWUPBBT_BLK_OFF is an integer multiple of write block size.

	memset(tempBuf, 0xff, bbtWriteSize);
	memcpy(bbtTable + bbtLen, &checkSum, SWUPBBT_CSUM_SIZE);
	memcpy(tempBuf, bbtTable, bbtLen + SWUPBBT_CSUM_SIZE);

	eraseRet = mtdchar_erase(bbtPos, mtd->erasesize, mtd);
	writeRet = mtd_write(mtd, bbtPos + SWUPBBT_BLK_OFF, bbtWriteSize, &retlen, tempBuf);
	vfree(tempBuf);
#endif
}
#endif

static void write_to_mtd(void)
{
	uint32_t eraseLength = (loff_t)mtd->erasesize;
	uint32_t writeBlockLength = (loff_t)mtd->writesize;
	uint32_t epos = 0;
	uint32_t wpos = 0;
	uint32_t rpos = 0;
	uint32_t firstWpos = 0;
	size_t retlen;
	uint32_t totalRemaining = swupd_length_written;
	loff_t blockRemaining;
	uint32_t blockWrite;
	int eraseRet;
	char* tempBuf;
	int badBlockRet;
	int writeRet = 0;
	int swupmagic_location = 0;
	int swupmagic = MSU_SWUP_MAGIC_NEW_FIRMWARE;
	int write_first_block = 0;

	if(!mtd){
		printk("mtd is null, update failed\n");
		return;
	}

	if(freeze_processes()) {
		printk("Unable to freeze user processes, aborting flash write\n");
		return;
	}

#if SWUPBBT_ENABLE
	swupbbt_clear_bbt(bbtTable, bbtLen);
#endif

	//Loop to erase blocks, possibly marking and advancing past bad blocks.
	while (totalRemaining > 0) {
		roku_wdt_keepalive();
		if(totalRemaining < eraseLength)
			blockRemaining = totalRemaining;
		else blockRemaining = eraseLength;

		//Check if block is bad, and add to bbt if bad
		badBlockRet = mtd_block_isbad(mtd, epos);

		if(badBlockRet != 0)
		{
#if SWUPBBT_ENABLE
			swupbbt_block_markbad(bbtTable,epos/eraseLength);
#endif
			printk("Found a bad block at erase offset: %d, skipping block returncode: %d\n", epos, badBlockRet);
			epos += eraseLength;
			wpos += eraseLength;
			continue;
		}

		eraseRet = mtdchar_erase(epos, eraseLength, mtd);

		//Erase a block, add to bbt on failure
		if(eraseRet < 0)
		{
			printk("Erase failed at erase offset: %d, skipping block returncode: %d\n", epos, eraseRet);
#if SWUPBBT_ENABLE
			swupbbt_block_markbad(bbtTable,epos/eraseLength);
#endif

#if defined(CONFIG_ARCH_MESON)
			//MESON doesn't need to update the BBT at the end of partition
			mtd_block_markbad(mtd, epos);
#endif
			epos += eraseLength;
			wpos += eraseLength;
			continue;
		}

		//Loop to write full write blocks within an erase block.
		while(blockRemaining > 0)
		{
			if(blockRemaining < writeBlockLength)
				blockWrite = blockRemaining;
			else blockWrite = writeBlockLength;

			//Do not write the very first block until the end.
			if(rpos == 0 && totalRemaining > eraseLength)
			{
				firstWpos = wpos;
				wpos += eraseLength;
				rpos += eraseLength;
				totalRemaining -= eraseLength;
				write_first_block = 1;
				break;
			}

			//Create padded buffer to always write page aligned
			tempBuf = (char*)vmalloc(writeBlockLength * sizeof(char));
			memcpy(tempBuf, kbuf+rpos, blockWrite);
			if(blockWrite < writeBlockLength)
			{
				memset(tempBuf + blockWrite, 0xff, writeBlockLength - blockWrite);
			}

			writeRet = mtd_write(mtd, wpos, writeBlockLength, &retlen, tempBuf, 0);
			vfree(tempBuf);
			if(writeRet < 0)
			{
				printk("Write failed at Pos: %u, size: %llu, len: %u\n", wpos, mtd->size, blockWrite);
				// break out of loops
				blockRemaining = 0;
				totalRemaining = 0;
				continue;
			}

			wpos += writeBlockLength;
			rpos += blockWrite;
			blockRemaining -= blockWrite;
			totalRemaining -= blockWrite;
		}

		epos += eraseLength;

		//printk("Software Update in progress, written so far: %u, remaining: %u bytes\n", epos, totalRemaining);
	}

	if (write_swup_magic) {
		// Find the next good block and store SWUP Magic to MSU_SWUP_MAGIC_NEW_FIRMWARE let boot loader know that software update is pending.
		// Its not required to update SWUPBBT since the SWUP Magic will always be written on the next good block and the physical offset of this
		// block is stored in the first aimage header. This block is also outside the purview of the mtdblock_robbs driver used to mount rootfs.
		wpos = epos; // Write SWUP Magic value at the next erase block
		while ((swupmagic_location == 0) && (epos < mtd->size))
		{
			//Check if block is bad, and add to bbt if bad
			badBlockRet = mtd_block_isbad(mtd, epos);

			if(badBlockRet != 0)
			{
				printk("Found a bad block at erase offset: %d, skipping block returncode: %d\n", epos, badBlockRet);
				epos += eraseLength;
				wpos += eraseLength;
				continue;
			}

			eraseRet = mtdchar_erase(epos, eraseLength, mtd);

			//Erase a block, add to bbt on failure
			if(eraseRet < 0)
			{
				printk("Erase failed at erase offset: %d, skipping block returncode: %d\n", epos, eraseRet);
				epos += eraseLength;
				wpos += eraseLength;
				continue;
			}

			//Create padded buffer to always write page aligned
			tempBuf = (char*)vmalloc(writeBlockLength * sizeof(char));
			memset(tempBuf, 0xff, writeBlockLength * sizeof(char));
			memcpy(tempBuf, (const char *)&swupmagic, sizeof(swupmagic));
			writeRet = mtd_write(mtd, wpos, writeBlockLength, &retlen, tempBuf, 0);
			vfree(tempBuf);
			if(writeRet < 0)
			{
				printk("Write failed at Pos: %u, size: %llu, len: %u\n", wpos, mtd->size, blockWrite);
				epos += eraseLength;
				wpos += eraseLength;
				continue;
			}
			swupmagic_location = wpos / eraseLength; // Store block offset from beginning of firmware partition.
			printk("SWUP Magic written at %u\n", swupmagic_location);
			break;
		}

		if (!swupmagic_location) {
			printk("Unable to write SWUP Magic\n");
		}
	}
	else {
		printk("Not writing SWUP Magic\n");
	}

	if (writeRet >= 0 && write_first_block) {
		// Set swup magic location
		aimage_v1_header_t *hdr;
		hdr = (aimage_v1_header_t *)kbuf;
		hdr->boot_version = swupmagic_location;

		//Write the very first block. Block already erased.
		blockRemaining = eraseLength;
		wpos = firstWpos;
		rpos = 0;
		while(blockRemaining > 0)
		{
			if(blockRemaining < writeBlockLength)
				blockWrite = blockRemaining;
			else blockWrite = writeBlockLength;

			//Create padded buffer to always write page aligned
			tempBuf = (char*)vmalloc(writeBlockLength * sizeof(char));
			memcpy(tempBuf, kbuf+rpos, blockWrite);
			if(blockWrite < writeBlockLength)
			{
				memset(tempBuf + blockWrite, 0xff, writeBlockLength - blockWrite);
			}

			writeRet = mtd_write(mtd, wpos, writeBlockLength, &retlen, tempBuf, 0);
			vfree(tempBuf);
			if(writeRet < 0)
			{
				printk("Write failed at Pos: %u, size: %llu, len: %u\n", wpos, mtd->size, blockWrite);
				// break out of loops
				blockRemaining = 0;
				totalRemaining = 0;
				break;
			}

			wpos += writeBlockLength;
			rpos += blockWrite;
			blockRemaining -= blockWrite;
		}
	}

#if SWUPBBT_ENABLE
	if (writeRet >= 0) {
		flush_bbt();
	}
#endif

	kernel_restart(NULL);
}

#define USER_PROC_BUFFER_SIZE 15
static ssize_t swupdev_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	size_t requested_count = count;
	char buffer[USER_PROC_BUFFER_SIZE];

	if (!count)
		return count;

	if (count >= USER_PROC_BUFFER_SIZE)
		count = USER_PROC_BUFFER_SIZE - 1;

	if (copy_from_user(buffer, buf, count))
		return -EFAULT;

	buffer[count] = '\0';

	if (!strncmp(buffer, "commit", 6)) {
		commit_to_flash = all_blocks_written;
		if (commit_to_flash)
			write_to_mtd();
	} else if (!strncmp(buffer, "memlock", 7)) {
		all_blocks_written = (swupd_length_written != 0);
	} else if (!strncmp(buffer, "writeswupmagic", 14)) {
		write_swup_magic = 1;
	} else if (!strncmp(buffer, "abort", 5)) {
		/* Free kbuf on abort. If userspace writes to the device file again then
    * the write handler will allocate a new buffer */
	 	if (kbuf){
    	vfree(kbuf);
    	kbuf = NULL;
		}
		if (bbtTable){
    	vfree(bbtTable);
			bbtTable = NULL;
		}

		write_swup_magic = 0;
		swupd_length_written = 0;
		all_blocks_written = 0;
		commit_to_flash = 0;
	} else if (!strncmp(buffer, "bootrescue", 10)) {
		boot_rescue();
	} else {
		return -EINVAL;
	}
	/*
	 * Must return original count, or most programs will call
	 * write again with any bytes beyond the original
	 * USER_PROC_BUFFER_SIZE - 1
	 */
	return requested_count;
}

static int swupdev_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "Buffer level %u, buffer is %svalid, buffer will %sbe committed\n",
		swupd_length_written, all_blocks_written?"":"not ", commit_to_flash?"":"not ");

	return 0;
}

static int swupdev_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, swupdev_proc_show, NULL);
}

static struct file_operations proc_ops = {
	.open = swupdev_proc_open,
	.read = seq_read,
	.write = swupdev_proc_write,
	.llseek = seq_lseek,
	.release = single_release
};

static int __init swup_init(void){
	swupdev_entry = proc_create("swupdev", 0666, NULL, &proc_ops);
	if (NULL == swupdev_entry){
		printk(KERN_WARNING "swupdev: create proc file failed!\n");
		return -ENOMEM;
	}

	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0){
		printk(KERN_ALERT "SWUPChar failed to register a major number\n");
		return majorNumber;
	}

	// Register the device class
	swupClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(swupClass)){                // Check for error and clean up if there is
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(swupClass);          // Correct way to return an error on a pointer
	}
	// Register the device driver
	swupDevice = device_create(swupClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(swupDevice)){               // Clean up if there is an error
		class_destroy(swupClass);           // Repeated code but the alternative is goto statements
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(swupDevice);
	}
	return 0;
}

static void __exit swup_exit(void){

	if(kbuf){
  	vfree(kbuf);
		kbuf = NULL;
	}
	if (bbtTable){
		vfree(bbtTable);
		bbtTable = NULL;
	}
	device_destroy(swupClass, MKDEV(majorNumber, 0));     // remove the device
	class_unregister(swupClass);                          // unregister the device class
	class_destroy(swupClass);                             // remove the device class
	unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number

	if(swupdev_entry){
		proc_remove(swupdev_entry);
	}
}

static int swupd_dev_open(struct inode *inodep, struct file *filep){
	unsigned long mtd_size;

	mtd = get_firmware_mtd();
	mtd_size = mtd->size;
	//If we don't have a valid buffer from a previous open allocate a bad block
	//table and map the RAM buffer.
	if(!all_blocks_written) {
		if(bbtTable) vfree(bbtTable);
		bbtLen = (mtd_size/mtd->erasesize) / SWUPBBT_N_BLK_PER_BYTE;
		bbtTable = (uint8_t*) vmalloc(bbtLen + SWUPBBT_CSUM_SIZE);
		if(bbtTable == NULL) return -ENOMEM;

		kbuf = (char*) vmalloc(mtd_size);
		if(kbuf == NULL) return -ENOMEM;
		swupd_length_written = 0;
		write_swup_magic = 0;
	}

	return 0;
}

static ssize_t swupd_dev_write(struct file *filep, const char *buf, size_t len, loff_t *offset){
	unsigned long mtd_size;

	mtd = get_firmware_mtd();
	mtd_size = mtd->size;
	//A new write operation means the data buffer is not yet valid.
	all_blocks_written = 0;
	commit_to_flash = 0;

	if (kbuf == NULL)
		return -EFAULT;
	if (*offset + len > mtd_size) {
		/* Something bad happened, don't allow the commit to proceed. */
		swupd_length_written = 0;
		return -ENOSPC;
	}

	//Accrue data as its written to the driver
	if (copy_from_user(kbuf + *offset, buf, len)) {
		return -EFAULT;
	}
	swupd_length_written += len;
	*offset += len;

	return len;
}

static loff_t swupd_dev_llseek(struct file *file, loff_t offset, int orig)
{
	switch(orig) {
		case SEEK_SET:
			file->f_pos = offset;
			break;
		case SEEK_CUR:
			offset += file->f_pos;
			file->f_pos = offset;
			break;
		case SEEK_END:
			file->f_pos = swupd_length_written;
			break;
		default: /* can't happen */
			return -EINVAL;
	}
	return file->f_pos;
}

static ssize_t swupd_dev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	if(!all_blocks_written) {
		return 0;
	}
	if (*ppos + count > swupd_length_written)
		count = swupd_length_written - *ppos;
	if (copy_to_user(buf, kbuf + *ppos, count)) {
		return -EFAULT;
	}
	*ppos = *ppos + count;
	return count;
}

static int swupd_dev_release(struct inode *inodep, struct file *filep){
	//If data is marked for commit write it to mtd at device close. Otherwise
	//preserve it for a future commit.
	if(commit_to_flash)
		write_to_mtd();

	return 0;
}

module_init(swup_init);
module_exit(swup_exit);

