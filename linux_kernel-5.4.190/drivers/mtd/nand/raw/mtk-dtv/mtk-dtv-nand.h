/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#ifndef __MTK_DTV_NAND_H__
#define __MTK_DTV_NAND_H__

#define DMA_BIT_MASK_FCIE	32
#define MAX_CLK_SOURCE		16

struct mtk_fcie_nand_clk_table {
	u32 clk_hz;
	u32 value;
};

struct mtk_fcie_nand_clk_info {
	struct clk *clk;
	struct clk *smi_fcie2fcie;
	u32 clk_cnt;
	struct mtk_fcie_nand_clk_table clk_table[MAX_CLK_SOURCE];
};

struct mtk_fcie_nand_host {
	struct nand_controller controller;
	struct nand_chip chip;
	struct device *dev;
	void __iomem *fciebase;
	dma_addr_t miu_base;
	u8 enable_sar5;
	struct mtk_fcie_nand_clk_info clk_info;
	struct nand_flash_dev table;
};
#endif
