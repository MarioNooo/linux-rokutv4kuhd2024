/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef _REG_ALP_REG_H_
#define _REG_ALP_REG_H_

//-------------------------------------------------------------------------------------------------
//	Macro and Define
//-------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Base Address
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Reg
//------------------------------------------------------------------------------
#define REG_0034_TSP_ALP (0x0034)
	#define REG_RECORD_ALL_ALP_FORMAT Fld(1, 11, AC_MSKB1)//[11:11]
	#define REG_RECORD_TIMESTAMP_KEYWORD_EN Fld(1, 12, AC_MSKB1)//[12:12]

#define REG_0050_TSP_ALP (0x0050)
	#define REG_KEY_WORD2 Fld(8, 0, AC_FULLB0)//[7:0]
	#define REG_KEY_WORD3 Fld(8, 8, AC_FULLB1)//[15:8]

#define REG_006C_TSP_ALP (0x006C)
	#define REG_ALP_PROCESS_ENABLE Fld(1, 0, AC_MSKB0)//[0:0]
	#define REG_ALP_PROCESS_EVER_OVERFLOW_FLAG Fld(1, 1, AC_MSKB0)//[1:1]
	#define REG_CLR_ALP_PROCESS_EVER_OVERFLOW Fld(1, 2, AC_MSKB0)//[2:2]
	#define REG_INSERT_KEY_WORD_SEL Fld(3, 3, AC_MSKB0)//[5:3]
		#define REG_INSERT_KEY_WORD_SEL_NO_INSERT (0x0) //(0b000)
		#define REG_INSERT_KEY_WORD_SEL_TIMESTAMP_4B (0x1) //(0b001)
		#define REG_INSERT_KEY_WORD_SEL_KEY_WORD_1B_TIMESTAMP_4B (0x3) //(0b011)
		#define REG_INSERT_KEY_WORD_SEL_KEY_WORD_4B (0x6) //(0b110)
		#define REG_INSERT_KEY_WORD_SEL_KEY_WORD_4B_TIMESTAMP_4B (0x7) //(0b111)
	#define REG_CLR_PACKET_NOT_COMPLETE Fld(1, 6, AC_MSKB0)//[6:6]
	#define REG_BYTE_CNT_OUT_NOT_COMPLETE Fld(8, 8, AC_FULLB1)//[15:8]

#define REG_0070_TSP_ALP (0x0070)
	#define REG_KEY_WORD0 Fld(8, 0, AC_FULLB0)//[7:0]
	#define REG_KEY_WORD1 Fld(8, 8, AC_FULLB1)//[15:8]

#define REG_0080_TSP_ALP (0x0080)
	#define REG_FMR_ENABLE_IN Fld(1, 0, AC_MSKB0)//[0:0]
	#define REG_ALP_IP_ENABLE Fld(1, 1, AC_MSKB0)//[1:1]
	#define REG_ALP_ENABLE Fld(1, 2, AC_MSKB0)//[2:2]
	#define REG_ALP_MPEG2_ENABLE Fld(1, 3, AC_MSKB0)//[3:3]
	#define REG_ALP_PTP_AUTO_OVER_ENABLE Fld(1, 4, AC_MSKB0)//[4:4]
	#define REG_ECO_CFG Fld(1, 5, AC_MSKB0)//[5:5]

#define REG_0084_TSP_ALP (0x0084)
	#define REG_FMR_CONFIG0_IN_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0088_TSP_ALP (0x0088)
	#define REG_FMR_CONFIG0_IN_1 Fld(16, 0, AC_FULLW10)//Fld(3,0,AC_MSKB0)//[2:0]

#define REG_00C0_TSP_ALP (0x00C0)
	#define REG_IP_SOURCE_ADDR_MMTP_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00C4_TSP_ALP (0x00C4)
	#define REG_IP_SOURCE_ADDR_MMTP_1 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00C8_TSP_ALP (0x00C8)
	#define REG_IP_SOURCE_ADDR_MMTP_2 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00CC_TSP_ALP (0x00CC)
	#define REG_IP_SOURCE_ADDR_MMTP_3 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00D0_TSP_ALP (0x00D0)
	#define REG_IP_SOURCE_ADDR_MMTP_4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00D4_TSP_ALP (0x00D4)
	#define REG_IP_SOURCE_ADDR_MMTP_5 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00D8_TSP_ALP (0x00D8)
	#define REG_IP_SOURCE_ADDR_MMTP_6 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00DC_TSP_ALP (0x00DC)
	#define REG_IP_SOURCE_ADDR_MMTP_7 Fld(16, 0, AC_FULLW10)//[15:0]

