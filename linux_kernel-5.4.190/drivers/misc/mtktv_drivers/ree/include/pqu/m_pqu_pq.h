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

#ifndef _M_PQU_PQ_H_
#define _M_PQU_PQ_H_

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
#define M_PQ_DISPLAY_MDW_CTRL_META_TAG	"m_pq_display_mdw_ctrl"
#define M_PQ_DISPLAY_IDR_CTRL_META_TAG	"m_pq_display_idr_ctrl"
#define M_PQ_DISPLAY_B2R_CTRL_META_TAG	"m_pq_display_b2r_ctrl"
#define M_PQ_COMMON_STREAM_INFO_TAG	"m_pq_common_stream_info"
#define M_PQ_COMMON_DEBUG_INFO_TAG	"m_pq_common_debug_info"
#define M_PQ_COMMON_QBUF_INFO_TAG	"m_pq_common_input_info"
#define M_PQ_DISPLAY_FLOW_CTRL_META_TAG "m_pq_display_flow_ctrl"
#define M_PQ_PQMAP_SETTING_META_TAG "m_pq_pqmap_rule_settings"
#define M_PQ_QUEUE_EXT_INFO_TAG		"m_pq_queue_ext_info"
#define M_PQ_DISPLAY_NON_LINEAR_SCALING_TAG	"m_disp_non_linear_scaling"
#define M_PQ_DV_HDMI_INFO_TAG		"srccap_dv_hdmi_info"
#define M_PQ_DV_DEBUG_INFO_TAG		"m_pq_dv_debug_info"
#define M_PQ_PQPARAM_TAG			"m_pq_pqparam_pqu_info"
#define M_PQ_DISPLAY_WM_META_TAG	"m_pq_display_wm"
#define M_PQ_BBD_INFO_TAG		"m_pq_bbd_info"
#define M_PQ_DV_INFO_TAG            "m_pq_dv_info"
#define M_PQ_DV_OUTPUT_FRAME_INFO_TAG	"m_pq_dv_output_frame_info"
#define M_PQ_DV_CORE_PARAM_TAG		"m_pq_dv_core_parameter"
#define M_PQ_DISPLAY_FRC_SCALING_TAG	"m_pq_disp_frc_scaling"
#define M_PQ_THERMAL_INFO_TAG		"m_pq_thermal_info"

#define fourcc_code(a, b, c, d)	((__u32)(a) | ((__u32)(b) << 8) | \
				((__u32)(c) << 16) | ((__u32)(d) << 24))

/* ============================================================================================== */
/*   RGB fourcc character definition                                                              */
/*                                                                                                */
/*   x y _ _  , xy = AR(ARGB)   AB(ABGR)    RA(RGBA)    BA(BGRA)    RG(RGB)    BG(BGR)            */
/*                   ar(AGB CE) ab(ABGR CE) ra(RGBA CE) ba(BGRA CE) rg(RGB CE) bg(BGR CE)         */
/*                   AR(ARGB)   AB(ABGR)    RA(RGBA)    BA(BGRA)                                  */
/*                   ar(AGB CE) ab(ABGR CE) ra(RGBA CE) ba(BGRA CE)                               */
/*                                                                                                */
/*   _ _ z _  , z = 6(565) 8(888/8888) A(101010/2101010) C(121212)                                */
/*                                                                                                */
/*   _ _ _ w  , w = P(planar) ' '(none)                                                           */
/* ============================================================================================== */

/* 3 plane contiguous RGB */
#define MEM_FMT_RGB_8_8_8_CE		fourcc_code('r', 'g', '8', 'P')

/* 1 plane RGB */
#define MEM_FMT_RGB565			fourcc_code('R', 'G', '6', ' ')
#define MEM_FMT_RGB888			fourcc_code('R', 'G', '8', ' ')
#define MEM_FMT_RGB101010		fourcc_code('R', 'G', 'A', ' ')
#define MEM_FMT_RGB121212		fourcc_code('R', 'G', 'C', ' ')

#define MEM_FMT_ARGB8888		fourcc_code('A', 'R', '8', ' ')
#define MEM_FMT_ABGR8888		fourcc_code('A', 'B', '8', ' ')
#define MEM_FMT_RGBA8888		fourcc_code('R', 'A', '8', ' ')
#define MEM_FMT_BGRA8888		fourcc_code('B', 'A', '8', ' ')

