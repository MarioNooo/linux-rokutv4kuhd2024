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
/// @file   drvSystem.h
/// @brief  System Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "mdrv_system_st.h"
#include "mst_platform.h"

#ifndef _DRV_SYSTEM_H_
#define _DRV_SYSTEM_H_

#if defined(CONFIG_MSTAR_KENYA) || defined(CONFIG_MSTAR_AMBER1) || defined(CONFIG_MSTAR_KERES)
   #define DEFINE_IN_MSTAR_CHIP_H 1
   #include <include/mstar/mstar_chip.h>
#endif

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define SYS_BOARD_NAME_MAX          32                                  ///< Maximum length of board name
#define SYS_PLATFORM_NAME_MAX       32                                  ///< Maximum length of playform name

#if 1
#define REG_SW_RESET_CPU_AEON                   0x1086
    //---------------------------------------------
    // definition for REG_SW_RESET_CPU_AEON   //reg[0x1086]
    #define AEON_SW_RESET                           BIT0

#define AEON_SPI_ADDR0                          0x0FFE

#include "mstar/mstar_chip.h"
#define MS_MIU_INTERVAL 0x80000000


#define AEON_REG_CTRL                           0x0FF0

    //---------------------------------------------
    // definition for AEON_REG_CTRL   //reg[0x0FF0]
    #define AEON_CTRL_EN                            BIT0
    #define AEON_CTRL_RST                           BIT1
    #define AEON_DWB_SWAP                           BIT3

#define REG_PCMCIA_PCM_MEM_IO_CMD           0x00
#define REG_PCMCIA_ADDR0                    0x02
#define REG_PCMCIA_ADDR1                    0x03
#define REG_PCMCIA_FIRE_READ_DATA_CLEAR     0x06
#define REG_PCMCIA_INT_MASK_CLEAR           0x0A
#define REG_PCMCIA_INT_MASK_CLEAR1          0x0B
#define REG_PCMCIA_STAT_INT_RAW_INT         0x0E
#define REG_PCMCIA_STAT_INT_RAW_INT1        0x0F
#define REG_PCMCIA_MODULE_VCC_OOB           0x10
#define PCMCIA_ATTRIBMEMORY_READ            0x03
#define PCMCIA_ATTRIBMEMORY_WRITE           0x04
#define PCMCIA_IO_READ                      0x05
#define PCMCIA_IO_WRITE                     0x06
#if defined(CONFIG_MSTAR_M7322)
    #define REG_PCMCIA_BASE                     0x60940//note: pcm bank 0x1609 only support M7322
#else
    #define REG_PCMCIA_BASE                     0x3440
#endif
#define PCMCIA_FIRE_COMMAND         BIT0
#define PCMCIA_CLEAN_STATE_RD_DONE  BIT1
#define PCMCIA_STATE_RD_DONE        BIT0
#define PCMCIA_STATE_BUS_IDLE       BIT1
#define PCMCIA_STATE_WD_DONE        BIT4
#define PCMCIA_MAX_DETECT_COUNT     1
#define PCMCIA_MAX_POLLING_COUNT    20000
#define MAX_PCMCIA_CONFIGS              6       //!< The maximum number of configurations supported by a PCMCIA card
#define MAX_PCMCIA_STRLEN               (20)    //!< The maximum name of vendor/manufacturer/info strings
#define MAX_CIS_SIZE                    0x100   //!< The maximum size of a CIS, that is understood by this driver
#define PCMCIA_HW_TIMEOUT               3000
#define PCMCIA_HW_MAX_RETRY_COUNT       1000      //  PCMCIA hardware register maximum access times
#define PCMCIA_IRQ_ENABLE               1       // FIXME_ALEC

#endif

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

/// System output pad switch type
typedef enum
{
    E_SYS_PAD_MSD5010_SM2_IIC2,                                         ///< 5010 SM2, IIC2
    E_SYS_PAD_MSD5011_SM2_IIC2,                                         ///< 5011 SM2, IIC2
    E_SYS_PAD_MSD5015_GPIO,                                             ///< 5015 GPIO
    E_SYS_PAD_MSD5018_SM2,                                              ///< 5018 SM2
} SYS_PadType;

