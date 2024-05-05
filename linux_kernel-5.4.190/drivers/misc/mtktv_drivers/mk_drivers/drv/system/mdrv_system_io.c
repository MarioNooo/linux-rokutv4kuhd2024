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

/*
 * dev/system user module
 * msos/linux/MsOS_linux.c
 */

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/delay.h>
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
#include <linux/device.h>
#include <linux/version.h>
#include <linux/compat.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/of.h>
#include <asm/io.h>
#include <asm/types.h>
#if defined(CONFIG_MIPS)
#include <asm/mips-boards/prom.h>
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#include <prom.h>
#endif

#include "mst_devid.h"
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system_io.h"
#include "mdrv_system_st.h"
#include "mdrv_system.h"
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MOD_SYS_DEVICE_COUNT    1
#define MOD_SYS_NAME            "system"
#define SYS_WARNING(fmt, args...)       printk(KERN_WARNING "[SYSMOD][%06d] " fmt, __LINE__, ## args)
#if 0
#define SYS_PRINT(fmt, args...)         printk("[SYSMOD][%06d]     " fmt, __LINE__, ## args)
#else
#define SYS_PRINT(fmt, args...)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------
typedef struct
{
    int                         s32SysMajor;
    int                         s32SysMinor;
    struct cdev                 cDevice;
    struct file_operations      SysFop;
} SysModHandle;


//-------------------------------------------------------------------------------------------------
//  Forward declaration
//-------------------------------------------------------------------------------------------------
MSYSTEM_STATIC int __init   _mod_sys_init(void);
MSYSTEM_STATIC void __exit  _mod_sys_exit(void);

static int _mod_sys_open (struct inode *inode, struct file *filp);
static int _mod_sys_release(struct inode *inode, struct file *filp);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long _mod_sys_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int _mod_sys_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif

#if defined (CONFIG_COMPAT)
static long Compat_mod_sys_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
static ssize_t _mod_sys_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
#endif

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static SysModHandle SysDev=
{
    .s32SysMajor=               MDRV_MAJOR_SYSTEM,
    .s32SysMinor=               MDRV_MINOR_SYSTEM,
    .cDevice=
    {
        .kobj=                  {.name= MDRV_NAME_SYSTEM, },
        .owner  =               THIS_MODULE,
    },
    .SysFop=
    {
        .open=                  _mod_sys_open,
        .release=               _mod_sys_release,
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
        .unlocked_ioctl=          _mod_sys_ioctl,
	#else
        .ioctl=                 _mod_sys_ioctl,
	#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
        .read =                 _mod_sys_read,
#endif
	#if defined(CONFIG_COMPAT)
        .compat_ioctl= Compat_mod_sys_ioctl,
	#endif

    },
};

#ifdef CONFIG_MSTAR_UDEV_NODE
static struct class *system_class;
#endif

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0)) && defined(CONFIG_MSTAR_RTC)
extern u32 str_suspend_tm;
#endif

extern unsigned char datapool[];
extern int dataidx;
extern int uidx;
extern void enable_MUDI(void);
extern void disable_MUDI(void);
extern struct
{
    wait_queue_head_t wq;
    struct semaphore sem;
}MUDI_dev;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0)) && defined(CONFIG_MSTAR_RTC)
static int current_time_proc_show(struct seq_file *m, void *v)
{
    struct timespec uptime;

    do_posix_clock_monotonic_gettime(&uptime);
    monotonic_to_bootbased(&uptime);

    seq_printf(m, "%lu.%02lu\n",
    (unsigned long) (uptime.tv_sec + str_suspend_tm),
    (uptime.tv_nsec / (NSEC_PER_SEC / 100)));
    return 0;
}

static int current_time_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, current_time_proc_show, NULL);
}

