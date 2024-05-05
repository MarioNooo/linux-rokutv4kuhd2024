/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */
//Page TSP1_TEST_1
#define REG_0000_TSP1 (0x000)
	#define TSP1_REG_MCU_MSG2_0_0000 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0004_TSP1 (0x004)
	#define TSP1_REG_MCU_MSG2_1_0004 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0008_TSP1 (0x008)
	#define TSP1_REG_LPCR1_PVR1_0_0008 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_000C_TSP1 (0x00C)
	#define TSP1_REG_LPCR1_PVR1_1_000C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0010_TSP1 (0x010)
	#define TSP1_REG_LPCR2_0_0010 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0014_TSP1 (0x014)
	#define TSP1_REG_LPCR2_1_0014 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0018_TSP1 (0x018)
	#define TSP1_REG_LPCR1_WLD_PVR1_0018 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_LPCR1_RLD_PVR1_0018 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP1_REG_LPCR2_WLD_0018 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP1_REG_LPCR2_RLD_0018 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP1_REG_PKT192_EN_PVR_0018 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP1_REG_PKT192_EN_FILEIN_0018 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP1_REG_RVU_TIMESTAMP_EN_0018 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP1_REG_ORZ_DMAW_PROT_EN_0018 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP1_REG_CLR_PIDFLT_BYTE_CNT_0018 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_RM_PINPON_LIMIT_0018 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_RST_TS_FIN_0018 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_001C_TSP1 (0x01C)
	#define TSP1_REG_VQTX0_BLOCK_DIS_001C Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_VQTX1_BLOCK_DIS_001C Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP1_REG_VQTX2_BLOCK_DIS_001C Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP1_REG_VQTX3_BLOCK_DIS_001C Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP1_REG_DIS_MIU_RQ_001C Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP1_REG_RM_DMA_GLITCH_001C Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP1_REG_RESET_VFIFO_001C Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_RESET_AFIFO_001C Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_RESET_GDMA_001C Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP1_REG_CLEAR_ALL_FLT_MATCH_001C Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_RESET_AFIFOB_001C Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_RESET_VFIFO_3D_001C Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_PVR_WPRIORITY_HIGH_001C Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_OPT_TIMING_001C Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0020_TSP1 (0x020)
	#define TSP1_REG_PKT_CHK_SIZE_FIN_0020 Fld(8, 0, AC_FULLB0)//[7:0]
	#define TSP1_REG_PKT192_BLK_DIS_FIN_0020 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_AV_CLR_0020 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_HW_STANDBY_MODE_0020 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP1_REG_LFSR_SEL_0020 Fld(3, 11, AC_MSKB1)//[13:11]
	#define TSP1_REG_WR_SRC_SEL_0020 Fld(2, 14, AC_MSKB1)//[15:14]
#define REG_0024_TSP1 (0x024)
	#define TSP1_REG_STC_DIFF_EN_0024 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP1_REG_SYS_TIME_MODE_0024 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP1_REG_SEC_DMA_BURST_EN_0024 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP1_REG_PVR_CLR_0024 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_DUP_PKT_SKIP_V_0024 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_DUP_PKT_SKIP_V3D_0024 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP1_REG_DUP_PKT_SKIP_A_0024 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_DUP_PKT_SKIP_AD_0024 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_DUP_PKT_SKIP_AD_C_0024 Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_DUP_PKT_SKIP_AD_D_0024 Fld(1, 14, AC_MSKB1)//[14:14]
#define REG_0028_TSP1 (0x028)
	#define TSP1_REG_DMA_RADDR_MSB_0028 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_002C_TSP1 (0x02C)
	#define TSP1_REG_VPID_3D_BYPASS_002C Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_VPID_3D_ERR_RM_EN_002C Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_PS_VID_3D_EN_002C Fld(1, 14, AC_MSKB1)//[14:14]
