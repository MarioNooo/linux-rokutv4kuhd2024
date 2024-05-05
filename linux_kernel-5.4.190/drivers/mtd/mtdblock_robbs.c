/*
 * Direct MTD block device access, readonly with NAND bad block skipping
 *
 * (C) 2007 Andre McCurdy, NXP Semiconductors
 * (C) 2000-2003 Nicolas Pitre <nico@cam.org>
 * (C) 1999-2003 David Woodhouse <dwmw2@infradead.org>
 */
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/vmalloc.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/blktrans.h>
#include <linux/mutex.h>
#include <linux/printk.h>
//#include <linux/iblockstruct.h>
#include <linux/reboot.h>
#include <linux/ctype.h>
//#include <linux/roku.h>
static int roku_get_fskey(int size, u8 *roku_fs_key);
// --------------------------------------------------------------------
// Definitions dependent on Linux version.

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,1)
#define RELEASE_RETURN_TYPE       void
#define RELEASE_RETURN            return
#else
#define RELEASE_RETURN_TYPE       int
#define RELEASE_RETURN            return 0
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,1)
#define MTD_BLOCK_ISBAD(mtd,pos)  mtd_block_isbad(mtd,pos)
#define MTD_READ(mtd,from,len,retlen,buf,enc) mtd_read(mtd,from,len,retlen,buf,enc)
#else
#define MTD_BLOCK_ISBAD(mtd,pos)  (mtd)->block_isbad(mtd,pos)
#define MTD_READ(mtd,from,len,retlen,buf,enc) (mtd)->read(mtd,from,len,retlen,buf,enc)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,100)
#define SECTOR_OFFSET_TYPE        unsigned long
#else
#define SECTOR_OFFSET_TYPE        unsigned long
#endif

// --------------------------------------------------------------------
// Configurable parameters.
// FIXME: it would probably be better to have all of these defined directly in the
// config file, rather than defined here conditionally based on architecture defines.

#define AIMAGE_DECRYPTION




#ifdef AIMAGE_DECRYPTION

// Authenticate cramfs data here in the driver.
// If not defined, should use a dm-auth mapper in authenticate_root.
#if defined(CONFIG_MSTAR_KIWI) // littlefield
#define ROBBS_AUTHENTICATE_REGION (64*1024*1024)
#elif (defined(CONFIG_ARCH_BCM2708) || defined(CONFIG_ARCH_BCM2709)) && defined(CONFIG_512MB_DDR) // sugarland, briscoe
#define ROBBS_AUTHENTICATE_REGION (48*1024*1024)
#endif

// Reserved area at start of cramfs image.
#if defined(CONFIG_ARCH_BCM2708) || defined(CONFIG_ARCH_CAPRI) // giga, austin
#define CRAMFS_OFFSET        (512*1024)
#else
#define CRAMFS_OFFSET        (0)
#endif

// Fields in aimage header that should be zeroed when read.
#if !defined(CONFIG_BCM7468B0) // everything except paolo
#define ROBBS_ZERO_BOOTVERSION
#endif
#if defined(CONFIG_SIGMA_SOC_SX7) || defined(CONFIG_MSTAR_CHIP) // dfwr, mstar
#define ROBBS_ZERO_SIGNATURE 128
#elif defined(CONFIG_MIPS_TRIDENT_UXL) // liberty
#define ROBBS_ZERO_SIGNATURE 64
#endif

// Different ways of getting the filesystem key.
// [FIXME] I check ROBBS_FSKEY_FROM_TEE on R819
//         R819 doesn't enable it
// #define ROBBS_FSKEY_FROM_TEE

#if defined(CONFIG_MSTAR_CURRY)
#elif defined(CONFIG_MSTAR_KIWI)
extern uint8_t _text[];
#define ROBBS_FSKEY_VAR (_text+32)
#elif defined(CONFIG_MSTAR_M7632) || defined(CONFIG_MSTAR_M7332) || defined(CONFIG_MSTAR_MT5867)
extern uint8_t roku_fs_key[16];
#define ROBBS_FSKEY_VAR roku_fs_key
#elif defined(CONFIG_SIGMA_SOC_SX7)
extern int sx7_get_fs_key(unsigned char const* src, unsigned char* dst);
#define ROBBS_FSKEY_FUNCTION sx7_get_fs_key
#else
extern int decrypt_16_with_private_key(unsigned char* src, unsigned char* dst);
#define ROBBS_FSKEY_FUNCTION decrypt_16_with_private_key
#endif

