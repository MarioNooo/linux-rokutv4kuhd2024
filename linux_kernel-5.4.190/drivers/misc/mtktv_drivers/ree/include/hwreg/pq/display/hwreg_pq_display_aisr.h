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

#ifndef _HWREG_PQ_DISPLAY_AISR_H_
#define _HWREG_PQ_DISPLAY_AISR_H_

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
enum hwreg_pq_aisr_scale {
	HWREG_PQ_AISR_SCALE_1X,
	HWREG_PQ_AISR_SCALE_2X,
	HWREG_PQ_AISR_SCALE_3X,
	HWREG_PQ_AISR_SCALE_4X,
};

enum hwreg_pq_aisr_hvsp_sram_type {
	HWREG_PQ_AISR_HVSP_CSRAM1,
	HWREG_PQ_AISR_HVSP_CSRAM2,
	HWREG_PQ_AISR_HVSP_YSRAM1,
	HWREG_PQ_AISR_HVSP_YSRAM2,
};

struct hwreg_pq_aisr_win {
	uint16_t width_in;
	uint16_t height_in;
	uint16_t width_out;
	uint16_t height_out;
};

struct hwreg_pq_aisr_active_win {
	uint16_t h_start;
	uint16_t v_start;
	uint16_t h_end;
	uint16_t v_end;
};

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
int drv_hwreg_pq_display_aisr_set_stub(bool stub);
int drv_hwreg_pq_display_aisr_get_stub(bool *stub);
int drv_hwreg_pq_display_aisr_enable(bool bRIU,
	struct hwregOut *hwOut, bool bEn);
int drv_hwreg_pq_display_aisr_ratio(bool bRIU,
	struct hwregOut *hwOut, enum hwreg_pq_aisr_scale ratio);
int drv_hwreg_pq_display_aisr_win(bool bRIU,
	struct hwregOut *hwOut, struct hwreg_pq_aisr_win win);
int drv_hwreg_pq_display_aisr_inifactor(bool bRIU,
	struct hwregOut *hwOut, enum hwreg_pq_aisr_scale ratio);
int drv_hwreg_pq_display_aisr_scalefactor(bool bRIU,
	struct hwregOut *hwOut, enum hwreg_pq_aisr_scale ratio);
int drv_hwreg_pq_display_aisr_repeat_num(bool bRIU,
	struct hwregOut *hwOut, enum hwreg_pq_aisr_scale ratio);
int drv_hwreg_pq_display_aisr_adl_enable(bool bRIU,
	struct hwregOut *hwOut, bool aisrAdlEnable);
int drv_hwreg_pq_display_aisr_get_sram(uint16_t addr, void *pData);
int drv_hwreg_pq_display_aisr_hvsp_adl_enable(bool bRIU,
	struct hwregOut *hwOut, bool enable);
int drv_hwreg_pq_display_aisr_hvsp_set_readback(uint16_t addr, enum hwreg_pq_aisr_hvsp_sram_type type);
int drv_hwreg_pq_display_aisr_hvsp_get_csram(void *pData);
int drv_hwreg_pq_display_aisr_hvsp_get_ysram1(void *pData);
int drv_hwreg_pq_display_aisr_hvsp_get_ysram2(void *pData);
int drv_hwreg_pq_display_aisr_set_active_window(bool bRIU,
	struct hwregOut *hwOut, bool enable, struct hwreg_pq_aisr_active_win win);

#endif // _HWREG_PQ_DISPLAY_AISR_H_
