////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020-2024 MediaTek Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MediaTek Inc. and be kept in strict confidence
// ("MediaTek Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MediaTek Confidential
// Information is unlawful and strictly prohibited. MediaTek hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HWREG_RENDER_COMMON_IRQ_H_
#define _HWREG_RENDER_COMMON_IRQ_H_

#include "hwreg_common.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

enum drm_hwreg_irq_type {
	VIDEO_IRQTYPE_HK = 0,
	VIDEO_IRQTYPE_FRC,
	VIDEO_IRQTYPE_PQ,
	VIDEO_IRQTYPE_MAX,
};

enum drm_hwreg_irq_event {
	VIDEO_IRQEVENT_SW_IRQ_TRIG_DISP = 0,
	VIDEO_IRQEVENT_PQ_IRQ_TRIG_DISP,
	VIDEO_IRQEVENT_MAX,
};
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
struct hwregIRQForceIn {
	bool RIU;
	enum drm_hwreg_irq_type irqType;
	enum drm_hwreg_irq_event irqEvent;
	bool force;
};

struct hwregIRQmaskIn {
	bool RIU;
	enum drm_hwreg_irq_type irqType;
	enum drm_hwreg_irq_event irqEvent;
	bool mask;
};

struct hwregIRQClrIn {
	bool RIU;
	enum drm_hwreg_irq_type irqType;
	enum drm_hwreg_irq_event irqEvent;
};

struct hwregIRQstatusIn {
	bool RIU;
	enum drm_hwreg_irq_type irqType;
	enum drm_hwreg_irq_event irqEvent;
};
//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
int drv_hwreg_render_common_irq_set_force(
	struct hwregIRQForceIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_render_common_irq_set_mask(
	struct hwregIRQmaskIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_render_common_irq_set_clr(
	struct hwregIRQClrIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_render_common_irq_get_mask(
	struct hwregIRQmaskIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_render_common_irq_get_status(
	struct hwregIRQstatusIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

#endif // _HWREG_RENDER_COMMON_IRQ_H_
