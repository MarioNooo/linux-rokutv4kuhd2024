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

#ifndef _HWREG_PQ_DISPLAY_SCMI_H_
#define _HWREG_PQ_DISPLAY_SCMI_H_

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
struct hwreg_scmi_vflip {
	bool bflip;
	bool bIPflip; /*ip flip or op flip*/
	bool bInterlace;
};

struct hwreg_pq_window_info {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint16_t w_align;
	uint16_t h_align;
};

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
void drv_hwreg_pq_display_scmi_set_address(bool bRIU,
	struct hwregOut *hwOut, uint64_t base_addr_r,
	uint64_t base_addr_g, uint64_t base_addr_b);
void drv_hwreg_pq_display_scmi_set_hOffset(bool bRIU,
	struct hwregOut *hwOut, uint16_t src_size);
void drv_hwreg_pq_display_scmi_set_framePitch(bool bRIU,
	struct hwregOut *hwOut, uint64_t frame_pitch_r,
	uint64_t frame_pitch_g, uint64_t frame_pitch_b);
void drv_hwreg_pq_display_scmi_set_vflip(bool bRIU,
	struct hwregOut *hwOut, struct hwreg_scmi_vflip vflip);
void drv_hwreg_pq_display_scmi_set_dmaInputSize(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info inputWin);
void drv_hwreg_pq_display_scmi_set_dmaOutputSize(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info outputWin);
void drv_hwreg_pq_display_scmi_set_ipmrSize(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info ipmrWin);
void drv_hwreg_pq_display_scmi_set_opmwSize(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info opmwWin);
void drv_hwreg_pq_display_scmi_get_vSize(uint16_t *pu16vSize);
void drv_hwreg_pq_display_scmi_get_hSize(uint16_t *pu16hSize);
void drv_hwreg_pq_display_scmi_get_hOffset(uint16_t *pu16hOffset);

#endif // _HWREG_PQ_DISPLAY_SCMI_H_
