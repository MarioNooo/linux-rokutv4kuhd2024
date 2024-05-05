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

#ifndef _HWREG_PQ_DISPLAY_B2R_H_
#define _HWREG_PQ_DISPLAY_B2R_H_

#include "hwreg_common.h"

//------------------------------------------------------------------------------
//  Driver Capability
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Macro and Define
//------------------------------------------------------------------------------
#define REG_ADDR_NUM 64
//------------------------------------------------------------------------------
//  Type and Structure
//------------------------------------------------------------------------------
struct b2r_init_info {
	uint64_t regbase;
	uint32_t b2r_version;
};

struct b2r_timing_info {
	uint16_t V_TotalCount;
	uint32_t H_TotalCount;
	uint16_t VBlank0_Start;
	uint16_t VBlank0_End;
	uint16_t VBlank1_Start;
	uint16_t VBlank1_End;
	uint16_t TopField_Start;
	uint16_t BottomField_Start;
	uint16_t TopField_VS;
	uint16_t BottomField_VS;
	uint16_t HActive_Start;
	uint16_t HImg_Start;
	uint16_t VImg_Start0;
	uint16_t VImg_Start1;
	uint32_t Hcnt_FineTune;
	uint64_t ClockFreq;
};

struct b2r_reg_fmt_info {
	struct {
		uint32_t yuv422:1;
		uint32_t rgb:3;
		uint32_t compress:1;
		uint32_t bit_depth:3;
		uint32_t lsb_raster:1;
		uint32_t comp_jump:1;
		uint32_t vsd:3;
		uint32_t tile_16x32:1;
		uint32_t vsd_ce_mode:1;
		uint32_t auo_mode:1;
		uint32_t nv12:1;
		uint32_t uvswap:1;
		uint32_t dummy:13;
	} fmt;
	struct {
		uint32_t pro_frame:3;
		uint32_t field:1;
		uint32_t dummy:27;
	} scantype;
};

struct b2r_reg_size_info {
	uint16_t ufo_x;
	uint16_t ufo_y;
	uint16_t ufo_width;
	uint16_t ufo_height;
	uint16_t ufo_pitch;
	uint16_t b2r_width;
	uint16_t b2r_height;
	uint16_t b2r_xpre;
	uint16_t b2r_xpost;
	uint16_t blen_pitch;
};

struct b2r_reg_addr_info {
	uint32_t luma_fb;
	uint32_t chroma_fb;
	uint32_t luma_bln_fb;
	uint32_t chroma_bln_fb;
	uint16_t luma_offset;
	uint16_t chroma_offset;
	uint16_t bitlen_offset;
};

struct b2r_crc_info {
	uint64_t crc_x;
	uint64_t crc_y;
	uint64_t crc_m;
	uint64_t crc_o;
};

struct b2r_reg_aid_info {
	uint8_t ufod_aid;
	uint8_t ufodl_hdr_aid;
};

struct b2r_reg_ctrl_info {
	uint8_t fd_mask;
	struct {
		uint8_t rot_en:1;
		uint8_t h_mir:1;
		uint8_t v_mir:1;
        uint8_t dummy:5;
	} rot;
};

enum b2r_pattern {
	B2R_PAT_None,
	B2R_PAT_Colorbar,
	B2R_PAT_Purecolor,
};

struct b2r_reg_patyuv {
	uint8_t pat_y;
	uint8_t pat_u;
	uint8_t pat_v;
};

struct b2r_reg_dmapat_info {
	enum b2r_pattern type;
	struct b2r_reg_patyuv yuv_value;
	bool bNeedSet;
};

