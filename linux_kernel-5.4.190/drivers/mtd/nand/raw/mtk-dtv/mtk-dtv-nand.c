// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#include "drvNAND.h"

#define DRV_NAME	"mtk_fcie_nand"

static int read_byte_idx;

#define REG_BASE           0x1C000000
#define REG_MOD_A_BANK     0x2423
#define REG_TEST_MOD_BANK  0x2426
void * mmio_base_2423 = NULL;
void * mmio_base_2426 = NULL;

/*** nand_chip replaceable API ***/
static uint8_t mtk_fcie_nand_read_byte(struct nand_chip *chip)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U8 *pu8_SpareBuf = pNandDrv->pu8_PageSpareBuf;
	uint8_t val;

	val = pu8_SpareBuf[read_byte_idx];
	read_byte_idx++;

	return val;
}

static void mtk_fcie_nand_select_chip(struct nand_chip *chip, int cs)
{
	// DUMMY
}

static void mtk_fcie_nand_cmdfunc(struct nand_chip *chip, unsigned int command, int column,
	int page_addr)
{
	U32 u32_Err;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U8 *pu8_SpareBuf = pNandDrv->pu8_PageSpareBuf;

	if (command == NAND_CMD_RESET) {
		NC_ResetNandFlash();
	} else if (command == NAND_CMD_READID) {
		read_byte_idx = 0;
		NC_ReadID();
		memcpy(pu8_SpareBuf, pNandDrv->au8_ID, NAND_ID_BYTE_CNT);
	} else if (command == NAND_CMD_STATUS) {
		read_byte_idx = 0;
		NC_ReadStatus();
	} else if (command == NAND_CMD_ERASE1) {
		u32_Err = NC_EraseBlk(page_addr);
		pu8_SpareBuf[0] = u32_Err; //checked by waitfunc
	} else if (command == NAND_CMD_ERASE2) {

	} else {
		pr_err("error, unsupported command 0x%X\n", command);
	}
}

static int mtk_fcie_nand_waitfunc(struct nand_chip *chip)
{
	U32 u32_Err;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U8 *pu8_SpareBuf = pNandDrv->pu8_PageSpareBuf;

	u32_Err = pu8_SpareBuf[0];
	if (u32_Err)
		return NAND_STATUS_FAIL;

	return (int)REG(NC_ST_READ);
}

/*** nand_ecc_ctrl replaceable API ***/
static int mtk_fcie_nand_read_page_raw(struct nand_chip *chip, uint8_t *buf,
	int oob_required, int page, int encrypted)
{
	U32 u32_Err;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U8 *pu8_SpareBuf = oob_required ? chip->oob_poi : pNandDrv->pu8_PageSpareBuf;

	u32_Err = NC_ReadPages(page, buf, pu8_SpareBuf, 1);
	if (u32_Err)
		pr_info("%s error %X\n", __func__, u32_Err);

	return 0;
}

static int mtk_fcie_nand_write_page_raw(struct nand_chip *chip, const uint8_t *buf,
	int oob_required, int page, int encrypted)
{
	U32 u32_Err;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U8 *pu8_SpareBuf = oob_required ? chip->oob_poi : pNandDrv->pu8_PageSpareBuf;

	if (!oob_required)
		memset(pNandDrv->pu8_PageSpareBuf, MAX_U8_VALUE, pNandDrv->u16_SpareByteCnt);

	u32_Err = NC_WritePages(page, (U8 *)buf, pu8_SpareBuf, 1);
	if (u32_Err)
		return -EIO;

	return 0;
}

static int mtk_fcie_nand_read_page(struct nand_chip *chip, uint8_t *buf, int oob_required,
	int page, int encrypted)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	U32 u32_Err;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U8 *pu8_SpareBuf = oob_required ? chip->oob_poi : pNandDrv->pu8_PageSpareBuf;
	unsigned int max_bitflips = 0;

	u32_Err = NC_ReadPages(page, buf, pu8_SpareBuf, 1);
	if (u32_Err)
		mtd->ecc_stats.failed++;
	else {
		mtd->ecc_stats.corrected += NC_GetECCBits();
		max_bitflips = NC_GetECCBits();
	}

	return max_bitflips;
}

static int mtk_fcie_nand_write_page(struct nand_chip *chip, const uint8_t *buf,
	int oob_required, int page, int encrypted)
{
	U32 u32_Err;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U8 *pu8_SpareBuf = oob_required ? chip->oob_poi : pNandDrv->pu8_PageSpareBuf;

	if (!oob_required)
		memset(pNandDrv->pu8_PageSpareBuf, MAX_U8_VALUE, pNandDrv->u16_SpareByteCnt);

	u32_Err = NC_WritePages(page, (U8 *)buf, pu8_SpareBuf, 1);
	if (u32_Err)
		return -EIO;

	return 0;
}

