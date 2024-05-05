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

#ifndef _HWREG_RENDER_VIDEO_FRC_H_
#define _HWREG_RENDER_VIDEO_FRC_H_

#include "hwreg_common.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
struct hwregFrcInsIn {
	bool RIU;
	uint32_t hvspOut_width;
	uint32_t hvspOut_height;
	uint32_t hvspOut_w_align; //Aligned
	uint32_t hvspOut_h_align; //Aligned
	uint32_t buffer_h;
	uint32_t buffer_v;
};

struct hwregFrcBaseAddrIn {
	bool RIU;
	uint64_t frc_pq_addr;
	uint64_t frc_pq_size;
};

struct hwregFrcClockIn {
	bool RIU;
	bool bEnable;
};

typedef enum E_DRV_IPM_MEMORY_CONFIG
{
    E_DRV_IPM_MEMORY_CONFIG_444_12B,
    E_DRV_IPM_MEMORY_CONFIG_422_12B,
    E_DRV_IPM_MEMORY_CONFIG_444_8B,
    E_DRV_IPM_MEMORY_CONFIG_422_8B,
    E_DRV_IPM_MEMORY_CONFIG_420_8B,
    E_DRV_IPM_MEMORY_CONFIG_444_6B,
    E_DRV_IPM_MEMORY_CONFIG_422_6B,
    E_DRV_IPM_MEMORY_CONFIG_420_6B,
    E_DRV_IPM_MEMORY_CONFIG_422_6B_MEDS_420_4B,
    E_DRV_IPM_MEMORY_CONFIG_MAX,

};

typedef enum
{
    E_DRV_MFC_MEMORY_CONFIG_444,
    E_DRV_MFC_MEMORY_CONFIG_422,
}E_DRV_MFC_MEMORY_CONFIG;

struct hwregFrc3DTableIn {
	bool RIU;
	enum E_DRV_IPM_MEMORY_CONFIG EN_Config;
	uint16_t u16cemodecontrol1;
	uint16_t u16cemodecontrol2;
	uint16_t s_u16Hsize;
	uint16_t s_u16Vsize;
	bool bEnMem420Mode;
	bool bEnIPM_Main444to422;
	bool bEnIPM_Main444to422_Filter;
	bool bEnIPM_Sub444to422;
	bool bEnIPM_Sub444to422_Filter;
	bool bEnOPM_422to444Md;
	bool bEnOPM_422to444Bypass;
	E_DRV_MFC_MEMORY_CONFIG MemMode;
	uint32_t u32FrameOffset;
	uint8_t FrameBuf;
};

struct hwregFrcUnmaskIn {
	bool RIU;
	bool unmask;
};

struct hwregFrcOpmMaskEn {
	bool RIU;
	bool enable;
};

struct hwregFrctriggenIn {
	bool RIU;
};

struct hwregFrcInitIn {
	bool RIU;
};

struct hwregFrcSwReset {
	bool RIU;
	bool bEnable;
};

struct hwregFrcBlendOutIn {
	bool RIU;
	bool blackEn;
	bool snowEn;
	uint16_t Hsize;
};

struct hwregFrcPatIn {
	bool RIU;
	bool bEn;
	uint16_t Hsize;
	uint16_t Vsize;
};


#define TRIG_GEN_OK 0
#define TRIG_GEN_NG 0xAA

#define REG_NUM_MAX 300
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
int drv_hwreg_render_frc_set_ins(
	struct hwregFrcInsIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_set_cropWindow(
	struct hwregCropWinIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_set_baseAddr(
	struct hwregFrcBaseAddrIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_set_clock(
	struct hwregFrcClockIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_set_3DTable(
	struct hwregFrc3DTableIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_opmMaskEn(
	struct hwregFrcOpmMaskEn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_unmask(
	struct hwregFrcUnmaskIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_trig_gen(
	struct hwregFrctriggenIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_sw_reset(
	struct hwregFrcSwReset paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_init(
	struct hwregFrcInitIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_init_1(
	struct hwregFrcInitIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_init_2(
	struct hwregFrcInitIn paramIn,
	struct hwregOut *paramOut);


int drv_hwreg_render_frc_set_blendOut(
	struct hwregFrcBlendOutIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_ipversion(
	uint8_t u8frc_ipversion);

void drv_hwreg_render_frc_set_trig_gen(uint8_t u8Result);
uint8_t drv_hwreg_render_frc_get_trig_gen(void);

int drv_hwreg_render_frc_set_ipmpattern(
	struct hwregFrcPatIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_set_vs_sw_trig(
	struct hwregFrcPatIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_set_opmpattern(
	struct hwregFrcPatIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_frc_set_freeze(
	struct hwregFrcClockIn paramIn,
	struct hwregOut *paramOut);

#endif // _HWREG_RENDER_VIDEO_FRC_H_
