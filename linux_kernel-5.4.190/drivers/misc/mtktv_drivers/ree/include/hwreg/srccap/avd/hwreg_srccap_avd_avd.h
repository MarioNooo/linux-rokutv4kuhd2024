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

#ifndef _HWREG_SRCCAP_AVD_AVD_H_
#define _HWREG_SRCCAP_AVD_AVD_H_

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

#define AVD_PAULO_1_A               0
#define AVD_PAULO_1_B               1   // Paulo1, Paulo2
#define AVD_PAULO_2_A               2
#define AVD_SATURN_A                10  // S1
#define AVD_LOLA_A                  20  // Lola, Lomeo, S2, S3, Neptune
#define AVD_LATTE_A                 21  // Latte, Laser
#define AVD_MOSES_A                 22  // Moses
#define AVD_ERIS_A                  23  // Eris,Pluto,T1,T2
#define AVD_EUCLID_A                24  // Euclid(13K code size with SPL2/DPL2)
// For New Version VD MCU below
#define AVD_LOPEZ_A                 30  // Lopez,Metis,Martina
#define AVD_MUSE_A                  31  // Maria5
#define AVD_RAPHAEL_A               32  // Rapfael
#define AVD_TITANIA3_A              33  // T3
#define AVD_METIS_A                 34  // Metis,Martina
#define AVD_TITANIA4_A              35  // T4,T7,Janus
#define AVD_AMBER1_A                36  // Amber1,Amber5
#define AVD_AMBER7_A                37//Amber7,Agate,Amethyst,Eagle

#define VD_CHIP_VERSION             AVD_AMBER7_A

// 20090812 turn off MID function temporarily because of COMB bug, 1
//#define AVD_COMB_3D_MID            1
#if((VD_CHIP_VERSION>=AVD_TITANIA3_A))
#define NEW_VD_MCU              1
#else
#define NEW_VD_MCU              0
#endif
#if((VD_CHIP_VERSION==AVD_AMBER1_A))
#define TWO_VD_DSP_CODE              1
#else
#define TWO_VD_DSP_CODE              0
#endif
#if ((VD_CHIP_VERSION==AVD_TITANIA4_A))
#define LOAD_CODE_BYTE_WRITE_ONLY 1
#else
#define LOAD_CODE_BYTE_WRITE_ONLY 0
#endif
#if ((VD_CHIP_VERSION==AVD_TITANIA3_A))
#define T3_LOAD_CODE 1
#else
#define T3_LOAD_CODE 0
#endif
/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */
// Coarse Gain Define
#if (VD_CHIP_VERSION==AVD_AMBER1_A)      // Brian 20110428  A1 & A5 ADC gain mapping are different
#define VD_AGC_COARSE_GAIN  0xE // 4'b:1110    0.857
#else
#define VD_AGC_COARSE_GAIN  0xA // 4'b:1010    0.867
#endif

#define FSC_AUTO_DET_ENABLE                     0x00
#define FSC_AUTO_DET_DISABLE                    0x01

#define FSC_MODE_PAL                            0x00
#define FSC_MODE_SECAM                          0x01
#define FSC_MODE_NTSC                           0x02
#define FSC_MODE_NTSC_443                       0x03
#define FSC_MODE_PAL_M                          0x04
#define FSC_MODE_PAL_60                         0x05
#define FSC_MODE_PAL_N                          0x06

#define AVD_AGC_ENABLE                              0x00
#define AVD_AGC_DISABLE                             0x03

#define AVD_DSP_CODE_MSTAR 0
#define AVD_DSP_CODE_LGE_ATSC 1
#define AVD_DSP_CODE_LGE_MSTAR 2
#define AVD_DSP_CODE_SEC_ATSC 3
#define AVD_DSP_CODE_SEC 4
#define AVD_DSP_CODE_TYPE_VIF 5    //Milan VIF/ADC use different DSP AGC
#define AVD_DSP_CODE_TYPE_ADC 6   //Milan VIF/ADC use different DSP AGC

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
typedef enum
{
    HW_REG_AVD_VDMCU_CLOCK_170Mhz,
    HW_REG_AVD_VDMCU_CLOCK_160Mhz,
    HW_REG_AVD_VDMCU_CLOCK_144Mhz,
    HW_REG_AVD_VDMCU_CLOCK_123Mhz,
    HW_REG_AVD_VDMCU_CLOCK_108Mhz,
    HW_REG_AVD_VDMCU_CLOCK_MEM,
    HW_REG_AVD_VDMCU_CLOCK_MEM_DIV2,
    HW_REG_AVD_VDMCU_CLOCK_XTAL,
    HW_REG_AVD_VDMCU_CLOCK_54Mhz, // for drvavd.c compatibility, will set to 108Mhz
    HW_REG_AVD_VDMCU_CLOCK_86Mhz, // for drvavd.c compatibility, will set to 108Mhz
} HW_REG_AVD_VDMCUClockSpeed;

