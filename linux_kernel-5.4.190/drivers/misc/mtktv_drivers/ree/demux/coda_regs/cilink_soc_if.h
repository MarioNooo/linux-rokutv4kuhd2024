/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */
//Page CILINK_SOC_IF
#define REG_0000_CILINK_SOC_IF (0x0000)
    #define REG_CILINK_SW_RST Fld(1, 0, AC_MSKB0)//[0:0]
    #define REG_CILINK_INT_CLR Fld(1, 1, AC_MSKB0)//[1:1]
#define REG_0004_CILINK_SOC_IF (0x0004)
    #define REG_CILINK_INT_EN Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0008_CILINK_SOC_IF (0x0008)
    #define REG_CILINK_STATUS Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_000C_CILINK_SOC_IF (0x000C)
    #define REG_CILINK_TESTOUT_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0010_CILINK_SOC_IF (0x0010)
    #define REG_CILINK_TESTOUT_1 Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_0014_CILINK_SOC_IF (0x0014)
    #define REG_C1_TS_SRC_SEL Fld(2, 0, AC_MSKB0)//[1:0]
    #define REG_C1_2B_LOOPBACK Fld(1, 2, AC_MSKB0)//[2:2]
    #define REG_C1_TX_TS_SWAP Fld(1, 3, AC_MSKB0)//[3:3]
    #define REG_C1_RX_2B_SWAP Fld(1, 4, AC_MSKB0)//[4:4]
    #define REG_C1_FULL_TO_TSO_EN Fld(1, 5, AC_MSKB0)//[5:5]
    #define REG_C1_TS_GENERATOR_START Fld(1, 6, AC_MSKB0)//[6:6]
    #define REG_C1_TS_PAYLOADPP_DETECT_START Fld(1, 7, AC_MSKB0)//[7:7]
#define REG_0018_CILINK_SOC_IF (0x0018)
    #define REG_C1_PKT_LEN Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_001C_CILINK_SOC_IF (0x001C)
    #define REG_C1_TS_GEN_PID Fld(13, 0, AC_MSKW10)//[12:0]
#define REG_0020_CILINK_SOC_IF (0x0020)
    #define REG_C2_TS_SRC_SEL Fld(2, 0, AC_MSKB0)//[1:0]
    #define REG_C2_2B_LOOPBACK Fld(1, 2, AC_MSKB0)//[2:2]
    #define REG_C2_TX_TS_SWAP Fld(1, 3, AC_MSKB0)//[3:3]
    #define REG_C2_RX_2B_SWAP Fld(1, 4, AC_MSKB0)//[4:4]
    #define REG_C2_FULL_TO_TSO_EN Fld(1, 5, AC_MSKB0)//[5:5]
    #define REG_C2_TS_GENERATOR_START Fld(1, 6, AC_MSKB0)//[6:6]
    #define REG_C2_TS_PAYLOADPP_DETECT_START Fld(1, 7, AC_MSKB0)//[7:7]
#define REG_0024_CILINK_SOC_IF (0x0024)
    #define REG_C2_PKT_LEN Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_0028_CILINK_SOC_IF (0x0028)
    #define REG_C2_TS_GEN_PID Fld(13, 0, AC_MSKW10)//[12:0]
#define REG_002C_CILINK_SOC_IF (0x002C)
    #define REG_C1_TS_IN_ERROR_BYTECNT Fld(8, 0, AC_FULLB0)//[7:0]
    #define REG_C1_TS_OUT_ERROR_BYTECNT Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_0030_CILINK_SOC_IF (0x0030)
    #define REG_C2_TS_IN_ERROR_BYTECNT Fld(8, 0, AC_FULLB0)//[7:0]
    #define REG_C2_TS_OUT_ERROR_BYTECNT Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_0034_CILINK_SOC_IF (0x0034)
    #define REG_CILINK_TESTOUT_SEL Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_0038_CILINK_SOC_IF (0x0038)
    #define REG_CILINK_C1_SW_RST Fld(1, 0, AC_MSKB0)//[0:0]
    #define REG_CILINK_C2_SW_RST Fld(1, 1, AC_MSKB0)//[1:1]
