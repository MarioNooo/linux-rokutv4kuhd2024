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

////////////////////////////////////////////////////////////////////////////////
///
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HWREG_PQ_ENHANCE_UCD_H_
#define _HWREG_PQ_ENHANCE_UCD_H_

#include "hwreg_common.h"

//------------------------------------------------------------------------------
//  Driver Capability
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Macro and Define
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Type and Structure
//------------------------------------------------------------------------------
typedef enum
{
    E_UCD_SOURCE_HDIMI0 = 0,
    E_UCD_SOURCE_HDIMI1,
    E_UCD_SOURCE_DNR,
    E_UCD_SOURCE_HDR,
    E_UCD_SOURCE_MEMC,
    E_UCD_SOURCE_MAX,
} E_UCD_INPUT_SOURCE_SELECT;

typedef enum
{
    E_UCD_LINEAR_MODE = 0,
    E_UCD_CURVE_MODE,
    E_UCD_MAX_MODE,
} E_UCD_MODE;

struct UCD_INPUT_FORMAT
{
    bool limit2full_en;
    E_UCD_INPUT_SOURCE_SELECT source;
};

struct UCD_WIND_SIZE
{
    uint16_t mcg_width;
    uint16_t mcg_height;
};

struct UCD_WIND_RATIO
{
    uint32_t dphratio;
    uint32_t dpvratio;
    uint32_t scale_factor_ho;
    uint32_t scale_factor_ve;
    bool en_ho;
    bool en_ve;
    uint32_t ini_factor_ho;
    uint32_t ini_factor_ve;
};

struct STATS_CROP_SIZE {
    uint16_t stat_x;
    uint16_t stat_y;
    uint16_t stat_width;
    uint16_t stat_height;
};

struct UCD_MURA_INFO
{
    uint16_t Adl2MuraGainSize;
    uint32_t MuraCompGainHOffset;
    uint32_t MuraCompGainVOffset;
};



//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
void drv_hwreg_pq_enhance_ucd_set_baseaddress(bool bRIU, struct hwregOut *hwOut,
	uint32_t address);
void drv_hwreg_pq_enhance_ucd_set_force_ini(bool bRIU,struct hwregOut *hwOut,bool en);
void drv_hwreg_pq_enhance_ucd_init(bool bRIU,struct hwregOut *hwOut,bool en);
void drv_hwreg_pq_enhance_ucd_setinput(bool bRIU,struct hwregOut *hwOut,struct UCD_INPUT_FORMAT format);
void drv_hwreg_pq_enhance_ucd_setcrop(bool bRIU,struct hwregOut *hwOut,struct STATS_CROP_SIZE ucdwin);
void drv_hwreg_pq_enhance_ucd_setratio(bool bRIU,struct hwregOut *hwOut,struct UCD_WIND_RATIO win_ratio);
void drv_hwreg_pq_enhance_ucd_setwinsize(bool bRIU,struct hwregOut *hwOut,struct UCD_WIND_SIZE win_size);
void drv_hwreg_pq_enhance_ucd_setflowcontrol(bool bRIU,struct hwregOut *hwOut);
void drv_hwreg_pq_enhance_ucd_setmode(bool bRIU,struct hwregOut *hwOut,E_UCD_MODE mode);
void drv_hwreg_pq_enhance_ucd_setmura(bool bRIU,struct hwregOut *hwOut,struct UCD_MURA_INFO info);
void drv_hwreg_pq_enhance_ucd_setgain_en(bool bRIU,struct hwregOut *hwOut,bool en);
void drv_hwreg_pq_enhance_ucd_setoffset_en(bool bRIU,struct hwregOut *hwOut,bool en);
void drv_hwreg_pq_enhance_ucd_local_color_set_baseaddress(bool bRIU, struct hwregOut *hwOut,
	uint32_t address);
void drv_hwreg_pq_enhance_ucd_local_color_set_blk_size(bool bRIU, struct hwregOut *hwOut,
	uint16_t width, uint16_t height);
void drv_hwreg_pq_enhance_ucd_local_color_set_input_ratio(bool bRIU, struct hwregOut *hwOut,
	uint32_t HRatio, uint32_t VRatio);
void drv_hwreg_pq_enhance_ucd_set_bldma_en(bool bRIU,struct hwregOut *hwOut,bool en);
void drv_hwreg_pq_enhance_ucd_set_input_ratio(bool bRIU, struct hwregOut *hwOut,
	uint32_t HRatio, uint32_t VRatio);
void drv_hwreg_pq_enhance_ucd_set_stat_size(bool bRIU, struct hwregOut *hwOut,
	uint16_t width, uint16_t height);
void drv_hwreg_pq_enhance_ucd_set_blk_size(bool bRIU, struct hwregOut *hwOut,
	uint16_t width, uint16_t height);
#endif // _HWREG_PQ_ENHANCE_UCD_H_