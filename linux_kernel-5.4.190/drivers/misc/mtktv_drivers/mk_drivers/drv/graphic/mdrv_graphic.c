// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#define _MDRV_GRAPHIC_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#if defined(CONFIG_MIPS)
#elif defined(CONFIG_ARM)
#include <asm/io.h>
#endif

#include "mdrv_mstypes.h"
#include "mdrv_graphic.h"
#include "mhal_graphic.h"

void _MDrv_GRAPHIC_Init(MS_U8 u8GopIdx)
{
    _HAL_GOP_Init(u8GopIdx);
}

void _MDrv_GRAPHIC_SetHMirror(MS_U8 u8GOP,MS_BOOL bEnable)
{
    _HAL_GOP_SetHMirror(u8GOP,bEnable);
}

void _MDrv_GRAPHIC_SetVMirror(MS_U8 u8GOP,MS_BOOL bEnable)
{
    _HAL_GOP_SetVMirror(u8GOP, bEnable);
}

void _MDrv_GRAPHIC_OutputColor_EX(MS_U8 u8GOP,EN_DRV_GOP_OUTPUT_COLOR type)
{
    _HAL_GOP_OutputColor_EX(u8GOP, type);
}

void _MDrv_GRAPHIC_MIUSel(MS_U8 u8GOP, E_DRV_GOP_SEL_TYPE MiuSel)
{
    _HAL_GOP_MIUSel(u8GOP, MiuSel);
}

void _MDrv_GRAPHIC_SetGOPDst(MS_U8 u8GOP, EN_DRV_GOP_DST_TYPE eDsttype, MS_BOOL bOnlyCheck)
{
   _HAL_GOP_GWIN_SetDstPlane(u8GOP,eDsttype,bOnlyCheck);
}

void _MDrv_GRAPHIC_EnableTransClr_EX(MS_U8 u8GOP,EN_DRV_GOP_TRANSCLR_FMT eFmt, MS_BOOL bEnable)
{
    _HAL_GOP_EnableTransClr_EX(u8GOP, eFmt, bEnable);
}

void _MDrv_GRAPHIC_SetBlending(MS_U8 u8GOP,MS_U8 u8win, MS_BOOL bEnable, MS_U8 u8coef)
{
    _HAL_GOP_SetBlending(u8GOP, u8win, bEnable, u8coef);
}

void _MDrv_GOP_SetGwinInfo(MS_U8 u8GOP, MS_U8 u8win,DRV_GWIN_INFO WinInfo)
{
    _HAL_GOP_SetGwinInfo( u8GOP, u8win, WinInfo);
}

void _MDrv_GRAPHIC_SetStretchWin(MS_U8 u8GOP,MS_U16 u16x, MS_U16 u16y, MS_U16 u16width, MS_U16 u16height)
{
    _HAL_GOP_SetStretchWin(u8GOP, u16x, u16y, u16width, u16height);
}

void _MDrv_GRAPHIC_GetXCTiming(MS_U16* pu16Width, MS_U16* pu16Height)
{
    _HAL_GOP_GettXCTiming(pu16Width, pu16Height);
}

void _MDrv_GRAPHIC_SetHScale(MS_U8 u8GOP,MS_BOOL bEnable, MS_U16 u16src, MS_U16 u16dst)
{
    _HAL_GOP_SetHScale(u8GOP, bEnable, u16src, u16dst);
}

void _MDrv_GRAPHIC_SetVScale(MS_U8 u8GOP,MS_BOOL bEnable, MS_U16 u16src, MS_U16 u16dst)
{
    _HAL_GOP_SetVScale(u8GOP, bEnable, u16src, u16dst);
}

void _MDrv_GRAPHIC_GWIN_Enable(MS_U8 u8GOP, MS_U8 u8win,MS_BOOL bEnable)
{
    _HAL_GOP_GWIN_Enable(u8GOP, u8win, bEnable);
}

void _MDrv_GRAPHIC_UpdateReg(MS_U8 u8Gop)
{
    _HAL_GOP_UpdateReg(u8Gop);
}

void _MDrv_GRAPHIC_SetForceWrite(MS_BOOL bEnable)
{
    _HAL_GOP_SetForceWrite(bEnable);
}

MS_BOOL MDrv_GRAPHIC_GetPixelModeSupport(void)
{
    return MHal_GRAPHIC_GetPixelModeSupport();
}

MS_BOOL MDrv_GRAPHIC_IsInit(MS_U8 u8GOPNum)
{
    return MHal_GRAPHIC_IsInit(u8GOPNum);
}

MS_BOOL MDrv_GRAPHIC_InitGopPart(MS_U8 u8GOPNum)
{
    return MHal_GRAPHIC_InitGopPart(u8GOPNum);
}

MS_BOOL MDrv_GRAPHIC_SetIoBase(MS_U8 u8Idx, MS_U32 u32P_Addr, MS_U64 u32P_Size, MS_U64 u64V_Addr)
{
    return MHal_GRAPHIC_SetIOBase(u8Idx, u32P_Addr, u32P_Size, u64V_Addr);
}

MS_BOOL MDrv_GRAPHIC_GetPanelMirrorMode(MS_BOOL *bHMirror_Pnl, MS_BOOL *bVMirror_Pnl)
{
    return MHal_GRAPHIC_GetPanelMirrorMode(bHMirror_Pnl, bVMirror_Pnl);
}