static int mtk_fcie_nand_read_oob(struct nand_chip *chip, int page)
{
	U32 u32_Err;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U8 *pu8_DataBuf = pNandDrv->pu8_PageDataBuf;

	u32_Err = NC_ReadPages(page, pu8_DataBuf, chip->oob_poi, 1);
	if (u32_Err)
		pr_info("%s error %X\n", __func__, u32_Err);

	return 0;
}

static int mtk_fcie_nand_write_oob(struct nand_chip *chip, int page)
{
	U32 u32_Err;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U8 *pu8_DataBuf = pNandDrv->pu8_PageDataBuf;

	memset(pNandDrv->pu8_PageDataBuf, MAX_U8_VALUE, pNandDrv->u16_PageByteCnt);

	u32_Err = NC_WritePages(page, pu8_DataBuf, chip->oob_poi, 1);
	if (u32_Err)
		return -EIO;

	return 0;
}

static int mtk_fcie_nand_ooblayout_free(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *oob_region)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	u32 eccsteps;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	eccsteps = mtd->writesize / chip->ecc.size;

	if (section >= eccsteps)
		return -ERANGE;

	oob_region->length = pNandDrv->u16_SectorSpareByteCnt - pNandDrv->u16_ECCCodeByteCnt;
	oob_region->offset = section * pNandDrv->u16_SectorSpareByteCnt;

	return 0;
}

static int mtk_fcie_nand_ooblayout_ecc(struct mtd_info *mtd, int section,
				 struct mtd_oob_region *oob_region)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	u32 eccsteps;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	eccsteps = mtd->writesize / chip->ecc.size;

	if (section >= eccsteps)
		return -ERANGE;

	oob_region->length = pNandDrv->u16_ECCCodeByteCnt;
	oob_region->offset = (section + 1) * pNandDrv->u16_SectorSpareByteCnt
		- pNandDrv->u16_ECCCodeByteCnt;

	return 0;
}

static const struct mtd_ooblayout_ops mtk_fcie_nand_ooblayout_ops = {
	.free = mtk_fcie_nand_ooblayout_free,
	.ecc = mtk_fcie_nand_ooblayout_ecc,
};

static int fill_nand_flash_dev(struct nand_flash_dev *table)
{
	#define SHIFT_1K	10
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	table->name = (char *)pNandDrv->u8_PartNumber;
	memcpy((void *)table->id, (const void *)pNandDrv->au8_ID, pNandDrv->u8_IDByteCnt);
	table->id_len = (uint16_t)pNandDrv->u8_IDByteCnt;
	table->pagesize = (int)pNandDrv->u16_PageByteCnt;
	table->erasesize = (unsigned int)pNandDrv->u16_BlkPageCnt
		* (unsigned int)pNandDrv->u16_PageByteCnt;
	table->chipsize = ((table->erasesize >> SHIFT_1K) * (unsigned int)pNandDrv->u16_BlkCnt)
		>> SHIFT_1K;
	table->options = 0;
	table->oobsize = (uint16_t)pNandDrv->u16_SpareByteCnt;
	table->ecc.strength_ds = pNandDrv->u16_ECCCorretableBit;
	table->ecc.step_ds = pNandDrv->u16_SectorByteCnt;
	table->onfi_timing_mode_default = 0;

	return 0;
}
#ifdef CONFIG_ROKU_MTD_NAND_MODIFY
static ssize_t partname_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	return snprintf(buf, strlen(pNandDrv->u8_PartNumber) + strlen(pNandDrv->u8_Vendor) + 2, "%s %s\n", pNandDrv->u8_Vendor, pNandDrv->u8_PartNumber);
}

static DEVICE_ATTR_RO(partname);
#endif

static int mtk_fcie_nand_attach_chip(struct nand_chip *chip)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	struct mtd_info *mtd = nand_to_mtd(chip);

	chip->ecc.mode = NAND_ECC_HW;
	chip->ecc.size = pNandDrv->u16_SectorByteCnt;
	chip->ecc.bytes = pNandDrv->u16_ECCCodeByteCnt;
	chip->ecc.strength = pNandDrv->u16_ECCCorretableBit;
	chip->ecc.read_page_raw = mtk_fcie_nand_read_page_raw;
	chip->ecc.write_page_raw = mtk_fcie_nand_write_page_raw;
	chip->ecc.read_page = mtk_fcie_nand_read_page;
	chip->ecc.write_page = mtk_fcie_nand_write_page;
	chip->ecc.read_oob = mtk_fcie_nand_read_oob;
	chip->ecc.write_oob = mtk_fcie_nand_write_oob;

	if (chip->bbt_options & NAND_BBT_USE_FLASH)
		chip->bbt_options |= NAND_BBT_NO_OOB;
	
	if (pNandDrv->u16_BitflipThreshold)
		mtd->bitflip_threshold = pNandDrv->u16_BitflipThreshold;

	return 0;
}