#define MEM_FMT_ARGB2101010		fourcc_code('A', 'R', 'A', ' ')
#define MEM_FMT_ABGR2101010		fourcc_code('A', 'B', 'A', ' ')
#define MEM_FMT_RGBA1010102		fourcc_code('R', 'A', 'A', ' ')
#define MEM_FMT_BGRA1010102		fourcc_code('B', 'A', 'A', ' ')

/*MEMC*/
#define MEM_FMT_RGB444_6B		fourcc_code('R', 'G', '4', 'C')
#define MEM_FMT_RGB444_8B		fourcc_code('R', 'G', '4', 'D')

/* ============================================================================================== */
/*   YUV fourcc character definition                                                              */
/*                                                                                                */
/*   a _ _ _  , a = Y(YUV) U(UVY) V(VYU) y(YVU) u(UYV) v(VUY)                                     */
/*                                                                                                */
/*   _ b _ _  , b = 4(444) 2(422) 0(420)                                                          */
/*                                                                                                */
/*   _ _ c _  , c = 6(YC6bit) 8(YC8bit) A(YC10bit) C(YC12bit) %(Y8bitC6bit)...                    */
/*                                                                                                */
/*   _ _ _ d  , d = C(compressed) P(planar) &(compressed & planar) ' '(none)                      */
/* ============================================================================================== */

/* packed YUV */
#define MEM_FMT_YUV444_VYU_10B		fourcc_code('V', '4', 'A', ' ')
#define MEM_FMT_YUV444_VYU_8B		fourcc_code('V', '4', '8', ' ')

#define MEM_FMT_YUV422_YVYU_8B		fourcc_code('Y', 'V', 'Y', 'U')	//YVYU


/* 2 plane YUV */
#define MEM_FMT_YUV422_Y_UV_12B		fourcc_code('Y', '2', 'C', 'P')
#define MEM_FMT_YUV422_Y_UV_10B		fourcc_code('Y', '2', 'A', 'P')
#define MEM_FMT_YUV422_Y_VU_10B		fourcc_code('y', '2', 'A', 'P')
#define MEM_FMT_YUV422_Y_UV_8B		fourcc_code('Y', '2', '8', 'P')	//NV16
#define MEM_FMT_YUV422_Y_VU_8B		fourcc_code('y', '2', '8', 'P')	//NV61
#define MEM_FMT_YUV422_Y_UV_8B_CE	fourcc_code('Y', '2', '8', '&')
#define MEM_FMT_YUV422_Y_VU_8B_CE	fourcc_code('y', '2', '8', '&')
#define MEM_FMT_YUV422_Y_UV_8B_6B_CE	fourcc_code('Y', '2', '%', '&')
#define MEM_FMT_YUV422_Y_UV_6B_CE	fourcc_code('Y', '2', '6', '&')
#define MEM_FMT_YUV422_Y_VU_6B_CE	fourcc_code('y', '2', '6', '&')

#define MEM_FMT_YUV420_Y_UV_10B		fourcc_code('Y', '0', 'A', 'P')
#define MEM_FMT_YUV420_Y_VU_10B		fourcc_code('y', '0', 'A', 'P')
#define MEM_FMT_YUV420_Y_UV_8B		fourcc_code('Y', '0', '8', 'P')	//NV12
#define MEM_FMT_YUV420_Y_VU_8B		fourcc_code('y', '0', '8', 'P')	//NV21
#define MEM_FMT_YUV420_Y_UV_8B_CE	fourcc_code('Y', '0', '8', '&')
#define MEM_FMT_YUV420_Y_VU_8B_CE	fourcc_code('y', '0', '8', '&')
#define MEM_FMT_YUV420_Y_UV_8B_6B_CE	fourcc_code('Y', '0', '%', '&')
#define MEM_FMT_YUV420_Y_UV_6B_CE	fourcc_code('Y', '0', '6', '&')
#define MEM_FMT_YUV420_Y_VU_6B_CE	fourcc_code('y', '0', '6', '&')

/* 3 plane YUV */
#define MEM_FMT_YUV444_Y_U_V_8B_CE	fourcc_code('Y', '4', '8', '&')