#define REG_003C_CILINK_SOC_IF (0x003C)
    #define REG_STUFFING_BYTE Fld(8, 0, AC_FULLB0)//[7:0]
    #define REG_STUFFING_INV Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_0040_CILINK_SOC_IF (0x0040)
    #define REG_DC_BALANCE_EN Fld(1, 0, AC_MSKB0)//[0:0]
    #define REG_COMPA_RST Fld(1, 1, AC_MSKB0)//[1:1]
    #define REG_PCM_OR_CD12_EN Fld(1, 2, AC_MSKB0)//[2:2]
    #define REG_CMP_INT_PATCH_MODE Fld(1, 3, AC_MSKB0)//[3:3]
#define REG_0044_CILINK_SOC_IF (0x0044)
    #define REG_C1_S2P_P_MODE Fld(1, 0, AC_MSKB0)//[0:0]
    #define REG_C1_S2P_EXT_SYNC_SEL Fld(1, 1, AC_MSKB0)//[1:1]
    #define REG_C1_S2P_SERIAL_EXT_SYNC_1T Fld(1, 2, AC_MSKB0)//[2:2]
    #define REG_C1_S2P_SIN_C0 Fld(1, 3, AC_MSKB0)//[3:3]
    #define REG_C1_S2P_SIN_C1 Fld(1, 4, AC_MSKB0)//[4:4]
    #define REG_C1_S2P_DIS_LOCLED_PKT_CNT Fld(1, 5, AC_MSKB0)//[5:5]
    #define REG_C1_S2P_CLR_LOCKED_PKT_CNT Fld(1, 6, AC_MSKB0)//[6:6]
    #define REG_C1_S2P_FORCE_SYNC_OUT_47 Fld(1, 7, AC_MSKB0)//[7:7]
#define REG_0048_CILINK_SOC_IF (0x0048)
    #define REG_C1_S2P_SYNC_IN Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_004C_CILINK_SOC_IF (0x004C)
    #define REG_C1_S2P_PKT_CNT_DBG Fld(8, 0, AC_FULLB0)//[7:0]
    #define REG_C1_S2P_LOCKED_PKT_CNT Fld(4, 8, AC_MSKB1)//[11:8]
#define REG_0050_CILINK_SOC_IF (0x0050)
    #define REG_C2_S2P_P_MODE Fld(1, 0, AC_MSKB0)//[0:0]
    #define REG_C2_S2P_EXT_SYNC_SEL Fld(1, 1, AC_MSKB0)//[1:1]
    #define REG_C2_S2P_SERIAL_EXT_SYNC_1T Fld(1, 2, AC_MSKB0)//[2:2]
    #define REG_C2_S2P_SIN_C0 Fld(1, 3, AC_MSKB0)//[3:3]
    #define REG_C2_S2P_SIN_C1 Fld(1, 4, AC_MSKB0)//[4:4]
    #define REG_C2_S2P_DIS_LOCLED_PKT_CNT Fld(1, 5, AC_MSKB0)//[5:5]
    #define REG_C2_S2P_CLR_LOCKED_PKT_CNT Fld(1, 6, AC_MSKB0)//[6:6]
    #define REG_C2_S2P_FORCE_SYNC_OUT_47 Fld(1, 7, AC_MSKB0)//[7:7]
#define REG_0054_CILINK_SOC_IF (0x0054)
    #define REG_C2_S2P_SYNC_IN Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_0058_CILINK_SOC_IF (0x0058)
    #define REG_C2_S2P_PKT_CNT_DBG Fld(8, 0, AC_FULLB0)//[7:0]
    #define REG_C2_S2P_LOCKED_PKT_CNT Fld(4, 8, AC_MSKB1)//[11:8]
#define REG_005C_CILINK_SOC_IF (0x005C)
    #define REG_STAT_INT_RAW Fld(8, 0, AC_FULLB0)//[7:0]
#define REG_0060_CILINK_SOC_IF (0x0060)
    #define REG_STAT_INT Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_0064_CILINK_SOC_IF (0x0064)
    #define REG_PCM_INT_MASK Fld(8, 0, AC_FULLB0)//[7:0]
    #define REG_PCM_INT_CLR Fld(8, 8, AC_FULLB1)//[15:8]
#define REG_0068_CILINK_SOC_IF (0x0068)
    #define REG_PCM_INT_FORCE Fld(8, 0, AC_FULLB0)//[7:0]