static const struct nand_controller_ops mtk_fcie_nand_controller_ops = {
	.attach_chip = mtk_fcie_nand_attach_chip,
};

static int mtk_fcie_nand_probe(struct platform_device *pdev)
{
	struct mtk_fcie_nand_host *host;
	int ret;
	struct mtd_info *mtd;
	struct nand_chip *chip;
	U32 u32_Err;
	struct device *dev = &pdev->dev;
	struct resource *res;
	int irq;
	size_t sz = 0;
	int len = 0;
	u32 tmp;

	host = devm_kzalloc(dev, sizeof(*host), GFP_KERNEL);
	if (!host)
		return -ENOMEM;

	ret = of_property_read_u32(dev->of_node, "miu-base", &tmp);
	if (ret) {
		dev_err(dev, "Failed to read miu-base setting: %d\n", ret);
		return ret;
	}
	host->miu_base = (dma_addr_t)tmp;
	dev_info(dev, "miu-base = 0x%llx\n", host->miu_base);

	if (!of_get_property(dev->of_node, "freq-table-hz", &len) || len <= 0) {
		dev_err(dev, "Failed to read freq-table-hz\n");
		return -EINVAL;
	}
	sz = len / sizeof(struct mtk_fcie_nand_clk_table);
	if (sz > MAX_CLK_SOURCE)
		sz = MAX_CLK_SOURCE;

	len = (sz * sizeof(struct mtk_fcie_nand_clk_table)) / sizeof(u32);
	ret = of_property_read_u32_array(dev->of_node, "freq-table-hz",
		(u32 *)&host->clk_info.clk_table[0], len);
	if (ret) {
		dev_err(dev, "Failed to read freq-table-hz settings: %d\n", ret);
		return ret;
	}
	host->clk_info.clk_cnt = sz;
	dev_info(dev, "clk_cnt = %ld\n", sz);
	for (len = 0 ; len < sz ; len++) {
		dev_info(dev, "[%d] clk_hz = %d, value = %d\n", len,
			host->clk_info.clk_table[len].clk_hz,
			host->clk_info.clk_table[len].value);
	}

	if (of_property_read_bool(dev->of_node, "sar5-disable"))
		host->enable_sar5 = 0;
	else
		host->enable_sar5 = 1;

	host->dev = dev;
	chip = &host->chip;
	mtd = nand_to_mtd(chip);

	mtd->owner = THIS_MODULE;
	mtd->dev.parent = host->dev;
	mtd->name = DRV_NAME;
	mtd_set_ooblayout(mtd, &mtk_fcie_nand_ooblayout_ops);

	nand_set_flash_node(chip, dev->of_node);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	host->fciebase = devm_ioremap_resource(dev, res);
	if (IS_ERR(host->fciebase))
		return PTR_ERR(host->fciebase);

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0)
		return -EINVAL;

	host->clk_info.clk = devm_clk_get(dev, "clk");
	if (IS_ERR(host->clk_info.clk))
		return PTR_ERR(host->clk_info.clk);

	host->clk_info.smi_fcie2fcie = devm_clk_get(dev, "smi_fcie2fcie");
	if (IS_ERR(host->clk_info.smi_fcie2fcie))
		return PTR_ERR(host->clk_info.smi_fcie2fcie);

	ret = clk_prepare_enable(host->clk_info.clk);
	if (ret) {
		dev_err(dev, "Unable to enable clock!\n");
		return ret;
	}

	ret = clk_prepare_enable(host->clk_info.smi_fcie2fcie);
	if (ret) {
		dev_err(dev, "Unable to enable smi_fcie2fcie clock!\n");
		return ret;
	}
	mmio_base_2423 = ioremap_wc(REG_BASE + (REG_MOD_A_BANK << 9), 1024);
	mmio_base_2426 = ioremap_wc(REG_BASE + (REG_TEST_MOD_BANK << 9), 1024);
	ret = devm_request_irq(dev, irq, NC_FCIE_IRQ, 0, DRV_NAME, host);
	if (ret) {
		dev_err(dev, "Failed to request IRQ: %d\n", ret);
		goto disable_clk;
	}

	u32_Err = drvNAND_Init((void *)host);
	if (u32_Err) {
		dev_err(dev, "Failed to init NAND driver: 0x%X\n", u32_Err);
		ret = -ENODEV;
		goto disable_clk;
	}

	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(DMA_BIT_MASK_FCIE));
	if (ret) {
		dev_err(dev, "Failed to set dma mask and coherent: %d\n", ret);
		goto disable_clk;
	}

	chip->legacy.read_byte = mtk_fcie_nand_read_byte;
	chip->legacy.select_chip = mtk_fcie_nand_select_chip;
	chip->legacy.cmdfunc = mtk_fcie_nand_cmdfunc;
	chip->legacy.waitfunc = mtk_fcie_nand_waitfunc;
	chip->options |= NAND_NO_SUBPAGE_WRITE | NAND_USE_BOUNCE_BUFFER;
	chip->buf_align = ARCH_DMA_MINALIGN;

	ret = fill_nand_flash_dev(&host->table);
	if (ret) {
		dev_err(dev, "Failed to fill nand flash dev: %d\n", ret);
		goto disable_clk;
	}