/*MEMC*/
#define MEM_FMT_YUV444_YUV_6B		fourcc_code('Y', '4', '6', 'C')
#define MEM_FMT_YUV444_YUV_8B		fourcc_code('Y', '4', '8', 'D')
#define MEM_FMT_YUV422_YUV_6B		fourcc_code('Y', '2', '6', 'C')
#define MEM_FMT_YUV422_YUV_8B		fourcc_code('Y', '2', '8', 'D')
#define MEM_FMT_YUV420_YUV_6B		fourcc_code('Y', '0', '6', 'C')
#define MEM_FMT_YUV420_YUV_8B		fourcc_code('Y', '0', '8', 'D')

//-----------------------------------------------------------------------------
//  Enum
//-----------------------------------------------------------------------------
enum pqu_idr_input_path {
	PQU_PATH_IPDMA_0 = 0,
	PQU_PATH_IPDMA_1,
};

enum pqu_source {
	pqu_source_ipdma = 0,
	pqu_source_b2r,
	pqu_source_max,
};

enum pqu_mode {
	pqu_mode_ts = 0,
	pqu_mode_legacy,
	pqu_mode_max,
};
enum pqu_scene {
	pqu_scene_normal = 0,
	pqu_scene_game,
	pqu_scene_pc,
	pqu_scene_memc,
	pqu_scene_pip,
	pqu_scene_max,
};

enum pqu_framerate {
	pqu_framerate_24 = 0,
	pqu_framerate_30,
	pqu_framerate_50,
	pqu_framerate_60,
	pqu_framerate_120,
	pqu_framerate_max,
};
enum pqu_quality {
	pqu_quality_full = 0,
	pqu_quality_lite,
	pqu_quality_zfd,
	pqu_quality_max,
};
enum pqu_colorformat {
	pqu_colorformat_rgb = 0,
	pqu_colorformat_yuv,
	pqu_colorformat_max,
};

enum pqu_buffer_type {
	PQU_BUF_SCMI = 0,
	PQU_BUF_ZNR,
	PQU_BUF_UCM,
	PQU_BUF_ABF,
	PQU_BUF_MCDI,
	PQU_BUF_MAX,
};

enum pqu_fmt_modifier_vsd {
	PQU_VSD_NONE = 0,
	PQU_VSD_8x2 = 1,
	PQU_VSD_8x4 = 2,
};

enum pqu_buffer_channel {
	PQU_BUF_CH_0 = 0,	/* R / Y */
	PQU_BUF_CH_1,		/* G / CB */
	PQU_BUF_CH_2,		/* B / CR / Motion */
	PQU_BUF_CH_MAX,
};

enum pqu_ip_window {
	/* control size */
	pqu_ip_capture = 0,
	pqu_ip_ip2_in,
	pqu_ip_ip2_out,
	pqu_ip_scmi_in,
	pqu_ip_scmi_out,
	pqu_ip_aisr_in,
	pqu_ip_aisr_out,
	pqu_ip_hvsp_in,
	pqu_ip_hvsp_out,
	pqu_ip_display,

	/* size fixed */
	pqu_ip_hdr,
	pqu_ip_ucm,
	pqu_ip_abf,
	pqu_ip_1st_vip,
    pqu_ip_vip,
    pqu_ip_film_ip,
    pqu_ip_film_op,
    pqu_ip_spf,
	pqu_ip_yhsl,
	pqu_ip_ylite,
	pqu_ip_freq_det,
	pqu_ip_3d_meter,
	pqu_ip_obj_hist,

	pqu_ip_max,
};

enum pqu_dip_cap_point {
	pqu_dip_cap_pqin = 0,
	pqu_dip_cap_iphdr,
	pqu_dip_cap_prespf,
	pqu_dip_cap_hvsp,
	pqu_dip_cap_srs,
	pqu_dip_cap_vip,
	pqu_dip_cap_mdw,
	pqu_dip_cap_max,
};

enum pqu_order_type {
	pqu_order_type_bottom = 0,
	pqu_order_type_top,
	pqu_order_type_max,
};

enum pqu_scan_type {
	pqu_scan_type_progressive = 0,
	pqu_scan_type_interlace_frame,
	pqu_scan_type_interlace_field,
	pqu_scan_type_max,
};

enum pqu_field_type {
	pqu_field_type_none,
	pqu_field_type_top,
	pqu_field_type_bottom,
	pqu_field_type_both,
	pqu_field_type_max,
};

