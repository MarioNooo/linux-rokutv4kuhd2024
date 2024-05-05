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

#ifndef _HWREG_RENDER_COMMON_TRIGGER_GEN_H_
#define _HWREG_RENDER_COMMON_TRIGGER_GEN_H_

#include "hwreg_common.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
enum drm_hwreg_triggen_domain {
	RENDER_COMMON_TRIGEN_DOMAIN_DISP = 0,
	RENDER_COMMON_TRIGEN_DOMAIN_FRC1,
	RENDER_COMMON_TRIGEN_DOMAIN_FRC2,
	RENDER_COMMON_TRIGEN_DOMAIN_TGEN,
	RENDER_COMMON_TRIGEN_DOMAIN_MAX,
};
enum drm_hwreg_triggen_src_sel {
	RENDER_COMMON_TRIGEN_INPUT_IP0 = 0,
	RENDER_COMMON_TRIGEN_INPUT_IP1,
	RENDER_COMMON_TRIGEN_INPUT_B2R0,
	RENDER_COMMON_TRIGEN_INPUT_B2R1,
	RENDER_COMMON_TRIGEN_INPUT_TGEN,
	RENDER_COMMON_TRIGEN_INPUT_MAX,
};
enum drm_hwreg_triggen_trig_sel {
	RENDER_COMMON_TRIGEN_TRIG_SEL_VS = 0,
	RENDER_COMMON_TRIGEN_TRIG_SEL_VFDE,
	RENDER_COMMON_TRIGEN_TRIG_SEL_MAX,
};

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
struct hwregTrigGenInputSrcSelIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	enum drm_hwreg_triggen_src_sel src;
};

struct hwregTrigGenCommonTrigSelIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	enum drm_hwreg_triggen_trig_sel trig_sel;
};

struct hwregTrigGenTrigSelSepIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	bool trig_sel_separate;
};

struct hwregTrigGenVsSwTrigIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	bool sw_trig_mode;
};

struct hwregTrigGenSwHcntIniValIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	uint16_t sw_hcnt;
};

struct hwregTrigGenSwHttSizeIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	uint16_t sw_htt;
};

struct hwregTrigGenSwUserModeIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	bool sw_user_mode_h;
	bool sw_user_mode_v;
};

struct hwregTrigGenVcntKeepEnIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	bool vcnt_keep_en;
	uint8_t vcnt_upd_mask_range;
};

struct hwregTrigGenCfgIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	bool trig_sel_separate;
	enum drm_hwreg_triggen_trig_sel trig_sel;
	bool sw_trig_mode;
	uint16_t sw_hcnt;
	uint16_t sw_htt;
};

struct hwregTrigGenRWbankDlyIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	uint16_t RWBk_ud_dly_h;
	uint16_t RWBk_ud_dly_v;
	enum drm_hwreg_triggen_trig_sel rwbank_trig_sel;
};

struct hwregTrigGenDSDlyIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	uint16_t ds_dly_v;
	enum drm_hwreg_triggen_trig_sel ds_trig_sel;
};

struct hwregTrigGenMLDlyIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	uint16_t ml_dly_v;
	enum drm_hwreg_triggen_trig_sel ml_trig_sel;
};

struct hwregTrigGenVSDlyIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	uint16_t vs_dly_h;
	uint16_t vs_dly_v;
	enum drm_hwreg_triggen_trig_sel vs_trig_sel;
};

struct hwregTrigGenDMARDlyIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	uint16_t dma_r_dly_h;
	uint16_t dma_r_dly_v;
	enum drm_hwreg_triggen_trig_sel dmar_trig_sel;
};

struct hwregTrigGenSTRDlyIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	uint16_t str_dly_v;
	enum drm_hwreg_triggen_trig_sel str_trig_sel;
};

struct hwregTrigGenSWIRQDlyIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	uint16_t sw_irq_dly_v;
	enum drm_hwreg_triggen_trig_sel swirq_trig_sel;
};

struct hwregTrigGenPQIRQDlyIn{
	bool RIU;
	enum drm_hwreg_triggen_domain domain;
	uint16_t pq_irq_dly_v;
	enum drm_hwreg_triggen_trig_sel pqirq_trig_sel;
};

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
int drv_hwreg_render_common_triggen_set_inputSrcSel(
	struct hwregTrigGenInputSrcSelIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_commonTrigSel(
	struct hwregTrigGenCommonTrigSelIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_trigSelSep(
	struct hwregTrigGenTrigSelSepIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_vsSwTrig(
	struct hwregTrigGenVsSwTrigIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_swHcntIniVal(
	struct hwregTrigGenSwHcntIniValIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_swHttSize(
	struct hwregTrigGenSwHttSizeIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_swUserMode(
	struct hwregTrigGenSwUserModeIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_vcntKeepEnable(
	struct hwregTrigGenVcntKeepEnIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_rwbankTrig(
	struct hwregTrigGenRWbankDlyIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_dsTrig(
	struct hwregTrigGenDSDlyIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_mlTrig(
	struct hwregTrigGenMLDlyIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_vsyncTrig(
	struct hwregTrigGenVSDlyIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_dmaRdTrig(
	struct hwregTrigGenDMARDlyIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_strTrig(
	struct hwregTrigGenSTRDlyIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_swIrqTrig(
	struct hwregTrigGenSWIRQDlyIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_common_triggen_set_pqIrqTrig(
	struct hwregTrigGenPQIRQDlyIn paramIn,
	struct hwregOut *paramOut);

#endif // _HWREG_RENDER_COMMON_TRIGGER_GEN_H_

