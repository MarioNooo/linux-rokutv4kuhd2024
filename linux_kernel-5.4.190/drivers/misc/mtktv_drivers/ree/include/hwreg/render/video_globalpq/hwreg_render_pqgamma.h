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

#ifndef _HWREG_RENDER_PQGAMMA_H_
#define _HWREG_RENDER_PQGAMMA_H_

#include "hwreg_common.h"

/* ---------------------------------------------------------------------------------------------- */
/* 	Define											  */
/* ---------------------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------------------- */
/* 	Macro											  */
/* ---------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------- */
/* 	Enum											  */
/* ---------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------- */
/*	Structure										  */
/* ---------------------------------------------------------------------------------------------- */

struct hwreg_render_pqgamma_gainoffset {
	uint16_t offset_r;
	uint16_t offset_g;
	uint16_t offset_b;
	uint16_t gain_r;
	uint16_t gain_g;
	uint16_t gain_b;
	bool pregainoffset;
};

struct hwreg_render_pqgamma_maxvalue {
	uint16_t max_r;
	uint16_t max_g;
	uint16_t max_b;
};

/* ---------------------------------------------------------------------------------------------- */
/*	Function										  */
/* ---------------------------------------------------------------------------------------------- */
int drv_hwreg_render_pqgamma_set_stub(bool stub);
int drv_hwreg_render_pqgamma_get_stub(bool *stub);
int drv_hwreg_render_pqgamma_adl_enable(bool pnlAdlEnable);
void drv_hwreg_render_pqgamma_hwversion(uint16_t hwversion);
int drv_hwreg_render_pqgamma_curve(uint16_t *r,uint16_t *g,uint16_t *b,uint16_t size);
int drv_hwreg_render_pqgamma_enable(bool bRIU, struct hwregOut *hwOut,
	bool pnlAdlEnable);
int drv_hwreg_render_pqgamma_gainoffset(bool bRIU, struct hwregOut *hwOut,
	struct hwreg_render_pqgamma_gainoffset gainoffset);
int drv_hwreg_render_pqgamma_gainoffset_enable(bool bRIU, struct hwregOut *hwOut,
	bool enable, bool pregainoffset);
int drv_hwreg_render_pqgamma_maxvalue_enable(bool bRIU, struct hwregOut *hwOut,
	bool enable);
int drv_hwreg_render_pqgamma_set_maxvalue(bool bRIU, struct hwregOut *hwOut,
	struct hwreg_render_pqgamma_maxvalue maxvalue);

#endif // _HWREG_RENDER_PQGAMMA_H_