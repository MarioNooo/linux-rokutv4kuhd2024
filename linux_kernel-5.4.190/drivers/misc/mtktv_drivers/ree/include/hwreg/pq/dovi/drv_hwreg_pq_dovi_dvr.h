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

#ifndef _DRV_HWREG_PQ_DOVI_DVR_H_
#define _DRV_HWREG_PQ_DOVI_DVR_H_

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
enum reg_pq_dovi_dvr_input_path {
	REG_PQ_DOVI_DVR_INPUT_PATH_IDW0 = 0,
	REG_PQ_DOVI_DVR_INPUT_PATH_IDW1,
};

enum reg_pq_dovi_dvr_dvdma_output {
	REG_PQ_DOVI_DVR_DVDMA_OUTPUT_OFF = 0,
	REG_PQ_DOVI_DVR_DVDMA_OUTPUT_IDR,
	REG_PQ_DOVI_DVR_DVDMA_OUTPUT_IDW0,
	REG_PQ_DOVI_DVR_DVDMA_OUTPUT_IDW1,
};

enum reg_pq_dovi_dvr_format {
	REG_PQ_DOVI_DVR_FORMAT_YUV420 = 0,
	REG_PQ_DOVI_DVR_FORMAT_YUV422 = 2,
	REG_PQ_DOVI_DVR_FORMAT_YUV444 = 4,
	REG_PQ_DOVI_DVR_FORMAT_RGB    = 11,
};

struct reg_pq_dovi_dvr_format_conversion {
	bool fmt_en;
	enum reg_pq_dovi_dvr_format fmt;
};

struct reg_pq_dovi_dvr_force_read {
	uint8_t index;
	uint8_t mode;
	bool en;
};

struct reg_pq_dovi_dvr_crop {
	uint32_t h_size;
	uint32_t v_size;
	uint32_t x;
	uint32_t y;
};

struct reg_pq_dovi_dvr_param_in {
	enum reg_pq_dovi_dvr_input_path input_path;
	enum reg_pq_dovi_dvr_dvdma_output dvdma_out;
	struct reg_pq_dovi_dvr_format_conversion fmt_conv;
	uint8_t access_mode;
	struct reg_pq_dovi_dvr_force_read force_read;
	struct reg_pq_dovi_dvr_crop crop;
};

struct reg_pq_dovi_dvr_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};

//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
int drv_hwreg_pq_dovi_dvr_set_input_path(
	struct reg_pq_dovi_dvr_param_in *param_in,
	struct reg_pq_dovi_dvr_param_out *param_out);

int drv_hwreg_pq_dovi_dvr_set_dvdma_output(
	struct reg_pq_dovi_dvr_param_in *param_in,
	struct reg_pq_dovi_dvr_param_out *param_out);

int drv_hwreg_pq_dovi_dvr_set_format(
	struct reg_pq_dovi_dvr_param_in *param_in,
	struct reg_pq_dovi_dvr_param_out *param_out);

int drv_hwreg_pq_dovi_dvr_set_access(
	struct reg_pq_dovi_dvr_param_in *param_in,
	struct reg_pq_dovi_dvr_param_out *param_out);

int drv_hwreg_pq_dovi_dvr_set_force_read(
	struct reg_pq_dovi_dvr_param_in *param_in,
	struct reg_pq_dovi_dvr_param_out *param_out);

int drv_hwreg_pq_dovi_dvr_set_crop(
	struct reg_pq_dovi_dvr_param_in *param_in,
	struct reg_pq_dovi_dvr_param_out *param_out);

#endif // _DRV_HWREG_PQ_DOVI_DVR_H_
