/* SPDX-License-Identifier: GPL-2.0
 * Copyright (c) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _M_PQU_PATTERN_H_
#define _M_PQU_PATTERN_H_

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#else
#include "pqu_port.h"
#endif

//-----------------------------------------------------------------------------
//  Driver Capability
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macro and Define
//-----------------------------------------------------------------------------
#define M_PQ_PATTERN_INFO_TAG					"m_pq_pattern_info"
#define M_PQ_PATTERN_INFO_VERSION				(1)
#define M_PQ_PATTERN_SIZE_INFO_TAG				"m_pq_pattern_size_info"
#define M_PQ_PATTERN_SIZE_INFO_VERSION			(1)
#if defined(__KERNEL__)
#define PQU_PATTERN_TEST
#endif

#ifndef BIT
#define BIT(_bit_)								(1 << (_bit_))
#endif

#ifndef MTK_PATTERN_FLAG_PURE_COLOR
#define MTK_PATTERN_FLAG_PURE_COLOR				(BIT(0))
#endif

#ifndef MTK_PATTERN_FLAG_RAMP
#define MTK_PATTERN_FLAG_RAMP					(BIT(1))
#endif

#ifndef MTK_PATTERN_FLAG_PURE_COLORBAR
#define MTK_PATTERN_FLAG_PURE_COLORBAR			(BIT(2))
#endif

#ifndef MTK_PATTERN_FLAG_GRADIENT_COLORBAR
#define MTK_PATTERN_FLAG_GRADIENT_COLORBAR		(BIT(3))
#endif

#ifndef MTK_PATTERN_FLAG_BLACK_WHITE_COLORBAR
#define MTK_PATTERN_FLAG_BLACK_WHITE_COLORBAR	(BIT(4))
#endif

#ifndef MTK_PATTERN_FLAG_CROSS
#define MTK_PATTERN_FLAG_CROSS					(BIT(5))
#endif

#ifndef MTK_PATTERN_FLAG_PIP_WINDOW
#define MTK_PATTERN_FLAG_PIP_WINDOW				(BIT(6))
#endif

#ifndef MTK_PATTERN_FLAG_DOT_MATRIX
#define MTK_PATTERN_FLAG_DOT_MATRIX				(BIT(7))
#endif

#ifndef MTK_PATTERN_FLAG_CROSSHATCH
#define MTK_PATTERN_FLAG_CROSSHATCH				(BIT(8))
#endif

#define MATRIX_MAX_ADDR							(128)
#define MIN_DIFF_LEVEL							(4)
#define MATRIX_MAX_CHAR							(32)
#define INVALID_PATTERN_POSITION				(0xFF)
//-----------------------------------------------------------------------------
//  Type and Structure
//-----------------------------------------------------------------------------
enum pq_pattern_position {
	PQ_PATTERN_POSITION_IP2,
	PQ_PATTERN_POSITION_NR_DNR,
	PQ_PATTERN_POSITION_NR_IPMR,
	PQ_PATTERN_POSITION_NR_OPM,
	PQ_PATTERN_POSITION_NR_DI,
	PQ_PATTERN_POSITION_DI,
	PQ_PATTERN_POSITION_SRS_IN,
	PQ_PATTERN_POSITION_SRS_OUT,
	PQ_PATTERN_POSITION_VOP,
	PQ_PATTERN_POSITION_IP2_POST,
	PQ_PATTERN_POSITION_SCIP_DV,
	PQ_PATTERN_POSITION_SCDV_DV,
	PQ_PATTERN_POSITION_B2R_DMA,
	PQ_PATTERN_POSITION_B2R_LITE1_DMA,
	PQ_PATTERN_POSITION_B2R_PRE,
	PQ_PATTERN_POSITION_B2R_POST,
	PQ_PATTERN_POSITION_MAX,
};

enum pq_pattern_colorbar_color {
	PQ_PATTERN_COLOR_WHITE,
	PQ_PATTERN_COLOR_BLUE,
	PQ_PATTERN_COLOR_GREEN,
	PQ_PATTERN_COLOR_RED,
	PQ_PATTERN_COLOR_MAX,
};

struct pq_pattern_size_info {
	unsigned char win_id;
	unsigned short h_size[PQ_PATTERN_POSITION_MAX];
	unsigned short v_size[PQ_PATTERN_POSITION_MAX];
};

struct pq_pattern_color {
	unsigned short red;		//10 bits
	unsigned short green;	//10 bits
	unsigned short blue;	//10 bits
};

struct pq_pattern_pure_color {
	struct pq_pattern_color color;
};

struct pq_pattern_ramp {
	struct pq_pattern_color start;
	struct pq_pattern_color end;
	bool b_vertical;
	unsigned short diff_level;
};

struct pq_pattern_pure_colorbar {
	unsigned int colorbar_h_size;
	unsigned int colorbar_v_size;
	unsigned char shift_speed;
	bool movable;
};

struct pq_pattern_gradient_colorbar {
	struct pq_pattern_color start;
	struct pq_pattern_color end;
	enum pq_pattern_colorbar_color color_1st;
	enum pq_pattern_colorbar_color color_2nd;
	enum pq_pattern_colorbar_color color_3rd;
	enum pq_pattern_colorbar_color color_4th;
	unsigned short diff_level;
	bool b_vertical;
};

struct pq_pattern_black_white_colorbar {
	struct pq_pattern_color first_color;
	struct pq_pattern_color second_color;
	unsigned int colorbar_h_size;
	unsigned int colorbar_v_size;
};

struct pq_pattern_cross {
	struct pq_pattern_color cross_color;
	struct pq_pattern_color border_color;
	unsigned short h_position;
	unsigned short v_position;
};

struct pq_pattern_dot_matrix {
	unsigned short matrix_data[MATRIX_MAX_ADDR];
	unsigned short h_pos;
	unsigned short v_pos;
};

struct pq_pattern_info {
	enum pq_pattern_position position;
	unsigned short pattern_type;
	unsigned char win_id;
	union {
		struct pq_pattern_pure_color pure_color;
		struct pq_pattern_ramp ramp;
		struct pq_pattern_pure_colorbar pure_colorbar;
		struct pq_pattern_gradient_colorbar gradient_colorbar;
		struct pq_pattern_black_white_colorbar black_white_colorbar;
		struct pq_pattern_cross cross;
		struct pq_pattern_dot_matrix dot_matrix;
	} m;
	bool enable_ts;
	bool enable;
};

struct pq_pattern_capability {
	unsigned short pattern_type[PQ_PATTERN_POSITION_MAX];
};

enum stage_pattern_color_space {
	COLOR_SPACE_RGB,
	COLOR_SPACE_YUV_FULL,
	COLOR_SPACE_YUV_LIMITED,
	COLOR_SPACE_MAX,
};

struct stage_pattern_info {
	bool enable;
	enum pq_pattern_position position;
	enum stage_pattern_color_space color_space;
	struct pq_pattern_color color;
};

//-----------------------------------------------------------------------------
//  Function and Variable
//-----------------------------------------------------------------------------

#endif
