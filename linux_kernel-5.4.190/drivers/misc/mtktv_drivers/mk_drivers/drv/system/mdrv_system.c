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
/// file    drv_system_io.c
/// @brief  System Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,53)
#include <linux/uaccess.h>
#else
#include <asm/uaccess.h>
#endif
#include <linux/errno.h>
#include <linux/module.h>
#include <asm/io.h>     //robert.yang
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <generated/autoconf.h>
#include <linux/vmalloc.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39)
#include <linux/slab.h>
#endif
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"
#include "mdrv_system_io.h"
#include "mhal_system.h"
#include "board/Board.h"
#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
#include "chip_setup.h"
#endif

#include <linux/reboot.h>
#include <linux/vmalloc.h>

#if defined(CONFIG_ARM64)
#include <linux/math64.h>
#endif

#include <linux/proc_fs.h>

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
/*
#define DEF_STC_SYNTH               0x14000000                          // 216/2.5  = 86.4, 2.5(dec) = 00010.1(bin) = 0x14000000(u5.27)
#define MAX_STC_SYNTH               0x15000000                          // 216/2.75 = 78.55
#define MIN_STC_SYNTH               0x13000000                          // 216/2.25 = 96
*/

#define SYS_DEBUG                   1

#if SYS_DEBUG
#define SYS_PRINT(fmt, args...)     printk("[SYS][%06d]     " fmt, __LINE__, ## args)
#else
#define SYS_PRINT(fmt, args...)
#endif

#define ALL_INT_DISABLE()       local_irq_disable()
#define ALL_INT_ENABLE()        local_irq_enable()
#define printf                  printk

#define PCMCIA_NAND_SEM(x)          (x)

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
#define PCMCIA_LOOP_DELAY_WAIT_EMMC_DONE	(2000000)
#endif

// NOTE
// PE capability :
// PAD as output
//  PE = 0 / 1, don't care
// PAD as input
//  PE = 0, Hi-Z (input disable)
//  PE = 1, internal pull up
//
// for PAD = PCM_A(output), PE don't care
// for PAD = PCM_D(input/output), PE must be 1

#define PAD_PCM_D_ENABLE() \
    REG_ADDR(REG_CHIP_PCM_PE)   |= REG_CHIP_PCM_PE_MASK;

#define PAD_PCM_A_ENABLE() \
    REG_ADDR(REG_CHIP_PCM_PE1)   |= REG_CHIP_PCM_PE1_MASK;

#define PAD_PCM_D_DISABLE() \
    REG_ADDR(REG_CHIP_PCM_PE)   &= ~REG_CHIP_PCM_PE_MASK;

#define PAD_PCM_A_DISABLE() \
    REG_ADDR(REG_CHIP_PCM_PE1)   &= ~REG_CHIP_PCM_PE1_MASK;


#define KERNEL_PCMCIA_PERF 0

unsigned int MDrv_SYS_GetMIU0Length(void)
{
#if  (defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
       {
           if(REG_ADDR(REG_MAU0_MIU0_SIZE)&0x0100)
           {
               return MS_MIU_INTERVAL;//256M
           }
           return 0x08000000;//128
       }
#else
     return MS_MIU_INTERVAL;
#endif
}

