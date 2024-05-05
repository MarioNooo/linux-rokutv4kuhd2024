/*------------------------------------------------------------------------------
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 *----------------------------------------------------------------------------*/

#ifndef _STI_MSOS_H_
#define _STI_MSOS_H_

//	#include <sys/ioccom.h>
#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/slab.h>
#include <linux/io.h>
#else
#include "pqu_port.h"
#endif
#include "mdrv_types_internal.h"

//////////////////////////////////// extern VAR defines
#define HAL_MIU1_BASE               0x80000000UL // 1512MB

#define MODULE_AVD 0

#define UTOPIA_STATUS_SUCCESS               0x00000000
#define UTOPIA_STATUS_FAIL                  0x00000001
#define UTOPIA_STATUS_NOT_SUPPORTED         0x00000002
#define UTOPIA_STATUS_PARAMETER_ERROR       0x00000003

//////////////////////////////////// compiler defines

#ifndef DLL_PACKED
#define DLL_PACKED __attribute__((__packed__))
#endif

#ifndef DLL_PUBLIC
#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#endif

#ifndef DLL_LOCAL
#define DLL_LOCAL __attribute__ ((visibility ("hidden")))
#endif

#ifndef SYMBOL_WEAK
#define SYMBOL_WEAK __attribute__((weak))
#endif

//////////////////////////////////// STDLIB defines

#ifdef MSOS_TYPE_LINUX_KERNEL
#undef printf
#define printf printk
#endif

#ifdef MSOS_TYPE_LINUX_KERNEL
#define free kfree
#define malloc(size) kmalloc((size), GFP_KERNEL)
#endif

//////////////////////////////////// type defines
#ifndef ENABLE
#define ENABLE 1
#endif
#ifndef DISABLE
#define DISABLE 0
#endif


#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#if 0
typedef int8_t   __s8;
typedef uint8_t  __u8;
typedef int16_t  __s16;
typedef uint16_t __u16;
typedef int32_t  __s32;
typedef uint32_t __u32;
typedef int64_t  __s64;
typedef uint64_t __u64;

#ifndef MS_U8
typedef __u8 MS_U8;
#endif

#ifndef MS_S8
typedef __s8 MS_S8;
#endif

#ifndef MS_U16
typedef __u16 MS_U16;
#endif

#ifndef MS_S16
typedef __s16 MS_S16;
#endif

#ifndef MS_U32
typedef __u32 MS_U32;
#endif

#ifndef MS_S32
typedef __s32 MS_S32;
#endif

#ifndef MS_U64
typedef __u64 MS_U64;
#endif

#ifndef MS_S64
typedef __s64 MS_S64;
#endif

#ifndef MS_BOOL
typedef unsigned char MS_BOOL;
#endif

//	#ifndef MS_FLOAT
//	typedef float                           MS_FLOAT;
//	#endif

#ifndef MS_PHY
typedef __u64 MS_PHY;                         // 8 bytes
#endif

#ifndef MS_VIRT
typedef size_t MS_VIRT;                         // 8 bytes
#endif
#endif

#ifndef MS_PHYADDR
typedef __u64 MS_PHYADDR;                         // 8 bytes
#endif

//////////////////////////////////// utility macro

#define ALIGN_4(_x_)                (((_x_) + 3) & ~3)
#define ALIGN_8(_x_)                (((_x_) + 7) & ~7)
#define ALIGN_16(_x_)               (((_x_) + 15) & ~15)           // No data type specified, optimized by complier
#define ALIGN_32(_x_)               (((_x_) + 31) & ~31)           // No data type specified, optimized by complier

#ifndef MIN
#define MIN(_a_, _b_)               ((_a_) < (_b_) ? (_a_) : (_b_))
#endif
#ifndef MAX
#define MAX(_a_, _b_)               ((_a_) > (_b_) ? (_a_) : (_b_))
#endif

#ifndef BIT	//for Linux_kernel type, BIT redefined in <linux/bitops.h>
#define BIT(_bit_)                  (1 << (_bit_))
#endif
#define BIT_(x)                     BIT(x) //[OBSOLETED] //TODO: remove it later
#define BITS(_bits_, _val_)         ((BIT(((1)?_bits_)+1)-BIT(((0)?_bits_))) & (_val_<<((0)?_bits_)))
#define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))
#define BIT_64(_bit_)                (1ULL << (_bit_))

//////////////////////////////////// debug macro

