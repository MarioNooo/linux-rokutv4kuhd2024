//<Mediatek Software>
//******************************************************************************
// Mediatek Software
// Copyright (c) 2020 - 2024 Mediatek Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("Mediatek Software") are
// intellectual property of Mediatek Semiconductor, Inc. ("Mediatek") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of Mediatek Software is expressly prohibited, unless prior written
// permission has been granted by Mediatek.
//
// By accessing, browsing and/or using Mediatek Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. Mediatek shall retain any and all right, ownership and interest to Mediatek
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to Mediatek Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that Mediatek Software might include, incorporate or be
//    supplied together with third party`s software and the use of Mediatek
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. Mediatek Software and any modification/derivatives thereof shall be deemed as
//    Mediatek`s confidential information and you agree to keep Mediatek`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. Mediatek Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by Mediatek, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against Mediatek for any loss, damage, cost or expense that you may
//    incur related to your use of Mediatek Software.
//    In no event shall Mediatek be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if Mediatek Software has been modified by Mediatek in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, Mediatek may from time to time provide technical supports or
//    services in relation with Mediatek Software to you for your use of
//    Mediatek Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use Mediatek name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating Mediatek or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020-2024 MediaTek Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MediaTek Inc. and be kept in strict confidence
// ("MediaTek Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MediaTek Confidential
// Information is unlawful and strictly prohibited. MediaTek hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MDRV_GOP_H
#define MDRV_GOP_H

#include "mdrv_types_internal.h"
#include "hwreg_common.h"

#ifndef INTERFACE
#define INTERFACE
#endif

#ifndef SYMBOL_WEAK
#define SYMBOL_WEAK
#endif

typedef enum
{
    /// color format RGB565
    E_GOP_FMT_RGB565,
    /// color format ARGB1555
    E_GOP_FMT_ARGB1555,
    /// color format ARGB4444
    E_GOP_FMT_ARGB4444,
    /// color format YUV422
    E_GOP_FMT_YUV422,
    /// color format ARGB8888
    E_GOP_FMT_ARGB8888,
    /// color format RGBA5551
    E_GOP_FMT_RGBA5551,
    /// color format RGBA4444
    E_GOP_FMT_RGBA4444,
    /// Start of New color define
    /// color format BGRA5551
    E_GOP_FMT_BGRA5551,
    /// color format ABGR1555
    E_GOP_FMT_ABGR1555,
    /// color format ABGR4444
    E_GOP_FMT_ABGR4444,
    /// color format BGRA4444
    E_GOP_FMT_BGRA4444,
    /// color format BGR565
    E_GOP_FMT_BGR565,
    /// color format RGBA8888
    E_GOP_FMT_RGBA8888,
    /// color format BGRA8888
    E_GOP_FMT_BGRA8888,
    /// color format ABGR8888
    E_GOP_FMT_ABGR8888,
    /// color format AYUV8888
    E_GOP_FMT_AYUV8888,
    /// color format A2RGB10
    E_GOP_FMT_A2RGB10,
    /// color format A2BGR10
    E_GOP_FMT_A2BGR10,
    /// color format RGB10A2
    E_GOP_FMT_RGB10A2,
    /// color format BGR10A2
    E_GOP_FMT_BGR10A2,
	/// color format RGB888
	E_GOP_FMT_RGB888,
} GOP_ColorFormat;

typedef enum
{
    E_GOP_RBLKAddr,
    E_GOP_DRAMVstr,
    E_GOP_RBLKSize,
    E_GOP_ScrollHVStopAddr,
    E_GOP_RBLK3DAddr
}E_GOP_ADDRType;

/// Transparent color format
typedef enum
{
    E_GOP_TRANSCLR_FMT0,   //RGB mode transparent color
    E_GOP_TRANSCLR_FMT1,   //index mode transparent color
    E_GOP_TRANSCLR_FMT2,   //ARGB 8888 transparent color
    E_GOP_TRANSCLR_FMT3,   //YUV mode transparent color
}E_GOP_TransClrFmt;

