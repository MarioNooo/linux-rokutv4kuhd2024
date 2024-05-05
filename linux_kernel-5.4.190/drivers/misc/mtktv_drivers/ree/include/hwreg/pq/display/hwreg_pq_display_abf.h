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

#ifndef _HWREG_PQ_DISPLAY_ABF_H_
#define _HWREG_PQ_DISPLAY_ABF_H_

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

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
int drv_hwreg_pq_display_set_stub(bool stub);
int drv_hwreg_pq_display_get_stub(bool *stub);
void drv_hwreg_pq_display_abf_set_address(bool bRIU,
	struct hwregOut *hwOut, uint64_t base_addr_0, uint64_t base_addr_1, uint64_t base_addr_2);
void drv_hwreg_pq_display_abf_set_winSize(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info InputWin,
	struct hwreg_pq_window_info OutputWin);
void drv_hwreg_pq_display_abf_set_blkMode(bool bRIU,
	struct hwregOut *hwOut,
	uint16_t ctur_blk_md, uint16_t abf_blk_act_end_x,
	uint16_t abf_blk_act_end_y, uint16_t ctur2_blk_md,
	uint16_t ctur2_blk_act_end_x, uint16_t ctur2_blk_act_end_y);
void drv_hwreg_pq_display_abf_set_ds(bool bRIU,
	struct hwregOut *hwOut,
	bool abf_ds_en);
void drv_hwreg_pq_display_abf_set_interlace(bool bRIU, struct hwregOut *hwOut, bool bInterlace);
void drv_hwreg_pq_display_abf_set_patch(bool bRIU, struct hwregOut *hwOut);
void drv_hwreg_pq_display_abf_set_ctur2Size(bool bRIU,
	struct hwregOut *hwOut,
	uint16_t ctur2_size_en, uint16_t ctur2_base_en, uint16_t ctur2_delay_time,
	uint16_t ctur2_size_w, uint16_t ctur2_size_h);

#endif // _HWREG_PQ_DISPLAY_UCM_H_
