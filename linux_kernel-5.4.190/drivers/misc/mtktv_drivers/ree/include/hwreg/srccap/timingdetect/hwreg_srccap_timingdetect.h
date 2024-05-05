/*
 *
 * Copyright (c) 2020-2024 MediaTek Inc.
 * All rights reserved.
 *
 * Unless otherwise stipulated in writing, any and all information contained
 * herein regardless in any format shall remain the sole proprietary of
 * MediaTek Inc. and be kept in strict confidence
 * ("MediaTek Confidential Information") by the recipient.
 * Any unauthorized act including without limitation unauthorized disclosure,
 * copying, use, reproduction, sale, distribution, modification, disassembling,
 * reverse engineering and compiling of the contents of MediaTek Confidential
 * Information is unlawful and strictly prohibited. MediaTek hereby reserves the
 * rights to any and all damages, losses, costs and expenses resulting therefrom.
 *
 */

/*
 *
 * @author Mediatek Inc.
 *
 */

#ifndef _HWREG_SRCCAP_TIMINGDETECT_H_
#define _HWREG_SRCCAP_TIMINGDETECT_H_

#include "hwreg_common.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
enum reg_srccap_timingdetect_source {
	REG_SRCCAP_TIMINGDETECT_SOURCE_ADCA = 0,
	REG_SRCCAP_TIMINGDETECT_SOURCE_ADCB,
	REG_SRCCAP_TIMINGDETECT_SOURCE_VD,
	REG_SRCCAP_TIMINGDETECT_SOURCE_HDMI,
	REG_SRCCAP_TIMINGDETECT_SOURCE_HDMI2,
	REG_SRCCAP_TIMINGDETECT_SOURCE_HDMI3,
	REG_SRCCAP_TIMINGDETECT_SOURCE_HDMI4,
};

enum reg_srccap_timingdetect_video_type {
	REG_SRCCAP_TIMINGDETECT_VIDEO_TYPE_ANALOG1 = 0,
	REG_SRCCAP_TIMINGDETECT_VIDEO_TYPE_ANALOG2,
	REG_SRCCAP_TIMINGDETECT_VIDEO_TYPE_ANALOG3,
	REG_SRCCAP_TIMINGDETECT_VIDEO_TYPE_DVI,
	REG_SRCCAP_TIMINGDETECT_VIDEO_TYPE_VIDEO,
	REG_SRCCAP_TIMINGDETECT_VIDEO_TYPE_RESERVED,
	REG_SRCCAP_TIMINGDETECT_VIDEO_TYPE_HDMI = 7,
};

enum reg_srccap_timingdetect_sync {
	REG_SRCCAP_TIMINGDETECT_SYNC_CSYNC = 0,		/* component sync */
	REG_SRCCAP_TIMINGDETECT_SYNC_SOG,		/* sync on green */
};

enum reg_srccap_timingdetect_sync_detection {
	REG_SRCCAP_TIMINGDETECT_SYNC_DETECTION_AUTO = 0,
	REG_SRCCAP_TIMINGDETECT_SYNC_DETECTION_SEPARATE_SYNC,	/* R, G, B, H sync, V sync */
	REG_SRCCAP_TIMINGDETECT_SYNC_DETECTION_COMPOSITE_SYNC,	/* R, G, B, H&V syncs */
	REG_SRCCAP_TIMINGDETECT_SYNC_DETECTION_SOG,		/* R, G with H&V syncs, B */
};

enum reg_srccap_timingdetect_video_port {
	REG_SRCCAP_TIMINGDETECT_PORT_EXT_8_10_BIT = 0,
	REG_SRCCAP_TIMINGDETECT_PORT_INT_VD_A,
	REG_SRCCAP_TIMINGDETECT_PORT_EXT_16_20_BIT,
	REG_SRCCAP_TIMINGDETECT_PORT_INT_VD_B,
};

enum reg_srccap_timingdetect_memory_control_method {
	REG_SRCCAP_TIMINGDETECT_METHOD_VSTART_WITH_2YLOCK = 0,
	REG_SRCCAP_TIMINGDETECT_METHOD_VSTART_WITH_YLOCK,
	REG_SRCCAP_TIMINGDETECT_METHOD_VSTART,
	REG_SRCCAP_TIMINGDETECT_METHOD_VEND,
};

enum reg_srccap_timingdetect_field_detection_mode {
	REG_SRCCAP_TIMINGDETECT_MODE_HSYNCS_OF_FIELD = 0,
	REG_SRCCAP_TIMINGDETECT_MODE_HSYNC_VSYNC_RELATIONSHIP,
};

enum reg_srccap_timingdetect_hv_loss_detection_mode {
	REG_SRCCAP_TIMINGDETECT_MODE_OVERFLOW = 0,
	REG_SRCCAP_TIMINGDETECT_MODE_OVERFLOW_AND_SYNCLOSS,
};