#define REG_0030_TSP1 (0x030)
	#define TSP1_REG_MATCH_PID_SCR_TS_0_0030 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0034_TSP1 (0x034)
	#define TSP1_REG_MATCH_PID_SCR_TS_1_0034 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0040_TSP1 (0x040)
	#define TSP1_REG_PCR64_2_RIU_0_0040 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0044_TSP1 (0x044)
	#define TSP1_REG_PCR64_2_RIU_1_0044 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0048_TSP1 (0x048)
	#define TSP1_REG_PCR64_2_RIU_2_0048 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_004C_TSP1 (0x04C)
	#define TSP1_REG_PCR64_2_RIU_3_004C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0050_TSP1 (0x050)
	#define TSP1_REG_DMAW_LBND0_0_0050 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0054_TSP1 (0x054)
	#define TSP1_REG_DMAW_LBND0_1_0054 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0058_TSP1 (0x058)
	#define TSP1_REG_DMAW_UBND0_0_0058 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_005C_TSP1 (0x05C)
	#define TSP1_REG_DMAW_UBND0_1_005C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0060_TSP1 (0x060)
	#define TSP1_REG_DMAW_LBND1_0_0060 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0064_TSP1 (0x064)
	#define TSP1_REG_DMAW_LBND1_1_0064 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0068_TSP1 (0x068)
	#define TSP1_REG_DMAW_UBND1_0_0068 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_006C_TSP1 (0x06C)
	#define TSP1_REG_DMAW_UBND1_1_006C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0070_TSP1 (0x070)
	#define TSP1_REG_CLR_NO_HIT_INT_0070 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_DMAW_WADDR_ERR_SRC_SEL_0070 Fld(4, 1, AC_MSKB0)//[4:1]
	#define TSP1_REG_CLR_SEC_DMAW_OVERFLOW_0070 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP1_REG_APES_B_ERR_RM_EN_0070 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP1_REG_APES_C_ERR_RM_EN_0070 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_APES_D_ERR_RM_EN_0070 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_BLK_AF_SCRMB_BIT_0070 Fld(1, 10, AC_MSKB1)//[10:10]
#define REG_0074_TSP1 (0x074)
	#define TSP1_REG_PKT_SIZE_FILEIN_0074 Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_0078_TSP1 (0x078)
	#define TSP1_REG_AUDC_SRC_0078 Fld(3, 0, AC_MSKB0)//[2:0]
	#define TSP1_REG_AUDD_SRC_0078 Fld(3, 3, AC_MSKB0)//[5:3]
#define REG_007C_TSP1 (0x07C)
	#define TSP1_REG_CLR_SRAM_COLLISION_007C Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_TS_OUT_EN_007C Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP1_REG_PDFLT_ALL_VLD_EN_007C Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_PKT130_PSI_EN_007C Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_PKT130_TEI_EN_007C Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_PKT130_ERR_CLR_007C Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_PKT130_EN_007C Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0080_TSP1 (0x080)
	#define TSP1_REG_VQ0_BASE_0_0080 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0084_TSP1 (0x084)
	#define TSP1_REG_VQ0_BASE_1_0084 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0088_TSP1 (0x088)
	#define TSP1_REG_VQ0_SIZE_192BYTE_0088 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_008C_TSP1 (0x08C)
	#define TSP1_REG_VQ0_WR_THRESHOLD_008C Fld(4, 0, AC_MSKB0)//[3:0]
	#define TSP1_REG_VQ0_PRIORITY_THRESHOLD_008C Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP1_REG_VQ0_FORCEFIRE_CNT_1K_008C Fld(4, 8, AC_MSKB1)//[11:8]
	#define TSP1_REG_VQ0_RESET_008C Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_VQ0_OVERFLOW_INT_ENABLE_008C Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_VQ0_CLR_OVERFLOW_INT_008C Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0090_TSP1 (0x090)
	#define TSP1_REG_VQ_RX_THRESHOLD_0090 Fld(3, 1, AC_MSKB0)//[3:1]