enum pqu_rotate_type {
	pqu_rotate_type_0 = 0,
	pqu_rotate_type_90,
	pqu_rotate_type_180,
	pqu_rotate_type_270,
	pqu_rotate_type_max,
};

enum pqu_pq_output_path {
	PQ_OUTPUT_PATH_WITHOUT_BUF = 0,
	PQ_OUTPUT_PATH_WITH_BUF_MDW,
	PQ_OUTPUT_PATH_WITH_BUF_FRC,
	PQ_OUTPUT_PATH_WITH_MAX,
};

enum m_pqu_pqmap_type {
	M_PQU_PQMAP_MAIN,
	M_PQU_PQMAP_MAIN_EX,
	M_PQU_PQMAP_MAX,
};

enum pqu_dv_dma_status {
	PQU_DV_DMA_STATUS_DISABLE = 0,
	PQU_DV_DMA_STATUS_ENABLE_FB,
	PQU_DV_DMA_STATUS_ENABLE_FBL,
	PQU_DV_DMA_STATUS_MAX
};

enum pqu_dv_interface {
	PQU_DV_INTERFACE_NONE = 0,
	PQU_DV_INTERFACE_SINK_LED,
	PQU_DV_INTERFACE_SOURCE_LED_RGB,
	PQU_DV_INTERFACE_SOURCE_LED_YUV,
	PQU_DV_INTERFACE_DRM_SOURCE_LED_RGB,
	PQU_DV_INTERFACE_DRM_SOURCE_LED_YUV,
	PQU_DV_INTERFACE_FORM_1,
	PQU_DV_INTERFACE_FORM_2_RGB,
	PQU_DV_INTERFACE_FORM_2_YUV,
	PQU_DV_INTERFACE_MAX
};

//this struct should be same as "enum mtk_pq_input_mode" defined in mtk-v4l2-pq.h
enum pqu_idr_buf_ctrl_mode {
	PQU_IDR_BUF_CTRL_MODE_BYPASS,
	PQU_IDR_BUF_CTRL_MODE_SW,
	PQU_IDR_BUF_CTRL_MODE_HW,
	PQU_IDR_BUF_CTRL_MODE_MAX,
};

enum pqu_rgb_type {
	PQU_RGB_NONE,
	PQU_RGB_8888,
	PQU_RGB_2AAA,
};
//-----------------------------------------------------------------------------
//  Structure
//-----------------------------------------------------------------------------
struct buffer_info {
	uint32_t size[PQU_BUF_CH_MAX];
	uint64_t addr[PQU_BUF_CH_MAX]; // by HW
	uint8_t frame_num;
};

struct pqu_fmt_modifier {
	bool tile;
	bool raster;
	bool compress;
	bool jump;
	enum pqu_fmt_modifier_vsd vsd_mode;
	bool vsd_ce_mode;
};

struct window_info {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint16_t w_align;
	uint16_t h_align;
};

struct stream_b2r_info {
	uint32_t h_max_size;
	uint32_t v_max_size;
	uint32_t b2r_version;
};

struct dip_window_info {
	uint16_t width;
	uint16_t height;
	uint8_t color_fmt; /* refer EN_PQ_CFD_DATA_FORMAT */
	uint8_t p_engine;
};

struct m_pq_display_rect {
	uint32_t left;
	uint32_t top;
	uint32_t width;
	uint32_t height;
};

struct m_pq_display_idr_duplicate_out {
	bool user_config;
	bool duplicate;
};

struct m_pq_wm_config {
	bool bWm_en;
};

struct m_pq_wm_result {
	__u32 u32Version;
	__u32 u32Length;
	__u16 u16Wm_En;
	__u16 u16Wm_decoded[30];
	__u8 u8Wm_DetectStatus;
};

struct aisr_active_window_info {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
};

/* refer to struct meta_pq_dv_gd_info */
struct dv_gd_info{
	bool valid;
	uint16_t gd;
	uint16_t delay;
};


//-----------------------------------------------------------------------------
//  Function and Variable
//-----------------------------------------------------------------------------

/*
 * Version 1: Init structure
 */
