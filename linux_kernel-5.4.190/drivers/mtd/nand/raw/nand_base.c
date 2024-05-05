// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Overview:
 *   This is the generic MTD driver for NAND flash devices. It should be
 *   capable of working with almost all NAND chips currently available.
 *
 *	Additional technical information is available on
 *	http://www.linux-mtd.infradead.org/doc/nand.html
 *
 *  Copyright (C) 2000 Steven J. Hill (sjhill@realitydiluted.com)
 *		  2002-2006 Thomas Gleixner (tglx@linutronix.de)
 *
 *  Credits:
 *	David Woodhouse for adding multichip support
 *
 *	Aleph One Ltd. and Toby Churchill Ltd. for supporting the
 *	rework for 2K page size chips
 *
 *  TODO:
 *	Enable cached programming for 2k page size chips
 *	Check, if mtd->ecctype should be set to MTD_ECC_HW
 *	if we have HW ECC support.
 *	BBT table is not serialized, has to be fixed
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#define CONFIG_ROKU_MTD_NAND_ENCRYPTION 1
#define CRYPT_BLOCK_SIZE  16

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/nand_bch.h>
#include <linux/interrupt.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/mtd/partitions.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/scatterlist.h>

#include "internals.h"

DEFINE_MUTEX(nand_crypto_mutex);
static struct crypto_blkcipher *nand_cipher;
static struct crypto_blkcipher *iv_cipher;
static unsigned char nand_key_hash[CRYPT_BLOCK_SIZE];
static unsigned char iv[CRYPT_BLOCK_SIZE];

/* Define default oob placement schemes for large and small page devices */
static int nand_ooblayout_ecc_sp(struct mtd_info *mtd, int section,
				 struct mtd_oob_region *oobregion)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_ecc_ctrl *ecc = &chip->ecc;

	if (section > 1)
		return -ERANGE;

	if (!section) {
		oobregion->offset = 0;
		if (mtd->oobsize == 16)
			oobregion->length = 4;
		else
			oobregion->length = 3;
	} else {
		if (mtd->oobsize == 8)
			return -ERANGE;

		oobregion->offset = 6;
		oobregion->length = ecc->total - 4;
	}

	return 0;
}

static int nand_ooblayout_free_sp(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *oobregion)
{
	if (section > 1)
		return -ERANGE;

	if (mtd->oobsize == 16) {
		if (section)
			return -ERANGE;

		oobregion->length = 8;
		oobregion->offset = 8;
	} else {
		oobregion->length = 2;
		if (!section)
			oobregion->offset = 3;
		else
			oobregion->offset = 6;
	}

	return 0;
}

const struct mtd_ooblayout_ops nand_ooblayout_sp_ops = {
	.ecc = nand_ooblayout_ecc_sp,
	.free = nand_ooblayout_free_sp,
};
EXPORT_SYMBOL_GPL(nand_ooblayout_sp_ops);

static int nand_ooblayout_ecc_lp(struct mtd_info *mtd, int section,
				 struct mtd_oob_region *oobregion)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_ecc_ctrl *ecc = &chip->ecc;

	if (section || !ecc->total)
		return -ERANGE;

	oobregion->length = ecc->total;
	oobregion->offset = mtd->oobsize - oobregion->length;

	return 0;
}

static int nand_ooblayout_free_lp(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *oobregion)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_ecc_ctrl *ecc = &chip->ecc;

	if (section)
		return -ERANGE;

	oobregion->length = mtd->oobsize - ecc->total - 2;
	oobregion->offset = 2;

	return 0;
}

const struct mtd_ooblayout_ops nand_ooblayout_lp_ops = {
	.ecc = nand_ooblayout_ecc_lp,
	.free = nand_ooblayout_free_lp,
};
EXPORT_SYMBOL_GPL(nand_ooblayout_lp_ops);

/*
 * Support the old "large page" layout used for 1-bit Hamming ECC where ECC
 * are placed at a fixed offset.
 */
static int nand_ooblayout_ecc_lp_hamming(struct mtd_info *mtd, int section,
					 struct mtd_oob_region *oobregion)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_ecc_ctrl *ecc = &chip->ecc;

	if (section)
		return -ERANGE;

	switch (mtd->oobsize) {
	case 64:
		oobregion->offset = 40;
		break;
	case 128:
		oobregion->offset = 80;
		break;
	default:
		return -EINVAL;
	}

	oobregion->length = ecc->total;
	if (oobregion->offset + oobregion->length > mtd->oobsize)
		return -ERANGE;

	return 0;
}

static int nand_ooblayout_free_lp_hamming(struct mtd_info *mtd, int section,
					  struct mtd_oob_region *oobregion)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_ecc_ctrl *ecc = &chip->ecc;
	int ecc_offset = 0;

	if (section < 0 || section > 1)
		return -ERANGE;

	switch (mtd->oobsize) {
	case 64:
		ecc_offset = 40;
		break;
	case 128:
		ecc_offset = 80;
		break;
	default:
		return -EINVAL;
	}

	if (section == 0) {
		oobregion->offset = 2;
		oobregion->length = ecc_offset - 2;
	} else {
		oobregion->offset = ecc_offset + ecc->total;
		oobregion->length = mtd->oobsize - oobregion->offset;
	}

	return 0;
}

static const struct mtd_ooblayout_ops nand_ooblayout_lp_hamming_ops = {
	.ecc = nand_ooblayout_ecc_lp_hamming,
	.free = nand_ooblayout_free_lp_hamming,
};

static int check_offs_len(struct nand_chip *chip, loff_t ofs, uint64_t len)
{
	int ret = 0;

	/* Start address must align on block boundary */
	if (ofs & ((1ULL << chip->phys_erase_shift) - 1)) {
		pr_debug("%s: unaligned address\n", __func__);
		ret = -EINVAL;
	}

	/* Length must align on block boundary */
	if (len & ((1ULL << chip->phys_erase_shift) - 1)) {
		pr_debug("%s: length not block aligned\n", __func__);
		ret = -EINVAL;
	}

	return ret;
}

/**
 * nand_select_target() - Select a NAND target (A.K.A. die)
 * @chip: NAND chip object
 * @cs: the CS line to select. Note that this CS id is always from the chip
 *	PoV, not the controller one
 *
 * Select a NAND target so that further operations executed on @chip go to the
 * selected NAND target.
 */
void nand_select_target(struct nand_chip *chip, unsigned int cs)
{
	/*
	 * cs should always lie between 0 and nanddev_ntargets(), when that's
	 * not the case it's a bug and the caller should be fixed.
	 */
	if (WARN_ON(cs > nanddev_ntargets(&chip->base)))
		return;

	chip->cur_cs = cs;

	if (chip->legacy.select_chip)
		chip->legacy.select_chip(chip, cs);
}
EXPORT_SYMBOL_GPL(nand_select_target);

/**
 * nand_deselect_target() - Deselect the currently selected target
 * @chip: NAND chip object
 *
 * Deselect the currently selected NAND target. The result of operations
 * executed on @chip after the target has been deselected is undefined.
 */
void nand_deselect_target(struct nand_chip *chip)
{
	if (chip->legacy.select_chip)
		chip->legacy.select_chip(chip, -1);

	chip->cur_cs = -1;
}
EXPORT_SYMBOL_GPL(nand_deselect_target);

/**
 * nand_release_device - [GENERIC] release chip
 * @chip: NAND chip object
 *
 * Release chip lock and wake up anyone waiting on the device.
 */
static void nand_release_device(struct nand_chip *chip)
{
	/* Release the controller and the chip */
	mutex_unlock(&chip->controller->lock);
	mutex_unlock(&chip->lock);
}

/**
 * nand_bbm_get_next_page - Get the next page for bad block markers
 * @chip: NAND chip object
 * @page: First page to start checking for bad block marker usage
 *
 * Returns an integer that corresponds to the page offset within a block, for
 * a page that is used to store bad block markers. If no more pages are
 * available, -EINVAL is returned.
 */
int nand_bbm_get_next_page(struct nand_chip *chip, int page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int last_page = ((mtd->erasesize - mtd->writesize) >>
			 chip->page_shift) & chip->pagemask;
	unsigned int bbm_flags = NAND_BBM_FIRSTPAGE | NAND_BBM_SECONDPAGE
		| NAND_BBM_LASTPAGE;

	if (page == 0 && !(chip->options & bbm_flags))
		return 0;
	if (page == 0 && chip->options & NAND_BBM_FIRSTPAGE)
		return 0;
	if (page <= 1 && chip->options & NAND_BBM_SECONDPAGE)
		return 1;
	if (page <= last_page && chip->options & NAND_BBM_LASTPAGE)
		return last_page;

	return -EINVAL;
}

/**
 * nand_block_bad - [DEFAULT] Read bad block marker from the chip
 * @chip: NAND chip object
 * @ofs: offset from device start
 *
 * Check, if the block is bad.
 */
static int nand_block_bad(struct nand_chip *chip, loff_t ofs)
{
	int first_page, page_offset;
	int res;
	u8 bad;

	first_page = (int)(ofs >> chip->page_shift) & chip->pagemask;
	page_offset = nand_bbm_get_next_page(chip, 0);

	while (page_offset >= 0) {
		res = chip->ecc.read_oob(chip, first_page + page_offset);
		if (res < 0)
			return res;

		bad = chip->oob_poi[chip->badblockpos];

		if (likely(chip->badblockbits == 8))
			res = bad != 0xFF;
		else
			res = hweight8(bad) < chip->badblockbits;
		if (res)
			return res;

		page_offset = nand_bbm_get_next_page(chip, page_offset + 1);
	}

	return 0;
}

static int nand_isbad_bbm(struct nand_chip *chip, loff_t ofs)
{
	if (chip->legacy.block_bad)
		return chip->legacy.block_bad(chip, ofs);

	return nand_block_bad(chip, ofs);
}

/**
 * nand_get_device - [GENERIC] Get chip for selected access
 * @chip: NAND chip structure
 *
 * Lock the device and its controller for exclusive access
 *
 * Return: -EBUSY if the chip has been suspended, 0 otherwise
 */
static int nand_get_device(struct nand_chip *chip)
{
	mutex_lock(&chip->lock);
	if (chip->suspended) {
		mutex_unlock(&chip->lock);
		return -EBUSY;
	}
	mutex_lock(&chip->controller->lock);

	return 0;
}

/**
 * nand_check_wp - [GENERIC] check if the chip is write protected
 * @chip: NAND chip object
 *
 * Check, if the device is write protected. The function expects, that the
 * device is already selected.
 */
static int nand_check_wp(struct nand_chip *chip)
{
	u8 status;
	int ret;

	/* Broken xD cards report WP despite being writable */
	if (chip->options & NAND_BROKEN_XD)
		return 0;

	/* Check the WP bit */
	ret = nand_status_op(chip, &status);
	if (ret)
		return ret;

	return status & NAND_STATUS_WP ? 0 : 1;
}

/**
 * nand_fill_oob - [INTERN] Transfer client buffer to oob
 * @chip: NAND chip object
 * @oob: oob data buffer
 * @len: oob data write length
 * @ops: oob ops structure
 */
static uint8_t *nand_fill_oob(struct nand_chip *chip, uint8_t *oob, size_t len,
			      struct mtd_oob_ops *ops)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int ret;

	/*
	 * Initialise to all 0xFF, to avoid the possibility of left over OOB
	 * data from a previous OOB read.
	 */
	memset(chip->oob_poi, 0xff, mtd->oobsize);

	switch (ops->mode) {

	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_RAW:
		memcpy(chip->oob_poi + ops->ooboffs, oob, len);
		return oob + len;

	case MTD_OPS_AUTO_OOB:
		ret = mtd_ooblayout_set_databytes(mtd, oob, chip->oob_poi,
						  ops->ooboffs, len);
		BUG_ON(ret);
		return oob + len;

	default:
		BUG();
	}
	return NULL;
}

/**
 * nand_do_write_oob - [MTD Interface] NAND write out-of-band
 * @chip: NAND chip object
 * @to: offset to write to
 * @ops: oob operation description structure
 *
 * NAND write out-of-band.
 */
static int nand_do_write_oob(struct nand_chip *chip, loff_t to,
			     struct mtd_oob_ops *ops)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int chipnr, page, status, len, ret;

	pr_debug("%s: to = 0x%08x, len = %i\n",
			 __func__, (unsigned int)to, (int)ops->ooblen);

	len = mtd_oobavail(mtd, ops);

	/* Do not allow write past end of page */
	if ((ops->ooboffs + ops->ooblen) > len) {
		pr_debug("%s: attempt to write past end of page\n",
				__func__);
		return -EINVAL;
	}

	chipnr = (int)(to >> chip->chip_shift);

	/*
	 * Reset the chip. Some chips (like the Toshiba TC5832DC found in one
	 * of my DiskOnChip 2000 test units) will clear the whole data page too
	 * if we don't do this. I have no clue why, but I seem to have 'fixed'
	 * it in the doc2000 driver in August 1999.  dwmw2.
	 */
	ret = nand_reset(chip, chipnr);
	if (ret)
		return ret;

	nand_select_target(chip, chipnr);

	/* Shift to get page */
	page = (int)(to >> chip->page_shift);

	/* Check, if it is write protected */
	if (nand_check_wp(chip)) {
		nand_deselect_target(chip);
		return -EROFS;
	}

	/* Invalidate the page cache, if we write to the cached page */
	if (page == chip->pagecache.page)
		chip->pagecache.page = -1;

	nand_fill_oob(chip, ops->oobbuf, ops->ooblen, ops);

	if (ops->mode == MTD_OPS_RAW)
		status = chip->ecc.write_oob_raw(chip, page & chip->pagemask);
	else
		status = chip->ecc.write_oob(chip, page & chip->pagemask);

	nand_deselect_target(chip);

	if (status)
		return status;

	ops->oobretlen = ops->ooblen;

	return 0;
}

/**
 * nand_default_block_markbad - [DEFAULT] mark a block bad via bad block marker
 * @chip: NAND chip object
 * @ofs: offset from device start
 *
 * This is the default implementation, which can be overridden by a hardware
 * specific driver. It provides the details for writing a bad block marker to a
 * block.
 */
static int nand_default_block_markbad(struct nand_chip *chip, loff_t ofs)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct mtd_oob_ops ops;
	uint8_t buf[2] = { 0, 0 };
	int ret = 0, res, page_offset;

	memset(&ops, 0, sizeof(ops));
	ops.oobbuf = buf;
	ops.ooboffs = chip->badblockpos;
	if (chip->options & NAND_BUSWIDTH_16) {
		ops.ooboffs &= ~0x01;
		ops.len = ops.ooblen = 2;
	} else {
		ops.len = ops.ooblen = 1;
	}
	ops.mode = MTD_OPS_PLACE_OOB;

	page_offset = nand_bbm_get_next_page(chip, 0);

	while (page_offset >= 0) {
		res = nand_do_write_oob(chip,
					ofs + (page_offset * mtd->writesize),
					&ops);

		if (!ret)
			ret = res;

		page_offset = nand_bbm_get_next_page(chip, page_offset + 1);
	}

	return ret;
}

/**
 * nand_markbad_bbm - mark a block by updating the BBM
 * @chip: NAND chip object
 * @ofs: offset of the block to mark bad
 */
int nand_markbad_bbm(struct nand_chip *chip, loff_t ofs)
{
	if (chip->legacy.block_markbad)
		return chip->legacy.block_markbad(chip, ofs);

	return nand_default_block_markbad(chip, ofs);
}

/**
 * nand_block_markbad_lowlevel - mark a block bad
 * @chip: NAND chip object
 * @ofs: offset from device start
 *
 * This function performs the generic NAND bad block marking steps (i.e., bad
 * block table(s) and/or marker(s)). We only allow the hardware driver to
 * specify how to write bad block markers to OOB (chip->legacy.block_markbad).
 *
 * We try operations in the following order:
 *
 *  (1) erase the affected block, to allow OOB marker to be written cleanly
 *  (2) write bad block marker to OOB area of affected block (unless flag
 *      NAND_BBT_NO_OOB_BBM is present)
 *  (3) update the BBT
 *
 * Note that we retain the first error encountered in (2) or (3), finish the
 * procedures, and dump the error in the end.
*/
static int nand_block_markbad_lowlevel(struct nand_chip *chip, loff_t ofs)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int res, ret = 0;

	if (!(chip->bbt_options & NAND_BBT_NO_OOB_BBM)) {
		struct erase_info einfo;

		/* Attempt erase before marking OOB */
		memset(&einfo, 0, sizeof(einfo));
		einfo.addr = ofs;
		einfo.len = 1ULL << chip->phys_erase_shift;
		nand_erase_nand(chip, &einfo, 0);

		/* Write bad block marker to OOB */
		ret = nand_get_device(chip);
		if (ret)
			return ret;

		ret = nand_markbad_bbm(chip, ofs);
		nand_release_device(chip);
	}

	/* Mark block bad in BBT */
	if (chip->bbt) {
		res = nand_markbad_bbt(chip, ofs);
		if (!ret)
			ret = res;
	}

	if (!ret)
		mtd->ecc_stats.badblocks++;

	return ret;
}

/**
 * nand_block_isreserved - [GENERIC] Check if a block is marked reserved.
 * @mtd: MTD device structure
 * @ofs: offset from device start
 *
 * Check if the block is marked as reserved.
 */
static int nand_block_isreserved(struct mtd_info *mtd, loff_t ofs)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	if (!chip->bbt)
		return 0;
	/* Return info from the table */
	return nand_isreserved_bbt(chip, ofs);
}

/**
 * nand_block_checkbad - [GENERIC] Check if a block is marked bad
 * @chip: NAND chip object
 * @ofs: offset from device start
 * @allowbbt: 1, if its allowed to access the bbt area
 *
 * Check, if the block is bad. Either by reading the bad block table or
 * calling of the scan function.
 */
static int nand_block_checkbad(struct nand_chip *chip, loff_t ofs, int allowbbt)
{
	/* Return info from the table */
	if (chip->bbt)
		return nand_isbad_bbt(chip, ofs, allowbbt);

	return nand_isbad_bbm(chip, ofs);
}

/**
 * nand_soft_waitrdy - Poll STATUS reg until RDY bit is set to 1
 * @chip: NAND chip structure
 * @timeout_ms: Timeout in ms
 *
 * Poll the STATUS register using ->exec_op() until the RDY bit becomes 1.
 * If that does not happen whitin the specified timeout, -ETIMEDOUT is
 * returned.
 *
 * This helper is intended to be used when the controller does not have access
 * to the NAND R/B pin.
 *
 * Be aware that calling this helper from an ->exec_op() implementation means
 * ->exec_op() must be re-entrant.
 *
 * Return 0 if the NAND chip is ready, a negative error otherwise.
 */
int nand_soft_waitrdy(struct nand_chip *chip, unsigned long timeout_ms)
{
	const struct nand_sdr_timings *timings;
	u8 status = 0;
	int ret;

	if (!nand_has_exec_op(chip))
		return -ENOTSUPP;

	/* Wait tWB before polling the STATUS reg. */
	timings = nand_get_sdr_timings(&chip->data_interface);
	ndelay(PSEC_TO_NSEC(timings->tWB_max));

	ret = nand_status_op(chip, NULL);
	if (ret)
		return ret;

	timeout_ms = jiffies + msecs_to_jiffies(timeout_ms);
	do {
		ret = nand_read_data_op(chip, &status, sizeof(status), true);
		if (ret)
			break;

		if (status & NAND_STATUS_READY)
			break;

		/*
		 * Typical lowest execution time for a tR on most NANDs is 10us,
		 * use this as polling delay before doing something smarter (ie.
		 * deriving a delay from the timeout value, timeout_ms/ratio).
		 */
		udelay(10);
	} while	(time_before(jiffies, timeout_ms));

	/*
	 * We have to exit READ_STATUS mode in order to read real data on the
	 * bus in case the WAITRDY instruction is preceding a DATA_IN
	 * instruction.
	 */
	nand_exit_status_op(chip);

	if (ret)
		return ret;

	return status & NAND_STATUS_READY ? 0 : -ETIMEDOUT;
};
EXPORT_SYMBOL_GPL(nand_soft_waitrdy);

/**
 * nand_gpio_waitrdy - Poll R/B GPIO pin until ready
 * @chip: NAND chip structure
 * @gpiod: GPIO descriptor of R/B pin
 * @timeout_ms: Timeout in ms
 *
 * Poll the R/B GPIO pin until it becomes ready. If that does not happen
 * whitin the specified timeout, -ETIMEDOUT is returned.
 *
 * This helper is intended to be used when the controller has access to the
 * NAND R/B pin over GPIO.
 *
 * Return 0 if the R/B pin indicates chip is ready, a negative error otherwise.
 */
int nand_gpio_waitrdy(struct nand_chip *chip, struct gpio_desc *gpiod,
		      unsigned long timeout_ms)
{

	/*
	 * Wait until R/B pin indicates chip is ready or timeout occurs.
	 * +1 below is necessary because if we are now in the last fraction
	 * of jiffy and msecs_to_jiffies is 1 then we will wait only that
	 * small jiffy fraction - possibly leading to false timeout.
	 */
	timeout_ms = jiffies + msecs_to_jiffies(timeout_ms) + 1;
	do {
		if (gpiod_get_value_cansleep(gpiod))
			return 0;

		cond_resched();
	} while	(time_before(jiffies, timeout_ms));

	return gpiod_get_value_cansleep(gpiod) ? 0 : -ETIMEDOUT;
};
EXPORT_SYMBOL_GPL(nand_gpio_waitrdy);

/**
 * panic_nand_wait - [GENERIC] wait until the command is done
 * @chip: NAND chip structure
 * @timeo: timeout
 *
 * Wait for command done. This is a helper function for nand_wait used when
 * we are in interrupt context. May happen when in panic and trying to write
 * an oops through mtdoops.
 */
void panic_nand_wait(struct nand_chip *chip, unsigned long timeo)
{
	int i;
	for (i = 0; i < timeo; i++) {
		if (chip->legacy.dev_ready) {
			if (chip->legacy.dev_ready(chip))
				break;
		} else {
			int ret;
			u8 status;

			ret = nand_read_data_op(chip, &status, sizeof(status),
						true);
			if (ret)
				return;

			if (status & NAND_STATUS_READY)
				break;
		}
		mdelay(1);
	}
}

static bool nand_supports_get_features(struct nand_chip *chip, int addr)
{
	return (chip->parameters.supports_set_get_features &&
		test_bit(addr, chip->parameters.get_feature_list));
}

static bool nand_supports_set_features(struct nand_chip *chip, int addr)
{
	return (chip->parameters.supports_set_get_features &&
		test_bit(addr, chip->parameters.set_feature_list));
}

#ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION

static int get_nv_key_from_dts(unsigned char *nv_key)
{
	struct device_node *roku_keys;
	struct property *prop;
	unsigned int tmp[4];
	int ret, i;

	roku_keys = of_find_node_by_name(NULL, "roku_keys");
	if (!roku_keys) {
		pr_err("can't find roku keys node\n");
		return -1;
	}
	//
	// trinidad:/nvram # ls -l  /sys/firmware/devicetree/base/chosen/nvkey
	// -r--r--r--    1 root     root            32 May 26 06:10 /sys/firmware/devicetree/base/chosen/nvkey

	ret = of_property_read_u32_array(roku_keys, "nvkey", tmp, 4);
	if (ret) {
		pr_info("%s, read nv key failed\n", __func__);
		return ret;
	}
	tmp[0] = cpu_to_be32(tmp[0]);
	tmp[1] = cpu_to_be32(tmp[1]);
	tmp[2] = cpu_to_be32(tmp[2]);
	tmp[3] = cpu_to_be32(tmp[3]);
	memcpy(nv_key, tmp, CRYPT_BLOCK_SIZE);
	pr_info("found nv_key:\n");
	for (i = 0; i < 16; i++)
		pr_err(" %02x ", nv_key[i]);
	pr_err("\n");

	// Remove the key from the device tree. Note that get_nand_key() already saves the key.
	prop = of_find_property(roku_keys, "nvkey", &ret); // re-use ret for len
	if (!prop) {
		pr_err("%s: dt property nv_key not found, unable to remove\n", __func__);
	}
	else {
		if (of_remove_property(roku_keys, prop))
			pr_warn("%s: failed to remove nv_key\n", __func__);
		else
			pr_warn("%s: removed nvkey\n", __func__);
	}
	return 0;
}

