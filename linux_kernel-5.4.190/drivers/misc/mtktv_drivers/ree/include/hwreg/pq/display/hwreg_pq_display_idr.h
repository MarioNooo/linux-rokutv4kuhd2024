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

#ifndef _HWREG_PQ_DISPLAY_IDR_H_
#define _HWREG_PQ_DISPLAY_IDR_H_

#include "hwreg_common.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */
#define REG_PQ_DISPLAY_IDR_PLANE_NUM (3)
/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
enum reg_pq_display_idr_input_path {
	REG_PQ_DISPLAY_IDR_INPUT_PATH_IDW0 = 0,
	REG_PQ_DISPLAY_IDR_INPUT_PATH_IDW1,
};

enum reg_pq_display_ipdma_output {
	REG_PQ_DISPLAY_IPDMA_OUTPUT_OFF = 0,
	REG_PQ_DISPLAY_IPDMA_OUTPUT_IDR,
	REG_PQ_DISPLAY_IPDMA_OUTPUT_IDW0,
	REG_PQ_DISPLAY_IPDMA_OUTPUT_IDW1,
};

enum reg_pq_display_idr_format {
	REG_PQ_DISPLAY_IDR_FORMAT_YUV420 = 0,
	REG_PQ_DISPLAY_IDR_FORMAT_YUV422 = 2,
	REG_PQ_DISPLAY_IDR_FORMAT_YUV444 = 4,
	REG_PQ_DISPLAY_IDR_FORMAT_RGB = 11,
	REG_PQ_DISPLAY_IDR_FORMAT_ARGB = 12,
	REG_PQ_DISPLAY_IDR_FORMAT_ABGR = 13,
	REG_PQ_DISPLAY_IDR_FORMAT_RGBA = 14,
	REG_PQ_DISPLAY_IDR_FORMAT_BGRA = 15,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */
struct reg_pq_display_idr_format_conversion {
	bool fmt_en;
	enum reg_pq_display_idr_format fmt;
	enum reg_pq_display_idr_format ipdma_fmt;
};

struct reg_pq_display_idr_crop {
	uint32_t x;
	uint32_t y;
	uint32_t hsize;
	uint32_t vsize;
};

struct reg_pq_display_force_r_setting {
	uint8_t index;
	uint8_t mode;
	bool en;
};

struct reg_pq_display_deposit_r_setting {
	uint8_t index;
	bool mode;
};

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
int drv_hwreg_pq_display_idr_set_input_path(
	enum reg_pq_display_idr_input_path path,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_ipdma_output(
	enum reg_pq_display_ipdma_output out,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_idr_set_format(
	struct reg_pq_display_idr_format_conversion fmt_conv,
	bool riu,
	struct reg_info *reg,
	bool bypassMode,
	uint16_t *count);
int drv_hwreg_pq_display_idr_set_access(
	uint8_t mode,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_idr_set_crop(
	struct reg_pq_display_idr_crop crop,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_idr_set_vflip(
	bool v_flip,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_force_read(
	enum reg_pq_display_idr_input_path path,
	struct reg_pq_display_force_r_setting setting,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_deposit_read(
	enum reg_pq_display_idr_input_path path,
	struct reg_pq_display_deposit_r_setting setting,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_idr_aid(
	uint8_t aid,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_config_custom_enable(
	bool enable,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_ce_custom_enable(
	bool enable,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_dma_base_enable(
	bool enable,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_bit_mode(
	uint8_t mode[REG_PQ_DISPLAY_IDR_PLANE_NUM],
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_compression(
	uint8_t ce[REG_PQ_DISPLAY_IDR_PLANE_NUM],
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_pack(
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_argb(
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_base_addr(
	uint64_t addr,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_pq_display_set_line_offset(
	uint32_t offset,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_frame_pitch(
	uint32_t pitch,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_force_field_enable(
	bool enable,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_force_field_value(
	uint8_t value,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_duplicate_out_mode(
	bool sw_mode_supported,
	enum reg_pq_display_idr_input_path path,
	uint8_t mode,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_pq_display_set_duplicate_out_cfg_value(
	bool sw_mode_supported,
	enum reg_pq_display_idr_input_path path,
	uint8_t value,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
#endif // _HWREG_PQ_DISPLAY_IDR_H_
