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
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HWREG_RENDER_VIDEO_PIXELSHIFT_H_
#define _HWREG_RENDER_VIDEO_PIXELSHIFT_H_

#include "hwreg_common.h"
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

// Please align with two "video_crtc_pixelshift_type" in mtk_tv_drm.h
enum video_pixelshift_type {
	PIXELSHIFT_PRE_JUSTSCAN,
	PIXELSHIFT_PRE_OVERSCAN,
	PIXELSHIFT_POST_JUSTSCAN,
};

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
int drv_hwreg_render_pixelshift_set_video_h(
		struct hwregOut *paramOut,
		bool RIU,
		int8_t direction);
int drv_hwreg_render_pixelshift_set_graph_h(
		struct hwregOut *paramOut,
		bool RIU,
		int8_t direction);
int drv_hwreg_render_pixelshift_set_vg_h(
		struct hwregOut *paramOut,
		bool RIU,
		int8_t direction);
int drv_hwreg_render_pixelshift_set_video_v(
		struct hwregOut *paramOut,
		bool RIU,
		int8_t direction);
int drv_hwreg_render_pixelshift_set_graph_v(
		struct hwregOut *paramOut,
		bool RIU,
		int8_t direction);
int drv_hwreg_render_pixelshift_set_vg_v(
		struct hwregOut *paramOut,
		bool RIU,
		int8_t direction);
int drv_hwreg_render_pixelshift_set_type(
	struct hwregOut *paramOut,
	bool RIU,
	enum video_pixelshift_type type);
int drv_hwreg_render_pixelshift_set_enable(
	struct hwregOut *paramOut,
	bool RIU,
	bool bEnable);



#endif // _HWREG_RENDER_VIDEO_PIXELSHIFT_H_
