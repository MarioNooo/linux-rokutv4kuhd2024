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

#ifndef _PQU_B2R_TEST_H_
#define _PQU_B2R_TEST_H_

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#else
#include "pqu_port.h"
#endif
#include "m_pqu_pq.h"
#include "m_pqu_pattern.h"
//-----------------------------------------------------------------------------
//  Driver Capability
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macro and Define
//-----------------------------------------------------------------------------
/*  Four-character-code (FOURCC) */
#define v4l2_fourcc(a, b, c, d)\
	((__u32)(a) | ((__u32)(b) << 8) | ((__u32)(c) << 16) | ((__u32)(d) << 24))

/* MTK data format */
#define V4L2_PIX_FMT_MT21C    v4l2_fourcc('M', 'T', '2', '1') /* Mediatek compressed block mode  */

#define V4L2_PIX_FMT_MT21C    v4l2_fourcc('M', 'T', '2', '1') /* Mediatek compressed block mode  */
#define V4L2_PIX_FMT_YUYV    v4l2_fourcc('Y', 'U', 'Y', 'V') /* 16  YUV 4:2:2     */
#define V4L2_PIX_FMT_ARGB32  v4l2_fourcc('B', 'A', '2', '4') /* 32  ARGB-8-8-8-8  */
#define V4L2_PIX_FMT_NV12    v4l2_fourcc('N', 'V', '1', '2') /* 12  Y/CbCr 4:2:0  */
#define V4L2_PIX_FMT_NV21    v4l2_fourcc('N', 'V', '2', '1') /* 12  Y/CrCb 4:2:0  */
//-----------------------------------------------------------------------------
//  Type and Structure
//-----------------------------------------------------------------------------
enum b2r_test_dev_id {
	B2R_TEST_DEV_ID_0,
	B2R_TEST_DEV_ID_1,
	B2R_TEST_DEV_ID_2,
};

enum b2r_test_field_type {
	B2R_TEST_FIELD_PRO,
	B2R_TEST_FIELD_INT,
	B2R_TEST_FIELD_ALT,
};

enum b2r_test_rotate {
	B2R_TEST_ROT_0,
	B2R_TEST_ROT_90,
	B2R_TEST_ROT_180,
	B2R_TEST_ROT_270,
};

enum b2r_test_wtgen_clk_src {
	B2R_TEST_WT_CLK_SRC_NONE             = 0x0,
	B2R_TEST_WT_CLK_SRC_XC_SLW           = 0x1,
	B2R_TEST_WT_CLK_SRC_STC_0            = 0x2,
	B2R_TEST_WT_CLK_SRC_STC_1            = 0x3,
};

struct b2r_test_info {
	enum b2r_test_dev_id  b2r_id;
	uint8_t  win_id;
	uint64_t luma_fb_base;
	uint64_t chroma_fb_base;
	uint64_t luma_bl_fb_base;
	uint64_t chroma_bl_fb_base;
	uint16_t start_x;
	uint16_t start_y;
	uint16_t width;
	uint16_t height;
	uint16_t pitch;
	uint32_t fourcc;
	enum b2r_test_field_type field_type;
	bool field;
	uint16_t output_width; //thinnig read
	uint16_t output_height;
	enum b2r_test_rotate rotate;
	struct {
		uint32_t yuv422:1;
		uint32_t rgb:3;
		uint32_t compress:1;
		uint32_t bit_depth:6;
		uint32_t lsb_raster:1;
		uint32_t comp_jump:1;
		uint32_t tile_16x32:1;
		uint32_t vsd_ce_mode:1;
		uint32_t auo_mode:1;
		uint32_t dummy:15;
		} fmt;
	enum pqu_fmt_modifier_vsd vsd_mode;
	uint8_t ufod_aid;
	uint8_t ufodl_hdr_aid;
	bool fd_mask;
};

struct b2r_test_cmd_id {
	enum b2r_test_dev_id  b2r_id;
	uint8_t  win_id;
};

struct b2r_test_global_timing {
	uint8_t win_id;
	bool enable;
};

struct b2r_test_wtmg_info {
	uint32_t vde;
	uint32_t hde;
	uint32_t fps;
	enum b2r_test_wtgen_clk_src clock_src;
};

struct b2r_test_slice_info {
	uint8_t slice_num;
	uint8_t slice_done_id;
};
//-----------------------------------------------------------------------------
//  Function and Variable
//-----------------------------------------------------------------------------
int PQU_B2R_Test_SetB2rInfo(struct b2r_test_cmd_id id, struct b2r_test_info *b2r_frame_info);
int PQU_B2R_Test_Fire(struct b2r_test_cmd_id id);
int PQU_B2R_Test_Force_RIU(struct b2r_test_cmd_id id, bool USE_RIU);
int PQU_B2R_Test_SetWTGen(struct b2r_test_global_timing global_win);
int PQU_B2R_Test_StoreWTmg(uint8_t win_id, struct b2r_test_wtmg_info win_timing);
int PQU_B2R_Test_Enable_MT(bool mt_en);
void PQU_B2R_Test_Set_Version(struct m_pq_common_stream_info *stream_info, uint8_t win);
int PQU_B2R_Test_pattern_set_info(struct pq_pattern_info *pattern_info);
void PQU_B2R_Test_Set_SliceInfo(struct b2r_test_slice_info *slice_info);
void PQU_B2R_Test_Get_SliceInfo(struct b2r_test_slice_info *slice_info);
void PQU_B2R_Test_Set_VFlip(struct b2r_test_cmd_id test_id, bool enable_vflip);
void PQU_B2R_Test_Stream_On(uint8_t win);
void PQU_B2R_Test_Stream_Off(void);
#endif
