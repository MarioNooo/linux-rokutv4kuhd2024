// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

//=============================================================================
// Include Files
//=============================================================================
#include <linux/kernel.h>   /* printk() */
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/moduleparam.h>

#include "mdrv_mstypes.h"
#include "mdrv_graphic.h"
#include "mhal_graphic.h"
#include "GOP0_BKA4DE.h"
#include "GOP0_UDMA_BKA4DF.h"
#include "GOPG_BKA4D9.h"

//=============================================================================
// Macros
//=============================================================================
#define ALIGN_CHECK(value,factor) ((value + factor-1) & (~(factor-1)))

//=============================================================================
// Global Variables
//=============================================================================
MS_BOOL bForceWriteIn = FALSE;
MS_U16 gu16PanelW = 0;
MS_U16 gu16PanelH = 0;
static struct ST_GOP_BANK_GROUP gGopBkGroup[GOP_REG_GROUP_NUM];
static int dlgmode = 0;

module_param(dlgmode, int, S_IRUSR);

uint32_t _MHal_GRAPHIC_Get_DTS_U32(struct device_node *np, const char *name)
{
    int u32Tmp = 0x0;
    int ret = 0;

    if (!np) {
        printk("[FBDEV][%s][%d] device_node is NULL\n", __FUNCTION__, __LINE__);
        return -ENODEV;
    }

    if (!name) {
        printk("[FBDEV][%s][%d] name is NULL\n", __FUNCTION__, __LINE__);
        return -ENODEV;
    }

    ret = of_property_read_u32(np, name, &u32Tmp);
    if (ret != 0x0) {
        printk("[FBDEV][%s][%d] ead u32 failed, name = %s\n",
			__FUNCTION__, __LINE__, name);
    }
    return u32Tmp;
}

uint32_t _MHal_GRAPHIC_FldMapping(
    uint8_t write, uint32_t tmp32, uint32_t val32, uint32_t fld)
{
    uint32_t t = 0;

    switch (Fld_ac(fld)) {
    case AC_FULLB0:
    case AC_FULLB1:
    case AC_FULLB2:
    case AC_FULLB3:
        if (write == 1)
            t = (tmp32&(~((uint32_t)0xFF<<
                (8*(Fld_ac(fld)-AC_FULLB0))))) |
                ((val32&0xFF)<<(8*(Fld_ac(fld)-AC_FULLB0)));
        else
            t = (tmp32&((uint32_t)0xFF<<
                (8*(Fld_ac(fld)-AC_FULLB0)))) >>
                (8*(Fld_ac(fld)-AC_FULLB0));
        break;
    case AC_FULLW10:
    case AC_FULLW21:
    case AC_FULLW32:
        if (write == 1)
            t = (tmp32&(~((uint32_t)0xFFFF<<
                (8*(Fld_ac(fld)-AC_FULLW10))))) |
                ((val32&0xFFFF)<<(8*(Fld_ac(fld)-AC_FULLW10)));
        else
            t = (tmp32&(((uint32_t)0xFFFF<<
                (8*(Fld_ac(fld)-AC_FULLW10))))) >>
                (8*(Fld_ac(fld)-AC_FULLW10));
        break;
    case AC_FULLDW:
        t = val32;
        break;
    case AC_MSKB0:
    case AC_MSKB1:
    case AC_MSKB2:
    case AC_MSKB3:
        if (write == 1)
            t = (tmp32&(~(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shift(fld))))) |
                (((val32&(((uint32_t)1<<Fld_wid(fld))-1))<<
                Fld_shift(fld)));
        else
            t = (tmp32&(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shift(fld)))) >>
                Fld_shift(fld);
        break;
    case AC_MSKW10:
    case AC_MSKW21:
    case AC_MSKW32:
        if (write == 1)
            t = (tmp32&(~(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shift(fld))))) |
                (((val32&(((uint32_t)1<<Fld_wid(fld))-1))<<
                Fld_shift(fld)));
        else
            t = (tmp32&(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shift(fld)))) >>
                Fld_shift(fld);
        break;
    case AC_MSKDW:
        if (write == 1)
            t = (tmp32&(~(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shift(fld))))) |
                (((val32&(((uint32_t)1<<Fld_wid(fld))-1))<<
                Fld_shift(fld)));
        else
            t = (tmp32&(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shift(fld)))) >>
                Fld_shift(fld);
        break;
    default:
        break;
    }
    return t;
}

MS_U16 _GOP_GetBPP(EN_DRV_GOPColorType fbFmt)
{
    MS_U16 u16bpp=0;

    switch (fbFmt)
    {
        case E_DRV_GOP_COLOR_RGB555_BLINK:
        case E_DRV_GOP_COLOR_RGB565:
        case E_DRV_GOP_COLOR_ARGB1555:
        case E_DRV_GOP_COLOR_RGBA5551:
        case E_DRV_GOP_COLOR_ARGB4444:
        case E_DRV_GOP_COLOR_RGBA4444:
        case E_DRV_GOP_COLOR_YUV422:
        case E_DRV_GOP_COLOR_RGB555YUV422:
        case E_DRV_GOP_COLOR_2266:
            u16bpp = 2;
            break;
        case E_DRV_GOP_COLOR_ARGB8888:
        case E_DRV_GOP_COLOR_ABGR8888:
            u16bpp = 4;
            break;
        case E_DRV_GOP_COLOR_I8:
            u16bpp = 1;
            break;
        default:
            u16bpp = 0xFFFF;
            break;
    }
    return u16bpp;
}