#if defined(CONFIG_SIGMA_SOC_SX7) // dfwr
static uint32_t const fskey_seed[4] = { 0x8a49b50d, 0x1c5a3569, 0x45699cf7, 0x27f5ba25 };
#define FSKEY_SEED fskey_seed
#elif defined(CONFIG_MSTAR_CHIP) // mstar
static uint32_t const fskey_seed[4] = { 0x8a49b50d, 0x1c5a3569, 0x45699cf7, 0x27f5ba25 };
#define FSKEY_SEED fskey_seed
#endif

// Filesystem test key for debugging.
////#define DEBUG_FSKEY "abcdefghijklmnop" // DO NOT CHECK-IN WITH THIS DEFINED!
#ifdef DEBUG_FSKEY
#warning ##teamcity[buildStatus status='FAILURE' text='Using debug fs key in mtdblock_robbs.']
#endif

// Use swup bad block table.
#if defined(CONFIG_ARCH_BCM2708) || defined(CONFIG_ARCH_CAPRI) || defined(CONFIG_MSTAR_CHIP) // giga, austin, mstar
#define ROBBS_SWUPBBT
#endif

// --------------------------------------------------------------------
// Includes and externs.

#include <crypto/skcipher.h>
#include <linux/scatterlist.h>
#include <linux/random.h>
#endif /* AIMAGE_DECRYPTION */

extern int erase_write(struct mtd_info *mtd, SECTOR_OFFSET_TYPE pos, int len, const char *buf);

#define ROBBS_FSKEY_FROM_TEE 1
#ifdef ROBBS_FSKEY_FROM_TEE
// ignored this function
// extern int __init roku_get_teedata(ptrdiff_t offset, size_t length, u8 *buf);
#endif

// --------------------------------------------------------------------
// Definitions and types.

#ifdef ROBBS_SWUPBBT
#include <linux/mtd/swupbbt.h>
#define SWUPBBT_BLOCK_ISBAD(mtdblk, blk) (mtdblk->bbt_cache && swupbbt_block_isbad(mtdblk->bbt_cache, blk))
#else
#define SWUPBBT_BLOCK_ISBAD(mtdblk, blk) 0
#endif

#define DEV_BLOCKSIZE        (512)             /* fixed by MTD block driver infrastructure */
#define CACHE_SIZE           (4 * 1024)        /* must be a multiple of DEV_BLOCKSIZE and a fraction of NAND erase block size */
#define CACHE_BASE_INVALID   ((unsigned long) -1)

#define ROBBS_NAME           "mtdblock_robbs"
#define ROBBS_MAJOR          44
#define ROBBS_RAW_NAME       "mtdblock_robbs_raw"
#define ROBBS_RAW_MAJOR      48
#if CRAMFS_OFFSET > 0
#define ROBBS_FULL_NAME      "mtdblock_robbs_full"
#define ROBBS_FULL_MAJOR     45
#endif

#ifdef ROBBS_AUTHENTICATE_REGION
#define MAX_ROBBS_SIZE       (ROBBS_AUTHENTICATE_REGION - CRAMFS_OFFSET)
#define MAX_ROBBS_BLOCKS     (MAX_ROBBS_SIZE/4096*16)
#endif

#ifdef AIMAGE_DECRYPTION

#define IMG_MAGIC                       0x41676d69
#define IMG_MAGIC2                      0x43634d52
#define IMG_MAGIC2B                     0x47634d52
#define IMG_TYPE_INITFS_CRAMFS          (0x0A)
#define IMG_TYPE_APP_CRAMFS             (0x0D)
#define IMG_TYPE_OMV_CRAMFS             (0x11)
#define IMG_TYPE_OMV_ASSETS             (0x07)
#define IMG_FLAG_ENCMODE_NONE           (0 << 6)
#define IMG_FLAG_ENCMODE_CBC_ONEPASS    (1 << 6)
#define IMG_FLAG_ENCMODE_CBC_4KBLOCKS   (2 << 6)
#define IMG_FLAG_ENCMODE_CTR            (3 << 6)
#define IMG_FLAG_ENCMODE_MASK           (7 << 6)

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

struct encrypted_region {
    struct encrypted_region* next;
    unsigned long start_pos;
    unsigned long block_pos;
    unsigned long length;
    unsigned char iv_prefix[12];
};
#endif // AIMAGE_DECRYPTION

struct mtdblk_dev {
    struct mtd_blktrans_dev mbd;
    int count;
    unsigned int bblist_count;
    unsigned long *bblist_data;
    unsigned long cache_base;
    unsigned long cramfs_offset;
    struct mutex cache_mutex;
#ifdef AIMAGE_DECRYPTION
    struct encrypted_region* encrypted_regions;
    unsigned char key[16];
    unsigned int aimage_length;
    struct crypto_skcipher *tfm_decrypt;
#endif
#ifdef ROBBS_AUTHENTICATE_REGION
    struct crypto_hash* hash;
#endif
    unsigned char cache_buffer[CACHE_SIZE];
#ifdef ROBBS_SWUPBBT
    uint8_t *bbt_cache;
#endif
};

