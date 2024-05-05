/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */
//Page TSP5_TEST_1
#define REG_0000_TSP5 (0x000)
	#define TSP5_REG_ADJ_PERIOD_0000 Fld(4, 0, AC_MSKB0)//[3:0]
#define REG_0004_TSP5 (0x004)
	#define TSP5_REG_ATS_MODE_FI_0004 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_ATS_OFFSET_EN_FI_0004 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_ATS_OFFSET_FI_0004 Fld(5, 8, AC_MSKB1)//[12:8]
#define REG_0008_TSP5 (0x008)
	#define TSP5_REG_TS_IF_FI_EN_0008 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_TS_DATA_FI_SWAP_0008 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_P_SEL_FI_0008 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_EXT_SYNC_SEL_FI_0008 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_MUX_LIVE_FILE_PATH_0008 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_PKT_CHK_SIZE_FI_0008 Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_000C_TSP5 (0x00C)
	#define TSP5_REG_MATCH_PID_SEL_000C Fld(4, 0, AC_MSKB0)//[3:0]
#define REG_0010_TSP5 (0x010)
	#define TSP5_REG_TSIF_TSO_BLK_EN_0010 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_TSIF1_TSO_BLK_EN_0010 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_TSIF2_TSO_BLK_EN_0010 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP5_REG_TSIF_FI_TSO_BLK_EN_0010 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP5_REG_WB_FSM_RESET_0010 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP5_REG_WB_FSM_RESET_FINISHED_0010 Fld(1, 13, AC_MSKB1)//[13:13]
#define REG_0014_TSP5 (0x014)
	#define TSP5_REG_TRACE_MARK_V_EN_0014 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_TRACE_MARK_V3D_EN_0014 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_TRACE_MARK_A_EN_0014 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_TRACE_MARK_AD_EN_0014 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_TRACE_MARK_ADC_EN_0014 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_TRACE_MARK_ADD_EN_0014 Fld(1, 5, AC_MSKB0)//[5:5]
#define REG_0018_TSP5 (0x018)
	#define TSP5_REG_FIX_192_TIMER_0_EN_0018 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_VQ_CLR_0018 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_FILTER_NULL_PKT0_0018 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_FILTER_NULL_PKT1_0018 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_FILTER_NULL_PKT2_0018 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_FILTER_NULL_PKT_FILE_0018 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_FLUSH_PVR1_DATA_0018 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_FLUSH_PVR2_DATA_0018 Fld(1, 9, AC_MSKB1)//[9:9]
#define REG_001C_TSP5 (0x01C)
	#define TSP5_REG_INIT_TIMESTAMP_001C_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_INIT_TIMESTAMP_001C_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_INIT_TIMESTAMP_001C_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_MATCH_CNT_FILEIN_EN_001C Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_MATCH_CNT_THRESHOLD_001C Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP5_REG_INIT_TRUST_SYNC_CNT_VALUE_001C_15_8 Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_0020_TSP5 (0x020)
	#define TSP5_REG_MIU_SEL_FILEIN_0020 Fld(2, 0, AC_MSKB0)//[1:0]
	#define TSP5_REG_MIU_SEL_SECTION_0020 Fld(2, 2, AC_MSKB0)//[3:2]
	#define TSP5_REG_MIU_SEL_MMFILEIN0_0020 Fld(2, 4, AC_MSKB0)//[5:4]
	#define TSP5_REG_MIU_SEL_MMFILEIN1_0020 Fld(2, 6, AC_MSKB0)//[7:6]
	#define TSP5_REG_MIU_SEL_VQ_RW_0020 Fld(2, 8, AC_MSKB1)//[9:8]
	#define TSP5_REG_MIU_SEL_OR_RW_0020 Fld(2, 10, AC_MSKB1)//[11:10]
	#define TSP5_REG_MIU_SEL_FW_0020 Fld(2, 12, AC_MSKB1)//[13:12]
#define REG_0024_TSP5 (0x024)
	#define TSP5_REG_MIU_SEL_PVR1_0024 Fld(2, 0, AC_MSKB0)//[1:0]
	#define TSP5_REG_MIU_SEL_PVR2_0024 Fld(2, 2, AC_MSKB0)//[3:2]
	#define TSP5_REG_MIU_SEL_FIQ0_RW_0024 Fld(2, 8, AC_MSKB1)//[9:8]
	#define TSP5_REG_MIU_SEL_FIQ1_RW_0024 Fld(2, 10, AC_MSKB1)//[11:10]
