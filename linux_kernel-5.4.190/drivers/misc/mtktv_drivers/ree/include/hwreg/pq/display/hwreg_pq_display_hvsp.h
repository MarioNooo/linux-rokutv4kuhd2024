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

#ifndef _HWREG_PQ_DISPLAY_HVSP_H_
#define _HWREG_PQ_DISPLAY_HVSP_H_

#include "hwreg_common.h"
#include "hwreg_pq_display_scmi.h"

//------------------------------------------------------------------------------
//  Driver Capability
//------------------------------------------------------------------------------
#define HVSP_ADL_WORD_LENGTH	(32) //byte
#define SRAM_PHASE	(0x81)
#define C_SRAM_0_DEPTH	(0x40)
#define C_SRAM_0_DEPTH_START	(0x0)
#define C_SRAM_0_DEPTH_END	(0x40)
#define C_SRAM_1_DEPTH_START	(0x81)
#define C_SRAM_1_DEPTH_END	(0xC1)
#define Y_SRAM_0_DEPTH	(0x80)
#define Y_SRAM_0_DEPTH_START	(0x0)
#define Y_SRAM_0_DEPTH_END	(0x80)
#define Y_SRAM_1_DEPTH_START	(0x81)
#define Y_SRAM_1_DEPTH_END	(0x101)
#define C_COEF_NUM	(4)
#define Y_COEF_NUM	(10)
#define C_READBACK_NUM	(3)
#define Y_READBACK_NUM	(7)
//100 bits
#define Y_VALID_BITS	(100)
#define Y_TOTAL_BITS	(160)
//40 bits
#define C_VALID_BITS	(40)
#define C_TOTAL_BITS	(48)
//32 Byte : Y(20), C(8), Dummy(4)
#define C_SRAM_OFFSET	(10)
#define Y_SRAM_OFFSET	(0)
//------------------------------------------------------------------------------
//  Macro and Define
//------------------------------------------------------------------------------
enum hwreg_pq_hvsp_sram_type {
	HWREG_PQ_HVSP_CSRAM0,
	HWREG_PQ_HVSP_CSRAM1,
	HWREG_PQ_HVSP_CSRAM2,
	HWREG_PQ_HVSP_YSRAM0,
	HWREG_PQ_HVSP_YSRAM1,
	HWREG_PQ_HVSP_YSRAM2,
};

#define HVSP_Y_COEF_SET_NUM		(129)
#define HVSP_C_COEF_SET_NUM		(65)
#define HVSP_ADL_CMD_SIZE		(32)//256bit

#define HVSP_Y_SRAM0		(0x00)	//main, vertical, Y coef
#define HVSP_Y_SRAM1		(0x01)	//main, horizontol, Y coef
#define HVSP_Y_SRAM2		(0x02)
#define HVSP_Y_SRAM3		(0x03)
#define HVSP_C_SRAM0		(0x04)	//main, vertical, C coef
#define HVSP_C_SRAM1		(0x05)	//main, horizontol, C coef
#define HVSP_C_SRAM2		(0x06)
#define HVSP_C_SRAM3		(0x07)
//------------------------------------------------------------------------------
//  Type and Structure
//------------------------------------------------------------------------------
struct hvsp_postScaling_info {
	bool bHScaling;
	bool bVScaling;
	bool bHShiftMode;
	bool bVShiftMode;
	uint32_t HScalingPhase;
	uint32_t VScalingPhase;
	uint32_t HScalingFactor;
	uint32_t VScalingFactor;
};
//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
void drv_hwreg_pq_display_hvsp_set_postScaling(bool bRIU,
	struct hwregOut *hwOut, struct hvsp_postScaling_info SU_info);
void drv_hwreg_pq_display_hvsp_set_hvspSize(bool bRIU,
	struct hwregOut *hwOut,
	struct hwreg_pq_window_info hvspInputWin,
	struct hwreg_pq_window_info hvspOutnputWin);
int drv_hwreg_pq_display_hvsp_adl_enable(bool bRIU,
	struct hwregOut *hwOut, bool enable);
int drv_hwreg_pq_display_hvsp_bypass_enable(bool bRIU,
	struct hwregOut *hwOut, bool enable);
int drv_hwreg_pq_display_hvsp_set_readback(uint16_t addr, enum hwreg_pq_hvsp_sram_type type);
int drv_hwreg_pq_display_hvsp_set_sram(uint16_t addr, enum hwreg_pq_hvsp_sram_type type, uint8_t* data_per_word);
int drv_hwreg_pq_display_hvsp_get_csram(void *pData);
int drv_hwreg_pq_display_hvsp_get_ysram(void *pData);
int drv_hwreg_pq_display_hvsp_set_scscl(bool bRIU, struct hwregOut *hwOut,
	uint32_t val);
int drv_hwreg_pq_display_hvsp_set_12tapEn(bool bRIU, struct hwregOut *hwOut, bool En);
bool drv_hwreg_pq_display_hvsp_set_blk_size(bool bRIU,
	struct hwregOut *hwOut,uint16_t width, uint16_t height);
bool drv_hwreg_pq_display_hvsp_set_ini_factor(bool bRIU,
	struct hwregOut *hwOut, uint32_t ho,uint32_t ve);
bool drv_hwreg_pq_display_hvsp_set_shift_en(bool bRIU,
	struct hwregOut *hwOut, bool h_en, bool v_en);
bool drv_hwreg_pq_display_hvsp_set_scale_en(bool bRIU,
	struct hwregOut *hwOut, bool h_en, bool v_en);
bool drv_hwreg_pq_display_hvsp_set_scale_ratio(bool bRIU,
	struct hwregOut *hwOut, uint32_t h_rotio, uint32_t v_rotio);
bool drv_hwreg_pq_display_hvsp_set_adl_cnt(bool bRIU,
	struct hwregOut *hwOut, uint32_t h_cnt, uint32_t v_cnt);
bool drv_hwreg_pq_display_hvsp_set_adl_write_done(bool bRIU,
	struct hwregOut *hwOut, bool en);
#endif // _HWREG_PQ_DISPLAY_HVSP_H_