void _HAL_GOP_Init(MS_U8 u8GopIdx)
{
    MS_U32 bankoffset = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GopIdx, &bankoffset);

    //enable mixer4 src and layer
    switch (u8GopIdx) {
    case E_GRAPHIC_GOP0:
        MHal_GRAPHIC_Write2byteMask_Coda(REG_004C_GOPG_BKA4D9, 0x1, REG_004C_GOPG_BKA4D9_REG_MIXER4_SRC0_EN);
        MHal_GRAPHIC_Write2byteMask_Coda(REG_0050_GOPG_BKA4D9, 0x1, REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER0_EN);
        break;
    case E_GRAPHIC_GOP1:
        MHal_GRAPHIC_Write2byteMask_Coda(REG_004C_GOPG_BKA4D9, 0x1, REG_004C_GOPG_BKA4D9_REG_MIXER4_SRC1_EN);
        MHal_GRAPHIC_Write2byteMask_Coda(REG_0050_GOPG_BKA4D9, 0x1, REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER1_EN);
        break;
    case E_GRAPHIC_GOP2:
        MHal_GRAPHIC_Write2byteMask_Coda(REG_004C_GOPG_BKA4D9, 0x1, REG_004C_GOPG_BKA4D9_REG_MIXER4_SRC2_EN);
        MHal_GRAPHIC_Write2byteMask_Coda(REG_0050_GOPG_BKA4D9, 0x1, REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER2_EN);
        break;
    case E_GRAPHIC_GOP3:
        MHal_GRAPHIC_Write2byteMask_Coda(REG_004C_GOPG_BKA4D9, 0x1, REG_004C_GOPG_BKA4D9_REG_MIXER4_SRC3_EN);
        MHal_GRAPHIC_Write2byteMask_Coda(REG_0050_GOPG_BKA4D9, 0x1, REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER3_EN);
        MHal_GRAPHIC_Write2byteMask_Coda(REG_006C_GOPG_BKA4D9, 0x1, REG_006C_GOPG_BKA4D9_REG_MIXER2_SRC1_EN);
        MHal_GRAPHIC_Write2byteMask_Coda(REG_0070_GOPG_BKA4D9, 0x1, REG_0070_GOPG_BKA4D9_REG_MIXER2_LAYER1_EN);
        break;
    default:
        break;
    }

    //enable mixer2 src and layer
    MHal_GRAPHIC_Write2byteMask_Coda(REG_006C_GOPG_BKA4D9, 0x1, REG_006C_GOPG_BKA4D9_REG_MIXER2_SRC0_EN);
    MHal_GRAPHIC_Write2byteMask_Coda(REG_0070_GOPG_BKA4D9, 0x1, REG_0070_GOPG_BKA4D9_REG_MIXER2_LAYER0_EN);

    MHal_GRAPHIC_Write2byteMask_Coda(REG_01E0_GOPG_BKA4D9, 0x1, REG_01E0_GOPG_BKA4D9_REG_OSDB_VIDEO_DE_SEL);

    //set output GOP alpha
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0004_GOP0_BKA4DE, 0x0, REG_0004_GOP0_BKA4DE_REG_ALPHA_INV);

    //disable H scaling
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0058_GOP0_BKA4DE, 0x0, REG_0058_GOP0_BKA4DE_REG_HSP_RATIO_0);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_005C_GOP0_BKA4DE, 0x0, REG_005C_GOP0_BKA4DE_REG_HSP_RATIO_1);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_005C_GOP0_BKA4DE, 0x0, REG_005C_GOP0_BKA4DE_REG_HSP_EN);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0074_GOP0_BKA4DE, 0x0, REG_0074_GOP0_BKA4DE_REG_HSP_INI_RATIO_M1);

    //disable V scaling
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0060_GOP0_BKA4DE, 0x0, REG_0060_GOP0_BKA4DE_REG_VSP_RATIO_0);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0064_GOP0_BKA4DE, 0x0, REG_0064_GOP0_BKA4DE_REG_VSP_RATIO_1);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0064_GOP0_BKA4DE, 0x0, REG_0064_GOP0_BKA4DE_REG_VSP_EN);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_007C_GOP0_BKA4DE, 0x0, REG_007C_GOP0_BKA4DE_REG_VSP_INI_RATIO_M1);

    //index 0 ~ 3 of alpha value for A2RGB10 format
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0030_GOP0_BKA4DE, GOP_GRAPHIC_A2RGB10_DEF0, REG_0030_GOP0_BKA4DE_REG_ALPHA_DEF0);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0030_GOP0_BKA4DE, GOP_GRAPHIC_A2RGB10_DEF1, REG_0030_GOP0_BKA4DE_REG_ALPHA_DEF1);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0034_GOP0_BKA4DE, GOP_GRAPHIC_A2RGB10_DEF2, REG_0034_GOP0_BKA4DE_REG_ALPHA_DEF2);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0034_GOP0_BKA4DE, GOP_GRAPHIC_A2RGB10_DEF3, REG_0034_GOP0_BKA4DE_REG_ALPHA_DEF3);

    //Mixer 4 output graphic alpha
    MHal_GRAPHIC_Write2byteMask_Coda(REG_0048_GOPG_BKA4D9, 0x1, REG_0048_GOPG_BKA4D9_REG_MIXER4_OUT_GFX_ALPHA);

    //Mixer 2 output graphic alpha
    MHal_GRAPHIC_Write2byteMask_Coda(REG_0068_GOPG_BKA4D9, 0x1, REG_0068_GOPG_BKA4D9_REG_MIXER2_OUT_GFX_ALPHA);

    //Set hblk extendperiod
    MHal_GRAPHIC_Write2byteMask_Coda(REG_000C_GOPG_BKA4D9, GOP_GRAPHIC_HBLK_PERIOD, REG_000C_GOPG_BKA4D9_REG_HBLK_EXTEND_PERIOD);

    //Set dynamic ultra
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0054_GOP0_UDMA_BKA4DF, 0x0, REG_0054_GOP0_UDMA_BKA4DF_REG_REQ_PREULTRA_MASK);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0054_GOP0_UDMA_BKA4DF, 0x0, REG_0054_GOP0_UDMA_BKA4DF_REG_REQ_ULTRA_MASK);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0058_GOP0_UDMA_BKA4DF, 0x0, REG_0058_GOP0_UDMA_BKA4DF_REG_REQ_URGENT_MASK);

    if (u8GopIdx != GOP_GRAPHIC_CURSOR) {
        MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0054_GOP0_UDMA_BKA4DF, GOPG_GRAPHIC_PREULTRA, REG_0054_GOP0_UDMA_BKA4DF_REG_REQ_PREULTRA_LV);
        MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0054_GOP0_UDMA_BKA4DF, GOPG_GRAPHIC_ULTRA, REG_0054_GOP0_UDMA_BKA4DF_REG_REQ_ULTRA_LV);
        MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0058_GOP0_UDMA_BKA4DF, GOPG_GRAPHIC_URGULTRA, REG_0058_GOP0_UDMA_BKA4DF_REG_REQ_URGENT_LV);
    } else {
        MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0054_GOP0_UDMA_BKA4DF, GOPC_GRAPHIC_PREULTRA, REG_0054_GOP0_UDMA_BKA4DF_REG_REQ_PREULTRA_LV);
        MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0054_GOP0_UDMA_BKA4DF, GOPC_GRAPHIC_ULTRA, REG_0054_GOP0_UDMA_BKA4DF_REG_REQ_ULTRA_LV);
        MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0058_GOP0_UDMA_BKA4DF, GOPC_GRAPHIC_URGULTRA, REG_0058_GOP0_UDMA_BKA4DF_REG_REQ_URGENT_LV);
    }

    //set UDMA to RR mode
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0088_GOP0_UDMA_BKA4DF, 0x1, REG_0088_GOP0_UDMA_BKA4DF_REG_ADR_ARB_LBL_MODE);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0088_GOP0_UDMA_BKA4DF, 0x1, REG_0088_GOP0_UDMA_BKA4DF_REG_DAT_ARB_LBL_MODE);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0088_GOP0_UDMA_BKA4DF, 0x1, REG_0088_GOP0_UDMA_BKA4DF_REG_ADR_ARB_RR_MODE);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0088_GOP0_UDMA_BKA4DF, 0x1, REG_0088_GOP0_UDMA_BKA4DF_REG_DAT_ARB_RR_MODE);
    MHal_GRAPHIC_Write2byteMask_Coda(bankoffset + REG_0088_GOP0_UDMA_BKA4DF, 0x1, REG_0088_GOP0_UDMA_BKA4DF_REG_SUM_CNT_MASK_EN);

    //set free run mode when vg separate
    MHal_GRAPHIC_Write2byteMask_Coda(REG_00C0_GOPG_BKA4D9, 0x0, REG_00C0_GOPG_BKA4D9_REG_TG_VG_SYNC_EN);

    MHal_GRAPHIC_Write2byteMask_Coda(REG_001C_GOPG_BKA4D9, 0x1, REG_001C_GOPG_BKA4D9_REG_AFBC_CORE0_VS_REF);
}

