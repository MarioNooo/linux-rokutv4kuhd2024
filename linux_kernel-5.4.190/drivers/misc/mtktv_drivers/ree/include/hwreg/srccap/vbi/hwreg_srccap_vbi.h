/*
 *
 * Copyright (c) 2020-2024 MediaTek Inc.
 * All rights reserved.
 *
 * Unless otherwise stipulated in writing, any and all information contained
 * herein regardless in any format shall remain the sole proprietary of
 * MediaTek Inc. and be kept in strict confidence
 * ("MediaTek Confidential Information") by the recipient.
 * Any unauthorized act including without limitation unauthorized disclosure,
 * copying, use, reproduction, sale, distribution, modification, disassembling,
 * reverse engineering and compiling of the contents of MediaTek Confidential
 * Information is unlawful and strictly prohibited. MediaTek hereby reserves the
 * rights to any and all damages, losses, costs and expenses resulting therefrom.
 *
 */

/*
 *
 * @author Mediatek Inc.
 *
 */

#ifndef _HWREG_SRCCAP_VBI_H_
#define _HWREG_SRCCAP_VBI_H_

#include "hwreg_common.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */
#ifdef INTERFACE
#undef INTERFACE
#endif
#define INTERFACE 		extern

#ifdef SYMBOL_WEAK
#undef SYMBOL_WEAK
#endif
#define SYMBOL_WEAK 	__attribute__((weak))


/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */
// Coarse Gain Define


/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */


/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */


/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
INTERFACE bool HWREG_VBI_TTX_CheckCircuitReady(void);
INTERFACE uint16_t HWREG_VBI_TTX_GetPacketCount(void);
INTERFACE void HWREG_VBI_WSSInit(void);
INTERFACE void HWREG_VBI_TTXInit(void);
INTERFACE void HWREG_VBI_TTX_InitSlicer(uint64_t addr, uint16_t packetCount);
INTERFACE void HWREG_VBI_TTX_EnableSlicer(bool bEnable);
INTERFACE uint8_t HWREG_VBI_TTX_GetHardware_Indication(void);
INTERFACE uint16_t HWREG_VBI_GetWSS_Data(void);
INTERFACE void HWREG_VBI_GetVPS_Data(uint8_t *byte1, uint8_t *byte2, uint8_t *byte3, uint8_t *byte4);
INTERFACE void HWREG_VBI_Set_Secam_VideoStandard(void);
INTERFACE void HWREG_VBI_Set_PalNC_VideoStandard(void);
INTERFACE void HWREG_VBI_Set_Pal_VideoStandard(void);
INTERFACE void HWREG_VBI_TTX_EnableLine(uint16_t StartLine, uint16_t EndLine);
INTERFACE void HWREG_VBI_CC_Init(uint64_t phyAddr, uint16_t u16Len);
INTERFACE void HWREG_VBI_CC_EnableSlicer(bool bEnable);
INTERFACE uint8_t HWREG_VBI_CC_DataRateSet(uint8_t *ptable);
INTERFACE bool HWREG_VBI_CC_SetSCWindowLen(uint8_t u8Len);
INTERFACE uint8_t HWREG_VBI_CC_GetPacketCnt(void);
INTERFACE uint8_t HWREG_VBI_CC_GetByteFoundIndication(void);
INTERFACE uint32_t HWREG_VBI_CC_GetPacket(void);
INTERFACE void HWREG_VBI_SetCCFrameCnt(uint8_t cnt);
INTERFACE void HWREG_VBI_CC_SetCCLine(uint16_t StartLine, uint16_t EndLine, uint8_t val); // val -> 0:NTSC, 1:PAL
INTERFACE void HWREG_VBI_SetWssVpsByteNum(uint8_t cnt);
INTERFACE uint16_t HWREG_VBI_GetWSS_Count(void);
INTERFACE void HWREG_VBI_RegStateStore(void);
INTERFACE void HWREG_VBI_RegStateRestore(void);
INTERFACE bool HWREG_VBI_ProtectMemory(bool bEnable, uint32_t phyAddr, uint32_t u32Size);

#endif // _HWREG_SRCCAP_VBI_H_