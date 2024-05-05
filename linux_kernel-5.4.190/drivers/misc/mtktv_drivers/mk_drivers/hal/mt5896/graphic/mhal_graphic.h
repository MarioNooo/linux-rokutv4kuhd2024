/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define GOP_REG_GROUP_NUM	(2)

#define GOP_GRAPHIC_REG_BASE	(0x1D49B200)
#define GOP_GRAPHIC_REG_SIZE	(0x2e00)

#define GOP_GRAPHIC_CUROR_REG_BASE	(0x1D49A600)
#define GOP_GRAPHIC_CUROR_REG_SIZE	(0x600)

#define GOP_GRAPHIC_BUS_OFFSET	(0x1C000000)
#define GOP_GRAPHIC_MIUBUS_OFFSET	(0x20000000)

#define GOP_GRAPHIC_CURSOR	(3)

#define GOP_GRAPHIC_A2RGB10_DEF0	(0x3F)
#define GOP_GRAPHIC_A2RGB10_DEF1	(0x7E)
#define GOP_GRAPHIC_A2RGB10_DEF2	(0xBD)
#define GOP_GRAPHIC_A2RGB10_DEF3	(0xFF)
#define GOP_GRAPHIC_HBLK_PERIOD	(0x3)
#define GOPG_GRAPHIC_PREULTRA	(0x20)
#define GOPG_GRAPHIC_ULTRA	(0x30)
#define GOPG_GRAPHIC_URGULTRA	(0x3A)
#define GOPC_GRAPHIC_PREULTRA	(0x24)
#define GOPC_GRAPHIC_ULTRA	(0x36)
#define GOPC_GRAPHIC_URGULTRA	(0x40)
#define GOP_GRAPHIC_SCALING_MULITPLIE	(0x100000)

//---------------- AC_FULL/MSK ----------------
//  B3[31:24] | B2[23:16] | B1[15:8] | B0[7:0]
//  W32[31:16] | W21[23:8] | W10 [15:0]
//  DW[31:0]
//---------------------------------------------
#define AC_FULLB0           1
#define AC_FULLB1           2
#define AC_FULLB2           3
#define AC_FULLB3           4
#define AC_FULLW10          5
#define AC_FULLW21          6
#define AC_FULLW32          7
#define AC_FULLDW           8
#define AC_MSKB0            9
#define AC_MSKB1            10
#define AC_MSKB2            11
#define AC_MSKB3            12
#define AC_MSKW10           13
#define AC_MSKW21           14
#define AC_MSKW32           15
#define AC_MSKDW            16

//-------------------- Fld --------------------
//    wid[31:16] | shift[15:8] | ac[7:0]
//---------------------------------------------
#define Fld(wid, shft, ac)    (((uint32_t)wid<<16)|(shft<<8)|ac)
#define Fld_wid(fld)        (uint8_t)((fld)>>16)
#define Fld_shift(fld)       (uint8_t)((fld)>>8)
#define Fld_ac(fld)         (uint8_t)((fld))

#define _phy_to_miu_offset(MiuSel, Offset, PhysAddr) if (PhysAddr < ARM_MIU1_BASE_ADDR) \
                                                        {MiuSel = E_DRV_GOP_SEL_MIU0; Offset = PhysAddr;} \

#define GOP0_GRAPHIC_BNK_ADDR_OFST (0x0)
#define GOP1_GRAPHIC_BNK_ADDR_OFST (0xC00)
#define GOP2_GRAPHIC_BNK_ADDR_OFST (0x1400)
#define GOP3_GRAPHIC_BNK_ADDR_OFST (0x1C00)
#define GOPC_GRAPHIC_BNK_ADDR_OFST (0x0)

#define GOP_GRAPHIC_RGB565 (0x1)
#define GOP_GRAPHIC_ARGB4444 (0x2)
#define GOP_GRAPHIC_ARGB8888 (0x5)
#define GOP_GRAPHIC_ARGB1555 (0x6)
#define GOP_GRAPHIC_YUV422 (0x9)
#define GOP_GRAPHIC_RGB888 (0xE)
#define GOP_GRAPHIC_A2RGB10 (0xF)
#define GOP_GRAPHIC_ADDRUNIT	(0x20)

#define GOP_GRAPHIC_WORD_MASK	(0xFFFF)

#define PANEL_COMPATIBLE_STR "mtk,mt5896-panel"
#define PANEL_INFO_STR "panel_info"
#define PANEL_INFO_DLG_STR "panel_info_DLG"
#define PANEL_WIDTH_STR "resolution_width"
#define PANEL_HEIGHT_STR "resolution_height"
#define PANEL_CUS_INFO_STR "panel-cus-setting"
#define PANEL_CUS_MIRROR_STR "PanelMirrorMode"

#define GOP_WordUnit                                32