// --------------------------------------------------------------------
// Static data.

static DEFINE_MUTEX(mtdblks_lock);

#ifdef ROBBS_AUTHENTICATE_REGION
uint32_t mtdblock_robbs_block_count = MAX_ROBBS_BLOCKS;
uint8_t mtdblock_robbs_hashes[MAX_ROBBS_BLOCKS] = {1}; // initialize to put in .data, not .bss
#endif

#ifdef ROBBS_FSKEY_FROM_TEE
static u8 teekey[16];
#endif


// --------------------------------------------------------------------
#ifdef AIMAGE_DECRYPTION
static int decrypt(struct mtdblk_dev *mtdblk, const u8 *data,
		   unsigned int data_size, const u8 *key, unsigned int key_size,
		   const u8 *iv, unsigned int iv_size)
{
	struct skcipher_request *req = NULL;
	struct scatterlist sg;
	DECLARE_CRYPTO_WAIT(wait);
	int result;

	result = crypto_skcipher_setkey(mtdblk->tfm_decrypt, key, key_size);
	if (result) {
		pr_err("Error setting key: %d\n", result);
		return result;
	}

	/* Allocate a request object */
	req = skcipher_request_alloc(mtdblk->tfm_decrypt, GFP_KERNEL);
	if (!req)
		return -ENOMEM;

	sg_init_one(&sg, data, data_size);
	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG |
				      CRYPTO_TFM_REQ_MAY_SLEEP,
				      crypto_req_done, &wait);
	skcipher_request_set_crypt(req, &sg, &sg, data_size, (void *)iv);
	result = crypto_wait_req(crypto_skcipher_decrypt(req), &wait);
	if (result)
		pr_err("Error encrypting data: %d\n", result);

	skcipher_request_free(req);

	return result;
}


static struct encrypted_region* get_encrypted_region(struct mtdblk_dev* mtdblk, unsigned long pos, unsigned long len)
{
    struct encrypted_region* region;

    for (region = mtdblk->encrypted_regions;  region != NULL;  region = region->next) {
        if (pos + len > region->start_pos && pos < region->start_pos + region->length)
            return region;
    }
    return NULL;
}
#endif

