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

#ifndef _HAL_HWREG_EARC_H_
#define _HAL_HWREG_EARC_H_

#include "hwreg_common.h"


/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */
#ifndef UNUSED
#define UNUSED(x) ((x)=(x))
#endif

#ifndef BIT
#define BIT(_bit_)                  (1 << (_bit_))
#endif

#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif

#ifndef true
/// definition for true
#define true                        1
/// definition for false
#define false                       0
#endif

/// definition for ENABLE
#ifndef ENABLE
#define ENABLE 1
#endif
/// definition for DISABLE
#ifndef DISABLE
#define DISABLE 0
#endif

#ifndef BIT64
#define BIT64(_bit_) (1ull << (_bit_))
#endif

#ifdef NULL
#undef NULL
#endif
#define NULL                        0

#define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))

#define EARC_ASSERT(_bool_)                                                                                   \
    {                                                                                                   \
        if (!(_bool_))                                                                                  \
        {                                                                                               \
            printk("\033[35m ASSERT FAIL: %s, %s %s %d\033[35m\n", #_bool_, __FILE__, __PRETTY_FUNCTION__, __LINE__);    \
            panic("mstar panic\n");                                                                     \
        }                                                                                               \
    }


/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */


/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */

INTERFACE void HWREG_EARC_Init (void);
INTERFACE void HWREG_EARC_SetHWStatus(uint32_t Para1, uint32_t Para2);
INTERFACE void HWREG_EARC_OutputPortSelect (uint8_t u8InputPortType,bool bEnable5VDetectInvert,bool bEnableHPDInvert);
INTERFACE void HWREG_EARC_SupportMode (uint8_t u8SupportMode);
INTERFACE void HWREG_EARC_ArcPin (uint8_t u8ArcPinEnable);
INTERFACE void HWREG_EARC_TX_SetOpCodeDeviceId(uint8_t u8DeviceID);
INTERFACE void HWREG_EARC_TX_SetOpCodeOffset(uint8_t u8Offset);
INTERFACE void HWREG_EARC_TX_SetTransactionDataThreshold(uint8_t u8DataThreshold);
INTERFACE void HWREG_EARC_TX_SetSetIrqClear0(uint16_t u16IrqClear);
INTERFACE void HWREG_EARC_TX_SetSetIrqClear1(uint16_t u16IrqClear);
INTERFACE void HWREG_EARC_TX_SetReadTrig(bool bEarcReadTrig);
INTERFACE void HWREG_EARC_TX_SetWriteTrig(bool bEarcWriteTrig);
INTERFACE void HWREG_EARC_TX_SetPacketTxFifoWritePulse(bool bEarcPacketTxFifoWritePulse);
INTERFACE void HWREG_EARC_TX_ConfigHeartbeatStatus(uint8_t u8earc_state, uint8_t u8HeartbeatStatus, bool bOverWriteEnable, bool bSetValue);
INTERFACE void HWREG_EARC_TX_SetDifferentialDriveStrength(uint8_t u8Level);
INTERFACE void HWREG_EARC_TX_SetDifferentialSkew(uint8_t u8Level);
INTERFACE void HWREG_EARC_TX_SetCommonDriveStrength(uint8_t u8Level);
INTERFACE void HWREG_EARC_TX_SetDifferentialOnOff(bool bOnOff);
INTERFACE void HWREG_EARC_TX_SetCommonOnOff(bool bOnOff);
INTERFACE void HWREG_EARC_TX_ResetGetOnce (void);

INTERFACE bool HWREG_EARC_TX_GetDifferentialOnOff(void);
INTERFACE bool HWREG_EARC_TX_GetCAPChangeFlag (bool bResetIRQ);
INTERFACE bool HWREG_EARC_TX_GetSTATChangeFlag (bool bResetIRQ);
INTERFACE bool HWREG_EARC_TX_WriteByteCommand(uint8_t u8Count,uint8_t u8Value);

INTERFACE uint8_t HWREG_EARC_TX_GetArcPinStatus(void);
INTERFACE uint8_t HWREG_EARC_TX_GetDiscState(void);
INTERFACE uint8_t HWREG_EARC_TX_GetCapbilityChange(void);
INTERFACE uint8_t HWREG_EARC_TX_GetLatencyChange(void);
INTERFACE uint8_t HWREG_EARC_TX_GetCapbilityValue(void);
INTERFACE uint8_t HWREG_EARC_TX_GetLatencyValue(void);
INTERFACE uint8_t HWREG_EARC_TX_GetCommonOnOff(void);
INTERFACE uint8_t HWREG_EARC_TX_GetDifferentialDriveStrength(void);
INTERFACE uint8_t HWREG_EARC_TX_GetDifferentialSkew(void);
INTERFACE uint8_t HWREG_EARC_TX_GetCommonDriveStrength(void);
INTERFACE uint8_t HWREG_EARC_TX_ReadByteCommand(uint8_t u8Count);
INTERFACE uint16_t HWREG_EARC_TX_GetHeartBeatStatus(void);
INTERFACE uint16_t HWREG_EARC_TX_GetIrqStatus0(void);
INTERFACE uint16_t HWREG_EARC_TX_GetIrqStatus1(void);

#endif // _HAL_HWREG_EARC_H_
