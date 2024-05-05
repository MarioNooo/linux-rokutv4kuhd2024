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

#ifndef _HWREG_RENDER_CVBSO_H_
#define _HWREG_RENDER_CVBSO_H_

#include "hwreg_common.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */
#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif

#ifndef true
/// definition for true
#define true                        1
/// definition for false
#define false                       0
#endif

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */

enum reg_render_cvbso_clk_setting_type {
	REG_RENDER_CVBSO_CLK_SETTING_OFF = 0,
	REG_RENDER_CVBSO_CLK_SETTING_SV_ON,
	REG_RENDER_CVBSO_CLK_SETTING_SV_OFF,
	REG_RENDER_CVBSO_CLK_SETTING_CVBS_ON,
	REG_RENDER_CVBSO_CLK_SETTING_CVBS_OFF,
	REG_RENDER_CVBSO_CLK_SETTING_VIF_DAC,
	REG_RENDER_CVBSO_CLK_SETTING_VIF_VIF,
	REG_RENDER_CVBSO_CLK_SETTING_NUM,
};

enum reg_render_cvbso_swen_type {
	REG_RENDER_CVBSO_SWEN_OFF = 0,
	REG_RENDER_CVBSO_SWEN_SV_ON,
	REG_RENDER_CVBSO_SWEN_SV_OFF,
	REG_RENDER_CVBSO_SWEN_CVBS_ON,
	REG_RENDER_CVBSO_SWEN_CVBS_OFF,
	REG_RENDER_CVBSO_SWEN_VIF_DAC,
	REG_RENDER_CVBSO_SWEN_VIF_VIF,
	REG_RENDER_CVBSO_SWEN_NUM,
};

enum reg_render_cvbso_type {
	REG_RENDER_CVBSO_OFF = 0,
	REG_RENDER_CVBSO_SV_ON,
	REG_RENDER_CVBSO_SV_OFF,
	REG_RENDER_CVBSO_CVBS_ON,
	REG_RENDER_CVBSO_CVBS_OFF,
	REG_RENDER_CVBSO_VIF_DAC,
	REG_RENDER_CVBSO_VIF_VIF,
	REG_RENDER_CVBSO_NUM,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */
struct reg_render_cvbso_clk_info {
	char clk_name[50];
	char clk_data[50];
};

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
int drv_hwreg_render_cvbso_load_table(enum reg_render_cvbso_type cvbso_type);
int drv_hwreg_render_cvbso_set_enable(bool enable);
int drv_hwreg_render_cvbso_get_clk_setting(
	enum reg_render_cvbso_clk_setting_type cvbso_clk_type,
	uint16_t max_clk_nums,
	struct reg_render_cvbso_clk_info *clk_info,
	uint16_t *clk_cnt);
int drv_hwreg_render_cvbso_get_clk_swen(
	enum reg_render_cvbso_swen_type cvbso_swen_type,
	uint16_t max_clk_nums,
	struct reg_render_cvbso_clk_info *clk_info,
	uint16_t *clk_cnt);
int drv_hwreg_render_cvbso_set_hw_version(uint32_t version);
#endif // _HWREG_RENDER_CVBSO_H_