#if defined(CONFIG_ARM) ||  defined(CONFIG_MIPS)
void MDrv_SYS_GetMIU1Base(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_GetMIU1Base(unsigned long argv)
#endif
{
    unsigned int u32Data;
    u32Data=MDrv_SYS_GetMIU0Length();
    if(copy_to_user((void*) argv, (void*)&u32Data, sizeof(unsigned int)))
        {
            printk( "copy_to_user error\n" ) ;
        }
}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_GetMIU1BusBase(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_GetMIU1BusBase(unsigned long argv)
#endif
{
    unsigned int u32Data=0;

    u32Data=0x08000000;
    if(copy_to_user((void*)argv, (void*)&u32Data, sizeof(unsigned int)))
        {
            printk( "copy_to_user error\n" ) ;
        }
}

//-------------------------------------------------------------------------------------------------
//  Export
//-------------------------------------------------------------------------------------------------
EXPORT_SYMBOL(MDrv_SYS_GetPanelInfo);
EXPORT_SYMBOL(MDrv_SYS_GetMIU0Length);

//-------------------------------------------------------------------------------------------------
//  Import
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
MST_PLATFORM_INFO_t gPlatformInfo;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,20)
DEFINE_SEMAPHORE(PfModeSem);
#else
DECLARE_MUTEX(PfModeSem);
#endif


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

/*
inline void _System_SetStcSynth(MS_U8 u8StcId, MS_U32 u32StcCw)
{
    REG(REG_STC0_SYNC_CW_LO + (8*u8StcId)) = (MS_U16)(u32StcCw & 0xFFFF);
    REG(REG_STC0_SYNC_CW_HI + (8*u8StcId)) = (MS_U16)(u32StcCw >> 16);
    REG(REG_DCSYNC_MISC) = (REG(REG_DCSYNC_MISC) & ~(STC0_CW_SEL_TSP<<(1*u8StcId))) | (UPDATE_STC0_CW << (1*u8StcId));
}
*/

static int gfx_gop_index=0; //default is GOP 0
static int display_controller_seperated=0;
static int using_nexus=0;
static int nexus_pid=-1;

static U32 u32MsBinAddr = 0x00;
static U32 u32MsBinSzie = 0x00;
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
U32 MDrv_SYS_SetPanelInfo(U32 arg)
#elif defined(CONFIG_ARM64)
U32 MDrv_SYS_SetPanelInfo(unsigned long arg)
#endif
{
    IO_SYS_PANEL_INFO_t info;

    SYS_PRINT("%s\n", __FUNCTION__);

    if (copy_from_user(&info, (IO_SYS_PANEL_INFO_t __user*)arg, sizeof(IO_SYS_PANEL_INFO_t)))
    {
        return -EFAULT;
    }

    if ((info.u16Len != sizeof(MST_PANEL_INFO_t)) ||
        (info.pPanelInfo == NULL))
    {
        return  -EFAULT;
    }
    if (copy_from_user((void*)&gPlatformInfo.panel, (void __user*)info.pPanelInfo, sizeof(MST_PANEL_INFO_t)))
    {
        return -EFAULT;
    }

    return 0;
}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_GetPanelRes(U32 arg)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_GetPanelRes(unsigned long arg)
#endif
{
    IO_SYS_PANEL_GET_RES_t info;

    info.u16Width  = gPlatformInfo.panel.u16Width;
    info.u16Height  = gPlatformInfo.panel.u16Height;


    if(copy_to_user((void*)arg, (void*)&info, sizeof(IO_SYS_PANEL_GET_RES_t)))
        {
            printk( "copy_to_user error\n" ) ;
        }
}


PMST_PANEL_INFO_t MDrv_SYS_GetPanelInfo(void)
{
    return &gPlatformInfo.panel;
}

void MDrv_SYS_MMAP_Dump( void ){
    return;
}

unsigned int MDrv_SYS_GetKernelMemLen(void)
{
	return 0;
}

void MDrv_SYS_PD_ADC_R(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2508) &= ~BIT5; //ADC_R
    }
    else
    {
        REG_ADDR(0x2508) |= BIT5; //ADC_R
    }
}

void MDrv_SYS_PD_ADC_G(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2508) &= ~BIT4; //ADC_G
    }
    else
    {
        REG_ADDR(0x2508) |= BIT4; //ADC_G
    }
}

void MDrv_SYS_PD_ADC_B(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2508) &= ~BIT3; //ADC_B
    }
    else
    {
        REG_ADDR(0x2508) |= BIT3; //ADC_B
    }
}

void MDrv_SYS_PD_ADC_Y(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2508) &= ~BIT12; //ADC_Y
    }
    else
    {
        REG_ADDR(0x2508) |= BIT12; //ADC_Y
    }
}

void MDrv_SYS_PD_GMC_P(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x250a) &= ~(BIT4 | BIT3 | BIT2); //GMC parameter
    }
    else
    {
        REG_ADDR(0x250a) |= (BIT4 | BIT3 | BIT2); //GMC parameter
    }
}

void MDrv_SYS_PD_GMC_Y(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x250a) &= ~BIT1; //GMC_Y
    }
    else
    {
        REG_ADDR(0x250a) |= BIT1; //GMC_Y
    }
}

void MDrv_SYS_PD_GMC_C(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x250a) &= ~BIT0; //GMC_C
    }
    else
    {
        REG_ADDR(0x250a) |= BIT0; //GMC_C
    }
}