typedef enum
{
    E_GRAPHIC_GOP0 = 0,
    E_GRAPHIC_GOP1 = 1,
    E_GRAPHIC_GOP2 = 2,
    E_GRAPHIC_GOP3 = 3,
    E_GRAPHIC_GOP4 = 4,
}E_GRAPHIC_GOP_TYPE;

typedef enum
{
    E_GRAPHIC_OSDB0,
    E_GRAPHIC_OSDB1,
    E_GRAPHIC_OSDB_INVALID,
}E_GRAPHIC_GOP_OSDB;

struct ST_GOP_BANK_GROUP {
    MS_U32 u32P_Addr;
    MS_U32 u32P_Size;
    MS_U64 u64V_Addr;
};

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

void _HAL_GOP_Write16Reg(MS_U32 u32addr, MS_U16 u16val, MS_U16 mask);
void _HAL_GOP_Read16Reg(MS_U32 u32addr, MS_U16* pu16ret);
void _HAL_GOP_Write32Reg(MS_U32 u32addr, MS_U32 u32val);
void _HAL_GOP_Init(MS_U8 u8GOP);
void _HAL_GOP_GWIN_SetDstPlane(MS_U8 u8GOP, EN_DRV_GOP_DST_TYPE eDsttype, MS_BOOL bOnlyCheck);
void _HAL_GOP_SetBlending(MS_U8 u8GOP, MS_U8 u8win, MS_BOOL bEnable, MS_U8 u8coef);
MS_U8 _HAL_GOP_GetBnkOfstByGop(MS_U8 gop, MS_U32 *pBnkOfst);
void _HAL_GOP_SetHMirror(MS_U8 u8GOP,MS_BOOL bEnable);
void _HAL_GOP_SetVMirror(MS_U8 u8GOP,MS_BOOL bEnable);
void _HAL_GOP_OutputColor_EX(MS_U8 u8GOP,EN_DRV_GOP_OUTPUT_COLOR type);
void _HAL_GOP_MIUSel(MS_U8 u8GOP, E_DRV_GOP_SEL_TYPE MiuSel);
void _HAL_GOP_EnableTransClr_EX(MS_U8 u8GOP,EN_DRV_GOP_TRANSCLR_FMT eFmt, MS_BOOL bEnable);
MS_BOOL _HAL_GOP_IsVMirrorOn(MS_U8 u8GOPnum);
void _HAL_GOP_SetGwinInfo(MS_U8 u8GOP, MS_U8 u8win,DRV_GWIN_INFO WinInfo);
void _HAL_GOP_SetStretchWin(MS_U8 u8GOP,MS_U16 u16x, MS_U16 u16y, MS_U16 u16width, MS_U16 u16height);
void _HAL_GOP_SetHScale(MS_U8 u8GOP,MS_BOOL bEnable, MS_U16 u16src, MS_U16 u16dst);
void _HAL_GOP_SetVScale(MS_U8 u8GOP,MS_BOOL bEnable, MS_U16 u16src, MS_U16 u16dst);
void _HAL_GOP_GWIN_Enable(MS_U8 u8GOP, MS_U8 u8win,MS_BOOL bEnable);
void _HAL_GOP_UpdateReg(MS_U8 u8Gop);
void _HAL_GOP_SetForceWrite(MS_BOOL bEnable);
void _HAL_GOP_GettXCTiming(MS_U16* pu16Width, MS_U16* pu16Height);
MS_BOOL MHal_GRAPHIC_GetPixelModeSupport(void);
MS_BOOL MHal_GRAPHIC_IsInit(MS_U8 u8GOPNum);
MS_BOOL MHal_GRAPHIC_InitGopPart(MS_U8 u8GOPNum);
MS_BOOL MHal_GRAPHIC_SetIOBase(MS_U8 u8Idx, MS_U32 u32P_Addr, MS_U64 u32P_Size, MS_U64 u64V_Addr);
void MHal_GRAPHIC_Write2byteMask_Coda(MS_U32 u32P_Addr, MS_U32 u32Value, MS_U32 u32fld);
MS_U32 MHal_GRAPHIC_Read2byte_Coda(MS_U32 u32P_Addr);
MS_U32 MHal_GRAPHIC_Read2bytemask_Coda(MS_U32 u32P_Addr, MS_U32 u32fld);
MS_BOOL MHal_GRAPHIC_SetOSDBEnable(MS_BOOL bEnable, E_GRAPHIC_GOP_OSDB eOSDB);
MS_BOOL MHal_GRAPHIC_SetFormat(MS_U8 u8GOP, EN_DRV_GOPColorType eClrType);
MS_BOOL MHal_GRAPHIC_GetPanelSize(void);
MS_BOOL MHal_GRAPHIC_GetPanelMirrorMode(MS_BOOL *bHMirror_Pnl, MS_BOOL *bVMirror_Pnl);