static const struct file_operations current_time_proc_fops = {
	.open		= current_time_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

int _mod_sys_get_dts_value(char* node, char* key, char* value, unsigned int size)
{
    struct device_node *key_node;
    struct property *prop;

    /* Check parameter. */
    if ((node == NULL) || (key == NULL) || (value == NULL))
    {
        return -EINVAL;
    }

    /* Find device-tree node */
    key_node = of_find_node_by_path(node);
    if (!key_node)
    {
        return -ENODATA;
    }

    /* Find device-tree node's property. */
    prop = of_find_property(key_node, key, NULL);
    if (!prop)
    {
        return -ENODATA;
    }

    /* Check buffer size. */
    if (size < (strlen(prop->value) + 1))
    {
        return -ENOBUFS;
    }

    /* Copy buffer. */
    strncpy(value, prop->value, strlen(prop->value));
    value[strlen(prop->value)] = '\0';

    return 0;

}

MSYSTEM_STATIC int __init _mod_sys_init(void)
{
    int         s32Ret;
    dev_t       dev;

#ifdef CONFIG_MSTAR_UDEV_NODE
    system_class = class_create(THIS_MODULE,MDRV_NAME_SYSTEM);
    if (IS_ERR(system_class))
    {
        return PTR_ERR(system_class);
    }
#endif
    SYS_PRINT("%s is invoked\n", __FUNCTION__);
    if (SysDev.s32SysMajor)
    {
        dev=                    MKDEV(SysDev.s32SysMajor, SysDev.s32SysMinor);
        s32Ret=                 register_chrdev_region(dev, MOD_SYS_DEVICE_COUNT, MDRV_NAME_SYSTEM);
    }
    else
    {
		dev = 0;
        s32Ret=                 alloc_chrdev_region(&dev, SysDev.s32SysMinor, MOD_SYS_DEVICE_COUNT, MDRV_NAME_SYSTEM);
        SysDev.s32SysMajor=     MAJOR(dev);
    }
    if (0> s32Ret)
    {
        SYS_WARNING("Unable to get major %d\n", SysDev.s32SysMajor);
        return s32Ret;
    }

    cdev_init(&SysDev.cDevice, &SysDev.SysFop);
    if (0!= (s32Ret= cdev_add(&SysDev.cDevice, dev, MOD_SYS_DEVICE_COUNT)))
    {
        SYS_WARNING("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_SYS_DEVICE_COUNT);
        return s32Ret;
    }
#ifdef CONFIG_MSTAR_UDEV_NODE
    device_create(system_class, NULL, dev, NULL,MDRV_NAME_SYSTEM);
#endif
    #if 0
    // @FIXME: for GE bug. Slow down MCU clock to 123MHz
    TOP_REG(REG_TOP_MCU) = (TOP_REG(REG_TOP_MCU) & ~(TOP_CKG_MCU_MASK)) | TOP_CKG_MCU_SRC_123;

    MDrv_MIU_Kernel_Init();
    #endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0)) && defined(CONFIG_MSTAR_RTC)
        proc_create("current_time", 0, NULL, &current_time_proc_fops);
#endif

    return 0;
}

MSYSTEM_STATIC void __exit _mod_sys_exit(void)
{
    SYS_PRINT("%s is invoked\n", __FUNCTION__);

    cdev_del(&SysDev.cDevice);
    unregister_chrdev_region(MKDEV(SysDev.s32SysMajor, SysDev.s32SysMinor), MOD_SYS_DEVICE_COUNT);
#ifdef CONFIG_MSTAR_UDEV_NODE
    class_destroy(system_class);
#endif
}

static int _mod_sys_open (struct inode *inode, struct file *filp)
{
    SYS_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static int _mod_sys_release(struct inode *inode, struct file *filp)
{
    SYS_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

#if defined (CONFIG_COMPAT)
static long Compat_mod_sys_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err= 0;
    switch(cmd)
    {
    case IOCTL_SYS_INFO:
    case IOCTL_SYS_INFO_EX:
    case IOCTL_SYS_INIT:
    case IOCTL_SYS_SET_PANEL_INFO:
    case IOCTL_SYS_SET_BOARD_INFO:
    case IOCTL_SYS_GET_PANEL_RES:
#ifdef IO_SYS_REG_OP
    case IOCTL_SYS_REG_OP:
#endif
#ifdef IO_SYS_GET_RAW_UART
    case IOCTL_SYS_GET_RAW_UART:
#endif
    case IOCTL_SYS_TIMER:
    case IOCTL_SYS_POWER_DOWN:
    case IOCTL_SYS_SET_GFX_GOP_INDEX:
    case IOCTL_SYS_GET_GFX_GOP_INDEX:
    case IOCTL_SYS_SET_DISPLAY_CTLR_SEPT_INDEX:
    case IOCTL_SYS_IS_DISPLAY_CTLR_SEPT_INDEX:
    case IOCTL_SYS_SET_NEXUS:
    case IOCTL_SYS_HAS_NEXUS:
    case IOCTL_SYS_PRINT_MSG:
    case IOCTL_SYS_GET_GFX_GOP_PIPELINE_DELAY:
    case IOCTL_SYS_GET_PANEL_H_START:
    case IOCTL_SYS_SET_NEXUS_PID:
    case IOCTL_SYS_GET_NEXUS_PID:
    case IOCTL_SYS_GET_MBOX_SHM:
    case IOCTL_SYS_GET_MSBIN_INFO:
    case IOCTL_SYS_GET_MIU1_BUS_BASE:
    case IOCTL_SYS_GET_MIU1_BASE:
    case IOCTL_SYS_FLUSH_MEMORY:
    case IOCTL_SYS_READ_MEMORY:
    case IOCTL_SYS_STOP_UART_CLK:
    case IOCTL_SYS_RESUME_UART_CLK:
    case IOCTL_SYS_ENABLE_MUDI:
    case IOCTL_SYS_DISABLE_MUDI:
    case IOCTL_SYS_SPI_LOAD: //20100120 Terry, SPI Load Code
	{
		return filp->f_op->unlocked_ioctl(filp, cmd,(unsigned long)compat_ptr(arg));
	}

    default:
        SYS_WARNING("Unknown ioctl command %d\n", cmd);
        return -ENOTTY;
    }

    return err;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long _mod_sys_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int  _mod_sys_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    int err= 0;
    // extract the type and number bitfields, and don¡¦t decode
    // wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
    if (_IOC_TYPE(cmd) != SYS_IOCTL_MAGIC) return -ENOTTY;

    // the direction is a bitmask, and VERIFY_WRITE catches R/W
    // transfers. ¡¥Type¡¦ is user oriented, while
    // access_ok is kernel oriented, so the concept of "read" and
    // "write" is reversed
    if (_IOC_DIR(cmd) & _IOC_READ)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
        err = !access_ok((void *)arg, _IOC_SIZE(cmd));
#else
        err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
#endif
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
        err = !access_ok((void *)arg, _IOC_SIZE(cmd));
#else
        err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
#endif
    if (err) return -EFAULT;

    switch(cmd)
    {
    case IOCTL_SYS_INFO:
    case IOCTL_SYS_INFO_EX:
    case IOCTL_SYS_INIT:
	WARN(1, "not support ioctl\n");
        break;
    case IOCTL_SYS_SET_PANEL_INFO:
        err = MDrv_SYS_SetPanelInfo(arg);
        break;

    case IOCTL_SYS_SET_BOARD_INFO:

        break;

    case IOCTL_SYS_GET_PANEL_RES:
        MDrv_SYS_GetPanelRes(arg);
        break;

#ifdef IO_SYS_REG_OP
    case IOCTL_SYS_REG_OP:
        SYS_PRINT("ioctl command IOCTL_SYS_REG_OP is not supported.\n");
        err = -ENOTTY;
        break ;
#endif

    case IOCTL_SYS_TIMER:
#if 0
        err = MDrv_SYS_Timer(arg);
#else
	WARN(1, "ioctl not support\n");
#endif
        break ;

    case IOCTL_SYS_POWER_DOWN:
        MDrv_SYS_PowerDown( arg );
        break;

    case IOCTL_SYS_SET_GFX_GOP_INDEX:
        MDrv_SYS_SetGFXGOPIndex(arg);
        break;

    case IOCTL_SYS_GET_GFX_GOP_INDEX:
        MDrv_SYS_GetGFXGOPIndex(arg);
        break;

    case IOCTL_SYS_SET_DISPLAY_CTLR_SEPT_INDEX:
        MDrv_SYS_SetDisplayControllerSeparated(arg);
        break;

    case IOCTL_SYS_IS_DISPLAY_CTLR_SEPT_INDEX:
        MDrv_SYS_IsDisplayControllerSeparated(arg);
    break;

    case IOCTL_SYS_SET_NEXUS:
        MDrv_SYS_SetNexus(arg);
        break;

    case IOCTL_SYS_HAS_NEXUS:
        MDrv_SYS_HasNexus(arg);
        break;

    case IOCTL_SYS_PRINT_MSG:
            MDrv_SYS_PrintMsg(arg);
        break;

    case IOCTL_SYS_GET_GFX_GOP_PIPELINE_DELAY:
        MDrv_SYS_GetGFXGOPPipelineDelay(arg);
        break;
    case IOCTL_SYS_GET_PANEL_H_START:
        MDrv_SYS_GetPanelHStart(arg);
        break;

    case IOCTL_SYS_SET_NEXUS_PID:
          MDrv_SYS_SetNexusPID(arg);
          break;

    case IOCTL_SYS_GET_NEXUS_PID:
        MDrv_SYS_GetNexusPID(arg);
        break;

    case IOCTL_SYS_GET_MBOX_SHM:
        MDrv_SYS_GetMBoxShareMemory(arg);
        break;

    case IOCTL_SYS_GET_MSBIN_INFO:
        MDrv_SYS_GetMsBinInfo(arg);
        break;

    case IOCTL_SYS_GET_MIU1_BUS_BASE:
        MDrv_SYS_GetMIU1BusBase(arg);
        break;

    case IOCTL_SYS_GET_MIU1_BASE:
        MDrv_SYS_GetMIU1Base(arg);
        break;

    case IOCTL_SYS_FLUSH_MEMORY:
        MDrv_SYS_FlushMemory();
        break;

    case IOCTL_SYS_READ_MEMORY:
        MDrv_SYS_ReadMemory();
        break;

    case IOCTL_SYS_STOP_UART_CLK:
#if 0
    	MDrv_SYS_StopUARTClock();
#else
	WARN(1, "ioctl not support\n");
#endif
    	break;
    case IOCTL_SYS_RESUME_UART_CLK:
#if 0
    	MDrv_SYS_ResumeUARTClock();
#else
	WARN(1, "ioctl not support\n");
#endif
        break;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
    case IOCTL_SYS_ENABLE_MUDI:
        enable_MUDI();
        break;

    case IOCTL_SYS_DISABLE_MUDI:
        disable_MUDI();
        break;
#endif
    default:
        SYS_WARNING("Unknown ioctl command %d\n", cmd);
        return -ENOTTY;
    }

    return err;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#define UART_DATA_MASK   63
static ssize_t _mod_sys_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{

    if (down_interruptible(&MUDI_dev.sem))
    {
        return -ERESTARTSYS;
    }

    while(dataidx==uidx)
    {
        up(&MUDI_dev.sem); /* release the lock */
        if (wait_event_interruptible(MUDI_dev.wq,dataidx!=uidx))
        {
            return -ERESTARTSYS;
        }

        if (down_interruptible(&MUDI_dev.sem))
        {
            return -ERESTARTSYS;
        }
    }

    if(copy_to_user(buf,&datapool[uidx++],1))
    {
        up(&MUDI_dev.sem);
        return 0;
    }

    uidx&=UART_DATA_MASK;
    up(&MUDI_dev.sem);

    return 1;

}
#endif

EXPORT_SYMBOL(_mod_sys_get_dts_value);
#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
#else//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
module_init(_mod_sys_init);
module_exit(_mod_sys_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("SYSTEM driver");
#endif//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
