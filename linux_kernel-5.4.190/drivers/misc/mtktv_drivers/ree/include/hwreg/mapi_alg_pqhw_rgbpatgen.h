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
/// @file   mapi_alg_pqhw_rgbpatgen.h
/// @brief  Algorithem Driver Interface
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MAPI_ALG_PQHW_RGBPATGEN_H_
#define _MAPI_ALG_PQHW_RGBPATGEN_H_

#include "mapi_pqhw_types.h"
#include "mapi_alg_pqhw_output_format.h"

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define NUM_PQGAMMA_LUT_ENTRY 256

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef struct _ST_STATUS
{
    MS_U32 u32Version;
    MS_U16 u16Length;
    MS_U16 u16UpdateStatus; // Check whitch IPs need to update
} ST_STATUS;

typedef struct _ST_RGBPATGEN_IN
{
    MS_U32 u32Version;
    MS_U32 u32Length;

    MS_BOOL bEnable;
    MS_U16 u16_u0p16R;
    MS_U16 u16_u0p16G;
    MS_U16 u16_u0p16B;
} ST_RGBPATGEN_IN;

typedef struct _ST_RGBPATGEN_HW_OUT
{
    MS_U32 u32Version;
    MS_U16 u16Length;
    ST_Register_Table stRegTable;
    ST_STATUS stStatus;
} ST_RGBPATGEN_HW_OUT;


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
EN_RC_RESULT MApi_Alg_PQHW_RGBPatGen(const ST_RGBPATGEN_IN *pst_rgbpatgen_in, ST_RGBPATGEN_HW_OUT *pst_rgbpatgen_hw_out);


#endif // _MAPI_ALG_PQHW_RGBPATGEN_H_
