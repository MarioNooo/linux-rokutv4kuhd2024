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

////////////////////////////////////////////////////////////////////////////////
///
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HWREG_PQ_DISPLAY_IP2_H_
#define _HWREG_PQ_DISPLAY_IP2_H_

#include "hwreg_common.h"

//------------------------------------------------------------------------------
//  Driver Capability
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Macro and Define
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Type and Structure
//------------------------------------------------------------------------------
enum ip2_source_type {
	ip2_source_ipdma0 = 0,
	ip2_source_ipdma1,
	ip2_source_b2r,
};

struct ip2_prescaling_info {
	bool bHScaling;
	uint64_t HScalingRatio;
	bool bHFilterMode;
	bool bVScaling;
	uint64_t VScalingRatio;
	bool bVCBMode;
	uint16_t VSDIPMFetch;
};
//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
void drv_hwreg_pq_display_ip2_set_inputMux(bool bRIU,
	struct hwregOut *hwOut, enum ip2_source_type enInputType);
void drv_hwreg_pq_display_ip2_set_inputSize(bool bRIU,
	struct hwregOut *hwOut, uint16_t hsize, uint16_t vsize);
void drv_hwreg_pq_display_ip2_set_preScaling(bool bRIU,
	struct hwregOut *hwOut, struct ip2_prescaling_info SD_info);
void drv_hwreg_pq_display_ip2_set_HProtect(bool bRIU,
	struct hwregOut *hwOut, uint16_t hsize);
void drv_hwreg_pq_display_dynamic_ultra_init(bool bRIU,
	struct hwregOut *hwOut);
#endif // _HWREG_PQ_DISPLAY_IP2_H_
