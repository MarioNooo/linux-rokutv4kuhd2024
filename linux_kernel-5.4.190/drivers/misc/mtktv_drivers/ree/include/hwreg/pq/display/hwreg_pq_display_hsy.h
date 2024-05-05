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

#ifndef _HWREG_PQ_DISPLAY_HSY_H_
#define _HWREG_PQ_DISPLAY_HSY_H_

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
enum hwreg_pq_hsy_bypass_type {
	HWREG_PQ_HSY_NOT_BYPASS,
	HWREG_PQ_HSY_FUNCTION_BYPASS,
	HWREG_PQ_HSY_PATH_BYPASS,
};

enum hwreg_pq_hsy_lut_type {
	HWREG_PQ_HSY_HUE_BY_HUE_LUT,
	HWREG_PQ_HSY_HUE_BY_SAT_LUT,
	HWREG_PQ_HSY_HUE_BY_LUMA_LUT,
	HWREG_PQ_HSY_SAT_BY_HUE_LUT,
	HWREG_PQ_HSY_SAT_BY_HUE_MAX_LUT,
	HWREG_PQ_HSY_SAT_BY_HUE_MIN_LUT,
	HWREG_PQ_HSY_SAT_BY_LUMA_LUT,
	HWREG_PQ_HSY_LIG_BY_SAT_LUT,
	HWREG_PQ_HSY_LIG_BY_LIG_LUT,
	HWREG_PQ_HSY_LUMA_GAIN_THRLD_LUT,
	HWREG_PQ_HSY_LUMA_GAIN_SLOPE_LUT,
	HWREG_PQ_HSY_ENHANCE_CURVE_LUT,

	HWREG_PQ_HSY_LUT_MAX,
};

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
int drv_hwreg_pq_display_hsy_bypass(bool bRIU,
	struct hwregOut *hwOut, enum hwreg_pq_hsy_bypass_type enBypassType);
int drv_hwreg_pq_display_hsy_get_readback_lut(uint16_t addr,
	enum hwreg_pq_hsy_lut_type type, uint16_t *pdata);
void drv_hwreg_pq_display_hsy_set_size(bool bRIU, struct hwregOut *hwOut,
	uint16_t width, uint16_t height);
bool drv_hwreg_pq_display_hsy_set_blk_size(bool bRIU,
	struct hwregOut *hwOut,uint16_t width, uint16_t height);
bool drv_hwreg_pq_display_hsy_set_ini_factor(bool bRIU,
	struct hwregOut *hwOut, uint32_t ho,uint32_t ve);
bool drv_hwreg_pq_display_hsy_set_shift_en(bool bRIU,
	struct hwregOut *hwOut, bool h_en, bool v_en);
bool drv_hwreg_pq_display_hsy_set_scale_en(bool bRIU,
	struct hwregOut *hwOut, bool h_en, bool v_en);
bool drv_hwreg_pq_display_hsy_set_scale_ratio(bool bRIU,
	struct hwregOut *hwOut, uint32_t h_rotio, uint32_t v_rotio);
bool drv_hwreg_pq_display_hsy_set_adl_cnt(bool bRIU,
	struct hwregOut *hwOut, uint32_t h_cnt, uint32_t v_cnt);
bool drv_hwreg_pq_display_hsy_set_adl_write_done(bool bRIU,
	struct hwregOut *hwOut, bool en);
#endif // _HWREG_PQ_DISPLAY_HSY_H_