/*
 * Get the AES key to use for NAND encryption/decryption.
 */
static uint8_t* get_nand_key(void)
{
	printk("roku: get_nand_key() \n");
#if 1
  	static unsigned char nand_key[CRYPT_BLOCK_SIZE];
  	static int have_nand_key = 0;

  	if (!have_nand_key) {
  		if (!get_nv_key_from_dts(nand_key))
  			have_nand_key = 1;
  		else
  			return NULL;
  	}
  	return nand_key;
#else
	static uint8_t nand_key[CRYPT_BLOCK_SIZE];
	static int have_nand_key = 0;

	if (!have_nand_key) {
		/* NAND key is the box-encryption of this seed value. */
		static const uint8_t seed[CRYPT_BLOCK_SIZE] =
			{ 0x06,0xe9,0x0f,0xdf,0x1b,0x63,0x3d,0x5f,0x20,0x66,0x15,0x2d,0x1f,0x6c,0xd1,0x20 };
		if (encrypt_with_box_key(seed, nand_key, CRYPT_BLOCK_SIZE) < 0)
			return NULL;
		pr_info("mtd: nand_key: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", nand_key[0], nand_key[1], nand_key[2], nand_key[3], nand_key[4], nand_key[5], nand_key[6], nand_key[7], nand_key[8], nand_key[9], nand_key[10], nand_key[11], nand_key[12], nand_key[13], nand_key[14], nand_key[15]);
		have_nand_key = 1;
	}
	return nand_key;
#endif
}

/*
 * Get the MD5 hash of the NAND key.
 */
static uint8_t* get_nand_key_hash(void)
{
	static uint8_t nand_key_hash[CRYPT_BLOCK_SIZE];
	static int have_nand_key_hash = 0;
	if (!have_nand_key_hash) {
		uint8_t* nand_key = get_nand_key();
		struct shash_desc* desc = 0;
		int descsize;
		struct crypto_shash* hash;

		if (nand_key == NULL)
			return NULL;
		hash = crypto_alloc_shash("md5", 0, CRYPTO_ALG_ASYNC);
		if (IS_ERR(hash))
			return NULL;
		if (crypto_shash_digestsize(hash) > CRYPT_BLOCK_SIZE)
			return NULL;
		descsize = sizeof(struct shash_desc) + crypto_shash_descsize(hash);
		desc = kzalloc(descsize, GFP_KERNEL);
		if (!desc)
			return NULL;
		desc->tfm = hash;
		//desc->flags = 0; // 'struct shash_desc' has no member named 'flags'
		crypto_shash_init(desc);
		crypto_shash_digest(desc, nand_key, CRYPT_BLOCK_SIZE, nand_key_hash);
		crypto_free_shash(hash);
		kfree(desc);
		//pr_debug("mtd: nand_key_hash_hash: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", nand_key_hash[0], nand_key_hash[1], nand_key_hash[2], nand_key_hash[3], nand_key_hash[4], nand_key_hash[5], nand_key_hash[6], nand_key_hash[7], nand_key_hash[8], nand_key_hash[9], nand_key_hash[10], nand_key_hash[11], nand_key_hash[12], nand_key_hash[13], nand_key_hash[14], nand_key_hash[15]);
		have_nand_key_hash = 1;
	}
	return nand_key_hash;
}

/*
 * Allocate and initialize a cipher.
 */
int get_cipher(struct crypto_blkcipher** p_cipher, const uint8_t* key)
{
	int result;

	if (*p_cipher != NULL)
		/* Already allocated. */
		return 0;

	if (key == NULL)
		return -1;
	*p_cipher = crypto_alloc_blkcipher("cbc(aes)", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(*p_cipher)) {
		result = PTR_ERR(*p_cipher);
		*p_cipher = NULL;
		return result;
	}
	return crypto_blkcipher_setkey(*p_cipher, key, CRYPT_BLOCK_SIZE);
}

/*
 * Get the IV for a page.
 * Use ESSIV -- the IV is the encryption of the page number,
 * using the hash of the NAND key as the encryption key.
 */
# if 1
static int get_iv(int page, uint8_t* iv)
{
	printk("roku: get_iv \n");
	struct blkcipher_desc desc;
	struct scatterlist iv_sg;
	int result;

	/* First set iv = page number. */
	iv[0] = page & 0xFF;
	iv[1] = (page >> 8) & 0xFF;
	iv[2] = (page >> 16) & 0xFF;
	iv[3] = (page >> 24) & 0xFF;
	memset(&iv[4], 0, CRYPT_BLOCK_SIZE - 4);

	/* Encrypt iv using the iv_cipher. */
	/* assert(mutex_is_locked(&nand_crypto_mutex); */
	result = get_cipher(&iv_cipher, get_nand_key_hash());
	if (result < 0)
		return result;
	memset(crypto_blkcipher_crt(iv_cipher)->iv, 0, CRYPT_BLOCK_SIZE);
	desc.tfm = iv_cipher;
	desc.flags = 0;
	sg_init_one(&iv_sg, iv, CRYPT_BLOCK_SIZE);
	result = crypto_blkcipher_encrypt(&desc, &iv_sg, &iv_sg, CRYPT_BLOCK_SIZE);
	if (result < 0) {
		printk("NAND IV encrypt error %d\n", result);
		return result;
	}
	//pr_info("mtd: iv for page %d: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", page, iv[0], iv[1], iv[2], iv[3], iv[4], iv[5], iv[6], iv[7], iv[8], iv[9], iv[10], iv[11], iv[12], iv[13], iv[14], iv[15]);
	return 0;
}
#endif
/*
 * Encrypt/decrypt a page of data.
 */
typedef enum { ENCRYPT, DECRYPT } CryptType;

static int nand_crypt(CryptType crypt_type, const uint8_t* from, uint8_t* to, size_t len, int page)
{
#if 1
	struct blkcipher_desc desc;
	struct scatterlist from_sg;
	struct scatterlist to_sg;
	int result;

	BUG_ON(from == NULL);
	BUG_ON(to == NULL);

	mutex_lock(&nand_crypto_mutex);
	result = get_cipher(&nand_cipher, get_nand_key());
	if (result < 0)
		goto unlock_exit;
	result = get_iv(page, crypto_blkcipher_crt(nand_cipher)->iv);
	if (result < 0)
		goto unlock_exit;
	desc.tfm = nand_cipher;
	desc.flags = 0;
	sg_init_one(&from_sg, from, len);
	sg_init_one(&to_sg, to, len);
	result = (crypt_type == ENCRYPT) ?
			crypto_blkcipher_encrypt(&desc, &to_sg, &from_sg, len) :
			crypto_blkcipher_decrypt(&desc, &to_sg, &from_sg, len);

unlock_exit:
	if (result < 0)
		pr_err("NAND %s error %d\n", (crypt_type == ENCRYPT) ? "encrypt" : "decrypt", result);
	mutex_unlock(&nand_crypto_mutex);
	return result;
#else
  	struct skcipher_request *req = NULL;
  	struct scatterlist sg, sg_dst, *d = &sg;
  	DECLARE_CRYPTO_WAIT(wait);
  	int result;

  	/* Allocate a request object */
  	req = skcipher_request_alloc(nand_cipher, GFP_KERNEL);
  	if (!req)
  		return -ENOMEM;

  	sg_init_one(&sg, data, data_size);
  	if (data != dst) {
  		sg_init_one(&sg_dst, dst, data_size);
  		d = &sg_dst;
  	}

  	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG |
  				      CRYPTO_TFM_REQ_MAY_SLEEP,
  				      crypto_req_done, &wait);
  	skcipher_request_set_crypt(req, &sg,
  				   d, data_size, (void *)iv);

  	if (crypt_type == ENCRYPT)
  		result = crypto_wait_req(crypto_skcipher_encrypt(req), &wait);
  	else
  		result = crypto_wait_req(crypto_skcipher_decrypt(req), &wait);
  	if (result)
  		pr_err("Error encrypting data: %d\n", result);

  	skcipher_request_free(req);

  	return result;


#endif

}

/*
 * Determine if a block of data is completely erased.
 */
static int all_erased(const uint8_t* buf, size_t len)
{
	unsigned int i;

	for (i = 0;  i < len;  i++) {
		if (buf[i] != 0xFF)
			return 0;
	}
	return 1;
}

/*
 * Decrypt a page of data, in place.
 */
void roku_nand_decrypt_page(struct mtd_info* mtd, uint8_t* buf, int page)
{
	int s;

	might_sleep();

	if (all_erased(buf, mtd->writesize))
		/* Don't decrypt completely erased data; let the caller see the FFs. */
		return;
	s = nand_crypt(DECRYPT, buf, buf, mtd->writesize, page);
	/* TODO: understand failure modes of nand_crypt */
	BUG_ON(s < 0);
}

/*
 * Encrypt a page of data and return the encrypted data.
 */
uint8_t* roku_nand_encrypt_page(struct mtd_info* mtd, const uint8_t* buf, int page)
{
	uint8_t* new_buf = kmalloc(mtd->writesize, GFP_KERNEL);
	int s;

	might_sleep();

	if (WARN_ON_ONCE(!new_buf))
		return NULL;

	BUG_ON(!new_buf);

	s = nand_crypt(ENCRYPT, buf, new_buf, mtd->writesize, page);
	/* TODO: understand failure modes of nand_crypt.
	   This probably ought to be BUG_ON */
	if (WARN_ON_ONCE(s < 0))
		return NULL;

	return new_buf;
}
#endif /* CONFIG_ROKU_MTD_NAND_ENCRYPTION */

/**
 * nand_reset_data_interface - Reset data interface and timings
 * @chip: The NAND chip
 * @chipnr: Internal die id
 *
 * Reset the Data interface and timings to ONFI mode 0.
 *
 * Returns 0 for success or negative error code otherwise.
 */
static int nand_reset_data_interface(struct nand_chip *chip, int chipnr)
{
	int ret;

	if (!nand_has_setup_data_iface(chip))
		return 0;

	/*
	 * The ONFI specification says:
	 * "
	 * To transition from NV-DDR or NV-DDR2 to the SDR data
	 * interface, the host shall use the Reset (FFh) command
	 * using SDR timing mode 0. A device in any timing mode is
	 * required to recognize Reset (FFh) command issued in SDR
	 * timing mode 0.
	 * "
	 *
	 * Configure the data interface in SDR mode and set the
	 * timings to timing mode 0.
	 */

	onfi_fill_data_interface(chip, NAND_SDR_IFACE, 0);
	ret = chip->controller->ops->setup_data_interface(chip, chipnr,
							&chip->data_interface);
	if (ret)
		pr_err("Failed to configure data interface to SDR timing mode 0\n");

	return ret;
}

/**
 * nand_setup_data_interface - Setup the best data interface and timings
 * @chip: The NAND chip
 * @chipnr: Internal die id
 *
 * Find and configure the best data interface and NAND timings supported by
 * the chip and the driver.
 * First tries to retrieve supported timing modes from ONFI information,
 * and if the NAND chip does not support ONFI, relies on the
 * ->onfi_timing_mode_default specified in the nand_ids table.
 *
 * Returns 0 for success or negative error code otherwise.
 */
static int nand_setup_data_interface(struct nand_chip *chip, int chipnr)
{
	u8 tmode_param[ONFI_SUBFEATURE_PARAM_LEN] = {
		chip->onfi_timing_mode_default,
	};
	int ret;

	if (!nand_has_setup_data_iface(chip))
		return 0;

	/* Change the mode on the chip side (if supported by the NAND chip) */
	if (nand_supports_set_features(chip, ONFI_FEATURE_ADDR_TIMING_MODE)) {
		nand_select_target(chip, chipnr);
		ret = nand_set_features(chip, ONFI_FEATURE_ADDR_TIMING_MODE,
					tmode_param);
		nand_deselect_target(chip);
		if (ret)
			return ret;
	}

	/* Change the mode on the controller side */
	ret = chip->controller->ops->setup_data_interface(chip, chipnr,
							&chip->data_interface);
	if (ret)
		return ret;

	/* Check the mode has been accepted by the chip, if supported */
	if (!nand_supports_get_features(chip, ONFI_FEATURE_ADDR_TIMING_MODE))
		return 0;

	memset(tmode_param, 0, ONFI_SUBFEATURE_PARAM_LEN);
	nand_select_target(chip, chipnr);
	ret = nand_get_features(chip, ONFI_FEATURE_ADDR_TIMING_MODE,
				tmode_param);
	nand_deselect_target(chip);
	if (ret)
		goto err_reset_chip;

	if (tmode_param[0] != chip->onfi_timing_mode_default) {
		pr_warn("timing mode %d not acknowledged by the NAND chip\n",
			chip->onfi_timing_mode_default);
		goto err_reset_chip;
	}

	return 0;

err_reset_chip:
	/*
	 * Fallback to mode 0 if the chip explicitly did not ack the chosen
	 * timing mode.
	 */
	nand_reset_data_interface(chip, chipnr);
	nand_select_target(chip, chipnr);
	nand_reset_op(chip);
	nand_deselect_target(chip);

	return ret;
}

/**
 * nand_init_data_interface - find the best data interface and timings
 * @chip: The NAND chip
 *
 * Find the best data interface and NAND timings supported by the chip
 * and the driver.
 * First tries to retrieve supported timing modes from ONFI information,
 * and if the NAND chip does not support ONFI, relies on the
 * ->onfi_timing_mode_default specified in the nand_ids table. After this
 * function nand_chip->data_interface is initialized with the best timing mode
 * available.
 *
 * Returns 0 for success or negative error code otherwise.
 */
static int nand_init_data_interface(struct nand_chip *chip)
{
	int modes, mode, ret;

	if (!nand_has_setup_data_iface(chip))
		return 0;

	/*
	 * First try to identify the best timings from ONFI parameters and
	 * if the NAND does not support ONFI, fallback to the default ONFI
	 * timing mode.
	 */
	if (chip->parameters.onfi) {
		modes = chip->parameters.onfi->async_timing_mode;
	} else {
		if (!chip->onfi_timing_mode_default)
			return 0;

		modes = GENMASK(chip->onfi_timing_mode_default, 0);
	}

	for (mode = fls(modes) - 1; mode >= 0; mode--) {
		ret = onfi_fill_data_interface(chip, NAND_SDR_IFACE, mode);
		if (ret)
			continue;

		/*
		 * Pass NAND_DATA_IFACE_CHECK_ONLY to only check if the
		 * controller supports the requested timings.
		 */
		ret = chip->controller->ops->setup_data_interface(chip,
						 NAND_DATA_IFACE_CHECK_ONLY,
						 &chip->data_interface);
		if (!ret) {
			chip->onfi_timing_mode_default = mode;
			break;
		}
	}

	return 0;
}

/**
 * nand_fill_column_cycles - fill the column cycles of an address
 * @chip: The NAND chip
 * @addrs: Array of address cycles to fill
 * @offset_in_page: The offset in the page
 *
 * Fills the first or the first two bytes of the @addrs field depending
 * on the NAND bus width and the page size.
 *
 * Returns the number of cycles needed to encode the column, or a negative
 * error code in case one of the arguments is invalid.
 */
static int nand_fill_column_cycles(struct nand_chip *chip, u8 *addrs,
				   unsigned int offset_in_page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	/* Make sure the offset is less than the actual page size. */
	if (offset_in_page > mtd->writesize + mtd->oobsize)
		return -EINVAL;

	/*
	 * On small page NANDs, there's a dedicated command to access the OOB
	 * area, and the column address is relative to the start of the OOB
	 * area, not the start of the page. Asjust the address accordingly.
	 */
	if (mtd->writesize <= 512 && offset_in_page >= mtd->writesize)
		offset_in_page -= mtd->writesize;

	/*
	 * The offset in page is expressed in bytes, if the NAND bus is 16-bit
	 * wide, then it must be divided by 2.
	 */
	if (chip->options & NAND_BUSWIDTH_16) {
		if (WARN_ON(offset_in_page % 2))
			return -EINVAL;

		offset_in_page /= 2;
	}

	addrs[0] = offset_in_page;

	/*
	 * Small page NANDs use 1 cycle for the columns, while large page NANDs
	 * need 2
	 */
	if (mtd->writesize <= 512)
		return 1;

	addrs[1] = offset_in_page >> 8;

	return 2;
}

static int nand_sp_exec_read_page_op(struct nand_chip *chip, unsigned int page,
				     unsigned int offset_in_page, void *buf,
				     unsigned int len)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	const struct nand_sdr_timings *sdr =
		nand_get_sdr_timings(&chip->data_interface);
	u8 addrs[4];
	struct nand_op_instr instrs[] = {
		NAND_OP_CMD(NAND_CMD_READ0, 0),
		NAND_OP_ADDR(3, addrs, PSEC_TO_NSEC(sdr->tWB_max)),
		NAND_OP_WAIT_RDY(PSEC_TO_MSEC(sdr->tR_max),
				 PSEC_TO_NSEC(sdr->tRR_min)),
		NAND_OP_DATA_IN(len, buf, 0),
	};
	struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);
	int ret;

	/* Drop the DATA_IN instruction if len is set to 0. */
	if (!len)
		op.ninstrs--;

	if (offset_in_page >= mtd->writesize)
		instrs[0].ctx.cmd.opcode = NAND_CMD_READOOB;
	else if (offset_in_page >= 256 &&
		 !(chip->options & NAND_BUSWIDTH_16))
		instrs[0].ctx.cmd.opcode = NAND_CMD_READ1;

	ret = nand_fill_column_cycles(chip, addrs, offset_in_page);
	if (ret < 0)
		return ret;

	addrs[1] = page;
	addrs[2] = page >> 8;

	if (chip->options & NAND_ROW_ADDR_3) {
		addrs[3] = page >> 16;
		instrs[1].ctx.addr.naddrs++;
	}

	return nand_exec_op(chip, &op);
}

static int nand_lp_exec_read_page_op(struct nand_chip *chip, unsigned int page,
				     unsigned int offset_in_page, void *buf,
				     unsigned int len)
{
	const struct nand_sdr_timings *sdr =
		nand_get_sdr_timings(&chip->data_interface);
	u8 addrs[5];
	struct nand_op_instr instrs[] = {
		NAND_OP_CMD(NAND_CMD_READ0, 0),
		NAND_OP_ADDR(4, addrs, 0),
		NAND_OP_CMD(NAND_CMD_READSTART, PSEC_TO_NSEC(sdr->tWB_max)),
		NAND_OP_WAIT_RDY(PSEC_TO_MSEC(sdr->tR_max),
				 PSEC_TO_NSEC(sdr->tRR_min)),
		NAND_OP_DATA_IN(len, buf, 0),
	};
	struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);
	int ret;

	/* Drop the DATA_IN instruction if len is set to 0. */
	if (!len)
		op.ninstrs--;

	ret = nand_fill_column_cycles(chip, addrs, offset_in_page);
	if (ret < 0)
		return ret;

	addrs[2] = page;
	addrs[3] = page >> 8;

	if (chip->options & NAND_ROW_ADDR_3) {
		addrs[4] = page >> 16;
		instrs[1].ctx.addr.naddrs++;
	}

	return nand_exec_op(chip, &op);
}

/**
 * nand_read_page_op - Do a READ PAGE operation
 * @chip: The NAND chip
 * @page: page to read
 * @offset_in_page: offset within the page
 * @buf: buffer used to store the data
 * @len: length of the buffer
 *
 * This function issues a READ PAGE operation.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_read_page_op(struct nand_chip *chip, unsigned int page,
		      unsigned int offset_in_page, void *buf, unsigned int len)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	if (len && !buf)
		return -EINVAL;

	if (offset_in_page + len > mtd->writesize + mtd->oobsize)
		return -EINVAL;

	if (nand_has_exec_op(chip)) {
		if (mtd->writesize > 512)
			return nand_lp_exec_read_page_op(chip, page,
							 offset_in_page, buf,
							 len);
		return nand_sp_exec_read_page_op(chip, page, offset_in_page,
						 buf, len);
	}
	chip->legacy.cmdfunc(chip, NAND_CMD_READ0, offset_in_page, page);
	if (len)
		chip->legacy.read_buf(chip, buf, len);
	return 0;
}
EXPORT_SYMBOL_GPL(nand_read_page_op);

/**
 * nand_read_param_page_op - Do a READ PARAMETER PAGE operation
 * @chip: The NAND chip
 * @page: parameter page to read
 * @buf: buffer used to store the data
 * @len: length of the buffer
 *
 * This function issues a READ PARAMETER PAGE operation.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_read_param_page_op(struct nand_chip *chip, u8 page, void *buf,
			    unsigned int len)
{
	unsigned int i;
	u8 *p = buf;

	if (len && !buf)
		return -EINVAL;

	if (nand_has_exec_op(chip)) {
		const struct nand_sdr_timings *sdr =
			nand_get_sdr_timings(&chip->data_interface);
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_PARAM, 0),
			NAND_OP_ADDR(1, &page, PSEC_TO_NSEC(sdr->tWB_max)),
			NAND_OP_WAIT_RDY(PSEC_TO_MSEC(sdr->tR_max),
					 PSEC_TO_NSEC(sdr->tRR_min)),
			NAND_OP_8BIT_DATA_IN(len, buf, 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		/* Drop the DATA_IN instruction if len is set to 0. */
		if (!len)
			op.ninstrs--;

		return nand_exec_op(chip, &op);
	}

	chip->legacy.cmdfunc(chip, NAND_CMD_PARAM, page, -1);
	for (i = 0; i < len; i++)
		p[i] = chip->legacy.read_byte(chip);

	return 0;
}

/**
 * nand_change_read_column_op - Do a CHANGE READ COLUMN operation
 * @chip: The NAND chip
 * @offset_in_page: offset within the page
 * @buf: buffer used to store the data
 * @len: length of the buffer
 * @force_8bit: force 8-bit bus access
 *
 * This function issues a CHANGE READ COLUMN operation.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_change_read_column_op(struct nand_chip *chip,
			       unsigned int offset_in_page, void *buf,
			       unsigned int len, bool force_8bit)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	if (len && !buf)
		return -EINVAL;

	if (offset_in_page + len > mtd->writesize + mtd->oobsize)
		return -EINVAL;

	/* Small page NANDs do not support column change. */
	if (mtd->writesize <= 512)
		return -ENOTSUPP;

	if (nand_has_exec_op(chip)) {
		const struct nand_sdr_timings *sdr =
			nand_get_sdr_timings(&chip->data_interface);
		u8 addrs[2] = {};
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_RNDOUT, 0),
			NAND_OP_ADDR(2, addrs, 0),
			NAND_OP_CMD(NAND_CMD_RNDOUTSTART,
				    PSEC_TO_NSEC(sdr->tCCS_min)),
			NAND_OP_DATA_IN(len, buf, 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);
		int ret;

		ret = nand_fill_column_cycles(chip, addrs, offset_in_page);
		if (ret < 0)
			return ret;

		/* Drop the DATA_IN instruction if len is set to 0. */
		if (!len)
			op.ninstrs--;

		instrs[3].ctx.data.force_8bit = force_8bit;

		return nand_exec_op(chip, &op);
	}

	chip->legacy.cmdfunc(chip, NAND_CMD_RNDOUT, offset_in_page, -1);
	if (len)
		chip->legacy.read_buf(chip, buf, len);

	return 0;
}
EXPORT_SYMBOL_GPL(nand_change_read_column_op);

