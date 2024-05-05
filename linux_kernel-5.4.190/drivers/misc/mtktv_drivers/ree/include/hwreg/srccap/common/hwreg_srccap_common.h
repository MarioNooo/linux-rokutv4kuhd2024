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

#ifndef _HWREG_SRCCAP_COMMON_H_
#define _HWREG_SRCCAP_COMMON_H_

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

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
int drv_hwreg_srccap_common_set_rgb_write(
	uint8_t dev_id,
	bool en,
	uint8_t mc,
	bool riu,
	struct reg_info *reg,
	uint16_t *count,
	uint8_t hw_version);
int drv_hwreg_srccap_common_set_hdmi_420to444(
	uint8_t dev_id,
	bool enable,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_common_set_hdmi422pack(
	uint8_t dev_id,
	bool enable,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
bool KDrv_HW_XVYCC_readSram_degamma(void *pData);
#endif // _HWREG_SRCCAP_COMMON_H_