#define REG_00A8_TSP1 (0x0A8)
	#define TSP1_REG_DMAW_LBND2_0_00A8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00AC_TSP1 (0x0AC)
	#define TSP1_REG_DMAW_LBND2_1_00AC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00B0_TSP1 (0x0B0)
	#define TSP1_REG_DMAW_UBND2_0_00B0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00B4_TSP1 (0x0B4)
	#define TSP1_REG_DMAW_UBND2_1_00B4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00B8_TSP1 (0x0B8)
	#define TSP1_REG_DMAW_LBND3_0_00B8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00BC_TSP1 (0x0BC)
	#define TSP1_REG_DMAW_LBND3_1_00BC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00C0_TSP1 (0x0C0)
	#define TSP1_REG_DMAW_UBND3_0_00C0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00C4_TSP1 (0x0C4)
	#define TSP1_REG_DMAW_UBND3_1_00C4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00C8_TSP1 (0x0C8)
	#define TSP1_REG_DMAW_LBND4_0_00C8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00CC_TSP1 (0x0CC)
	#define TSP1_REG_DMAW_LBND4_1_00CC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00D0_TSP1 (0x0D0)
	#define TSP1_REG_DMAW_UBND4_0_00D0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00D4_TSP1 (0x0D4)
	#define TSP1_REG_DMAW_UBND4_1_00D4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00D8_TSP1 (0x0D8)
	#define TSP1_REG_ORZ_DMAW_LBND_0_00D8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00DC_TSP1 (0x0DC)
	#define TSP1_REG_ORZ_DMAW_LBND_1_00DC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00E0_TSP1 (0x0E0)
	#define TSP1_REG_ORZ_DMAW_UBND_0_00E0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00E4_TSP1 (0x0E4)
	#define TSP1_REG_ORZ_DMAW_UBND_1_00E4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00F0_TSP1 (0x0F0)
	#define TSP1_REG_PCR0_LATCH_0_00F0_15_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00F4_TSP1 (0x0F4)
	#define TSP1_REG_PCR0_LATCH_1_00F4_31_16 Fld(16, 16, AC_FULLW32)//[31:16]
#define REG_00F8_TSP1 (0x0F8)
	#define TSP1_REG_PCR_EXT_LATCH_00F8_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_PCR_EXT_LATCH_00F8_9_1 Fld(9, 1, AC_MSKW10)//[9:1]
	#define TSP1_REG_PCR_EXT_LATCH_00F8_15_10 Fld(6, 10, AC_MSKB1)//[15:10]
#define REG_00FC_TSP1 (0x0FC)
	#define TSP1_REG_ORZ2MI_ADDR_EXT_00FC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0100_TSP1 (0x100)
	#define TSP1_REG_CA_CTRL_0_0100_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_CA_CTRL_0_0100_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP1_REG_CA_CTRL_0_0100_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP1_REG_CA_CTRL_0_0100_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP1_REG_CA_CTRL_0_0100_4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP1_REG_CA_CTRL_0_0100_5 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP1_REG_CA_CTRL_0_0100_6 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP1_REG_CA_CTRL_0_0100_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP1_REG_CA_CTRL_0_0100_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_CA_CTRL_0_0100_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_CA_CTRL_0_0100_10 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP1_REG_CA_CTRL_0_0100_11 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_CA_CTRL_0_0100_12 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_CA_CTRL_0_0100_13 Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_CA_CTRL_0_0100_14 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_CA_CTRL_0_0100_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0104_TSP1 (0x104)
	#define TSP1_REG_CA_CTRL_1_0104_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_CA_CTRL_1_0104_2_1 Fld(2, 1, AC_MSKB0)//[2:1]
	#define TSP1_REG_CA_CTRL_1_0104_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP1_REG_CA_CTRL_1_0104_4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP1_REG_CA_CTRL_1_0104_5 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP1_REG_CA_CTRL_1_0104_6 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP1_REG_CA_CTRL_1_0104_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP1_REG_CA_CTRL_1_0104_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_CA_CTRL_1_0104_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_CA_CTRL_1_0104_10 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP1_REG_CA_CTRL_1_0104_11 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_CA_CTRL_1_0104_12 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_CA_CTRL_1_0104_13 Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_CA_CTRL_1_0104_14 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_CA_CTRL_1_0104_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0108_TSP1 (0x108)
	#define TSP1_REG_ONEWAY_0108_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_ONEWAY_0108_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP1_REG_ONEWAY_0108_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP1_REG_ONEWAY_0108_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP1_REG_ONEWAY_0108_4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP1_REG_ONEWAY_0108_5 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP1_REG_ONEWAY_0108_RESERVED_6 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP1_REG_ONEWAY_0108_RESERVED_7 Fld(1, 7, AC_MSKB0)//[7:7]