/**
 * nand_read_oob_op - Do a READ OOB operation
 * @chip: The NAND chip
 * @page: page to read
 * @offset_in_oob: offset within the OOB area
 * @buf: buffer used to store the data
 * @len: length of the buffer
 *
 * This function issues a READ OOB operation.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_read_oob_op(struct nand_chip *chip, unsigned int page,
		     unsigned int offset_in_oob, void *buf, unsigned int len)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	if (len && !buf)
		return -EINVAL;

	if (offset_in_oob + len > mtd->oobsize)
		return -EINVAL;

	if (nand_has_exec_op(chip))
		return nand_read_page_op(chip, page,
					 mtd->writesize + offset_in_oob,
					 buf, len);

	chip->legacy.cmdfunc(chip, NAND_CMD_READOOB, offset_in_oob, page);
	if (len)
		chip->legacy.read_buf(chip, buf, len);

	return 0;
}
EXPORT_SYMBOL_GPL(nand_read_oob_op);

static int nand_exec_prog_page_op(struct nand_chip *chip, unsigned int page,
				  unsigned int offset_in_page, const void *buf,
				  unsigned int len, bool prog)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	const struct nand_sdr_timings *sdr =
		nand_get_sdr_timings(&chip->data_interface);
	u8 addrs[5] = {};
	struct nand_op_instr instrs[] = {
		/*
		 * The first instruction will be dropped if we're dealing
		 * with a large page NAND and adjusted if we're dealing
		 * with a small page NAND and the page offset is > 255.
		 */
		NAND_OP_CMD(NAND_CMD_READ0, 0),
		NAND_OP_CMD(NAND_CMD_SEQIN, 0),
		NAND_OP_ADDR(0, addrs, PSEC_TO_NSEC(sdr->tADL_min)),
		NAND_OP_DATA_OUT(len, buf, 0),
		NAND_OP_CMD(NAND_CMD_PAGEPROG, PSEC_TO_NSEC(sdr->tWB_max)),
		NAND_OP_WAIT_RDY(PSEC_TO_MSEC(sdr->tPROG_max), 0),
	};
	struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);
	int naddrs = nand_fill_column_cycles(chip, addrs, offset_in_page);
	int ret;
	u8 status;

	if (naddrs < 0)
		return naddrs;

	addrs[naddrs++] = page;
	addrs[naddrs++] = page >> 8;
	if (chip->options & NAND_ROW_ADDR_3)
		addrs[naddrs++] = page >> 16;

	instrs[2].ctx.addr.naddrs = naddrs;

	/* Drop the last two instructions if we're not programming the page. */
	if (!prog) {
		op.ninstrs -= 2;
		/* Also drop the DATA_OUT instruction if empty. */
		if (!len)
			op.ninstrs--;
	}

	if (mtd->writesize <= 512) {
		/*
		 * Small pages need some more tweaking: we have to adjust the
		 * first instruction depending on the page offset we're trying
		 * to access.
		 */
		if (offset_in_page >= mtd->writesize)
			instrs[0].ctx.cmd.opcode = NAND_CMD_READOOB;
		else if (offset_in_page >= 256 &&
			 !(chip->options & NAND_BUSWIDTH_16))
			instrs[0].ctx.cmd.opcode = NAND_CMD_READ1;
	} else {
		/*
		 * Drop the first command if we're dealing with a large page
		 * NAND.
		 */
		op.instrs++;
		op.ninstrs--;
	}

	ret = nand_exec_op(chip, &op);
	if (!prog || ret)
		return ret;

	ret = nand_status_op(chip, &status);
	if (ret)
		return ret;

	return status;
}

/**
 * nand_prog_page_begin_op - starts a PROG PAGE operation
 * @chip: The NAND chip
 * @page: page to write
 * @offset_in_page: offset within the page
 * @buf: buffer containing the data to write to the page
 * @len: length of the buffer
 *
 * This function issues the first half of a PROG PAGE operation.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_prog_page_begin_op(struct nand_chip *chip, unsigned int page,
			    unsigned int offset_in_page, const void *buf,
			    unsigned int len)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	if (len && !buf)
		return -EINVAL;

	if (offset_in_page + len > mtd->writesize + mtd->oobsize)
		return -EINVAL;

	if (nand_has_exec_op(chip))
		return nand_exec_prog_page_op(chip, page, offset_in_page, buf,
					      len, false);

	chip->legacy.cmdfunc(chip, NAND_CMD_SEQIN, offset_in_page, page);

	if (buf)
		chip->legacy.write_buf(chip, buf, len);

	return 0;
}
EXPORT_SYMBOL_GPL(nand_prog_page_begin_op);

/**
 * nand_prog_page_end_op - ends a PROG PAGE operation
 * @chip: The NAND chip
 *
 * This function issues the second half of a PROG PAGE operation.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_prog_page_end_op(struct nand_chip *chip)
{
	int ret;
	u8 status;

	if (nand_has_exec_op(chip)) {
		const struct nand_sdr_timings *sdr =
			nand_get_sdr_timings(&chip->data_interface);
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_PAGEPROG,
				    PSEC_TO_NSEC(sdr->tWB_max)),
			NAND_OP_WAIT_RDY(PSEC_TO_MSEC(sdr->tPROG_max), 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		ret = nand_exec_op(chip, &op);
		if (ret)
			return ret;

		ret = nand_status_op(chip, &status);
		if (ret)
			return ret;
	} else {
		chip->legacy.cmdfunc(chip, NAND_CMD_PAGEPROG, -1, -1);
		ret = chip->legacy.waitfunc(chip);
		if (ret < 0)
			return ret;

		status = ret;
	}

	if (status & NAND_STATUS_FAIL)
		return -EIO;

	return 0;
}
EXPORT_SYMBOL_GPL(nand_prog_page_end_op);

/**
 * nand_prog_page_op - Do a full PROG PAGE operation
 * @chip: The NAND chip
 * @page: page to write
 * @offset_in_page: offset within the page
 * @buf: buffer containing the data to write to the page
 * @len: length of the buffer
 *
 * This function issues a full PROG PAGE operation.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_prog_page_op(struct nand_chip *chip, unsigned int page,
		      unsigned int offset_in_page, const void *buf,
		      unsigned int len)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int status;

	if (!len || !buf)
		return -EINVAL;

	if (offset_in_page + len > mtd->writesize + mtd->oobsize)
		return -EINVAL;

	if (nand_has_exec_op(chip)) {
		status = nand_exec_prog_page_op(chip, page, offset_in_page, buf,
						len, true);
	} else {
		chip->legacy.cmdfunc(chip, NAND_CMD_SEQIN, offset_in_page,
				     page);
		chip->legacy.write_buf(chip, buf, len);
		chip->legacy.cmdfunc(chip, NAND_CMD_PAGEPROG, -1, -1);
		status = chip->legacy.waitfunc(chip);
	}

	if (status & NAND_STATUS_FAIL)
		return -EIO;

	return 0;
}
EXPORT_SYMBOL_GPL(nand_prog_page_op);

/**
 * nand_change_write_column_op - Do a CHANGE WRITE COLUMN operation
 * @chip: The NAND chip
 * @offset_in_page: offset within the page
 * @buf: buffer containing the data to send to the NAND
 * @len: length of the buffer
 * @force_8bit: force 8-bit bus access
 *
 * This function issues a CHANGE WRITE COLUMN operation.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_change_write_column_op(struct nand_chip *chip,
				unsigned int offset_in_page,
				const void *buf, unsigned int len,
				bool force_8bit)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	if (len && !buf)
		return -EINVAL;

	if (offset_in_page + len > mtd->writesize + mtd->oobsize)
		return -EINVAL;

	/* Small page NANDs do not support column change. */
	if (mtd->writesize <= 512)
		return -ENOTSUPP;

	if (nand_has_exec_op(chip)) {
		const struct nand_sdr_timings *sdr =
			nand_get_sdr_timings(&chip->data_interface);
		u8 addrs[2];
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_RNDIN, 0),
			NAND_OP_ADDR(2, addrs, PSEC_TO_NSEC(sdr->tCCS_min)),
			NAND_OP_DATA_OUT(len, buf, 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);
		int ret;

		ret = nand_fill_column_cycles(chip, addrs, offset_in_page);
		if (ret < 0)
			return ret;

		instrs[2].ctx.data.force_8bit = force_8bit;

		/* Drop the DATA_OUT instruction if len is set to 0. */
		if (!len)
			op.ninstrs--;

		return nand_exec_op(chip, &op);
	}

	chip->legacy.cmdfunc(chip, NAND_CMD_RNDIN, offset_in_page, -1);
	if (len)
		chip->legacy.write_buf(chip, buf, len);

	return 0;
}
EXPORT_SYMBOL_GPL(nand_change_write_column_op);

/**
 * nand_readid_op - Do a READID operation
 * @chip: The NAND chip
 * @addr: address cycle to pass after the READID command
 * @buf: buffer used to store the ID
 * @len: length of the buffer
 *
 * This function sends a READID command and reads back the ID returned by the
 * NAND.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_readid_op(struct nand_chip *chip, u8 addr, void *buf,
		   unsigned int len)
{
	unsigned int i;
	u8 *id = buf;

	if (len && !buf)
		return -EINVAL;

	if (nand_has_exec_op(chip)) {
		const struct nand_sdr_timings *sdr =
			nand_get_sdr_timings(&chip->data_interface);
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_READID, 0),
			NAND_OP_ADDR(1, &addr, PSEC_TO_NSEC(sdr->tADL_min)),
			NAND_OP_8BIT_DATA_IN(len, buf, 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		/* Drop the DATA_IN instruction if len is set to 0. */
		if (!len)
			op.ninstrs--;

		return nand_exec_op(chip, &op);
	}

	chip->legacy.cmdfunc(chip, NAND_CMD_READID, addr, -1);

	for (i = 0; i < len; i++)
		id[i] = chip->legacy.read_byte(chip);

	return 0;
}
EXPORT_SYMBOL_GPL(nand_readid_op);

/**
 * nand_status_op - Do a STATUS operation
 * @chip: The NAND chip
 * @status: out variable to store the NAND status
 *
 * This function sends a STATUS command and reads back the status returned by
 * the NAND.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_status_op(struct nand_chip *chip, u8 *status)
{
	if (nand_has_exec_op(chip)) {
		const struct nand_sdr_timings *sdr =
			nand_get_sdr_timings(&chip->data_interface);
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_STATUS,
				    PSEC_TO_NSEC(sdr->tADL_min)),
			NAND_OP_8BIT_DATA_IN(1, status, 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		if (!status)
			op.ninstrs--;

		return nand_exec_op(chip, &op);
	}

	chip->legacy.cmdfunc(chip, NAND_CMD_STATUS, -1, -1);
	if (status)
		*status = chip->legacy.read_byte(chip);

	return 0;
}
EXPORT_SYMBOL_GPL(nand_status_op);

/**
 * nand_exit_status_op - Exit a STATUS operation
 * @chip: The NAND chip
 *
 * This function sends a READ0 command to cancel the effect of the STATUS
 * command to avoid reading only the status until a new read command is sent.
 *
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_exit_status_op(struct nand_chip *chip)
{
	if (nand_has_exec_op(chip)) {
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_READ0, 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		return nand_exec_op(chip, &op);
	}

	chip->legacy.cmdfunc(chip, NAND_CMD_READ0, -1, -1);

	return 0;
}

/**
 * nand_erase_op - Do an erase operation
 * @chip: The NAND chip
 * @eraseblock: block to erase
 *
 * This function sends an ERASE command and waits for the NAND to be ready
 * before returning.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_erase_op(struct nand_chip *chip, unsigned int eraseblock)
{
	unsigned int page = eraseblock <<
			    (chip->phys_erase_shift - chip->page_shift);
	int ret;
	u8 status;

	if (nand_has_exec_op(chip)) {
		const struct nand_sdr_timings *sdr =
			nand_get_sdr_timings(&chip->data_interface);
		u8 addrs[3] = {	page, page >> 8, page >> 16 };
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_ERASE1, 0),
			NAND_OP_ADDR(2, addrs, 0),
			NAND_OP_CMD(NAND_CMD_ERASE2,
				    PSEC_TO_MSEC(sdr->tWB_max)),
			NAND_OP_WAIT_RDY(PSEC_TO_MSEC(sdr->tBERS_max), 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		if (chip->options & NAND_ROW_ADDR_3)
			instrs[1].ctx.addr.naddrs++;

		ret = nand_exec_op(chip, &op);
		if (ret)
			return ret;

		ret = nand_status_op(chip, &status);
		if (ret)
			return ret;
	} else {
		chip->legacy.cmdfunc(chip, NAND_CMD_ERASE1, -1, page);
		chip->legacy.cmdfunc(chip, NAND_CMD_ERASE2, -1, -1);

		ret = chip->legacy.waitfunc(chip);
		if (ret < 0)
			return ret;

		status = ret;
	}

	if (status & NAND_STATUS_FAIL)
		return -EIO;

	return 0;
}
EXPORT_SYMBOL_GPL(nand_erase_op);

/**
 * nand_set_features_op - Do a SET FEATURES operation
 * @chip: The NAND chip
 * @feature: feature id
 * @data: 4 bytes of data
 *
 * This function sends a SET FEATURES command and waits for the NAND to be
 * ready before returning.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
static int nand_set_features_op(struct nand_chip *chip, u8 feature,
				const void *data)
{
	const u8 *params = data;
	int i, ret;

	if (nand_has_exec_op(chip)) {
		const struct nand_sdr_timings *sdr =
			nand_get_sdr_timings(&chip->data_interface);
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_SET_FEATURES, 0),
			NAND_OP_ADDR(1, &feature, PSEC_TO_NSEC(sdr->tADL_min)),
			NAND_OP_8BIT_DATA_OUT(ONFI_SUBFEATURE_PARAM_LEN, data,
					      PSEC_TO_NSEC(sdr->tWB_max)),
			NAND_OP_WAIT_RDY(PSEC_TO_MSEC(sdr->tFEAT_max), 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		return nand_exec_op(chip, &op);
	}

	chip->legacy.cmdfunc(chip, NAND_CMD_SET_FEATURES, feature, -1);
	for (i = 0; i < ONFI_SUBFEATURE_PARAM_LEN; ++i)
		chip->legacy.write_byte(chip, params[i]);

	ret = chip->legacy.waitfunc(chip);
	if (ret < 0)
		return ret;

	if (ret & NAND_STATUS_FAIL)
		return -EIO;

	return 0;
}

/**
 * nand_get_features_op - Do a GET FEATURES operation
 * @chip: The NAND chip
 * @feature: feature id
 * @data: 4 bytes of data
 *
 * This function sends a GET FEATURES command and waits for the NAND to be
 * ready before returning.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
static int nand_get_features_op(struct nand_chip *chip, u8 feature,
				void *data)
{
	u8 *params = data;
	int i;

	if (nand_has_exec_op(chip)) {
		const struct nand_sdr_timings *sdr =
			nand_get_sdr_timings(&chip->data_interface);
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_GET_FEATURES, 0),
			NAND_OP_ADDR(1, &feature, PSEC_TO_NSEC(sdr->tWB_max)),
			NAND_OP_WAIT_RDY(PSEC_TO_MSEC(sdr->tFEAT_max),
					 PSEC_TO_NSEC(sdr->tRR_min)),
			NAND_OP_8BIT_DATA_IN(ONFI_SUBFEATURE_PARAM_LEN,
					     data, 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		return nand_exec_op(chip, &op);
	}

	chip->legacy.cmdfunc(chip, NAND_CMD_GET_FEATURES, feature, -1);
	for (i = 0; i < ONFI_SUBFEATURE_PARAM_LEN; ++i)
		params[i] = chip->legacy.read_byte(chip);

	return 0;
}

static int nand_wait_rdy_op(struct nand_chip *chip, unsigned int timeout_ms,
			    unsigned int delay_ns)
{
	if (nand_has_exec_op(chip)) {
		struct nand_op_instr instrs[] = {
			NAND_OP_WAIT_RDY(PSEC_TO_MSEC(timeout_ms),
					 PSEC_TO_NSEC(delay_ns)),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		return nand_exec_op(chip, &op);
	}

	/* Apply delay or wait for ready/busy pin */
	if (!chip->legacy.dev_ready)
		udelay(chip->legacy.chip_delay);
	else
		nand_wait_ready(chip);

	return 0;
}

/**
 * nand_reset_op - Do a reset operation
 * @chip: The NAND chip
 *
 * This function sends a RESET command and waits for the NAND to be ready
 * before returning.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_reset_op(struct nand_chip *chip)
{
	if (nand_has_exec_op(chip)) {
		const struct nand_sdr_timings *sdr =
			nand_get_sdr_timings(&chip->data_interface);
		struct nand_op_instr instrs[] = {
			NAND_OP_CMD(NAND_CMD_RESET, PSEC_TO_NSEC(sdr->tWB_max)),
			NAND_OP_WAIT_RDY(PSEC_TO_MSEC(sdr->tRST_max), 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		return nand_exec_op(chip, &op);
	}

	chip->legacy.cmdfunc(chip, NAND_CMD_RESET, -1, -1);

	return 0;
}
EXPORT_SYMBOL_GPL(nand_reset_op);

/**
 * nand_read_data_op - Read data from the NAND
 * @chip: The NAND chip
 * @buf: buffer used to store the data
 * @len: length of the buffer
 * @force_8bit: force 8-bit bus access
 *
 * This function does a raw data read on the bus. Usually used after launching
 * another NAND operation like nand_read_page_op().
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_read_data_op(struct nand_chip *chip, void *buf, unsigned int len,
		      bool force_8bit)
{
	if (!len || !buf)
		return -EINVAL;

	if (nand_has_exec_op(chip)) {
		struct nand_op_instr instrs[] = {
			NAND_OP_DATA_IN(len, buf, 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		instrs[0].ctx.data.force_8bit = force_8bit;

		return nand_exec_op(chip, &op);
	}

	if (force_8bit) {
		u8 *p = buf;
		unsigned int i;

		for (i = 0; i < len; i++)
			p[i] = chip->legacy.read_byte(chip);
	} else {
		chip->legacy.read_buf(chip, buf, len);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(nand_read_data_op);

/**
 * nand_write_data_op - Write data from the NAND
 * @chip: The NAND chip
 * @buf: buffer containing the data to send on the bus
 * @len: length of the buffer
 * @force_8bit: force 8-bit bus access
 *
 * This function does a raw data write on the bus. Usually used after launching
 * another NAND operation like nand_write_page_begin_op().
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_write_data_op(struct nand_chip *chip, const void *buf,
		       unsigned int len, bool force_8bit)
{
	if (!len || !buf)
		return -EINVAL;

	if (nand_has_exec_op(chip)) {
		struct nand_op_instr instrs[] = {
			NAND_OP_DATA_OUT(len, buf, 0),
		};
		struct nand_operation op = NAND_OPERATION(chip->cur_cs, instrs);

		instrs[0].ctx.data.force_8bit = force_8bit;

		return nand_exec_op(chip, &op);
	}

	if (force_8bit) {
		const u8 *p = buf;
		unsigned int i;

		for (i = 0; i < len; i++)
			chip->legacy.write_byte(chip, p[i]);
	} else {
		chip->legacy.write_buf(chip, buf, len);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(nand_write_data_op);

/**
 * struct nand_op_parser_ctx - Context used by the parser
 * @instrs: array of all the instructions that must be addressed
 * @ninstrs: length of the @instrs array
 * @subop: Sub-operation to be passed to the NAND controller
 *
 * This structure is used by the core to split NAND operations into
 * sub-operations that can be handled by the NAND controller.
 */
struct nand_op_parser_ctx {
	const struct nand_op_instr *instrs;
	unsigned int ninstrs;
	struct nand_subop subop;
};

/**
 * nand_op_parser_must_split_instr - Checks if an instruction must be split
 * @pat: the parser pattern element that matches @instr
 * @instr: pointer to the instruction to check
 * @start_offset: this is an in/out parameter. If @instr has already been
 *		  split, then @start_offset is the offset from which to start
 *		  (either an address cycle or an offset in the data buffer).
 *		  Conversely, if the function returns true (ie. instr must be
 *		  split), this parameter is updated to point to the first
 *		  data/address cycle that has not been taken care of.
 *
 * Some NAND controllers are limited and cannot send X address cycles with a
 * unique operation, or cannot read/write more than Y bytes at the same time.
 * In this case, split the instruction that does not fit in a single
 * controller-operation into two or more chunks.
 *
 * Returns true if the instruction must be split, false otherwise.
 * The @start_offset parameter is also updated to the offset at which the next
 * bundle of instruction must start (if an address or a data instruction).
 */