#define REG_002C_TSP5 (0x02C)
	#define TSP5_REG_FIQ0_MUX_SRC_SEL_002C Fld(3, 0, AC_MSKB0)//[2:0]
#define REG_0030_TSP5 (0x030)
	#define TSP5_REG_FIXED_TIMESTAMP_RING_BACK_EN_0030 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_FIX_LPCR_RING_BACK_EN_0030 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_PVR_WRITE_POINTER_TO_NEXT_ADDRESS_EN_0030 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_NMATCH_DIS_0030 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_SCR_BIT_AFTER_CA_0030 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_ECO_TS_SYNC_OUT_DELAY_0030 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_ECO_TS_SYNC_OUT_REVERSE_BLOCK_0030 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP5_REG_ECO_FIQ_INPUT_0030 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_SECFLT_CTRL_DMA_DIS_0030 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_PKT_CONVERTER_FIRST_SYNC_VALID_MASK_EN_0030 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_ABT_STATUS_0030 Fld(6, 10, AC_MSKB1)//[15:10]
#define REG_0034_TSP5 (0x034)
	#define TSP5_REG_CONFIG_BY_TEE_SEC_PRIVILEGE_0034_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_CONFIG_BY_TEE_SEC_PRIVILEGE_0034_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_CONFIG_BY_TEE_SEC_PRIVILEGE_0034_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_CONFIG_BY_TEE_SEC_PRIVILEGE_0034_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_CONFIG_BY_TEE_SEC_PRIVILEGE_0034_4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_CONFIG_BY_TEE_SEC_PRIVILEGE_0034_5 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_CONFIG_BY_TEE_SEC_PRIVILEGE_0034_6 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP5_REG_CONFIG_BY_TEE_SEC_PRIVILEGE_0034_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_CONFIG_BY_TEE_SEC_PRIVILEGE_0034_15_8 Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_0038_TSP5 (0x038)
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT1_0038_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT1_0038_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT1_0038_7_2 Fld(6, 2, AC_MSKB0)//[7:2]
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT3_0038_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT3_0038_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT3_0038_15_10 Fld(6, 10, AC_MSKB1)//[15:10]
#define REG_003C_TSP5 (0x03C)
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT2_003C_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT2_003C_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT2_003C_7_2 Fld(6, 2, AC_MSKB0)//[7:2]
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT4_003C_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT4_003C_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_CONFIG_BY_TEE_PVRFLT4_003C_15_10 Fld(6, 10, AC_MSKB1)//[15:10]
#define REG_0040_TSP5 (0x040)
	#define TSP5_REG_TEE_TSP_WP_MMFI0_0040 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_TEE_TSP_WP_MMFI1_0040 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_TEE_TSP_WP_FILEIN_0040 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_TEE_TSP_WP_ORZ_0040 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_TEE_TSP_WP_VQ_0040 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_TEE_TSP_WP_FW_0040 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_TEE_TSP_WP_QMEM_0040 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP5_REG_TEE_TSP_WP_TOP_0040 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_TEE_TSP_WP_SEC_PRIVILEGE_0040 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_TEE_TSP_WP_PVR1_0040 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_TEE_TSP_WP_PVR2_0040 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP5_REG_TEE_TSP_WP_FIQ_0040 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP5_REG_TEE_TSP_WP_SEC_0040 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP5_REG_TEE_TSP_WP_PROTECT_MIU_0040 Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP5_REG_TEE_TSP_WP_DSCRMB_0040 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP5_REG_TEE_TSP_WP_SWITCH_0040 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0044_TSP5 (0x044)
	#define TSP5_REG_TEE_TSP_RP_MMFI0_0044 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_TEE_TSP_RP_MMFI1_0044 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_TEE_TSP_RP_FILEIN_0044 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_TEE_TSP_RP_ORZ_0044 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_TEE_TSP_RP_VQ_0044 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_TEE_TSP_RP_FW_0044 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_TEE_TSP_RP_QMEM_0044 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP5_REG_TEE_TSP_RP_TOP_0044 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_TEE_TSP_RP_SEC_PRIVILEGE_0044 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_TEE_TSP_RP_PVR1_0044 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_TEE_TSP_RP_PVR2_0044 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP5_REG_TEE_TSP_RP_FIQ_0044 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP5_REG_TEE_TSP_RP_SEC_0044 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP5_REG_TEE_TSP_RP_PROTECT_MIU_0044 Fld(1, 13, AC_MSKB1)//[13:13]
