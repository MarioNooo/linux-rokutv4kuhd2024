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

#ifndef _HWREG_PQ_DISPLAY_UCM_H_
#define _HWREG_PQ_DISPLAY_UCM_H_

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
void drv_hwreg_pq_display_ucm_set_address(bool bRIU,
	struct hwregOut *hwOut, uint64_t base_addr_r,
	uint64_t base_addr_g, uint64_t base_addr_b);
void drv_hwreg_pq_display_ucm_set_hOffset(bool bRIU,
	struct hwregOut *hwOut, uint16_t src_size);
void drv_hwreg_pq_display_ucm_set_framePitch(bool bRIU,
	struct hwregOut *hwOut, uint64_t frame_pitch_r,
	uint64_t frame_pitch_g, uint64_t frame_pitch_b);
void drv_hwreg_pq_display_ucm_set_dmaInputSize(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info InputWin);
void drv_hwreg_pq_display_ucm_set_dmaOutputSize(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info OutputWin);
void drv_hwreg_pq_display_znr_set_address(bool bRIU,
	struct hwregOut *hwOut, uint64_t base_addr_0, uint64_t base_addr_1);
void drv_hwreg_pq_display_znr_set_framePitch(bool bRIU,
	struct hwregOut *hwOut, uint64_t frame_pitch_0, uint64_t frame_pitch_1);
void drv_hwreg_pq_display_znr_set_dmaInputSize(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info InputWin);
void drv_hwreg_pq_display_znr_set_dmaOutputSize(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info OutputWin);
void drv_hwreg_pq_display_znr_set_avgEn(bool bRIU,
	struct hwregOut *hwOut, bool h_avg_en, bool v_avg_en, bool mi_1d_en);
void drv_hwreg_pq_display_znr_set_lbctrl(bool bRIU,
	struct hwregOut *hwOut, bool znr_me_bwb_lbctrl_bypass, bool znr_lbctrl_bypass);
void drv_hwreg_pq_display_znr_set_interlace(bool bRIU, struct hwregOut *hwOut, bool bInterlace);

#endif // _HWREG_PQ_DISPLAY_UCM_H_
