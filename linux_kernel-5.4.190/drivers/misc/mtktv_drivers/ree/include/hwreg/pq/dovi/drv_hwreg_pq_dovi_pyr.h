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

#ifndef _DRV_HWREG_PQ_DOVI_PYR_H_
#define _DRV_HWREG_PQ_DOVI_PYR_H_

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
struct reg_pq_dovi_pyr_pyramid {
	uint32_t frame_pitch;
	uint32_t addr;
	uint16_t width;
	uint16_t height;
};

struct reg_pq_dovi_pyr_w_param_in {
	bool    pyr_en;
	uint8_t pyr_bank_num;
	uint8_t pyr_rw_diff;
	struct reg_pq_dovi_pyr_pyramid pyramids[7];
};

struct reg_pq_dovi_pyr_w_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};

struct reg_pq_dovi_pyr_r_param_in {
	bool   pyr_en;
	struct reg_pq_dovi_pyr_pyramid pyramids[7];
};

struct reg_pq_dovi_pyr_r_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};

struct reg_pq_dovi_pyr_crop_param_in {
	int i;
};

struct reg_pq_dovi_pyr_crop_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};

struct reg_pq_dovi_pyr_sd_param_in {
	bool hsd_en;
	bool hsd_mode;
	uint32_t hsd_ratio;
	bool vsd_en;
	bool vsd_mode;
	uint32_t vsd_ratio;
	uint16_t vsd_ipm_fetch;
};

struct reg_pq_dovi_pyr_sd_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};

struct reg_pq_dovi_pyr_h_protect_param_in {
	bool en;
	uint16_t width;
	uint16_t height;
};

struct reg_pq_dovi_pyr_h_protect_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};

//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
int drv_hwreg_pq_dovi_pyr_set_write(
	struct reg_pq_dovi_pyr_w_param_in *param_in,
	struct reg_pq_dovi_pyr_w_param_out *param_out);

int drv_hwreg_pq_dovi_pyr_set_read(
	struct reg_pq_dovi_pyr_r_param_in *param_in,
	struct reg_pq_dovi_pyr_r_param_out *param_out);

int drv_hwreg_pq_dovi_pyr_set_crop(
	struct reg_pq_dovi_pyr_crop_param_in *param_in,
	struct reg_pq_dovi_pyr_crop_param_out *param_out);

int drv_hwreg_pq_dovi_pyr_set_scaling(
	struct reg_pq_dovi_pyr_sd_param_in *param_in,
	struct reg_pq_dovi_pyr_sd_param_out *param_out);

int drv_hwreg_pq_dovi_pyr_set_h_protect(
	struct reg_pq_dovi_pyr_h_protect_param_in *param_in,
	struct reg_pq_dovi_pyr_h_protect_param_out *param_out);

#endif // _DRV_HWREG_PQ_DOVI_PYR_H_
