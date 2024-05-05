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

#ifndef _HWREG_SRCCAP_DSCL_H_
#define _HWREG_SRCCAP_DSCL_H_

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
enum reg_srccap_dscl_mode {
	REG_SRCCAP_DSCL_SOURCE_8P_MODE = 0,
	REG_SRCCAP_DSCL_SOURCE_1P_MODE = 1,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */
struct reg_srccap_dscl_scaling_info {
	bool h_scaling;
	uint64_t h_scaling_ratio;
	bool h_filter_mode;
	bool v_scaling;
	uint64_t v_scaling_ratio;
	bool v_linear_mode;
	uint16_t vsd_ipmfetch;
};

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
int drv_hwreg_srccap_dscl_set_source(
	uint8_t dev_id,
	enum reg_srccap_dscl_mode mode,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dscl_set_scaling(
	uint8_t dev_id,
	struct reg_info *reg,
	uint16_t *count,
	bool riu,
	struct reg_srccap_dscl_scaling_info *dscl_scaling_info);
#endif // _HWREG_SRCCAP_DSCL_H_