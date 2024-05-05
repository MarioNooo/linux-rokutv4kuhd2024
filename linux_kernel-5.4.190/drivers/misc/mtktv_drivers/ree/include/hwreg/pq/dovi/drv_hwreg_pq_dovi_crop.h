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

#ifndef _DRV_HWREG_PQ_DOVI_CROP_H_
#define _DRV_HWREG_PQ_DOVI_CROP_H_

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

//-----------------------------------------------------------------------------
// Enums and Structures
//-----------------------------------------------------------------------------
struct reg_pq_dovi_crop_param_in {
	uint8_t  h_crop_en;
	uint16_t h_crop_start;
	uint16_t h_crop_size;
	uint8_t  v_crop_en;
	uint16_t v_crop_start;
	uint16_t v_crop_size;
	uint8_t  hv_insert_en;
	uint8_t  boundary_mode;
	uint16_t insert_color_r;
	uint16_t insert_color_g;
	uint16_t insert_color_b;
	uint16_t pre_h_insert_size;
	uint16_t h_output_size;
	uint16_t pre_v_insert_size;
	uint16_t v_output_size;
	uint32_t u32Hsize;
	uint32_t u32Vsize;
};

struct reg_pq_dovi_crop_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};

//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
int drv_hwreg_pq_dovi_set_hvsize(
	struct reg_pq_dovi_crop_param_in *param_in,
	struct reg_pq_dovi_crop_param_out *param_out);

int drv_hwreg_pq_dovi_set_pre_crop(
	struct reg_pq_dovi_crop_param_in *param_in,
	struct reg_pq_dovi_crop_param_out *param_out);

int drv_hwreg_pq_dovi_set_post_crop(
	struct reg_pq_dovi_crop_param_in *param_in,
	struct reg_pq_dovi_crop_param_out *param_out);

int drv_hwreg_pq_dovi_set_pyr_crop(
	struct reg_pq_dovi_crop_param_in *param_in,
	struct reg_pq_dovi_crop_param_out *param_out);

int drv_hwreg_pq_dovi_set_pyr2_crop(
	struct reg_pq_dovi_crop_param_in *param_in,
	struct reg_pq_dovi_crop_param_out *param_out);
#endif // _DRV_HWREG_PQ_DOVI_CROP_H_