typedef enum
{
    HW_REG_AVD_VDMCU_CLOCK_NORMAL,
    HW_REG_AVD_VDMCU_CLOCK_INV,
} HW_REG_AVD_VDMCUClockInverse;

typedef enum
{
    HW_REG_E_INPUT_SOURCE_INVALID                  =   0x00,               // Video source Invalid
    HW_REG_E_INPUT_SOURCE_ATV                      =   0x01,               // Video source ATV
    HW_REG_E_INPUT_SOURCE_CVBS1                    =   0x02,               // Video source CVBS 1
    HW_REG_E_INPUT_SOURCE_CVBS2                    =   0x03,               // Video source CVBS 2
    HW_REG_E_INPUT_SOURCE_CVBS3                    =   0x04,               // Video source CVBS 2
    HW_REG_E_INPUT_SOURCE_SVIDEO1                  =   0x05,               // Video source SVIDEO 1
    HW_REG_E_INPUT_SOURCE_SVIDEO2                  =   0x06,               // Video source SVIDEO 2
    HW_REG_E_INPUT_SOURCE_SCART1                   =   0x07,               // Video source SCART 1
    HW_REG_E_INPUT_SOURCE_SCART2                   =   0x08,               // Video source SCART 2
    HW_REG_E_INPUT_SOURCE_YPbPr                    =   0x09,               // Video source YPbPr
    HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB           =   0x50,               // Video source through 3D Comb
    HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB_ATV       =   (HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB | HW_REG_E_INPUT_SOURCE_ATV),   // through 3D Comb ATV
    HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB_CVBS1     =   (HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB | HW_REG_E_INPUT_SOURCE_CVBS1),// through 3D Comb CVBS1
    HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB_CVBS2     =   (HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB | HW_REG_E_INPUT_SOURCE_CVBS2),// through 3D Comb CVBS2
    HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB_SCART1    =   (HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB | HW_REG_E_INPUT_SOURCE_SCART1),// through 3D Comb SCART1
    HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB_SCART2    =   (HW_REG_E_INPUT_SOURCE_THROUGH_3DCOMB | HW_REG_E_INPUT_SOURCE_SCART2),// through 3D Comb SCART2
    HW_REG_E_INPUT_SOURCE_MAX                      =   0x60
} HW_REG_AVD_InputSourceType;

typedef enum
{
    HW_REG_DEMODE_NORMAL                   = 0,
    HW_REG_DEMODE_MSTAR_VIF                = 1,
    HW_REG_DEMODE_MSTAR_VIF_MSB1210        = 2,
} HW_REG_AVD_DemodType;
/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

typedef struct
{
    uint8_t u8DetectWinBeforeLock;
    uint8_t u8DetectWinAfterLock;
    uint8_t u8CNTRFailBeforeLock;
    uint8_t u8CNTRSyncBeforeLock;
    uint8_t u8CNTRSyncAfterLock;
} HW_REG_VD_HSYNC_SENSITIVITY;

typedef enum
{
    HW_REG_E_ATV_CLK_ORIGIN_43P2MHZ   =    0x00,
    HW_REG_E_ATV_CLK_TYPE1_42MHZ      =    0x01,
    HW_REG_E_ATV_CLK_TYPE2_44P4MHZ    =    0x02,
}HW_REG_AVD_ATV_CLK_TYPE;

