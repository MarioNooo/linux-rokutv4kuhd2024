///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
/// @file   dv_core_if.h
/// @brief  DoVi Core Interface and common definitions
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_HDRVIVID_IF_H_
#define _MDRV_HDRVIVID_IF_H_

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------
#include "pqu_port.h"
#include "msos_wrapper.h"


//-----------------------------------------------------------------------------
// Driver Capability
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macros and Defines
//-----------------------------------------------------------------------------
#define HDRVIVID_VERSION (0x01002000) /* v1.2.0 */
#define HDRVIVID_TONE_PARAM_NUM (2)
#define HDRVIVID_3SPLINE_PARAM_NUM (4)
#define HDRVIVID_COLORSAT_PARAM_NUM (16)


//-----------------------------------------------------------------------------
// Enums and Structures
//-----------------------------------------------------------------------------

struct st_hdrvivid_meta
{
    u16 u16max_display_luminance;

    u8 u8version_code;
    u8 u8transfer_character;
    u8 u8graphic_source_display_value_pq;
    u16 u16max_display_mastering_luminance;

    // for mm/hdmi common
    u16 u16minimum_maxrgb;
    u16 u16average_maxrgb;
    u16 u16variance_maxrgb;
    u16 u16maximum_maxrgb;
    u8 u8tone_mapping_mode;
    u8 u8tone_mapping_param_num;
    u16 u16targeted_system_display_maximum_luminance[HDRVIVID_TONE_PARAM_NUM];
    u8 u8Base_flag[HDRVIVID_TONE_PARAM_NUM];
    u16 u16Base_param_m_p[HDRVIVID_TONE_PARAM_NUM];
    u8 u8Base_param_m_m[HDRVIVID_TONE_PARAM_NUM];
    u16 u16Base_param_m_a[HDRVIVID_TONE_PARAM_NUM];
    u16 u16Base_param_m_b[HDRVIVID_TONE_PARAM_NUM];
    u8 u8Base_param_m_n[HDRVIVID_TONE_PARAM_NUM];
    u8 u8Base_param_K1[HDRVIVID_TONE_PARAM_NUM];
    u8 u8Base_param_K2[HDRVIVID_TONE_PARAM_NUM];
    u8 u8Base_param_K3[HDRVIVID_TONE_PARAM_NUM];
    u8 u8base_param_Delta_mode[HDRVIVID_TONE_PARAM_NUM];
    u8 u8base_param_Delta[HDRVIVID_TONE_PARAM_NUM];
    u8 u8P3Spline_flag[HDRVIVID_TONE_PARAM_NUM];
    u8 u8P3Spline_num[HDRVIVID_TONE_PARAM_NUM];
    u8 u8P3Spline_TH_mode[HDRVIVID_TONE_PARAM_NUM][HDRVIVID_3SPLINE_PARAM_NUM];
    u8 u8P3Spline_TH_MB[HDRVIVID_TONE_PARAM_NUM][HDRVIVID_3SPLINE_PARAM_NUM];
    u16 u16P3Spline_TH_enable[HDRVIVID_TONE_PARAM_NUM][HDRVIVID_3SPLINE_PARAM_NUM];
    u16 u16P3Spline_TH_delta1[HDRVIVID_TONE_PARAM_NUM][HDRVIVID_3SPLINE_PARAM_NUM];
    u16 u16P3Spline_TH_delta2[HDRVIVID_TONE_PARAM_NUM][HDRVIVID_3SPLINE_PARAM_NUM];
    u8 u8P3Spline_Strength[HDRVIVID_TONE_PARAM_NUM][HDRVIVID_3SPLINE_PARAM_NUM];
    u8 u8color_saturation_mapping_flag;
    u8 u8color_saturation_num;
    u8 u8color_saturation_gain[HDRVIVID_COLORSAT_PARAM_NUM];
};

struct st_hdrvivid_cb
{
    u32 (*pfnGenPow)(u32 base, u32 exp); /* for base <= 1*/
    u32 (*pfnGet2084Code)(u32 u32Nits); /* 0 ~ 10000 nits */
    u32 (*pfnGet2084Nits)(u32 u32Code); /* 0 ~ 4095 */
    u32 (*pfnGetNormNits)(u32 u32Nits); /* 0 ~ 10000 nits */
    u32 (*pfnGet2084NormCodeIdx)(u32 u32Idx); /* 0 ~ 511 */
    u32 (*pfnGenNormNitsByNormCode)(u32 u32Nits); /* 0 ~ 0x1000000 norm code */
};


struct st_hdrvivid_tmo
{
    u16 u16lut_num;
    u32* pu32_lut;
};

//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
MS_BOOL MApi_HdrVivid_Handler(struct st_hdrvivid_meta *pstu_Param,
    struct st_hdrvivid_cb *pfnCallback,
    struct st_hdrvivid_tmo *psttmo);


#endif // _MDRV_HDRVIVID_IF_H_