/// System information
typedef struct
{
    /// Software information
    struct
    {
        U8                          Board[SYS_BOARD_NAME_MAX];          ///< Board name
        U8                          Platform[SYS_PLATFORM_NAME_MAX];    ///< Platform name
    } SWLib;
} SYS_Info;

/// Memory mapping type
typedef enum
{
    E_SYS_MMAP_LINUX_BASE,      //0
    E_SYS_MMAP_BIN_MEM,         //1
    E_SYS_MMAP_MAD_BASE,        //2
    E_SYS_MMAP_SCALER_DNR_BUF,  //3
    E_SYS_MMAP_RLD_BUF,         //4
    E_SYS_MMAP_MVD_SW,          //5
    E_SYS_MMAP_VD_3DCOMB,       //6
    E_SYS_MMAP_VE,              //7
    E_SYS_MMAP_TTX_BUF,         //8
    E_SYS_MMAP_MPOOL,           //9
    E_SYS_MMAP_EMAC_MEM,        //10
    E_SYS_MMAP_LINUX_MEM,       //11
    E_SYS_MMAP_SVD,             //12
    E_SYS_MMAP_SVD_ALL,         //13
    E_SYS_MMAP_MVD_FB,          //14
    E_SYS_MMAP_MVD_BS,          //15
    E_SYS_MMAP_POSD0_MEM,       //16
    E_SYS_MMAP_POSD1_MEM,       //17
    E_SYS_MMAP_TSP, // samuel, 20081107 //18
    E_SYS_MMAP_AUDIO_CLIP_MEM, // samuel, 20081107  //19
    E_SYS_MMAP_MBOX_SHM,                //20
    E_SYS_MMAP_CHAKRA_SUBSYSTEM,        //21
    E_SYS_MMAP_CHAKRA_FW,           //22
#ifdef CONFIG_MSTAR_KIP
    E_SYS_MMAP_AEON_SHM,               // 23
#endif
    E_SYS_MMAP_NUMBER,          //23

} SYS_Memory_Mapping;


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
BOOL MDrv_System_Init(void);
//BOOL MDrv_System_SwitchPad(SYS_PadType ePadType);
void MDrv_System_WDTEnable(BOOL bEnable);
void MDrv_System_WDTClear(void);
BOOL MDrv_System_WDTLastStatus(void);
void MDrv_System_WDTSetTime(U32 u32Ms);
void MDrv_System_ResetChip(void);
void MDrv_System_ResetCPU(void);


PMST_PANEL_INFO_t MDrv_SYS_GetPanelInfo(void);

void MDrv_SYS_PowerDown(int src);
void MDrv_SYS_PD_ADC_R(B16 bStatus);
void MDrv_SYS_PD_ADC_G(B16 bStatus);
void MDrv_SYS_PD_ADC_B(B16 bStatus);
void MDrv_SYS_PD_ADC_Y(B16 bStatus);
void MDrv_SYS_PD_GMC_P(B16 bStatus);
void MDrv_SYS_PD_GMC_Y(B16 bStatus);
void MDrv_SYS_PD_GMC_C(B16 bStatus);
void MDrv_SYS_PD_CVBS_Buffer(B16 bStatus);
void MDrv_SYS_PD_DAC_CVBS(B16 bStatus);
void MDrv_SYS_PD_DAC(B16 bStatus);
void MDrv_SYS_PD_FB_DAC(B16 bStatus);
void MDrv_SYS_PD_DAC_RGB(B16 bStatus);
void MDrv_SYS_PD_Audio(B16 bStatus);
void MDrv_SYS_PD_LVDS(B16 bStatus);
void MDrv_SYS_PD_VD(B16 bStatus);
void MDrv_SYS_PD_SVD(B16 bStatus);
void MDrv_SYS_PD_MVD_M4V(B16 bStatus);
void MDrv_SYS_PD_TSP(B16 bStatus);
void MDrv_SYS_PD_VE(B16 bStatus);
void MDrv_SYS_PD_RVD(B16 bStatus);
void MDrv_SYS_PD_STRLD(B16 bStatus);
void MDrv_SYS_PD_AEON(B16 bStatus);
void MDrv_SYS_PD_GOPG2(B16 bStatus);


