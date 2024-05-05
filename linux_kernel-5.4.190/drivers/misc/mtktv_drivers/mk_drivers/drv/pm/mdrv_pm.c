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
/// file    mdrv_mpool.c
/// @brief  Memory Pool Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>  //added
#include <linux/timer.h> //added
#include <linux/types.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/binfmts.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/cacheflush.h>
#include <linux/backlight.h>

#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif

#include "power.h"

#include "mdrv_mstypes.h"
#include "mdrv_mpm.h"
#include "mdrv_pm.h"
#include "mhal_pm.h"

//--------------------------------------------------------------------------------------------------
//  Forward declaration
//--------------------------------------------------------------------------------------------------
/* MMAP lay out. */
#define PM_OFFSET_DEFAULT       (0x0000)
#define PM_OFFSET_BT            (0x0070)
#define PM_OFFSET_USB           (0x0073)
#define PM_OFFSET_EWBS          (0x0075)
#define PM_OFFSET_BTW           (0x0078)
#define PM_OFFSET_VAD           (0x007A)
#define PM_OFFSET_POWER_DOWN    (0x00A0)
#define PM_OFFSET_SAR0          (0x00C0)
#define PM_OFFSET_SAR1          (0x00E0)
#define PM_OFFSET_LED           (0x0140)
#define PM_OFFSET_IR_VER        (0x0150)
#define PM_OFFSET_IR_NORMAL     (0x0160) //data_size + data(4* irkey_number)
#define PM_OFFSET_IR_EXTEND     (0x0200) //data_size + data(7* irkey_number)

/* MMAP Align. */
#define MMAP_DATA_ALIGN         (0x1000UL)
#define MMAP_DATA_SIZE          (0x1000UL)
#define MMAP_DRAM_ALIGN         (0x10000UL)
#define MMAP_DRAM_SIZE          (0x10000UL)

#define PM_SUPPORT_SAR_KEYS_MAX     5

/* Debug. */
#define DBG_CMD_SIZE            (15)
#define DBG_CMD_CAE             (0xF1)
#define DBG_CMD_PW              (0xF2)
#define DBG_CMD_STR             (0xF3)
#define DBG_CMD_DVI013          (PM_WAKEUPSRC_DVI)
#define DBG_CMD_DVI2            (PM_WAKEUPSRC_DVI2)
#define DBG_CMD_LAN             (PM_WAKEUPSRC_WOL)
#define DBG_CMD_VOC             (PM_WAKEUPSRC_VOICE)
#define DBG_CMD_USB             (PM_WAKEUPSRC_USB)
#define DBG_CMD_VAD             (PM_WAKEUPSRC_VAD)
#define DBG_CMD_CEC             (PM_WAKEUPSRC_CEC)
#define DBG_CMD_BTW             (0xF4)
#define DBG_CMD_WIFI            (PM_WAKEUPSRC_GPIO_WOWLAN)
#define DBG_CMD_BT              (PM_WAKEUPSRC_GPIO_WOBT)
#define DBG_CMD_EWBS            (PM_WAKEUPSRC_GPIO_WOEWBS)

//-------------------------------------------------------------------------------------------------
//  Data structure
//-------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//  Local variable
//--------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Golbal variable
//-------------------------------------------------------------------------------------------------
/* Other: Remove...? */
static u8                   gu8PmIr[32] = {0};
static u8                   gu8PmLed = PM_SOURCE_DISABLE;

#ifdef CONFIG_AMAZON_WOL
static atomic_t PM_WOL_EN = ATOMIC_INIT(0);
#endif

//-------------------------------------------------------------------------------------------------
//  Local function
//-------------------------------------------------------------------------------------------------
/* TODO: Remove. */
#ifdef CONFIG_AMAZON_WOL
static void _MDrv_PM_Set_Lan(u8 u8Mode)
{
    PM_WakeCfg_t    stCfgDef = {0};

    MDRV_PM_INFO("Azamon change WOL flag = %d.\n", stCfgDef.bPmWakeEnableWOL);
    MDrv_PM_Read_Key(PM_KEY_DEFAULT, (void *)&stCfgDef);
    stCfgDef.bPmWakeEnableWOL = (u8)atomic_read(&PM_WOL_EN);
    MDrv_PM_Write_Key(PM_KEY_DEFAULT, (void *)&stCfgDef, sizeof(stCfgDef));
}
#endif

