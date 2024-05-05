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

#ifndef _HWREG_SRCCAP_MUX_H_
#define _HWREG_SRCCAP_MUX_H_

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
enum reg_srccap_mux_source {
	REG_SRCCAP_MUX_SOURCE_ADCA = 0,
	REG_SRCCAP_MUX_SOURCE_ADCB,
	REG_SRCCAP_MUX_SOURCE_VD,
	REG_SRCCAP_MUX_SOURCE_HDMI,
	REG_SRCCAP_MUX_SOURCE_HDMI2,
	REG_SRCCAP_MUX_SOURCE_HDMI3,
	REG_SRCCAP_MUX_SOURCE_HDMI4,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
int drv_hwreg_srccap_mux_set_source(
	uint8_t dev_id,
	enum reg_srccap_mux_source src,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);


#endif // _HWREG_SRCCAP_MUX_H_