void _HAL_GOP_SetHMirror(MS_U8 u8GOP,MS_BOOL bEnable)
{
    //no corresponding setting
}

void _HAL_GOP_SetVMirror(MS_U8 u8GOP,MS_BOOL bEnable)
{
    MS_U32 u32Bankoffset = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32Bankoffset);

    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0004_GOP0_BKA4DE, bEnable?1:0, REG_0004_GOP0_BKA4DE_REG_MIRROR_V);
}

void _HAL_GOP_OutputColor_EX(MS_U8 u8GOP,EN_DRV_GOP_OUTPUT_COLOR type)
{
    //no corresponding setting
}

void _HAL_GOP_MIUSel(MS_U8 u8GOP, E_DRV_GOP_SEL_TYPE MiuSel)
{
    //no corresponding setting
}

void _HAL_GOP_Write16Reg(MS_U32 u32addr, MS_U16 u16val, MS_U16 mask)
{
    //no corresponding setting
}

void _HAL_GOP_Read16Reg(MS_U32 u32addr, MS_U16* pu16ret)
{
    //no corresponding setting
}

void _HAL_GOP_Write32Reg(MS_U32 u32addr, MS_U32 u32val)
{
    //no corresponding setting
}

MS_U8 _HAL_GOP_GetBnkOfstByGop(MS_U8 gop, MS_U32 *pBnkOfst)
{
    if (gop == E_GRAPHIC_GOP0) {
        *pBnkOfst = GOP0_GRAPHIC_BNK_ADDR_OFST;
    } else if (gop == E_GRAPHIC_GOP1) {
        *pBnkOfst = GOP1_GRAPHIC_BNK_ADDR_OFST;
    } else if (gop == E_GRAPHIC_GOP2) {
        *pBnkOfst = GOP2_GRAPHIC_BNK_ADDR_OFST;
    } else if (gop == GOP_GRAPHIC_CURSOR) {
        *pBnkOfst = GOPC_GRAPHIC_BNK_ADDR_OFST;
    } else {
        *pBnkOfst = 0;
        return FALSE;
    }

    return TRUE;
}

