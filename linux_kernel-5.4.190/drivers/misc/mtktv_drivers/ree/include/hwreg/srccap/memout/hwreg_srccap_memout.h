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

#ifndef _HWREG_SRCCAP_MEMOUT_H_
#define _HWREG_SRCCAP_MEMOUT_H_

#include "hwreg_common.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */
#define REG_SRCCAP_MEMOUT_PLANE_NUM (3)

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
enum reg_srccap_memout_format {
	REG_SRCCAP_MEMOUT_FORMAT_YUV420 = 0,
	REG_SRCCAP_MEMOUT_FORMAT_YUV422 = 2,
	REG_SRCCAP_MEMOUT_FORMAT_YUV444 = 4,
	REG_SRCCAP_MEMOUT_FORMAT_RGB = 11,
	REG_SRCCAP_MEMOUT_FORMAT_ARGB = 12,
	REG_SRCCAP_MEMOUT_FORMAT_ABGR = 13,
	REG_SRCCAP_MEMOUT_FORMAT_RGBA = 14,
	REG_SRCCAP_MEMOUT_FORMAT_BGRA = 15,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */
struct reg_srccap_memout_resolution {
	uint32_t hsize;
	uint32_t vsize;
};

struct reg_srccap_memout_format_conversion {
	bool en;
	enum reg_srccap_memout_format ip_fmt;
	enum reg_srccap_memout_format ipdma_fmt;
};

struct reg_srccap_memout_force_w_setting {
	uint8_t index;
	uint8_t mode;
	bool en;
};

struct reg_srccap_memout_deposit_w_setting {
	uint8_t index;
	uint8_t mode;
};

struct reg_srccap_riu_addr {
	uint32_t regbase;
	uint32_t regbase_ext;
	uint32_t bank_sc;
	uint32_t bank_sc_ext;
	uint32_t bank_mux;
};

struct reg_srccap_memout_sstinfo {
	bool ready;
	uint8_t secure_hdmi_0;
	uint8_t secure_hdmi_1;
	uint8_t secure_hdmi_2;
	uint8_t secure_hdmi_3;
};

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
int drv_hwreg_srccap_memout_set_input_resolution(
	uint8_t dev_id,
	struct reg_srccap_memout_resolution res,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_vflip(
	uint8_t dev_id,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_format(
	uint8_t dev_id,
	struct reg_srccap_memout_format_conversion fmt_conv,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_access(
	uint8_t dev_id,
	uint8_t mode,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_bit_mode(
	uint8_t dev_id,
	uint8_t mode[REG_SRCCAP_MEMOUT_PLANE_NUM],	/* 0:R/Y, 1:G/C, 2:B/M */
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_compression(
	uint8_t dev_id,
	uint8_t ce[REG_SRCCAP_MEMOUT_PLANE_NUM],	/* 0:R/Y, 1:G/C, 2:B/M */
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_pack(
	uint8_t dev_id,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_argb(
	uint8_t dev_id,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_base_addr(
	uint8_t dev_id,
	uint64_t addr[REG_SRCCAP_MEMOUT_PLANE_NUM],	/* 0:R/Y, 1:G/C, 2:B/M */
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_line_offset(
	uint8_t dev_id,
	uint32_t offset,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_frame_pitch(
	uint8_t dev_id,
	uint32_t pitch[REG_SRCCAP_MEMOUT_PLANE_NUM],	/* 0:R/Y, 1:G/C, 2:B/M */
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_frame_num(
	uint8_t dev_id,
	uint8_t num,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_rw_diff(
	uint8_t dev_id,
	uint8_t diff,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_force_write(
	uint8_t dev_id,
	struct reg_srccap_memout_force_w_setting setting,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_set_deposit_write(
	uint8_t dev_id,
	struct reg_srccap_memout_deposit_w_setting setting,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_get_access(
	uint8_t dev_id,
	uint8_t *mode);
int drv_hwreg_srccap_memout_get_w_index(
	uint8_t dev_id,
	uint8_t *index);
int drv_hwreg_srccap_memout_get_r_index(
	uint8_t dev_id,
	uint8_t *index);
void drv_hwreg_srccap_memout_riu_init(
	struct reg_srccap_riu_addr *riu_addr);
int drv_hwreg_srccap_memout_get_sst(
	struct reg_srccap_memout_sstinfo *sstinfo);
int drv_hwreg_srccap_memout_get_timestamp(
	uint8_t dev_id,
	uint32_t *timestamp);
int drv_hwreg_srccap_memout_set_ipdmaw_size(
	uint8_t dev_id,
	bool riu,
	uint32_t width,
	uint32_t height,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_memout_get_field(
	uint8_t dev_id,
	uint8_t index,
	uint8_t *field);
int drv_hwreg_srccap_memout_reset_ipdma(
	uint8_t dev_id,
	bool enable,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
#endif // _HWREG_SRCCAP_MEMOUT_H_