#define REG_010C_TSP1 (0x10C)
	#define TSP1_REG_CA_CTRL2_010C_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_CA_CTRL2_010C_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP1_REG_CA_CTRL2_010C_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP1_REG_CA_CTRL2_010C_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP1_REG_CA_CTRL2_010C_4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP1_REG_CA_CTRL2_010C_5 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP1_REG_CA_CTRL2_010C_6 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP1_REG_CA_CTRL2_010C_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP1_REG_CA_CTRL2_010C_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_CA_CTRL2_010C_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_CA_CTRL2_010C_10 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP1_REG_CA_CTRL2_010C_11 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_CA_CTRL2_010C_12 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_CA_CTRL2_010C_13 Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_CA_CTRL2_010C_14 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_CA_CTRL2_010C_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0110_TSP1 (0x110)
	#define TSP1_REG_PCR1_LATCH_0_0110_15_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0114_TSP1 (0x114)
	#define TSP1_REG_PCR1_LATCH_1_0114_31_16 Fld(16, 16, AC_FULLW32)//[31:16]
#define REG_0118_TSP1 (0x118)
	#define TSP1_REG_PCR1_EXT_LATCH_0118_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_PCR1_EXT_LATCH_0118_9_1 Fld(9, 1, AC_MSKW10)//[9:1]
	#define TSP1_REG_PCR1_EXT_LATCH_0118_15_10 Fld(6, 10, AC_MSKB1)//[15:10]
#define REG_0120_TSP1 (0x120)
	#define TSP1_REG_LPCR_CB_BUF_0_0120 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0124_TSP1 (0x124)
	#define TSP1_REG_LPCR_CB_BUF_1_0124 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0128_TSP1 (0x128)
	#define TSP1_REG_STR2MI2_MIU_HEAD_0_0128 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_012C_TSP1 (0x12C)
	#define TSP1_REG_STR2MI2_MIU_HEAD_1_012C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0130_TSP1 (0x130)
	#define TSP1_REG_STR2MI2_MIU_MID_0_0130 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0134_TSP1 (0x134)
	#define TSP1_REG_STR2MI2_MIU_MID_1_0134 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0138_TSP1 (0x138)
	#define TSP1_REG_STR2MI2_MIU_TAIL_0_0138 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_013C_TSP1 (0x13C)
	#define TSP1_REG_STR2MI2_MIU_TAIL_1_013C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0140_TSP1 (0x140)
	#define TSP1_REG_AUD_SRC_0140 Fld(3, 0, AC_MSKB0)//[2:0]
	#define TSP1_REG_AUDB_SRC_0140 Fld(3, 3, AC_MSKB0)//[5:3]
	#define TSP1_REG_VID_SRC_0140 Fld(3, 6, AC_MSKW10)//[8:6]
	#define TSP1_REG_VID3D_SRC_0140 Fld(3, 9, AC_MSKB1)//[11:9]
	#define TSP1_REG_PVRFLT1_SRC_0140 Fld(3, 12, AC_MSKB1)//[14:12]
#define REG_0144_TSP1 (0x144)
	#define TSP1_REG_PCR0_SRC_0144 Fld(3, 2, AC_MSKB0)//[4:2]
	#define TSP1_REG_PCR1_SRC_0144 Fld(3, 5, AC_MSKB0)//[7:5]
	#define TSP1_REG_TEI_SKIP_PKT_PCR0_0144 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_PCR0_RESET_0144 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_PCR0_READ_0144 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_TEI_SKIP_PKT_PCR1_0144 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_PCR1_RESET_0144 Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_PCR1_READ_0144 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0148_TSP1 (0x148)
	#define TSP1_REG_STC_DIFF_0_0148 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_014C_TSP1 (0x14C)
	#define TSP1_REG_STC_DIFF_1_014C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0150_TSP1 (0x150)
	#define TSP1_REG_STC_DIFF_2_0150 Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_0154_TSP1 (0x154)
	#define TSP1_REG_PVRFLT2_SRC_0154 Fld(3, 0, AC_MSKB0)//[2:0]
	#define TSP1_REG_PVRFLT3_SRC_0154 Fld(3, 3, AC_MSKB0)//[5:3]
	#define TSP1_REG_PVRFLT4_SRC_0154 Fld(3, 6, AC_MSKW10)//[8:6]
