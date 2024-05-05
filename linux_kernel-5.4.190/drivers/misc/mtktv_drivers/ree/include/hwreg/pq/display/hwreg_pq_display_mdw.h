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

#ifndef _HWREG_PQ_DISPLAY_MDW_H_
#define _HWREG_PQ_DISPLAY_MDW_H_

#include "hwreg_common.h"

/* ---------------------------------------------------------------------------------------------- */
/* 	Define											  */
/* ---------------------------------------------------------------------------------------------- */
// pq output path
#define HWREG_PQ_OUTPUT_PATH_WITHOUT_BUF    (0x01)
#define HWREG_PQ_OUTPUT_PATH_WITH_BUF_MDW   (0x02)
#define HWREG_PQ_OUTPUT_PATH_WITH_BUF_FRC   (0x04)

/* ---------------------------------------------------------------------------------------------- */
/* 	Macro											  */
/* ---------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------- */
/* 	Enum											  */
/* ---------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------- */
/*	Structure										  */
/* ---------------------------------------------------------------------------------------------- */
struct hwreg_pq_display_output_path_ctrl {
	uint8_t output_path;
};

struct hwreg_pq_display_mdw_mem {
	uint32_t addr_g;
	uint32_t addr_b;
	uint32_t addr_r;
	uint16_t cfg;
};

struct hwreg_pq_display_mdw_size {
	bool v_size_en;
	uint32_t v_size;
	uint32_t h_size;
	uint32_t line_offset;
	uint32_t frame_offset_g;
	uint32_t frame_offset_b;
	uint32_t frame_offset_r;
};

/* ---------------------------------------------------------------------------------------------- */
/*	Function										  */
/* ---------------------------------------------------------------------------------------------- */
int drv_hwreg_pq_display_set_output_path(
	struct hwreg_pq_display_output_path_ctrl path_ctrl,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_get_output_path(struct hwreg_pq_display_output_path_ctrl *path_ctrl);
int drv_hwreg_pq_display_mdw_set_en(
	bool en,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_mdw_get_en(bool *en);
int drv_hwreg_pq_display_mdw_set_req_off(
	bool req_off,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_mdw_get_req_off(bool *req_off);
int drv_hwreg_pq_display_mdw_set_mem(
	struct hwreg_pq_display_mdw_mem mem,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_mdw_get_mem(struct hwreg_pq_display_mdw_mem *mem);
int drv_hwreg_pq_display_mdw_set_size(
	struct hwreg_pq_display_mdw_size size,
	bool riu,
	struct hwregWriteMethodInfo *writeInfo);
int drv_hwreg_pq_display_mdw_get_size(struct hwreg_pq_display_mdw_size *size);
int drv_hwreg_pq_display_mdw_set_flip(
	bool v_flip,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_mdw_get_flip(bool *v_flip);
int drv_hwreg_pq_display_mdw_set_win_id(
	uint8_t win_id,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_mdw_get_win_id(uint8_t *win_id);
void drv_hwreg_pq_display_mdw_set_version(uint32_t mdw_version);


#endif // _HWREG_PQ_DISPLAY_MDW_H_