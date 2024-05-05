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

#ifndef _HWREG_PQ_ENHANCE_FILM_H_
#define _HWREG_PQ_ENHANCE_FILM_H_

#include "hwreg_common.h"
#include "hwreg_pq_display_scmi.h"

//------------------------------------------------------------------------------
//  Driver Capability
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Macro and Define
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Type and Structure
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
void drv_hwreg_pq_display_film_ip_set_size(bool bRIU, struct hwregOut *hwOut,
	struct hwreg_pq_window_info filmWin);
void drv_hwreg_pq_display_film_op_set_size(bool bRIU, struct hwregOut *hwOut,
	struct hwreg_pq_window_info filmWin);
void drv_hwreg_pq_display_film_znr_set_size(bool bRIU, struct hwregOut *hwOut,
	struct hwreg_pq_window_info filmWin);
int drv_hwreg_pq_enhance_film_init(uint32_t version);
#endif // _HWREG_PQ_ENHANCE_FILM_H_