#define M_PQ_DISPLAY_MDW_CTRL_VERSION (1)
#define MAX_PLANE_NUM	(4)
#define MAX_FPLANE_NUM	(MAX_PLANE_NUM - 1)	// exclude metadata plane
struct m_pq_display_mdw_ctrl {
	uint8_t plane_num;
	uint64_t plane_offset[3];	// by HW
	enum pqu_pq_output_path output_path;
	uint32_t mem_fmt;
	uint16_t width;
	uint16_t height;
	uint16_t line_offset;	// in pixel
	uint32_t frame_offset[MAX_FPLANE_NUM];	// in byte
	bool vflip;
	bool req_off;
	uint64_t vb;		// ptr of struct vb2_buffer, use in callback
	uint8_t bpw;		//byte per word
#if (1)//(MEMC_CONFIG == 1)
	/*FRC*/
	bool frc_efuse;
	bool frc_enable;
	uint64_t frc_meds_addr;
	uint32_t frc_ipm_offset;	// in byte
	uint32_t frc_meds_offset;	// in byte
	uint16_t frc_framecount;
	uint8_t frc_ipversion;      //ip version
	uint64_t frc_mvbff_addr;
	bool frc_aid_enable;
#endif
};

/*
 * Version 1: Init structure
 */
#define M_PQ_DISPLAY_IDR_CTRL_VERSION (1)
struct m_pq_display_idr_ctrl {
	bool sw_mode_supported;
	struct buffer_info fbuf[3];
	struct m_pq_display_rect crop;
	uint32_t mem_fmt;
	uint8_t yuv_mode;
	uint32_t width;
	uint32_t height;
	uint16_t index;		/* force read index */
	uint64_t vb;		// ptr of struct vb2_buffer, use in callback
	enum pqu_idr_input_path path;
	bool bypass_ipdma;
	bool v_flip;
	uint8_t aid;		// access id
	enum pqu_idr_buf_ctrl_mode buf_ctrl_mode;
	uint32_t frame_pitch;
	uint32_t line_offset;
	struct m_pq_display_idr_duplicate_out duplicate_out;
};

/*
 * Version 1: Init structure
 * Version 2: Add AISR support flag
 * Version 3: Add clock&engine phase info
 */
#define M_PQ_COMMON_STREAM_INFO_VERSION (3)
struct m_pq_common_stream_info {
	int input_source;
	uint32_t width;
	uint32_t height;
	bool interlace;
	bool adaptive_stream;
	bool low_latency;
	bool v_flip;
	bool stub;
	enum pqu_source source;
	enum pqu_mode mode;
	enum pqu_scene scene;
	enum pqu_framerate framerate;
	enum pqu_quality quality;
	enum pqu_colorformat colorformat;
	struct buffer_info pq_buf[PQU_BUF_MAX];
	uint32_t mem_fmt[PQU_BUF_MAX];
	uint8_t pixel_align;
	uint16_t h_max_size;
	uint16_t v_max_size;
	uint8_t ip_diff;	/* domain ip, set as 0*/
	uint8_t opf_diff;	/* diff between opf and ip*/
	uint8_t opb_diff;	/* diff between opb and ip*/
	uint8_t op2_diff;	/* diff between op2 and ip*/
	bool scmi_enable;
	bool ucm_enable;
	bool aisr_support;
	uint32_t ED_clock;
	uint32_t FN_clock;
	uint32_t FS_clock;
	uint32_t FD_clock;
	uint32_t NREnginePhase;
	uint32_t MDWEnginePhase;
	uint16_t znr_me_h_max_size;
	uint16_t znr_me_v_max_size;
	uint32_t phase_ip2;
	uint8_t idr_input_mode;
	enum pqu_idr_input_path idr_input_path;
	uint32_t aisr_version;
	uint32_t cfd_pq_version;
	uint32_t mdw_version;
	uint8_t scenario_idx;
	struct stream_b2r_info stream_b2r;
	bool idr_sw_mode_supported;
};


/*
 * Version 1: Init structure
 */
#define M_PQ_COMMON_DEBUG_INFO_VERSION (1)
struct m_pq_common_debug_info {
	bool cmdq_timeout;
};

/*
 * Version 1: Init structure
 * Version 2: Add AISR enable flag
 * Version 3: Add flow control & Htt/Vtt info
 */