void _HAL_GOP_GWIN_SetDstPlane(MS_U8 u8GOP, EN_DRV_GOP_DST_TYPE eDsttype, MS_BOOL bOnlyCheck)
{
    switch (eDsttype) {
    case E_DRV_GOP_DST_OP0:
        if (u8GOP == E_GRAPHIC_GOP0)
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0004_GOPG_BKA4D9, 0x0, REG_0004_GOPG_BKA4D9_REG_GOP0_DEST_SEL);
        else if(u8GOP == GOP_GRAPHIC_CURSOR)
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0004_GOPG_BKA4D9, 0x1, REG_0004_GOPG_BKA4D9_REG_GOPC_DEST_SEL);

        MHal_GRAPHIC_Write2byteMask_Coda(REG_0004_GOPG_BKA4D9, 0x0, REG_0004_GOPG_BKA4D9_REG_VG_SEP_PATH);
        MHal_GRAPHIC_SetOSDBEnable(TRUE, E_GRAPHIC_OSDB1);
        break;
    case E_DRV_GOP_DST_FRC:
        if (u8GOP == E_GRAPHIC_GOP0)
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0004_GOPG_BKA4D9, 0x0, REG_0004_GOPG_BKA4D9_REG_GOP0_DEST_SEL);
        else if(u8GOP == GOP_GRAPHIC_CURSOR)
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0004_GOPG_BKA4D9, 0x1, REG_0004_GOPG_BKA4D9_REG_GOPC_DEST_SEL);

        MHal_GRAPHIC_Write2byteMask_Coda(REG_0004_GOPG_BKA4D9, 0x1, REG_0004_GOPG_BKA4D9_REG_VG_SEP_PATH);
        break;
    default:
        break;
    }

    MHal_GRAPHIC_Write2byteMask_Coda(REG_0040_GOPG_BKA4D9, gu16PanelW, REG_0040_GOPG_BKA4D9_REG_MIXER4_DISP_HSIZE);
    MHal_GRAPHIC_Write2byteMask_Coda(REG_0044_GOPG_BKA4D9, gu16PanelH, REG_0044_GOPG_BKA4D9_REG_MIXER4_DISP_VSIZE);
    MHal_GRAPHIC_Write2byteMask_Coda(REG_0060_GOPG_BKA4D9, gu16PanelW, REG_0060_GOPG_BKA4D9_REG_MIXER2_DISP_HSIZE);
    MHal_GRAPHIC_Write2byteMask_Coda(REG_0064_GOPG_BKA4D9, gu16PanelH, REG_0064_GOPG_BKA4D9_REG_MIXER2_DISP_VSIZE);
}

void _HAL_GOP_EnableTransClr_EX(MS_U8 u8GOP,EN_DRV_GOP_TRANSCLR_FMT eFmt, MS_BOOL bEnable)
{
    MS_U32 u32BankOffSet = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32BankOffSet);

    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_0050_GOP0_BKA4DE, bEnable?1:0, REG_0050_GOP0_BKA4DE_REG_TRANS_CLR_EN);
}