/// Define GOP destination displayplane type
typedef enum
{
    E_HW_GOP_DST_IP0       =   0,
    E_HW_GOP_DST_OP0       =   1,
    E_HW_GOP_DST_FRC       =   2,
    E_HW_GOP_DST_SUPPORT_MAX,
    E_HW_GOP_DST_INVALID,
} E_HW_GOP_DstType;

/// Define GOP Mux
typedef enum
{
    ///Select gop output to mux0
    E_HW_GOP_MUX0 = 0,
    /// Select gop output to mux1
    E_HW_GOP_MUX1 = 1,
    /// Select gop output to mux2
    E_HW_GOP_MUX2 = 2,
    /// Select gop output to mux3
    E_HW_GOP_MUX3 = 3,
    /// Select gop output to mux4
    E_HW_GOP_MUX4 = 4,
    ///Select gop output to IP0
    E_HW_GOP_IP0_MUX,
    /// Select gop output to FRC mux0
    E_HW_GOP_FRC_MUX0,
    /// Select gop output to FRC mux1
    E_HW_GOP_FRC_MUX1,
    /// Select gop output to FRC mux2
    E_HW_GOP_FRC_MUX2,
    /// Select gop output to FRC mux3
    E_HW_GOP_FRC_MUX3,
    /// Select gop output to BYPASS mux
    E_HW_GOP_BYPASS_MUX0,
    /// Select gop output to VE1
    E_HW_GOP_VE1_MUX,

    E_HW_MAX_GOP_MUX_SUPPORT,
    E_HW_GOP_MUX_INVAILD,
} E_HW_GOP_MuxSel;

/// Define Scaler GOP IP setting.
typedef enum
{
    E_HW_IP0_SEL_GOP0,
    E_HW_IP0_SEL_GOP1,
    E_HW_IP0_SEL_GOP2,
    E_HW_NIP_SEL_GOP0,
    E_HW_NIP_SEL_GOP1,
    E_HW_NIP_SEL_GOP2,
    E_HW_MVOP_SEL,
}E_HW_IPSEL_GOP;

/// Define GOP H-Stretch mode.
typedef enum
{
    E_HW_GOP_HSTRCH_DUPLICATE,
    E_HW_GOP_HSTRCH_6TAPE_LINEAR ,
    E_HW_GOP_HSTRCH_6TAPE_NEAREST,
    E_HW_GOP_HSTRCH_4TAPE,
    E_HW_GOP_HSTRETCH_MODE_SUPPORT,
}E_HW_GOPStrchHMode;

/// Define GOP V-Stretch mode.
typedef enum
{
    E_HW_GOP_VSTRCH_LINEAR,
    E_HW_GOP_VSTRCH_DUPLICATE,
    E_HW_GOP_VSTRCH_NEAREST,
    E_HW_GOP_VSTRCH_BILINEAR,
    E_HW_GOP_VSTRCH_4TAP,
    E_HW_GOP_VSTRETCH_MODE_SUPPORT,
} E_HW_GOPStrchVMode;

typedef enum
{
    E_HW_POWER_SUSPEND,
    E_HW_POWER_RESUME,
} E_HW_POWER_MODE;

typedef enum
{
    E_HW_GOP_FORCEWRITE,
    E_HW_GOP_BNKFORCEWRITE,
    E_HW_GOP_WITHVSYNC,
    E_HW_GOP_WITHSYNC_MULTIGOPUPDATED,
} E_HW_GOPTriggerMode;

typedef enum
{
    E_HW_GOP_VOPPATH_DEFAULT,
    E_HW_GOP_VOPPATH_BEF_RGB3DLOOKUP,
    E_HW_GOP_VOPPATH_BEF_PQGAMMA,
    E_HW_GOP_VOPPATH_AFT_PQGAMMA,
    E_HW_GOP_VOPPATH_MAX,
}EN_HW_GOP_VOPPATH_SEL;