unsigned int MDrv_SYS_GetDRAMLength(void);

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
extern int MDrv_SYS_GetMMAP(int type, unsigned int *addr, unsigned int *len);
#elif defined(CONFIG_ARM64)
extern int MDrv_SYS_GetMMAP(int type, u64 *addr, u64 *len);
#endif

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)

U32 MDrv_SYS_SetPanelInfo(U32 arg);
void MDrv_SYS_GetPanelRes(U32 arg);
void MDrv_SYS_GetPanelHStart(U32 argv);
void MDrv_SYS_GetGFXGOPPipelineDelay(U32 argv);

void MDrv_SYS_SetNexusPID(U32 argv);
void MDrv_SYS_GetNexusPID(U32 argv);

void MDrv_SYS_GetMBoxShareMemory(unsigned long argv);

void MDrv_SYS_GetMsBinInfo(U32 argv);
void MDrv_SYS_GetMIU1Base(U32 argv);
void MDrv_SYS_GetMIU1BusBase(U32 argv);
void MDrv_SYS_PrintMsg(U32 arg);

U32 MDrv_SYS_GetRawUART(U32 arg);
void MDrv_SYS_ReloadAeon( U32 arg ) ;
U32 MDrv_SYS_Timer(U32 arg) ;
U32 MDrv_SYS_RegOP(U32 arg);
extern void MDrv_SYS_MMAP_Dump( void ) ;

void MDrv_SYS_SetGFXGOPIndex(U32 argv);
void MDrv_SYS_GetGFXGOPIndex(U32 argv);

void MDrv_SYS_SetDisplayControllerSeparated(U32 argv);
U32  MDrv_SYS_GetDisplayControllerSeparated(void);
void MDrv_SYS_IsDisplayControllerSeparated(U32 argv);

void MDrv_SYS_SetNexus(U32 argv);
void MDrv_SYS_HasNexus(U32 argv);


#elif defined(CONFIG_ARM64)

U32 MDrv_SYS_SetPanelInfo(unsigned long arg);
void MDrv_SYS_GetPanelRes(unsigned long arg);
void MDrv_SYS_GetPanelHStart(unsigned long argv);
void MDrv_SYS_GetGFXGOPPipelineDelay(unsigned long argv);

void MDrv_SYS_SetNexusPID(unsigned long argv);
void MDrv_SYS_GetNexusPID(unsigned long argv);

void MDrv_SYS_GetMBoxShareMemory(unsigned long argv);

void MDrv_SYS_GetMsBinInfo(unsigned long argv);
void MDrv_SYS_GetMIU1Base(unsigned long argv);
void MDrv_SYS_GetMIU1BusBase(unsigned long argv);
void MDrv_SYS_PrintMsg(unsigned long arg);

U32 MDrv_SYS_GetRawUART(unsigned long arg);
void MDrv_SYS_ReloadAeon(unsigned long arg ) ;
U32 MDrv_SYS_Timer(unsigned long arg) ;
U32 MDrv_SYS_RegOP(unsigned long arg);
extern void MDrv_SYS_MMAP_Dump( void ) ;

void MDrv_SYS_SetGFXGOPIndex(unsigned long argv);
void MDrv_SYS_GetGFXGOPIndex(unsigned long argv);

void MDrv_SYS_SetDisplayControllerSeparated(unsigned long argv);
U32  MDrv_SYS_GetDisplayControllerSeparated(void);
void MDrv_SYS_IsDisplayControllerSeparated(unsigned long argv);

void MDrv_SYS_SetNexus(unsigned long argv);
void MDrv_SYS_HasNexus(unsigned long argv);


#endif

void MDrv_SYS_ReadMemory(void);
void MDrv_SYS_FlushMemory(void);

unsigned int MDrv_SYS_GetPowerStates(void);
unsigned int MDrv_SYS_GetGPIOIR(void);
unsigned int MDrv_SYS_GetGPIOIRType(void);

void MDrv_SYS_UtopiaMdbMkdir(void);

#endif // _DRV_SYSTEM_H_

