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

#ifndef _HWREG_SRCCAP_PATTERN_H_
#define _HWREG_SRCCAP_PATTERN_H_

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
enum hwreg_srccap_pattern_position {
	HWREG_SRCCAP_PATTERN_POSITION_IP1,
	HWREG_SRCCAP_PATTERN_POSITION_PRE_IP2_0,
	HWREG_SRCCAP_PATTERN_POSITION_PRE_IP2_1,
	HWREG_SRCCAP_PATTERN_POSITION_PRE_IP2_0_POST,
	HWREG_SRCCAP_PATTERN_POSITION_PRE_IP2_1_POST,
	HWREG_SRCCAP_PATTERN_POSITION_MAX,
};

enum hwreg_srccap_pattern_color_space {
	HWREG_SRCCAP_PATTERN_COLOR_SPACE_RGB,
	HWREG_SRCCAP_PATTERN_COLOR_SPACE_YUV_FULL,
	HWREG_SRCCAP_PATTERN_COLOR_SPACE_YUV_LIMITED,
	HWREG_SRCCAP_PATTERN_COLOR_SPACE_MAX,
};

//-------------------------------------------------------------------------------------------------
//  structs
//-------------------------------------------------------------------------------------------------
struct hwreg_srccap_pattern_common_data {
	enum hwreg_srccap_pattern_position position;
	enum hwreg_srccap_pattern_color_space color_space;
	unsigned short h_size;
	unsigned short v_size;
	bool hsk_force_signal;
	bool cbcr_swap;
	bool enable;
};

struct hwreg_srccap_pattern_pure_colorbar {
	struct hwreg_srccap_pattern_common_data common;
	unsigned int diff_h;
	unsigned int diff_v;
	unsigned char shift_speed;
	bool movable;
	bool move2left;
};

struct hwreg_srccap_pattern_capability {
	enum hwreg_srccap_pattern_position position;
	unsigned short pattern_type;
};

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
int drv_hwreg_srccap_pattern_get_capability(
	struct hwreg_srccap_pattern_capability *pattern_cap);

int drv_hwreg_srccap_pattern_set_off(bool riu,
	enum hwreg_srccap_pattern_position point, struct hwregOut *pattern_reg);

int drv_hwreg_srccap_pattern_set_pure_colorbar(bool riu,
	struct hwreg_srccap_pattern_pure_colorbar *data, struct hwregOut *pattern_reg);

#endif // _HWREG_SRCCAP_PATTERN_H_