#define MS_ASSERT(_bool_)                                                                                   \
    {                                                                                                   \
        if (!(_bool_))                                                                                  \
        {                                                                                               \
            printk("\033[35m ASSERT FAIL: %s, %s %s %d\033[35m\n", #_bool_, __FILE__, __PRETTY_FUNCTION__, __LINE__);    \
            panic("mstar panic\n");                                                                     \
        }                                                                                               \
    }


#if defined (MS_DEBUG)
  #define MS_DEBUG_MSG(x)       x
#elif defined (MS_OPTIMIZE)
  #define MS_DEBUG_MSG(x)               // retail version remove debug message
#else
  #define MS_DEBUG_MSG(x)
#endif
#define MS_CRITICAL_MSG(x)      x       // for dump critical message
#define MS_FATAL_MSG(fmt,...) printf( "[MS_FATAL]: %s: %d \n"  fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)

#define ULOGI(tag,fmt,...)      printf("[Utopia][" tag "]: " fmt, ##__VA_ARGS__)
#define ULOGW(tag,fmt,...)      printf("[Utopia][" tag "]: " fmt, ##__VA_ARGS__)
#define ULOGD(tag,fmt,...)      printf("[Utopia][" tag "]: " fmt, ##__VA_ARGS__)
#define ULOGE(tag,fmt,...)      printf("[Utopia][" tag "]: " fmt, ##__VA_ARGS__)
#define ULOGF(tag,fmt,...)      printf("[Utopia][" tag "]: " fmt, ##__VA_ARGS__)

//////////////////////////////////// enum defines

typedef enum
{
    /// color format I1
    E_MS_FMT_I1                     = 0x0,
    /// color format I2
    E_MS_FMT_I2                     = 0x1,
    /// color format I4
    E_MS_FMT_I4                     = 0x2,
    /// color format palette 256(I8)
    E_MS_FMT_I8                     = 0x4,
    /// color format blinking display
    E_MS_FMT_FaBaFgBg2266  = 0x6,
    /// color format for blinking display format
    E_MS_FMT_1ABFgBg12355           = 0x7,
    /// color format RGB565
    E_MS_FMT_RGB565                 = 0x8,
    /// color format ARGB1555
    /// @note <b>[URANUS] <em>ARGB1555 is only RGB555</em></b>
    E_MS_FMT_ARGB1555               = 0x9,
    /// color format ARGB4444
    E_MS_FMT_ARGB4444               = 0xa,
    /// color format ARGB1555 DST
    E_MS_FMT_ARGB1555_DST           = 0xc,
    /// color format YUV422
    E_MS_FMT_YUV422                 = 0xe,
    /// color format ARGB8888
    E_MS_FMT_ARGB8888               = 0xf,
    /// color format RGBA5551
    E_MS_FMT_RGBA5551               = 0x10,
    /// color format RGBA4444
    E_MS_FMT_RGBA4444               = 0x11,
    /// Start of New color define
    /// color format BGRA5551
    E_MS_FMT_BGRA5551               = 0x12,
    /// color format ABGR1555
    E_MS_FMT_ABGR1555               = 0x13,
    /// color format ABGR4444
    E_MS_FMT_ABGR4444               = 0x14,
    /// color format BGRA4444
    E_MS_FMT_BGRA4444               = 0x15,
    /// color format BGR565
    E_MS_FMT_BGR565                 = 0x16,
    /// color format RGBA8888
    E_MS_FMT_RGBA8888               = 0x1d,
    /// color format BGRA8888
    E_MS_FMT_BGRA8888               = 0x1e,
    /// color format ABGR8888
    E_MS_FMT_ABGR8888               = 0x1f,
    /// color format AYUV8888
    E_MS_FMT_AYUV8888               = 0x20,

    E_MS_FMT_GENERIC                = 0xFFFF,

} MS_ColorFormat;

typedef enum
{
    E_POWER_SUSPEND     = 1,    // State is backup in DRAM, components power off.
    E_POWER_RESUME      = 2,    // Resume from Suspend or Hibernate mode
    E_POWER_MECHANICAL  = 3,    // Full power off mode. System return to working state only after full reboot
    E_POWER_SOFT_OFF    = 4,    // The system appears to be off, but some components remain powered for trigging boot-up.
} EN_POWER_MODE;

typedef union _MSIF_Version
{
    struct _DDI
    {
        MS_U8                       tag[4];
        MS_U8                       type[2];
        MS_U16                      customer;
        MS_U16                      model;
        MS_U16                      chip;
        MS_U8                       cpu;
        MS_U8                       name[4];
        MS_U8                       version[2];
        MS_U8                       build[2];
        MS_U8                       change[8];
        MS_U8                       os;
    } DDI;
    struct _MW
    {
        MS_U8                                     tag[4];
        MS_U8                                     type[2];
        MS_U16                                    customer;
        MS_U16                                    mod;
        MS_U16                                    chip;
        MS_U8                                     cpu;
        MS_U8                                     name[4];
        MS_U8                                     version[2];
        MS_U8                                     build[2];
        MS_U8                                     changelist[8];
        MS_U8                                     os;
    } MW;
    struct _APP
    {
        MS_U8                                     tag[4];
        MS_U8                                     type[2];
        MS_U8                                     id[4];
        MS_U8                                     quality;
        MS_U8                                     version[4];
        MS_U8                                     time[6];
        MS_U8                                     changelist[8];
        MS_U8                                     reserve[3];
    } APP;
} MSIF_Version;

//////////////////////////////////// MSOS function prototypes

typedef MS_U32 (*FUtopiaOpen)(void** ppInstance, const void* const pAttribute);
typedef MS_U32 (*FUtopiaIOctl)(void* pInstance, MS_U32 u32Cmd, void* const pArgs);
typedef MS_U32 (*FUtopiaClose)(void* pInstance);


//////////////////////////////////// MSOS function defines

#define _miu_offset_to_phy(MiuSel, Offset, PhysAddr) {PhysAddr = Offset;}
#define _phy_to_miu_offset(MiuSel, Offset, PhysAddr) {MiuSel = 0; Offset = PhysAddr;}

void MsOS_DelayTask(MS_U32 u32Ms);
MS_U32 MsOS_GetSystemTime(void);
MS_S32 MsOS_GetOSThreadID(void);


/*
 * instance functions
 */
MS_U32 UtopiaInstanceCreate(MS_U32 u32PrivateSize, void** ppInstance);
MS_U32 UtopiaInstanceDelete(void* pInstant);
MS_U32 UtopiaInstanceGetPrivate(void* pInstance, void** ppPrivate);
MS_U32 UtopiaInstanceGetModule(void* pInstance, void** ppModule);
/* We hope, we can support poly, ex: JPD and JPD3D as different Module */
MS_U32 UtopiaInstanceGetModuleID(void* pInstance, MS_U32* pu32ModuleID);
/* We hope, we can support poly, ex: JPD and JPD3D as different Module */
MS_U32 UtopiaInstanceGetModuleVersion(void* pInstant, MS_U32* pu32Version);
/* We hope we can support interface version mantain */
MS_U32 UtopiaInstanceGetAppRequiredModuleVersion(void* pInstance,
		MS_U32* pu32ModuleVersion);
MS_U32 UtopiaInstanceGetPid(void* pInstance);

/*
 * module functions
 */
MS_U32 UtopiaModuleCreate(MS_U32 u32ModuleID,
		MS_U32 u32PrivateSize, void** ppModule);
MS_U32 UtopiaModuleGetPrivate(void* pModule, void** ppPrivate);
MS_U32 UtopiaModuleRegister(void* pModule);
MS_U32 UtopiaModuleSetupFunctionPtr(void* pModule, FUtopiaOpen fpOpen,
		FUtopiaClose fpClose, FUtopiaIOctl fpIoctl);


/*
 * resource functions
 */
MS_U32 UtopiaResourceCreate(char* u8ResourceName,
		MS_U32 u32PrivateSize, void** ppResource);
MS_U32 UtopiaResourceGetPrivate(void* pResource, void** ppPrivate);
MS_U32 UtopiaResourceRegister(void* pModule, void* pResouce, MS_U32 u32PoolID);
MS_U32 UtopiaResourceObtain(void* pInstant,
		MS_U32 u32PoolID, void** ppResource);
MS_U32 UtopiaResourceTryObtain(void* pInstant,
		MS_U32 u32PoolID, void** ppResource);
MS_U32 UtopiaResourceRelease(void* pResource);
MS_U32 UtopiaResourceGetPid(void* pResource);
MS_U32 UtopiaResourceGetNext(void* pModTmp, void** ppResource);

MS_U32 UtopiaModuleAddResourceStart(void* psModule, MS_U32 u32PoolID);
MS_U32 UtopiaModuleAddResourceEnd(void* psModule, MS_U32 u32PoolID);
MS_U32 UtopiaModuleResetPool(void* pModuleTmp, MS_U32 u32RPoolID);

MS_U32 UtopiaOpen(MS_U32 u32ModuleID, void** pInstant, MS_U32 u32ModuleVersion,
		const void* const pAttribte);
MS_U32 UtopiaIoctl(void* pInstant, MS_U32 u32Cmd, void* const pArgs);
MS_U32 UtopiaClose(void* pInstant);

#endif // _STI_MSOS_H_