#define REG_00E0_TSP_ALP (0x00E0)
	#define REG_IP_DES_ADDR_MMTP_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00E4_TSP_ALP (0x00E4)
	#define REG_IP_DES_ADDR_MMTP_1 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00E8_TSP_ALP (0x00E8)
	#define REG_IP_DES_ADDR_MMTP_2 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00EC_TSP_ALP (0x00EC)
	#define REG_IP_DES_ADDR_MMTP_3 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00F0_TSP_ALP (0x00F0)
	#define REG_IP_DES_ADDR_MMTP_4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00F4_TSP_ALP (0x00F4)
	#define REG_IP_DES_ADDR_MMTP_5 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00F8_TSP_ALP (0x00F8)
	#define REG_IP_DES_ADDR_MMTP_6 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_00FC_TSP_ALP (0x00FC)
	#define REG_IP_DES_ADDR_MMTP_7 Fld(16, 0, AC_FULLW10)//[15:0]

#define REG_0100_TSP_ALP (0x0100)
	#define REG_UDP_PORT_MMTP_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0104_TSP_ALP (0x0104)
	#define REG_UDP_PORT_MMTP_1 Fld(16, 0, AC_FULLW10)//[15:0]

#define REG_0108_TSP_ALP (0x0108)
	#define REG_UDP_PORT_MMTP1_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_010C_TSP_ALP (0x010C)
	#define REG_UDP_PORT_MMTP1_1 Fld(16, 0, AC_FULLW10)//[15:0]

#define REG_0140_TSP_ALP (0x0140)
	#define REG_IP_SOURCE_ADDR_MMTP1_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0144_TSP_ALP (0x0144)
	#define REG_IP_SOURCE_ADDR_MMTP1_1 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0148_TSP_ALP (0x0148)
	#define REG_IP_SOURCE_ADDR_MMTP1_2 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_014C_TSP_ALP (0x014C)
	#define REG_IP_SOURCE_ADDR_MMTP1_3 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0150_TSP_ALP (0x0150)
	#define REG_IP_SOURCE_ADDR_MMTP1_4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0154_TSP_ALP (0x0154)
	#define REG_IP_SOURCE_ADDR_MMTP1_5 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0158_TSP_ALP (0x0158)
	#define REG_IP_SOURCE_ADDR_MMTP1_6 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_015C_TSP_ALP (0x015C)
	#define REG_IP_SOURCE_ADDR_MMTP1_7 Fld(16, 0, AC_FULLW10)//[15:0]

#define REG_0160_TSP_ALP (0x0160)
	#define REG_IP_DES_ADDR_MMTP1_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0164_TSP_ALP (0x0164)
	#define REG_IP_DES_ADDR_MMTP1_1 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0168_TSP_ALP (0x0168)
	#define REG_IP_DES_ADDR_MMTP1_2 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_016C_TSP_ALP (0x016C)
	#define REG_IP_DES_ADDR_MMTP1_3 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0170_TSP_ALP (0x0170)
	#define REG_IP_DES_ADDR_MMTP1_4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0174_TSP_ALP (0x0174)
	#define REG_IP_DES_ADDR_MMTP1_5 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0178_TSP_ALP (0x0178)
	#define REG_IP_DES_ADDR_MMTP1_6 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_017C_TSP_ALP (0x017C)
	#define REG_IP_DES_ADDR_MMTP1_7 Fld(16, 0, AC_FULLW10)//[15:0]

#define REG_0180_TSP_ALP (0x0180)
	#define REG_IP_SOURCE_ADDR_NTP_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0184_TSP_ALP (0x0184)
	#define REG_IP_SOURCE_ADDR_NTP_1 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0188_TSP_ALP (0x0188)
	#define REG_IP_SOURCE_ADDR_NTP_2 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_018C_TSP_ALP (0x018C)
	#define REG_IP_SOURCE_ADDR_NTP_3 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0190_TSP_ALP (0x0190)
	#define REG_IP_SOURCE_ADDR_NTP_4 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0194_TSP_ALP (0x0194)
	#define REG_IP_SOURCE_ADDR_NTP_5 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_0198_TSP_ALP (0x0198)
	#define REG_IP_SOURCE_ADDR_NTP_6 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_019C_TSP_ALP (0x019C)
	#define REG_IP_SOURCE_ADDR_NTP_7 Fld(16, 0, AC_FULLW10)//[15:0]

