/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */
//Page TSP_TLV2_TEST
#define REG_0000_TSP_TLV2 (0x000)
	#define TSP_TLV2_REG_LPCR2_LOAD_0000 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP_TLV2_REG_RESET_RINGBACK_0000 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP_TLV2_REG_TIMER_EN_0000 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP_TLV2_REG_TIMESTAMP_MODE_0000 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP_TLV2_REG_TIMESTAMP_MODE_BLK_DISABLE_0000 \
			Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP_TLV2_REG_LPCR2_WLD_0000 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP_TLV2_REG_LPCR_FREG_27M_90K_0000 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP_TLV2_REG_RM_VLD_EN_0000 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP_TLV2_REG_CHECK_SIZE_MAX_EN_0000 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP_TLV2_REG_FILEIN_DMA_SEL_0000 Fld(2, 9, AC_MSKB1)//[10:9]
#define REG_0004_TSP_TLV2 (0x004)
	#define TSP_TLV2_REG_TIMER_BYTE_0004 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0008_TSP_TLV2 (0x008)
	#define TSP_TLV2_REG_SYNC_BYTE_0008 Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_000C_TSP_TLV2 (0x00C)
	#define TSP_TLV2_REG_CHECK_SIZE_MAX_000C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0010_TSP_TLV2 (0x010)
	#define TSP_TLV2_REG_LPCR_BUF_0_0010 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0014_TSP_TLV2 (0x014)
	#define TSP_TLV2_REG_LPCR_BUF_1_0014 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0018_TSP_TLV2 (0x018)
	#define TSP_TLV2_REG_LPCR_TSIF_0_0018 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_001C_TSP_TLV2 (0x01C)
	#define TSP_TLV2_REG_LPCR_TSIF_1_001C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0020_TSP_TLV2 (0x020)
	#define TSP_TLV2_REG_TIMESTAMP_0_0020 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0024_TSP_TLV2 (0x024)
	#define TSP_TLV2_REG_TIMESTAMP_1_0024 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0028_TSP_TLV2 (0x028)
	#define TSP_TLV2_REG_LOCKED_PKT_CNT_0028 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_002C_TSP_TLV2 (0x02C)
	#define TSP_TLV2_REG_LOCKED_PKT_CNT_LOAD_002C Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP_TLV2_REG_LOCKED_PKT_CNT_CLR_002C Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP_TLV2_REG_UNLOCKED_PKT_CNT_MODE_002C \
			Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP_TLV2_REG_RM_VLD_FLAG_002C Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP_TLV2_REG_CLR_FLAG_002C Fld(1, 4, AC_MSKB0)//[4:4]
#define REG_0040_TSP_TLV2 (0x040)
	#define TSP_TLV2_REG_FI1_LPCR2_LOAD_0040 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP_TLV2_REG_FI1_RESET_RINGBACK_0040 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP_TLV2_REG_FI1_TIMER_EN_0040 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP_TLV2_REG_FI1_TIMESTAMP_MODE_0040 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP_TLV2_REG_FI1_TIMESTAMP_MODE_BLK_DISABLE_0040 \
			Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP_TLV2_REG_FI1_LPCR2_WLD_0040 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP_TLV2_REG_FI1_LPCR_FREG_FI1_27M_90K_0040 \
			Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP_TLV2_REG_FI1_RM_VLD_EN_0040 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP_TLV2_REG_FI1_CHECK_SIZE_MAX_EN_0040 \
			Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP_TLV2_REG_FI1_FILEIN_DMA_SEL_0040 Fld(2, 9, AC_MSKB1)//[10:9]
#define REG_0044_TSP_TLV2 (0x044)
	#define TSP_TLV2_REG_FI1_TIMER_BYTE_0044 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0048_TSP_TLV2 (0x048)
	#define TSP_TLV2_REG_FI1_SYNC_BYTE_0048 Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_004C_TSP_TLV2 (0x04C)
	#define TSP_TLV2_REG_FI1_CHECK_SIZE_MAX_004C \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0050_TSP_TLV2 (0x050)
	#define TSP_TLV2_REG_FI1_LPCR_BUF_0_0050 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0054_TSP_TLV2 (0x054)
	#define TSP_TLV2_REG_FI1_LPCR_BUF_1_0054 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0058_TSP_TLV2 (0x058)
	#define TSP_TLV2_REG_FI1_LPCR_TSIF_0_0058 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_005C_TSP_TLV2 (0x05C)
	#define TSP_TLV2_REG_FI1_LPCR_TSIF_1_005C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0060_TSP_TLV2 (0x060)
	#define TSP_TLV2_REG_FI1_TIMESTAMP_0_0060 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0064_TSP_TLV2 (0x064)
	#define TSP_TLV2_REG_FI1_TIMESTAMP_1_0064 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0068_TSP_TLV2 (0x068)
	#define TSP_TLV2_REG_FI1_LOCKED_PKT_CNT_0068 \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_006C_TSP_TLV2 (0x06C)
	#define TSP_TLV2_REG_FI1_LOCKED_PKT_CNT_LOAD_006C \
			Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP_TLV2_REG_FI1_LOCKED_PKT_CNT_CLR_006C \
			Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP_TLV2_REG_FI1_UNLOCKED_PKT_CNT_MODE_006C \
			Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP_TLV2_REG_FI1_RM_VLD_FLAG_006C Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP_TLV2_REG_FI1_CLR_FLAG_006C Fld(1, 4, AC_MSKB0)//[4:4]