typedef enum
{
    HW_REG_E_VIDEOSTANDARD_PAL_BGHI        = 0x00,        // Video standard PAL BGHI
    HW_REG_HW_REG_E_VIDEOSTANDARD_NTSC_M          = 0x01,        // Video standard NTSC M
    HW_REG_E_VIDEOSTANDARD_SECAM           = 0x02,        // Video standard SECAM
    HW_REG_E_VIDEOSTANDARD_NTSC_44         = 0x03,        // Video standard  NTSC 44
    HW_REG_E_VIDEOSTANDARD_PAL_M           = 0x04,        // Video standard  PAL M
    HW_REG_E_VIDEOSTANDARD_PAL_N           = 0x05,        // Video standard  PAL N
    HW_REG_E_VIDEOSTANDARD_PAL_60          = 0x06,        // Video standard PAL 60
    HW_REG_E_VIDEOSTANDARD_NOTSTANDARD     = 0x07,        // NOT Video standard
    HW_REG_E_VIDEOSTANDARD_AUTO            = 0x08,         // Video standard AUTO
    HW_REG_E_VIDEOSTANDARD_MAX                                // Max Number
} HW_REG_AVD_VideoStandardType;

typedef struct
{
   uint8_t *pu8RiuBaseAddr;
   uint32_t u32DramBaseAddr;
   uint32_t u32Comb3dAliment;
   uint32_t u32Comb3dSize;
}HW_REG_AVD_HWinfo;
/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
INTERFACE void HWREG_AVD_ADC_SetGMC(uint8_t u8Value);
INTERFACE void HWREG_AVD_VDMCU_SoftStop (void);
INTERFACE void HWREG_AVD_VDMCU_SetFreeze (bool bEnable);
INTERFACE void HWREG_AVD_VDMCU_SetClock (HW_REG_AVD_VDMCUClockSpeed eClock, HW_REG_AVD_VDMCUClockInverse eInverse);
INTERFACE void HWREG_AVD_VDMCU_LoadDSP(const  uint8_t *pu8VD_DSP, uint32_t len);
INTERFACE bool HWREG_AVD_AFEC_GetHWHsync(void);
INTERFACE bool HWREG_AVD_AFEC_GetBurstOn(void);
INTERFACE bool HWREG_AVD_AFEC_GetCoChannelOn(void);
INTERFACE uint16_t HWREG_AVD_AFEC_GetVTotal(void);
INTERFACE uint16_t HWREG_AVD_AFEC_GetHTotal(void);
INTERFACE uint8_t HWREG_AVD_AFEC_GetNoiseMag(void);
INTERFACE void HWREG_AVD_AFEC_McuReset(void);
INTERFACE void HWREG_AVD_AFEC_SetClock(bool bEnable);
INTERFACE void HWREG_AVD_AFEC_SetPatchFlag(uint32_t u32VDPatchFlag);
INTERFACE void HWREG_AVD_AFEC_SetRegFromDSP (void);
INTERFACE void HWREG_AVD_AFEC_SetHTotal (uint32_t u32HTotal);
INTERFACE void HWREG_AVD_AFEC_SetVtotal (uint8_t u8Mode);
INTERFACE void HWREG_AVD_AFEC_SetBT656Width (uint8_t u8BT656Width);
INTERFACE void HWREG_AVD_AFEC_EnableForceMode (bool bEnable);
INTERFACE void HWREG_AVD_AFEC_SetFSCMode (uint8_t u8FSCMode);
INTERFACE void HWREG_AVD_AFEC_EnableCVBSLPF(bool bEnable);
INTERFACE void HWREG_AVD_AFEC_EnableBottomAverage(bool bEnable);
INTERFACE void HWREG_AVD_AFEC_EnableVBIDPLSpeedup(bool bEnable);
INTERFACE void HWREG_AVD_AFEC_AGCSetMode (uint8_t u8AgcMode);
INTERFACE void HWREG_AVD_AFEC_AGCSetCoarseGain (uint8_t u8AgcCoarseGain);
INTERFACE void HWREG_AVD_AFEC_AGCSetFineGain (uint8_t u8AgcFineGain);
INTERFACE void HWREG_AVD_AFEC_SetColorKillLevel (uint8_t u8ColorKillLevel);
INTERFACE void HWREG_AVD_AFEC_SetHsyncSensitivity(HW_REG_VD_HSYNC_SENSITIVITY eVDHsyncSensitivityTuning);
INTERFACE void HWREG_AVD_AFEC_SetSwingLimit(uint8_t u8Limit);
INTERFACE void HWREG_AVD_AFEC_SetChannelChange (void);
INTERFACE void HWREG_AVD_AFEC_SetColorStripe(uint8_t u8Value);
INTERFACE bool HWREG_AVD_COMB_Get3dCombTimingCheck(void);
INTERFACE void HWREG_AVD_COMB_SetMemoryProtect (uint64_t u32COMB_3D_Addr, uint32_t u32COMB_3D_Len);
INTERFACE void HWREG_AVD_COMB_Set3dCombMid(bool bEnable);
INTERFACE void HWREG_AVD_COMB_Set3dComb(bool bEnable);
INTERFACE void HWREG_AVD_COMB_Set3dCombSpeed(uint8_t u8COMB57, uint8_t u8COMB58, uint8_t u8COMB5F);
INTERFACE void HWREG_AVD_COMB_Set3dDetectionTolerance(uint8_t u8Threshold);
INTERFACE void HWREG_AVD_COMB_Set3dFineTune(bool bEnable);
INTERFACE void HWREG_AVD_COMB_SetF2(uint8_t u8Value);
INTERFACE void HWREG_AVD_COMB_SetNonStandardFSC(bool bIsPAL,bool bIsNonstandard);
INTERFACE void HWREG_AVD_COMB_SetYCPipe(uint8_t u8YCPipe);
INTERFACE void HWREG_AVD_COMB_SetCbCrInverse(uint8_t u8CbCrInverse);
INTERFACE void HWREG_AVD_COMB_SetVerticalTimingDetectMode(uint8_t u8Mode);
INTERFACE void HWREG_AVD_COMB_SetLineBufferMode(uint8_t u8Mode);
INTERFACE void HWREG_AVD_COMB_SetNonStandardHtotal(bool bEnable);
INTERFACE void HWREG_AVD_COMB_SetHtotal(uint16_t u16Htotal);
INTERFACE void HWREG_AVD_COMB_SetHsyncTolerance(uint8_t u8Tolerance);
INTERFACE void HWREG_AVD_COMB_SetMemoryRequest(bool bEnable);
INTERFACE void HWREG_AVD_VBI_SetTTSigDetSel(bool bEnable);
INTERFACE void HWREG_AVD_VBI_SetVPSPhaseAcc(uint16_t u16Parameter);
INTERFACE uint8_t HWREG_AVD_GetReg(uint32_t u32Addr);
INTERFACE void HWREG_AVD_SetReg(uint32_t u32Addr, uint8_t u8Value);
INTERFACE void HWREG_AVD_SetPQFineTune(void);
INTERFACE uint8_t HWREG_AVD_GetHsyncEdge(void);
INTERFACE void HWREG_AVD_Set2D3DPatchOnOff(bool bEnable);
INTERFACE void HWREG_AVD_AFEC_BackPorchWindowPosition(bool bEnable ,uint8_t u8Value);
INTERFACE void HWREG_AVD_SetDSPCodeType(uint32_t u32VDPatchFlag);
INTERFACE void HWREG_AVD_ShiftClk(HW_REG_AVD_ATV_CLK_TYPE eShiftMode, HW_REG_AVD_VideoStandardType eStandard,uint32_t u32XTAL_Clock);
INTERFACE bool HWREG_AVD_AFEC_GetMacroVisionDetect(void);
INTERFACE bool HWREG_AVD_VBI_GetCGMSDetect(void);
INTERFACE void HWREG_AVD_SetBurstWinStart(uint8_t u8BusrtStartPosition);
INTERFACE void HWREG_AVD_SetBurstWinEnd(uint8_t u8BusrtEndPosition);
INTERFACE void HWREG_AVD_AFEC_SET_DPL_K2(uint8_t u8Value);
INTERFACE uint8_t HWREG_AVD_AFEC_GET_DPL_K1(void);
INTERFACE void HWREG_AVD_AFEC_SET_DPL_K1(uint8_t u8Value);
INTERFACE void HWREG_AVD_SCART_Setting(HW_REG_AVD_InputSourceType eSource);
INTERFACE void HWREG_AVD_COMB_SET_StillImage(uint8_t u8COMB37,uint8_t u8COMB38,uint8_t u8COMB7C,uint8_t u8COMBED);
INTERFACE uint8_t HWREG_AVD_DSPReadByte(uint8_t u8Cmd, uint16_t u16Addr);
INTERFACE void HWREG_AVD_SET_SECAM_CB_CR_LPF(uint8_t u8Value);
INTERFACE uint8_t HWREG_AVD_GET_SECAM_CB_CR_LPF(void);
INTERFACE uint8_t HWREG_AVD_AFEC_GET_PACTH_D4(void);
INTERFACE void HWREG_AVD_AFEC_SET_PACTH_D4(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_PACTH_D8(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_FORCE_K1K2(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_RF_LOW_HSYNC_DETECTION(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_ENABLE_SLICE_LEVEL(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_V_SLICE_LEVEL(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_H_SLICE_LEVEL(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_FIX_GAIN(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_AGC_UPDATE(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_RF_MODE(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_JILIN_PATCH(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_CoarseGain(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_FineGain(uint8_t u8Value);
INTERFACE void HWREG_AVD_AFEC_SET_LPF_Gain(uint8_t u8Value);
INTERFACE uint8_t HWREG_AVD_AFEC_GET_MSB_DPL(void);
INTERFACE void HWREG_AVD_AFEC_SET_MSB_DPL(uint8_t u8Value);
INTERFACE uint8_t HWREG_AVD_AFEC_GET_LSN_DPL(void);
INTERFACE void HWREG_AVD_AFEC_SET_LSN_DPL(uint8_t u8Value);
INTERFACE void HWREG_AVD_SECAM_SET_Magnitude_threshold(void);
INTERFACE void HWREG_AVD_AFEC_SET_ReadBus(uint8_t u8Value);
INTERFACE uint8_t HWREG_AVD_AFEC_GET_Window_Hsync_Edge(void);
INTERFACE uint8_t HWREG_AVD_GET_VD_MCU_SRAM(void);
INTERFACE void HWREG_AVD_SET_VD_MCU_SRAM(uint8_t u8Value);
INTERFACE uint8_t HWREG_AVD_AFEC_GET_DSPVersion(void);
INTERFACE uint8_t HWREG_AVD_AFEC_GET_Heartbeat_Counter(void);
INTERFACE void HWREG_AVD_STR_COMB_STORE_SUB_REG(void);
INTERFACE void HWREG_AVD_STR_LOAD_REG(void);
INTERFACE void HWREG_AVD_STR_STORE_REG(void);
INTERFACE void HWREG_AVD_STR_COMB_LOAD_SUB_REG(void);
INTERFACE uint8_t HWREG_AVD_AFEC_GET_FSC_Detection_Flag(void);
INTERFACE uint8_t HWREG_AVD_AFEC_GET_Hsync_Mag(void);
INTERFACE uint8_t HWREG_AVD_COMB_GET_3D_Status(void);
INTERFACE void HWREG_AVD_RESET_DSP_Code(void);
INTERFACE void HWREG_AVD_RESET_AFEC_HW(void);
INTERFACE void HWREG_AVD_RESET_COMB(void);
INTERFACE void HWREG_AVD_Test_Pattern(uint8_t u8Value);
INTERFACE void HWREG_AVD_ByPass_COMB(uint8_t u8Value);
INTERFACE void HWREG_AVD_SET_COMB_Threshold(uint8_t u8Value);
INTERFACE uint8_t HWREG_AVD_AVD_COMB_GET_StillImageParam38(void);
INTERFACE void HWREG_AVD_COMB_Adjustment_Jagged_Line(HW_REG_AVD_InputSourceType eSource, uint16_t u16StandardVtotal);
INTERFACE void HWREG_AVD_RegInit (void);
INTERFACE void HWREG_AVD_RegInitExt(uint8_t *_u8VdDecInitializeExt);
INTERFACE void HWREG_AVD_AFEC_SetClockSource(bool bSource);
INTERFACE void HWREG_AVD_AFEC_SetInput(HW_REG_AVD_InputSourceType eSource, uint8_t u8ScartFB, HW_REG_AVD_DemodType eDemodType, uint32_t u32XTAL_Clock);
INTERFACE uint8_t HWREG_AVD_AFEC_GET_Standard_Detection_Flag(void);
INTERFACE void HWREG_AVD_AFEC_SET_Standard_Detection_Flag(uint8_t u8Value);
INTERFACE uint16_t HWREG_AVD_AFEC_GetStatus(void);
INTERFACE bool HWREG_AVD_SET_HW_Info(HW_REG_AVD_HWinfo stHWInfo);
#endif // _HWREG_SRCCAP_AVD_AVD_H_