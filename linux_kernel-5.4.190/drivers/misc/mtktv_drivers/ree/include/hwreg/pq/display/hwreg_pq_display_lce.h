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

#ifndef _HWREG_PQ_DISPLAY_LCE_H_
#define _HWREG_PQ_DISPLAY_LCE_H_
#include "hwreg_common.h"
#include "hwreg_pq_display_scmi.h"


//------------------------------------------------------------------------------
//  Driver Capability
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Macro and Define
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Type and Structure
//------------------------------------------------------------------------------
struct hwreg_pq_lce_ace_config {
	uint16_t blk_height;
	uint16_t blk_width;
	uint16_t blk_x_num;
	uint16_t blk_y_num;
	uint32_t hsp_scale_factor_h;
	uint32_t vsp_scale_factor_v;
	uint16_t hsize;
};

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
int drv_hwreg_pq_display_lce_ace_adl_enable(bool bRIU, struct hwregOut *hwOut, bool enable);
int drv_hwreg_pq_display_lce_ace_adl_config(bool bRIU, struct hwregOut *hwOut, struct hwreg_pq_lce_ace_config blk);
int drv_hwreg_pq_display_lce_ace_get_sram(uint16_t addr, void *pData);

#endif // _HWREG_PQ_DISPLAY_LCE_H_