/* TODO: Remove. */
#ifdef CONFIG_MSTAR_GPIO
static void _MDrv_PM_Set_Led(void)
{
    u8 u8LedPad = 0;

    u8LedPad = MDrv_PM_Get_PowerOffLed();
    MDRV_PM_INFO("Get LedPadNum = %d.\n", u8LedPad);
    if (u8LedPad != PM_SOURCE_DISABLE)
    {
        MDrv_GPIO_Set_Low(u8LedPad);
        MDRV_PM_INFO("======= set red led on ========\n");
    }
}
#endif

//-------------------------------------------------------------------------------------------------
//  Golbal function
//-------------------------------------------------------------------------------------------------
void MDrv_PM_Mapping(u16 u16Buf, unsigned long long ullAddr, unsigned long long ullSize)
{
    MDRV_PM_ERR("%s not support.\n", __func__);
}
EXPORT_SYMBOL(MDrv_PM_Mapping);

void MDrv_PM_Unmapping(u16 u16Buf)
{
    MDRV_PM_ERR("%s not support.\n", __func__);
}
EXPORT_SYMBOL(MDrv_PM_Unmapping);

void MDrv_PM_Reset_Key(u16 u16Key)
{
    MDRV_PM_ERR("%s not support.\n", __func__);
}

ssize_t MDrv_PM_Read_Key(u16 u16Key, const char *buf)
{
    size_t tSize = 0;

    if (!buf)
        return 0;

    switch (u16Key)
    {
        case PM_KEY_DEFAULT:
        {
            PM_WakeCfg_t *data = (PM_WakeCfg_t *)buf;
            bool enable = 0;
            extern s32 pm_get_wakeup_config(const char *name, bool *enable);

            tSize = sizeof(PM_WakeCfg_t);
            pm_get_wakeup_config("ir0", &enable);
            data->bPmWakeEnableIR = enable;
            enable = 0;
            pm_get_wakeup_config("keypad0", &enable);
            data->bPmWakeEnableSAR = enable;
            enable = 0;
            pm_get_wakeup_config("uart", &enable);
            data->bPmWakeEnableUART1 = enable;
            enable = 0;
            pm_get_wakeup_config("sync", &enable);
            data->bPmWakeEnableSYNC = enable;
            enable = 0;
            pm_get_wakeup_config("esync", &enable);
            data->bPmWakeEnableESYNC = enable;
            enable = 0;
            pm_get_wakeup_config("rtc0", &enable);
            data->bPmWakeEnableRTC0 = enable;
            enable = 0;
            pm_get_wakeup_config("rtc1", &enable);
            data->bPmWakeEnableRTC1 = enable;
            enable = 0;
            pm_get_wakeup_config("dvi0", &enable);
            data->bPmWakeEnableDVI0 = enable;
            enable = 0;
            pm_get_wakeup_config("dvi2", &enable);
            data->bPmWakeEnableDVI2 = enable;
            enable = 0;
            pm_get_wakeup_config("cec0", &enable);
            data->bPmWakeEnableCEC = enable;
            enable = 0;
            pm_get_wakeup_config("avlink", &enable);
            data->bPmWakeEnableAVLINK = enable;
            enable = 0;
            pm_get_wakeup_config("mhl", &enable);
            data->bPmWakeEnableMHL = enable;
            enable = 0;
            pm_get_wakeup_config("lan", &enable);
            data->bPmWakeEnableWOL = enable;
            enable = 0;
            pm_get_wakeup_config("voice", &enable);
            data->bPmWakeEnableCM4 = enable;
            enable = 0;
            pm_get_wakeup_config("wifi-gpio", &enable);
            data->u8PmWakeEnableWOWLAN = enable;
            break;
        }
        case PM_KEY_IR:
            tSize = sizeof(PM_IrInfoCfg_t);
            break;

        case PM_KEY_IR_VERSION:
            tSize = sizeof(u8);
            break;

        case PM_KEY_IR_NORMAL:
            tSize = (IR_POWER_KEY_NORMAL_SIZE_MAX+1);
            break;

        case PM_KEY_IR_EXT:
            tSize = (IR_POWER_KEY_EXTEND_SIZE_MAX+1);
            break;

        case PM_KEY_SAR0:
            tSize = sizeof(SAR_RegCfg);
            break;

        case PM_KEY_SAR1:
            tSize = sizeof(SAR_RegCfg);
            break;

        case PM_KEY_LED:
            tSize = sizeof(u8) * 10;
            break;

        default:
            break;
    }
    return tSize;
}
EXPORT_SYMBOL(MDrv_PM_Read_Key);