enum reg_srccap_timingdetect_vrr_type {
	REG_SRCCAP_TIMINGDETECT_VRR_TYPE_NVRR = 0,
	REG_SRCCAP_TIMINGDETECT_VRR_TYPE_VRR,
	REG_SRCCAP_TIMINGDETECT_VRR_TYPE_CVRR = 3,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */
struct reg_srccap_timingdetect_window {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
};

struct reg_srccap_timingdetect_auto_position {
	bool en;
	uint16_t valid_data_threshold;
};

struct reg_srccap_timingdetect_auto_range {
	bool en;
	struct reg_srccap_timingdetect_window win;
};

struct reg_srccap_timingdetect_user_scantype {
	bool en;
	bool interlaced;
};

struct reg_srccap_timingdetect_user_hsync_polarity {
	bool en;
	bool high;
};

struct reg_srccap_timingdetect_user_vsync_polarity {
	bool en;
	bool high;
};

struct reg_srccap_timingdetect_watchdog {
	uint16_t h_pulse_sel;
	uint16_t v_pulse_sel;
	bool en;
};

struct reg_srccap_timingdetect_de_info {
	uint16_t vstart;
	uint16_t hstart;
	uint16_t vend;
	uint16_t hend;
};

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
int drv_hwreg_srccap_timingdetect_set_source(
	enum reg_srccap_timingdetect_source src,
	enum reg_srccap_timingdetect_video_type type,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_sync(
	enum reg_srccap_timingdetect_source src,
	enum reg_srccap_timingdetect_sync sync,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_sync_detection(
	enum reg_srccap_timingdetect_source src,
	enum reg_srccap_timingdetect_sync_detection sync_detection,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_video_port(
	enum reg_srccap_timingdetect_source src,
	enum reg_srccap_timingdetect_video_port video_port,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_de_only_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_hv_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_de_bypass_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_dot_based_hsync(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_adc_source_sel(
	enum reg_srccap_timingdetect_source src,
	bool component,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_coast_polarity(
	enum reg_srccap_timingdetect_source src,
	bool high,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_front_coast(
	enum reg_srccap_timingdetect_source src,
	uint8_t coast,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_back_coast(
	enum reg_srccap_timingdetect_source src,
	uint8_t coast,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_2p_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_8p_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_capture_win(
	enum reg_srccap_timingdetect_source src,
	struct reg_srccap_timingdetect_window win,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_interlaced_auto_switch_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_auto_position(
	enum reg_srccap_timingdetect_source src,
	struct reg_srccap_timingdetect_auto_position auto_pos,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_auto_range(
	enum reg_srccap_timingdetect_source src,
	struct reg_srccap_timingdetect_auto_range auto_range,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_ans_mute(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_ans_function(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_ans_vtt_det(
	enum reg_srccap_timingdetect_source src,
	bool disable,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_main_det_mute_en(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_hdmi_det_mute_en(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_hdmi_vmute_det_en(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_1080i_detection_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_memory_control_method(
	enum reg_srccap_timingdetect_source src,
	enum reg_srccap_timingdetect_memory_control_method method,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_ylock(
	enum reg_srccap_timingdetect_source src,
	uint8_t val,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_delayline(
	enum reg_srccap_timingdetect_source src,
	uint8_t val,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_user_scantype(
	enum reg_srccap_timingdetect_source src,
	struct reg_srccap_timingdetect_user_scantype scantype,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_user_hsync_polarity(
	enum reg_srccap_timingdetect_source src,
	struct reg_srccap_timingdetect_user_hsync_polarity polarity,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_user_vsync_polarity(
	enum reg_srccap_timingdetect_source src,
	struct reg_srccap_timingdetect_user_vsync_polarity polarity,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_i_field_detection_mode(
	enum reg_srccap_timingdetect_source src,
	enum reg_srccap_timingdetect_field_detection_mode mode,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_external_field_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_hv_loss_detection_mode(
	enum reg_srccap_timingdetect_source src,
	enum reg_srccap_timingdetect_hv_loss_detection_mode mode,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_watchdog(
	enum reg_srccap_timingdetect_source src,
	struct reg_srccap_timingdetect_watchdog dog,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_htt_report_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_hsync_ref_edge(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_vsync_delay_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_direct_de_mode(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_vdo_vsync_ref_edge(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_input_field_invert(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_external_field_en(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_output_field_invert(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_8p(
	bool enable_8p,
	enum reg_srccap_timingdetect_source src,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_set_hw_version(
       uint32_t version,
       bool stub);
int drv_hwreg_srccap_timingdetect_reset(
	enum reg_srccap_timingdetect_source src,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_timingdetect_get_de_info(
	enum reg_srccap_timingdetect_source src,
	struct reg_srccap_timingdetect_de_info *info);
int drv_hwreg_srccap_timingdetect_get_vsync_polarity(
	enum reg_srccap_timingdetect_source src,
	bool *high);
int drv_hwreg_srccap_timingdetect_get_hsync_polarity(
	enum reg_srccap_timingdetect_source src,
	bool *high);
int drv_hwreg_srccap_timingdetect_get_field_type(
	enum reg_srccap_timingdetect_source src,
	uint8_t *field);
int drv_hwreg_srccap_timingdetect_get_interlace_status(
	enum reg_srccap_timingdetect_source src,
	bool *interlaced);
int drv_hwreg_srccap_timingdetect_get_vtotal(
	enum reg_srccap_timingdetect_source src,
	uint16_t *vtt);
int drv_hwreg_srccap_timingdetect_get_hperiod(
	enum reg_srccap_timingdetect_source src,
	uint16_t *hpd);
int drv_hwreg_srccap_timingdetect_get_vsync2vsync_pix_cnt(
	enum reg_srccap_timingdetect_source src,
	uint32_t *pix_cnt);
int drv_hwreg_srccap_timingdetect_get_htt(
	enum reg_srccap_timingdetect_source src,
	uint16_t *htt);
int drv_hwreg_srccap_timingdetect_get_vrr(
	enum reg_srccap_timingdetect_source src,
	enum reg_srccap_timingdetect_vrr_type *vrr);
int drv_hwreg_srccap_timingdetect_get_frl_mode(
	enum reg_srccap_timingdetect_source src,
	bool *frl_md);
int drv_hwreg_srccap_timingdetect_get_yuv420(
	enum reg_srccap_timingdetect_source src,
	bool *yuv420);
#endif // _HWREG_SRCCAP_TIMINGDETECT_H_