// --------------------------------------------------------------------
static int mtdblock_robbs_cache_load(struct mtdblk_dev *mtdblk, unsigned long pos_phys, unsigned long pos_virt)
{
    struct mtd_info *mtd = mtdblk->mbd.mtd;
    size_t retlen;
    int ret;

    if (pos_phys & (CACHE_SIZE-1)) {
        printk("%s: pos_phys (0x%08lx) not CACHE_SIZE aligned ?!?\n", __FUNCTION__, pos_phys);
        return -EIO;
    }

    mutex_lock(&mtdblk->cache_mutex);

    mtdblk->cache_base = CACHE_BASE_INVALID;
    ret = MTD_READ(mtd, pos_phys, CACHE_SIZE, &retlen, mtdblk->cache_buffer, 0);
    if (ret != 0 && ret != -EUCLEAN)
        goto out;
    mtdblk->cache_base = (retlen == CACHE_SIZE) ? pos_phys : CACHE_BASE_INVALID;

#ifdef ROBBS_ZERO_BOOTVERSION
{
    unsigned long const pos_boot_version = offsetof(aimage_v1_header_t, boot_version);
    if (mtdblk->mbd.tr->major == ROBBS_MAJOR && mtd->index == 1 && pos_phys <= pos_boot_version) {
        // Force boot_version in aimage hdr to zero.
        // This is how it was when the Merkle tree was generated,
        // but it may have been changed by the boot code since then.
        aimage_v1_header_t* ai = 0;
        memset(mtdblk->cache_buffer + pos_boot_version - pos_phys, 0, sizeof(ai->boot_version));
#ifdef ROBBS_ZERO_SIGNATURE
        {
        unsigned long const pos_signature = offsetof(aimage_v1_header_t, signature);
        memset(mtdblk->cache_buffer + pos_signature - pos_phys, 0, ROBBS_ZERO_SIGNATURE);
        }
#endif
    }
}
#endif

#ifdef ROBBS_AUTHENTICATE_REGION
    if (pos_virt < mtdblock_robbs_block_count*CACHE_SIZE) {
        struct hash_desc hashdesc;
        struct scatterlist sg;
        unsigned char hash_value[16];

        hashdesc.tfm = mtdblk->hash;
        hashdesc.flags = 0;
        crypto_hash_init(&hashdesc);
        sg_init_one(&sg, mtdblk->cache_buffer, CACHE_SIZE);
        crypto_hash_update(&hashdesc, &sg, CACHE_SIZE);
        crypto_hash_final(&hashdesc, hash_value);
        if (memcmp(hash_value, mtdblock_robbs_hashes+pos_virt/4096*16, 16) != 0) {
            printk("Hash check failed: %lx\n", pos_virt);
            memset(mtdblk->cache_buffer, 0, CACHE_SIZE);
            mtdblk->cache_base = CACHE_BASE_INVALID;
            kernel_restart(NULL);
            ret = -EACCES;
            goto out;
        }
    }
#endif

#ifdef AIMAGE_DECRYPTION
{
    struct encrypted_region* region = get_encrypted_region(mtdblk, pos_virt, CACHE_SIZE);
    if (region != NULL) {
        /* Some part (maybe all) of this data is encrypted.
         * Decrypt the encrypted part. */
        unsigned int enc_start_block;
        unsigned char* src;
        unsigned int size;
        unsigned char iv[16];

        src = mtdblk->cache_buffer;
        size = CACHE_SIZE;
        BUG_ON(region->block_pos > pos_virt);
        enc_start_block = (pos_virt - region->block_pos) / 16;
        if (region->start_pos > pos_virt) {
            /* There are unencrypted bytes at the beginning of the buffer. */
            unsigned long unenc = region->start_pos - pos_virt;
            BUG_ON(unenc >= CACHE_SIZE);
            src += unenc;
            size -= unenc;
            enc_start_block += unenc / 16;
        }
        if (pos_virt + CACHE_SIZE > region->start_pos + region->length) {
            /* There are unencrypted bytes at the end of the buffer. */
            size -= (pos_virt + CACHE_SIZE) - (region->start_pos + region->length);
        }
        memcpy(iv, region->iv_prefix, 12);
        iv[12] = (enc_start_block >> 24) & 0xFF;
        iv[13] = (enc_start_block >> 16) & 0xFF;
        iv[14] = (enc_start_block >> 8) & 0xFF;
        iv[15] = (enc_start_block >> 0) & 0xFF;
        ret = decrypt(mtdblk, src, size, mtdblk->key, sizeof(mtdblk->key), iv, sizeof(iv));
        if (ret < 0)
            goto out;
    }
}
#endif

out:
    mutex_unlock(&mtdblk->cache_mutex);

    if (ret && ret != -EUCLEAN)
        return ret;
    if (retlen != CACHE_SIZE)
        return -EIO;

    return 0;
}

// --------------------------------------------------------------------
static unsigned long robbs_offset(struct mtd_blktrans_dev *dev, unsigned long pos_base)
{
    struct mtdblk_dev *mtdblk = container_of(dev, struct mtdblk_dev, mbd);
    struct mtd_info *mtd = mtdblk->mbd.mtd;
    int i;

    if (mtdblk->bblist_count != 0) {
        for (i = 0; i < mtdblk->bblist_count; i++) {
            if (pos_base < mtdblk->bblist_data[i])
                break;
            pos_base += mtd->erasesize;
            if (pos_base >= mtd->size) {
                printk("%s: %s 0x%08lx slipped out of range (0x%08llx)\n",
                    __FUNCTION__, mtd->name, pos_base, mtd->size);
                return -1;
            }
        }
    }
    return pos_base;
}

// --------------------------------------------------------------------
static int mtdblock_robbs_readsect(struct mtd_blktrans_dev *dev, SECTOR_OFFSET_TYPE lba, char *buf)
{
    struct mtdblk_dev *mtdblk = container_of(dev, struct mtdblk_dev, mbd);
    unsigned long pos_abs = mtdblk->cramfs_offset + lba * DEV_BLOCKSIZE;
    unsigned long pos_base = (pos_abs / CACHE_SIZE) * CACHE_SIZE;
    unsigned long pos_offset = pos_abs - pos_base;
    unsigned long pos_base_original = pos_base - mtdblk->cramfs_offset;
    int result;

    pos_base = robbs_offset(dev, pos_base);
    if (pos_base == (unsigned long)-1)
        return -EIO;

    if (pos_base != mtdblk->cache_base)
        if ((result = mtdblock_robbs_cache_load(mtdblk, pos_base, pos_base_original)) != 0)
            return result;

    memcpy(buf, mtdblk->cache_buffer + pos_offset, DEV_BLOCKSIZE);
    return 0;
}

