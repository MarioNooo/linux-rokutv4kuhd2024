/*
 *
 * Copyright (c) 2020-2024 MediaTek Inc.
 * All rights reserved.
 *
 * Unless otherwise stipulated in writing, any and all information contained
 * herein regardless in any format shall remain the sole proprietary of
 * MediaTek Inc. and be kept in strict confidence
 * ("MediaTek Confidential Information") by the recipient.
 * Any unauthorized act including without limitation unauthorized disclosure,
 * copying, use, reproduction, sale, distribution, modification, disassembling,
 * reverse engineering and compiling of the contents of MediaTek Confidential
 * Information is unlawful and strictly prohibited. MediaTek hereby reserves the
 * rights to any and all damages, losses, costs and expenses resulting therefrom.
 *
 */

/*
 *
 * @author Mediatek Inc.
 *
 */

#ifndef _HWREG_PQ_COMMON_TRIGGER_GEN_H_
#define _HWREG_PQ_COMMON_TRIGGER_GEN_H_

#include "hwreg_common.h"

/* ---------------------------------------------------------------------------------------------- */
/* 	Define											  */
/* ---------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------- */
/* 	Macro											  */
/* ---------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------- */
/* 	Enum											  */
/* ---------------------------------------------------------------------------------------------- */
enum pq_common_triggen_domain {
	PQ_COMMON_TRIGEN_DOMAIN_B2R = 0,
	PQ_COMMON_TRIGEN_DOMAIN_IP,
	PQ_COMMON_TRIGEN_DOMAIN_OP1,
	PQ_COMMON_TRIGEN_DOMAIN_OP2,
	PQ_COMMON_TRIGEN_DOMAIN_B2RLITE0,
	PQ_COMMON_TRIGEN_DOMAIN_B2RLITE1,
	PQ_COMMON_TRIGEN_DOMAIN_FRC_IPM,
	PQ_COMMON_TRIGEN_DOMAIN_MAX,
};
enum pq_common_triggen_src_sel {
	PQ_COMMON_TRIGEN_INPUT_IP0,
	PQ_COMMON_TRIGEN_INPUT_IP1,
	PQ_COMMON_TRIGEN_INPUT_B2R0,
	PQ_COMMON_TRIGEN_INPUT_B2R1,
	PQ_COMMON_TRIGEN_INPUT_TGEN,
	PQ_COMMON_TRIGEN_INPUT_MAX,
};
enum pq_common_triggen_trig_sel {
	PQ_COMMON_TRIGEN_TRIG_SEL_VS = 0,
	PQ_COMMON_TRIGEN_TRIG_SEL_VFDE,
	PQ_COMMON_TRIGEN_TRIG_SEL_MAX,
};

/* ---------------------------------------------------------------------------------------------- */
/*	Structure										  */
/* ---------------------------------------------------------------------------------------------- */
struct pq_common_triggen_input_src_sel {
	enum pq_common_triggen_domain domain;
	enum pq_common_triggen_src_sel src;
};

struct pq_common_triggen_common_trig_sel{
	enum pq_common_triggen_domain domain;
	enum pq_common_triggen_trig_sel trig_sel;
};

struct pq_common_triggen_trig_sel_sep{
	enum pq_common_triggen_domain domain;
	bool trig_sel_separate;
};

struct pq_common_triggen_vs_sw_trig{
	enum pq_common_triggen_domain domain;
	bool sw_trig_mode;
};

struct pq_common_triggen_sw_hcnt_ini_val{
	enum pq_common_triggen_domain domain;
	uint16_t sw_hcnt;
};

struct pq_common_triggen_sw_htt_size{
	enum pq_common_triggen_domain domain;
	uint16_t sw_htt;
};

struct pq_common_triggen_sw_user_mode{
	enum pq_common_triggen_domain domain;
	bool sw_user_mode_h;
	bool sw_user_mode_v;
};

struct pq_common_triggen_vcnt_keep_enable{
	enum pq_common_triggen_domain domain;
	bool vcnt_keep_en;
	uint8_t vcnt_upd_mask_range;
};

struct pq_common_triggen_rwbank_dly{
	enum pq_common_triggen_domain domain;
	uint16_t RWBk_ud_dly_h;
	uint16_t RWBk_ud_dly_v;
	enum pq_common_triggen_trig_sel rwbank_trig_sel;
};

struct pq_common_triggen_ds_dly{
	enum pq_common_triggen_domain domain;
	uint16_t ds_dly_v;
	enum pq_common_triggen_trig_sel ds_trig_sel;
};

struct pq_common_triggen_ml_dly{
	enum pq_common_triggen_domain domain;
	uint16_t ml_dly_v;
	enum pq_common_triggen_trig_sel ml_trig_sel;
};

struct pq_common_triggen_vs_dly{
	enum pq_common_triggen_domain domain;
	uint16_t vs_dly_h;
	uint16_t vs_dly_v;
	enum pq_common_triggen_trig_sel vs_trig_sel;
};

struct pq_common_triggen_dma_r_dly{
	enum pq_common_triggen_domain domain;
	uint16_t dma_r_dly_h;
	uint16_t dma_r_dly_v;
	enum pq_common_triggen_trig_sel dmar_trig_sel;
};

struct pq_common_triggen_str_dly{
	enum pq_common_triggen_domain domain;
	uint16_t str_dly_v;
	enum pq_common_triggen_trig_sel str_trig_sel;
};

struct pq_common_triggen_sw_irq_dly{
	enum pq_common_triggen_domain domain;
	uint16_t sw_irq_dly_v;
	enum pq_common_triggen_trig_sel swirq_trig_sel;
};

struct pq_common_triggen_pq_irq_dly{
	enum pq_common_triggen_domain domain;
	uint16_t pq_irq_dly_v;
	enum pq_common_triggen_trig_sel pqirq_trig_sel;
};

/* ---------------------------------------------------------------------------------------------- */
/*	Function										  */
/* ---------------------------------------------------------------------------------------------- */
int drv_hwreg_pq_common_triggen_set_inputSrcSel(
	struct pq_common_triggen_input_src_sel paramIn,
	bool riu,
	struct hwregOut *out,
	bool stub);

int drv_hwreg_pq_common_triggen_set_commonTrigSel(
	struct pq_common_triggen_common_trig_sel paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_trigSelSep(
	struct pq_common_triggen_trig_sel_sep paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_vsSwTrig(
	struct pq_common_triggen_vs_sw_trig paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_swHcntIniVal(
	struct pq_common_triggen_sw_hcnt_ini_val paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_swHttSize(
	struct pq_common_triggen_sw_htt_size paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_swUserMode(
	struct pq_common_triggen_sw_user_mode paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_vcntKeepEnable(
	struct pq_common_triggen_vcnt_keep_enable paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_rwbankTrig(
	struct pq_common_triggen_rwbank_dly paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_dsTrig(
	struct pq_common_triggen_ds_dly paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_mlTrig(
	struct pq_common_triggen_ml_dly paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_vsyncTrig(
	struct pq_common_triggen_vs_dly paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_dmaRdTrig(
	struct pq_common_triggen_dma_r_dly paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_strTrig(
	struct pq_common_triggen_str_dly paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_swIrqTrig(
	struct pq_common_triggen_sw_irq_dly paramIn,
	bool riu,
	struct hwregOut *out);

int drv_hwreg_pq_common_triggen_set_pqIrqTrig(
	struct pq_common_triggen_pq_irq_dly paramIn,
	bool riu,
	struct hwregOut *out);

#endif // _HWREG_PQ_COMMON_TRIGGER_GEN_H_