ssize_t MDrv_PM_Write_Key(u16 u16Key, const char *buf, size_t size)
{
    size_t tSize = 0;

    if (!buf)
        return 0;

    switch (u16Key)
    {
        case PM_KEY_DEFAULT:
        {
            PM_WakeCfg_t *data = (PM_WakeCfg_t *)buf;
            extern s32 pm_set_wakeup_config(const char *name, bool enable);

            if (size != sizeof(PM_WakeCfg_t))
                break;
            tSize = sizeof(PM_WakeCfg_t);
            pm_set_wakeup_config("ir0", data->bPmWakeEnableIR);
            pm_set_wakeup_config("keypad0", data->bPmWakeEnableSAR);
            pm_set_wakeup_config("uart", data->bPmWakeEnableUART1);
            pm_set_wakeup_config("sync", data->bPmWakeEnableSYNC);
            pm_set_wakeup_config("esync", data->bPmWakeEnableESYNC);
            pm_set_wakeup_config("rtc0", data->bPmWakeEnableRTC0);
            pm_set_wakeup_config("rtc1", data->bPmWakeEnableRTC1);
            pm_set_wakeup_config("dvi0", data->bPmWakeEnableDVI0);
            pm_set_wakeup_config("dvi1", data->bPmWakeEnableDVI0);
            pm_set_wakeup_config("dvi2", data->bPmWakeEnableDVI2);
            pm_set_wakeup_config("dvi3", data->bPmWakeEnableDVI0);
            pm_set_wakeup_config("cec0", data->bPmWakeEnableCEC);
            pm_set_wakeup_config("cec1", data->bPmWakeEnableCEC);
            pm_set_wakeup_config("cec2", data->bPmWakeEnableCEC);
            pm_set_wakeup_config("cec3", data->bPmWakeEnableCEC);
            pm_set_wakeup_config("avlink", data->bPmWakeEnableAVLINK);
            pm_set_wakeup_config("mhl", data->bPmWakeEnableMHL);
            pm_set_wakeup_config("lan", data->bPmWakeEnableWOL);
            pm_set_wakeup_config("voice", data->bPmWakeEnableCM4);
            pm_set_wakeup_config("wifi-gpio", data->u8PmWakeEnableWOWLAN != PM_SOURCE_DISABLE);
            break;
        }
        case PM_KEY_IR:
            tSize = sizeof(PM_IrInfoCfg_t);
            break;

        case PM_KEY_IR_VERSION:
            tSize = sizeof(PM_IrInfoCfg_t);
            break;

        case PM_KEY_IR_NORMAL:
            tSize = (IR_POWER_KEY_NORMAL_SIZE_MAX+1);
            break;

        case PM_KEY_IR_EXT:
            tSize = (IR_POWER_KEY_EXTEND_SIZE_MAX+1);
            break;

        case PM_KEY_SAR0:
            tSize = sizeof(SAR_RegCfg);
            break;

        case PM_KEY_SAR1:
            tSize = sizeof(SAR_RegCfg);
            break;

        case PM_KEY_LED:
            tSize = 10;
            break;

        default:
            break;
    }
    return tSize;
}
EXPORT_SYMBOL(MDrv_PM_Write_Key);

void MDrv_PM_Show_Config(u16 u16Key)
{
    MDRV_PM_ERR("%s not support.\n", __func__);
}

PM_Result MDrv_PM_Suspend(PM_STATE eState)
{
    MDRV_PM_ERR("%s not support.\n", __func__);
    return E_PM_OK;
}

