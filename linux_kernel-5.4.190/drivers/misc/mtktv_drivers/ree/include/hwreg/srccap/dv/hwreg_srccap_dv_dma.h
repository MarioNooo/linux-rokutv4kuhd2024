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

#ifndef _HWREG_SRCCAP_DV_DMA_H_
#define _HWREG_SRCCAP_DV_DMA_H_

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------
#include "hwreg_common.h"

//-----------------------------------------------------------------------------
// Driver Capability
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macros and Defines
//-----------------------------------------------------------------------------
#define REG_SRCCAP_DV_PLANE_NUM (3)

//-----------------------------------------------------------------------------
// Enums and Structures
//-----------------------------------------------------------------------------
enum reg_srccap_dv_dma_format {
	REG_SRCCAP_DV_DMA_FORMAT_YUV420 = 0,
	REG_SRCCAP_DV_DMA_FORMAT_YUV422 = 2,
	REG_SRCCAP_DV_DMA_FORMAT_YUV444 = 4,
	REG_SRCCAP_DV_DMA_FORMAT_RGB    = 11,
	REG_SRCCAP_DV_DMA_FORMAT_ARGB   = 12,
	REG_SRCCAP_DV_DMA_FORMAT_ABGR   = 13,
	REG_SRCCAP_DV_DMA_FORMAT_RGBA   = 14,
	REG_SRCCAP_DV_DMA_FORMAT_BGRA   = 15,
};

struct reg_srccap_dv_dma_format_conversion {
	bool en;
	enum reg_srccap_dv_dma_format ip_fmt;
	enum reg_srccap_dv_dma_format dvdma_fmt;
};

struct reg_srccap_dv_dma_resolution {
	uint32_t hsize;
	uint32_t vsize;
};

struct reg_srccap_dv_dma_force_w_setting {
	uint8_t index;
	uint8_t mode;
	bool en;
};

//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
int drv_hwreg_srccap_dv_dma_set_format(
	uint8_t dev_id,
	struct reg_srccap_dv_dma_format_conversion fmt_conv,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_pack(
	uint8_t dev_id,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_argb(
	uint8_t dev_id,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_bit_mode(
	uint8_t dev_id,
	uint8_t mode[REG_SRCCAP_DV_PLANE_NUM],
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_compression(
	uint8_t dev_id,
	uint8_t ce[REG_SRCCAP_DV_PLANE_NUM],
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_input_resolution(
	uint8_t dev_id,
	struct reg_srccap_dv_dma_resolution res,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_line_offset(
	uint8_t dev_id,
	uint32_t offset,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_frame_pitch(
	uint8_t dev_id,
	uint32_t pitch[REG_SRCCAP_DV_PLANE_NUM],
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_frame_num(
	uint8_t dev_id,
	uint8_t num,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_rw_diff(
	uint8_t dev_id,
	uint8_t diff,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_base_addr(
	uint8_t dev_id,
	uint64_t addr[REG_SRCCAP_DV_PLANE_NUM],
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_set_access(
	uint8_t dev_id,
	uint8_t mode,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_dma_get_w_index(
	uint8_t dev_id,
	uint8_t *index);

int drv_hwreg_srccap_dv_dma_get_r_index(
	uint8_t dev_id,
	uint8_t *index);

int drv_hwreg_srccap_dv_dma_force_w_index(
	uint8_t dev_id,
	struct reg_srccap_dv_dma_force_w_setting *setting,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

#endif // _HWREG_SRCCAP_DV_DMA_H_