#define REG_0048_TSP5 (0x048)
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_5 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_6 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_10 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_11 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_12 Fld(1, 12, AC_MSKB1)//[12:12]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_13 Fld(1, 13, AC_MSKB1)//[13:13]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_14 Fld(1, 14, AC_MSKB1)//[14:14]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_15 Fld(1, 15, AC_MSKB1)//[15:15]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_RESERVED_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_RESERVED_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_RESERVED_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_RESERVED_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_RESERVED_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_RESERVED_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_TEE_TSP_MIU_NS_0048_RESERVED_9 Fld(1, 9, AC_MSKB1)//[9:9]
#define REG_004C_TSP5 (0x04C)
	#define TSP5_REG_TSIF0_CONFIG_004C_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_TSIF0_CONFIG_004C_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_TSIF0_CONFIG_004C_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_TSIF0_CONFIG_004C_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_TSIF0_CONFIG_004C_7_4 Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP5_REG_TSIF_FI_CONFIG_004C_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_TSIF_FI_CONFIG_004C_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_TSIF_FI_CONFIG_004C_10 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP5_REG_TSIF_FI_CONFIG_004C_11 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP5_REG_TSIF_FI_CONFIG_004C_15_12 Fld(4, 12, AC_MSKB1)//[15:12]
#define REG_0050_TSP5 (0x050)
	#define TSP5_REG_TSIF1_CONFIG_0050_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_TSIF1_CONFIG_0050_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_TSIF1_CONFIG_0050_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_TSIF1_CONFIG_0050_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_TSIF1_CONFIG_0050_7_4 Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP5_REG_TSIF2_CONFIG_0050_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_TSIF2_CONFIG_0050_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_TSIF2_CONFIG_0050_10 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP5_REG_TSIF2_CONFIG_0050_11 Fld(1, 11, AC_MSKB1)//[11:11]
	#define TSP5_REG_TSIF2_CONFIG_0050_15_12 Fld(4, 12, AC_MSKB1)//[15:12]
#define REG_0054_TSP5 (0x054)
	#define TSP5_REG_TEE_TSP_WP_PVR_SWITCH_0054 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_TEE_TSP_WP_PVR3_0054 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_TEE_TSP_RP_PVR3_0054 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_TEE_TSP_WP_PVR4_0054 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_TEE_TSP_RP_PVR4_0054 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_TEE_TSP_WP_PVR5_0054 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_TEE_TSP_RP_PVR5_0054 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP5_REG_TEE_TSP_WP_PVR6_0054 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_TEE_TSP_RP_PVR6_0054 Fld(1, 8, AC_MSKB1)//[8:8]
#define REG_0060_TSP5 (0x060)
	#define TSP5_REG_CC_PUSI_DROP_SEL_VID_0060_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_CC_PUSI_DROP_SEL_VID_0060_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_CC_PUSI_DROP_SEL_VID_0060_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_CC_PUSI_DROP_SEL_VID_0060_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_CC_PUSI_DROP_SEL_V3D_0060 Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP5_REG_CC_PUSI_DROP_SEL_AUD_0060 Fld(4, 8, AC_MSKB1)//[11:8]
	#define TSP5_REG_CC_PUSI_DROP_SEL_AUDB_0060 Fld(4, 12, AC_MSKB1)//[15:12]