MS_BOOL _HAL_GOP_IsVMirrorOn(MS_U8 u8GOPnum)
{
    MS_BOOL bRet = FALSE;
    MS_U32 u32BankOffSet = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOPnum, &u32BankOffSet);

    bRet = (MS_BOOL)MHal_GRAPHIC_Read2bytemask_Coda(u32BankOffSet + REG_0004_GOP0_BKA4DE, REG_0004_GOP0_BKA4DE_REG_MIRROR_V);

    return bRet;
}

void _HAL_GOP_SetGwinInfo(MS_U8 u8GOP, MS_U8 u8win,DRV_GWIN_INFO WinInfo)
{
    MS_U16 u16DispW = 0, u16DispH = 0, u16AddrLW = 0, u16AddrHW = 0;
    MS_U32 u32Bankoffset = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32Bankoffset);

    //Color fmt
    MHal_GRAPHIC_SetFormat(u8GOP, WinInfo.clrType);

    //buffer pitch
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0010_GOP0_BKA4DE, WinInfo.u16Pitch, REG_0010_GOP0_BKA4DE_REG_PIC_HSIZE);
    //buffer height
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0014_GOP0_BKA4DE, WinInfo.u16Height, REG_0014_GOP0_BKA4DE_REG_PIC_VSIZE);
    //buffer x crop
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0018_GOP0_BKA4DE, WinInfo.u16HStart, REG_0018_GOP0_BKA4DE_REG_PIC_X_POS);
    //buffer y crop
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_001C_GOP0_BKA4DE, WinInfo.u16VStart, REG_001C_GOP0_BKA4DE_REG_PIC_Y_POS);
    //dispaly width
    u16DispW = WinInfo.u16HEnd - WinInfo.u16HStart;
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0020_GOP0_BKA4DE, u16DispW, REG_0020_GOP0_BKA4DE_REG_DISP_HSIZE);
    //dispaly height
    u16DispH = WinInfo.u16VEnd - WinInfo.u16VStart;
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0024_GOP0_BKA4DE, u16DispH, REG_0024_GOP0_BKA4DE_REG_DISP_VSIZE);

    //address
    WinInfo.u64Addr = WinInfo.u64Addr/GOP_GRAPHIC_ADDRUNIT;
    u16AddrLW = (MS_U16)(WinInfo.u64Addr & GOP_GRAPHIC_WORD_MASK);
    u16AddrHW = (MS_U16)(((WinInfo.u64Addr >> 16) & GOP_GRAPHIC_WORD_MASK));
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0008_GOP0_BKA4DE, u16AddrLW, REG_0008_GOP0_BKA4DE_REG_DRAM_BASE_STR_0);
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_000C_GOP0_BKA4DE, u16AddrHW, REG_000C_GOP0_BKA4DE_REG_DRAM_BASE_STR_1);
}

void _HAL_GOP_SetStretchWin(MS_U8 u8GOP,MS_U16 u16x, MS_U16 u16y, MS_U16 u16width, MS_U16 u16height)
{
    MS_U32 u32BankOffSet = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32BankOffSet);
    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_0028_GOP0_BKA4DE, u16x, REG_0028_GOP0_BKA4DE_REG_DISP_HSTR);
    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_002C_GOP0_BKA4DE, u16y, REG_002C_GOP0_BKA4DE_REG_DISP_VSTR);
}

void _HAL_GOP_SetHScale(MS_U8 u8GOP,MS_BOOL bEnable, MS_U16 u16src, MS_U16 u16dst)
{
    MS_U16 u16RegLW = 0, u16RegHW = 0;
    MS_U32 u32BankOffSet = 0;
    MS_U64 u64Hratio = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32BankOffSet);

    u64Hratio = ((u16src * GOP_GRAPHIC_SCALING_MULITPLIE)/u16dst);
    u16RegLW = (MS_U16)(u64Hratio & GOP_GRAPHIC_WORD_MASK);
    u16RegHW = (MS_U16)((u64Hratio >> 16) & GOP_GRAPHIC_WORD_MASK);

    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_0058_GOP0_BKA4DE, u16RegLW, REG_0058_GOP0_BKA4DE_REG_HSP_RATIO_0);
    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_005C_GOP0_BKA4DE, u16RegHW, REG_005C_GOP0_BKA4DE_REG_HSP_RATIO_1);
    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_005C_GOP0_BKA4DE, bEnable, REG_005C_GOP0_BKA4DE_REG_HSP_EN);
    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_0068_GOP0_BKA4DE, u16dst, REG_0068_GOP0_BKA4DE_REG_HSP_OUT_HSIZE);
}

