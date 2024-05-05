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


///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mapi_alg_pqhw_output_format.h
/// @brief  PQ HW Driver Interface
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MAPI_PQHW_OUTPUT_FORMAT_H_
#define _MAPI_PQHW_OUTPUT_FORMAT_H_

#include "mapi_pqhw_types.h"

typedef enum
{
    E_REG_IP,
    E_REG_OP,
    E_REG_VOP,
    E_REG_FO,
    E_REG_MAX,
} EN_REG_TYPE;

typedef enum /// Output version of PQ module
{
    E_PQ_CLIENT_HDR10PLUS,
    E_PQ_CLIENT_GCE,
    E_PQ_CLIENT_COMMON_STATUS_SW_VERSION,
    E_PQ_CLIENT_CFD,
    E_PQ_CLIENT_MAX,
} EN_PQ_CLIENT_TYPE;

typedef struct _ST_Register_Table
{
    MS_U32 u32MaxSize;
    MS_U32 u32UsedSize;
    MS_U32 *pu32Address;
    MS_U16 *pu16Value;
    MS_U16 *pu16Mask;
    MS_U16 *pu16Client;
} ST_Register_Table;

typedef struct _ST_Autodownload_Table
{
    MS_U32 u32MaxSize;
    MS_U32 u32UsedSize;
    MS_U16 u16client;
    MS_U8 *pu8Data;
} ST_Autodownload_Table;

#undef INTERFACE
#endif // _MDRV_PQHW_OUTPUT_FORMAT_H_