#define REG_01A0_TSP_ALP (0x01A0)
	#define REG_IP_DES_ADDR_NTP_0 Fld(16, 0, AC_FULLW10)//[15:0]
	#define REG_PLP_ID4 Fld(6, 0, AC_MSKB0)//[5:0]
	#define REG_PLP4_VLD Fld(1, 6, AC_MSKB0)//[6:6]
	#define REG_PLP_ID3 Fld(6, 8, AC_MSKB1)//[13:8]
	#define REG_PLP3_VLD Fld(1, 14, AC_MSKB1)//[14:14]
#define REG_01A4_TSP_ALP (0x01A4)
	#define REG_IP_DES_ADDR_NTP_1 Fld(16, 0, AC_FULLW10)//[15:0]
	#define REG_PLP_ID2 Fld(6, 0, AC_MSKB0)//[5:0]
	#define REG_PLP2_VLD Fld(1, 6, AC_MSKB0)//[6:6]
	#define REG_PLP_ID1 Fld(6, 8, AC_MSKB1)//[13:8]
	#define REG_PLP1_VLD Fld(1, 14, AC_MSKB1)//[14:14]
	#define REG_SINGLE_PLP_MODE Fld(1, 15, AC_MSKB1)//[15:15]

#define REG_01A8_TSP_ALP (0x01A8)
	#define REG_FMR_TLV_CID01_0 Fld(16, 0, AC_FULLW10)//[15:0]
	#define REG_CID Fld(8, 0, AC_MSKB0)//[7:0]
	#define REG_SID Fld(8, 8, AC_MSKB1)//[15:8]

#define REG_01AC_TSP_ALP (0x01AC)
	#define REG_FMR_TLV_CID01_1 Fld(16, 0, AC_FULLW10)//[15:0]
	#define REG_PLP_ID Fld(6, 0, AC_MSKB0)//[5:0]
	#define REG_COMP_FLAG Fld(1, 8, AC_MSKB1)//[8:8]
	#define REG_SID_FLAG Fld(1, 9, AC_MSKB1)//[9:9]
	#define REG_PLP_MASK Fld(1, 10, AC_MSKB1)//[10:10]
	#define REG_FILTER_ID Fld(5, 11, AC_MSKB1)//[15:11]

#define REG_01B0_TSP_ALP (0x01B0)
	#define REG_FMR_TLV_CID23_0 Fld(16, 0, AC_FULLW10)//[15:0]
	#define REG_MTK_ALP_I Fld(8, 0, AC_MSKB0)//[7:0]
	#define REG_MTK_ALP_I_TOGGLE Fld(1, 9, AC_MSKB1)//[9:9]

#define REG_01B4_TSP_ALP (0x01B4)
	#define REG_FMR_TLV_CID23_1 Fld(16, 0, AC_FULLW10)//[15:0]

#define REG_01B8_TSP_ALP (0x01B8)
	#define REG_FMR_TLV_CID45_0 Fld(16, 0, AC_FULLW10)//[15:0]
#define REG_01BC_TSP_ALP (0x01BC)
	#define REG_FMR_TLV_CID45_1 Fld(16, 0, AC_FULLW10)//[15:0]

#define REG_01C0_TSP_ALP (0x01C0)
	#define REG_MTK_ALP_CONFIG0 Fld(16, 0, AC_FULLW10)//[15:0]
	#define REG_MTK_ALP_CONFIG0_RESET_N_IN Fld(3, 0, AC_MSKB0)//[2:0]

#define REG_01C4_TSP_ALP (0x01C4)
	#define REG_MTK_ALP_CONFIG1 Fld(16, 0, AC_FULLW10)//[15:0]
	#define REG_MTK_ALP_CONFIG1_SELECT_MTK_ALP_TOP_OUTPUT Fld(1, 1, AC_MSKB0)//[1:1]
	#define REG_MTK_ALP_CONFIG1_SIDEBAND_EN Fld(1, 2, AC_MSKB0)//[2:2]

