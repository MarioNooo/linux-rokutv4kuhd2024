/* SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause */
/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2019 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2019 MediaTek Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mhal_gflip.c
/// @brief  MStar GFLIP DDI HAL LEVEL
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_GFLIP_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/kernel.h>   /* printk() */
#include <linux/io.h>

#include "mdrv_mstypes.h"
#include "mhal_gflip_reg.h"
#include "mdrv_gflip_st.h"
#include "mhal_gflip.h"
#include "GOP0_BKA4DE.h"
#include "GOP0_UDMA_BKA4DF.h"
#include "GOP0_HVSP_BKA4E1.h"
#include "GOPG_BKA4D9.h"

//=============================================================================
// Compile options
//=============================================================================


//=============================================================================
// Debug Macros
//=============================================================================
#define GFLIPHAL_DEBUG
#ifdef GFLIPHAL_DEBUG
    #define GFLIPHAL_PRINT(fmt, args...)      printk("[GFLIP (HAL Driver)][%05d] " fmt, __LINE__, ## args)
    #define GFLIPHAL_ASSERT(_cnd, _fmt, _args...)                   \
                                    if (!(_cnd)) {              \
                                        GFLIPHAL_PRINT(_fmt, ##_args);  \
                                        while(1);               \
                                    }
#else
    #define GFLIPHAL_PRINT(_fmt, _args...)
    #define GFLIPHAL_ASSERT(_cnd, _fmt, _args...)
#endif

//=============================================================================
// Macros
//=============================================================================
#define LO16BIT(x)  (x & 0x0000ffff)
#define HI16BIT(x)  (x >> 16)
#define EX2BIT(x)   (((((MS_PHY)(x)) >> 32UL) & 0x3UL) << 14UL)


struct ST_GOP_BANK {
    MS_U32 u32P_Addr;
    MS_U32 u32P_Size;
    MS_U64 u64V_Addr;
};

//=============================================================================
// Local Variables
//=============================================================================
static volatile MS_U16 _u16GflipGOPDst[MAX_GOP_SUPPORT] = { GFLIP_GOP_DST_OP0 };

//=============================================================================
// Global Variables
//=============================================================================
static struct ST_GOP_BANK gstGopBkTbl[GOP_REG_NUM];
static MS_U32 gu32GopBaseBnk[MAX_GOP_SUPPORT][GOP_REG_NUM_PER_BANK];
static MS_U32 gu32GopUdmaBnk[MAX_GOP_SUPPORT][GOP_REG_NUM_PER_BANK];
static MS_U32 gu32GopHvspBnk[MAX_GOP_SUPPORT][GOP_REG_NUM_PER_BANK];
static MS_U32 gu32GopGlobalBnk[GOP_REG_NUM_PER_BANK];

//=============================================================================
// Local Function Prototypes
//=============================================================================
MS_U32 MHal_GFLIP_GetValidGWinIDPerGOPIdx(MS_U32 u32GopIdx, MS_U32 u32GwinIdx);
MS_S32 _MHal_GFLIP_DirectSerialDiff(MS_U16 u16TagID1,  MS_U16 u16TagID2);
MS_BOOL _MHal_GFLIP_SetGopDstClk(MS_U32 u32GopIdx, MS_U16 u16GopDst);

//=============================================================================================
// Local Function
//=============================================================================================
uint32_t _MHal_GFLIP_FldMapping(
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

void _MHal_GFLIP_Write2byte(
    uint32_t u32P_Addr,
    uint32_t u32Value)
{
    int i;
    uint32_t PA_Offset = 0x00;
    uint64_t VA_Offset = 0x00;

    for (i = 0; i < GOP_REG_NUM; i++) {
        if(gstGopBkTbl[i].u32P_Addr <= u32P_Addr &&
                u32P_Addr < gstGopBkTbl[i].u32P_Addr + gstGopBkTbl[i].u32P_Size) {
            PA_Offset = u32P_Addr - gstGopBkTbl[i].u32P_Addr;
            VA_Offset = gstGopBkTbl[i].u64V_Addr + PA_Offset;
            writel(u32Value, (volatile void *)((uintptr_t)VA_Offset));
            break;
        }
    }
}

void _MHal_GFLIP_Write2bytemask(
    uint32_t u32P_Addr,
    uint32_t u32Value,
    uint32_t fld)
{
    int i;
    uint32_t PA_Offset = 0x00;
    uint64_t VA_Offset = 0x00;
    uint32_t tmp = 0x00;

    for (i = 0; i < GOP_REG_NUM; i++) {
        if ((gstGopBkTbl[i].u32P_Addr <= u32P_Addr) &&
                (u32P_Addr < (gstGopBkTbl[i].u32P_Addr + gstGopBkTbl[i].u32P_Size))) {
            PA_Offset = u32P_Addr - gstGopBkTbl[i].u32P_Addr;
            VA_Offset = gstGopBkTbl[i].u64V_Addr + PA_Offset;
            tmp = _MHal_GFLIP_FldMapping(
                1,
                readl((const volatile void *)((uintptr_t)VA_Offset)),
                u32Value,
                fld);
            writel(tmp, (volatile void *)((uintptr_t)VA_Offset));
            break;
        }
    }
}

uint32_t _MHal_GFLIP_Read2byte(
    uint32_t u32P_Addr)
{
    int i;
    uint32_t PA_Offset = 0x00;
    uint64_t VA_Offset = 0x00;

    for (i = 0; i < GOP_REG_NUM; i++) {
        if(gstGopBkTbl[i].u32P_Addr <= u32P_Addr &&
                        u32P_Addr < gstGopBkTbl[i].u32P_Addr + gstGopBkTbl[i].u32P_Size) {
            PA_Offset = u32P_Addr - gstGopBkTbl[i].u32P_Addr;
            VA_Offset = gstGopBkTbl[i].u64V_Addr + PA_Offset;
            return readl((const volatile void *)((uintptr_t)VA_Offset));
        }
    }

    return -1;
}

uint32_t _MHal_GFLIP_Read2bytemask(
    uint32_t u32P_Addr,
    uint32_t fld)
{
    int i;
    uint32_t PA_Offset = 0x00;
    uint64_t VA_Offset = 0x00;
    uint32_t tmp;

    for (i = 0; i < GOP_REG_NUM; i++) {
        if(gstGopBkTbl[i].u32P_Addr <= u32P_Addr &&
                                u32P_Addr < gstGopBkTbl[i].u32P_Addr + gstGopBkTbl[i].u32P_Size) {
            PA_Offset = u32P_Addr - gstGopBkTbl[i].u32P_Addr;
            VA_Offset = gstGopBkTbl[i].u64V_Addr + PA_Offset;
            tmp = _MHal_GFLIP_FldMapping(
                0,
                readl((const volatile void *)((uintptr_t)VA_Offset)),
                0,
                fld);
            return tmp;
        }
    }

    return -1;
}

MS_BOOL MHal_GFLIP_SetIoBase(MS_U8 u8Idx, MS_U32 u32P_Addr, MS_U64 u32P_Size, MS_U64 u64V_Addr)
{
    if (u32P_Size == 0)
        return FALSE;

    if (u64V_Addr == 0)
        return FALSE;

    gstGopBkTbl[u8Idx].u32P_Addr = u32P_Addr - BUS_OFFSET;
    gstGopBkTbl[u8Idx].u32P_Size = u32P_Size;
    gstGopBkTbl[u8Idx].u64V_Addr = u64V_Addr;

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Set GOP Regs.
/// @param  u32GopIdx                  \b IN: The gop idx value
/// @param  u16BankIdx                  \b IN: The gop bank idx value, should be GFLIP_GOP_BANK_IDX_0/GFLIP_GOP_BANK_IDX_1
/// @return The gop bank offset value
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_U8 MHal_GFLIP_GetBankOffset(MS_U32 u32GopIdx, MS_U16 u16BankIdx)
{
    if(u32GopIdx<4)
    return (MS_U8)((GFLIP_GOP_BANKOFFSET * u32GopIdx) + u16BankIdx);
    else if(u32GopIdx==4)
    {
       if(u16BankIdx==0)
           return (MS_U8)(GFLIP_GOP4_BANK0);
       else if(u16BankIdx==1)
           return (MS_U8)(GFLIP_GOP4_BANK1);
       else if(u16BankIdx==2)
           return (MS_U8)(GFLIP_GOP4_BANK2);
    }
    return 0;
}

MS_BOOL _GetBnkOffsetByGop(MS_U32 gop, MS_S64 *pBnkOfst)
{
    if (gop >= MAX_GOP_SUPPORT) {
        return FALSE;
    }

    if (gop == E_GOP0)
        *pBnkOfst = GOP0_BNK_ADDR_OFST;
    else if (gop == E_GOP1)
        *pBnkOfst = GOP1_BNK_ADDR_OFST;
    else if (gop == E_GOP2)
        *pBnkOfst = GOP2_BNK_ADDR_OFST;
    else if (gop == E_GOP3)
        *pBnkOfst = GOP3_BNK_ADDR_OFST;
    else if (gop == E_GOP4)
        *pBnkOfst = GOP4_BNK_ADDR_OFST;

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Set GOP Regs.
/// @param  u32GopIdx                  \b IN: The gop idx value
/// @param  u16BankIdx                  \b IN: The gop bank idx value, should be GFLIP_GOP_BANK_IDX_0/GFLIP_GOP_BANK_IDX_1
/// @param  u16Addr                  \b IN: The gop reg address
/// @param  u16Val                  \b IN: The gop reg value
/// @param  u16Mask                  \b IN: The gop reg mask
/// @return void
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
void MHal_GFLIP_WriteGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16 u16Val, MS_U16 u16Mask)
{
    MS_U16 u16RegVal;

   // GFLIPHAL_ASSERT(((u32GopIdx==GFLIP_GOP_IDX_4G)||(u32GopIdx==GFLIP_GOP_IDX_2G)), "Invalid GopIdx, Currently Only support Gop_4G and Gop_2G!\n");

    if (u32GopIdx < MAX_GOP_SUPPORT)
    {
        u16RegVal = GOP_REG(u16Addr, MHal_GFLIP_GetBankOffset(u32GopIdx, u16BankIdx));

        u16RegVal &= ~u16Mask;
        u16RegVal |= (u16Val & u16Mask);

        GOP_REG(u16Addr, MHal_GFLIP_GetBankOffset(u32GopIdx, u16BankIdx)) = u16RegVal;
    }
}

//-------------------------------------------------------------------------------------------------
/// Get GOP Regs.
/// @param  u32GopIdx                  \b IN: The gop idx value
/// @param  u16BankIdx                  \b IN: The gop bank idx value, should be GFLIP_GOP_BANK_IDX_0/GFLIP_GOP_BANK_IDX_1
/// @param  u16Addr                  \b IN: The gop reg address
/// @param  *u16Val                  \b IN: The return gop reg value
/// @return void
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
void MHal_GFLIP_ReadGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16* u16Val)
{
    if (u32GopIdx < MAX_GOP_SUPPORT)
    {
        *u16Val = GOP_REG(u16Addr, MHal_GFLIP_GetBankOffset(u32GopIdx, u16BankIdx));
    }
    else
    {
        *u16Val = 0;
    }
}

//-------------------------------------------------------------------------------------------------
/// Set GOP Regs.
/// @param  u32GopIdx                  \b IN: The gop idx value
/// @param  u32GwinIdx                  \b IN: The gop gwin idx value,
/// @return MS_U32 The valid GWinID for related Gop
/// @attention
/// <b>[GFLIP] <em>GOP0: 0/1/2/3; GOP1: 0/1; GOP2: 1; GOP3: 1</em></b>
//-------------------------------------------------------------------------------------------------
MS_U32 MHal_GFLIP_GetValidGWinIDPerGOPIdx(MS_U32 u32GopIdx, MS_U32 u32GwinIdx)
{
    u32GwinIdx = 0;

    return u32GwinIdx;
}

//-------------------------------------------------------------------------------------------------
/// Differ Two TagIDs.
/// @param  u16TagID1                  \b IN: The First TagID
/// @param  u16TagID2                  \b IN: The Second TagID
/// @return MS_S32: The different of two IDs.
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_S32 _MHal_GFLIP_DirectSerialDiff(MS_U16 u16TagID1,  MS_U16 u16TagID2)
{
    if(u16TagID1 < u16TagID2)
    {
        if((u16TagID2-u16TagID1)>0x7FFF)
         {
             return (MS_S32)(0xFFFFUL-u16TagID2+u16TagID1+1);
         }
        else
            return -(MS_S32)(u16TagID2-u16TagID1);
    }
    else
    {
        if((u16TagID1-u16TagID2)>0x7FFF)
         {
             return -(MS_S32)(0xFFFFUL-u16TagID1+u16TagID2+1);
         }
        else
            return (MS_S32)(u16TagID1-u16TagID2);
    }
}

//-------------------------------------------------------------------------------------------------
/// Get Gop Current Dst Plane.
/// @param  u32GopIdx                  \b IN: The gop idx who is the owner
/// @return TRUE/FALSE
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_U16 MHal_GFLIP_GetGopDst(MS_U32 u32GopIdx)
{
    MS_U16 u16RegVal = 0;

    if(u32GopIdx >= MAX_GOP_SUPPORT)
    {
        GFLIPHAL_PRINT("Invalid GOP index = %u\n", u32GopIdx);
        return 0;
    }

    u16RegVal = _MHal_GFLIP_Read2bytemask(REG_0004_GOPG_BKA4D9, REG_0004_GOPG_BKA4D9_REG_GOP0_DEST_SEL);
    if(u16RegVal == GFLIP_GOP0_DST_IP && u32GopIdx == E_GOP0)
    {
        return GFLIP_GOP_DST_IPMAIN;
    }

    u16RegVal = _MHal_GFLIP_Read2bytemask(REG_01C8_GOPG_BKA4D9, REG_01C8_GOPG_BKA4D9_REG_OPOSD0_MUX);
    if(u16RegVal == u32GopIdx)
    {
        return GFLIP_GOP_DST_OSDB0;
    }

    u16RegVal = _MHal_GFLIP_Read2bytemask(REG_0004_GOPG_BKA4D9, REG_0004_GOPG_BKA4D9_REG_VG_SEP_PATH);
    if(u16RegVal)
    {
        return GFLIP_GOP_DST_FRC;
    }
    else
    {
        return GFLIP_GOP_DST_OP0;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
/// Set Gop Dst Plane Clock Src.
/// @param  u16GopDst                  \b IN: The gop Dst Clock Src
/// @return TRUE/FALSE
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL _MHal_GFLIP_SetGopDstClk(MS_U32 u32GopIdx, MS_U16 u16GopDst)
{
    return TRUE;
}

//=============================================================================
// HAL Driver Function
//=============================================================================
//-------------------------------------------------------------------------------------------------
/// Enable/Disable Vsync. Interrupt of gop.
/// @param  u32GopIdx                  \b IN: The gop idx which need enable/disable vsync. interrupt
/// @param bEnable                          \b IN: enable or disable
/// @return TRUE/FALSE
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_IntEnable(MS_U32 u32GopIdx, MS_BOOL bEnable)
{
    MS_U16 u16RegVal = 0;

    u16RegVal = _MHal_GFLIP_Read2bytemask(REG_0004_GOPG_BKA4D9, REG_0004_GOPG_BKA4D9_REG_VG_SEP_PATH);

    if (u16RegVal == 0) {
        _MHal_GFLIP_Write2bytemask(REG_0028_GOPG_BKA4D9, 0x1, REG_0028_GOPG_BKA4D9_REG_OCTG_INT_MASK);
    } else {
        _MHal_GFLIP_Write2bytemask(REG_0028_GOPG_BKA4D9, bEnable?0:1, REG_0028_GOPG_BKA4D9_REG_OCTG_INT_MASK);
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// check if the vsync. interrupt
/// @param  void
/// @return MS_BOOL: TRUE/FALSE
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_IsVSyncInt(MS_U32 u32GopIdx)
{
    MS_U16 u16RegVal = 0;

    u16RegVal = _MHal_GFLIP_Read2bytemask(REG_0004_GOPG_BKA4D9, REG_0004_GOPG_BKA4D9_REG_VG_SEP_PATH);

    if (u16RegVal == 0) {
        if (_MHal_GFLIP_Read2bytemask(REG_0028_GOPG_BKA4D9, REG_0028_GOPG_BKA4D9_REG_SCTC_INT)) {
            if (_MHal_GFLIP_Read2bytemask(REG_0028_GOPG_BKA4D9, REG_0028_GOPG_BKA4D9_REG_SCTC_INT))
                return TRUE;
            else
                return FALSE;
        } else {
            return FALSE;
        }
    } else {
        if (_MHal_GFLIP_Read2bytemask(REG_0028_GOPG_BKA4D9, REG_0028_GOPG_BKA4D9_REG_OCTG_INT)) {
            if (_MHal_GFLIP_Read2bytemask(REG_0028_GOPG_BKA4D9, REG_0028_GOPG_BKA4D9_REG_OCTG_INT))
                return TRUE;
            else
                return FALSE;
        } else {
            return FALSE;
        }
    }   
}

//-------------------------------------------------------------------------------------------------
/// Get the Gop Idx who fire the vsync. interrupt
/// @param  void
/// @return MS_U32: the gop idx
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_U32 MHal_GFLIP_GetIntGopIdx(void)
{
    return GFLIP_GOP_IDX_2G;
}

//-------------------------------------------------------------------------------------------------
/// Check GOP ACK.
/// @param  u32GopIdx                  \b IN: The gop idx
/// @return TRUE: GOP ACK
/// @return FALSE: GOP not ACK
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_IsGOPACK(MS_U32 u32GopIdx)
{
    MS_BOOL bUpdated = FALSE;
    MS_U16 u16RegVal = 0;

    switch(u32GopIdx) {
    case E_GOP0:
        u16RegVal = _MHal_GFLIP_Read2bytemask(REG_0020_GOPG_BKA4D9, REG_0020_GOPG_BKA4D9_REG_GOP0_CFG_ACK);
        break;
    case E_GOP1:
        u16RegVal = _MHal_GFLIP_Read2bytemask(REG_0020_GOPG_BKA4D9, REG_0020_GOPG_BKA4D9_REG_GOP1_CFG_ACK);
        break;
    case E_GOP2:
        u16RegVal = _MHal_GFLIP_Read2bytemask(REG_0020_GOPG_BKA4D9, REG_0020_GOPG_BKA4D9_REG_GOP2_CFG_ACK);
        break;
    case E_GOP3:
        u16RegVal = _MHal_GFLIP_Read2bytemask(REG_0020_GOPG_BKA4D9, REG_0020_GOPG_BKA4D9_REG_GOP3_CFG_ACK);
        break;
    case E_GOP4:
        u16RegVal = _MHal_GFLIP_Read2bytemask(REG_0020_GOPG_BKA4D9, REG_0020_GOPG_BKA4D9_REG_GOPC_CFG_ACK);
        break;
    default:
        break;
    }

    if (u16RegVal == 0)
        bUpdated = TRUE;
    else
        bUpdated = FALSE;

    return bUpdated;
}

//-------------------------------------------------------------------------------------------------
/// Trigger GOP.
/// @param  u32GopBitMask                  \b IN: The gop idx bit mask
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
void MHal_GFLIP_Fire(MS_U32 u32GopIdx)
{
    switch(u32GopIdx) {
        case E_GOP0:
            _MHal_GFLIP_Write2bytemask(REG_0020_GOPG_BKA4D9, 1, REG_0020_GOPG_BKA4D9_REG_GOP0_CFG_TRIG);
            break;
        case E_GOP1:
            _MHal_GFLIP_Write2bytemask(REG_0020_GOPG_BKA4D9, 1, REG_0020_GOPG_BKA4D9_REG_GOP1_CFG_TRIG);
            break;
        case E_GOP2:
            _MHal_GFLIP_Write2bytemask(REG_0020_GOPG_BKA4D9, 1, REG_0020_GOPG_BKA4D9_REG_GOP2_CFG_TRIG);
            break;
        case E_GOP3:
            _MHal_GFLIP_Write2bytemask(REG_0020_GOPG_BKA4D9, 1, REG_0020_GOPG_BKA4D9_REG_GOP3_CFG_TRIG);
            break;
        case E_GOP4:
            _MHal_GFLIP_Write2bytemask(REG_0020_GOPG_BKA4D9, 1, REG_0020_GOPG_BKA4D9_REG_GOPC_CFG_TRIG);
            break;
        default:
            break;
        }
        _MHal_GFLIP_Write2bytemask(REG_0020_GOPG_BKA4D9, 1, REG_0020_GOPG_BKA4D9_REG_GOPG_CFG_TRIG);
}

MS_BOOL _MHal_GFLIP_IsForceWrite(MS_U32 u32GopIdx)
{
    MS_U32 u32Temp = 0;

    switch(u32GopIdx) {
        case E_GOP0:
            u32Temp = _MHal_GFLIP_Read2bytemask(REG_0024_GOPG_BKA4D9, REG_0024_GOPG_BKA4D9_REG_GOP0_FORCE_WR);
            break;
        case E_GOP1:
            u32Temp = _MHal_GFLIP_Read2bytemask(REG_0024_GOPG_BKA4D9, REG_0024_GOPG_BKA4D9_REG_GOP1_FORCE_WR);
            break;
        case E_GOP2:
            u32Temp = _MHal_GFLIP_Read2bytemask(REG_0024_GOPG_BKA4D9, REG_0024_GOPG_BKA4D9_REG_GOP2_FORCE_WR);
            break;
        case E_GOP3:
            u32Temp = _MHal_GFLIP_Read2bytemask(REG_0024_GOPG_BKA4D9, REG_0024_GOPG_BKA4D9_REG_GOP3_FORCE_WR);
            break;
        case E_GOP4:
            u32Temp = _MHal_GFLIP_Read2bytemask(REG_0024_GOPG_BKA4D9, REG_0024_GOPG_BKA4D9_REG_GOPC_FORCE_WR);
            break;
        default:
            break;
    }
    if (u32Temp)
        return TRUE;
    else 
        return FALSE;
}

MS_BOOL _MHal_GFLIP_SetForceWrite(MS_U32 u32GopIdx, MS_BOOL bEnable)
{
    switch(u32GopIdx) {
        case E_GOP0:
            _MHal_GFLIP_Write2bytemask(REG_0024_GOPG_BKA4D9, bEnable, REG_0024_GOPG_BKA4D9_REG_GOP0_FORCE_WR);
            break;
        case E_GOP1:
            _MHal_GFLIP_Write2bytemask(REG_0024_GOPG_BKA4D9, bEnable, REG_0024_GOPG_BKA4D9_REG_GOP1_FORCE_WR);
            break;
        case E_GOP2:
            _MHal_GFLIP_Write2bytemask(REG_0024_GOPG_BKA4D9, bEnable, REG_0024_GOPG_BKA4D9_REG_GOP2_FORCE_WR);
            break;
        case E_GOP3:
            _MHal_GFLIP_Write2bytemask(REG_0024_GOPG_BKA4D9, bEnable, REG_0024_GOPG_BKA4D9_REG_GOP3_FORCE_WR);
            break;
        case E_GOP4:
            _MHal_GFLIP_Write2bytemask(REG_0024_GOPG_BKA4D9, bEnable, REG_0024_GOPG_BKA4D9_REG_GOPC_FORCE_WR);
            break;
        default:
            break;
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Kick off Flip to Gop HW.
/// @param  u32GopIdx                  \b IN: The gop idx who is the flip owner
/// @param  u32GwinIdx                 \b IN: The gwin idx who is the flip owner
/// @param  u32MainAddr                \b IN: The flip address for normal 2D display
/// @param  u32SubAddr                 \b IN: The 2nd fb flip address for GOP 3D display
/// @param  bForceWriteIn        \b IN: use forcewrite or bank wr
/// @return TRUE: success
/// @return FALSE: fail
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_SetFlipToGop(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_PHY64 u32MainAddr, MS_PHY64 u32SubAddr, MS_BOOL bForceWriteIn)
{
    MS_PHY64 phytmp = 0;
    MS_U32 u32SetVal = 0;
    MS_S64 s64BankOffSet = 0;

    _GetBnkOffsetByGop(u32GopIdx, &s64BankOffSet);
    phytmp = (u32MainAddr/GOP_WordUnit);
    u32SetVal = phytmp&0xFFFF;
    _MHal_GFLIP_Write2bytemask(s64BankOffSet + REG_0008_GOP0_BKA4DE, u32SetVal, REG_0008_GOP0_BKA4DE_REG_DRAM_BASE_STR_0);
    u32SetVal = (phytmp>>16)&0xFFFF;
    _MHal_GFLIP_Write2bytemask(s64BankOffSet + REG_000C_GOP0_BKA4DE, u32SetVal, REG_000C_GOP0_BKA4DE_REG_DRAM_BASE_STR_1);

    if(bForceWriteIn)
    {
        if(_MHal_GFLIP_IsForceWrite(u32GopIdx) == FALSE)
        {
             _MHal_GFLIP_SetForceWrite(u32GopIdx, TRUE);
             _MHal_GFLIP_SetForceWrite(u32GopIdx, FALSE);
        }
    }
    else
    {
        MHal_GFLIP_Fire(u32GopIdx);
    }
    return TRUE;
}

MS_BOOL MHal_GFLIP_SetTLBFlipToGop(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_PHY64 u32MainAddr, MS_PHY64 u32SubAddr, MS_BOOL bForceWriteIn)
{
    return TRUE;
}

#ifdef  GFLIP_MULTI_FLIP
MS_BOOL MHal_GFLIP_SetMultiFlipToGop(MS_GFLIP_MULTIINFO* pMultiFlipInfo,MS_BOOL bForceWriteIn)
{
    return TRUE;
}
MS_BOOL MHal_GFLIP_SetTLBMultiFlipToGop(MS_TLB_GFLIP_MULTIINFO* pTLBMultiFlipInfo,MS_BOOL bForceWriteIn)
{
    return TRUE;
}

void MHal_GFLIP_SetMultiFire(MS_U32 u32GopBitMask,MS_BOOL bForceWriteIn)
{
    return;
}
#endif
//-------------------------------------------------------------------------------------------------
/// Clear DWinIRQ if It happened.
/// @return TRUE: success
/// @return FALSE: fail
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_ClearDWINIRQ(GFLIP_DWININT_INFO *pGFlipDWinIntInfo)
{
    //GOP not support DWIN
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Handle HW Limitation -- Vsync Limitation.
/// @param  u32GopIdx                  \b IN: The gop idx who is the owner
/// @return TRUE: success
/// @return FALSE: fail
/// @attention
/// <b>[GFLIP] <em>The Limitation is found on U3/T2/T3. When GOP Src Clock is no signal and GOP
///     Vsync is on, Then there will be no chance to Reset GOP Vsync Interrupt. this will leads to
///     system looks like hang(busy handling interrupt always). T2/T3/U3 will Switch GOP Dst
///     to OP to fix the issue and later chips could Reset GOP to clear interrupt Bit.</em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_HandleVsyncLimitation(MS_U32 u32GopIdx)
{
    //Save Current Dst Clock Src:
    _u16GflipGOPDst[u32GopIdx] = MHal_GFLIP_GetGopDst(u32GopIdx);

    //Set Dst Clock Src to OP:
    _MHal_GFLIP_SetGopDstClk(u32GopIdx, GFLIP_GOP_DST_OP0);

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Restore HW Limitation -- Vsync Limitation.
/// @param  u32GopIdx                  \b IN: The gop idx who is the owner
/// @return TRUE: success
/// @return FALSE: fail
/// @attention
/// <b>[GFLIP] <em>The Limitation is found on U3/T2/T3. When GOP Src Clock is no signal and GOP
///     Vsync is on, Then there will be no chance to Reset GOP Vsync Interrupt. this will leads to
///     system looks like hang(busy handling interrupt always). T2/T3/U3 will Switch GOP Dst
///     to OP to fix the issue and later chips could Reset GOP to clear interrupt Bit.</em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_RestoreFromVsyncLimitation(MS_U32 u32GopIdx)
{
    //Restore Dst Clock Src:
    _MHal_GFLIP_SetGopDstClk(u32GopIdx, _u16GflipGOPDst[u32GopIdx]);

    //Restore Interrupt:
    MHal_GFLIP_IntEnable(u32GopIdx, TRUE);

    //Restore Current Dst Clock Src to OP:
    _u16GflipGOPDst[u32GopIdx] = GFLIP_GOP_DST_OP0;

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Check If the gived TagId has back or not.
/// @param  u32TagId                  \b IN: The TagId which need to be check
/// @return TRUE: back
/// @return FALSE: not back
/// @attention
/// <b>[GFLIP] <em> </em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_IsTagIDBack(MS_U16 u16TagId)
{
    /*MS_U16 u16TagIDHW;
    MS_U32 u32RegVal;

    u16TagIDHW = GE_REG(REG_GE_TAG);

    if(_MHal_GFLIP_DirectSerialDiff(u16TagIDHW, u16TagId) >= 0)
    {    //TagID is back!
          // printk("tagIDHW = %04x\n", tagID_HW);
        return TRUE;
    }

    u32RegVal = GE_REG(REG_GE_STATUS);
    if((u32RegVal & GE_CMDQ2_STATUS) < (16UL<<11)) //still has commands in queue.
    {
        return FALSE;
    }

    if((u32RegVal & GE_CMDQ1_STATUS) < (16UL<<3)) //still has commands in queue.
    {
        return FALSE;
    }

    if(GE_REG(REG_GE_FMT_BLT) & GE_EN_VCMDQ) //still has commands in VCMDQ.
    {
        u32RegVal = GE_REG(REG_GE_VQ_FIFO_STATUS_L);
        u32RegVal |= (GE_REG(REG_GE_VQ_FIFO_STATUS_H)&1)<<16;
        if(u32RegVal)
        {
            return FALSE;
        }
    }

    if(GE_REG(REG_GE_STATUS) & GE_BUSY) //GE still busy
    {
        return FALSE;
    }*/

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Enable or Disable VE capture function
/// @param  bEnable                  \b IN: TRUE: Enable, FALSE: Disable
/// @return TRUE: success
/// @return FALSE: fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_VECaptureEnable(MS_BOOL bEnable)
{
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Get the current frame number that the VE HW is writing in VE memory
/// For interlace mode, it is 0~3; for progressive mode, it is 0~2 .
/// @return TRUE: VE frame index of current HW writing
//-------------------------------------------------------------------------------------------------
MS_U8 MHal_GFLIP_GetFrameIdx(void)
{
    return 0;
}

MS_BOOL MHAL_GFLIP_SetOpmuxInfo(MS_SDR2HDR_INFO* pstSDR2HDRInfo)
{
    return TRUE;
}
//-------------------------------------------------------------------------------------------------
/// Check if VE HW is ready to downscaling data to memory(After VE reset)
/// @return TRUE: VE has start to downscaling
/// @return FALSE: VE is not ready
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_GFLIP_CheckVEReady(void)
{
    return TRUE;
}

MS_BOOL KHal_HW_GOP_SetGOPACKMask(MS_U16 u16GopMask)
{
    MS_U16 u16RegVal = 0;

    u16RegVal = _MHal_GFLIP_Read2byte(REG_0020_GOPG_BKA4D9);
    u16RegVal |= u16GopMask;
    _MHal_GFLIP_Write2byte(REG_0020_GOPG_BKA4D9, u16RegVal);

    return TRUE;
}

MS_BOOL KHal_HW_GOP_Trigger_MUX(void)
{
    _MHal_GFLIP_Write2bytemask(REG_0020_GOPG_BKA4D9, 0x1, REG_0020_GOPG_BKA4D9_REG_GOPG_CFG_TRIG);

    return TRUE;
}


MS_BOOL MHal_GFLIP_PowerState(MS_U16 u16PowerState)
{
    MS_U8 i = 0, j = 0;
    MS_U32  u32GOPBitMask = 0;
    MS_S64 s64BankOffSet = 0;

    switch(u16PowerState) {
    case GLFIP_SUSPEND:
        for (i = 0; i < MAX_GOP_SUPPORT; i++) {
            _GetBnkOffsetByGop(i, &s64BankOffSet);

            for (j = 1; j < GOP_REG_NUM_PER_BANK; j++) {
                gu32GopBaseBnk[i][j] = _MHal_GFLIP_Read2byte(s64BankOffSet + REG_0004_GOP0_BKA4DE + ((j-1) * 0x4));
                gu32GopUdmaBnk[i][j] = _MHal_GFLIP_Read2byte(s64BankOffSet + REG_0004_GOP0_UDMA_BKA4DF + ((j-1) * 0x4));
                gu32GopHvspBnk[i][j] = _MHal_GFLIP_Read2byte(s64BankOffSet + REG_0004_GOP0_HVSP_BKA4E1 + ((j-1) * 0x4));
            }
        }
        for (j = 1; j < GOP_REG_NUM_PER_BANK; j++) {
            gu32GopGlobalBnk[j] = _MHal_GFLIP_Read2byte(REG_0004_GOPG_BKA4D9 + ((j-1) * 0x4));
        }
        break;
    case GLFIP_RESUME:
        for (i = 0; i < MAX_GOP_SUPPORT; i++) {
            u32GOPBitMask |= (1 << i);
            _GetBnkOffsetByGop(i, &s64BankOffSet);
            for (j = 1; j < GOP_REG_NUM_PER_BANK; j++) {
                _MHal_GFLIP_Write2byte(s64BankOffSet + REG_0004_GOP0_BKA4DE + ((j-1) * 0x4), gu32GopBaseBnk[i][j]);
                _MHal_GFLIP_Write2byte(s64BankOffSet + REG_0004_GOP0_UDMA_BKA4DF + ((j-1) * 0x4), gu32GopUdmaBnk[i][j]);
                _MHal_GFLIP_Write2byte(s64BankOffSet + REG_0004_GOP0_HVSP_BKA4E1 + ((j-1) * 0x4), gu32GopHvspBnk[i][j]);
            }
        }
        for (j = 1; j < GOP_REG_NUM_PER_BANK; j++) {
            _MHal_GFLIP_Write2byte(REG_0004_GOPG_BKA4D9 + ((j-1) * 0x4), gu32GopGlobalBnk[j]);
        }
        KHal_HW_GOP_Trigger_MUX();
        KHal_HW_GOP_SetGOPACKMask(u32GOPBitMask);
        break;
    default:
        break;
    }

    return TRUE;
}