// --------------------------------------------------------------------
#ifdef AIMAGE_DECRYPTION
static int decrypt_img_key(unsigned char* key)
{
#ifdef ROBBS_FSKEY_FROM_TEE
    static u8 localcopy[16];
    static int havit = 0;
    if (!havit) {
        memcpy(localcopy, teekey, 16);
        memset(teekey, 0, 16);
        havit = 1;
    }
    memcpy(key, localcopy, 16);
    return 0;
#elif defined(ROBBS_FSKEY_VAR)
    memcpy(key, ROBBS_FSKEY_VAR, 16);
    return 0;
#elif defined(ROBBS_FSKEY_FUNCTION)
    // return ROBBS_FSKEY_FUNCTION(key, key);
    return 0;
#else
    #error No fskey method defined!
#endif
}
#endif

// --------------------------------------------------------------------
#ifdef AIMAGE_DECRYPTION
// Read the last block of an encrypted partition and get the fs key from it.
static int get_img_info(struct mtd_blktrans_dev *mbd, aimage_v1_header_t *ahdr)
{
    struct mtdblk_dev *mtdblk = container_of(mbd, struct mtdblk_dev, mbd);
    int ret = 0;

    /* We don't yet know whether there are any encrypted aimages on this partition.
     * So get the key unconditionally just in case. */
    if (1) {
        /* Decrypt encrypted key to get the real key. */
#ifdef FSKEY_SEED
        memcpy(mtdblk->key, FSKEY_SEED, sizeof(mtdblk->key));
#endif
        ret = decrypt_img_key(mtdblk->key);
#ifdef DEBUG_FSKEY // overwrite with debug key
        memcpy(mtdblk->key, DEBUG_FSKEY, sizeof(mtdblk->key));
#endif
    }
    return ret;
}
#endif

// --------------------------------------------------------------------
static bool is_dev_unit;
static int __init is_dev_unit_setup(char *s)
{
    if (s[0] == '1' && s[1] == 0)
        is_dev_unit = 1;
	return 0; // allow other __setup("dev=")
}
__setup("dev=", is_dev_unit_setup);

// --------------------------------------------------------------------
/**
 * mtdblock_load_swupbbt - Read softare upgrade BBT from active partition
 *
 * The BBT will be loaded in order to map out bad blocks in activee partition
 * so that cramfs rootfs can be correctly mounted. If error occurs, ignore
 * BBT and assume no new bad blocks by filling cache copy with 0xff.
 */
#ifdef ROBBS_SWUPBBT
static int mtdblock_load_swupbbt(struct mtdblk_dev *mtdblk)
{
    struct mtd_info *mtd = mtdblk->mbd.mtd;
    loff_t bbt_off;
    uint32_t bbt_len;
    size_t retlen;
    int ret = 0;

    /* Already initialized */
    if (mtdblk->bbt_cache)
        return 0;

    /* Allocate BBT cache */
    /* Force 32 bit division since 64 bit div doesn't work and heavy,
       this also means limiting mtd->size to 4GB which is enough */
    bbt_len = (uint32_t)mtd->size / mtd->erasesize / SWUPBBT_N_BLK_PER_BYTE
        + SWUPBBT_CSUM_SIZE;
    if (bbt_len <= SWUPBBT_CSUM_SIZE) {
        pr_err("%s: Invalid BBT length %d\n", __func__, bbt_len);
        return -EINVAL;
    }
    mtdblk->bbt_cache = kmalloc(bbt_len, GFP_KERNEL);
    if (!mtdblk->bbt_cache)
        return -ENOMEM;
    swupbbt_clear_bbt(mtdblk->bbt_cache, bbt_len);

    /* Read BBT in the last block of the partition, same block as iblock */
    bbt_off = mtd->size - mtd->erasesize + SWUPBBT_BLK_OFF;
    ret = MTD_READ(mtd, bbt_off, bbt_len, &retlen, mtdblk->bbt_cache, 0);
    if (ret < 0) {
        pr_err("%s: Error reading BBT 0x%08llx\n", __func__, bbt_off);
        goto read_err;
    }

    /* Verify checksum of BBT content */
    if (swupbbt_verify_checksum(mtdblk->bbt_cache, bbt_len)) {
        pr_warn("%s: Ignore BBT with bad checksum\n", __func__);
        ret = -EINVAL;
        goto read_err;
    }

    pr_info("%s: Load BBT length %d total %d bad blocks\n",
           __func__, bbt_len, swupbbt_total_badblock(mtdblk->bbt_cache, bbt_len));
    return 0;

read_err:
    swupbbt_clear_bbt(mtdblk->bbt_cache, bbt_len);
    return 0;
}
#endif