void _HAL_GOP_SetVScale(MS_U8 u8GOP,MS_BOOL bEnable, MS_U16 u16src, MS_U16 u16dst)
{
    MS_U16 u16RegLW = 0, u16RegHW = 0;
    MS_U32 u32BankOffSet = 0;
    MS_U64 u64Vratio = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32BankOffSet);

    u64Vratio = ((u16src * GOP_GRAPHIC_SCALING_MULITPLIE)/u16dst);
    u16RegLW = (MS_U16)(u64Vratio & GOP_GRAPHIC_WORD_MASK);
    u16RegHW = (MS_U16)((u64Vratio >> 16) & GOP_GRAPHIC_WORD_MASK);

    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_0060_GOP0_BKA4DE, u16RegLW, REG_0060_GOP0_BKA4DE_REG_VSP_RATIO_0);
    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_0064_GOP0_BKA4DE, u16RegHW, REG_0064_GOP0_BKA4DE_REG_VSP_RATIO_1);
    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_0064_GOP0_BKA4DE, bEnable, REG_0064_GOP0_BKA4DE_REG_VSP_EN);
    MHal_GRAPHIC_Write2byteMask_Coda(u32BankOffSet + REG_006C_GOP0_BKA4DE, u16dst, REG_006C_GOP0_BKA4DE_REG_VSP_OUT_VSIZE);
}

void _HAL_GOP_GettXCTiming(MS_U16* pu16Width, MS_U16* pu16Height)
{
    if (gu16PanelW != 0 && gu16PanelH != 0) {
        *pu16Width = gu16PanelW;
        *pu16Height = gu16PanelH;
    } else {
        if (MHal_GRAPHIC_GetPanelSize()) {
            *pu16Width = gu16PanelW;
            *pu16Height = gu16PanelH;
        } else {
            *pu16Width = 0;
            *pu16Height = 0;
        }
    }
}

void _HAL_GOP_GWIN_Enable(MS_U8 u8GOP, MS_U8 u8win,MS_BOOL bEnable)
{
    MS_U32 u32Bankoffset = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32Bankoffset);

    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0004_GOP0_BKA4DE, bEnable?0x1:0x0, REG_0004_GOP0_BKA4DE_REG_GOP_EN);
}

void _HAL_GOP_UpdateReg(MS_U8 u8Gop)
{
    MS_U32 u32Bankoffset = 0;

    _HAL_GOP_GetBnkOfstByGop(u8Gop, &u32Bankoffset);

    if (bForceWriteIn) {
        switch (u8Gop) {
        case E_GRAPHIC_GOP0:
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0024_GOPG_BKA4D9, 0x1, REG_0024_GOPG_BKA4D9_REG_GOP0_FORCE_WR);
            break;
        case E_GRAPHIC_GOP1:
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0024_GOPG_BKA4D9, 0x1, REG_0024_GOPG_BKA4D9_REG_GOP1_FORCE_WR);
            break;
        case E_GRAPHIC_GOP2:
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0024_GOPG_BKA4D9, 0x1, REG_0024_GOPG_BKA4D9_REG_GOP2_FORCE_WR);
            break;
        case E_GRAPHIC_GOP3:
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0024_GOPG_BKA4D9, 0x1, REG_0024_GOPG_BKA4D9_REG_GOPC_FORCE_WR);
            break;
        default:
            break;
        }
        MHal_GRAPHIC_Write2byteMask_Coda(REG_0024_GOPG_BKA4D9, 0x1, REG_0024_GOPG_BKA4D9_REG_GOPG_FORCE_WR);
    } else {
        switch (u8Gop) {
        case E_GRAPHIC_GOP0:
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0020_GOPG_BKA4D9, 0x1, REG_0020_GOPG_BKA4D9_REG_GOP0_CFG_TRIG);
            break;
        case E_GRAPHIC_GOP1:
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0020_GOPG_BKA4D9, 0x1, REG_0020_GOPG_BKA4D9_REG_GOP1_CFG_TRIG);
            break;
        case E_GRAPHIC_GOP2:
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0020_GOPG_BKA4D9, 0x1, REG_0020_GOPG_BKA4D9_REG_GOP2_CFG_TRIG);
            break;
        case E_GRAPHIC_GOP3:
            MHal_GRAPHIC_Write2byteMask_Coda(REG_0020_GOPG_BKA4D9, 0x1, REG_0020_GOPG_BKA4D9_REG_GOPC_CFG_TRIG);
            break;
        default:
            break;
        }
        MHal_GRAPHIC_Write2byteMask_Coda(REG_0020_GOPG_BKA4D9, 0x1, REG_0020_GOPG_BKA4D9_REG_GOPG_CFG_TRIG);
    }
}

void _HAL_GOP_SetForceWrite(MS_BOOL bEnable)
{
    bForceWriteIn = bEnable;
}

void _HAL_GOP_SetBlending(MS_U8 u8GOP, MS_U8 u8win, MS_BOOL bEnable, MS_U8 u8coef)
{
    MS_U32 u32Bankoffset = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32Bankoffset);

    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0004_GOP0_BKA4DE, bEnable?0x1:0x0, REG_0004_GOP0_BKA4DE_REG_PIX_ALPHA_EN);
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0030_GOP0_BKA4DE, u8coef, REG_0030_GOP0_BKA4DE_REG_ALPHA_DEF0);
}

MS_BOOL MHal_GRAPHIC_GetPixelModeSupport(void)
{
    return TRUE;
}

