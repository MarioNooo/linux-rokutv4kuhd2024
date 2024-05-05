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

#ifndef _HWREG_SRCCAP_COMMON_IRQ_H_
#define _HWREG_SRCCAP_COMMON_IRQ_H_

#include "hwreg_common.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

enum srccap_hwreg_irq_type {
	srccap_hwreg_irq_type_hk = 0,
	srccap_hwreg_irq_type_frc,
	srccap_hwreg_irq_type_pq,
	srccap_hwreg_irq_type_max,
};

enum srccap_hwreg_irq_event {
	srccap_hwreg_irq_event_sw_irq_trig_src0 = 0,
	srccap_hwreg_irq_event_sw_irq_trig_src1,
	srccap_hwreg_irq_event_pq_irq_trig_src0,
	srccap_hwreg_irq_event_dv_hw5_ip_int0,
	srccap_hwreg_irq_event_dv_hw5_ip_int1,
	srccap_hwreg_irq_event_max,
};
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
struct hwregSrcIRQForceIn {
	bool RIU;
	enum srccap_hwreg_irq_type irqType;
	enum srccap_hwreg_irq_event irqEvent;
	bool force;
};

struct hwregSrcIRQMaskIn {
	bool RIU;
	enum srccap_hwreg_irq_type irqType;
	enum srccap_hwreg_irq_event irqEvent;
	bool mask;
};

struct hwregSrcIRQClrIn {
	bool RIU;
	enum srccap_hwreg_irq_type irqType;
	enum srccap_hwreg_irq_event irqEvent;
};

struct hwregSrcIRQStatusIn {
	bool RIU;
	enum srccap_hwreg_irq_type irqType;
	enum srccap_hwreg_irq_event irqEvent;
};
//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
int drv_hwreg_srccap_common_irq_set_force(
	struct hwregSrcIRQForceIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_srccap_common_irq_set_mask(
	struct hwregSrcIRQMaskIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_srccap_common_irq_set_clr(
	struct hwregSrcIRQClrIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_srccap_common_irq_get_mask(
	struct hwregSrcIRQMaskIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

int drv_hwreg_srccap_common_irq_get_status(
	struct hwregSrcIRQStatusIn paramIn,
	struct hwregOut *paramOut,
	uint32_t u32IRQ_Version);

#endif // _HWREG_SRCCAP_COMMON_IRQ_H_
