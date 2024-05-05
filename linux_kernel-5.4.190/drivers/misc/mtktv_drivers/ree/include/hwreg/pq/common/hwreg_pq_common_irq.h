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

#ifndef _HWREG_PQ_COMMON_IRQ_H_
#define _HWREG_PQ_COMMON_IRQ_H_

#include "hwreg_common.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

enum pq_hwreg_irq_type {
	pq_hwreg_irq_type_hk = 0,
	pq_hwreg_irq_type_frc,
	pq_hwreg_irq_type_pq,
	pq_hwreg_irq_type_max,
};

enum pq_hwreg_irq_event {
	pq_hwreg_irq_event_sw_irq_trig_ip = 0,
	pq_hwreg_irq_event_sw_irq_trig_op2,
	pq_hwreg_irq_event_sw_irq_trig_disp,
	pq_hwreg_irq_event_sw_irq_trig_cmdq_done,
	pq_hwreg_irq_event_pq_irq_trig_ip,
	pq_hwreg_irq_event_sw_irq_trig_each_win_done,
	pq_hwreg_irq_event_pq_irq_trig_disp,
	pq_hwreg_irq_event_pq_irq_trig_disp_b2r,
	pq_hwreg_irq_event_pq_irq_trig_disp_ipdma,
	pq_hwreg_irq_event_max,
};
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
struct hwregPQIRQForceIn {
	bool RIU;
	enum pq_hwreg_irq_type irqType;
	enum pq_hwreg_irq_event irqEvent;
	bool force;
};

struct hwregPQIRQMaskIn {
	bool RIU;
	enum pq_hwreg_irq_type irqType;
	enum pq_hwreg_irq_event irqEvent;
	bool mask;
};

struct hwregPQIRQClrIn {
	bool RIU;
	enum pq_hwreg_irq_type irqType;
	enum pq_hwreg_irq_event irqEvent;
};

struct hwregPQIRQStatusIn {
	bool RIU;
	enum pq_hwreg_irq_type irqType;
	enum pq_hwreg_irq_event irqEvent;
};
//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
int drv_hwreg_pq_common_irq_set_patch(
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_pq_common_irq_set_force(
	struct hwregPQIRQForceIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_pq_common_irq_set_mask(
	struct hwregPQIRQMaskIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_pq_common_irq_set_clr(
	struct hwregPQIRQClrIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_pq_common_irq_get_mask(
	struct hwregPQIRQMaskIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_pq_common_irq_get_status(
	struct hwregPQIRQStatusIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

#endif // _HWREG_PQ_COMMON_IRQ_H_
