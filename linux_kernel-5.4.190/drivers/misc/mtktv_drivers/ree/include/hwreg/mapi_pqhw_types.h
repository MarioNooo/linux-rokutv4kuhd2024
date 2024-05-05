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

#ifndef __MAPI_PQHW_TYPES_H__
#define __MAPI_PQHW_TYPES_H__

#ifndef __MDRV_TYPES_H__

#ifdef INTERFACE
#undef INTERFACE
#endif
#define INTERFACE       extern

#ifdef SYMBOL_WEAK
#undef SYMBOL_WEAK
#endif
#define SYMBOL_WEAK     __attribute__((weak))

#ifndef MS_U8

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#else
#include "pqu_port.h"
#endif

/// data type unsigned char, data length 1 byte
typedef unsigned char                          MS_U8;                                 // 1 byte
/// data type unsigned short, data length 2 byte
typedef unsigned short                         MS_U16;                                // 2 bytes
/// data type unsigned int, data length 4 byte
typedef unsigned int                           MS_U32;                                // 4 bytes
/// data type unsigned int, data length 8 byte
typedef unsigned long long                     MS_U64;                               // 8 bytes

/// data type signed char, data length 1 byte
typedef signed char                            MS_S8;                                 // 1 byte
/// data type signed short, data length 2 byte
typedef signed short                           MS_S16;                                // 2 bytes
/// data type signed int, data length 4 byte
typedef signed int                             MS_S32;                                // 4 bytes
/// data type unsigned int, data length 8 byte
typedef signed long long                       MS_S64;                               // 8 bytes

/// definitaion for MStar BOOL
typedef unsigned short                         MS_B16;
/// definition for MS_BOOL, this will be removed later
typedef unsigned char                        MS_BOOL;

typedef MS_U64                         MS_PHY;                                        // 8 bytes

typedef size_t                        MS_VIRT;

#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif

#ifndef true
/// definition for true
#define true                        1
/// definition for false
#define false                       0
#endif

/// definition for ENABLE
#ifndef ENABLE
#define ENABLE 1
#endif
/// definition for DISABLE
#ifndef DISABLE
#define DISABLE 0
#endif

#define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))

/// @name BIT#
/// definition of one bit mask
/// @{
#if !defined(BIT0) && !defined(BIT1)
#define BIT0                        0x00000001
#define BIT1                        0x00000002
#define BIT2                        0x00000004
#define BIT3                        0x00000008
#define BIT4                        0x00000010
#define BIT5                        0x00000020
#define BIT6                        0x00000040
#define BIT7                        0x00000080
#define BIT8                        0x00000100
#define BIT9                        0x00000200
#define BIT10                       0x00000400
#define BIT11                       0x00000800
#define BIT12                       0x00001000
#define BIT13                       0x00002000
#define BIT14                       0x00004000
#define BIT15                       0x00008000
#define BIT16                       0x00010000
#define BIT17                       0x00020000
#define BIT18                       0x00040000
#define BIT19                       0x00080000
#define BIT20                       0x00100000
#define BIT21                       0x00200000
#define BIT22                       0x00400000
#define BIT23                       0x00800000
#define BIT24                       0x01000000
#define BIT25                       0x02000000
#define BIT26                       0x04000000
#define BIT27                       0x08000000
#define BIT28                       0x10000000
#define BIT29                       0x20000000
#define BIT30                       0x40000000
#define BIT31                       0x80000000
#endif
/// @}

#endif


#endif // #ifndef __MDRV_TYPES_H__

typedef enum _EN_RC_RESULT
{
    E_RC_SUCCESS               = 0,
    E_RC_FAIL                  = 0x80,
    E_RC_FAIL_INSUF_LUT_SIZE   = E_RC_FAIL | 1,
    E_RC_FAIL_NULL_PTR         = E_RC_FAIL | 2,
} EN_RC_RESULT;


#endif // #ifndef __MAPI_PQHW_TYPES_H__