#define REG_01C8_TSP_ALP (0x01C8)
	#define REG_MTK_ALP_CONFIG2 Fld(16, 0, AC_FULLW10)//[15:0]
	#define REG_MTK_ALP_CONFIG2_ALP_FILEIN_ENABLE Fld(1, 1, AC_MSKB0)//[1:1]
	#define REG_MTK_ALP_CONFIG2_ALP_FILEIN_TOP Fld(2, 4, AC_MSKB0)//[5:4]
		#define REG_MTK_ALP_CONFIG2_ALP_FILEIN_TOP_FROM_FILEIN (0x01)
		#define REG_MTK_ALP_CONFIG2_ALP_FILEIN_TOP_FROM_MMFI0 (0x10)
		#define REG_MTK_ALP_CONFIG2_ALP_FILEIN_TOP_FROM_MMFI1 (0x11)

#define REG_01CC_TSP_ALP (0x01CC)
	#define REG_MTK_ALP_CONFIG3 Fld(16, 0, AC_FULLW10)//[15:0]
	#define REG_MTK_ALP_CONFIG3_MTK_ALP_TOP Fld(1, 0, AC_MSKB0)//[0:0]
		#define REG_MTK_ALP_CONFIG3_MTK_ALP_TOP_FROM_TSIF_ALP (0x00)
		#define REG_MTK_ALP_CONFIG3_MTK_ALP_TOP_FROM_ALP_FILEIN_TOP (0x01)

// defined in ts_sample.h
//#define REG_0144_TS_SAMPLE (0x0144)
	#define REG_CKG_TS_ALP0 Fld(8, 0, AC_FULLB0)//[7:0]
	#define REG_CKG_TS_ALP0_DISABLE_CLOCK Fld(1, 0, AC_MSKB0)//[0:0]
	#define REG_CKG_TS_ALP0_INVERT_CLOCK Fld(1, 1, AC_MSKB0)//[1:1]
	#define REG_CKG_TS_ALP0_CLOCK_SOURCE Fld(4, 2, AC_MSKB0)//[5:2]
	#define REG_CKG_TS_ALP1 Fld(8, 8, AC_FULLB1)//[15:8]
	#define REG_CKG_TS_ALP1_DISABLE_CLOCK Fld(1, 8, AC_MSKB1)//[8:8]
	#define REG_CKG_TS_ALP1_INVERT_CLOCK Fld(1, 9, AC_MSKB1)//[9:9]
	#define REG_CKG_TS_ALP1_CLOCK_SOURCE Fld(4, 10, AC_MSKB1)//[13:10]
		#define REG_CKG_TS_ALP1_CLOCK_SOURCE_TS0		(0x0000)
		#define REG_CKG_TS_ALP1_CLOCK_SOURCE_TS1		(0x0001)
		#define REG_CKG_TS_ALP1_CLOCK_SOURCE_TS2		(0x0002)
		#define REG_CKG_TS_ALP1_CLOCK_SOURCE_TS3		(0x0003)
		#define REG_CKG_TS_ALP1_CLOCK_SOURCE_TS4		(0x0004)
		#define REG_CKG_TS_ALP1_CLOCK_SOURCE_TS5		(0x0005)
		#define REG_CKG_TS_ALP1_CLOCK_SOURCE_TSO		(0x000A)
		#define REG_CKG_TS_ALP1_CLOCK_SOURCE_CILINK1	(0x000C)
		#define REG_CKG_TS_ALP1_CLOCK_SOURCE_CILINK2	(0x000D)
		#define REG_CKG_TS_ALP1_CLOCK_SOURCE_DEMOD		(0x000F)

// defined in ts_sample.h
//#define REG_0148_TS_SAMPLE (0x0148)
	#define REG_CKG_TS_ALP2 Fld(8, 0, AC_FULLB0)//[7:0]
	#define REG_CKG_TS_ALP2_DISABLE_CLOCK Fld(1, 0, AC_MSKB0)//[0:0]
	#define REG_CKG_TS_ALP2_INVERT_CLOCK Fld(1, 1, AC_MSKB0)//[1:1]
	#define REG_CKG_TS_ALP2_CLOCK_SOURCE Fld(4, 2, AC_MSKB0)//[5:2]
	#define REG_CKG_TS_ALP_FI Fld(8, 8, AC_FULLB1)//[15:8]
	#define REG_CKG_TS_ALP_FI_DISABLE_CLOCK Fld(1, 8, AC_MSKB1)//[8:8]
	#define REG_CKG_TS_ALP_FI_INVERT_CLOCK Fld(1, 9, AC_MSKB1)//[9:9]
	#define REG_CKG_TS_ALP_FI_CLOCK_SOURCE Fld(4, 10, AC_MSKB1)//[13:10]

#endif // _REG_ALP_REG_H_