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

#ifndef _HWREG_RENDER_PNLGAMMA_H_
#define _HWREG_RENDER_PNLGAMMA_H_

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
struct hwreg_render_pnlgamma_gainoffset {
	uint16_t offset_r;
	uint16_t offset_g;
	uint16_t offset_b;
	uint16_t gain_r;
	uint16_t gain_g;
	uint16_t gain_b;
};

/* ---------------------------------------------------------------------------------------------- */
/*	Function										  */
/* ---------------------------------------------------------------------------------------------- */
int drv_hwreg_render_pnlgamma_set_stub(bool stub);
int drv_hwreg_render_pnlgamma_get_stub(bool *stub);
int drv_hwreg_render_pnlgamma_adl_enable(bool pnlAdlEnable);
int drv_hwreg_render_pnlgamma_curve(uint16_t *r,uint16_t *g,uint16_t *b,uint16_t size);
int drv_hwreg_render_pnlgamma_enable(bool bRIU, struct hwregOut *hwOut,
	bool pnlAdlEnable);
int drv_hwreg_render_pnlgamma_gainoffset(bool bRIU, struct hwregOut *hwOut,
	struct hwreg_render_pnlgamma_gainoffset gainoffset);
int drv_hwreg_render_pnlgamma_gainoffset_enable(bool bRIU, struct hwregOut *hwOut,
	bool enable);
#endif // _HWREG_RENDER_PNLGAMMA_H_