#define REG_0064_TSP5 (0x064)
	#define TSP5_REG_CC_PUSI_DROP_SEL_AUDC_0064 Fld(4, 0, AC_MSKB0)//[3:0]
	#define TSP5_REG_CC_PUSI_DROP_SEL_AUDD_0064 Fld(4, 4, AC_MSKB0)//[7:4]
	#define TSP5_REG_CC_PUSI_REF_PAYLOAD_FLAG_EN_0064 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0080_TSP5 (0x080)
	#define TSP5_REG_FILEIN_DMAR_LBND_0_0080 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0084_TSP5 (0x084)
	#define TSP5_REG_FILEIN_DMAR_LBND_1_0084 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0088_TSP5 (0x088)
	#define TSP5_REG_FILEIN_DMAR_UBND_0_0088 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_008C_TSP5 (0x08C)
	#define TSP5_REG_FILEIN_DMAR_UBND_1_008C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0090_TSP5 (0x090)
	#define TSP5_REG_MMFI0_DMAR_LBND_0_0090 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0094_TSP5 (0x094)
	#define TSP5_REG_MMFI0_DMAR_LBND_1_0094 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0098_TSP5 (0x098)
	#define TSP5_REG_MMFI0_DMAR_UBND_0_0098 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_009C_TSP5 (0x09C)
	#define TSP5_REG_MMFI0_DMAR_UBND_1_009C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00A0_TSP5 (0x0A0)
	#define TSP5_REG_MMFI1_DMAR_LBND_0_00A0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00A4_TSP5 (0x0A4)
	#define TSP5_REG_MMFI1_DMAR_LBND_1_00A4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00A8_TSP5 (0x0A8)
	#define TSP5_REG_MMFI1_DMAR_UBND_0_00A8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00AC_TSP5 (0x0AC)
	#define TSP5_REG_MMFI1_DMAR_UBND_1_00AC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00B0_TSP5 (0x0B0)
	#define TSP5_REG_ORZ_DMAR_LBND_0_00B0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00B4_TSP5 (0x0B4)
	#define TSP5_REG_ORZ_DMAR_LBND_1_00B4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00B8_TSP5 (0x0B8)
	#define TSP5_REG_ORZ_DMAR_UBND_0_00B8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00BC_TSP5 (0x0BC)
	#define TSP5_REG_ORZ_DMAR_UBND_1_00BC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00C0_TSP5 (0x0C0)
	#define TSP5_REG_VQTX0_DMAW_LBND_0_00C0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00C4_TSP5 (0x0C4)
	#define TSP5_REG_VQTX0_DMAW_LBND_1_00C4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00C8_TSP5 (0x0C8)
	#define TSP5_REG_VQTX0_DMAW_UBND_0_00C8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00CC_TSP5 (0x0CC)
	#define TSP5_REG_VQTX0_DMAW_UBND_1_00CC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00D0_TSP5 (0x0D0)
	#define TSP5_REG_VQTX1_DMAW_LBND_0_00D0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00D4_TSP5 (0x0D4)
	#define TSP5_REG_VQTX1_DMAW_LBND_1_00D4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00D8_TSP5 (0x0D8)
	#define TSP5_REG_VQTX1_DMAW_UBND_0_00D8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00DC_TSP5 (0x0DC)
	#define TSP5_REG_VQTX1_DMAW_UBND_1_00DC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00E0_TSP5 (0x0E0)
	#define TSP5_REG_VQTX2_DMAW_LBND_0_00E0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00E4_TSP5 (0x0E4)
	#define TSP5_REG_VQTX2_DMAW_LBND_1_00E4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0100_TSP5 (0x100)
	#define TSP5_REG_VQTX2_DMAW_UBND_0_0100 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0104_TSP5 (0x104)
	#define TSP5_REG_VQTX2_DMAW_UBND_1_0104 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0108_TSP5 (0x108)
	#define TSP5_REG_VQTX3_DMAW_LBND_0_0108 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_010C_TSP5 (0x10C)
	#define TSP5_REG_VQTX3_DMAW_LBND_1_010C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0110_TSP5 (0x110)
	#define TSP5_REG_VQTX3_DMAW_UBND_0_0110 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0114_TSP5 (0x114)
	#define TSP5_REG_VQTX3_DMAW_UBND_1_0114 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0118_TSP5 (0x118)
	#define TSP5_REG_VQRX_DMAR_LBND_0_0118 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_011C_TSP5 (0x11C)
	#define TSP5_REG_VQRX_DMAR_LBND_1_011C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0120_TSP5 (0x120)
	#define TSP5_REG_VQRX_DMAR_UBND_0_0120 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0124_TSP5 (0x124)
	#define TSP5_REG_VQRX_DMAR_UBND_1_0124 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0128_TSP5 (0x128)
	#define TSP5_REG_FIQ0_DMA_LBND_0_0128 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_012C_TSP5 (0x12C)
	#define TSP5_REG_FIQ0_DMA_LBND_1_012C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0130_TSP5 (0x130)
	#define TSP5_REG_FIQ0_DMA_UBND_0_0130 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0134_TSP5 (0x134)
	#define TSP5_REG_FIQ0_DMA_UBND_1_0134 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0138_TSP5 (0x138)
	#define TSP5_REG_FIQ1_DMA_LBND_0_0138 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_013C_TSP5 (0x13C)
	#define TSP5_REG_FIQ1_DMA_LBND_1_013C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0140_TSP5 (0x140)
	#define TSP5_REG_FIQ1_DMA_UBND_0_0140 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0144_TSP5 (0x144)
	#define TSP5_REG_FIQ1_DMA_UBND_1_0144 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0148_TSP5 (0x148)
	#define TSP5_REG_VID_SECURE_FLAG_ENABLE_0148_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_VID_SECURE_FLAG_ENABLE_0148_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_VID_SECURE_FLAG_ENABLE_0148_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_VID_SECURE_FLAG_ENABLE_0148_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_VID_3D_SECURE_FLAG_ENABLE_0148_4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_VID_3D_SECURE_FLAG_ENABLE_0148_5 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_VID_3D_SECURE_FLAG_ENABLE_0148_6 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP5_REG_VID_3D_SECURE_FLAG_ENABLE_0148_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_VQTX_DMAW_PROTECT_EN_0148_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_VQTX_DMAW_PROTECT_EN_0148_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_VQTX_DMAW_PROTECT_EN_0148_10 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP5_REG_VQTX_DMAW_PROTECT_EN_0148_11 Fld(1, 11, AC_MSKB1)//[11:11]