#ifdef CONFIG_ROKU_MTD_NAND_MODIFY
	ret = device_create_file(&pdev->dev, &dev_attr_partname);
	if (ret) {
		dev_err(dev, "Failed to create device file: %d\n", ret);
		goto disable_clk;
	}
#endif
	nand_controller_init(&host->controller);
	host->controller.ops = &mtk_fcie_nand_controller_ops;
	chip->controller = &host->controller;

	ret = nand_scan_with_ids(chip, 1, &host->table);
	if (ret) {
		dev_err(dev, "Failed to scan NAND: %d\n", ret);
		goto disable_clk;
	}

	platform_set_drvdata(pdev, host);

	/* Register device in MTD */
	ret = mtd_device_register(mtd, NULL, 0);
	if (ret)
		goto cleanup_nand;

	#if defined(ENABLE_NAND_POWER_SAVING_MODE) && ENABLE_NAND_POWER_SAVING_MODE
	if (host->enable_sar5)
		nand_Prepare_Power_Saving_Mode_Queue();
	#endif

	return 0;

cleanup_nand:
	nand_cleanup(chip);
disable_clk:
	clk_disable_unprepare(host->clk_info.clk);

	return ret;
}

static const struct of_device_id mtk_fcie_nand_ids[] = {
	{
		.compatible = "mediatek,nand-fcie",
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, mtk_fcie_nand_ids);

static int mtk_fcie_nand_remove(struct platform_device *pdev)
{
	struct mtk_fcie_nand_host *host = platform_get_drvdata(pdev);

	nand_release(&host->chip);

	clk_disable_unprepare(host->clk_info.clk);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int mtk_fcie_nand_suspend(struct device *dev)
{
	struct mtk_fcie_nand_host *host = dev_get_drvdata(dev);

	dev_info(dev, "%s +\n", __func__);

	clk_disable_unprepare(host->clk_info.clk);
	clk_disable_unprepare(host->clk_info.smi_fcie2fcie);

	dev_info(dev, "%s -\n", __func__);

	return 0;
}

static int mtk_fcie_nand_resume(struct device *dev)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	struct mtk_fcie_nand_host *host = dev_get_drvdata(dev);
	int err;

	dev_info(dev, "%s +\n", __func__);

	err = clk_prepare_enable(host->clk_info.clk);
	if (err)
		return err;

	err = clk_prepare_enable(host->clk_info.smi_fcie2fcie);
	if (err)
		return err;

	// TO DO : reinit FCIE reg
	NC_ReConfig();

	#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
	REG_WRITE_UINT16(NC_MIE_INT_EN, BIT_NC_JOB_END_EN);
	#endif

	#if defined(ENABLE_NAND_POWER_SAVING_MODE) && ENABLE_NAND_POWER_SAVING_MODE
	if (host->enable_sar5)
		nand_Prepare_Power_Saving_Mode_Queue();
	#endif

	dev_info(dev, "%s -\n", __func__);

	return 0;
}

static SIMPLE_DEV_PM_OPS(mtk_fcie_nand_pm_ops, mtk_fcie_nand_suspend, mtk_fcie_nand_resume);
#endif

static struct platform_driver mtk_fcie_nand_driver = {
	.probe  = mtk_fcie_nand_probe,
	.remove = mtk_fcie_nand_remove,
	.driver = {
		.name  = DRV_NAME,
		.of_match_table = mtk_fcie_nand_ids,
#ifdef CONFIG_PM_SLEEP
		.pm = &mtk_fcie_nand_pm_ops,
#endif
	},
};

module_platform_driver(mtk_fcie_nand_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Edward-CH Lee <edward-ch.lee@mediatek.com>");
MODULE_DESCRIPTION("MTK DTV Nand Flash Controller Driver");