MS_BOOL MHal_GRAPHIC_IsInit(MS_U8 u8GOPNum)
{
    return TRUE;
}

MS_BOOL MHal_GRAPHIC_InitGopPart(MS_U8 u8GOPNum)
{
    return TRUE;
}

MS_BOOL MHal_GRAPHIC_SetIOBase(MS_U8 u8Idx, MS_U32 u32P_Addr, MS_U64 u32P_Size, MS_U64 u64V_Addr)
{
    if (u32P_Size == 0)
        return FALSE;

    if (u64V_Addr == 0)
        return FALSE;

    gGopBkGroup[u8Idx].u32P_Addr = u32P_Addr - GOP_GRAPHIC_BUS_OFFSET;
    gGopBkGroup[u8Idx].u32P_Size = u32P_Size;
    gGopBkGroup[u8Idx].u64V_Addr = u64V_Addr;

    return TRUE;
}

void MHal_GRAPHIC_Write2byteMask_Coda(MS_U32 u32P_Addr, MS_U32 u32Value, MS_U32 u32fld)
{
    MS_U8 i;
    MS_U32 tmp = 0x00;
    MS_U32 PA_Offset = 0x00;
    MS_U64 VA_Offset = 0x00;

    for (i = 0; i < GOP_REG_GROUP_NUM; i++) {
        if ((gGopBkGroup[i].u32P_Addr <= u32P_Addr) &&
                (u32P_Addr < (gGopBkGroup[i].u32P_Addr + gGopBkGroup[i].u32P_Size))) {
            PA_Offset = u32P_Addr - gGopBkGroup[i].u32P_Addr;
            VA_Offset = gGopBkGroup[i].u64V_Addr + PA_Offset;
            tmp = _MHal_GRAPHIC_FldMapping(
                1,
                readl((const volatile void *)((uintptr_t)VA_Offset)),
                u32Value,
                u32fld);
            writel(tmp, (volatile void *)((uintptr_t)VA_Offset));
            break;
        }
    }
}

MS_U32 MHal_GRAPHIC_Read2byte_Coda(MS_U32 u32P_Addr)
{
    MS_U8 i;
    MS_U32 PA_Offset = 0x00;
    MS_U64 VA_Offset = 0x00;

    for (i = 0; i < GOP_REG_GROUP_NUM; i++) {
        if(gGopBkGroup[i].u32P_Addr <= u32P_Addr &&
                        u32P_Addr < gGopBkGroup[i].u32P_Addr + gGopBkGroup[i].u32P_Size) {
            PA_Offset = u32P_Addr - gGopBkGroup[i].u32P_Addr;
            VA_Offset = gGopBkGroup[i].u64V_Addr + PA_Offset;
            return readl((const volatile void *)((uintptr_t)VA_Offset));
        }
    }

    return -1;
}

MS_U32 MHal_GRAPHIC_Read2bytemask_Coda(MS_U32 u32P_Addr, MS_U32 u32fld)
{
    MS_U8 i;
    MS_U32 tmp;
    MS_U32 PA_Offset = 0x00;
    MS_U64 VA_Offset = 0x00;

    for (i = 0; i < GOP_REG_GROUP_NUM; i++) {
        if(gGopBkGroup[i].u32P_Addr <= u32P_Addr &&
                                u32P_Addr < gGopBkGroup[i].u32P_Addr + gGopBkGroup[i].u32P_Size) {
            PA_Offset = u32P_Addr - gGopBkGroup[i].u32P_Addr;
            VA_Offset = gGopBkGroup[i].u64V_Addr + PA_Offset;
            tmp = _MHal_GRAPHIC_FldMapping(
                0,
                readl((const volatile void *)((uintptr_t)VA_Offset)),
                0,
                u32fld);
            return tmp;
        }
    }

    return 0;
}

MS_BOOL MHal_GRAPHIC_SetOSDBEnable(MS_BOOL bEnable, E_GRAPHIC_GOP_OSDB eOSDB)
{
    switch (eOSDB) {
    case E_GRAPHIC_OSDB0:
        MHal_GRAPHIC_Write2byteMask_Coda(REG_01F0_GOPG_BKA4D9, bEnable?0x1:0x0, REG_01F0_GOPG_BKA4D9_REG_OSDB0_EN);
        break;
    case E_GRAPHIC_OSDB1:
        MHal_GRAPHIC_Write2byteMask_Coda(REG_01F0_GOPG_BKA4D9, bEnable?0x1:0x0, REG_01F0_GOPG_BKA4D9_REG_OSDB1_EN);
        break;
    default:
        printk("[FBDEV][%s][%d] not support OSDB:%d\n", __FUNCTION__, __LINE__, eOSDB);
        return FALSE;
    }

    return TRUE;
}

