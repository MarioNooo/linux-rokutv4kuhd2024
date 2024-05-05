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

#ifndef _HWREG_RENDER_PATTERN_H_
#define _HWREG_RENDER_PATTERN_H_

#include "hwreg_common.h"

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  enums
//-------------------------------------------------------------------------------------------------
enum hwreg_render_pattern_position {
	HWREG_RENDER_PATTERN_POSITION_OPDRAM,
	HWREG_RENDER_PATTERN_POSITION_IPBLEND,
	HWREG_RENDER_PATTERN_POSITION_MULTIWIN,
	HWREG_RENDER_PATTERN_POSITION_TCON,
	HWREG_RENDER_PATTERN_POSITION_GFX,
	HWREG_RENDER_PATTERN_POSITION_MAX,
};

enum hwreg_render_pattern_color_space {
	HWREG_RENDER_PATTERN_COLOR_SPACE_RGB,
	HWREG_RENDER_PATTERN_COLOR_SPACE_YUV_FULL,
	HWREG_RENDER_PATTERN_COLOR_SPACE_YUV_LIMITED,
	HWREG_RENDER_PATTERN_COLOR_SPACE_MAX,
};

//-------------------------------------------------------------------------------------------------
//  structs
//-------------------------------------------------------------------------------------------------
struct hwreg_render_pattern_window {
	unsigned short h_start;
	unsigned short h_end;
	unsigned short v_start;
	unsigned short v_end;
};

struct hwreg_render_pattern_color {
	unsigned short red;		//10bit
	unsigned short green;	//10bit
	unsigned short blue;		//10bit
};

struct hwreg_render_pattern_float_color {
	unsigned short red_integer;		//10bit
	unsigned short red_decimal;		//13bit
	unsigned short green_integer;		//10bit
	unsigned short green_decimal;	//13bit
	unsigned short blue_integer;		//10bit
	unsigned short blue_decimal;		//13bit
};

struct hwreg_render_pattern_common_data {
	enum hwreg_render_pattern_position position;
	enum hwreg_render_pattern_color_space color_space;
	unsigned short h_size;
	unsigned short v_size;
	bool hsk_force_signal;
	bool cbcr_swap;
	bool enable;
};

struct hwreg_render_pattern_pure_color {
	struct hwreg_render_pattern_common_data common;
	struct hwreg_render_pattern_color color;
};

struct hwreg_render_pattern_ramp {
	struct hwreg_render_pattern_common_data common;
	struct hwreg_render_pattern_float_color start;
	struct hwreg_render_pattern_float_color end;
	unsigned int diff_h;
	unsigned int ratio_h;
	unsigned int diff_v;
	unsigned int ratio_v;
	bool b_vertical;
	bool b_color_to_black;
};

struct hwreg_render_pattern_pure_colorbar {
	struct hwreg_render_pattern_common_data common;
	unsigned int diff_h;
	unsigned int diff_v;
	unsigned char shift_speed;
	bool movable;
	bool move2left;
};

struct hwreg_render_pattern_pip_window {
	struct hwreg_render_pattern_common_data common;
	struct hwreg_render_pattern_float_color pip;
	struct hwreg_render_pattern_float_color bg;
	struct hwreg_render_pattern_window window;
};

struct hwreg_render_pattern_crosshatch {
	struct hwreg_render_pattern_common_data common;
	struct hwreg_render_pattern_float_color line;
	struct hwreg_render_pattern_float_color bg;
	unsigned int h_line_interval;
	unsigned int v_line_interval;
	unsigned short h_position_st;
	unsigned short h_line_width;
	unsigned short v_position_st;
	unsigned short v_line_width;
};

struct hwreg_render_pattern_capability {
	enum hwreg_render_pattern_position position;
	unsigned short pattern_type;
};

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
int drv_hwreg_render_pattern_get_capability(
	struct hwreg_render_pattern_capability *pattern_cap);

int drv_hwreg_render_pattern_opdram_set_off(bool riu);

int drv_hwreg_render_pattern_ipblend_set_off(bool riu,
	struct hwregOut *pattern_reg);

int drv_hwreg_render_pattern_opblend_set_off(bool riu,
	enum hwreg_render_pattern_position point, struct hwregOut *pattern_reg);

int drv_hwreg_render_pattern_set_opdram_pure_color(bool riu,
	struct hwreg_render_pattern_pure_color *data, struct hwregOut *pattern_reg);

int drv_hwreg_render_pattern_set_ipblend_pure_color(bool riu,
	struct hwregOut *pattern_reg);

int drv_hwreg_render_pattern_set_opblend_pure_color(bool riu,
	struct hwreg_render_pattern_pure_color *data, struct hwregOut *pattern_reg);

int drv_hwreg_render_pattern_set_ramp(bool riu,
	struct hwreg_render_pattern_ramp *data, struct hwregOut *pattern_reg);

int drv_hwreg_render_pattern_set_pure_colorbar(bool riu,
	struct hwreg_render_pattern_pure_colorbar *data, struct hwregOut *pattern_reg);

int drv_hwreg_render_pattern_set_pip_window(bool riu,
	struct hwreg_render_pattern_pip_window *data, struct hwregOut *pattern_reg);

int drv_hwreg_render_pattern_set_crosshatch(bool riu,
	struct hwreg_render_pattern_crosshatch *data, struct hwregOut *pattern_reg);

int drv_hwreg_render_pattern_set_random(bool riu,
	struct hwregOut *pattern_reg);

int drv_hwreg_render_pattern_set_chessboard(bool riu,
	struct hwregOut *pattern_reg);

#endif // _HWREG_RENDER_PATTERN_H_