#define M_PQ_DISPLAY_FLOW_CTRL_VERSION (3)
struct m_pq_display_flow_ctrl {
	struct window_info content;
	struct window_info cap_win;
	struct window_info crop_win;
	struct window_info disp_win;
	struct window_info displayArea;
	struct window_info displayRange;
	bool aisr_en;

	/* output window info */
	uint8_t win_id;
	struct window_info ip_win[pqu_ip_max];
	struct dip_window_info dip_win[pqu_dip_cap_max];
	struct m_pq_wm_config wm_config;
	bool flowControlEn;
	uint16_t flowControlFactor;
	uint16_t InputHTT;
	uint16_t InputVTT;

	/* aisr active win info*/
	bool bAisrActiveWinEn;
	struct aisr_active_window_info aisr_active_win_info;
	struct window_info aisr_active_win;
};

/*
 * Version 1: Init structure
 * Version 2: Add b2r sub1 info
 */
#define M_PQ_DISPLAY_B2R_CTRL_VERSION (2)
struct m_pq_display_b2r_info {
	uint64_t luma_fb_offset;
	uint64_t chroma_fb_offset;
	uint64_t luma_blen_offset;
	uint64_t chroma_blen_offset;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint16_t bitdepth;
	uint16_t pitch;
	enum pqu_order_type order_type;
	enum pqu_scan_type scan_type;
	enum pqu_field_type field_type;
	enum pqu_rotate_type rotate_type;
	struct pqu_fmt_modifier modifier;
	uint8_t aid;
	uint8_t color_fmt_422;
	enum pqu_rgb_type rgb_type;
};

struct m_pq_display_b2r_ctrl {
	struct m_pq_display_b2r_info main;
	struct m_pq_display_b2r_info sub1;
};

#define M_PQ_QUEUE_EXT_INFO_VERSION (1)
struct m_pq_queue_ext_info {
	int32_t int32FrameId;
	__u64 u64Pts;
	__u64 u64UniIdx;
	__u64 u64ExtUniIdx;
	__u64 u64TimeStamp;
	__u64 u64RenderTimeNs;
	__u8 u8BufferValid;
	__u32 u32BufferSlot;
	__u32 u32GenerationIndex;
	__u8 u8RepeatStatus;
	__u8 u8FrcMode;
	__u8 u8Interlace;
	__u32 u32InputFps;
	__u32 u32OriginalInputFps;
	bool bEOS;
	__u8 u8MuteAction;
	__u8 u8SignalStable;
	__u8 u8DotByDotType;
	__u32 u32RefreshRate;
	__u32 u32QueueInputIndex;
	__u32 u32QueueOutputIndex;
	struct m_pq_wm_result wm_result;
};

#define M_PQ_BBD_INFO_VERSION (1)
struct m_pq_bbd_info {
	__u8 u8Validity;
	__u16 u16LeftOuterPos;
	__u16 u16RightOuterPos;
	__u16 u16LeftInnerPos;
	__u16 u16LeftInnerConf;
	__u16 u16RightInnerPos;
	__u16 u16RightInnerConf;
};

#define M_PQ_PQMAP_RULE_VERSION (1)
#define M_PQ_PQMAP_NODE_SIZE (1024)
struct m_pq_pqmap_rule_settings {
	uint16_t au16Nodes[M_PQ_PQMAP_NODE_SIZE];
	uint16_t au16NodesNum[M_PQU_PQMAP_MAX];
};

#define M_PQ_DISPLAY_NON_LINEAR_SCALING_VERSION (0)
struct m_pq_display_non_linear_scaling {
	__u8 u8HNL_En;
	__u16 u16SourceRatio;
	__u16 u16TargetRatio;
};

#define M_PQ_DV_OUTPUT_FRAME_INFO_VERSION (1)
/* refer to struct meta_pq_dv_l11_info */
struct m_pq_dv_l11_info {
	uint8_t u8WPValid;
	uint8_t u8ContentType;
	uint8_t u8WhitePoint;
	uint8_t u8L11Byte2;
	uint8_t u8L11Byte3;

	/* test */
	uint8_t u8Stub;

	/* result */
	uint8_t u8PQFrcLevel;
	uint8_t u8PQNrLevel;
	uint8_t u8PQSrLevel;
};