#define REG_0158_TSP1 (0x158)
	#define TSP1_REG_VQ1_BASE_0_0158 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_015C_TSP1 (0x15C)
	#define TSP1_REG_VQ1_BASE_1_015C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0160_TSP1 (0x160)
	#define TSP1_REG_0160 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0164_TSP1 (0x164)
	#define TSP1_REG_0164 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0168_TSP1 (0x168)
	#define TSP1_REG_0168 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_016C_TSP1 (0x16C)
	#define TSP1_REG_016C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0170_TSP1 (0x170)
	#define TSP1_REG_VQ1_SIZE_192BYTE_0170 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0174_TSP1 (0x174)
	#define TSP1_REG_VQ1_WR_THRESHOLD_0174 Fld(4, 0, AC_MSKB0)//[3:0]
	#define TSP1_REG_VQ1_PRIORITY_THRESHOLD_0174 Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP1_REG_VQ1_FORCEFIRE_CNT_1K_0174 Fld(4, 8, AC_MSKB1)//[11:8]
	#define TSP1_REG_VQ1_RESET_0174 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_VQ1_OVERFLOW_INT_ENABLE_0174 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_VQ1_CLR_OVERFLOW_INT_0174 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0178_TSP1 (0x178)
	#define TSP1_REG_VQ2_BASE_0_0178 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_017C_TSP1 (0x17C)
	#define TSP1_REG_VQ2_BASE_1_017C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0180_TSP1 (0x180)
	#define TSP1_REG_AV_FIFO_ST3_0180_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_AV_FIFO_ST3_0180_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP1_REG_AV_FIFO_ST3_0180_3_2 Fld(2, 2, AC_MSKB0)//[3:2]
	#define TSP1_REG_AV_FIFO_ST3_0180_4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP1_REG_AV_FIFO_ST3_0180_5 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP1_REG_AV_FIFO_ST3_0180_7_6 Fld(2, 6, AC_MSKB0)//[7:6]
	#define TSP1_REG_AV_FIFO_ST3_0180_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_AV_FIFO_ST3_0180_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_AV_FIFO_ST3_0180_10 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP1_REG_AV_FIFO_ST3_0180_11 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_AV_FIFO_ST3_0180_12 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_AV_FIFO_ST3_0180_13 Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_AV_FIFO_ST3_0180_14 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_AV_FIFO_ST3_0180_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0184_TSP1 (0x184)
	#define TSP1_REG_BIST_FAIL_STATUS2_0184 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0188_TSP1 (0x188)
	#define TSP1_REG_BIST_FAIL_STATUS1_0188 Fld(16, 0, AC_FULLW10)//[15:0]