#define REG_014C_TSP5 (0x14C)
	#define TSP5_REG_ATSC30_ENABLE_014C_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_ATSC30_ENABLE_014C_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_ATSC30_ENABLE_014C_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_ATSC30_ENABLE_014C_3 Fld(1, 3, AC_MSKB0)//[3:3]
#define REG_0150_TSP5 (0x150)
	#define TSP5_REG_VQTX0_ATSC30_MAP_PIDFILTER_NUM0_0150_6_0 Fld(7, 0, AC_MSKB0)//[6:0]
	#define TSP5_REG_VQTX0_ATSC30_MAP_PIDFILTER_NUM0_0150_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_VQTX0_ATSC30_MAP_PIDFILTER_NUM1_0150_14_8 Fld(7, 8, AC_MSKB1)//[14:8]
	#define TSP5_REG_VQTX0_ATSC30_MAP_PIDFILTER_NUM1_0150_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0154_TSP5 (0x154)
	#define TSP5_REG_VQTX0_ATSC30_MAP_PIDFILTER_NUM2_0154_6_0 Fld(7, 0, AC_MSKB0)//[6:0]
	#define TSP5_REG_VQTX0_ATSC30_MAP_PIDFILTER_NUM2_0154_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_VQTX0_ATSC30_MAP_PIDFILTER_NUM3_0154_14_8 Fld(7, 8, AC_MSKB1)//[14:8]
	#define TSP5_REG_VQTX0_ATSC30_MAP_PIDFILTER_NUM3_0154_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0158_TSP5 (0x158)
	#define TSP5_REG_VQTX1_ATSC30_MAP_PIDFILTER_NUM0_0158_6_0 Fld(7, 0, AC_MSKB0)//[6:0]
	#define TSP5_REG_VQTX1_ATSC30_MAP_PIDFILTER_NUM0_0158_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_VQTX1_ATSC30_MAP_PIDFILTER_NUM1_0158_14_8 Fld(7, 8, AC_MSKB1)//[14:8]
	#define TSP5_REG_VQTX1_ATSC30_MAP_PIDFILTER_NUM1_0158_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_015C_TSP5 (0x15C)
	#define TSP5_REG_VQTX1_ATSC30_MAP_PIDFILTER_NUM2_015C_6_0 Fld(7, 0, AC_MSKB0)//[6:0]
	#define TSP5_REG_VQTX1_ATSC30_MAP_PIDFILTER_NUM2_015C_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_VQTX1_ATSC30_MAP_PIDFILTER_NUM3_015C_14_8 Fld(7, 8, AC_MSKB1)//[14:8]
	#define TSP5_REG_VQTX1_ATSC30_MAP_PIDFILTER_NUM3_015C_15 Fld(1, 15, AC_MSKB1)//[15:15]

