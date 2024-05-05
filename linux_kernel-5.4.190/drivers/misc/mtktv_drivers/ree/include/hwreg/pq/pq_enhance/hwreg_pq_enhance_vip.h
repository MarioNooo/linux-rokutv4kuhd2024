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

#ifndef _HWREG_PQ_ENHANCE_VIP_H_
#define _HWREG_PQ_ENHANCE_VIP_H_

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

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
void drv_hwreg_pq_enhance_1st_vip_set_size(bool bRIU, struct hwregOut *hwOut,
	uint16_t width, uint16_t height);
void drv_hwreg_pq_enhance_vip_set_size(bool bRIU, struct hwregOut *hwOut,
	uint16_t width, uint16_t height);
void drv_hwreg_pq_enhance_vip_set_en(bool bRIU, struct hwregOut *hwOut,
    bool en);
bool drv_hwreg_pq_enhance_vip_set_blk_size(bool bRIU,
	struct hwregOut *hwOut,uint16_t width, uint16_t height);
bool drv_hwreg_pq_enhance_vip_set_ini_factor(bool bRIU,
	struct hwregOut *hwOut, uint32_t ho,uint32_t ve);
bool drv_hwreg_pq_enhance_vip_set_shift_en(bool bRIU,
	struct hwregOut *hwOut, bool h_en, bool v_en);
bool drv_hwreg_pq_enhance_vip_set_scale_en(bool bRIU,
	struct hwregOut *hwOut, bool h_en, bool v_en);
bool drv_hwreg_pq_enhance_vip_set_scale_ratio(bool bRIU,
	struct hwregOut *hwOut, uint32_t h_rotio, uint32_t v_rotio);
bool drv_hwreg_pq_enhance_vip_clk_gating_en(bool bRIU, struct hwregOut *hwOut,	bool en);
bool drv_hwreg_pq_enhance_vip_set_adl_cnt(bool bRIU,
	struct hwregOut *hwOut, uint32_t h_cnt, uint32_t v_cnt);
bool drv_hwreg_pq_enhance_vip_set_adl_write_done(bool bRIU,
	struct hwregOut *hwOut, bool en);
#endif // _HWREG_PQ_ENHANCE_VIP_H_