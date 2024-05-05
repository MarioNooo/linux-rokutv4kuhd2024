/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef MHAL_MVD_PARSER_H
#define MHAL_MVD_PARSER_H

#define MAX_HAL_VPAS_NUM	  2
#define ALLOC_CTX

typedef enum {
	E_VPAS1_DEV	   = 0,
	E_VPAS2_DEV	   = 1,
} VPAS_DevID;

#if 0
typedef enum {
	E_AID_NS = 0,     //00
	E_AID_SDC = 1,    //01
	E_AID_S = 2,      //10
	E_AID_CSP = 3,    //11
	E_AID_NUM = 4,
} EN_AID_TYPE;
#endif

#ifndef BIT
#define BIT(_bit_)					(1 << (_bit_))
#endif

#define PAS_LOG_LEVEL_ERROR			BIT(0)
#define PAS_LOG_LEVEL_WARN			BIT(1)
#define PAS_LOG_LEVEL_INFO			BIT(2)
#define PAS_LOG_LEVEL_DEBUG			BIT(3)

#define VPAS_DBG_CTRL 0xe0
#define VPAS_PAS_HK_ID_FILTER 0x0FE0
#define VPAS_VBUF_CTRL_MASK 0x30
#define VPAS_VBUF_CTRL_ID_SHIFT 4
#define VPAS_VBUF_CTRL_ID_MASK 0x3
#define VPAS_VBUF_HI_SHIFT 16
#define VPAS_PTS_HI_SHIFT 32
#define VPAS_ES_CNT_HI_MASK 0x3FF
#define VPAS_ES_CNT_HI_SHIFT 16
#define VPAS_DISABLE_PAUSE   1

extern int pas_log_level;
#define PAS_DEBUG_LOG(_dbgLevel_, _fmt, _args...) { \
	if ((_dbgLevel_ & pas_log_level) != 0) { \
		if (_dbgLevel_ & PAS_LOG_LEVEL_ERROR) { \
			pr_err("<mdbgerr_vpas> %s,%u "_fmt, __func__, __LINE__, ##_args); \
		} \
		else if (_dbgLevel_ & PAS_LOG_LEVEL_WARN) { \
			pr_err("<mdbgwrn_vpas> %s,%u "_fmt, __func__, __LINE__, ##_args); \
		} \
		else if (_dbgLevel_ & PAS_LOG_LEVEL_INFO) { \
			pr_err("<mdbginfo_vpas> %s,%u "_fmt, __func__, __LINE__, ##_args); \
		} \
		else if (_dbgLevel_ & PAS_LOG_LEVEL_DEBUG) { \
			pr_err("<mdbgdbg_vpas> %s,%u "_fmt, __func__, __LINE__, ##_args); \
		} \
	} \
}

void HAL_VPAS_SetStart(u8 u8ParserID);
void HAL_VPAS_SetBypass(u8 u8ParserID, u32 enable);
void HAL_VPAS_SetTLV(u8 u8ParserID, u32 enable);
void HAL_VPAS_SetStop(u8 u8ParserID);
void HAL_VPAS_SetPauseMode(u8 u8ParserID, u32 enable);
void HAL_VPAS_SetEsOffset(u8 u8ParserID, u32 u32EsRAddr);
void HAL_VPAS_SetReset(u8 u8ParserID, u64 u32ParserBufAddr, u32 u32ParserBufSize);
void HAL_VPAS_SetBuf(u8 u8ParserID, u64 u32ParserBufAddr, u32 u32ParserBufSize);
bool HAL_VPAS_InitVar(u8 u8ParserID);
void HAL_VPAS_GetWritePointer(u8 u8ParserID, u32 *param);
u8 HAL_VPAS_Sel_Interrupt(void);
u64 HAL_VPAS_ReadPTS(u8 u8ParserID);
bool HAL_VPAS_RegSetBase(void);
void HAL_VPAS_CLR_Interrupt(u8 u8ParserID);
void HAL_VPAS_GetByteCount(u8 u8ParserID, u32 *param);
void HAL_VPAS_SetAid(u8 u8ParserID, u32 *param);
void HAL_VPAS_Exit(u8 u8ParserID);
bool HAL_VPAS_IsPauseOn(u8 u8ParserID);
//void HAL_VPAS_SetLogLevel(u32 u32PasLogLevel);
//u32 HAL_VPAS_GetLogLevel(void);

#endif /* MHAL_MVD_PARSER_H */