// --------------------------------------------------------------------
static int mtdblock_robbs_open(struct mtd_blktrans_dev *mbd)
{
    struct mtdblk_dev *mtdblk = container_of(mbd, struct mtdblk_dev, mbd);
    struct mtd_info *mtd = mbd->mtd;
    unsigned long pos;
    unsigned int count;
    unsigned int blk;
    int ret = 0;

    // Get fskey from device tree
    roku_get_fskey(16, &teekey[0]);
    mutex_lock(&mtdblks_lock);
    if (mtdblk->count) {
        mtdblk->count++;
        printk("mtdblock_open: %s, count: %d\n", mtd->name, mtdblk->count);
        mutex_unlock(&mtdblks_lock);
        return 0;
    }

    pr_debug("%s: %s, erasesize: %d, sizeof(struct mtdblk_dev): %d\n",
        __func__, mtd->name, mtd->erasesize, sizeof(struct mtdblk_dev));

    mtdblk->cramfs_offset = 0;
#ifdef ROBBS_FULL_MAJOR
    if (mbd->tr->major != ROBBS_FULL_MAJOR)
        mtdblk->cramfs_offset = CRAMFS_OFFSET;
#endif

#ifdef ROBBS_SWUPBBT
    ret = mtdblock_load_swupbbt(mtdblk);
    if (ret < 0)
        goto out;
#endif

    mtdblk->bblist_count = 0;
    blk = 0;
    for (pos = 0; pos < mtd->size; pos += mtd->erasesize) {
        if (MTD_BLOCK_ISBAD(mtd, pos) || SWUPBBT_BLOCK_ISBAD(mtdblk, blk)) {
            mtdblk->bblist_count++;
            pr_info("%s: badpos: 0x%08lx (count %d)\n",
                __FUNCTION__, pos, mtdblk->bblist_count);
        }
        blk++;
    }

    if (mtdblk->bblist_count != 0) {
        mtdblk->bblist_data = kmalloc(mtdblk->bblist_count * sizeof (*mtdblk->bblist_data), GFP_KERNEL);
        if (! mtdblk->bblist_data) {
            kfree(mtdblk);
            ret = -ENOMEM;
            goto out;
        }
        count = 0;
        blk = 0;
        for (pos = 0; pos < mtd->size; pos += mtd->erasesize) {
            if (MTD_BLOCK_ISBAD(mtd, pos) || SWUPBBT_BLOCK_ISBAD(mtdblk, blk)) {
                count++;
                pr_debug("%s: badpos: 0x%08lx (count %d)\n", __FUNCTION__, pos, count);
                if (count > mtdblk->bblist_count) {
                    printk("%s: increase in bad block count since first pass !?!\n", __FUNCTION__);
                    break;
                }
                mtdblk->bblist_data[count - 1] = pos;
            }
            blk++;
        }
    }

#ifdef ROBBS_AUTHENTICATE_REGION
    mtdblk->hash = crypto_alloc_hash("md5", 0, CRYPTO_ALG_ASYNC);
    if (IS_ERR(mtdblk->hash)) {
        ret = PTR_ERR(mtdblk->hash);
        goto out;
    }
#endif

    mtdblk->cache_base = CACHE_BASE_INVALID;
    mutex_init(&mtdblk->cache_mutex);

#ifdef AIMAGE_DECRYPTION
	if (!mtdblk->tfm_decrypt) {
		mtdblk->tfm_decrypt =
			crypto_alloc_skcipher("cbc(aes)", 0, 0);
        if (IS_ERR(mtdblk->tfm_decrypt)) {
            ret = PTR_ERR(mtdblk->tfm_decrypt);
            mtdblk->tfm_decrypt = NULL;
            goto out;
        }
    }
    if (mbd->tr->major == ROBBS_MAJOR) { // no decryption in RAW or FULL
        unsigned char* sector_buf = kmalloc(DEV_BLOCKSIZE, GFP_KERNEL);
        aimage_v1_header_t *header = (aimage_v1_header_t *) sector_buf;
        int first_hdr = 1;
        unsigned long pos;
        mtdblk->encrypted_regions = NULL;
        if (!sector_buf) {
            ret = -ENOMEM;
            goto out;
        }

        // Walk through the aimage headers and look for encrypted ones.
        for (pos = 0; ; ) {
            if (pos % DEV_BLOCKSIZE) {
                printk("position %ld in %s not block aligned\n", (long) pos, mtd->name);
                ret = -EINVAL;
                kfree(sector_buf);
                goto out;
            }
            if ((ret = mtdblock_robbs_readsect(mbd, pos / DEV_BLOCKSIZE, sector_buf)) != 0) {
                kfree(sector_buf);
                goto out;
            }
            if (le32_to_cpu(header->magic) != IMG_MAGIC ||
                (le32_to_cpu(header->magic2) != IMG_MAGIC2 && le32_to_cpu(header->magic2) != IMG_MAGIC2B)) {
                break;
            }
            if (first_hdr && (ret = get_img_info(mbd, header)) != 0) {
                kfree(sector_buf);
                goto out;
            }
            if ((le32_to_cpu(header->type) == IMG_TYPE_INITFS_CRAMFS ||
                 le32_to_cpu(header->type) == IMG_TYPE_APP_CRAMFS ||
                 le32_to_cpu(header->type) == IMG_TYPE_OMV_CRAMFS ||
                 le32_to_cpu(header->type) == IMG_TYPE_OMV_ASSETS) &&
                (le32_to_cpu(header->flags) & IMG_FLAG_ENCMODE_MASK) == IMG_FLAG_ENCMODE_CBC_4KBLOCKS)
            {
                struct encrypted_region* region = (struct encrypted_region*)
                    kmalloc(sizeof(struct encrypted_region), GFP_KERNEL);
                region->block_pos = pos;
                region->start_pos = pos + 256;
                region->length = le32_to_cpu(header->length) - 256;
                memcpy(region->iv_prefix, header->iv, sizeof(region->iv_prefix));
                printk("MTD encrypted region on %s at 0x%lx, length 0x%lx\n",
                    mtd->name, region->start_pos, region->length);
                mutex_lock(&mtdblk->cache_mutex);
                region->next = mtdblk->encrypted_regions;
                mtdblk->encrypted_regions = region;
                /* Any cached data may be invalid since it may have been read without decryption. */
                mtdblk->cache_base = CACHE_BASE_INVALID;
                mutex_unlock(&mtdblk->cache_mutex);
            } else if (first_hdr && !is_dev_unit) {
                /* Non-dev units must use encrypted images */
                printk(KERN_ERR "cannot use unencrypted image on non-dev unit\n");
                ret = -EPERM;
                kfree(sector_buf);
                goto out;
            }
            pos += le32_to_cpu(header->length);
            if (first_hdr && pos < CRAMFS_OFFSET)
                pos = CRAMFS_OFFSET;
            first_hdr = 0;
        }
        kfree(sector_buf);
    }
#endif /* AIMAGE_DECRYPTION */

out:
    if (ret == 0) mtdblk->count = 1;
    mutex_unlock(&mtdblks_lock);
    return ret;
}