//Page TSP5_TEST_2
#define REG_0160_TSP5 (0x160)
	#define TSP5_REG_VQTX2_ATSC30_MAP_PIDFILTER_NUM0_0160_6_0 Fld(7, 0, AC_MSKB0)//[6:0]
	#define TSP5_REG_VQTX2_ATSC30_MAP_PIDFILTER_NUM0_0160_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_VQTX2_ATSC30_MAP_PIDFILTER_NUM1_0160_14_8 Fld(7, 8, AC_MSKB1)//[14:8]
	#define TSP5_REG_VQTX2_ATSC30_MAP_PIDFILTER_NUM1_0160_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0164_TSP5 (0x164)
	#define TSP5_REG_VQTX2_ATSC30_MAP_PIDFILTER_NUM2_0164_6_0 Fld(7, 0, AC_MSKB0)//[6:0]
	#define TSP5_REG_VQTX2_ATSC30_MAP_PIDFILTER_NUM2_0164_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_VQTX2_ATSC30_MAP_PIDFILTER_NUM3_0164_14_8 Fld(7, 8, AC_MSKB1)//[14:8]
	#define TSP5_REG_VQTX2_ATSC30_MAP_PIDFILTER_NUM3_0164_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0168_TSP5 (0x168)
	#define TSP5_REG_VQTX3_ATSC30_MAP_PIDFILTER_NUM0_0168_6_0 Fld(7, 0, AC_MSKB0)//[6:0]
	#define TSP5_REG_VQTX3_ATSC30_MAP_PIDFILTER_NUM0_0168_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_VQTX3_ATSC30_MAP_PIDFILTER_NUM1_0168_14_8 Fld(7, 8, AC_MSKB1)//[14:8]
	#define TSP5_REG_VQTX3_ATSC30_MAP_PIDFILTER_NUM1_0168_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_016C_TSP5 (0x16C)
	#define TSP5_REG_VQTX3_ATSC30_MAP_PIDFILTER_NUM2_016C_6_0 Fld(7, 0, AC_MSKB0)//[6:0]
	#define TSP5_REG_VQTX3_ATSC30_MAP_PIDFILTER_NUM2_016C_7 Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_VQTX3_ATSC30_MAP_PIDFILTER_NUM3_016C_14_8 Fld(7, 8, AC_MSKB1)//[14:8]
	#define TSP5_REG_VQTX3_ATSC30_MAP_PIDFILTER_NUM3_016C_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_0170_TSP5 (0x170)
	#define TSP5_REG_PRIVILEGE_SYNC_BYTE_PVRFLT3_0170 Fld(8, 0, AC_FULLB0)//[7:0]
	#define TSP5_REG_PRIVILEGE_SYNC_BYTE_PVRFLT4_0170 Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_0174_TSP5 (0x174)
	#define TSP5_REG_SYNC_BYTE_PRIVILEGE_FILE_0174 Fld(8, 0, AC_FULLB0)//[7:0]
	#define TSP5_REG_REPLACE_PRIVILEGE_SYNC_BYTE_FILE_0174 Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_0178_TSP5 (0x178)
	#define TSP5_REG_PRIVILEGE_SYNC_BYTE_PVRFLT1_0178 Fld(8, 0, AC_FULLB0)//[7:0]
	#define TSP5_REG_PRIVILEGE_SYNC_BYTE_PVRFLT2_0178 Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_017C_TSP5 (0x17C)
	#define TSP5_REG_CPU_LOAD_CODE_ONLY_ONE_TIME_BY_TEE_017C Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_CPU_SECURE_STATUS_017C Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_FIXED_DMA_RSTART_ONEWAY_LOAD_FW_017C Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_LEAF_DROP_DATA_ENABLE_017C Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_CHANGE_PRIVILEGE_SYNC_PVRFLT1_017C Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_CHANGE_PRIVILEGE_SYNC_PVRFLT2_017C Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_CHK_PRIVILEGE_SYNC_BYTE_ENABLE_FILEIN_017C Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP5_REG_CHANGE_PRIVILEGE_SYNC_PVRFLT3_017C Fld(1, 7, AC_MSKB0)//[7:7]
	#define TSP5_REG_CHANGE_PRIVILEGE_SYNC_PVRFLT4_017C Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_AV_FIFO_FULL_CONDITION_SEL_017C Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_TSIF_2TSO_FIFO_FULL_CONDITION_SEL_017C Fld(1, 10, AC_MSKB1)//[10:10]
