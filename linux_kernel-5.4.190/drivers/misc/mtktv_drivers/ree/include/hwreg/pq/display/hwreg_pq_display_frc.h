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

#ifndef _HWREG_PQ_DISPLAY_FRC_H_
#define _HWREG_PQ_DISPLAY_FRC_H_

#include "hwreg_common.h"
#include "hwreg_pq_display_scmi.h"

/* ---------------------------------------------------------------------------------------------- */
/* 	Define											  */
/* ---------------------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------------------- */
/* 	Macro											  */
/* ---------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------- */
/* 	Enum											  */
/* ---------------------------------------------------------------------------------------------- */
//MEMC
typedef enum E_DRV_IPM_MEMORY_CONFIG
{
    E_DRV_IPM_MEMORY_CONFIG_444_12B,
    E_DRV_IPM_MEMORY_CONFIG_422_12B,
    E_DRV_IPM_MEMORY_CONFIG_444_8B,
    E_DRV_IPM_MEMORY_CONFIG_422_8B,
    E_DRV_IPM_MEMORY_CONFIG_420_8B,
    E_DRV_IPM_MEMORY_CONFIG_444_6B,
    E_DRV_IPM_MEMORY_CONFIG_422_6B,
    E_DRV_IPM_MEMORY_CONFIG_420_6B,
    E_DRV_IPM_MEMORY_CONFIG_422_6B_MEDS_420_4B,
    E_DRV_IPM_MEMORY_CONFIG_MAX,

};

typedef enum
{
    E_DRV_MFC_MEMORY_CONFIG_444,
    E_DRV_MFC_MEMORY_CONFIG_422,
    
}E_DRV_MFC_MEMORY_CONFIG;

/* ---------------------------------------------------------------------------------------------- */
/*	Structure										  */
/* ---------------------------------------------------------------------------------------------- */
struct hwreg_pq_display_frc_mem {
	uint64_t ipm_addr;
	uint32_t ipm_offset;
	uint64_t meds_addr;
	uint32_t meds_offset;
	uint64_t mvbff_addr;
	enum E_DRV_IPM_MEMORY_CONFIG cfg;
	uint16_t frc_framecount;
};

struct hwreg_pq_display_frc_size {
	bool v_size_en;
	uint32_t v_size;
	uint32_t h_size;
	uint32_t line_offset;
	uint32_t frame_offset;
	enum E_DRV_IPM_MEMORY_CONFIG EN_Config;
};

struct hwreg_pq_display_frc_aid {
	bool frc_aid_enable;
};


/* ---------------------------------------------------------------------------------------------- */
/*	Function										  */
/* ---------------------------------------------------------------------------------------------- */
int drv_hwreg_pq_display_frc_ipversion(
	uint8_t u8frc_ipversion);
int drv_hwreg_pq_display_frc_sel_path(
	uint8_t path_ctrl,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_frc_get_path(uint8_t *path_ctrl);

int drv_hwreg_pq_display_frc_set_req_off(
	bool req_off,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_frc_get_req_off(bool *req_off);
int drv_hwreg_pq_display_frc_set_mem(
	struct hwreg_pq_display_frc_mem *pmem,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_frc_get_mem(struct hwreg_pq_display_frc_mem *mem);
int drv_hwreg_pq_display_frc_set_3d_table(
	struct hwreg_pq_display_frc_size size,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_frc_set_flip(
	bool v_flip,
	bool riu,
	struct hwregOut *out);
int drv_hwreg_pq_display_frc_get_flip(bool *v_flip);

int drv_hwreg_pq_display_frc_get_dummy_reg(uint32_t *pval);
int drv_hwreg_pq_display_frc_set_scaling(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info hvspOutnputWin,
	int buffer_h,
	int buffer_v);
int drv_hwreg_pq_display_frc_set_aid(bool bRIU,
	struct hwreg_pq_display_frc_aid paramIn,
	struct hwregOut *paramOut);

#endif // _HWREG_PQ_DISPLAY_FRC_H_