#define REG_0120_TSP_TLV2 (0x120)
	#define TSP_TLV2_REG_VQ_BASE_0_0120 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0124_TSP_TLV2 (0x124)
	#define TSP_TLV2_REG_VQ_BASE_1_0124 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0128_TSP_TLV2 (0x128)
	#define TSP_TLV2_REG_VQ_SIZE_208BYTE_0128 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_012C_TSP_TLV2 (0x12C)
	#define TSP_TLV2_REG_VQ_PRIORITY_THRESHOLD_012C \
			Fld(5, 0, AC_MSKB0)//[4:0]
	#define TSP_TLV2_REG_VQ_RESET_012C Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP_TLV2_REG_VQ_OVERFLOW_INT_EN_012C Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP_TLV2_REG_VQ_CLR_OVERFLOW_INT_012C \
			Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP_TLV2_REG_READ_VQ_EVER_FULL_012C \
			Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP_TLV2_REG_READ_VQ_EVER_OVERFLOW_012C \
			Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP_TLV2_REG_VQ_EMPTY_012C Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP_TLV2_REG_READ_VQ_BUSY_012C Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0130_TSP_TLV2 (0x130)
	#define TSP_TLV2_REG_VQ_STATUS_0130_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP_TLV2_REG_VQ_STATUS_0130_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP_TLV2_REG_VQ_STATUS_0130_3_2 Fld(2, 2, AC_MSKB0)//[3:2]
	#define TSP_TLV2_REG_VQ_STATUS_0130_RESERVED_4 \
			Fld(1, 4, AC_MSKB0)//[4:4]
#define REG_0134_TSP_TLV2 (0x134)
	#define TSP_TLV2_REG_VQ_CONFIG0_0134 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0138_TSP_TLV2 (0x138)
	#define TSP_TLV2_REG_VQ_WR_THRESHOLD_0138 Fld(4, 0, AC_MSKB0)//[3:0]
	#define TSP_TLV2_REG_VQ_FORCEFIRE_CNT_1K_0138 Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP_TLV2_REG_VQ_FORCEFIRE_CNT_1K_EXTEND_0138 \
			Fld(2, 8, AC_MSKB1)//[9:8]
	#define TSP_TLV2_REG_VQ_OVERFLOW_INT_ENABLE_0138 \
			Fld(1, 10, AC_MSKB1)//[10:10]
#define REG_0140_TSP_TLV2 (0x140)
	#define TSP_TLV2_REG_VQ5_BASE_0_0140 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0144_TSP_TLV2 (0x144)
	#define TSP_TLV2_REG_VQ5_BASE_1_0144 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0148_TSP_TLV2 (0x148)
	#define TSP_TLV2_REG_VQ5_SIZE_208BYTE_0148 \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_014C_TSP_TLV2 (0x14C)
	#define TSP_TLV2_REG_VQ5_PRIORITY_THRESHOLD_014C \
			Fld(5, 0, AC_MSKB0)//[4:0]
	#define TSP_TLV2_REG_VQ5_RESET_014C Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP_TLV2_REG_VQ5_OVERFLOW_INT_EN_014C Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP_TLV2_REG_VQ5_CLR_OVERFLOW_INT_014C \
			Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP_TLV2_REG_READ_VQ_EVER_FULL_014C \
			Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP_TLV2_REG_READ_VQ_EVER_OVERFLOW_014C \
			Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP_TLV2_REG_VQ5_EMPTY_014C Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP_TLV2_REG_READ_VQ_BUSY_014C Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0150_TSP_TLV2 (0x150)
	#define TSP_TLV2_REG_VQ5_STATUS_0150_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP_TLV2_REG_VQ5_STATUS_0150_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP_TLV2_REG_VQ5_STATUS_0150_3_2 Fld(2, 2, AC_MSKB0)//[3:2]
	#define TSP_TLV2_REG_VQ5_STATUS_0150_RESERVED_4 \
			Fld(1, 4, AC_MSKB0)//[4:4]