//Page TSP1_TEST_2
#define REG_018C_TSP1 (0x18C)
	#define TSP1_REG_BIST_FAIL_STATUS_018C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0190_TSP1 (0x190)
	#define TSP1_REG_VQ2_SIZE_192BYTE_0190 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0194_TSP1 (0x194)
	#define TSP1_REG_VQ2_WR_THRESHOLD_0194 Fld(4, 0, AC_MSKB0)//[3:0]
	#define TSP1_REG_VQ2_PRIORITY_THRESHOLD_0194 Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP1_REG_VQ2_FORCEFIRE_CNT_1K_0194 Fld(4, 8, AC_MSKB1)//[11:8]
	#define TSP1_REG_VQ2_RESET_0194 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_VQ2_OVERFLOW_INT_ENABLE_0194 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_VQ2_CLR_OVERFLOW_INT_0194 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0198_TSP1 (0x198)
	#define TSP1_REG_VQ_STATUS_0_0198_11_0 Fld(12, 0, AC_MSKW10)//[11:0]
	#define TSP1_REG_VQ_STATUS_0_0198_12 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_VQ_STATUS_0_0198_13 Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_VQ_STATUS_0_0198_14 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_VQ_STATUS_0_0198_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_019C_TSP1 (0x19C)
	#define TSP1_REG_VQ_STATUS_1_019C_16 Fld(1, 16, AC_MSKB2)//[16:16]
	#define TSP1_REG_VQ_STATUS_1_019C_17 Fld(1, 17, AC_MSKB2)//[17:17]
	#define TSP1_REG_VQ_STATUS_1_019C_18 Fld(1, 18, AC_MSKB2)//[18:18]
	#define TSP1_REG_VQ_STATUS_1_019C_19 Fld(1, 19, AC_MSKB2)//[19:19]
	#define TSP1_REG_VQ_STATUS_1_019C_20 Fld(1, 20, AC_MSKB2)//[20:20]
	#define TSP1_REG_VQ_STATUS_1_019C_21 Fld(1, 21, AC_MSKB2)//[21:21]
	#define TSP1_REG_VQ_STATUS_1_019C_22 Fld(1, 22, AC_MSKB2)//[22:22]
	#define TSP1_REG_VQ_STATUS_1_019C_23 Fld(1, 23, AC_MSKB2)//[23:23]
	#define TSP1_REG_VQ_STATUS_1_019C_24 Fld(1, 24, AC_MSKB3)//[24:24]
	#define TSP1_REG_VQ_STATUS_1_019C_25 Fld(1, 25, AC_MSKB3)//[25:25]
	#define TSP1_REG_VQ_STATUS_1_019C_26 Fld(1, 26, AC_MSKB3)//[26:26]
	#define TSP1_REG_VQ_STATUS_1_019C_27 Fld(1, 27, AC_MSKB3)//[27:27]
	#define TSP1_REG_VQ_STATUS_1_019C_28 Fld(1, 28, AC_MSKB3)//[28:28]
	#define TSP1_REG_VQ_STATUS_1_019C_29 Fld(1, 29, AC_MSKB3)//[29:29]
	#define TSP1_REG_VQ_STATUS_1_019C_30 Fld(1, 30, AC_MSKB3)//[30:30]
	#define TSP1_REG_VQ_STATUS_1_019C_31 Fld(1, 31, AC_MSKB3)//[31:31]