PM_Result MDrv_PM_Resume(PM_STATE eState)
{
    MDRV_PM_ERR("%s not support.\n", __func__);
    return E_PM_OK;
}

PM_Result MDrv_PM_Reboot(PM_STATE eState)
{
    MDRV_PM_ERR("%s not support.\n", __func__);
    return E_PM_OK;
}

PM_Result MDrv_PM_Panic(void)
{
    MDRV_PM_ERR("%s not support.\n", __func__);
    return E_PM_OK;
}

//--------------------------------------------------------------------------------------------------
// Other
//--------------------------------------------------------------------------------------------------
u8 MDrv_PM_GetWakeupSource(void)
{
    extern s32 pm_get_wakeup_reason(void);
    return (u8)pm_get_wakeup_reason();
}

u8 MDrv_PM_GetPowerOnKey(void)
{
    MDRV_PM_ERR("%s need to support.\n", __func__);
    return 0x00;
}

void MDrv_PM_CleanPowerOnKey(void)
{
    MDRV_PM_ERR("%s need to support.\n", __func__);
}

/* TODO: Remove to ir driver. */
ssize_t MDrv_PM_Set_IRCfg(const u8 *buf, size_t size)
{
    memcpy((void *)gu8PmIr, buf, sizeof(gu8PmIr));

    return sizeof(gu8PmIr);
}
EXPORT_SYMBOL(MDrv_PM_Set_IRCfg);

void MDrv_PM_WakeIrqMask(u8 mask)
{
    MDRV_PM_ERR("%s not support.\n", __func__);
}
EXPORT_SYMBOL(MDrv_PM_WakeIrqMask);

void MDrv_PM_WDT_RST(void)
{
    MDRV_PM_ERR("%s not support.\n", __func__);
}
EXPORT_SYMBOL(MDrv_PM_WDT_RST);

/* TODO: Remove to led framework. */
u8 MDrv_PM_Get_PowerOffLed(void)
{
    return gu8PmLed;
}

void MDrv_PM_Set_PowerOffLed(u8 u8LedPad)
{
    MDRV_PM_INFO("LedPadNum = %d.\n", u8LedPad);
    gu8PmLed = u8LedPad;
}

#ifdef CONFIG_AMAZON_WOL
void MDrv_PM_Show_PM_WOL_EN(void)
{
    MDRV_PM_INFO("Current Parameter of WOL_EN=%d \n", atomic_read(&PM_WOL_EN));
}

void MDrv_PM_SetPM_WOL_EN(const char *buf)
{
    unsigned int WOL_EN = 0;
    int readCount = 0;

    readCount = sscanf(buf, "%d", &WOL_EN);
    if (readCount != 1) {
        MDRV_PM_ERR("ERROR cannot read WOL_EN from [%s] \n", buf);
        return;
    }
    if (WOL_EN > 0x01) {
        MDRV_PM_ERR("ERROR Parameter WOL_EN=%d \n", WOL_EN);
        return;
    }

    MDRV_PM_INFO("Set Parameter WOL_EN=%d success\n", WOL_EN);
    atomic_set(&PM_WOL_EN, WOL_EN);
}
#endif

#ifdef CONFIG_MSTAR_SYSFS_BACKLIGHT
PM_Result MDrv_PM_TurnoffBacklight(void)
{
    struct backlight_device *bd;

    bd = backlight_device_get_by_type(BACKLIGHT_RAW);
    if (!bd) {
        MDRV_PM_ERR("MDrv_PM_TurnoffBacklight: get BACKLIGHT_RAW dev failed\n");
        return -ENODEV;
    }

    backlight_device_set_brightness(bd, 0);

    return E_PM_OK;
}
#endif

//--------------------------------------------------------------------------------------------------
// DEBUG
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// EARLY_PARAM
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//TODO: Remove API as follow.
//--------------------------------------------------------------------------------------------------
PM_Result MDrv_PM_CopyBin2Dram(void)
{
    return E_PM_OK;
}
EXPORT_SYMBOL(MDrv_PM_CopyBin2Dram);