// --------------------------------------------------------------------
static RELEASE_RETURN_TYPE mtdblock_robbs_release (struct mtd_blktrans_dev *mbd)
{
    struct mtdblk_dev *mtdblk = container_of(mbd, struct mtdblk_dev, mbd);

    mutex_lock(&mtdblks_lock);
    if (--mtdblk->count == 0) {
#ifdef AIMAGE_DECRYPTION
	if (mtdblk->tfm_decrypt) {
            crypto_free_skcipher(mtdblk->tfm_decrypt);
            mtdblk->tfm_decrypt = NULL;
        }
#endif
#ifdef ROBBS_AUTHENTICATE_REGION
        crypto_free_hash(mtdblk->hash);
#endif
        kfree(mtdblk->bblist_data);
        while (mtdblk->encrypted_regions != NULL) {
            struct encrypted_region* region = mtdblk->encrypted_regions;
            mtdblk->encrypted_regions = region->next;
            kfree(region);
        }
    }
    mutex_unlock(&mtdblks_lock);
    RELEASE_RETURN;
}

// --------------------------------------------------------------------
static void mtdblock_robbs_add_mtd(struct mtd_blktrans_ops *tr, struct mtd_info *mtd)
{
    struct mtdblk_dev *dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return;

    dev->mbd.mtd = mtd;
    dev->mbd.devnum = mtd->index;
    dev->mbd.size = mtd->size >> 9;
    dev->mbd.tr = tr;
    dev->mbd.readonly = 1;

    if (add_mtd_blktrans_dev(&dev->mbd))
        kfree(dev);
}

// --------------------------------------------------------------------
static void mtdblock_robbs_remove_dev(struct mtd_blktrans_dev *dev)
{
    del_mtd_blktrans_dev(dev);
}

// --------------------------------------------------------------------
static struct mtd_blktrans_ops mtdblock_robbs_tr =
{
    .name         = ROBBS_NAME,
    .major        = ROBBS_MAJOR,
    .part_bits    = 0,
    .blksize      = 512,
    .open         = mtdblock_robbs_open,
//  .flush        = mtdblock_robbs_flush,
    .release      = mtdblock_robbs_release,
    .readsect     = mtdblock_robbs_readsect,
    .add_mtd      = mtdblock_robbs_add_mtd,
    .remove_dev   = mtdblock_robbs_remove_dev,
    .owner        = THIS_MODULE,
};