static bool
nand_op_parser_must_split_instr(const struct nand_op_parser_pattern_elem *pat,
				const struct nand_op_instr *instr,
				unsigned int *start_offset)
{
	switch (pat->type) {
	case NAND_OP_ADDR_INSTR:
		if (!pat->ctx.addr.maxcycles)
			break;

		if (instr->ctx.addr.naddrs - *start_offset >
		    pat->ctx.addr.maxcycles) {
			*start_offset += pat->ctx.addr.maxcycles;
			return true;
		}
		break;

	case NAND_OP_DATA_IN_INSTR:
	case NAND_OP_DATA_OUT_INSTR:
		if (!pat->ctx.data.maxlen)
			break;

		if (instr->ctx.data.len - *start_offset >
		    pat->ctx.data.maxlen) {
			*start_offset += pat->ctx.data.maxlen;
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}

/**
 * nand_op_parser_match_pat - Checks if a pattern matches the instructions
 *			      remaining in the parser context
 * @pat: the pattern to test
 * @ctx: the parser context structure to match with the pattern @pat
 *
 * Check if @pat matches the set or a sub-set of instructions remaining in @ctx.
 * Returns true if this is the case, false ortherwise. When true is returned,
 * @ctx->subop is updated with the set of instructions to be passed to the
 * controller driver.
 */
static bool
nand_op_parser_match_pat(const struct nand_op_parser_pattern *pat,
			 struct nand_op_parser_ctx *ctx)
{
	unsigned int instr_offset = ctx->subop.first_instr_start_off;
	const struct nand_op_instr *end = ctx->instrs + ctx->ninstrs;
	const struct nand_op_instr *instr = ctx->subop.instrs;
	unsigned int i, ninstrs;

	for (i = 0, ninstrs = 0; i < pat->nelems && instr < end; i++) {
		/*
		 * The pattern instruction does not match the operation
		 * instruction. If the instruction is marked optional in the
		 * pattern definition, we skip the pattern element and continue
		 * to the next one. If the element is mandatory, there's no
		 * match and we can return false directly.
		 */
		if (instr->type != pat->elems[i].type) {
			if (!pat->elems[i].optional)
				return false;

			continue;
		}

		/*
		 * Now check the pattern element constraints. If the pattern is
		 * not able to handle the whole instruction in a single step,
		 * we have to split it.
		 * The last_instr_end_off value comes back updated to point to
		 * the position where we have to split the instruction (the
		 * start of the next subop chunk).
		 */
		if (nand_op_parser_must_split_instr(&pat->elems[i], instr,
						    &instr_offset)) {
			ninstrs++;
			i++;
			break;
		}

		instr++;
		ninstrs++;
		instr_offset = 0;
	}

	/*
	 * This can happen if all instructions of a pattern are optional.
	 * Still, if there's not at least one instruction handled by this
	 * pattern, this is not a match, and we should try the next one (if
	 * any).
	 */
	if (!ninstrs)
		return false;

	/*
	 * We had a match on the pattern head, but the pattern may be longer
	 * than the instructions we're asked to execute. We need to make sure
	 * there's no mandatory elements in the pattern tail.
	 */
	for (; i < pat->nelems; i++) {
		if (!pat->elems[i].optional)
			return false;
	}

	/*
	 * We have a match: update the subop structure accordingly and return
	 * true.
	 */
	ctx->subop.ninstrs = ninstrs;
	ctx->subop.last_instr_end_off = instr_offset;

	return true;
}

#if IS_ENABLED(CONFIG_DYNAMIC_DEBUG) || defined(DEBUG)
static void nand_op_parser_trace(const struct nand_op_parser_ctx *ctx)
{
	const struct nand_op_instr *instr;
	char *prefix = "      ";
	unsigned int i;

	pr_debug("executing subop:\n");

	for (i = 0; i < ctx->ninstrs; i++) {
		instr = &ctx->instrs[i];

		if (instr == &ctx->subop.instrs[0])
			prefix = "    ->";

		nand_op_trace(prefix, instr);

		if (instr == &ctx->subop.instrs[ctx->subop.ninstrs - 1])
			prefix = "      ";
	}
}
#else
static void nand_op_parser_trace(const struct nand_op_parser_ctx *ctx)
{
	/* NOP */
}
#endif

static int nand_op_parser_cmp_ctx(const struct nand_op_parser_ctx *a,
				  const struct nand_op_parser_ctx *b)
{
	if (a->subop.ninstrs < b->subop.ninstrs)
		return -1;
	else if (a->subop.ninstrs > b->subop.ninstrs)
		return 1;

	if (a->subop.last_instr_end_off < b->subop.last_instr_end_off)
		return -1;
	else if (a->subop.last_instr_end_off > b->subop.last_instr_end_off)
		return 1;

	return 0;
}

/**
 * nand_op_parser_exec_op - exec_op parser
 * @chip: the NAND chip
 * @parser: patterns description provided by the controller driver
 * @op: the NAND operation to address
 * @check_only: when true, the function only checks if @op can be handled but
 *		does not execute the operation
 *
 * Helper function designed to ease integration of NAND controller drivers that
 * only support a limited set of instruction sequences. The supported sequences
 * are described in @parser, and the framework takes care of splitting @op into
 * multiple sub-operations (if required) and pass them back to the ->exec()
 * callback of the matching pattern if @check_only is set to false.
 *
 * NAND controller drivers should call this function from their own ->exec_op()
 * implementation.
 *
 * Returns 0 on success, a negative error code otherwise. A failure can be
 * caused by an unsupported operation (none of the supported patterns is able
 * to handle the requested operation), or an error returned by one of the
 * matching pattern->exec() hook.
 */
int nand_op_parser_exec_op(struct nand_chip *chip,
			   const struct nand_op_parser *parser,
			   const struct nand_operation *op, bool check_only)
{
	struct nand_op_parser_ctx ctx = {
		.subop.instrs = op->instrs,
		.instrs = op->instrs,
		.ninstrs = op->ninstrs,
	};
	unsigned int i;

	while (ctx.subop.instrs < op->instrs + op->ninstrs) {
		const struct nand_op_parser_pattern *pattern;
		struct nand_op_parser_ctx best_ctx;
		int ret, best_pattern = -1;

		for (i = 0; i < parser->npatterns; i++) {
			struct nand_op_parser_ctx test_ctx = ctx;

			pattern = &parser->patterns[i];
			if (!nand_op_parser_match_pat(pattern, &test_ctx))
				continue;

			if (best_pattern >= 0 &&
			    nand_op_parser_cmp_ctx(&test_ctx, &best_ctx) <= 0)
				continue;

			best_pattern = i;
			best_ctx = test_ctx;
		}

		if (best_pattern < 0) {
			pr_debug("->exec_op() parser: pattern not found!\n");
			return -ENOTSUPP;
		}

		ctx = best_ctx;
		nand_op_parser_trace(&ctx);

		if (!check_only) {
			pattern = &parser->patterns[best_pattern];
			ret = pattern->exec(chip, &ctx.subop);
			if (ret)
				return ret;
		}

		/*
		 * Update the context structure by pointing to the start of the
		 * next subop.
		 */
		ctx.subop.instrs = ctx.subop.instrs + ctx.subop.ninstrs;
		if (ctx.subop.last_instr_end_off)
			ctx.subop.instrs -= 1;

		ctx.subop.first_instr_start_off = ctx.subop.last_instr_end_off;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(nand_op_parser_exec_op);

static bool nand_instr_is_data(const struct nand_op_instr *instr)
{
	return instr && (instr->type == NAND_OP_DATA_IN_INSTR ||
			 instr->type == NAND_OP_DATA_OUT_INSTR);
}

static bool nand_subop_instr_is_valid(const struct nand_subop *subop,
				      unsigned int instr_idx)
{
	return subop && instr_idx < subop->ninstrs;
}

static unsigned int nand_subop_get_start_off(const struct nand_subop *subop,
					     unsigned int instr_idx)
{
	if (instr_idx)
		return 0;

	return subop->first_instr_start_off;
}

/**
 * nand_subop_get_addr_start_off - Get the start offset in an address array
 * @subop: The entire sub-operation
 * @instr_idx: Index of the instruction inside the sub-operation
 *
 * During driver development, one could be tempted to directly use the
 * ->addr.addrs field of address instructions. This is wrong as address
 * instructions might be split.
 *
 * Given an address instruction, returns the offset of the first cycle to issue.
 */
unsigned int nand_subop_get_addr_start_off(const struct nand_subop *subop,
					   unsigned int instr_idx)
{
	if (WARN_ON(!nand_subop_instr_is_valid(subop, instr_idx) ||
		    subop->instrs[instr_idx].type != NAND_OP_ADDR_INSTR))
		return 0;

	return nand_subop_get_start_off(subop, instr_idx);
}
EXPORT_SYMBOL_GPL(nand_subop_get_addr_start_off);

/**
 * nand_subop_get_num_addr_cyc - Get the remaining address cycles to assert
 * @subop: The entire sub-operation
 * @instr_idx: Index of the instruction inside the sub-operation
 *
 * During driver development, one could be tempted to directly use the
 * ->addr->naddrs field of a data instruction. This is wrong as instructions
 * might be split.
 *
 * Given an address instruction, returns the number of address cycle to issue.
 */
unsigned int nand_subop_get_num_addr_cyc(const struct nand_subop *subop,
					 unsigned int instr_idx)
{
	int start_off, end_off;

	if (WARN_ON(!nand_subop_instr_is_valid(subop, instr_idx) ||
		    subop->instrs[instr_idx].type != NAND_OP_ADDR_INSTR))
		return 0;

	start_off = nand_subop_get_addr_start_off(subop, instr_idx);

	if (instr_idx == subop->ninstrs - 1 &&
	    subop->last_instr_end_off)
		end_off = subop->last_instr_end_off;
	else
		end_off = subop->instrs[instr_idx].ctx.addr.naddrs;

	return end_off - start_off;
}
EXPORT_SYMBOL_GPL(nand_subop_get_num_addr_cyc);

/**
 * nand_subop_get_data_start_off - Get the start offset in a data array
 * @subop: The entire sub-operation
 * @instr_idx: Index of the instruction inside the sub-operation
 *
 * During driver development, one could be tempted to directly use the
 * ->data->buf.{in,out} field of data instructions. This is wrong as data
 * instructions might be split.
 *
 * Given a data instruction, returns the offset to start from.
 */
unsigned int nand_subop_get_data_start_off(const struct nand_subop *subop,
					   unsigned int instr_idx)
{
	if (WARN_ON(!nand_subop_instr_is_valid(subop, instr_idx) ||
		    !nand_instr_is_data(&subop->instrs[instr_idx])))
		return 0;

	return nand_subop_get_start_off(subop, instr_idx);
}
EXPORT_SYMBOL_GPL(nand_subop_get_data_start_off);

/**
 * nand_subop_get_data_len - Get the number of bytes to retrieve
 * @subop: The entire sub-operation
 * @instr_idx: Index of the instruction inside the sub-operation
 *
 * During driver development, one could be tempted to directly use the
 * ->data->len field of a data instruction. This is wrong as data instructions
 * might be split.
 *
 * Returns the length of the chunk of data to send/receive.
 */
unsigned int nand_subop_get_data_len(const struct nand_subop *subop,
				     unsigned int instr_idx)
{
	int start_off = 0, end_off;

	if (WARN_ON(!nand_subop_instr_is_valid(subop, instr_idx) ||
		    !nand_instr_is_data(&subop->instrs[instr_idx])))
		return 0;

	start_off = nand_subop_get_data_start_off(subop, instr_idx);

	if (instr_idx == subop->ninstrs - 1 &&
	    subop->last_instr_end_off)
		end_off = subop->last_instr_end_off;
	else
		end_off = subop->instrs[instr_idx].ctx.data.len;

	return end_off - start_off;
}
EXPORT_SYMBOL_GPL(nand_subop_get_data_len);

/**
 * nand_reset - Reset and initialize a NAND device
 * @chip: The NAND chip
 * @chipnr: Internal die id
 *
 * Save the timings data structure, then apply SDR timings mode 0 (see
 * nand_reset_data_interface for details), do the reset operation, and
 * apply back the previous timings.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
int nand_reset(struct nand_chip *chip, int chipnr)
{
	struct nand_data_interface saved_data_intf = chip->data_interface;
	int ret;

	ret = nand_reset_data_interface(chip, chipnr);
	if (ret)
		return ret;

	/*
	 * The CS line has to be released before we can apply the new NAND
	 * interface settings, hence this weird nand_select_target()
	 * nand_deselect_target() dance.
	 */
	nand_select_target(chip, chipnr);
	ret = nand_reset_op(chip);
	nand_deselect_target(chip);
	if (ret)
		return ret;

	/*
	 * A nand_reset_data_interface() put both the NAND chip and the NAND
	 * controller in timings mode 0. If the default mode for this chip is
	 * also 0, no need to proceed to the change again. Plus, at probe time,
	 * nand_setup_data_interface() uses ->set/get_features() which would
	 * fail anyway as the parameter page is not available yet.
	 */
	if (!chip->onfi_timing_mode_default)
		return 0;

	chip->data_interface = saved_data_intf;
	ret = nand_setup_data_interface(chip, chipnr);
	if (ret)
		return ret;

	return 0;
}
EXPORT_SYMBOL_GPL(nand_reset);

/**
 * nand_get_features - wrapper to perform a GET_FEATURE
 * @chip: NAND chip info structure
 * @addr: feature address
 * @subfeature_param: the subfeature parameters, a four bytes array
 *
 * Returns 0 for success, a negative error otherwise. Returns -ENOTSUPP if the
 * operation cannot be handled.
 */
int nand_get_features(struct nand_chip *chip, int addr,
		      u8 *subfeature_param)
{
	if (!nand_supports_get_features(chip, addr))
		return -ENOTSUPP;

	if (chip->legacy.get_features)
		return chip->legacy.get_features(chip, addr, subfeature_param);

	return nand_get_features_op(chip, addr, subfeature_param);
}

/**
 * nand_set_features - wrapper to perform a SET_FEATURE
 * @chip: NAND chip info structure
 * @addr: feature address
 * @subfeature_param: the subfeature parameters, a four bytes array
 *
 * Returns 0 for success, a negative error otherwise. Returns -ENOTSUPP if the
 * operation cannot be handled.
 */
int nand_set_features(struct nand_chip *chip, int addr,
		      u8 *subfeature_param)
{
	if (!nand_supports_set_features(chip, addr))
		return -ENOTSUPP;

	if (chip->legacy.set_features)
		return chip->legacy.set_features(chip, addr, subfeature_param);

	return nand_set_features_op(chip, addr, subfeature_param);
}

/**
 * nand_check_erased_buf - check if a buffer contains (almost) only 0xff data
 * @buf: buffer to test
 * @len: buffer length
 * @bitflips_threshold: maximum number of bitflips
 *
 * Check if a buffer contains only 0xff, which means the underlying region
 * has been erased and is ready to be programmed.
 * The bitflips_threshold specify the maximum number of bitflips before
 * considering the region is not erased.
 * Note: The logic of this function has been extracted from the memweight
 * implementation, except that nand_check_erased_buf function exit before
 * testing the whole buffer if the number of bitflips exceed the
 * bitflips_threshold value.
 *
 * Returns a positive number of bitflips less than or equal to
 * bitflips_threshold, or -ERROR_CODE for bitflips in excess of the
 * threshold.
 */
static int nand_check_erased_buf(void *buf, int len, int bitflips_threshold)
{
	const unsigned char *bitmap = buf;
	int bitflips = 0;
	int weight;

	for (; len && ((uintptr_t)bitmap) % sizeof(long);
	     len--, bitmap++) {
		weight = hweight8(*bitmap);
		bitflips += BITS_PER_BYTE - weight;
		if (unlikely(bitflips > bitflips_threshold))
			return -EBADMSG;
	}

	for (; len >= sizeof(long);
	     len -= sizeof(long), bitmap += sizeof(long)) {
		unsigned long d = *((unsigned long *)bitmap);
		if (d == ~0UL)
			continue;
		weight = hweight_long(d);
		bitflips += BITS_PER_LONG - weight;
		if (unlikely(bitflips > bitflips_threshold))
			return -EBADMSG;
	}

	for (; len > 0; len--, bitmap++) {
		weight = hweight8(*bitmap);
		bitflips += BITS_PER_BYTE - weight;
		if (unlikely(bitflips > bitflips_threshold))
			return -EBADMSG;
	}

	return bitflips;
}

/**
 * nand_check_erased_ecc_chunk - check if an ECC chunk contains (almost) only
 *				 0xff data
 * @data: data buffer to test
 * @datalen: data length
 * @ecc: ECC buffer
 * @ecclen: ECC length
 * @extraoob: extra OOB buffer
 * @extraooblen: extra OOB length
 * @bitflips_threshold: maximum number of bitflips
 *
 * Check if a data buffer and its associated ECC and OOB data contains only
 * 0xff pattern, which means the underlying region has been erased and is
 * ready to be programmed.
 * The bitflips_threshold specify the maximum number of bitflips before
 * considering the region as not erased.
 *
 * Note:
 * 1/ ECC algorithms are working on pre-defined block sizes which are usually
 *    different from the NAND page size. When fixing bitflips, ECC engines will
 *    report the number of errors per chunk, and the NAND core infrastructure
 *    expect you to return the maximum number of bitflips for the whole page.
 *    This is why you should always use this function on a single chunk and
 *    not on the whole page. After checking each chunk you should update your
 *    max_bitflips value accordingly.
 * 2/ When checking for bitflips in erased pages you should not only check
 *    the payload data but also their associated ECC data, because a user might
 *    have programmed almost all bits to 1 but a few. In this case, we
 *    shouldn't consider the chunk as erased, and checking ECC bytes prevent
 *    this case.
 * 3/ The extraoob argument is optional, and should be used if some of your OOB
 *    data are protected by the ECC engine.
 *    It could also be used if you support subpages and want to attach some
 *    extra OOB data to an ECC chunk.
 *
 * Returns a positive number of bitflips less than or equal to
 * bitflips_threshold, or -ERROR_CODE for bitflips in excess of the
 * threshold. In case of success, the passed buffers are filled with 0xff.
 */
int nand_check_erased_ecc_chunk(void *data, int datalen,
				void *ecc, int ecclen,
				void *extraoob, int extraooblen,
				int bitflips_threshold)
{
	int data_bitflips = 0, ecc_bitflips = 0, extraoob_bitflips = 0;

	data_bitflips = nand_check_erased_buf(data, datalen,
					      bitflips_threshold);
	if (data_bitflips < 0)
		return data_bitflips;

	bitflips_threshold -= data_bitflips;

	ecc_bitflips = nand_check_erased_buf(ecc, ecclen, bitflips_threshold);
	if (ecc_bitflips < 0)
		return ecc_bitflips;

	bitflips_threshold -= ecc_bitflips;

	extraoob_bitflips = nand_check_erased_buf(extraoob, extraooblen,
						  bitflips_threshold);
	if (extraoob_bitflips < 0)
		return extraoob_bitflips;

	if (data_bitflips)
		memset(data, 0xff, datalen);

	if (ecc_bitflips)
		memset(ecc, 0xff, ecclen);

	if (extraoob_bitflips)
		memset(extraoob, 0xff, extraooblen);

	return data_bitflips + ecc_bitflips + extraoob_bitflips;
}
EXPORT_SYMBOL(nand_check_erased_ecc_chunk);

/**
 * nand_read_page_raw_notsupp - dummy read raw page function
 * @chip: nand chip info structure
 * @buf: buffer to store read data
 * @oob_required: caller requires OOB data read to chip->oob_poi
 * @page: page number to read
 *
 * Returns -ENOTSUPP unconditionally.
 */
int nand_read_page_raw_notsupp(struct nand_chip *chip, u8 *buf,
			       int oob_required, int page, int encrypted)
{
	return -ENOTSUPP;
}

/**
 * nand_read_page_raw - [INTERN] read raw page data without ecc
 * @chip: nand chip info structure
 * @buf: buffer to store read data
 * @oob_required: caller requires OOB data read to chip->oob_poi
 * @page: page number to read
 *
 * Not for syndrome calculating ECC controllers, which use a special oob layout.
 */
int nand_read_page_raw(struct nand_chip *chip, uint8_t *buf, int oob_required,
		       int page, int encrypted)
{
	BUG_ON(encrypted);
	struct mtd_info *mtd = nand_to_mtd(chip);
	int ret;

	ret = nand_read_page_op(chip, page, 0, buf, mtd->writesize);
	if (ret)
		return ret;

	if (oob_required) {
		ret = nand_read_data_op(chip, chip->oob_poi, mtd->oobsize,
					false);
		if (ret)
			return ret;
	}

	return 0;
}
EXPORT_SYMBOL(nand_read_page_raw);

/**
 * nand_read_page_raw_syndrome - [INTERN] read raw page data without ecc
 * @chip: nand chip info structure
 * @buf: buffer to store read data
 * @oob_required: caller requires OOB data read to chip->oob_poi
 * @page: page number to read
 *
 * We need a special oob layout and handling even when OOB isn't used.
 */
static int nand_read_page_raw_syndrome(struct nand_chip *chip, uint8_t *buf,
				       int oob_required, int page, int encrypted)
{
	BUG_ON(encrypted);
	struct mtd_info *mtd = nand_to_mtd(chip);
	int eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	uint8_t *oob = chip->oob_poi;
	int steps, size, ret;

	ret = nand_read_page_op(chip, page, 0, NULL, 0);
	if (ret)
		return ret;

	for (steps = chip->ecc.steps; steps > 0; steps--) {
		ret = nand_read_data_op(chip, buf, eccsize, false);
		if (ret)
			return ret;

		buf += eccsize;

		if (chip->ecc.prepad) {
			ret = nand_read_data_op(chip, oob, chip->ecc.prepad,
						false);
			if (ret)
				return ret;

			oob += chip->ecc.prepad;
		}

		ret = nand_read_data_op(chip, oob, eccbytes, false);
		if (ret)
			return ret;

		oob += eccbytes;

		if (chip->ecc.postpad) {
			ret = nand_read_data_op(chip, oob, chip->ecc.postpad,
						false);
			if (ret)
				return ret;

			oob += chip->ecc.postpad;
		}
	}

	size = mtd->oobsize - (oob - chip->oob_poi);
	if (size) {
		ret = nand_read_data_op(chip, oob, size, false);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * nand_read_page_swecc - [REPLACEABLE] software ECC based page read function
 * @chip: nand chip info structure
 * @buf: buffer to store read data
 * @oob_required: caller requires OOB data read to chip->oob_poi
 * @page: page number to read
 */
static int nand_read_page_swecc(struct nand_chip *chip, uint8_t *buf,
				int oob_required, int page, int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int i, eccsize = chip->ecc.size, ret;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *p = buf;
	uint8_t *ecc_calc = chip->ecc.calc_buf;
	uint8_t *ecc_code = chip->ecc.code_buf;
	unsigned int max_bitflips = 0;

	chip->ecc.read_page_raw(chip, buf, 1, page, 0); // NOT_ENCRYPTED

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize)
		chip->ecc.calculate(chip, p, &ecc_calc[i]);

	ret = mtd_ooblayout_get_eccbytes(mtd, ecc_code, chip->oob_poi, 0,
					 chip->ecc.total);
	if (ret)
		return ret;

	eccsteps = chip->ecc.steps;
	p = buf;

	for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		int stat;

		stat = chip->ecc.correct(chip, p, &ecc_code[i], &ecc_calc[i]);
		if (stat < 0) {
			mtd->ecc_stats.failed++;
		} else {
			mtd->ecc_stats.corrected += stat;
			max_bitflips = max_t(unsigned int, max_bitflips, stat);
		}
	}
#ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION
	if (encrypted) roku_nand_decrypt_page(mtd, buf, page);
#endif
	return max_bitflips;
}

/**
 * nand_read_subpage - [REPLACEABLE] ECC based sub-page read function
 * @chip: nand chip info structure
 * @data_offs: offset of requested data within the page
 * @readlen: data length
 * @bufpoi: buffer to store read data
 * @page: page number to read
 */
static int nand_read_subpage(struct nand_chip *chip, uint32_t data_offs,
			     uint32_t readlen, uint8_t *bufpoi, int page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int start_step, end_step, num_steps, ret;
	uint8_t *p;
	int data_col_addr, i, gaps = 0;
	int datafrag_len, eccfrag_len, aligned_len, aligned_pos;
	int busw = (chip->options & NAND_BUSWIDTH_16) ? 2 : 1;
	int index, section = 0;
	unsigned int max_bitflips = 0;
	struct mtd_oob_region oobregion = { };

	/* Column address within the page aligned to ECC size (256bytes) */
	start_step = data_offs / chip->ecc.size;
	end_step = (data_offs + readlen - 1) / chip->ecc.size;
	num_steps = end_step - start_step + 1;
	index = start_step * chip->ecc.bytes;

	/* Data size aligned to ECC ecc.size */
	datafrag_len = num_steps * chip->ecc.size;
	eccfrag_len = num_steps * chip->ecc.bytes;

	data_col_addr = start_step * chip->ecc.size;
	/* If we read not a page aligned data */
	p = bufpoi + data_col_addr;
	ret = nand_read_page_op(chip, page, data_col_addr, p, datafrag_len);
	if (ret)
		return ret;

	/* Calculate ECC */
	for (i = 0; i < eccfrag_len ; i += chip->ecc.bytes, p += chip->ecc.size)
		chip->ecc.calculate(chip, p, &chip->ecc.calc_buf[i]);

	/*
	 * The performance is faster if we position offsets according to
	 * ecc.pos. Let's make sure that there are no gaps in ECC positions.
	 */
	ret = mtd_ooblayout_find_eccregion(mtd, index, &section, &oobregion);
	if (ret)
		return ret;

	if (oobregion.length < eccfrag_len)
		gaps = 1;

	if (gaps) {
		ret = nand_change_read_column_op(chip, mtd->writesize,
						 chip->oob_poi, mtd->oobsize,
						 false);
		if (ret)
			return ret;
	} else {
		/*
		 * Send the command to read the particular ECC bytes take care
		 * about buswidth alignment in read_buf.
		 */
		aligned_pos = oobregion.offset & ~(busw - 1);
		aligned_len = eccfrag_len;
		if (oobregion.offset & (busw - 1))
			aligned_len++;
		if ((oobregion.offset + (num_steps * chip->ecc.bytes)) &
		    (busw - 1))
			aligned_len++;

		ret = nand_change_read_column_op(chip,
						 mtd->writesize + aligned_pos,
						 &chip->oob_poi[aligned_pos],
						 aligned_len, false);
		if (ret)
			return ret;
	}

	ret = mtd_ooblayout_get_eccbytes(mtd, chip->ecc.code_buf,
					 chip->oob_poi, index, eccfrag_len);
	if (ret)
		return ret;

	p = bufpoi + data_col_addr;
	for (i = 0; i < eccfrag_len ; i += chip->ecc.bytes, p += chip->ecc.size) {
		int stat;

		stat = chip->ecc.correct(chip, p, &chip->ecc.code_buf[i],
					 &chip->ecc.calc_buf[i]);
		if (stat == -EBADMSG &&
		    (chip->ecc.options & NAND_ECC_GENERIC_ERASED_CHECK)) {
			/* check for empty pages with bitflips */
			stat = nand_check_erased_ecc_chunk(p, chip->ecc.size,
						&chip->ecc.code_buf[i],
						chip->ecc.bytes,
						NULL, 0,
						chip->ecc.strength);
		}

		if (stat < 0) {
			mtd->ecc_stats.failed++;
		} else {
			mtd->ecc_stats.corrected += stat;
			max_bitflips = max_t(unsigned int, max_bitflips, stat);
		}
	}

//  81x remove this
// #ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION
//	if (encrypted) roku_nand_decrypt_page(mtd, buf, page);
// #endif
	return max_bitflips;
}

/**
 * nand_read_page_hwecc - [REPLACEABLE] hardware ECC based page read function
 * @chip: nand chip info structure
 * @buf: buffer to store read data
 * @oob_required: caller requires OOB data read to chip->oob_poi
 * @page: page number to read
 *
 * Not for syndrome calculating ECC controllers which need a special oob layout.
 */
static int nand_read_page_hwecc(struct nand_chip *chip, uint8_t *buf,
				int oob_required, int page, int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int i, eccsize = chip->ecc.size, ret;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *p = buf;
	uint8_t *ecc_calc = chip->ecc.calc_buf;
	uint8_t *ecc_code = chip->ecc.code_buf;
	unsigned int max_bitflips = 0;

	ret = nand_read_page_op(chip, page, 0, NULL, 0);
	if (ret)
		return ret;

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->ecc.hwctl(chip, NAND_ECC_READ);

		ret = nand_read_data_op(chip, p, eccsize, false);
		if (ret)
			return ret;

		chip->ecc.calculate(chip, p, &ecc_calc[i]);
	}

	ret = nand_read_data_op(chip, chip->oob_poi, mtd->oobsize, false);
	if (ret)
		return ret;

	ret = mtd_ooblayout_get_eccbytes(mtd, ecc_code, chip->oob_poi, 0,
					 chip->ecc.total);
	if (ret)
		return ret;

	eccsteps = chip->ecc.steps;
	p = buf;

	BUG_ON(encrypted);
	for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		int stat;

		stat = chip->ecc.correct(chip, p, &ecc_code[i], &ecc_calc[i]);
		if (stat == -EBADMSG &&
		    (chip->ecc.options & NAND_ECC_GENERIC_ERASED_CHECK)) {
			/* check for empty pages with bitflips */
			stat = nand_check_erased_ecc_chunk(p, eccsize,
						&ecc_code[i], eccbytes,
						NULL, 0,
						chip->ecc.strength);
		}

		if (stat < 0) {
			mtd->ecc_stats.failed++;
		} else {
			mtd->ecc_stats.corrected += stat;
			max_bitflips = max_t(unsigned int, max_bitflips, stat);
		}
	}
#ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION
	if (encrypted) roku_nand_decrypt_page(mtd, buf, page);
#endif
	return max_bitflips;
}

/**
 * nand_read_page_hwecc_oob_first - [REPLACEABLE] hw ecc, read oob first
 * @chip: nand chip info structure
 * @buf: buffer to store read data
 * @oob_required: caller requires OOB data read to chip->oob_poi
 * @page: page number to read
 *
 * Hardware ECC for large page chips, require OOB to be read first. For this
 * ECC mode, the write_page method is re-used from ECC_HW. These methods
 * read/write ECC from the OOB area, unlike the ECC_HW_SYNDROME support with
 * multiple ECC steps, follows the "infix ECC" scheme and reads/writes ECC from
 * the data area, by overwriting the NAND manufacturer bad block markings.
 */
static int nand_read_page_hwecc_oob_first(struct nand_chip *chip, uint8_t *buf,
					  int oob_required, int page, int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int i, eccsize = chip->ecc.size, ret;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *p = buf;
	uint8_t *ecc_code = chip->ecc.code_buf;
	uint8_t *ecc_calc = chip->ecc.calc_buf;
	unsigned int max_bitflips = 0;

	BUG_ON(encrypted);
	/* Read the OOB area first */
	ret = nand_read_oob_op(chip, page, 0, chip->oob_poi, mtd->oobsize);
	if (ret)
		return ret;

	ret = nand_read_page_op(chip, page, 0, NULL, 0);
	if (ret)
		return ret;

	ret = mtd_ooblayout_get_eccbytes(mtd, ecc_code, chip->oob_poi, 0,
					 chip->ecc.total);
	if (ret)
		return ret;

	BUG_ON(encrypted);
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		int stat;

		chip->ecc.hwctl(chip, NAND_ECC_READ);

		ret = nand_read_data_op(chip, p, eccsize, false);
		if (ret)
			return ret;

		chip->ecc.calculate(chip, p, &ecc_calc[i]);

		stat = chip->ecc.correct(chip, p, &ecc_code[i], NULL);
		if (stat == -EBADMSG &&
		    (chip->ecc.options & NAND_ECC_GENERIC_ERASED_CHECK)) {
			/* check for empty pages with bitflips */
			stat = nand_check_erased_ecc_chunk(p, eccsize,
						&ecc_code[i], eccbytes,
						NULL, 0,
						chip->ecc.strength);
		}

		if (stat < 0) {
			mtd->ecc_stats.failed++;
		} else {
			mtd->ecc_stats.corrected += stat;
			max_bitflips = max_t(unsigned int, max_bitflips, stat);
		}
	}
	return max_bitflips;
}

