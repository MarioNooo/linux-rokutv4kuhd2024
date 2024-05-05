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
/// @file   mapi_alg_pqhw_pqgamma.h
/// @brief  Algorithem Driver Interface
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MAPI_ALG_PQHW_PQGAMMA_H_
#define _MAPI_ALG_PQHW_PQGAMMA_H_

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
typedef enum
{
    E_PQ_VIDEO_CTRL_MODE_MAIN,
    E_PQ_VIDEO_CTRL_MODE_SUB,
} EN_PQ_VIDEO_HW_CTRL_SEL;

typedef struct
{
    MS_U16 u16_u6p10Gain;
    MS_U16 u16Offset; // bias 1024
} ST_PQ_GAIN_OFFSET;

typedef struct _ST_PQGAMMA_IN
{
    MS_U32 u32Version;
    MS_U32 u32Length;

    MS_BOOL bInit;

    EN_PQ_VIDEO_HW_CTRL_SEL enMainSubSel;

    ST_PQ_GAIN_OFFSET astGainOff1[3];
    ST_PQ_GAIN_OFFSET astGainOff2[3];

    MS_BOOL bOne_Drv3; // One drives 3 channels, R=G, B=G
    MS_BOOL bEnable;
    MS_U32 au32_u20p12MaxBase[3];
    MS_U32 au32_u20p12Lut[NUM_PQGAMMA_LUT_ENTRY*3]; // RGB LUTs
} ST_PQGAMMA_IN;


typedef struct _ST_PQGAMMA_HW_OUT
{
    MS_U32 u32Version;
    MS_U16 u16Length;
    ST_Register_Table stRegTable;
    ST_Autodownload_Table stAdl;
} ST_PQGAMMA_HW_OUT;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
EN_RC_RESULT MApi_Alg_PQHW_PQGamma_In_Init(ST_PQGAMMA_IN *pst_pq_gamma_in);
EN_RC_RESULT MApi_Alg_PQHW_PQGamma(const ST_PQGAMMA_IN *pst_pq_gamma_in, ST_PQGAMMA_HW_OUT *pst_pq_gamma_hw_out);

#endif // _MAPI_ALG_PQHW_PQGAMMA_H_