/* refer to struct meta_pq_dv_out_frame_info */
struct m_pq_dv_out_frame_info{
	struct dv_gd_info gd_info;
	__u32 dv_pq_hw_ver;
	struct m_pq_dv_l11_info l_11_Data;
};

#define M_PQ_DISPLAY_FRC_SCALING_VERSION (1)
struct m_pq_display_frc_scaling{
	int32_t ds_instance;
	uint8_t ds_index;
	uint32_t ds_depth; // DISP Domain Only
	struct window_info hvspout_win; //refer input meta data
};

#define M_PQ_DV_HDMI_INFO_VERSION (2)
struct m_pq_dv_hdmi_common {  //refer to meta_srccap_dv_common
	__u8 path;
	__u32 dv_src_hw_version;
	bool hdmi_422_pack_en;
};

struct m_pq_dv_hdmi_descrb {   //refer to meta_srccap_dv_descrb
	enum pqu_dv_interface interface;
};

struct m_pq_dv_hdmi_dma {  //refer to meta_srccap_dv_dma
	enum pqu_dv_dma_status status;
	__u64 addr[3];
	__u8  w_index;
	__u32 width;
	__u32 height;
};

struct m_pq_dv_hdmi_meta {  //refer to meta_srccap_dv_meta
	__u8  data[1024];
	__u32 data_length;
};

struct m_pq_dv_hdmi_info {  //refer to meta_srccap_dv_info
	__u16 version;
	struct m_pq_dv_hdmi_common common;
	struct m_pq_dv_hdmi_descrb descrb;
	struct m_pq_dv_hdmi_dma    dma;
	struct m_pq_dv_hdmi_meta   meta;
};

#define M_PQ_DV_DEBUG_INFO_VERSION    (1)
#define M_PQ_DV_DEBUG_MAX_SET_REG_NUM (20)
#define M_PQ_DV_DEBUG_MAX_PIC_MODE_NUM	(10)
#define M_PQ_DV_DEBUG_MAX_PIC_MODE_LEN	(32)
struct m_pq_dv_debug_level {
	bool valid;
	uint32_t debug_level_pqu;
	uint32_t debug_level_3pty;
};

struct m_pq_dv_debug_reg {
	uint32_t addr;
	uint32_t val;
	uint32_t mask;
};

struct m_pq_dv_debug_set_reg {
	bool valid;
	uint8_t reg_num;
	struct m_pq_dv_debug_reg regs[M_PQ_DV_DEBUG_MAX_SET_REG_NUM];
};

struct m_pq_dv_debug_get_picture_mode_info {
	uint8_t pic_mode_num;
	uint8_t pic_mode_name[M_PQ_DV_DEBUG_MAX_PIC_MODE_NUM][M_PQ_DV_DEBUG_MAX_PIC_MODE_LEN + 1];
	uint8_t cur_view_id;
};

struct m_pq_dv_debug_get_status {
	bool enable;
	struct m_pq_dv_debug_get_picture_mode_info pic_mode_info;
	__u16 result_adl_core1_size;
	__u16 result_adl_core2_size;
	__u16 result_ml_x16_count;
	__u16 result_ml_x32_count;
	//global dimming
	struct m_pq_dv_out_frame_info dv_out_frame_info;
	__u8  hdr_mode; //0:SDR 1:DV 2:HDR10 3:HLG 4:TCH 5:HDR10+
	bool pr_en;
};

struct m_pq_dv_debug_stub_mode {
	bool enable;
	enum pqu_source input_source;
};

/* refer to struct mtk_pq_dv_debug_force_view_mode */
struct m_pq_dv_debug_force_view_mode {
	bool en;
	uint8_t view_id;
};

/* refer to struct mtk_pq_dv_debug_force_disable_dolby */
struct m_pq_dv_debug_force_disable_dolby {
	bool en;
};

/* refer to struct mtk_pq_dv_debug_force_dark_detail */
struct m_pq_dv_debug_force_dark_detail {
	bool en;
	bool on;
};

/* refer to struct mtk_pq_dv_debug_force_global_dimming */
struct m_pq_dv_debug_force_global_dimming {
    bool en;
    bool on;
};

/* refer to struct mtk_pq_dv_debug_force_iq_apo */
struct m_pq_dv_debug_force_iq_apo {
	bool en;
	bool on;
};