/**
 * nand_read_page_syndrome - [REPLACEABLE] hardware ECC syndrome based page read
 * @chip: nand chip info structure
 * @buf: buffer to store read data
 * @oob_required: caller requires OOB data read to chip->oob_poi
 * @page: page number to read
 *
 * The hw generator calculates the error syndrome automatically. Therefore we
 * need a special oob layout and handling.
 */
static int nand_read_page_syndrome(struct nand_chip *chip, uint8_t *buf,
				   int oob_required, int page, int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int ret, i, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	int eccpadbytes = eccbytes + chip->ecc.prepad + chip->ecc.postpad;
	uint8_t *p = buf;
	uint8_t *oob = chip->oob_poi;
	unsigned int max_bitflips = 0;

	ret = nand_read_page_op(chip, page, 0, NULL, 0);
	if (ret)
		return ret;

	BUG_ON(encrypted);
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		int stat;

		chip->ecc.hwctl(chip, NAND_ECC_READ);

		ret = nand_read_data_op(chip, p, eccsize, false);
		if (ret)
			return ret;

		if (chip->ecc.prepad) {
			ret = nand_read_data_op(chip, oob, chip->ecc.prepad,
						false);
			if (ret)
				return ret;

			oob += chip->ecc.prepad;
		}

		chip->ecc.hwctl(chip, NAND_ECC_READSYN);

		ret = nand_read_data_op(chip, oob, eccbytes, false);
		if (ret)
			return ret;

		stat = chip->ecc.correct(chip, p, oob, NULL);

		oob += eccbytes;

		if (chip->ecc.postpad) {
			ret = nand_read_data_op(chip, oob, chip->ecc.postpad,
						false);
			if (ret)
				return ret;

			oob += chip->ecc.postpad;
		}

		if (stat == -EBADMSG &&
		    (chip->ecc.options & NAND_ECC_GENERIC_ERASED_CHECK)) {
			/* check for empty pages with bitflips */
			stat = nand_check_erased_ecc_chunk(p, chip->ecc.size,
							   oob - eccpadbytes,
							   eccpadbytes,
							   NULL, 0,
							   chip->ecc.strength);
		}

		if (stat < 0) {
			mtd->ecc_stats.failed++;
		} else {
			mtd->ecc_stats.corrected += stat;
			max_bitflips = max_t(unsigned int, max_bitflips, stat);
		}
	}

	/* Calculate remaining oob bytes */
	i = mtd->oobsize - (oob - chip->oob_poi);
	if (i) {
		ret = nand_read_data_op(chip, oob, i, false);
		if (ret)
			return ret;
	}

	return max_bitflips;
}

/**
 * nand_transfer_oob - [INTERN] Transfer oob to client buffer
 * @chip: NAND chip object
 * @oob: oob destination address
 * @ops: oob ops structure
 * @len: size of oob to transfer
 */
static uint8_t *nand_transfer_oob(struct nand_chip *chip, uint8_t *oob,
				  struct mtd_oob_ops *ops, size_t len)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int ret;

	switch (ops->mode) {

	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_RAW:
		memcpy(oob, chip->oob_poi + ops->ooboffs, len);
		return oob + len;

	case MTD_OPS_AUTO_OOB:
		ret = mtd_ooblayout_get_databytes(mtd, oob, chip->oob_poi,
						  ops->ooboffs, len);
		BUG_ON(ret);
		return oob + len;

	default:
		BUG();
	}
	return NULL;
}

/**
 * nand_setup_read_retry - [INTERN] Set the READ RETRY mode
 * @chip: NAND chip object
 * @retry_mode: the retry mode to use
 *
 * Some vendors supply a special command to shift the Vt threshold, to be used
 * when there are too many bitflips in a page (i.e., ECC error). After setting
 * a new threshold, the host should retry reading the page.
 */
static int nand_setup_read_retry(struct nand_chip *chip, int retry_mode)
{
	pr_debug("setting READ RETRY mode %d\n", retry_mode);

	if (retry_mode >= chip->read_retries)
		return -EINVAL;

	if (!chip->setup_read_retry)
		return -EOPNOTSUPP;

	return chip->setup_read_retry(chip, retry_mode);
}

static void nand_wait_readrdy(struct nand_chip *chip)
{
	const struct nand_sdr_timings *sdr;

	if (!(chip->options & NAND_NEED_READRDY))
		return;

	sdr = nand_get_sdr_timings(&chip->data_interface);
	WARN_ON(nand_wait_rdy_op(chip, PSEC_TO_MSEC(sdr->tR_max), 0));
}

/**
 * nand_do_read_ops - [INTERN] Read data with ECC
 * @chip: NAND chip object
 * @from: offset to read from
 * @ops: oob ops structure
 *
 * Internal function. Called with chip held.
 */
static int nand_do_read_ops(struct nand_chip *chip, loff_t from,
			    struct mtd_oob_ops *ops, int encrypted)
{
	int chipnr, page, realpage, col, bytes, aligned, oob_required;
	struct mtd_info *mtd = nand_to_mtd(chip);
	int ret = 0;
	uint32_t readlen = ops->len;
	uint32_t oobreadlen = ops->ooblen;
	uint32_t max_oobsize = mtd_oobavail(mtd, ops);

	uint8_t *bufpoi, *oob, *buf;
	int use_bufpoi;
	unsigned int max_bitflips = 0;
	int retry_mode = 0;
	bool ecc_fail = false;

	chipnr = (int)(from >> chip->chip_shift);
	nand_select_target(chip, chipnr);

	realpage = (int)(from >> chip->page_shift);
	page = realpage & chip->pagemask;

	col = (int)(from & (mtd->writesize - 1));

	buf = ops->datbuf;
	oob = ops->oobbuf;
	oob_required = oob ? 1 : 0;

	while (1) {
		unsigned int ecc_failures = mtd->ecc_stats.failed;

		bytes = min(mtd->writesize - col, readlen);
		aligned = (bytes == mtd->writesize);

		if (!aligned)
			use_bufpoi = 1;
		else if (chip->options & NAND_USE_BOUNCE_BUFFER)
			use_bufpoi = !virt_addr_valid(buf) ||
				     !IS_ALIGNED((unsigned long)buf,
						 chip->buf_align);
		else
			use_bufpoi = 0;

		/* Is the current page in the buffer? */
		if (realpage != chip->pagecache.page || oob) {
			bufpoi = use_bufpoi ? chip->data_buf : buf;

			if (use_bufpoi && aligned)
				pr_debug("%s: using read bounce buffer for buf@%p\n",
						 __func__, buf);

read_retry:
			/*
			 * Now read the page into the buffer.  Absent an error,
			 * the read methods return max bitflips per ecc step.
			 */
			if (unlikely(ops->mode == MTD_OPS_RAW))
				ret = chip->ecc.read_page_raw(chip, bufpoi,
							      oob_required,
							      page, 0);
			else if (!aligned && NAND_HAS_SUBPAGE_READ(chip) &&
				 !oob)
				ret = chip->ecc.read_subpage(chip, col, bytes,
							     bufpoi, page);
			else
				ret = chip->ecc.read_page(chip, bufpoi,
							  oob_required, page, 0);
			if (ret < 0) {
				if (use_bufpoi)
					/* Invalidate page cache */
					chip->pagecache.page = -1;
				break;
			}

			/* Transfer not aligned data */
			if (use_bufpoi) {
				if (!NAND_HAS_SUBPAGE_READ(chip) && !oob &&
				    !(mtd->ecc_stats.failed - ecc_failures) &&
				    (ops->mode != MTD_OPS_RAW)) {
					chip->pagecache.page = realpage;
					chip->pagecache.bitflips = ret;
				} else {
					/* Invalidate page cache */
					chip->pagecache.page = -1;
				}
				memcpy(buf, chip->data_buf + col, bytes);
			}

			if (unlikely(oob)) {
				int toread = min(oobreadlen, max_oobsize);

				if (toread) {
					oob = nand_transfer_oob(chip, oob, ops,
								toread);
					oobreadlen -= toread;
				}
			}

			nand_wait_readrdy(chip);

			if (mtd->ecc_stats.failed - ecc_failures) {
				if (retry_mode + 1 < chip->read_retries) {
					retry_mode++;
					ret = nand_setup_read_retry(chip,
							retry_mode);
					if (ret < 0)
						break;

					/* Reset failures; retry */
					mtd->ecc_stats.failed = ecc_failures;
					goto read_retry;
				} else {
					/* No more retry modes; real failure */
					ecc_fail = true;
				}
			}

			buf += bytes;
			max_bitflips = max_t(unsigned int, max_bitflips, ret);
		} else {
			memcpy(buf, chip->data_buf + col, bytes);
			buf += bytes;
			max_bitflips = max_t(unsigned int, max_bitflips,
					     chip->pagecache.bitflips);
		}

		readlen -= bytes;

		/* Reset to retry mode 0 */
		if (retry_mode) {
			ret = nand_setup_read_retry(chip, 0);
			if (ret < 0)
				break;
			retry_mode = 0;
		}

		if (!readlen)
			break;

		/* For subsequent reads align to page boundary */
		col = 0;
		/* Increment page address */
		realpage++;

		page = realpage & chip->pagemask;
		/* Check, if we cross a chip boundary */
		if (!page) {
			chipnr++;
			nand_deselect_target(chip);
			nand_select_target(chip, chipnr);
		}
	}
	nand_deselect_target(chip);

	ops->retlen = ops->len - (size_t) readlen;
	if (oob)
		ops->oobretlen = ops->ooblen - oobreadlen;

	if (ret < 0)
		return ret;

	if (ecc_fail)
		return -EBADMSG;

	return max_bitflips;
}

/**
 * nand_read_oob_std - [REPLACEABLE] the most common OOB data read function
 * @chip: nand chip info structure
 * @page: page number to read
 */
int nand_read_oob_std(struct nand_chip *chip, int page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	return nand_read_oob_op(chip, page, 0, chip->oob_poi, mtd->oobsize);
}
EXPORT_SYMBOL(nand_read_oob_std);

/**
 * nand_read_oob_syndrome - [REPLACEABLE] OOB data read function for HW ECC
 *			    with syndromes
 * @chip: nand chip info structure
 * @page: page number to read
 */
static int nand_read_oob_syndrome(struct nand_chip *chip, int page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int length = mtd->oobsize;
	int chunk = chip->ecc.bytes + chip->ecc.prepad + chip->ecc.postpad;
	int eccsize = chip->ecc.size;
	uint8_t *bufpoi = chip->oob_poi;
	int i, toread, sndrnd = 0, pos, ret;

	ret = nand_read_page_op(chip, page, chip->ecc.size, NULL, 0);
	if (ret)
		return ret;

	for (i = 0; i < chip->ecc.steps; i++) {
		if (sndrnd) {
			int ret;

			pos = eccsize + i * (eccsize + chunk);
			if (mtd->writesize > 512)
				ret = nand_change_read_column_op(chip, pos,
								 NULL, 0,
								 false);
			else
				ret = nand_read_page_op(chip, page, pos, NULL,
							0);

			if (ret)
				return ret;
		} else
			sndrnd = 1;
		toread = min_t(int, length, chunk);

		ret = nand_read_data_op(chip, bufpoi, toread, false);
		if (ret)
			return ret;

		bufpoi += toread;
		length -= toread;
	}
	if (length > 0) {
		ret = nand_read_data_op(chip, bufpoi, length, false);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * nand_write_oob_std - [REPLACEABLE] the most common OOB data write function
 * @chip: nand chip info structure
 * @page: page number to write
 */
int nand_write_oob_std(struct nand_chip *chip, int page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	return nand_prog_page_op(chip, page, mtd->writesize, chip->oob_poi,
				 mtd->oobsize);
}
EXPORT_SYMBOL(nand_write_oob_std);

/**
 * nand_write_oob_syndrome - [REPLACEABLE] OOB data write function for HW ECC
 *			     with syndrome - only for large page flash
 * @chip: nand chip info structure
 * @page: page number to write
 */
static int nand_write_oob_syndrome(struct nand_chip *chip, int page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int chunk = chip->ecc.bytes + chip->ecc.prepad + chip->ecc.postpad;
	int eccsize = chip->ecc.size, length = mtd->oobsize;
	int ret, i, len, pos, sndcmd = 0, steps = chip->ecc.steps;
	const uint8_t *bufpoi = chip->oob_poi;

	/*
	 * data-ecc-data-ecc ... ecc-oob
	 * or
	 * data-pad-ecc-pad-data-pad .... ecc-pad-oob
	 */
	if (!chip->ecc.prepad && !chip->ecc.postpad) {
		pos = steps * (eccsize + chunk);
		steps = 0;
	} else
		pos = eccsize;

	ret = nand_prog_page_begin_op(chip, page, pos, NULL, 0);
	if (ret)
		return ret;

	for (i = 0; i < steps; i++) {
		if (sndcmd) {
			if (mtd->writesize <= 512) {
				uint32_t fill = 0xFFFFFFFF;

				len = eccsize;
				while (len > 0) {
					int num = min_t(int, len, 4);

					ret = nand_write_data_op(chip, &fill,
								 num, false);
					if (ret)
						return ret;

					len -= num;
				}
			} else {
				pos = eccsize + i * (eccsize + chunk);
				ret = nand_change_write_column_op(chip, pos,
								  NULL, 0,
								  false);
				if (ret)
					return ret;
			}
		} else
			sndcmd = 1;
		len = min_t(int, length, chunk);

		ret = nand_write_data_op(chip, bufpoi, len, false);
		if (ret)
			return ret;

		bufpoi += len;
		length -= len;
	}
	if (length > 0) {
		ret = nand_write_data_op(chip, bufpoi, length, false);
		if (ret)
			return ret;
	}

	return nand_prog_page_end_op(chip);
}

/**
 * nand_do_read_oob - [INTERN] NAND read out-of-band
 * @chip: NAND chip object
 * @from: offset to read from
 * @ops: oob operations description structure
 *
 * NAND read out-of-band data from the spare area.
 */
static int nand_do_read_oob(struct nand_chip *chip, loff_t from,
			    struct mtd_oob_ops *ops)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	unsigned int max_bitflips = 0;
	int page, realpage, chipnr;
	struct mtd_ecc_stats stats;
	int readlen = ops->ooblen;
	int len;
	uint8_t *buf = ops->oobbuf;
	int ret = 0;

	pr_debug("%s: from = 0x%08Lx, len = %i\n",
			__func__, (unsigned long long)from, readlen);

	stats = mtd->ecc_stats;

	len = mtd_oobavail(mtd, ops);

	chipnr = (int)(from >> chip->chip_shift);
	nand_select_target(chip, chipnr);

	/* Shift to get page */
	realpage = (int)(from >> chip->page_shift);
	page = realpage & chip->pagemask;

	while (1) {
		if (ops->mode == MTD_OPS_RAW)
			ret = chip->ecc.read_oob_raw(chip, page);
		else
			ret = chip->ecc.read_oob(chip, page);

		if (ret < 0)
			break;

		len = min(len, readlen);
		buf = nand_transfer_oob(chip, buf, ops, len);

		nand_wait_readrdy(chip);

		max_bitflips = max_t(unsigned int, max_bitflips, ret);

		readlen -= len;
		if (!readlen)
			break;

		/* Increment page address */
		realpage++;

		page = realpage & chip->pagemask;
		/* Check, if we cross a chip boundary */
		if (!page) {
			chipnr++;
			nand_deselect_target(chip);
			nand_select_target(chip, chipnr);
		}
	}
	nand_deselect_target(chip);

	ops->oobretlen = ops->ooblen - readlen;

	if (ret < 0)
		return ret;

	if (mtd->ecc_stats.failed - stats.failed)
		return -EBADMSG;

	return max_bitflips;
}

/**
 * nand_read_oob - [MTD Interface] NAND read data and/or out-of-band
 * @mtd: MTD device structure
 * @from: offset to read from
 * @ops: oob operation description structure
 *
 * NAND read data and/or out-of-band data.
 */
static int nand_read_oob(struct mtd_info *mtd, loff_t from,
			 struct mtd_oob_ops *ops, int encrypted)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	int ret;

	ops->retlen = 0;

	if (ops->mode != MTD_OPS_PLACE_OOB &&
	    ops->mode != MTD_OPS_AUTO_OOB &&
	    ops->mode != MTD_OPS_RAW)
		return -ENOTSUPP;

	ret = nand_get_device(chip);
	if (ret)
		return ret;

	if (!ops->datbuf) {
		BUG_ON(encrypted);
		ret = nand_do_read_oob(chip, from, ops);
	} else
		ret = nand_do_read_ops(chip, from, ops, encrypted);

	nand_release_device(chip);
	return ret;
}

/**
 * nand_write_page_raw_notsupp - dummy raw page write function
 * @chip: nand chip info structure
 * @buf: data buffer
 * @oob_required: must write chip->oob_poi to OOB
 * @page: page number to write
 *
 * Returns -ENOTSUPP unconditionally.
 */
int nand_write_page_raw_notsupp(struct nand_chip *chip, const u8 *buf,
				int oob_required, int page)
{
	return -ENOTSUPP;
}

/**
 * nand_write_page_raw - [INTERN] raw page write function
 * @chip: nand chip info structure
 * @buf: data buffer
 * @oob_required: must write chip->oob_poi to OOB
 * @page: page number to write
 *
 * Not for syndrome calculating ECC controllers, which use a special oob layout.
 */
int nand_write_page_raw(struct nand_chip *chip, const uint8_t *buf,
			int oob_required, int page, int encrypted)
{
	BUG_ON(encrypted);
	struct mtd_info *mtd = nand_to_mtd(chip);
	int ret;

	ret = nand_prog_page_begin_op(chip, page, 0, buf, mtd->writesize);
	if (ret)
		return ret;

	if (oob_required) {
		ret = nand_write_data_op(chip, chip->oob_poi, mtd->oobsize,
					 false);
		if (ret)
			return ret;
	}

	return nand_prog_page_end_op(chip);
}
EXPORT_SYMBOL(nand_write_page_raw);

/**
 * nand_write_page_raw_syndrome - [INTERN] raw page write function
 * @chip: nand chip info structure
 * @buf: data buffer
 * @oob_required: must write chip->oob_poi to OOB
 * @page: page number to write
 *
 * We need a special oob layout and handling even when ECC isn't checked.
 */