MS_BOOL MHal_GRAPHIC_SetFormat(MS_U8 u8GOP, EN_DRV_GOPColorType eClrType)
{
    MS_U16 u16RegVal = 0, u16UVswap = 0, u16ADswap = 0;
    MS_U32 u32Bankoffset = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32Bankoffset);

    switch(eClrType) {
    case E_DRV_GOP_COLOR_ARGB4444:
        u16UVswap = 0;
        u16ADswap = 0;
        u16RegVal = GOP_GRAPHIC_ARGB4444;
        break;
    case E_DRV_GOP_COLOR_RGBA4444:
        u16UVswap = 0;
        u16ADswap = 1;
        u16RegVal = GOP_GRAPHIC_ARGB4444;
        break;
    case E_DRV_GOP_COLOR_ARGB8888:
        u16UVswap = 0;
        u16ADswap = 0;
        u16RegVal = GOP_GRAPHIC_ARGB8888;
        break;
    case E_DRV_GOP_COLOR_ABGR8888:
        u16UVswap = 1;
        u16ADswap = 0;
        u16RegVal = GOP_GRAPHIC_ARGB8888;
        break;
    case E_DRV_GOP_COLOR_ARGB1555:
        u16UVswap = 0;
        u16ADswap = 0;
        u16RegVal = GOP_GRAPHIC_ARGB1555;
        break;
    case E_DRV_GOP_COLOR_RGBA5551:
        u16UVswap = 0;
        u16ADswap = 1;
        u16RegVal = GOP_GRAPHIC_ARGB1555;
        break;
    case E_DRV_GOP_COLOR_RGB565:
        u16UVswap = 0;
        u16ADswap = 0;
        u16RegVal = GOP_GRAPHIC_RGB565;
        break;
    case E_DRV_GOP_COLOR_YUV422:
        u16RegVal = GOP_GRAPHIC_YUV422;
        u16UVswap = 0;
        break;
    default:
		printk("[FBDEV][%s][%d] not suppor color %d\n", __FUNCTION__, __LINE__, eClrType);
		return FALSE;
    }

    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0004_GOP0_BKA4DE, u16RegVal, REG_0004_GOP0_BKA4DE_REG_DATA_FMT);
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0004_GOP0_BKA4DE, u16UVswap, REG_0004_GOP0_BKA4DE_REG_UV_SWAP);
    MHal_GRAPHIC_Write2byteMask_Coda(u32Bankoffset + REG_0004_GOP0_BKA4DE, u16ADswap, REG_0004_GOP0_BKA4DE_REG_AD_SWAP);

    return TRUE;
}

MS_BOOL MHal_GRAPHIC_GetPanelSize(void)
{
    MS_BOOL bRet = TRUE;
    struct device_node *np, *infoNp;

    np = of_find_compatible_node(NULL, NULL, PANEL_COMPATIBLE_STR);
    if (np != NULL) {
        infoNp = of_find_node_by_name(np, dlgmode? PANEL_INFO_DLG_STR:PANEL_INFO_STR);
        if (infoNp != NULL) {
            gu16PanelW = (MS_U16)_MHal_GRAPHIC_Get_DTS_U32(infoNp, PANEL_WIDTH_STR);
            gu16PanelH = (MS_U16)_MHal_GRAPHIC_Get_DTS_U32(infoNp, PANEL_HEIGHT_STR);
        } else {
            printk("[FBDEV][%s][%d] can not find panel_info node\n", __FUNCTION__, __LINE__);
            bRet = FALSE;
        }
    } else {
        printk("[FBDEV][%s][%d] can not find panel device\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }

    return bRet;
}

MS_BOOL MHal_GRAPHIC_GetPanelMirrorMode(MS_BOOL *bHMirror_Pnl, MS_BOOL *bVMirror_Pnl)
{
    MS_BOOL bRet = TRUE;
    struct device_node *np, *infoNp;
    MS_U16 PanelMode = 0;

    np = of_find_compatible_node(NULL, NULL, PANEL_COMPATIBLE_STR);
    if (np != NULL) {
        infoNp = of_find_node_by_name(np, PANEL_CUS_INFO_STR);
        if (infoNp != NULL) {
            PanelMode = (MS_U16)_MHal_GRAPHIC_Get_DTS_U32(infoNp, PANEL_CUS_MIRROR_STR);
        } else {
            printk("[FBDEV][%s][%d] can not find panel-cus-setting node\n", __FUNCTION__, __LINE__);
            bRet = FALSE;
        }
    } else {
        printk("[FBDEV][%s][%d] can not find panel device\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }

    switch(PanelMode) {
    case 0:
        *bHMirror_Pnl = FALSE;
        *bVMirror_Pnl = FALSE;
        break;
    case 1:
        *bHMirror_Pnl = FALSE;
        *bVMirror_Pnl = TRUE;
        break;
    case 2:
        *bHMirror_Pnl = TRUE;
        *bVMirror_Pnl = FALSE;
        break;
    case 3:
        *bHMirror_Pnl = TRUE;
        *bVMirror_Pnl = TRUE;
        break;
    default:
        *bHMirror_Pnl = FALSE;
        *bVMirror_Pnl = FALSE;
        break;
    }
    return bRet;
}