#define REG_0154_TSP_TLV2 (0x154)
	#define TSP_TLV2_REG_VQ5_CONFIG0_0154 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0158_TSP_TLV2 (0x158)
	#define TSP_TLV2_REG_VQ5_WR_THRESHOLD_0158 Fld(4, 0, AC_MSKB0)//[3:0]
	#define TSP_TLV2_REG_VQ5_FORCEFIRE_CNT_1K_0158 \
			Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP_TLV2_REG_VQ5_FORCEFIRE_CNT_1K_EXTEND_0158 \
			Fld(2, 8, AC_MSKB1)//[9:8]
	#define TSP_TLV2_REG_VQ5_OVERFLOW_INT_ENABLE_0158 \
			Fld(1, 10, AC_MSKB1)//[10:10]
#define REG_0160_TSP_TLV2 (0x160)
	#define TSP_TLV2_REG_VQ6_BASE_0_0160 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0164_TSP_TLV2 (0x164)
	#define TSP_TLV2_REG_VQ6_BASE_1_0164 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0168_TSP_TLV2 (0x168)
	#define TSP_TLV2_REG_VQ6_SIZE_208BYTE_0168 \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_016C_TSP_TLV2 (0x16C)
	#define TSP_TLV2_REG_VQ6_PRIORITY_THRESHOLD_016C \
			Fld(5, 0, AC_MSKB0)//[4:0]
	#define TSP_TLV2_REG_VQ6_RESET_016C Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP_TLV2_REG_VQ6_OVERFLOW_INT_EN_016C Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP_TLV2_REG_VQ6_CLR_OVERFLOW_INT_016C \
			Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP_TLV2_REG_READ_VQ_EVER_FULL_016C \
			Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP_TLV2_REG_READ_VQ_EVER_OVERFLOW_016C \
			Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP_TLV2_REG_VQ6_EMPTY_016C Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP_TLV2_REG_READ_VQ_BUSY_016C Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0170_TSP_TLV2 (0x170)
	#define TSP_TLV2_REG_VQ6_STATUS_0170_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP_TLV2_REG_VQ6_STATUS_0170_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP_TLV2_REG_VQ6_STATUS_0170_3_2 Fld(2, 2, AC_MSKB0)//[3:2]
	#define TSP_TLV2_REG_VQ6_STATUS_0170_RESERVED_4 \
			Fld(1, 4, AC_MSKB0)//[4:4]
#define REG_0174_TSP_TLV2 (0x174)
	#define TSP_TLV2_REG_VQ6_CONFIG0_0174 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0178_TSP_TLV2 (0x178)
	#define TSP_TLV2_REG_VQ6_WR_THRESHOLD_0178 Fld(4, 0, AC_MSKB0)//[3:0]
	#define TSP_TLV2_REG_VQ6_FORCEFIRE_CNT_1K_0178 \
			Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP_TLV2_REG_VQ6_FORCEFIRE_CNT_1K_EXTEND_0178 \
			Fld(2, 8, AC_MSKB1)//[9:8]
	#define TSP_TLV2_REG_VQ6_OVERFLOW_INT_ENABLE_0178 \
			Fld(1, 10, AC_MSKB1)//[10:10]
#define REG_0180_TSP_TLV2 (0x180)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR1_0_0180 \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0184_TSP_TLV2 (0x184)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR1_1_0184 \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0188_TSP_TLV2 (0x188)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR1_2_0188 \
			Fld(2, 0, AC_MSKB0)//[1:0]
#define REG_018C_TSP_TLV2 (0x18C)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR2_0_018C \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0190_TSP_TLV2 (0x190)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR2_1_0190 \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0194_TSP_TLV2 (0x194)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR2_2_0194 \
			Fld(2, 0, AC_MSKB0)//[1:0]
#define REG_0198_TSP_TLV2 (0x198)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR3_0_0198 \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_019C_TSP_TLV2 (0x19C)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR3_1_019C \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01A0_TSP_TLV2 (0x1A0)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR3_2_01A0 \
			Fld(2, 0, AC_MSKB0)//[1:0]
#define REG_01A4_TSP_TLV2 (0x1A4)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR4_0_01A4 \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01A8_TSP_TLV2 (0x1A8)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR4_1_01A8 \
			Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01AC_TSP_TLV2 (0x1AC)
	#define TSP_TLV2_REG_TLV_PKT_END_WADR_PVR4_2_01AC \
			Fld(2, 0, AC_MSKB0)//[1:0]