static int nand_write_page_raw_syndrome(struct nand_chip *chip,
					const uint8_t *buf, int oob_required,
					int page, int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	uint8_t *oob = chip->oob_poi;
	int steps, size, ret;

	BUG_ON(encrypted);
	ret = nand_prog_page_begin_op(chip, page, 0, NULL, 0);
	if (ret)
		return ret;

	for (steps = chip->ecc.steps; steps > 0; steps--) {
		ret = nand_write_data_op(chip, buf, eccsize, false);
		if (ret)
			return ret;

		buf += eccsize;

		if (chip->ecc.prepad) {
			ret = nand_write_data_op(chip, oob, chip->ecc.prepad,
						 false);
			if (ret)
				return ret;

			oob += chip->ecc.prepad;
		}

		ret = nand_write_data_op(chip, oob, eccbytes, false);
		if (ret)
			return ret;

		oob += eccbytes;

		if (chip->ecc.postpad) {
			ret = nand_write_data_op(chip, oob, chip->ecc.postpad,
						 false);
			if (ret)
				return ret;

			oob += chip->ecc.postpad;
		}
	}

	size = mtd->oobsize - (oob - chip->oob_poi);
	if (size) {
		ret = nand_write_data_op(chip, oob, size, false);
		if (ret)
			return ret;
	}

	return nand_prog_page_end_op(chip);
}
/**
 * nand_write_page_swecc - [REPLACEABLE] software ECC based page write function
 * @chip: nand chip info structure
 * @buf: data buffer
 * @oob_required: must write chip->oob_poi to OOB
 * @page: page number to write
 */
static int nand_write_page_swecc(struct nand_chip *chip, const uint8_t *buf,
				 int oob_required, int page, int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int i, eccsize = chip->ecc.size, ret;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *ecc_calc = chip->ecc.calc_buf;
	const uint8_t *p = buf;


#ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION
	if (encrypted)
		p = buf = roku_nand_encrypt_page(mtd, buf, page);
	BUG_ON(!p);
#else
	(void) page;
	BUG_ON(encrypted);
#endif

	/* Software ECC calculation */
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize)
		chip->ecc.calculate(chip, p, &ecc_calc[i]);

	ret = mtd_ooblayout_set_eccbytes(mtd, ecc_calc, chip->oob_poi, 0, chip->ecc.total);
	if (ret) {
		if (encrypted)
			kfree(buf);
		return ret;
	}

	ret = chip->ecc.write_page_raw(chip, buf, 1, page, 0);

#ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION
	if (encrypted)
		kfree(buf);
#endif
	return ret;

}

/**
 * nand_write_page_hwecc - [REPLACEABLE] hardware ECC based page write function
 * @chip: nand chip info structure
 * @buf: data buffer
 * @oob_required: must write chip->oob_poi to OOB
 * @page: page number to write
 */
static int nand_write_page_hwecc(struct nand_chip *chip, const uint8_t *buf,
				 int oob_required, int page, int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int i, eccsize = chip->ecc.size, ret;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *ecc_calc = chip->ecc.calc_buf;
	const uint8_t *p = buf;

	ret = nand_prog_page_begin_op(chip, page, 0, NULL, 0);
	if (ret)
		return ret;

#ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION
	if (encrypted)
		p = buf = roku_nand_encrypt_page(mtd, buf, page);
#else
	(void) page;
	BUG_ON(encrypted);
#endif

#ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION
	if (encrypted)
		p = buf = roku_nand_encrypt_page(mtd, buf, page);
#else
	(void) page;
	BUG_ON(encrypted);
#endif

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->ecc.hwctl(chip, NAND_ECC_WRITE);

		ret = nand_write_data_op(chip, p, eccsize, false);
		if (ret)
			return ret;

		chip->ecc.calculate(chip, p, &ecc_calc[i]);
	}

	ret = mtd_ooblayout_set_eccbytes(mtd, ecc_calc, chip->oob_poi, 0,
					 chip->ecc.total);
	if (ret)
#ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION
		if (encrypted)
			kfree(buf);
#endif
	ret = nand_write_data_op(chip, chip->oob_poi, mtd->oobsize, false);
	if (ret)
		return ret;
#ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION
	if (encrypted)
		kfree(buf);
#endif
	return nand_prog_page_end_op(chip);
}


/**
 * nand_write_subpage_hwecc - [REPLACEABLE] hardware ECC based subpage write
 * @chip:	nand chip info structure
 * @offset:	column address of subpage within the page
 * @data_len:	data length
 * @buf:	data buffer
 * @oob_required: must write chip->oob_poi to OOB
 * @page: page number to write
 */
static int nand_write_subpage_hwecc(struct nand_chip *chip, uint32_t offset,
				    uint32_t data_len, const uint8_t *buf,
				    int oob_required, int page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	uint8_t *oob_buf  = chip->oob_poi;
	uint8_t *ecc_calc = chip->ecc.calc_buf;
	int ecc_size      = chip->ecc.size;
	int ecc_bytes     = chip->ecc.bytes;
	int ecc_steps     = chip->ecc.steps;
	uint32_t start_step = offset / ecc_size;
	uint32_t end_step   = (offset + data_len - 1) / ecc_size;
	int oob_bytes       = mtd->oobsize / ecc_steps;
	int step, ret;

	ret = nand_prog_page_begin_op(chip, page, 0, NULL, 0);
	if (ret)
		return ret;

	for (step = 0; step < ecc_steps; step++) {
		/* configure controller for WRITE access */
		chip->ecc.hwctl(chip, NAND_ECC_WRITE);

		/* write data (untouched subpages already masked by 0xFF) */
		ret = nand_write_data_op(chip, buf, ecc_size, false);
		if (ret)
			return ret;

		/* mask ECC of un-touched subpages by padding 0xFF */
		if ((step < start_step) || (step > end_step))
			memset(ecc_calc, 0xff, ecc_bytes);
		else
			chip->ecc.calculate(chip, buf, ecc_calc);

		/* mask OOB of un-touched subpages by padding 0xFF */
		/* if oob_required, preserve OOB metadata of written subpage */
		if (!oob_required || (step < start_step) || (step > end_step))
			memset(oob_buf, 0xff, oob_bytes);

		buf += ecc_size;
		ecc_calc += ecc_bytes;
		oob_buf  += oob_bytes;
	}

	/* copy calculated ECC for whole page to chip->buffer->oob */
	/* this include masked-value(0xFF) for unwritten subpages */
	ecc_calc = chip->ecc.calc_buf;
	ret = mtd_ooblayout_set_eccbytes(mtd, ecc_calc, chip->oob_poi, 0,
					 chip->ecc.total);
	if (ret)
		return ret;

	/* write OOB buffer to NAND device */
	ret = nand_write_data_op(chip, chip->oob_poi, mtd->oobsize, false);
	if (ret)
		return ret;

	return nand_prog_page_end_op(chip);
}


/**
 * nand_write_page_syndrome - [REPLACEABLE] hardware ECC syndrome based page write
 * @chip: nand chip info structure
 * @buf: data buffer
 * @oob_required: must write chip->oob_poi to OOB
 * @page: page number to write
 *
 * The hw generator calculates the error syndrome automatically. Therefore we
 * need a special oob layout and handling.
 */
static int nand_write_page_syndrome(struct nand_chip *chip, const uint8_t *buf,
				    int oob_required, int page, int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int i, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	const uint8_t *p = buf;
	uint8_t *oob = chip->oob_poi;
	int ret;

	ret = nand_prog_page_begin_op(chip, page, 0, NULL, 0);
	if (ret)
		return ret;

	BUG_ON(encrypted);

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->ecc.hwctl(chip, NAND_ECC_WRITE);

		ret = nand_write_data_op(chip, p, eccsize, false);
		if (ret)
			return ret;

		if (chip->ecc.prepad) {
			ret = nand_write_data_op(chip, oob, chip->ecc.prepad,
						 false);
			if (ret)
				return ret;

			oob += chip->ecc.prepad;
		}

		chip->ecc.calculate(chip, p, oob);

		ret = nand_write_data_op(chip, oob, eccbytes, false);
		if (ret)
			return ret;

		oob += eccbytes;

		if (chip->ecc.postpad) {
			ret = nand_write_data_op(chip, oob, chip->ecc.postpad,
						 false);
			if (ret)
				return ret;

			oob += chip->ecc.postpad;
		}
	}

	/* Calculate remaining oob bytes */
	i = mtd->oobsize - (oob - chip->oob_poi);
	if (i) {
		ret = nand_write_data_op(chip, oob, i, false);
		if (ret)
			return ret;
	}

	return nand_prog_page_end_op(chip);
}

/**
 * nand_write_page - write one page
 * @chip: NAND chip descriptor
 * @offset: address offset within the page
 * @data_len: length of actual data to be written
 * @buf: the data to write
 * @oob_required: must write chip->oob_poi to OOB
 * @page: page number to write
 * @raw: use _raw version of write_page
 */
static int nand_write_page(struct nand_chip *chip, uint32_t offset,
			   int data_len, const uint8_t *buf, int oob_required,
			   int page, int raw,  int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int status, subpage;

	if (!(chip->options & NAND_NO_SUBPAGE_WRITE) &&
		chip->ecc.write_subpage)
		subpage = offset || (data_len < mtd->writesize);
	else
		subpage = 0;

	if (unlikely(raw)) {
		BUG_ON(encrypted);
		status = chip->ecc.write_page_raw(chip, buf, oob_required,
						  page, encrypted);

	}
	else if (subpage) {
		BUG_ON(encrypted);
		status = chip->ecc.write_subpage(chip, offset, data_len, buf,
						 oob_required, page);
	} else
		status = chip->ecc.write_page(chip, buf, oob_required, page, encrypted);

	if (status < 0)
		return status;

	return 0;
}

#define NOTALIGNED(x)	((x & (chip->subpagesize - 1)) != 0)

/**
 * nand_do_write_ops - [INTERN] NAND write with ECC
 * @chip: NAND chip object
 * @to: offset to write to
 * @ops: oob operations description structure
 *
 * NAND write with ECC.
 */
static int nand_do_write_ops(struct nand_chip *chip, loff_t to,
			     struct mtd_oob_ops *ops, int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	int chipnr, realpage, page, column;
	uint32_t writelen = ops->len;

	uint32_t oobwritelen = ops->ooblen;
	uint32_t oobmaxlen = mtd_oobavail(mtd, ops);

	uint8_t *oob = ops->oobbuf;
	uint8_t *buf = ops->datbuf;
	int ret;
	int oob_required = oob ? 1 : 0;

	ops->retlen = 0;
	if (!writelen)
		return 0;

	/* Reject writes, which are not page aligned */
	if (NOTALIGNED(to) || NOTALIGNED(ops->len)) {
		pr_notice("%s: attempt to write non page aligned data\n",
			   __func__);
		return -EINVAL;
	}

	column = to & (mtd->writesize - 1);

	chipnr = (int)(to >> chip->chip_shift);
	nand_select_target(chip, chipnr);

	/* Check, if it is write protected */
	if (nand_check_wp(chip)) {
		ret = -EIO;
		goto err_out;
	}

	realpage = (int)(to >> chip->page_shift);
	page = realpage & chip->pagemask;

	/* Invalidate the page cache, when we write to the cached page */
	if (to <= ((loff_t)chip->pagecache.page << chip->page_shift) &&
	    ((loff_t)chip->pagecache.page << chip->page_shift) < (to + ops->len))
		chip->pagecache.page = -1;

	/* Don't allow multipage oob writes with offset */
	if (oob && ops->ooboffs && (ops->ooboffs + ops->ooblen > oobmaxlen)) {
		ret = -EINVAL;
		goto err_out;
	}

	while (1) {
		int bytes = mtd->writesize;
		uint8_t *wbuf = buf;
		int use_bufpoi;
		int part_pagewr = (column || writelen < mtd->writesize);

		if (part_pagewr)
			use_bufpoi = 1;
		else if (chip->options & NAND_USE_BOUNCE_BUFFER)
			use_bufpoi = !virt_addr_valid(buf) ||
				     !IS_ALIGNED((unsigned long)buf,
						 chip->buf_align);
		else
			use_bufpoi = 0;

		/* Partial page write?, or need to use bounce buffer */
		if (use_bufpoi) {
			pr_debug("%s: using write bounce buffer for buf@%p\n",
					 __func__, buf);
			if (part_pagewr)
				bytes = min_t(int, bytes - column, writelen);
			wbuf = nand_get_data_buf(chip);
			memset(wbuf, 0xff, mtd->writesize);
			memcpy(&wbuf[column], buf, bytes);
		}

		if (unlikely(oob)) {
			size_t len = min(oobwritelen, oobmaxlen);
			oob = nand_fill_oob(chip, oob, len, ops);
			oobwritelen -= len;
		} else {
			/* We still need to erase leftover OOB data */
			memset(chip->oob_poi, 0xff, mtd->oobsize);
		}

		ret = nand_write_page(chip, column, bytes, wbuf,
				      oob_required, page,
				      (ops->mode == MTD_OPS_RAW), encrypted);
		if (ret)
			break;

		writelen -= bytes;
		if (!writelen)
			break;

		column = 0;
		buf += bytes;
		realpage++;

		page = realpage & chip->pagemask;
		/* Check, if we cross a chip boundary */
		if (!page) {
			chipnr++;
			nand_deselect_target(chip);
			nand_select_target(chip, chipnr);
		}
	}

	ops->retlen = ops->len - writelen;
	if (unlikely(oob))
		ops->oobretlen = ops->ooblen;

err_out:
	nand_deselect_target(chip);
	return ret;
}

/**
 * panic_nand_write - [MTD Interface] NAND write with ECC
 * @mtd: MTD device structure
 * @to: offset to write to
 * @len: number of bytes to write
 * @retlen: pointer to variable to store the number of written bytes
 * @buf: the data to write
 *
 * NAND write with ECC. Used when performing writes in interrupt context, this
 * may for example be called by mtdoops when writing an oops while in panic.
 */
static int panic_nand_write(struct mtd_info *mtd, loff_t to, size_t len,
			    size_t *retlen, const uint8_t *buf, int encrypted)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	int chipnr = (int)(to >> chip->chip_shift);
	struct mtd_oob_ops ops;
	int ret;

	nand_select_target(chip, chipnr);

	/* Wait for the device to get ready */
	panic_nand_wait(chip, 400);

	memset(&ops, 0, sizeof(ops));
	ops.len = len;
	ops.datbuf = (uint8_t *)buf;
	ops.mode = MTD_OPS_PLACE_OOB;

	ret = nand_do_write_ops(chip, to, &ops, encrypted);

	*retlen = ops.retlen;
	return ret;
}

/**
 * nand_write_oob - [MTD Interface] NAND write data and/or out-of-band
 * @mtd: MTD device structure
 * @to: offset to write to
 * @ops: oob operation description structure
 */
static int nand_write_oob(struct mtd_info *mtd, loff_t to,
			  struct mtd_oob_ops *ops, int encrypted)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	int ret;

	ops->retlen = 0;

	ret = nand_get_device(chip);
	if (ret)
		return ret;

	switch (ops->mode) {
	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_AUTO_OOB:
	case MTD_OPS_RAW:
		break;

	default:
		goto out;
	}

	if (!ops->datbuf)
		ret = nand_do_write_oob(chip, to, ops);
	else
		ret = nand_do_write_ops(chip, to, ops, 0);

out:
	nand_release_device(chip);
	return ret;
}

/**
 * nand_erase - [MTD Interface] erase block(s)
 * @mtd: MTD device structure
 * @instr: erase instruction
 *
 * Erase one ore more blocks.
 */
static int nand_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	return nand_erase_nand(mtd_to_nand(mtd), instr, 0);
}

/**
 * nand_erase_nand - [INTERN] erase block(s)
 * @chip: NAND chip object
 * @instr: erase instruction
 * @allowbbt: allow erasing the bbt area
 *
 * Erase one ore more blocks.
 */
int nand_erase_nand(struct nand_chip *chip, struct erase_info *instr,
		    int allowbbt)
{
	int page, pages_per_block, ret, chipnr;
	loff_t len;

	pr_debug("%s: start = 0x%012llx, len = %llu\n",
			__func__, (unsigned long long)instr->addr,
			(unsigned long long)instr->len);

	if (check_offs_len(chip, instr->addr, instr->len))
		return -EINVAL;

	/* Grab the lock and see if the device is available */
	ret = nand_get_device(chip);
	if (ret)
		return ret;

	/* Shift to get first page */
	page = (int)(instr->addr >> chip->page_shift);
	chipnr = (int)(instr->addr >> chip->chip_shift);

	/* Calculate pages in each block */
	pages_per_block = 1 << (chip->phys_erase_shift - chip->page_shift);

	/* Select the NAND device */
	nand_select_target(chip, chipnr);

	/* Check, if it is write protected */
	if (nand_check_wp(chip)) {
		pr_debug("%s: device is write protected!\n",
				__func__);
		ret = -EIO;
		goto erase_exit;
	}

	/* Loop through the pages */
	len = instr->len;

	while (len) {
		/* Check if we have a bad block, we do not erase bad blocks! */
		if (nand_block_checkbad(chip, ((loff_t) page) <<
					chip->page_shift, allowbbt)) {
			pr_warn("%s: attempt to erase a bad block at page 0x%08x\n",
				    __func__, page);
			ret = -EIO;
			goto erase_exit;
		}

		/*
		 * Invalidate the page cache, if we erase the block which
		 * contains the current cached page.
		 */
		if (page <= chip->pagecache.page && chip->pagecache.page <
		    (page + pages_per_block))
			chip->pagecache.page = -1;

		ret = nand_erase_op(chip, (page & chip->pagemask) >>
				    (chip->phys_erase_shift - chip->page_shift));
		if (ret) {
			pr_debug("%s: failed erase, page 0x%08x\n",
					__func__, page);
			instr->fail_addr =
				((loff_t)page << chip->page_shift);
			goto erase_exit;
		}

		/* Increment page address and decrement length */
		len -= (1ULL << chip->phys_erase_shift);
		page += pages_per_block;

		/* Check, if we cross a chip boundary */
		if (len && !(page & chip->pagemask)) {
			chipnr++;
			nand_deselect_target(chip);
			nand_select_target(chip, chipnr);
		}
	}

	ret = 0;
erase_exit:

	/* Deselect and wake up anyone waiting on the device */
	nand_deselect_target(chip);
	nand_release_device(chip);

	/* Return more or less happy */
	return ret;
}

/**
 * nand_sync - [MTD Interface] sync
 * @mtd: MTD device structure
 *
 * Sync is actually a wait for chip ready function.
 */
static void nand_sync(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	pr_debug("%s: called\n", __func__);

	/* Grab the lock and see if the device is available */
	WARN_ON(nand_get_device(chip));
	/* Release it and go back */
	nand_release_device(chip);
}

/**
 * nand_block_isbad - [MTD Interface] Check if block at offset is bad
 * @mtd: MTD device structure
 * @offs: offset relative to mtd start
 */
static int nand_block_isbad(struct mtd_info *mtd, loff_t offs)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	int chipnr = (int)(offs >> chip->chip_shift);
	int ret;

	/* Select the NAND device */
	ret = nand_get_device(chip);
	if (ret)
		return ret;

	nand_select_target(chip, chipnr);

	ret = nand_block_checkbad(chip, offs, 0);

	nand_deselect_target(chip);
	nand_release_device(chip);

	return ret;
}

/**
 * nand_block_markbad - [MTD Interface] Mark block at the given offset as bad
 * @mtd: MTD device structure
 * @ofs: offset relative to mtd start
 */
static int nand_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	int ret;

	ret = nand_block_isbad(mtd, ofs);
	if (ret) {
		/* If it was bad already, return success and do nothing */
		if (ret > 0)
			return 0;
		return ret;
	}

	return nand_block_markbad_lowlevel(mtd_to_nand(mtd), ofs);
}

/**
 * nand_suspend - [MTD Interface] Suspend the NAND flash
 * @mtd: MTD device structure
 */
static int nand_suspend(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	mutex_lock(&chip->lock);
	chip->suspended = 1;
	mutex_unlock(&chip->lock);

	return 0;
}

/**
 * nand_resume - [MTD Interface] Resume the NAND flash
 * @mtd: MTD device structure
 */
static void nand_resume(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	mutex_lock(&chip->lock);
	if (chip->suspended)
		chip->suspended = 0;
	else
		pr_err("%s called for a chip which is not in suspended state\n",
			__func__);
	mutex_unlock(&chip->lock);
}

/**
 * nand_shutdown - [MTD Interface] Finish the current NAND operation and
 *                 prevent further operations
 * @mtd: MTD device structure
 */
static void nand_shutdown(struct mtd_info *mtd)
{
	nand_suspend(mtd);
}

/* Set default functions */
static void nand_set_defaults(struct nand_chip *chip)
{
	/* If no controller is provided, use the dummy, legacy one. */
	if (!chip->controller) {
		chip->controller = &chip->legacy.dummy_controller;
		nand_controller_init(chip->controller);
	}

	nand_legacy_set_defaults(chip);

	if (!chip->buf_align)
		chip->buf_align = 1;
}

/* Sanitize ONFI strings so we can safely print them */
void sanitize_string(uint8_t *s, size_t len)
{
	ssize_t i;

	/* Null terminate */
	s[len - 1] = 0;

	/* Remove non printable chars */
	for (i = 0; i < len - 1; i++) {
		if (s[i] < ' ' || s[i] > 127)
			s[i] = '?';
	}

	/* Remove trailing spaces */
	strim(s);
}

/*
 * nand_id_has_period - Check if an ID string has a given wraparound period
 * @id_data: the ID string
 * @arrlen: the length of the @id_data array
 * @period: the period of repitition
 *
 * Check if an ID string is repeated within a given sequence of bytes at
 * specific repetition interval period (e.g., {0x20,0x01,0x7F,0x20} has a
 * period of 3). This is a helper function for nand_id_len(). Returns non-zero
 * if the repetition has a period of @period; otherwise, returns zero.
 */
static int nand_id_has_period(u8 *id_data, int arrlen, int period)
{
	int i, j;
	for (i = 0; i < period; i++)
		for (j = i + period; j < arrlen; j += period)
			if (id_data[i] != id_data[j])
				return 0;
	return 1;
}

/*
 * nand_id_len - Get the length of an ID string returned by CMD_READID
 * @id_data: the ID string
 * @arrlen: the length of the @id_data array

 * Returns the length of the ID string, according to known wraparound/trailing
 * zero patterns. If no pattern exists, returns the length of the array.
 */
static int nand_id_len(u8 *id_data, int arrlen)
{
	int last_nonzero, period;

	/* Find last non-zero byte */
	for (last_nonzero = arrlen - 1; last_nonzero >= 0; last_nonzero--)
		if (id_data[last_nonzero])
			break;

	/* All zeros */
	if (last_nonzero < 0)
		return 0;

	/* Calculate wraparound period */
	for (period = 1; period < arrlen; period++)
		if (nand_id_has_period(id_data, arrlen, period))
			break;

	/* There's a repeated pattern */
	if (period < arrlen)
		return period;

	/* There are trailing zeros */
	if (last_nonzero < arrlen - 1)
		return last_nonzero + 1;

	/* No pattern detected */
	return arrlen;
}

/* Extract the bits of per cell from the 3rd byte of the extended ID */
static int nand_get_bits_per_cell(u8 cellinfo)
{
	int bits;

	bits = cellinfo & NAND_CI_CELLTYPE_MSK;
	bits >>= NAND_CI_CELLTYPE_SHIFT;
	return bits + 1;
}

/*
 * Many new NAND share similar device ID codes, which represent the size of the
 * chip. The rest of the parameters must be decoded according to generic or
 * manufacturer-specific "extended ID" decoding patterns.
 */