void MDrv_SYS_PD_CVBS_Buffer(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2570) &= ~BIT0; //CVBS output buffer
    }
    else
    {
        REG_ADDR(0x2570) |= BIT0; //CVBS output buffer
    }
}

void MDrv_SYS_PD_DAC_CVBS(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x258c) |= BIT7; //DAC_CVBS
    }
    else
    {
        REG_ADDR(0x258c) &= ~BIT7; //DAC_CVBS
    }
}

void MDrv_SYS_PD_DAC(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2534) = 0x001d; //adc_ibias_rgb
        REG_ADDR(0x253a) = 0x001d; //adc_ibias_ycu
        REG_ADDR(0x25ba) |= BIT0;
        REG_ADDR(0x25b8) |= (BIT15 | BIT14);
        REG_ADDR(0x2606) &= 0xffc0; //phase_delta
        REG_ADDR(0x2606) = 0x0500; //phase_delta
    }
    else
    {
        REG_ADDR(0x2534) = 0x001d; //adc_ibias_rgb
        REG_ADDR(0x253a) = 0x001d; //adc_ibias_ycu
        REG_ADDR(0x25ba) |= BIT0;
        REG_ADDR(0x25b8) |= (BIT15 | BIT14);
        REG_ADDR(0x2606) &= 0xffc0; //phase_delta
        REG_ADDR(0x2606) = 0x0500; //phase_delta
    }
}

void MDrv_SYS_PD_FB_DAC(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2580) &= ~BIT6; //fast blanking ADC
    }
    else
    {
        REG_ADDR(0x2580) |= BIT6; //fast blanking ADC
    }
}

void MDrv_SYS_PD_DAC_RGB(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2536) = 0x3fff; //ADC RGB bias current control
        REG_ADDR(0x253c) = 0x3fff; //ADC RGB bias current control
    }
    else
    {
        REG_ADDR(0x2536) = 0x0555; //ADC RGB bias current control
        REG_ADDR(0x253c) = 0x0555; //ADC RGB bias current control
    }
}

void MDrv_SYS_PD_Audio(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x2ce0) = 0x0000; //Audio low power mode
    }
    else
    {
        REG_ADDR(0x2ce0) = 0x5550; //Audio low power mode
    }
}

void MDrv_SYS_PD_LVDS(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x32f0) &= 0xff00; //LVDS
        REG_ADDR(0x32f0) |= (BIT6 | BIT3 | BIT2); //LVDS
        REG_ADDR(0x32da) = 0x5550; //LVDS
        REG_ADDR(0x32dc) = 0x0555; //LVDS
        REG_ADDR(0x32de) = 0x0000; //LVDS
    }
    else
    {
        REG_ADDR(0x32f0) &= 0xff00; //LVDS
        REG_ADDR(0x32f0) |= (BIT6 | BIT2 | BIT1); //LVDS
        REG_ADDR(0x32da) = 0x5550; //LVDS
        REG_ADDR(0x32dc) = 0x0555; //LVDS
        REG_ADDR(0x32de) = 0x0000; //LVDS
    }
}

void MDrv_SYS_PD_VD(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e2e) &= ~BIT0; //VD
        REG_ADDR(0x1e2e) &= ~BIT4; //VD
        REG_ADDR(0x1e2c) &= ~BIT12; //VD
        REG_ADDR(0x3612) &= ~BIT14; //VD
        REG_ADDR(0x3800) &= ~BIT15; //VD
        REG_ADDR(0x3770) &= ~BIT3; //VD
        REG_ADDR(0x3514) &= ~BIT7; //VD
        REG_ADDR(0x130c) &= ~BIT0; //VD
        REG_ADDR(0x1246) &= ~BIT11; //VD
        REG_ADDR(0x1266) &= ~BIT8; //VD
        REG_ADDR(0x1266) &= ~BIT9; //VD
        REG_ADDR(0x1286) &= ~BIT8; //VD
        REG_ADDR(0x0666) &= ~BIT8; //VD
        REG_ADDR(0x0666) &= ~BIT9; //VD
    }
    else
    {
        REG_ADDR(0x3612) |= BIT14; //VD
        REG_ADDR(0x3800) |= BIT15; //VD
        REG_ADDR(0x3770) |= BIT3; //VD
        REG_ADDR(0x3514) |= BIT7; //VD
        REG_ADDR(0x130c) |= BIT0; //VD
        REG_ADDR(0x1246) |= BIT11; //VD
        REG_ADDR(0x1266) |= BIT8; //VD
        REG_ADDR(0x1266) |= BIT9; //VD
        REG_ADDR(0x1286) |= BIT8; //VD
        REG_ADDR(0x0666) |= BIT8; //VD
        REG_ADDR(0x0666) |= BIT9; //VD
        REG_ADDR(0x1e2e) |= BIT0; //VD
        REG_ADDR(0x1e2e) |= BIT4; //VD
        REG_ADDR(0x1e2c) |= BIT12; //VD
    }
}

