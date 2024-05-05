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

#ifndef _HWREG_PQ_DISPLAY_METER_H_
#define _HWREG_PQ_DISPLAY_METER_H_

#include "hwreg_common.h"
#include "mapi_pq_stat_if.h"

//------------------------------------------------------------------------------
//  Driver Capability
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Macro and Define
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Type and Structure
//------------------------------------------------------------------------------
int drv_hwreg_pq_display_set_yhsl(struct hwregWriteMethodInfo *writeInfo,
	uint16_t vsize, enum EN_PQ_METER_0_POSITION position);
int drv_hwreg_pq_display_set_ylite(struct hwregWriteMethodInfo *writeInfo,
	uint16_t vsize, enum EN_PQ_METER_3_POSITION position);
int drv_hwreg_pq_display_set_freq_det(struct hwregWriteMethodInfo *writeInfo,
	uint16_t vsize, enum EN_PQ_METER_2_POSITION position);
int drv_hwreg_pq_display_set_3d_meter(struct hwregWriteMethodInfo *writeInfo,
	uint16_t vsize, enum EN_PQ_3D_HIST_0_POSITION position);
int drv_hwreg_pq_display_set_obj_hist(struct hwregWriteMethodInfo *writeInfo,
	uint16_t vsize, enum EN_PQ_METER_1_POSITION position);
#endif