void nand_decode_ext_id(struct nand_chip *chip)
{
	struct nand_memory_organization *memorg;
	struct mtd_info *mtd = nand_to_mtd(chip);
	int extid;
	u8 *id_data = chip->id.data;

	memorg = nanddev_get_memorg(&chip->base);

	/* The 3rd id byte holds MLC / multichip data */
	memorg->bits_per_cell = nand_get_bits_per_cell(id_data[2]);
	/* The 4th id byte is the important one */
	extid = id_data[3];

	/* Calc pagesize */
	memorg->pagesize = 1024 << (extid & 0x03);
	mtd->writesize = memorg->pagesize;
	extid >>= 2;
	/* Calc oobsize */
	memorg->oobsize = (8 << (extid & 0x01)) * (mtd->writesize >> 9);
	mtd->oobsize = memorg->oobsize;
	extid >>= 2;
	/* Calc blocksize. Blocksize is multiples of 64KiB */
	memorg->pages_per_eraseblock = ((64 * 1024) << (extid & 0x03)) /
				       memorg->pagesize;
	mtd->erasesize = (64 * 1024) << (extid & 0x03);
	extid >>= 2;
	/* Get buswidth information */
	if (extid & 0x1)
		chip->options |= NAND_BUSWIDTH_16;
}
EXPORT_SYMBOL_GPL(nand_decode_ext_id);

/*
 * Old devices have chip data hardcoded in the device ID table. nand_decode_id
 * decodes a matching ID table entry and assigns the MTD size parameters for
 * the chip.
 */
static void nand_decode_id(struct nand_chip *chip, struct nand_flash_dev *type)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct nand_memory_organization *memorg;

	memorg = nanddev_get_memorg(&chip->base);

	memorg->pages_per_eraseblock = type->erasesize / type->pagesize;
	mtd->erasesize = type->erasesize;
	memorg->pagesize = type->pagesize;
	mtd->writesize = memorg->pagesize;
	memorg->oobsize = memorg->pagesize / 32;
	mtd->oobsize = memorg->oobsize;

	/* All legacy ID NAND are small-page, SLC */
	memorg->bits_per_cell = 1;
}

/*
 * Set the bad block marker/indicator (BBM/BBI) patterns according to some
 * heuristic patterns using various detected parameters (e.g., manufacturer,
 * page size, cell-type information).
 */
static void nand_decode_bbm_options(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	/* Set the bad block position */
	if (mtd->writesize > 512 || (chip->options & NAND_BUSWIDTH_16))
		chip->badblockpos = NAND_BBM_POS_LARGE;
	else
		chip->badblockpos = NAND_BBM_POS_SMALL;
}

static inline bool is_full_id_nand(struct nand_flash_dev *type)
{
	return type->id_len;
}

static bool find_full_id_nand(struct nand_chip *chip,
			      struct nand_flash_dev *type)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct nand_memory_organization *memorg;
	u8 *id_data = chip->id.data;

	memorg = nanddev_get_memorg(&chip->base);

	if (!strncmp(type->id, id_data, type->id_len)) {
		memorg->pagesize = type->pagesize;
		mtd->writesize = memorg->pagesize;
		memorg->pages_per_eraseblock = type->erasesize /
					       type->pagesize;
		mtd->erasesize = type->erasesize;
		memorg->oobsize = type->oobsize;
		mtd->oobsize = memorg->oobsize;

		memorg->bits_per_cell = nand_get_bits_per_cell(id_data[2]);
		memorg->eraseblocks_per_lun =
			DIV_ROUND_DOWN_ULL((u64)type->chipsize << 20,
					   memorg->pagesize *
					   memorg->pages_per_eraseblock);
		chip->options |= type->options;
		chip->base.eccreq.strength = NAND_ECC_STRENGTH(type);
		chip->base.eccreq.step_size = NAND_ECC_STEP(type);
		chip->onfi_timing_mode_default =
					type->onfi_timing_mode_default;

		chip->parameters.model = kstrdup(type->name, GFP_KERNEL);
		if (!chip->parameters.model)
			return false;

		return true;
	}
	return false;
}

/*
 * Manufacturer detection. Only used when the NAND is not ONFI or JEDEC
 * compliant and does not have a full-id or legacy-id entry in the nand_ids
 * table.
 */
static void nand_manufacturer_detect(struct nand_chip *chip)
{
	/*
	 * Try manufacturer detection if available and use
	 * nand_decode_ext_id() otherwise.
	 */
	if (chip->manufacturer.desc && chip->manufacturer.desc->ops &&
	    chip->manufacturer.desc->ops->detect) {
		struct nand_memory_organization *memorg;

		memorg = nanddev_get_memorg(&chip->base);

		/* The 3rd id byte holds MLC / multichip data */
		memorg->bits_per_cell = nand_get_bits_per_cell(chip->id.data[2]);
		chip->manufacturer.desc->ops->detect(chip);
	} else {
		nand_decode_ext_id(chip);
	}
}

/*
 * Manufacturer initialization. This function is called for all NANDs including
 * ONFI and JEDEC compliant ones.
 * Manufacturer drivers should put all their specific initialization code in
 * their ->init() hook.
 */
static int nand_manufacturer_init(struct nand_chip *chip)
{
	if (!chip->manufacturer.desc || !chip->manufacturer.desc->ops ||
	    !chip->manufacturer.desc->ops->init)
		return 0;

	return chip->manufacturer.desc->ops->init(chip);
}

/*
 * Manufacturer cleanup. This function is called for all NANDs including
 * ONFI and JEDEC compliant ones.
 * Manufacturer drivers should put all their specific cleanup code in their
 * ->cleanup() hook.
 */
static void nand_manufacturer_cleanup(struct nand_chip *chip)
{
	/* Release manufacturer private data */
	if (chip->manufacturer.desc && chip->manufacturer.desc->ops &&
	    chip->manufacturer.desc->ops->cleanup)
		chip->manufacturer.desc->ops->cleanup(chip);
}

static const char *
nand_manufacturer_name(const struct nand_manufacturer *manufacturer)
{
	return manufacturer ? manufacturer->name : "Unknown";
}

/*
 * Get the flash and manufacturer id and lookup if the type is supported.
 */
static int nand_detect(struct nand_chip *chip, struct nand_flash_dev *type)
{
	const struct nand_manufacturer *manufacturer;
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct nand_memory_organization *memorg;
	int busw, ret;
	u8 *id_data = chip->id.data;
	u8 maf_id, dev_id;
	u64 targetsize;

	/*
	 * Let's start by initializing memorg fields that might be left
	 * unassigned by the ID-based detection logic.
	 */
	memorg = nanddev_get_memorg(&chip->base);
	memorg->planes_per_lun = 1;
	memorg->luns_per_target = 1;

	/*
	 * Reset the chip, required by some chips (e.g. Micron MT29FxGxxxxx)
	 * after power-up.
	 */
	ret = nand_reset(chip, 0);
	if (ret)
		return ret;

	/* Select the device */
	nand_select_target(chip, 0);

	/* Send the command for reading device ID */
	ret = nand_readid_op(chip, 0, id_data, 2);
	if (ret)
		return ret;

	/* Read manufacturer and device IDs */
	maf_id = id_data[0];
	dev_id = id_data[1];

	/*
	 * Try again to make sure, as some systems the bus-hold or other
	 * interface concerns can cause random data which looks like a
	 * possibly credible NAND flash to appear. If the two results do
	 * not match, ignore the device completely.
	 */

	/* Read entire ID string */
	ret = nand_readid_op(chip, 0, id_data, sizeof(chip->id.data));
	if (ret)
		return ret;

	if (id_data[0] != maf_id || id_data[1] != dev_id) {
		pr_info("second ID read did not match %02x,%02x against %02x,%02x\n",
			maf_id, dev_id, id_data[0], id_data[1]);
		return -ENODEV;
	}

	chip->id.len = nand_id_len(id_data, ARRAY_SIZE(chip->id.data));

	/* Try to identify manufacturer */
	manufacturer = nand_get_manufacturer(maf_id);
	chip->manufacturer.desc = manufacturer;

	if (!type)
		type = nand_flash_ids;

	/*
	 * Save the NAND_BUSWIDTH_16 flag before letting auto-detection logic
	 * override it.
	 * This is required to make sure initial NAND bus width set by the
	 * NAND controller driver is coherent with the real NAND bus width
	 * (extracted by auto-detection code).
	 */
	busw = chip->options & NAND_BUSWIDTH_16;

	/*
	 * The flag is only set (never cleared), reset it to its default value
	 * before starting auto-detection.
	 */
	chip->options &= ~NAND_BUSWIDTH_16;

	for (; type->name != NULL; type++) {
		if (is_full_id_nand(type)) {
			if (find_full_id_nand(chip, type))
				goto ident_done;
		} else if (dev_id == type->dev_id) {
			break;
		}
	}

	if (!type->name || !type->pagesize) {
		/* Check if the chip is ONFI compliant */
		ret = nand_onfi_detect(chip);
		if (ret < 0)
			return ret;
		else if (ret)
			goto ident_done;

		/* Check if the chip is JEDEC compliant */
		ret = nand_jedec_detect(chip);
		if (ret < 0)
			return ret;
		else if (ret)
			goto ident_done;
	}

	if (!type->name)
		return -ENODEV;

	chip->parameters.model = kstrdup(type->name, GFP_KERNEL);
	if (!chip->parameters.model)
		return -ENOMEM;

	if (!type->pagesize)
		nand_manufacturer_detect(chip);
	else
		nand_decode_id(chip, type);

	/* Get chip options */
	chip->options |= type->options;

	memorg->eraseblocks_per_lun =
			DIV_ROUND_DOWN_ULL((u64)type->chipsize << 20,
					   memorg->pagesize *
					   memorg->pages_per_eraseblock);

ident_done:
	if (!mtd->name)
		mtd->name = chip->parameters.model;

	if (chip->options & NAND_BUSWIDTH_AUTO) {
		WARN_ON(busw & NAND_BUSWIDTH_16);
		nand_set_defaults(chip);
	} else if (busw != (chip->options & NAND_BUSWIDTH_16)) {
		/*
		 * Check, if buswidth is correct. Hardware drivers should set
		 * chip correct!
		 */
		pr_info("device found, Manufacturer ID: 0x%02x, Chip ID: 0x%02x\n",
			maf_id, dev_id);
		pr_info("%s %s\n", nand_manufacturer_name(manufacturer),
			mtd->name);
		pr_warn("bus width %d instead of %d bits\n", busw ? 16 : 8,
			(chip->options & NAND_BUSWIDTH_16) ? 16 : 8);
		ret = -EINVAL;

		goto free_detect_allocation;
	}

	nand_decode_bbm_options(chip);

	/* Calculate the address shift from the page size */
	chip->page_shift = ffs(mtd->writesize) - 1;
	/* Convert chipsize to number of pages per chip -1 */
	targetsize = nanddev_target_size(&chip->base);
	chip->pagemask = (targetsize >> chip->page_shift) - 1;

	chip->bbt_erase_shift = chip->phys_erase_shift =
		ffs(mtd->erasesize) - 1;
	if (targetsize & 0xffffffff)
		chip->chip_shift = ffs((unsigned)targetsize) - 1;
	else {
		chip->chip_shift = ffs((unsigned)(targetsize >> 32));
		chip->chip_shift += 32 - 1;
	}

	if (chip->chip_shift - chip->page_shift > 16)
		chip->options |= NAND_ROW_ADDR_3;

	chip->badblockbits = 8;

	nand_legacy_adjust_cmdfunc(chip);

	pr_info("device found, Manufacturer ID: 0x%02x, Chip ID: 0x%02x\n",
		maf_id, dev_id);
	pr_info("%s %s\n", nand_manufacturer_name(manufacturer),
		chip->parameters.model);
	pr_info("%d MiB, %s, erase size: %d KiB, page size: %d, OOB size: %d\n",
		(int)(targetsize >> 20), nand_is_slc(chip) ? "SLC" : "MLC",
		mtd->erasesize >> 10, mtd->writesize, mtd->oobsize);
	return 0;

free_detect_allocation:
	kfree(chip->parameters.model);

	return ret;
}

static const char * const nand_ecc_modes[] = {
	[NAND_ECC_NONE]		= "none",
	[NAND_ECC_SOFT]		= "soft",
	[NAND_ECC_HW]		= "hw",
	[NAND_ECC_HW_SYNDROME]	= "hw_syndrome",
	[NAND_ECC_HW_OOB_FIRST]	= "hw_oob_first",
	[NAND_ECC_ON_DIE]	= "on-die",
};

static int of_get_nand_ecc_mode(struct device_node *np)
{
	const char *pm;
	int err, i;

	err = of_property_read_string(np, "nand-ecc-mode", &pm);
	if (err < 0)
		return err;

	for (i = 0; i < ARRAY_SIZE(nand_ecc_modes); i++)
		if (!strcasecmp(pm, nand_ecc_modes[i]))
			return i;

	/*
	 * For backward compatibility we support few obsoleted values that don't
	 * have their mappings into nand_ecc_modes_t anymore (they were merged
	 * with other enums).
	 */
	if (!strcasecmp(pm, "soft_bch"))
		return NAND_ECC_SOFT;

	return -ENODEV;
}

static const char * const nand_ecc_algos[] = {
	[NAND_ECC_HAMMING]	= "hamming",
	[NAND_ECC_BCH]		= "bch",
	[NAND_ECC_RS]		= "rs",
};

static int of_get_nand_ecc_algo(struct device_node *np)
{
	const char *pm;
	int err, i;

	err = of_property_read_string(np, "nand-ecc-algo", &pm);
	if (!err) {
		for (i = NAND_ECC_HAMMING; i < ARRAY_SIZE(nand_ecc_algos); i++)
			if (!strcasecmp(pm, nand_ecc_algos[i]))
				return i;
		return -ENODEV;
	}

	/*
	 * For backward compatibility we also read "nand-ecc-mode" checking
	 * for some obsoleted values that were specifying ECC algorithm.
	 */
	err = of_property_read_string(np, "nand-ecc-mode", &pm);
	if (err < 0)
		return err;

	if (!strcasecmp(pm, "soft"))
		return NAND_ECC_HAMMING;
	else if (!strcasecmp(pm, "soft_bch"))
		return NAND_ECC_BCH;

	return -ENODEV;
}

static int of_get_nand_ecc_step_size(struct device_node *np)
{
	int ret;
	u32 val;

	ret = of_property_read_u32(np, "nand-ecc-step-size", &val);
	return ret ? ret : val;
}

static int of_get_nand_ecc_strength(struct device_node *np)
{
	int ret;
	u32 val;

	ret = of_property_read_u32(np, "nand-ecc-strength", &val);
	return ret ? ret : val;
}

static int of_get_nand_bus_width(struct device_node *np)
{
	u32 val;

	if (of_property_read_u32(np, "nand-bus-width", &val))
		return 8;

	switch (val) {
	case 8:
	case 16:
		return val;
	default:
		return -EIO;
	}
}

static bool of_get_nand_on_flash_bbt(struct device_node *np)
{
	return of_property_read_bool(np, "nand-on-flash-bbt");
}

static int nand_dt_init(struct nand_chip *chip)
{
	struct device_node *dn = nand_get_flash_node(chip);
	int ecc_mode, ecc_algo, ecc_strength, ecc_step;

	if (!dn)
		return 0;

	if (of_get_nand_bus_width(dn) == 16)
		chip->options |= NAND_BUSWIDTH_16;

	if (of_property_read_bool(dn, "nand-is-boot-medium"))
		chip->options |= NAND_IS_BOOT_MEDIUM;

	if (of_get_nand_on_flash_bbt(dn))
		chip->bbt_options |= NAND_BBT_USE_FLASH;

	ecc_mode = of_get_nand_ecc_mode(dn);
	ecc_algo = of_get_nand_ecc_algo(dn);
	ecc_strength = of_get_nand_ecc_strength(dn);
	ecc_step = of_get_nand_ecc_step_size(dn);

	if (ecc_mode >= 0)
		chip->ecc.mode = ecc_mode;

	if (ecc_algo >= 0)
		chip->ecc.algo = ecc_algo;

	if (ecc_strength >= 0)
		chip->ecc.strength = ecc_strength;

	if (ecc_step > 0)
		chip->ecc.size = ecc_step;

	if (of_property_read_bool(dn, "nand-ecc-maximize"))
		chip->ecc.options |= NAND_ECC_MAXIMIZE;

	return 0;
}

/**
 * nand_scan_ident - Scan for the NAND device
 * @chip: NAND chip object
 * @maxchips: number of chips to scan for
 * @table: alternative NAND ID table
 *
 * This is the first phase of the normal nand_scan() function. It reads the
 * flash ID and sets up MTD fields accordingly.
 *
 * This helper used to be called directly from controller drivers that needed
 * to tweak some ECC-related parameters before nand_scan_tail(). This separation
 * prevented dynamic allocations during this phase which was unconvenient and
 * as been banned for the benefit of the ->init_ecc()/cleanup_ecc() hooks.
 */
static int nand_scan_ident(struct nand_chip *chip, unsigned int maxchips,
			   struct nand_flash_dev *table)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct nand_memory_organization *memorg;
	int nand_maf_id, nand_dev_id;
	unsigned int i;
	int ret;

	memorg = nanddev_get_memorg(&chip->base);

	/* Assume all dies are deselected when we enter nand_scan_ident(). */
	chip->cur_cs = -1;

	mutex_init(&chip->lock);

	/* Enforce the right timings for reset/detection */
	onfi_fill_data_interface(chip, NAND_SDR_IFACE, 0);

	ret = nand_dt_init(chip);
	if (ret)
		return ret;

	if (!mtd->name && mtd->dev.parent)
		mtd->name = dev_name(mtd->dev.parent);

	/* Set the default functions */
	nand_set_defaults(chip);

	ret = nand_legacy_check_hooks(chip);
	if (ret)
		return ret;

	memorg->ntargets = maxchips;

	/* Read the flash type */
	ret = nand_detect(chip, table);
	if (ret) {
		if (!(chip->options & NAND_SCAN_SILENT_NODEV))
			pr_warn("No NAND device found\n");
		nand_deselect_target(chip);
		return ret;
	}

	nand_maf_id = chip->id.data[0];
	nand_dev_id = chip->id.data[1];

	nand_deselect_target(chip);

	/* Check for a chip array */
	for (i = 1; i < maxchips; i++) {
		u8 id[2];

		/* See comment in nand_get_flash_type for reset */
		ret = nand_reset(chip, i);
		if (ret)
			break;

		nand_select_target(chip, i);
		/* Send the command for reading device ID */
		ret = nand_readid_op(chip, 0, id, sizeof(id));
		if (ret)
			break;
		/* Read manufacturer and device IDs */
		if (nand_maf_id != id[0] || nand_dev_id != id[1]) {
			nand_deselect_target(chip);
			break;
		}
		nand_deselect_target(chip);
	}
	if (i > 1)
		pr_info("%d chips detected\n", i);

	/* Store the number of chips and calc total size for mtd */
	memorg->ntargets = i;
	mtd->size = i * nanddev_target_size(&chip->base);

	return 0;
}

static void nand_scan_ident_cleanup(struct nand_chip *chip)
{
	kfree(chip->parameters.model);
	kfree(chip->parameters.onfi);
}

static int nand_set_ecc_soft_ops(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct nand_ecc_ctrl *ecc = &chip->ecc;

	if (WARN_ON(ecc->mode != NAND_ECC_SOFT))
		return -EINVAL;

	switch (ecc->algo) {
	case NAND_ECC_HAMMING:
		ecc->calculate = nand_calculate_ecc;
		ecc->correct = nand_correct_data;
		ecc->read_page = nand_read_page_swecc;
		ecc->read_subpage = nand_read_subpage;
		ecc->write_page = nand_write_page_swecc;
		ecc->read_page_raw = nand_read_page_raw;
		ecc->write_page_raw = nand_write_page_raw;
		ecc->read_oob = nand_read_oob_std;
		ecc->write_oob = nand_write_oob_std;
		if (!ecc->size)
			ecc->size = 256;
		ecc->bytes = 3;
		ecc->strength = 1;

		if (IS_ENABLED(CONFIG_MTD_NAND_ECC_SW_HAMMING_SMC))
			ecc->options |= NAND_ECC_SOFT_HAMMING_SM_ORDER;

		return 0;
	case NAND_ECC_BCH:
		if (!mtd_nand_has_bch()) {
			WARN(1, "CONFIG_MTD_NAND_ECC_SW_BCH not enabled\n");
			return -EINVAL;
		}
		ecc->calculate = nand_bch_calculate_ecc;
		ecc->correct = nand_bch_correct_data;
		ecc->read_page = nand_read_page_swecc;
		ecc->read_subpage = nand_read_subpage;
		ecc->write_page = nand_write_page_swecc;
		ecc->read_page_raw = nand_read_page_raw;
		ecc->write_page_raw = nand_write_page_raw;
		ecc->read_oob = nand_read_oob_std;
		ecc->write_oob = nand_write_oob_std;

		/*
		* Board driver should supply ecc.size and ecc.strength
		* values to select how many bits are correctable.
		* Otherwise, default to 4 bits for large page devices.
		*/
		if (!ecc->size && (mtd->oobsize >= 64)) {
			ecc->size = 512;
			ecc->strength = 4;
		}

		/*
		 * if no ecc placement scheme was provided pickup the default
		 * large page one.
		 */
		if (!mtd->ooblayout) {
			/* handle large page devices only */
			if (mtd->oobsize < 64) {
				WARN(1, "OOB layout is required when using software BCH on small pages\n");
				return -EINVAL;
			}

			mtd_set_ooblayout(mtd, &nand_ooblayout_lp_ops);

		}

		/*
		 * We can only maximize ECC config when the default layout is
		 * used, otherwise we don't know how many bytes can really be
		 * used.
		 */
		if (mtd->ooblayout == &nand_ooblayout_lp_ops &&
		    ecc->options & NAND_ECC_MAXIMIZE) {
			int steps, bytes;

			/* Always prefer 1k blocks over 512bytes ones */
			ecc->size = 1024;
			steps = mtd->writesize / ecc->size;

			/* Reserve 2 bytes for the BBM */
			bytes = (mtd->oobsize - 2) / steps;
			ecc->strength = bytes * 8 / fls(8 * ecc->size);
		}

		/* See nand_bch_init() for details. */
		ecc->bytes = 0;
		ecc->priv = nand_bch_init(mtd);
		if (!ecc->priv) {
			WARN(1, "BCH ECC initialization failed!\n");
			return -EINVAL;
		}
		return 0;
	default:
		WARN(1, "Unsupported ECC algorithm!\n");
		return -EINVAL;
	}
}

/**
 * nand_check_ecc_caps - check the sanity of preset ECC settings
 * @chip: nand chip info structure
 * @caps: ECC caps info structure
 * @oobavail: OOB size that the ECC engine can use
 *
 * When ECC step size and strength are already set, check if they are supported
 * by the controller and the calculated ECC bytes fit within the chip's OOB.
 * On success, the calculated ECC bytes is set.
 */
static int
nand_check_ecc_caps(struct nand_chip *chip,
		    const struct nand_ecc_caps *caps, int oobavail)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	const struct nand_ecc_step_info *stepinfo;
	int preset_step = chip->ecc.size;
	int preset_strength = chip->ecc.strength;
	int ecc_bytes, nsteps = mtd->writesize / preset_step;
	int i, j;

	for (i = 0; i < caps->nstepinfos; i++) {
		stepinfo = &caps->stepinfos[i];

		if (stepinfo->stepsize != preset_step)
			continue;

		for (j = 0; j < stepinfo->nstrengths; j++) {
			if (stepinfo->strengths[j] != preset_strength)
				continue;

			ecc_bytes = caps->calc_ecc_bytes(preset_step,
							 preset_strength);
			if (WARN_ON_ONCE(ecc_bytes < 0))
				return ecc_bytes;

			if (ecc_bytes * nsteps > oobavail) {
				pr_err("ECC (step, strength) = (%d, %d) does not fit in OOB",
				       preset_step, preset_strength);
				return -ENOSPC;
			}

			chip->ecc.bytes = ecc_bytes;

			return 0;
		}
	}

	pr_err("ECC (step, strength) = (%d, %d) not supported on this controller",
	       preset_step, preset_strength);

	return -ENOTSUPP;
}