void MDrv_SYS_PD_SVD(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e58) &= ~BIT0; //SVD
        REG_ADDR(0x1b14) &= ~BIT0; //SVD
        REG_ADDR(0x1b14) &= ~BIT1; //SVD
        REG_ADDR(0x1246) &= ~BIT12; //SVD
        REG_ADDR(0x1246) &= ~BIT13; //SVD
        REG_ADDR(0x1286) &= ~BIT0; //SVD
        REG_ADDR(0x1286) &= ~BIT6; //SVD
        REG_ADDR(0x0646) &= ~BIT7; //SVD
        REG_ADDR(0x0646) &= ~BIT8; //SVD
        REG_ADDR(0x0646) &= ~BIT9; //SVD
        REG_ADDR(0x0646) &= ~BIT10; //SVD
    }
    else
    {
        REG_ADDR(0x1b14) |= BIT0; //SVD
        REG_ADDR(0x1b14) |= BIT1; //SVD
        REG_ADDR(0x1246) |= BIT12; //SVD
        REG_ADDR(0x1246) |= BIT13; //SVD
        REG_ADDR(0x1286) |= BIT0; //SVD
        REG_ADDR(0x1286) |= BIT6; //SVD
        REG_ADDR(0x0646) |= BIT7; //SVD
        REG_ADDR(0x0646) |= BIT8; //SVD
        REG_ADDR(0x0646) |= BIT9; //SVD
        REG_ADDR(0x0646) |= BIT10; //SVD
        REG_ADDR(0x1e58) |= BIT0; //SVD
    }
}

void MDrv_SYS_PD_MVD_M4V(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e28) &= ~BIT12; //MVD & M4V
        REG_ADDR(0x1e28) &= ~BIT8; //MVD & M4V
        REG_ADDR(0x1e2a) &= ~BIT0; //MVD & M4V
        REG_ADDR(0x1e28) |= BIT9; //MVD & M4V
        REG_ADDR(0x1100) &= ~BIT0; //MVD & M4V
        REG_ADDR(0x1286) &= ~BIT6; //MVD & M4V
        REG_ADDR(0x1286) &= ~BIT11; //MVD & M4V
        REG_ADDR(0x0646) &= ~BIT9; //MVD & M4V
        REG_ADDR(0x0686) &= ~BIT11; //MVD & M4V
    }
    else
    {
        REG_ADDR(0x1100) |= BIT0; //MVD & M4V
        REG_ADDR(0x1286) |= BIT6; //MVD & M4V
        REG_ADDR(0x1286) |= BIT11; //MVD & M4V
        REG_ADDR(0x0646) |= BIT9; //MVD & M4V
        REG_ADDR(0x0686) |= BIT11; //MVD & M4V
        REG_ADDR(0x1e28) |= BIT12; //MVD & M4V
        REG_ADDR(0x1e28) |= BIT8; //MVD & M4V
        REG_ADDR(0x1e2a) |= BIT0; //MVD & M4V
    }
}

void MDrv_SYS_PD_TSP(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e26) &= ~BIT0; //TSP
        REG_ADDR(0x1e26) &= ~BIT8; //TSP
        REG_ADDR(0x1246) &= ~BIT10; //TSP
        REG_ADDR(0x1286) &= ~BIT1; //TSP
    }
    else
    {
        REG_ADDR(0x1e26) |= BIT0; //TSP
        REG_ADDR(0x1e26) |= BIT8; //TSP
        REG_ADDR(0x1246) |= BIT10; //TSP
        REG_ADDR(0x1286) |= BIT1; //TSP
    }
}