/* refer to struct mtk_pq_dv_debug_force_white_point */
struct m_pq_dv_debug_force_white_point {
	bool en;
	uint8_t wp_val;
};

/* refer to struct mtk_pq_dv_debug_force_light_sense */
struct m_pq_dv_debug_force_light_sense {
	bool en;
	uint8_t mode;
	uint32_t lux_val;
};

/* refer to struct mtk_pq_dv_debug_force_control */
struct m_pq_dv_debug_force_control {
	struct m_pq_dv_debug_force_view_mode view_mode;
	struct m_pq_dv_debug_force_disable_dolby disable_dolby;
	struct m_pq_dv_debug_force_dark_detail dark_detail;
    struct m_pq_dv_debug_force_global_dimming global_dimming;
    struct m_pq_dv_debug_force_iq_apo iq_apo;
	struct m_pq_dv_debug_force_white_point white_point;
	struct m_pq_dv_debug_force_light_sense light_sense;
	bool idkenable;
};

struct m_pq_dv_debug {
	struct m_pq_dv_debug_level debug_level;
	struct m_pq_dv_debug_set_reg set_reg;
	struct m_pq_dv_debug_get_status get_status;

	struct m_pq_dv_debug_stub_mode stub_mode;

	struct m_pq_dv_debug_force_control force_ctrl;
};

#define M_PQ_PQPARAM_VERSION (0)
struct m_pq_pqparam_meta {
	__u8  data[1024*3];
};

#define M_PQ_DISPLAY_WM_VERSION (1)
struct m_pq_display_wm_info {
	struct m_pq_wm_config wm_config;
	struct m_pq_wm_result wm_result;
};

#define M_PQ_DV_INFO_VERSION (2)
#define M_PQ_DV_PYR_NUM      (7)
/* refer to struct meta_pq_dv_pyr */
struct m_pq_dv_pyr {
	bool  valid;
	__u8  frame_num;
	__u8  rw_diff;
	__u32 frame_pitch[M_PQ_DV_PYR_NUM];
	__u64 addr[M_PQ_DV_PYR_NUM];
	__u32 width[M_PQ_DV_PYR_NUM];
	__u32 height[M_PQ_DV_PYR_NUM];
};

/* refer to struct meta_pq_dv_pr_ctrl */
struct m_pq_dv_pr_ctrl {
	bool en;
	__u32 fe_in_width;  /* input width of front end */
	__u32 fe_in_height; /* input height of front end */
	__u32 c1_in_width;  /* input width of core1 */
	__u32 c1_in_height; /* input height of core1 */
};

/* refer to struct ST_DV_AMBIENT_INFO */
struct m_pq_dv_ambient {
	__u32 u32Version;
	__u16 u16Length;

	bool bIsModeValid;
	__u32 u32Mode;
	bool bIsFrontLuxValid;
	signed long long s64FrontLux;
	bool  bIsRearLumValid;
	signed long long s64RearLum;
	bool bIsWhiteXYValid;
	__u32 u32WhiteX;
	__u32 u32WhiteY;

	bool bIsLsEnabledValid;
	bool bLsEnabled;
};

/* refer to struct meta_pq_dv_info */
struct m_pq_dv_info {
	struct m_pq_dv_pyr pyr;
	struct m_pq_dv_pr_ctrl pr_ctrl;
	struct m_pq_dv_ambient ambient;
};



/* refer to M_PQ_DV_CORE_PARAM_TAG */
#define M_PQ_DV_CORE_PARAM_VERSION	(1)
#define M_PQ_DV_CORE_HW_HIST_SIZE	(256)

struct m_pq_dv_core_hw_hist_data {
	bool en;
	__u8 data[M_PQ_DV_CORE_HW_HIST_SIZE];
	__u32 size;
};

struct m_pq_dv_core_hw_l1l4_data {
	bool en;
	__u16 l1_min_val;
	__u16 l1_max_val;
	__u16 l1_mid_val;
	__u16 l4_std_val;
};

struct m_pq_dv_core_param_info {
	struct m_pq_dv_core_hw_hist_data hw_hist;
	struct m_pq_dv_core_hw_l1l4_data hw_l1l4;
};

#define M_PQ_THERMAL_INFO_VERSION (1)
/*
 * Version 1 : AISR
 */
struct m_pq_thermal {
	bool thermal_disable_aisr;
};

#endif
