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

#ifndef _HWREG_PQ_DISPLAY_PATTERN_H_
#define _HWREG_PQ_DISPLAY_PATTERN_H_

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
enum hwreg_pq_pattern_position {
	HWREG_PQ_PATTERN_POSITION_IP2,
	HWREG_PQ_PATTERN_POSITION_NR_DNR,
	HWREG_PQ_PATTERN_POSITION_NR_IPMR,
	HWREG_PQ_PATTERN_POSITION_NR_OPM,
	HWREG_PQ_PATTERN_POSITION_NR_DI,
	HWREG_PQ_PATTERN_POSITION_DI,
	HWREG_PQ_PATTERN_POSITION_SRS_IN,
	HWREG_PQ_PATTERN_POSITION_SRS_OUT,
	HWREG_PQ_PATTERN_POSITION_VOP,
	HWREG_PQ_PATTERN_POSITION_IP2_POST,
	HWREG_PQ_PATTERN_POSITION_SCIP_DV,
	HWREG_PQ_PATTERN_POSITION_SCDV_DV,
	HWREG_PQ_PATTERN_POSITION_B2R_LITE1_DMA,
	HWREG_PQ_PATTERN_POSITION_B2R_DMA,
	HWREG_PQ_PATTERN_POSITION_B2R_PRE,
	HWREG_PQ_PATTERN_POSITION_B2R_POST,
	HWREG_PQ_PATTERN_POSITION_MAX,
};

enum hwreg_pq_pattern_color_space {
	HWREG_PQ_PATTERN_COLOR_SPACE_RGB,
	HWREG_PQ_PATTERN_COLOR_SPACE_YUV_FULL,
	HWREG_PQ_PATTERN_COLOR_SPACE_YUV_LIMITED,
	HWREG_PQ_PATTERN_COLOR_SPACE_MAX,
};

enum hwreg_pq_pattern_colorbar_color {
	HWREG_PQ_PATTERN_COLORBAR_WHITE,
	HWREG_PQ_PATTERN_COLORBAR_BLUE,
	HWREG_PQ_PATTERN_COLORBAR_GREEN,
	HWREG_PQ_PATTERN_COLORBAR_RED,
	HWREG_PQ_PATTERN_COLORBAR_MAX,
};

//-------------------------------------------------------------------------------------------------
//  structs
//-------------------------------------------------------------------------------------------------
struct hwreg_pq_pattern_color {
	unsigned short red;		//10bit
	unsigned short green;	//10bit
	unsigned short blue;		//10bit
};

struct hwreg_pq_pattern_float_color {
	unsigned short red_integer;		//10bit
	unsigned short red_decimal;		//13bit
	unsigned short green_integer;		//10bit
	unsigned short green_decimal;	//13bit
	unsigned short blue_integer;		//10bit
	unsigned short blue_decimal;		//13bit
};

struct hwreg_pq_pattern_common_data {
	enum hwreg_pq_pattern_position position;
	enum hwreg_pq_pattern_color_space color_space;
	unsigned short h_size;
	unsigned short v_size;
	bool hsk_force_signal;
	bool cbcr_swap;
	bool enable;
};

struct hwreg_pq_pattern_pure_color {
	struct hwreg_pq_pattern_common_data common;
	struct hwreg_pq_pattern_color color;
};

struct hwreg_pq_pattern_ramp {
	struct hwreg_pq_pattern_common_data common;
	struct hwreg_pq_pattern_float_color start;
	struct hwreg_pq_pattern_float_color end;
	unsigned int diff_h;
	unsigned int ratio_h;
	unsigned int diff_v;
	unsigned int ratio_v;
	bool b_vertical;
	bool b_color_to_black;
};

struct hwreg_pq_pattern_pure_colorbar {
	struct hwreg_pq_pattern_common_data common;
	unsigned int diff_h;
	unsigned int diff_v;
	unsigned char shift_speed;
	bool movable;
	bool move2left;
};

struct hwreg_pq_pattern_gradient_colorbar {
	struct hwreg_pq_pattern_common_data common;
	struct hwreg_pq_pattern_float_color start;
	struct hwreg_pq_pattern_float_color end;
	enum hwreg_pq_pattern_colorbar_color color0;
	enum hwreg_pq_pattern_colorbar_color color1;
	enum hwreg_pq_pattern_colorbar_color color2;
	enum hwreg_pq_pattern_colorbar_color color3;
	unsigned int diff_h;
	unsigned int ratio_h;
	unsigned int diff_v;
	unsigned int ratio_v;
	bool b_vertical;
	bool b_color_to_black;
};

struct hwreg_pq_pattern_black_white_colorbar {
	struct hwreg_pq_pattern_common_data common;
	struct hwreg_pq_pattern_color first_color;
	struct hwreg_pq_pattern_color second_color;
	unsigned int diff_h;
	unsigned int diff_v;
};

struct hwreg_pq_pattern_cross {
	struct hwreg_pq_pattern_common_data common;
	struct hwreg_pq_pattern_color cross_color;
	struct hwreg_pq_pattern_color border_color;
	unsigned int cross_position_h;
	unsigned int cross_position_v;
};

struct hwreg_pq_pattern_dot_matrix {
	struct hwreg_pq_pattern_common_data common;
	unsigned char addr;
	unsigned short data;
	bool manual_mode;
	unsigned short h_pos;
	unsigned short v_pos;
};

struct hwreg_pq_pattern_ts {
	enum hwreg_pq_pattern_position position;
	unsigned char win_id;
	bool enable;
};

struct hwreg_pq_pattern_capability {
	enum hwreg_pq_pattern_position position;
	unsigned short pattern_type;
};

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
int drv_hwreg_pq_pattern_get_capability(
	struct hwreg_pq_pattern_capability *pattern_cap);

int drv_hwreg_pq_pattern_set_off(bool riu,
	enum hwreg_pq_pattern_position point, struct hwregOut *pattern_reg);

int drv_hwreg_pq_pattern_set_pure_color(bool riu,
	struct hwreg_pq_pattern_pure_color *data, struct hwregOut *pattern_reg);

int drv_hwreg_pq_pattern_set_ramp(bool riu,
	struct hwreg_pq_pattern_ramp *data, struct hwregOut *pattern_reg);

int drv_hwreg_pq_pattern_set_pure_colorbar(bool riu,
	struct hwreg_pq_pattern_pure_colorbar *data, struct hwregOut *pattern_reg);

int drv_hwreg_pq_pattern_set_gradient_colorbar(bool riu,
	struct hwreg_pq_pattern_gradient_colorbar *data, struct hwregOut *pattern_reg);

int drv_hwreg_pq_pattern_set_black_white_colorbar(bool riu,
	struct hwreg_pq_pattern_black_white_colorbar *data, struct hwregOut *pattern_reg);

int drv_hwreg_pq_pattern_set_cross(bool riu,
	struct hwreg_pq_pattern_cross *data, struct hwregOut *pattern_reg);

int drv_hwreg_pq_pattern_update_dot_matrix(bool riu,
	struct hwreg_pq_pattern_dot_matrix *data, struct hwregOut *pattern_reg);

int drv_hwreg_pq_pattern_set_dot_matrix(bool riu,
	struct hwreg_pq_pattern_dot_matrix *data, struct hwregOut *pattern_reg);

int drv_hwreg_pq_pattern_set_ts(bool riu,
	struct hwreg_pq_pattern_ts *data, struct hwregOut *pattern_reg);

#endif // _HWREG_PQ_DISPLAY_PATTERN_H_