void MDrv_SYS_PD_VE(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e32) &= ~BIT8; //VE
        REG_ADDR(0x1e32) &= ~BIT12; //VE
        REG_ADDR(0x1e38) &= ~BIT0; //VE
        REG_ADDR(0x3b00) &= ~BIT4; //VE
        REG_ADDR(0x1246) &= ~BIT8; //VE
        REG_ADDR(0x1266) &= ~BIT10; //VE
        REG_ADDR(0x1266) &= ~BIT11; //VE
        REG_ADDR(0x0666) &= ~BIT5; //VE
        REG_ADDR(0x0666) &= ~BIT10; //VE
        REG_ADDR(0x0666) &= ~BIT11; //VE
    }
    else
    {
        REG_ADDR(0x3b00) &= 0xff00; //VE
        REG_ADDR(0x3b00) |= BIT4; //VE
        REG_ADDR(0x1246) |= BIT8; //VE
        REG_ADDR(0x1266) |= BIT10; //VE
        REG_ADDR(0x1266) |= BIT11; //VE
        REG_ADDR(0x0666) |= BIT5; //VE
        REG_ADDR(0x0666) |= BIT10; //VE
        REG_ADDR(0x0666) |= BIT11; //VE
        REG_ADDR(0x1e32) |= BIT8; //VE
        REG_ADDR(0x1e32) |= BIT12; //VE
        REG_ADDR(0x1e38) |= BIT0; //VE
    }
}

void MDrv_SYS_PD_RVD(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1286) |= BIT6; //RVD
        REG_ADDR(0x0646) |= BIT9; //RVD
        REG_ADDR(0x1e2a) |= BIT8; //RVD
    }
    else
    {
        REG_ADDR(0x1286) |= BIT6; //RVD
        REG_ADDR(0x0646) |= BIT9; //RVD
        REG_ADDR(0x1e2a) |= BIT8; //RVD
    }
}

void MDrv_SYS_PD_STRLD(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e44) &= ~BIT0; //STRLD
        REG_ADDR(0x1246) &= ~BIT3; //STRLD
        REG_ADDR(0x1246) &= ~BIT4; //STRLD
    }
    else
    {
        REG_ADDR(0x1e44) |= BIT0; //STRLD
        REG_ADDR(0x1246) |= BIT3; //STRLD
        REG_ADDR(0x1246) |= BIT4; //STRLD
    }
}