#define REG_01A0_TSP1 (0x1A0)
	#define TSP1_REG_DM2MI_WADDR_ERR_0_01A0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01A4_TSP1 (0x1A4)
	#define TSP1_REG_DM2MI_WADDR_ERR_1_01A4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01A8_TSP1 (0x1A8)
	#define TSP1_REG_ORZ_DMAW_WADDR_ERR_0_01A8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01AC_TSP1 (0x1AC)
	#define TSP1_REG_ORZ_DMAW_WADDR_ERR_1_01AC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01B0_TSP1 (0x1B0)
	#define TSP1_REG_INT2_EN_01B0_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_INT2_EN_01B0_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP1_REG_INT2_EN_01B0_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP1_REG_INT2_EN_01B0_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP1_REG_INT2_EN_01B0_4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP1_REG_INT2_EN_01B0_5 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP1_REG_INT2_EN_01B0_6 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP1_REG_INT2_EN_01B0_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP1_REG_INT2_FLAG_01B0_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_INT2_FLAG_01B0_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_INT2_FLAG_01B0_10 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP1_REG_INT2_FLAG_01B0_11 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_INT2_FLAG_01B0_12 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_INT2_FLAG_01B0_13 Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_INT2_FLAG_01B0_14 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_INT2_FLAG_01B0_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_01B4_TSP1 (0x1B4)
	#define TSP1_REG_TSP2_INT_0_01B4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01B8_TSP1 (0x1B8)
	#define TSP1_REG_TSP2_INT_1_01B8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01C0_TSP1 (0x1C0)
	#define TSP1_REG_TIMESTAMP_FILEIN_0_01C0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01C4_TSP1 (0x1C4)
	#define TSP1_REG_TIMESTAMP_FILEIN_1_01C4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01C8_TSP1 (0x1C8)
	#define TSP1_REG_RM_OVERFLOW_GLITCH_01C8 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP1_REG_FILEIN_RADDR_READ_01C8 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP1_REG_DUP_PKT_CNT_CLR_01C8 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP1_REG_RECORD_AT_SYNC_DIS_01C8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_PVR1_ALIGN_EN_01C8 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_RM_PKT_DEMUX_PIPE_01C8 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_VQ_EN_01C8 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_VQ2PINGPONG_EN_01C8 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_01CC_TSP1 (0x1CC)
	#define TSP1_REG_REC_ALL_PVR1_01CC Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP1_REG_REC_ALL_PVR2_01CC Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP1_REG_DMA_FLUSH_EN_01CC Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP1_REG_REC_ALL_OLD_01CC Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP1_REG_RD_ADDR_ALIGN_EN_PVR1_01CC Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP1_REG_RESET_AFIFOC_01CC Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP1_REG_RESET_AFIFOD_01CC Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP1_REG_C27M_EN_TSIF_01CC Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP1_REG_C27M_EN_PVR1_01CC Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP1_REG_C27M_EN_PVR2_01CC Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP1_REG_C27M_EN_PVRCB_01CC Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP1_REG_PS_AUDC_EN_01CC Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_PS_AUDD_EN_01CC Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP1_REG_RD_ADDR_ALIGN_EN_PVR2_01CC Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_01D0_TSP1 (0x1D0)
	#define TSP1_REG_VQ3_BASE_0_01D0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01D4_TSP1 (0x1D4)
	#define TSP1_REG_VQ3_BASE_1_01D4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01D8_TSP1 (0x1D8)
	#define TSP1_REG_VQ3_SIZE_192BYTE_01D8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01DC_TSP1 (0x1DC)
	#define TSP1_REG_VQ3_WR_THRESHOLD_01DC Fld(4, 0, AC_MSKB0)//[3:0]
	#define TSP1_REG_VQ3_PRIORITY_THRESHOLD_01DC Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP1_REG_VQ3_FORCEFIRE_CNT_1K_01DC Fld(4, 8, AC_MSKB1)//[11:8]
	#define TSP1_REG_VQ3_RESET_01DC Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP1_REG_VQ3_OVERFLOW_INT_ENABLE_01DC Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP1_REG_VQ3_CLR_OVERFLOW_INT_01DC Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_01E0_TSP1 (0x1E0)
	#define TSP1_REG_VQ_RX_ARBITER_MODE_01E0 Fld(4, 0, AC_MSKB0)//[3:0]
	#define TSP1_REG_VQ_RX0_PRIORITY_L_01E0 Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP1_REG_VQ_RX1_PRIORITY_L_01E0 Fld(4, 8, AC_MSKB1)//[11:8]
	#define TSP1_REG_VQ_RX2_PRIORITY_L_01E0 Fld(4, 12, AC_MSKB1)//[15:12]
#define REG_01E4_TSP1 (0x1E4)
	#define TSP1_REG_VQ_RX3_PRIORITY_L_01E4 Fld(4, 0, AC_MSKB0)//[3:0]
	#define TSP1_REG_VQ_RX0_PRIORITY_H_01E4 Fld(3, 4, AC_MSKB0)//[6:4]
	#define TSP1_REG_VQ_RX1_PRIORITY_H_01E4 Fld(3, 7, AC_MSKW10)//[9:7]
	#define TSP1_REG_VQ_RX2_PRIORITY_H_01E4 Fld(3, 10, AC_MSKB1)//[12:10]
	#define TSP1_REG_VQ_RX3_PRIORITY_H_01E4 Fld(3, 13, AC_MSKB1)//[15:13]
#define REG_01E8_TSP1 (0x1E8)
	#define TSP1_REG_DUMMY_TSP1_01E8 Fld(7, 0, AC_MSKB0)//[6:0]
#define REG_01F0_TSP1 (0x1F0)
	#define TSP1_REG_MCU_MSG3_0_01F0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01F4_TSP1 (0x1F4)
	#define TSP1_REG_MCU_MSG3_1_01F4 Fld(16, 0, AC_FULLW10)//[15:0]