typedef enum
{
    E_HW_GOP_GET_GWIN_ENGINE_ID,
    E_HW_GOP_GET_GWIN_MAX_NUMBER,
    E_HW_GOP_GET_GWIN_BASEID,
    E_HW_GOP_GWIN_MAX,
}EN_HW_GOP_GWIN_MODE;

typedef enum
{
	E_HW_GOP_PATTERN_MOVE_TO_RIGHT,
	E_HW_GOP_PATTERN_MOVE_TO_LEFT,
}EN_HW_GOP_TESTPATTERN_MOVE_DIR;

typedef enum {
	E_HW_GOP_OUT_A8RGB10,
	E_HW_GOP_OUT_ARGB8888_DITHER,
	E_HW_GOP_OUT_ARGB8888_ROUND,
} EN_HW_GOP_OUTPUT_MODE;

typedef enum {
	E_HW_GOP_VIDEO_OSDB0_OSDB1,
	E_HW_GOP_OSDB0_VIDEO_OSDB1,
	E_HW_GOP_OSDB0_OSDB1_VIDEO,
} EN_HW_GOP_VG_ORDER;

typedef struct
{
    MS_U16 u16CscControl;
    MS_U16 Matrix[3][3];
    MS_U16 u16BrightnessOffsetR;
    MS_U16 u16BrightnessOffsetG;
    MS_U16 u16BrightnessOffsetB;
}ST_HW_GOP_CSC_TABLE;

typedef struct
{
    MS_BOOL bHwCropEn;
    MS_U16 u16CropWinX0;
    MS_U16 u16CropWinY0;
    MS_U16 u16CropWinX1;
    MS_U16 u16CropWinY1;
}ST_HW_GOP_CROP_WINDOW;

typedef struct
{
	MS_BOOL bEnable;
	MS_PHY u64DRAMAddr;
	MS_U16 u16HPixelStart;
	MS_U16 u16HPixelEnd;
	MS_U16 u16VPixelStart;
	MS_U16 u16VPixelEnd;
}ST_HW_GOP_AFBC_INFO;

typedef struct
{
	MS_U8  PatEnable;
	MS_U8  HwAutoMode;
	MS_U32 DisWidth;
	MS_U32 DisHeight;
	MS_U32 ColorbarW;
	MS_U32 ColorbarH;
	MS_U8  EnColorbarMove;
	EN_HW_GOP_TESTPATTERN_MOVE_DIR MoveDir;
	MS_U16 MoveSpeed;
}ST_HW_GOP_PATINFO;

typedef struct
{
	MS_U16  Tgen_hs_st;
	MS_U16  Tgen_hs_end;
	MS_U16  Tgen_hfde_st;
	MS_U16  Tgen_hfde_end;
	MS_U16  Tgen_htt;
	MS_U16  Tgen_vs_st;
	MS_U16  Tgen_vs_end;
	MS_U16  Tgen_vfde_st;
	MS_U16  Tgen_vfde_end;
	MS_U16  Tgen_vtt;
	EN_HW_GOP_OUTPUT_MODE GOPOutMode;
}ST_HW_GOP_TGENINFO;

typedef struct
{
	MS_BOOL bEnable;
	MS_U8 GopIdx;
}ST_HW_GOP_SYS_DEBUG_INFO;


typedef enum {
	E_HW_GOP_NS,
	E_HW_GOP_SDC,
	E_HW_GOP_S,
	E_HW_GOP_CSP,
} EN_HW_GOP_AID_TYPE;

typedef struct
{
	MS_U8 bRetTotal;
	MS_U16 gopIdx;
	MS_U32 MainVRate;
	MS_U32 Threshold;
} ST_HW_GOP_GETROI;