struct b2r_reg_isr_info {
	union {
		struct {
			uint8_t reg_uf_mask:1;
			uint8_t reg_of_mask:1;
			uint8_t reg_vs_mask:1;
			uint8_t reg_hs_mask:1;
			uint8_t reg_rdy_mask:1;
			uint8_t reg_fld_mask:1;
			uint8_t reg_dummy:1;
			uint8_t reg_mvd_vs:1;
		};
		uint8_t val;
	} isr_1f;
	union {
		struct {
			uint8_t reg_of_lsb_mask:1;
			uint8_t reg_uf_lsb_mask:1;
			uint8_t reg_in_vsize_mask:1;
			uint8_t reg_dc_str_mask:1;
			uint8_t reg_dummy:4;
		};
		uint8_t val;
	} isr_3a;
};

enum b2r_bitdepth {
	B2R_BD_8Bit,
	B2R_BD_10Bit,
};

enum b2r_vsd {
	B2R_VSD_NONE,
	B2R_VSD_8x4,
	B2R_VSD_8x2,
};

enum b2r_int_type {
	B2R_INT_PRO,
	B2R_INT_Frame,
	B2R_INT_Field,
};

enum b2r_rgb_type {
	B2R_RGB_NONE,
	B2R_RGB_888,
	B2R_RGB_2AAA,
};

enum b2r_reg_dev {
	B2R_REG_DEV0,
	B2R_REG_DEV1,
	B2R_REG_DEV2,
};

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
void drv_hwreg_pq_display_b2r_init(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id,
	struct b2r_init_info init_info);
void drv_hwreg_pq_display_b2r_set_timing(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id,
	struct b2r_timing_info timing_info);
void drv_hwreg_pq_display_b2r_set_pattern(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id,
	struct b2r_reg_dmapat_info b2r_pat);
void drv_hwreg_pq_display_b2r_set_fmt(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id,
	struct b2r_reg_fmt_info format_info);
void drv_hwreg_pq_display_b2r_set_size(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id,
	struct b2r_reg_size_info size_info);
void drv_hwreg_pq_display_b2r_set_addr(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id,
	struct b2r_reg_addr_info addr_info);
void drv_hwreg_pq_display_b2r_get_fmt(bool bRIU,
	enum b2r_reg_dev dev_id,
	struct b2r_reg_fmt_info* format_info);
void drv_hwreg_pq_display_b2r_get_size(bool bRIU,
	enum b2r_reg_dev dev_id,
	struct b2r_reg_size_info* size_info);
void drv_hwreg_pq_display_b2r_get_dma_pat(bool bRIU,
	enum b2r_reg_dev dev_id,
	struct b2r_reg_dmapat_info* b2r_pat);
void drv_hwreg_pq_display_b2r_get_rotate_info(bool bRIU,
	enum b2r_reg_dev dev_id,
	struct b2r_reg_ctrl_info* b2r_ctrl);
void drv_hwreg_pq_display_b2r_enable_dma(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id,
	bool enable);
void drv_hwreg_pq_display_b2r_set_slt(enum b2r_reg_dev dev_id);
void drv_hwreg_pq_display_b2r_get_crc(struct b2r_crc_info* crc);
void drv_hwreg_pq_display_b2r_set_aid(bool bRIU,
	struct hwregOut* regouts,
	struct b2r_reg_aid_info aid_info);
void drv_hwreg_pq_display_b2r_set_fdmask(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id,
	uint8_t fd_mask);
void drv_hwreg_pq_display_b2r_set_rotate_info(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id,
	struct b2r_reg_ctrl_info ctrl_info);
void drv_hwreg_pq_display_b2r_set_wtgen(bool bRIU,
	struct hwregOut* regouts,
	struct b2r_timing_info timing_info);
void drv_hwreg_pq_display_b2r_get_wtgen(
	struct b2r_timing_info *timing_info);
void drv_hwreg_pq_display_b2r_set_version(
	uint32_t b2r_version);
void drv_hwreg_pq_display_b2r_set_isr(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id,
	struct b2r_reg_isr_info b2r_isr);
void drv_hwreg_pq_display_b2r_rst(bool bRIU,
	struct hwregOut* regouts,
	enum b2r_reg_dev dev_id);
#endif // _HWREG_PQ_DISPLAY_B2R_H_