void MDrv_SYS_PD_GOPG2(B16 bStatus)
{
    if(bStatus == TRUE)
    {
        REG_ADDR(0x1e30) &= ~BIT8; //GOPG2
    }
    else
    {
        REG_ADDR(0x1e30) |= BIT8; //GOPG2
    }
}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_SetGFXGOPIndex(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_SetGFXGOPIndex(unsigned long argv)
#endif
{
    gfx_gop_index=(int)argv;
}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_GetGFXGOPIndex(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_GetGFXGOPIndex(unsigned long argv)
#endif
{

   if(copy_to_user((void*)argv, (void*)&gfx_gop_index, sizeof(gfx_gop_index)))
    {
         printk( "copy_to_user error\n" ) ;
    }

}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_SetDisplayControllerSeparated(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_SetDisplayControllerSeparated(unsigned long argv)
#endif
{
    display_controller_seperated=(int)argv;
}

U32 MDrv_SYS_GetDisplayControllerSeparated(void)
{
    return display_controller_seperated;
}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_IsDisplayControllerSeparated(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_IsDisplayControllerSeparated(unsigned long argv)
#endif
{

    if(copy_to_user((void*)argv, (void*)&display_controller_seperated, sizeof(display_controller_seperated)))
        {
             printk( "copy_to_user error\n" ) ;
        }

}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_SetNexus(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_SetNexus(unsigned long argv)
#endif
{
    using_nexus=(int)argv;
}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_HasNexus(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_HasNexus(unsigned long argv)
#endif
{

    if(copy_to_user((void*)argv, (void*)&using_nexus, sizeof(using_nexus)))
        {
             printk( "copy_to_user error\n" ) ;
        }

}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_SetNexusPID(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_SetNexusPID(unsigned long argv)
#endif
{
    nexus_pid=(int)argv;
}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_GetNexusPID(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_GetNexusPID(unsigned long argv)
#endif
{

    if(copy_to_user((void*)argv, (void*)&nexus_pid, sizeof(nexus_pid)))
        {
             printk( "copy_to_user error\n" ) ;
        }
}

#define PCMCIA_PHYS_REG_DATA            (0)
#define PCMCIA_PHYS_REG_COMMANDSTATUS   (1)
#define PCMCIA_PHYS_REG_SIZELOW         (2)
#define PCMCIA_PHYS_REG_SIZEHIGH        (3)
#define PCMCIA_STATUS_FREE              (0x40)  //!< PCMCIA Status Register Bit - The module can accept data
#define PCMCIA_COMMAND_HOSTCONTROL      (0x01)  //!< PCMCIA Command register bit - Host control

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_GetGFXGOPPipelineDelay(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_GetGFXGOPPipelineDelay(unsigned long argv)
#endif
{
    int gop_index = 0, value = 0;

    if(copy_from_user((void*)&gop_index,(void*)argv,sizeof(gop_index)))
        {
            printk( "copy_from_user error\n" ) ;
        }

    //should be chip and GOP dependent
    if(gop_index==0)
    {
        value=0x60;

    }
    else if(gop_index==1)
    {
        value=0x60;
    }


    if(copy_to_user((void*)argv, (void*)&value, sizeof(value)))
        {
            printk( "copy_to_user error\n" ) ;
        }
}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_GetPanelHStart(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_GetPanelHStart(unsigned long argv)
#endif
{

    int value=gPlatformInfo.panel.u16HStart;
    if(copy_to_user((void*)argv, (void*)&value, sizeof(value)))
        {
            printk( "copy_to_user error\n" ) ;
        }
}

void MDrv_SYS_GetMBoxShareMemory(unsigned long argv)
{
}

void MDrv_SYS_PowerDown(int src)
{
}

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_PrintMsg(U32 arg)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_PrintMsg(unsigned long arg)
#endif
{
    char buffer[256];
    if (copy_from_user(buffer, (char*)arg, 256))
    {
        printk("MDrv_SYS_PrintMsg() - Warning!! There are some bytes that could not be copied!\n");
        return;
    }

    printk("%s", buffer);
}


#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
void MDrv_SYS_GetMsBinInfo(U32 argv)
#elif defined(CONFIG_ARM64)
void MDrv_SYS_GetMsBinInfo(unsigned long argv)
#endif
{
    unsigned int au32Data[2];

    au32Data[0] = u32MsBinAddr;
    au32Data[1] = u32MsBinSzie;
    if(copy_to_user((void*)argv, (void*)au32Data, sizeof(unsigned int)*2))
        {
            printk( "copy_to_user error\n" ) ;
        }
}



void Chip_Flush_Memory(void);
void Chip_Read_Memory(void);
void MDrv_SYS_FlushMemory(void)
{
    Chip_Flush_Memory();
}

void MDrv_SYS_ReadMemory(void)
{
}

#if defined(CONFIG_ARM64)
//__aeabi_uldivmod
unsigned long long __aeabi_uldivmod(unsigned long long n, unsigned long long d)
{
    return div64_u64(n, d);
}

//__aeabi_ldivmod
long long __aeabi_ldivmod(long long n, long long d)
{
    return div64_s64(n, d);
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,20)
DEFINE_SEMAPHORE(UtopiaMdbSem);
#else
DECLARE_MUTEX(UtopiaMdbSem);
#endif
static BOOL bMdbDirIsCreate = FALSE;
void MDrv_SYS_UtopiaMdbMkdir()
{
    down(&UtopiaMdbSem);
    if(FALSE == bMdbDirIsCreate)
    {
        bMdbDirIsCreate = TRUE;
        proc_mkdir("utopia_mdb", NULL);
    }
    up(&UtopiaMdbSem);
}
EXPORT_SYMBOL(MDrv_SYS_UtopiaMdbMkdir);

static unsigned int gGpio_IR = 0;
module_param(gGpio_IR, uint, 0644);
MODULE_PARM_DESC(gGpio_IR, "don't use this, we are trying to remove this now");

unsigned int MDrv_SYS_GetGPIOIR(void)
{
    return gGpio_IR;
}
EXPORT_SYMBOL(MDrv_SYS_GetGPIOIR);

static unsigned int gGpio_IR_type = 0;
module_param(gGpio_IR_type, uint, 0644);
MODULE_PARM_DESC(gGpio_IR_type, "don't use this, we are trying to remove this now");

unsigned int MDrv_SYS_GetGPIOIRType(void)
{
    return gGpio_IR_type;
}
EXPORT_SYMBOL(MDrv_SYS_GetGPIOIRType);
MODULE_LICENSE("GPL");