INTERFACE SYMBOL_WEAK MS_BOOL KDrv_GOP_Test(void);
INTERFACE MS_BOOL KDrv_HW_GOP_SetIOMapBase(MS_VIRT *MMIOBaseAdr);
INTERFACE MS_BOOL KDrv_HW_GOP_Init(MS_U8 u8GOP);
INTERFACE MS_BOOL KDrv_HW_GOP_PixelModeEnable(MS_U8 u8GOP, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_OpMuxDbEnable(MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_IsGWINEnabled(MS_U8 u8GOP, MS_U8 u8win);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_Enable(MS_U8 u8GOP, MS_U8 u8win, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_PixelAlphaEnable(MS_U8 u8GOP, MS_U8 u8win, MS_BOOL bEnable, MS_U8 u8coef);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_ProgressiveEnable(MS_U8 u8GOP, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_IsProgressive(MS_U8 u8GOP);
INTERFACE MS_BOOL KDrv_HW_GOP_IsGOPMirrorEnable(MS_U8 u8GOP, MS_BOOL *bHMirror, MS_BOOL *bVMirror);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_HMirrorEnable(MS_U8 u8GOP, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_VMirrorEnable(MS_U8 u8GOP, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_SetWinFmt(MS_U8 u8GOP, MS_U8 u8win, MS_U16 u16Clrtype);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_GetWinFmt(MS_U8 u8GOP, MS_U8 u8win, MS_U16* pu16Clrtype);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_SetWinHstr(MS_U8 u8GOP, MS_U8 u8win, MS_U16 u16RegVal);
INTERFACE MS_BOOL KDrv_HW_GOP_GetDram_Addr(MS_U8 u8GOP, MS_U8 u8win, MS_PHY* pphyAddr, E_GOP_ADDRType eAddrType);
INTERFACE MS_BOOL KDrv_HW_GOP_SetDram_Addr(MS_U8 u8GOP, MS_U8 u8win, MS_PHY PhyAddr, E_GOP_ADDRType eAddrType);
INTERFACE MS_BOOL KDrv_HW_GOP_SetPitch(MS_U8 u8GOP, MS_U8 u8win, MS_U16 u16FbWidth, MS_U16 u16FbHeight);
INTERFACE MS_U16 KDrv_HW_GOP_GetPitch(MS_U8 u8GOP, MS_U8 u8win);
INTERFACE MS_BOOL KDrv_HW_GOP_SetGopGwinHVPixel(MS_U8 u8GOP, MS_U8 u8win, MS_U16 u16Hstart, MS_U16 u16Hend, MS_U16 u16Vstart, MS_U16 u16Vend);
INTERFACE MS_BOOL KDrv_HW_GOP_GetGopGwinHVPixel(MS_U8 u8GOP, MS_U8 u8win, MS_U16* u16Hstart, MS_U16* u16Hend, MS_U16* u16Vstart, MS_U16* u16Vend);
INTERFACE MS_BOOL KDrv_HW_GOP_GetGopStretchWin(MS_U8 u8GOP, MS_U16* u16x, MS_U16* u16y, MS_U16* u16width, MS_U16* u16height);
INTERFACE MS_BOOL KDrv_HW_GOP_SetGopStretchWin(MS_U8 u8GOP, MS_U16 u16x, MS_U16 u16y, MS_U16 u16width, MS_U16 u16height);
INTERFACE MS_BOOL KDrv_HW_GOP_GetGopScaleRatio(MS_U8 u8GOP, MS_U32* u32Hratio, MS_U32* u32Vratio);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_Set_HSCALE(MS_U8 u8GOP, MS_U16 u16HInitPhase, MS_U64 u64Hratio);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_Set_VSCALE(MS_U8 u8GOP, MS_U16 u16VInitPhase, MS_U64 u64Vratio);
INTERFACE MS_BOOL KDrv_HW_GOP_SetGopVaildH(MS_U8 u8GOP, MS_U16 u16ValidH);
INTERFACE MS_BOOL KDrv_HW_GOP_SetGopVaildV(MS_U8 u8GOP, MS_U16 u16ValidV);
INTERFACE MS_BOOL KDrv_HW_GOP_SetGOP2SCEnable(MS_U8 u8GOP, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_TransClrEnable(MS_U8 u8GOP, E_GOP_TransClrFmt FmtMode, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_VopNblEnable(MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_AlphaInvEnable(MS_U8 u8GOP, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_GetDstPlane(MS_U8 u8GOP, E_HW_GOP_DstType *pGopDst);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_SetDstPlane( MS_U8 u8GOP, E_HW_GOP_DstType eDstType);
INTERFACE MS_BOOL KDrv_HW_GOP_PreAlphaEnable(MS_U8 u8GOP, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_GetPreAlphaMode(MS_U8 u8GOP);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_SetMux(MS_U8 u8GOP, E_HW_GOP_MuxSel eGopMux);
INTERFACE MS_U8 KDrv_HW_GOP_GWIN_GetMux(E_HW_GOP_MuxSel eGopMux);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_SetHSPipe(MS_U8 u8GOP, MS_U16 u16HSPipe);
INTERFACE EN_HW_GOP_VOPPATH_SEL KDrv_HW_GOP_GetVOPPathSel(void);
INTERFACE MS_BOOL KDrv_HW_GOP_SetVOPPathSel(EN_HW_GOP_VOPPATH_SEL eGopVopPath);
INTERFACE MS_BOOL KDrv_HW_GOP_OCEnable(MS_U8 u8GOP, MS_BOOL bOCEn);
INTERFACE MS_BOOL KDrv_HW_GOP_IsHDREnabled(void);
INTERFACE MS_BOOL KDrv_HW_GOP_FirstHsEnable(MS_U8 u8GOP, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_SetIPSel2SC(E_HW_IPSEL_GOP GopIPSel);
INTERFACE MS_BOOL KDrv_HW_GOP_SetGOPClk(MS_U8 u8GOP, E_HW_GOP_DstType eDstType);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_Load_HStretchModeTable(MS_U8 u8GOP, MS_U16* pu16GopHStretchTable);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_Load_VStretchModeTable(MS_U8 u8GOP, MS_U16* pu16GopVStretchTable);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_Set_HStretchMode(MS_U8 u8GOP, E_HW_GOPStrchHMode HStrchMode);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_Set_VStretchMode(MS_U8 u8GOP, E_HW_GOPStrchVMode VStrchMode);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_PowerState(E_HW_POWER_MODE ePowerState);
INTERFACE MS_BOOL KDrv_HW_GOP_GWIN_TriggerRegWriteIn(MS_U8 u8GOP, MS_BOOL bEnable, E_HW_GOPTriggerMode eGOPTriggerMode);
INTERFACE MS_BOOL KDrv_HW_GOP_GetRegUpdated(MS_U8 u8GOP);
INTERFACE MS_BOOL KDrv_HW_GOP_SetCSCCtrl(MS_U8 u8GOP, MS_BOOL bEnable, ST_HW_GOP_CSC_TABLE* pstGOPCscTbl);
INTERFACE MS_BOOL KDrv_GOP_AutoDectBuf(MS_U8 u8GOP, MS_BOOL bEnable, MS_U8 u8AlphaTh);
INTERFACE MS_BOOL KDrv_GOP_GetNonTransCnt(MS_U32* pu32Count);
INTERFACE MS_BOOL KDrv_HW_GOP_Trigger_MUX(void);
INTERFACE MS_BOOL KDrv_HW_GOP_SetCropWindow(MS_U8 u8GOP, ST_HW_GOP_CROP_WINDOW *pstCropWin);
INTERFACE MS_U8 KDrv_HW_GOP_GetGwinNum(MS_U8 u8GOP, MS_U8 u8EngineIdx, EN_HW_GOP_GWIN_MODE eGOPGwinMode);
INTERFACE MS_BOOL KDrv_HW_GOP_RstCtrl(MS_U8 u8GOP, MS_BOOL bSwRst);
INTERFACE MS_BOOL KDrv_HW_GOP_InterruptEnable(MS_U8 u8GOP, MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_HW_GOP_SetMixer2OutSize(MS_U16 u16Width, MS_U16 u16Height);
INTERFACE MS_BOOL KDrv_HW_GOP_GetStep2HVSPHScale(MS_BOOL *bEnable, MS_U32 *u32HRatio);
INTERFACE MS_BOOL KDrv_HW_GOP_SetStep2HVSPHscale(MS_BOOL bEnable, MS_U32 u32HRatio);
INTERFACE MS_BOOL KDrv_HW_GOP_GetStep2HVSPVScale(MS_BOOL *bEnable, MS_U32 *u32VRatio);
INTERFACE MS_BOOL KDrv_HW_GOP_SetStep2HVSPVscale(MS_BOOL bEnable, MS_U32 u32VRatio);
INTERFACE MS_BOOL KDrv_HW_GOP_SetMixer4OutSize(MS_U16 u16Width, MS_U16 u16Height);
INTERFACE MS_BOOL KDrv_HW_GOP_SetStep1HVSPHscale(MS_U8 u8GOP, MS_BOOL bEnable, MS_U32 u32HRatio, MS_U16 u16Hout);
INTERFACE MS_BOOL KDrv_HW_GOP_SetStep1HVSPVscale(MS_U8 u8GOP, MS_BOOL bEnable, MS_U32 u32VRatio, MS_U16 u16Vout);
INTERFACE MS_BOOL KDrv_HW_GOP_SetAFBC(MS_U8 u8GOP, ST_HW_GOP_AFBC_INFO *pstHWGopAFBCInfo);
INTERFACE MS_BOOL KDrv_HW_GOP_SetTestPattern(ST_HW_GOP_PATINFO *pGOPPatInfo);
INTERFACE MS_U32 KDrv_HW_GOP_GetCRC(void);
INTERFACE MS_U32 KDrv_HW_GOP_PrintInfo(MS_U8 GOPNum);
INTERFACE MS_BOOL KDrv_HW_GOP_SetBypassPath(MS_U8 u8GOP, MS_BOOL bEnable,  MS_BOOL bMixer4RmaEn,
	MS_BOOL bMixer2RmaEn, EN_HW_GOP_OUTPUT_MODE GOPOutMode);
INTERFACE MS_BOOL KDrv_HW_GOP_SetTGen(ST_HW_GOP_TGENINFO *pGOPTgenInfo);
INTERFACE MS_BOOL KDrv_HW_GOP_SetTGenVGSync(MS_BOOL bEnable);

INTERFACE bool KDrv_HW_GOP_XVYCC_readSram_degamma(uint8_t u8GOP, void *pData);
INTERFACE bool KDrv_HW_GOP_XVYCC_readSram_gamma(uint8_t u8GOP, void *pData);
INTERFACE MS_BOOL KDrv_HW_GOP_SetMixerOutMode(MS_BOOL Mixer1_OutPreAlpha, MS_BOOL Mixer2_OutPreAlpha);
INTERFACE MS_BOOL KDrv_HW_GOP_GetBypass_MixerMode(MS_U8 u8GOP, MS_BOOL *pEnable, MS_BOOL *pMixer2RmaEn);
INTERFACE MS_BOOL KDrv_HW_GOP_MlBegin(MS_U8 u8GOP);
INTERFACE MS_BOOL KDrv_HW_GOP_MlEnd(MS_U8 u8GOP, struct hwregOut* pGOPHWout, struct hwregOut* pGOPComHWout);
INTERFACE MS_BOOL KDrv_HW_GOP_MlTblFlush(MS_U8 u8GOP);
INTERFACE MS_BOOL KDrv_HW_GOP_SetIPVersion(MS_U32 u32IpVer);
INTERFACE MS_BOOL KDrv_HW_GOP_SetAidType(MS_U8 u8GOP, EN_HW_GOP_AID_TYPE GOPAidType);
INTERFACE MS_BOOL KDrv_HW_GOP_SetSysDebugInfo(ST_HW_GOP_SYS_DEBUG_INFO *pGOPDebugInfo);
INTERFACE MS_U64 KDrv_HW_GOP_GetROI(ST_HW_GOP_GETROI *pstGOPRoi);
INTERFACE MS_BOOL KDrv_HW_GOP_SetVGOrder(EN_HW_GOP_VG_ORDER eVGOrder);

#endif //MDRV_GOP_H
