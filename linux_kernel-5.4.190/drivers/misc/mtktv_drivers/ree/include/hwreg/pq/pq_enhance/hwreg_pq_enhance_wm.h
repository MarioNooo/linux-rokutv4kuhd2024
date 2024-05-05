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

#ifndef _HWREG_PQ_ENHANCE_WM_H_
#define _HWREG_PQ_ENHANCE_WM_H_

#include "hwreg_common.h"
//#include "m_pqu_pq.h"

//------------------------------------------------------------------------------
//  Driver Capability
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Macro and Define
//------------------------------------------------------------------------------

struct wm_result {
	__u32 u32Version;
	__u32 u32Length;
	__u16 u16Wm_En;
	__u16 u16Wm_decoded[30];
	__u16 u16Wm_histo[14];
	__u8 u8Wm_DetectStatus;
	__u16 u16Wm_RuninCheck;
};

//------------------------------------------------------------------------------
//  Type and Structure
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
int drv_hwreg_pq_enhance_set_stub(bool stub);
int drv_hwreg_pq_enhance_get_stub(bool *stub);
int drv_hwreg_pq_enhance_wm_enable(
	struct hwregOut *hwOut, bool srsEnable);
int drv_hwreg_pq_enhance_wm_get_decoded(struct wm_result *result);
int drv_hwreg_pq_enhance_wm_get_histo(struct wm_result *result);

#endif // _HWREG_PQ_ENHANCE_WM_H_