#define REG_0180_TSP5 (0x180)
	#define TSP5_REG_PVR1_AID_SEL_0180 Fld(2, 0, AC_MSKB0)//[1:0]
	#define TSP5_REG_PVR2_AID_SEL_0180 Fld(2, 2, AC_MSKB0)//[3:2]
	#define TSP5_REG_PVR3_AID_SEL_0180 Fld(2, 4, AC_MSKB0)//[5:4]
	#define TSP5_REG_PVR4_AID_SEL_0180 Fld(2, 6, AC_MSKB0)//[7:6]
	#define TSP5_REG_PVR5_AID_SEL_0180 Fld(2, 8, AC_MSKB1)//[9:8]
	#define TSP5_REG_PVR6_AID_SEL_0180 Fld(2, 10, AC_MSKB1)//[11:10]
	#define TSP5_REG_PVR7_AID_SEL_0180 Fld(2, 12, AC_MSKB1)//[13:12]
	#define TSP5_REG_PVR8_AID_SEL_0180 Fld(2, 14, AC_MSKB1)//[15:14]
#define REG_0184_TSP5 (0x184)
	#define TSP5_REG_FILEIN0_AID_SEL_0184 Fld(2, 0, AC_MSKB0)//[1:0]
	#define TSP5_REG_MM0_AID_SEL_0184 Fld(2, 2, AC_MSKB0)//[3:2]
	#define TSP5_REG_MM1_AID_SEL_0184 Fld(2, 4, AC_MSKB0)//[5:4]
	#define TSP5_REG_FILEIN0_VLD_IGNORED_0184 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_MM0_VLD_IGNORED_0184 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_MM1_VLD_IGNORED_0184 Fld(1, 10, AC_MSKB1)//[10:10]
#define REG_018C_TSP5 (0x18C)
	#define TSP5_REG_INIT_TIMESTAMP_VLD_FILE_0_018C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0190_TSP5 (0x190)
	#define TSP5_REG_INIT_TIMESTAMP_VLD_FILE_1_0190 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0194_TSP5 (0x194)
	#define TSP5_REG_INIT_TIMESTAMP_VLD_MMFI0_0_0194 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0198_TSP5 (0x198)
	#define TSP5_REG_INIT_TIMESTAMP_VLD_MMFI0_1_0198 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_019C_TSP5 (0x19C)
	#define TSP5_REG_INIT_TIMESTAMP_VLD_MMFI1_0_019C Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01A0_TSP5 (0x1A0)
	#define TSP5_REG_INIT_TIMESTAMP_VLD_MMFI1_1_01A0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01A4_TSP5 (0x1A4)
	#define TSP5_REG_SECFLT_SRAM_DBG_EN_01A4 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_SECFLT_SRAM_DBG_NM_EN_01A4 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_SECFLT_SRAM_DBG_START_01A4 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_SECFLT_SRAM_DBG_RW_01A4 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_SECFLT_BURST_CNT_MODE_01A4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_SECFLT_BURST_CNT_CLR_01A4 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_SECFLT_ABT_FORCE_LAST_01A4 Fld(1, 6, AC_MSKB0)//[6:6]
	#define TSP5_REG_SECFLT_ABT_FORCE_ACK0_01A4 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_SECFLT_ABT_FORCE_ACK1_01A4 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_SECFLT_ABT_FORCE_ACK2_01A4 Fld(1, 10, AC_MSKB1)//[10:10]
	#define TSP5_REG_SECFLT_ABT_FORCE_ACK3_01A4 Fld(1, 11, AC_MSKB1)//[11:11]
#define REG_01A8_TSP5 (0x1A8)
	#define TSP5_REG_SECFLT_SRAM_DBG_A_01A8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01AC_TSP5 (0x1AC)
	#define TSP5_REG_SECFLT_SRAM_DBG_WD_0_01AC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01B0_TSP5 (0x1B0)
	#define TSP5_REG_SECFLT_SRAM_DBG_WD_1_01B0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01B4_TSP5 (0x1B4)
	#define TSP5_REG_SECFLT_SRAM_DBG_RD_0_01B4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01B8_TSP5 (0x1B8)
	#define TSP5_REG_SECFLT_SRAM_DBG_RD_1_01B8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01C4_TSP5 (0x1C4)
	#define TSP5_REG_PKT_MEET_SIZE_PVR1_L_01C4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01C8_TSP5 (0x1C8)
	#define TSP5_REG_PKT_MEET_SIZE_PVR1_H_01C8 Fld(8, 0, AC_FULLB0)//[7:0]
	#define TSP5_REG_STR2MI_CNT_CLEAR_PVR1_01C8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_CNT_INT_MODE_PVR1_01C8 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_SYNC_INT_MODE_PVR1_01C8 Fld(1, 10, AC_MSKB1)//[10:10]