static struct mtd_blktrans_ops mtdblock_robbs_raw_tr =
{
    .name         = ROBBS_RAW_NAME,
    .major        = ROBBS_RAW_MAJOR,
    .part_bits    = 0,
    .blksize      = 512,
    .open         = mtdblock_robbs_open,
//  .flush        = mtdblock_robbs_flush,
    .release      = mtdblock_robbs_release,
    .readsect     = mtdblock_robbs_readsect,
    .add_mtd      = mtdblock_robbs_add_mtd,
    .remove_dev   = mtdblock_robbs_remove_dev,
    .owner        = THIS_MODULE,
};

#ifdef ROBBS_FULL_MAJOR
static struct mtd_blktrans_ops mtdblock_robbs_full_tr =
{
    .name         = ROBBS_FULL_NAME,
    .major        = ROBBS_FULL_MAJOR,
    .part_bits    = 0,
    .blksize      = 512,
    .open         = mtdblock_robbs_open,
//  .flush        = mtdblock_robbs_flush,
    .release      = mtdblock_robbs_release,
    .readsect     = mtdblock_robbs_readsect,
    .add_mtd      = mtdblock_robbs_add_mtd,
    .remove_dev   = mtdblock_robbs_remove_dev,
    .owner        = THIS_MODULE,
};
#endif
static bool valid_roku_fs_key = false;
static int roku_get_fskey(int size, u8 *roku_fs_key){
	 struct device_node *dt_node;
	 const u8 *prop = NULL;
	 int len;
	 if (valid_roku_fs_key == false) {
		dt_node = of_find_node_by_path("/chosen");
		if (!dt_node) {
			pr_err("Error, dt node roku_keys not found\n");
			return -1;
		}
		prop = of_get_property(dt_node, "fskey", &len);
		if (!prop) {
			pr_err("Error, dt property fskey not found\n");
			return -1;
		}

		if (len > 32) {
			pr_err("Error, invalid fskey len %d\n", len);
		} else {
			struct property *st_prop;

			pr_info("%s: found fs_key:\n", __func__);
			memcpy(roku_fs_key, prop, size);
			int i = 0;
			for (i = 0; i < 16; i++)
				pr_info("%x ", roku_fs_key[i]);

			// Remove the key from the device tree
			st_prop = of_find_property(dt_node, "fskey", &len);
			if (!st_prop) {
				pr_err("%s: dt property fs_key not found, unable to remove\n", __func__);
			}
			else {
				if (of_remove_property(dt_node, st_prop))
					pr_warn("%s: failed to remove fs_key\n", __func__);
				else
					pr_warn("%s: removed fskey\n", __func__);
			}

			valid_roku_fs_key = true;
		}
	}
	else {
		pr_info("%s: already found fskey, skip\n", __func__);
	}
	return 0;

}

// --------------------------------------------------------------------
static int __init init_mtdblock_robbs(void)
{
    int ret;
    mutex_init(&mtdblks_lock);
    ret = register_mtd_blktrans(&mtdblock_robbs_tr);
    if (ret != 0) return ret;
    ret = register_mtd_blktrans(&mtdblock_robbs_raw_tr);
    if (ret != 0) return ret;
#ifdef ROBBS_FULL_MAJOR
    ret = register_mtd_blktrans(&mtdblock_robbs_full_tr);
    if (ret != 0) return ret;
#endif
#ifndef AIMAGE_DECRYPTION
    printk(KERN_ERR "No AIMAGE_DECRYPTION\n");
#endif
#ifdef ROBBS_FSKEY_FROM_TEE
    pr_info("Get Kfs from tee\n");
    // Get fs key from device tree could not work, becasue device tree is not reaedy
    // roku_get_fskey(16, &teekey[0]);
#endif
    return ret;
}

// --------------------------------------------------------------------
static void __exit cleanup_mtdblock_robbs(void)
{
    deregister_mtd_blktrans(&mtdblock_robbs_tr);
    deregister_mtd_blktrans(&mtdblock_robbs_raw_tr);
#ifdef ROBBS_FULL_MAJOR
    deregister_mtd_blktrans(&mtdblock_robbs_full_tr);
#endif
}

// --------------------------------------------------------------------
module_init(init_mtdblock_robbs);
module_exit(cleanup_mtdblock_robbs);

MODULE_LICENSE("GPL");