/**
 * nand_match_ecc_req - meet the chip's requirement with least ECC bytes
 * @chip: nand chip info structure
 * @caps: ECC engine caps info structure
 * @oobavail: OOB size that the ECC engine can use
 *
 * If a chip's ECC requirement is provided, try to meet it with the least
 * number of ECC bytes (i.e. with the largest number of OOB-free bytes).
 * On success, the chosen ECC settings are set.
 */
static int
nand_match_ecc_req(struct nand_chip *chip,
		   const struct nand_ecc_caps *caps, int oobavail)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	const struct nand_ecc_step_info *stepinfo;
	int req_step = chip->base.eccreq.step_size;
	int req_strength = chip->base.eccreq.strength;
	int req_corr, step_size, strength, nsteps, ecc_bytes, ecc_bytes_total;
	int best_step, best_strength, best_ecc_bytes;
	int best_ecc_bytes_total = INT_MAX;
	int i, j;

	/* No information provided by the NAND chip */
	if (!req_step || !req_strength)
		return -ENOTSUPP;

	/* number of correctable bits the chip requires in a page */
	req_corr = mtd->writesize / req_step * req_strength;

	for (i = 0; i < caps->nstepinfos; i++) {
		stepinfo = &caps->stepinfos[i];
		step_size = stepinfo->stepsize;

		for (j = 0; j < stepinfo->nstrengths; j++) {
			strength = stepinfo->strengths[j];

			/*
			 * If both step size and strength are smaller than the
			 * chip's requirement, it is not easy to compare the
			 * resulted reliability.
			 */
			if (step_size < req_step && strength < req_strength)
				continue;

			if (mtd->writesize % step_size)
				continue;

			nsteps = mtd->writesize / step_size;

			ecc_bytes = caps->calc_ecc_bytes(step_size, strength);
			if (WARN_ON_ONCE(ecc_bytes < 0))
				continue;
			ecc_bytes_total = ecc_bytes * nsteps;

			if (ecc_bytes_total > oobavail ||
			    strength * nsteps < req_corr)
				continue;

			/*
			 * We assume the best is to meet the chip's requrement
			 * with the least number of ECC bytes.
			 */
			if (ecc_bytes_total < best_ecc_bytes_total) {
				best_ecc_bytes_total = ecc_bytes_total;
				best_step = step_size;
				best_strength = strength;
				best_ecc_bytes = ecc_bytes;
			}
		}
	}

	if (best_ecc_bytes_total == INT_MAX)
		return -ENOTSUPP;

	chip->ecc.size = best_step;
	chip->ecc.strength = best_strength;
	chip->ecc.bytes = best_ecc_bytes;

	return 0;
}

/**
 * nand_maximize_ecc - choose the max ECC strength available
 * @chip: nand chip info structure
 * @caps: ECC engine caps info structure
 * @oobavail: OOB size that the ECC engine can use
 *
 * Choose the max ECC strength that is supported on the controller, and can fit
 * within the chip's OOB.  On success, the chosen ECC settings are set.
 */
static int
nand_maximize_ecc(struct nand_chip *chip,
		  const struct nand_ecc_caps *caps, int oobavail)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	const struct nand_ecc_step_info *stepinfo;
	int step_size, strength, nsteps, ecc_bytes, corr;
	int best_corr = 0;
	int best_step = 0;
	int best_strength, best_ecc_bytes;
	int i, j;

	for (i = 0; i < caps->nstepinfos; i++) {
		stepinfo = &caps->stepinfos[i];
		step_size = stepinfo->stepsize;

		/* If chip->ecc.size is already set, respect it */
		if (chip->ecc.size && step_size != chip->ecc.size)
			continue;

		for (j = 0; j < stepinfo->nstrengths; j++) {
			strength = stepinfo->strengths[j];

			if (mtd->writesize % step_size)
				continue;

			nsteps = mtd->writesize / step_size;

			ecc_bytes = caps->calc_ecc_bytes(step_size, strength);
			if (WARN_ON_ONCE(ecc_bytes < 0))
				continue;

			if (ecc_bytes * nsteps > oobavail)
				continue;

			corr = strength * nsteps;

			/*
			 * If the number of correctable bits is the same,
			 * bigger step_size has more reliability.
			 */
			if (corr > best_corr ||
			    (corr == best_corr && step_size > best_step)) {
				best_corr = corr;
				best_step = step_size;
				best_strength = strength;
				best_ecc_bytes = ecc_bytes;
			}
		}
	}

	if (!best_corr)
		return -ENOTSUPP;

	chip->ecc.size = best_step;
	chip->ecc.strength = best_strength;
	chip->ecc.bytes = best_ecc_bytes;

	return 0;
}

/**
 * nand_ecc_choose_conf - Set the ECC strength and ECC step size
 * @chip: nand chip info structure
 * @caps: ECC engine caps info structure
 * @oobavail: OOB size that the ECC engine can use
 *
 * Choose the ECC configuration according to following logic
 *
 * 1. If both ECC step size and ECC strength are already set (usually by DT)
 *    then check if it is supported by this controller.
 * 2. If NAND_ECC_MAXIMIZE is set, then select maximum ECC strength.
 * 3. Otherwise, try to match the ECC step size and ECC strength closest
 *    to the chip's requirement. If available OOB size can't fit the chip
 *    requirement then fallback to the maximum ECC step size and ECC strength.
 *
 * On success, the chosen ECC settings are set.
 */
int nand_ecc_choose_conf(struct nand_chip *chip,
			 const struct nand_ecc_caps *caps, int oobavail)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	if (WARN_ON(oobavail < 0 || oobavail > mtd->oobsize))
		return -EINVAL;

	if (chip->ecc.size && chip->ecc.strength)
		return nand_check_ecc_caps(chip, caps, oobavail);

	if (chip->ecc.options & NAND_ECC_MAXIMIZE)
		return nand_maximize_ecc(chip, caps, oobavail);

	if (!nand_match_ecc_req(chip, caps, oobavail))
		return 0;

	return nand_maximize_ecc(chip, caps, oobavail);
}
EXPORT_SYMBOL_GPL(nand_ecc_choose_conf);

/*
 * Check if the chip configuration meet the datasheet requirements.

 * If our configuration corrects A bits per B bytes and the minimum
 * required correction level is X bits per Y bytes, then we must ensure
 * both of the following are true:
 *
 * (1) A / B >= X / Y
 * (2) A >= X
 *
 * Requirement (1) ensures we can correct for the required bitflip density.
 * Requirement (2) ensures we can correct even when all bitflips are clumped
 * in the same sector.
 */
static bool nand_ecc_strength_good(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct nand_ecc_ctrl *ecc = &chip->ecc;
	int corr, ds_corr;

	if (ecc->size == 0 || chip->base.eccreq.step_size == 0)
		/* Not enough information */
		return true;

	/*
	 * We get the number of corrected bits per page to compare
	 * the correction density.
	 */
	corr = (mtd->writesize * ecc->strength) / ecc->size;
	ds_corr = (mtd->writesize * chip->base.eccreq.strength) /
		  chip->base.eccreq.step_size;

	return corr >= ds_corr && ecc->strength >= chip->base.eccreq.strength;
}

static int rawnand_erase(struct nand_device *nand, const struct nand_pos *pos)
{
	struct nand_chip *chip = container_of(nand, struct nand_chip,
					      base);
	unsigned int eb = nanddev_pos_to_row(nand, pos);
	int ret;

	eb >>= nand->rowconv.eraseblock_addr_shift;

	nand_select_target(chip, pos->target);
	ret = nand_erase_op(chip, eb);
	nand_deselect_target(chip);

	return ret;
}

static int rawnand_markbad(struct nand_device *nand,
			   const struct nand_pos *pos)
{
	struct nand_chip *chip = container_of(nand, struct nand_chip,
					      base);

	return nand_markbad_bbm(chip, nanddev_pos_to_offs(nand, pos));
}

static bool rawnand_isbad(struct nand_device *nand, const struct nand_pos *pos)
{
	struct nand_chip *chip = container_of(nand, struct nand_chip,
					      base);
	int ret;

	nand_select_target(chip, pos->target);
	ret = nand_isbad_bbm(chip, nanddev_pos_to_offs(nand, pos));
	nand_deselect_target(chip);

	return ret;
}

static const struct nand_ops rawnand_ops = {
	.erase = rawnand_erase,
	.markbad = rawnand_markbad,
	.isbad = rawnand_isbad,
};

/**
 * nand_scan_tail - Scan for the NAND device
 * @chip: NAND chip object
 *
 * This is the second phase of the normal nand_scan() function. It fills out
 * all the uninitialized function pointers with the defaults and scans for a
 * bad block table if appropriate.
 */
static int nand_scan_tail(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct nand_ecc_ctrl *ecc = &chip->ecc;
	int ret, i;

	/* New bad blocks should be marked in OOB, flash-based BBT, or both */
	if (WARN_ON((chip->bbt_options & NAND_BBT_NO_OOB_BBM) &&
		   !(chip->bbt_options & NAND_BBT_USE_FLASH))) {
		return -EINVAL;
	}

	chip->data_buf = kmalloc(mtd->writesize + mtd->oobsize, GFP_KERNEL);
	if (!chip->data_buf)
		return -ENOMEM;

	/*
	 * FIXME: some NAND manufacturer drivers expect the first die to be
	 * selected when manufacturer->init() is called. They should be fixed
	 * to explictly select the relevant die when interacting with the NAND
	 * chip.
	 */
	nand_select_target(chip, 0);
	ret = nand_manufacturer_init(chip);
	nand_deselect_target(chip);
	if (ret)
		goto err_free_buf;

	/* Set the internal oob buffer location, just after the page data */
	chip->oob_poi = chip->data_buf + mtd->writesize;

	/*
	 * If no default placement scheme is given, select an appropriate one.
	 */
	if (!mtd->ooblayout &&
	    !(ecc->mode == NAND_ECC_SOFT && ecc->algo == NAND_ECC_BCH)) {
		switch (mtd->oobsize) {
		case 8:
		case 16:
			mtd_set_ooblayout(mtd, &nand_ooblayout_sp_ops);
			break;
		case 64:
		case 128:
			mtd_set_ooblayout(mtd, &nand_ooblayout_lp_hamming_ops);
			break;
		default:
			/*
			 * Expose the whole OOB area to users if ECC_NONE
			 * is passed. We could do that for all kind of
			 * ->oobsize, but we must keep the old large/small
			 * page with ECC layout when ->oobsize <= 128 for
			 * compatibility reasons.
			 */
			if (ecc->mode == NAND_ECC_NONE) {
				mtd_set_ooblayout(mtd,
						&nand_ooblayout_lp_ops);
				break;
			}

			WARN(1, "No oob scheme defined for oobsize %d\n",
				mtd->oobsize);
			ret = -EINVAL;
			goto err_nand_manuf_cleanup;
		}
	}

	/*
	 * Check ECC mode, default to software if 3byte/512byte hardware ECC is
	 * selected and we have 256 byte pagesize fallback to software ECC
	 */
	printk("[DEBUG][%s]@%u  ecc current mode:%d    \n",__func__,__LINE__,ecc->mode);
	switch (ecc->mode) {
	case NAND_ECC_HW_OOB_FIRST:
		/* Similar to NAND_ECC_HW, but a separate read_page handle */
		if (!ecc->calculate || !ecc->correct || !ecc->hwctl) {
			WARN(1, "No ECC functions supplied; hardware ECC not possible\n");
			ret = -EINVAL;
			goto err_nand_manuf_cleanup;
		}
		if (!ecc->read_page)
			ecc->read_page = nand_read_page_hwecc_oob_first;
		/* fall through */

	case NAND_ECC_HW:
		/* Use standard hwecc read page function? */
		if (!ecc->read_page)
			ecc->read_page = nand_read_page_hwecc;
		if (!ecc->write_page)
			ecc->write_page = nand_write_page_hwecc;
		if (!ecc->read_page_raw)
			ecc->read_page_raw = nand_read_page_raw;
		if (!ecc->write_page_raw)
			ecc->write_page_raw = nand_write_page_raw;
		if (!ecc->read_oob)
			ecc->read_oob = nand_read_oob_std;
		if (!ecc->write_oob)
			ecc->write_oob = nand_write_oob_std;
		if (!ecc->read_subpage)
			ecc->read_subpage = nand_read_subpage;
		if (!ecc->write_subpage && ecc->hwctl && ecc->calculate)
			ecc->write_subpage = nand_write_subpage_hwecc;
		/* fall through */

	case NAND_ECC_HW_SYNDROME:
		if ((!ecc->calculate || !ecc->correct || !ecc->hwctl) &&
		    (!ecc->read_page ||
		     ecc->read_page == nand_read_page_hwecc ||
		     !ecc->write_page ||
		     ecc->write_page == nand_write_page_hwecc)) {
			WARN(1, "No ECC functions supplied; hardware ECC not possible\n");
			ret = -EINVAL;
			goto err_nand_manuf_cleanup;
		}
		/* Use standard syndrome read/write page function? */
		if (!ecc->read_page)
			ecc->read_page = nand_read_page_syndrome;
		if (!ecc->write_page)
			ecc->write_page = nand_write_page_syndrome;
		if (!ecc->read_page_raw)
			ecc->read_page_raw = nand_read_page_raw_syndrome;
		if (!ecc->write_page_raw)
			ecc->write_page_raw = nand_write_page_raw_syndrome;
		if (!ecc->read_oob)
			ecc->read_oob = nand_read_oob_syndrome;
		if (!ecc->write_oob)
			ecc->write_oob = nand_write_oob_syndrome;

		if (mtd->writesize >= ecc->size) {
			if (!ecc->strength) {
				WARN(1, "Driver must set ecc.strength when using hardware ECC\n");
				ret = -EINVAL;
				goto err_nand_manuf_cleanup;
			}
			break;
		}
		pr_warn("%d byte HW ECC not possible on %d byte page size, fallback to SW ECC\n",
			ecc->size, mtd->writesize);
		ecc->mode = NAND_ECC_SOFT;
		ecc->algo = NAND_ECC_HAMMING;
		/* fall through */

	case NAND_ECC_SOFT:
		ret = nand_set_ecc_soft_ops(chip);
		if (ret) {
			ret = -EINVAL;
			goto err_nand_manuf_cleanup;
		}
		break;

	case NAND_ECC_ON_DIE:
		if (!ecc->read_page || !ecc->write_page) {
			WARN(1, "No ECC functions supplied; on-die ECC not possible\n");
			ret = -EINVAL;
			goto err_nand_manuf_cleanup;
		}
		if (!ecc->read_oob)
			ecc->read_oob = nand_read_oob_std;
		if (!ecc->write_oob)
			ecc->write_oob = nand_write_oob_std;
		break;

	case NAND_ECC_NONE:
		pr_warn("NAND_ECC_NONE selected by board driver. This is not recommended!\n");
		ecc->read_page = nand_read_page_raw;
		ecc->write_page = nand_write_page_raw;
		ecc->read_oob = nand_read_oob_std;
		ecc->read_page_raw = nand_read_page_raw;
		ecc->write_page_raw = nand_write_page_raw;
		ecc->write_oob = nand_write_oob_std;
		ecc->size = mtd->writesize;
		ecc->bytes = 0;
		ecc->strength = 0;
		break;

	default:
		WARN(1, "Invalid NAND_ECC_MODE %d\n", ecc->mode);
		ret = -EINVAL;
		goto err_nand_manuf_cleanup;
	}

	if (ecc->correct || ecc->calculate) {
		ecc->calc_buf = kmalloc(mtd->oobsize, GFP_KERNEL);
		ecc->code_buf = kmalloc(mtd->oobsize, GFP_KERNEL);
		if (!ecc->calc_buf || !ecc->code_buf) {
			ret = -ENOMEM;
			goto err_nand_manuf_cleanup;
		}
	}

	/* For many systems, the standard OOB write also works for raw */
	if (!ecc->read_oob_raw)
		ecc->read_oob_raw = ecc->read_oob;
	if (!ecc->write_oob_raw)
		ecc->write_oob_raw = ecc->write_oob;

	/* propagate ecc info to mtd_info */
	mtd->ecc_strength = ecc->strength;
	mtd->ecc_step_size = ecc->size;

	/*
	 * Set the number of read / write steps for one page depending on ECC
	 * mode.
	 */
	ecc->steps = mtd->writesize / ecc->size;
	if (ecc->steps * ecc->size != mtd->writesize) {
		WARN(1, "Invalid ECC parameters\n");
		ret = -EINVAL;
		goto err_nand_manuf_cleanup;
	}
	ecc->total = ecc->steps * ecc->bytes;
	if (ecc->total > mtd->oobsize) {
		WARN(1, "Total number of ECC bytes exceeded oobsize\n");
		ret = -EINVAL;
		goto err_nand_manuf_cleanup;
	}

	/*
	 * The number of bytes available for a client to place data into
	 * the out of band area.
	 */
	ret = mtd_ooblayout_count_freebytes(mtd);
	if (ret < 0)
		ret = 0;

	mtd->oobavail = ret;

	/* ECC sanity check: warn if it's too weak */
	if (!nand_ecc_strength_good(chip))
		pr_warn("WARNING: %s: the ECC used on your system is too weak compared to the one required by the NAND chip\n",
			mtd->name);

	/* Allow subpage writes up to ecc.steps. Not possible for MLC flash */
	if (!(chip->options & NAND_NO_SUBPAGE_WRITE) && nand_is_slc(chip)) {
		switch (ecc->steps) {
		case 2:
			mtd->subpage_sft = 1;
			break;
		case 4:
		case 8:
		case 16:
			mtd->subpage_sft = 2;
			break;
		}
	}
	chip->subpagesize = mtd->writesize >> mtd->subpage_sft;

	/* Invalidate the pagebuffer reference */
	chip->pagecache.page = -1;

	/* Large page NAND with SOFT_ECC should support subpage reads */
	switch (ecc->mode) {
	case NAND_ECC_SOFT:
		if (chip->page_shift > 9)
			chip->options |= NAND_SUBPAGE_READ;
		break;

	default:
		break;
	}

	ret = nanddev_init(&chip->base, &rawnand_ops, mtd->owner);
	if (ret)
		goto err_nand_manuf_cleanup;

	/* Adjust the MTD_CAP_ flags when NAND_ROM is set. */
	if (chip->options & NAND_ROM)
		mtd->flags = MTD_CAP_ROM;

	/* Fill in remaining MTD driver data */
	mtd->_erase = nand_erase;
	mtd->_point = NULL;
	mtd->_unpoint = NULL;
	mtd->_panic_write = panic_nand_write;
	mtd->_read_oob = nand_read_oob;
	mtd->_write_oob = nand_write_oob;
	mtd->_sync = nand_sync;
	mtd->_lock = NULL;
	mtd->_unlock = NULL;
	mtd->_suspend = nand_suspend;
	mtd->_resume = nand_resume;
	mtd->_reboot = nand_shutdown;
	mtd->_block_isreserved = nand_block_isreserved;
	mtd->_block_isbad = nand_block_isbad;
	mtd->_block_markbad = nand_block_markbad;
	mtd->_max_bad_blocks = nanddev_mtd_max_bad_blocks;

	/*
	 * Initialize bitflip_threshold to its default prior scan_bbt() call.
	 * scan_bbt() might invoke mtd_read(), thus bitflip_threshold must be
	 * properly set.
	 */
	if (!mtd->bitflip_threshold)
		mtd->bitflip_threshold = DIV_ROUND_UP(mtd->ecc_strength * 3, 4);

	/* Initialize the ->data_interface field. */
	ret = nand_init_data_interface(chip);
	if (ret)
		goto err_nanddev_cleanup;

	/* Enter fastest possible mode on all dies. */
	for (i = 0; i < nanddev_ntargets(&chip->base); i++) {
		ret = nand_setup_data_interface(chip, i);
		if (ret)
			goto err_nanddev_cleanup;
	}

	/* Check, if we should skip the bad block table scan */
	if (chip->options & NAND_SKIP_BBTSCAN)
		return 0;

	/* Build bad block table */
	ret = nand_create_bbt(chip);
	if (ret)
		goto err_nanddev_cleanup;

	return 0;

err_nanddev_cleanup:
	nanddev_cleanup(&chip->base);

err_nand_manuf_cleanup:
	nand_manufacturer_cleanup(chip);

err_free_buf:
	kfree(chip->data_buf);
	kfree(ecc->code_buf);
	kfree(ecc->calc_buf);
	return ret;
}

static int nand_attach(struct nand_chip *chip)
{
	if (chip->controller->ops && chip->controller->ops->attach_chip)
		return chip->controller->ops->attach_chip(chip);

	return 0;
}

static void nand_detach(struct nand_chip *chip)
{
	if (chip->controller->ops && chip->controller->ops->detach_chip)
		chip->controller->ops->detach_chip(chip);
}

/**
 * nand_scan_with_ids - [NAND Interface] Scan for the NAND device
 * @chip: NAND chip object
 * @maxchips: number of chips to scan for.
 * @ids: optional flash IDs table
 *
 * This fills out all the uninitialized function pointers with the defaults.
 * The flash ID is read and the mtd/chip structures are filled with the
 * appropriate values.
 */
int nand_scan_with_ids(struct nand_chip *chip, unsigned int maxchips,
		       struct nand_flash_dev *ids)
{
	int ret;

	if (!maxchips)
		return -EINVAL;

	ret = nand_scan_ident(chip, maxchips, ids);
	if (ret)
		return ret;

	ret = nand_attach(chip);
	if (ret)
		goto cleanup_ident;

	ret = nand_scan_tail(chip);
	if (ret)
		goto detach_chip;

	return 0;

detach_chip:
	nand_detach(chip);
cleanup_ident:
	nand_scan_ident_cleanup(chip);

	return ret;
}
EXPORT_SYMBOL(nand_scan_with_ids);

/**
 * nand_cleanup - [NAND Interface] Free resources held by the NAND device
 * @chip: NAND chip object
 */
void nand_cleanup(struct nand_chip *chip)
{
	if (chip->ecc.mode == NAND_ECC_SOFT &&
	    chip->ecc.algo == NAND_ECC_BCH)
		nand_bch_free((struct nand_bch_control *)chip->ecc.priv);

	nanddev_cleanup(&chip->base);

	/* Free bad block table memory */
	kfree(chip->bbt);
	kfree(chip->data_buf);
	kfree(chip->ecc.code_buf);
	kfree(chip->ecc.calc_buf);

	/* Free bad block descriptor memory */
	if (chip->badblock_pattern && chip->badblock_pattern->options
			& NAND_BBT_DYNAMICSTRUCT)
		kfree(chip->badblock_pattern);

	/* Free manufacturer priv data. */
	nand_manufacturer_cleanup(chip);

	/* Free controller specific allocations after chip identification */
	nand_detach(chip);

	/* Free identification phase allocations */
	nand_scan_ident_cleanup(chip);
}

EXPORT_SYMBOL_GPL(nand_cleanup);

/**
 * nand_release - [NAND Interface] Unregister the MTD device and free resources
 *		  held by the NAND device
 * @chip: NAND chip object
 */
void nand_release(struct nand_chip *chip)
{
	mtd_device_unregister(nand_to_mtd(chip));
	nand_cleanup(chip);
}
EXPORT_SYMBOL_GPL(nand_release);

static void __exit nand_base_exit(void)
{
#ifdef CONFIG_ROKU_MTD_NAND_ENCRYPTION
	mutex_lock(&nand_crypto_mutex);
	if (iv_cipher != NULL) {
		crypto_free_blkcipher(iv_cipher);
		iv_cipher = NULL;
	}
	if (nand_cipher != NULL) {
		crypto_free_blkcipher(nand_cipher);
		nand_cipher = NULL;
	}
	mutex_unlock(&nand_crypto_mutex);
#endif /* CONFIG_ROKU_MTD_NAND_ENCRYPTION */
}

module_exit(nand_base_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steven J. Hill <sjhill@realitydiluted.com>");
MODULE_AUTHOR("Thomas Gleixner <tglx@linutronix.de>");
MODULE_DESCRIPTION("Generic NAND flash driver code");