#define REG_01CC_TSP5 (0x1CC)
	#define TSP5_REG_PKT_MEET_SIZE_PVR2_L_01CC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01D0_TSP5 (0x1D0)
	#define TSP5_REG_PKT_MEET_SIZE_PVR2_H_01D0 Fld(8, 0, AC_FULLB0)//[7:0]
	#define TSP5_REG_STR2MI_CNT_CLEAR_PVR2_01D0 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_CNT_INT_MODE_PVR2_01D0 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_SYNC_INT_MODE_PVR2_01D0 Fld(1, 10, AC_MSKB1)//[10:10]
#define REG_01D4_TSP5 (0x1D4)
	#define TSP5_REG_MEET_SIZE_CNT_R_PVR1_01D4 Fld(8, 0, AC_FULLB0)//[7:0]
	#define TSP5_REG_MEET_SIZE_CNT_R_PVR2_01D4 Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_01D8_TSP5 (0x1D8)
	#define TSP5_REG_DMAW_LBND5_0_01D8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01DC_TSP5 (0x1DC)
	#define TSP5_REG_DMAW_LBND5_1_01DC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01E0_TSP5 (0x1E0)
	#define TSP5_REG_DMAW_UBND5_0_01E0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01E4_TSP5 (0x1E4)
	#define TSP5_REG_DMAW_UBND5_1_01E4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01E8_TSP5 (0x1E8)
	#define TSP5_REG_DMAW_LBND6_0_01E8 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01EC_TSP5 (0x1EC)
	#define TSP5_REG_DMAW_LBND6_1_01EC Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01F0_TSP5 (0x1F0)
	#define TSP5_REG_DMAW_UBND6_0_01F0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01F4_TSP5 (0x1F4)
	#define TSP5_REG_DMAW_UBND6_1_01F4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01F8_TSP5 (0x1F8)
	#define TSP5_REG_VQ_IDLE_CNT_CONFIG_01F8_1_0 Fld(2, 0, AC_MSKB0)//[1:0]
	#define TSP5_REG_VQ_IDLE_CNT_CONFIG_01F8_3_2 Fld(2, 2, AC_MSKB0)//[3:2]
	#define TSP5_REG_VQ_IDLE_CNT_CONFIG_01F8_5_4 Fld(2, 4, AC_MSKB0)//[5:4]
	#define TSP5_REG_VQ_IDLE_CNT_CONFIG_01F8_7_6 Fld(2, 6, AC_MSKB0)//[7:6]
	#define TSP5_REG_VQ_IDLE_CNT_CONFIG_01F8_8 Fld(1, 8, AC_MSKB1)//[8:8]
	#define TSP5_REG_VQ_IDLE_CNT_CONFIG_01F8_9 Fld(1, 9, AC_MSKB1)//[9:9]
	#define TSP5_REG_VQ_IDLE_CNT_CONFIG_01F8_14_10 Fld(5, 10, AC_MSKB1)//[14:10]
	#define TSP5_REG_VQ_IDLE_CNT_CONFIG_01F8_15 Fld(1, 15, AC_MSKB1)//[15:15]
#define REG_01FC_TSP5 (0x1FC)
	#define TSP5_REG_MIU_MCM_CONFIG_01FC_0 Fld(1, 0, AC_MSKB0)//[0:0]
	#define TSP5_REG_MIU_MCM_CONFIG_01FC_1 Fld(1, 1, AC_MSKB0)//[1:1]
	#define TSP5_REG_MIU_MCM_CONFIG_01FC_2 Fld(1, 2, AC_MSKB0)//[2:2]
	#define TSP5_REG_MIU_MCM_CONFIG_01FC_3 Fld(1, 3, AC_MSKB0)//[3:3]
	#define TSP5_REG_MIU_MCM_CONFIG_01FC_4 Fld(1, 4, AC_MSKB0)//[4:4]
	#define TSP5_REG_MIU_MCM_CONFIG_01FC_5 Fld(1, 5, AC_MSKB0)//[5:5]
	#define TSP5_REG_MIU_MCM_CONFIG_01FC_14_6 Fld(9, 6, AC_MSKW10)//[14:6]
	#define TSP5_REG_MIU_MCM_CONFIG_01FC_15 Fld(1, 15, AC_MSKB1)//[15:15]
