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

#ifndef _HWREG_SRCCAP_DV_SD_H_
#define _HWREG_SRCCAP_DV_SD_H_

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
struct reg_srccap_dv_sd_info {
	bool     h_scaling_en;
	uint32_t h_scaling_ratio;
	bool     h_filter_mode; // 0: cb mode, 1: linear mode
	bool     v_scaling_en;
	uint32_t v_scaling_ratio;
	bool     v_filter_mode; // 0: cb mode, 1: linear mode
	uint16_t vsd_ipm_fetch;
};

//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
int drv_hwreg_srccap_dv_sd_set_scaling(
	uint8_t dev_id,
	bool riu,
	struct reg_info *reg,
	uint16_t *count,
	struct reg_srccap_dv_sd_info *sd_info);

#endif // _HWREG_SRCCAP_DV_SD_H_
