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
/// @file   mapi_alg_pqhw_adp.h
/// @brief  Algorithem Driver Interface
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MAPI_ALG_PQHW_ADP_H_
#define _MAPI_ALG_PQHW_ADP_H_

#include "mapi_alg_pq_adp.h"
#include "mapi_alg_pqhw_output_format.h"

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef struct _ST_PQAPI_HW_OUT
{
    MS_U32 u32Version;
    MS_U16 u16Length;
    ST_Register_Table stRegTable;
} ST_PQAPI_HW_OUT;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
MS_U32 MApi_Alg_PQHW_Adp(const ST_PQAPI_ADAPTIVE_IN *pstPqAdp_in, const ST_PQAPI_ADAPTIVE_OUT *pstPqAdp_out, ST_PQAPI_HW_OUT *pstPqAdp_hw_out);

#endif // _MAPI_ALG_PQHW_ADP_H_
