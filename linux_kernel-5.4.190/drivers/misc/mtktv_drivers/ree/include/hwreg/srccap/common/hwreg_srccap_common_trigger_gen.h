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

#ifndef _HWREG_SRCCAP_COMMON_TRIGGER_GEN_H_
#define _HWREG_SRCCAP_COMMON_TRIGGER_GEN_H_


/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
enum reg_srccap_common_triggen_domain {
	SRCCAP_COMMON_TRIGEN_DOMAIN_SRC0 = 0,
	SRCCAP_COMMON_TRIGEN_DOMAIN_SRC1,
	SRCCAP_COMMON_TRIGEN_DOMAIN_MAX,
};

enum reg_srccap_common_triggen_src_sel {
	SRCCAP_COMMON_TRIGEN_INPUT_IP0 = 0,
	SRCCAP_COMMON_TRIGEN_INPUT_IP1,
	SRCCAP_COMMON_TRIGEN_INPUT_B2R0,
	SRCCAP_COMMON_TRIGEN_INPUT_B2R1,
	SRCCAP_COMMON_TRIGEN_INPUT_TGEN,
	SRCCAP_COMMON_TRIGEN_INPUT_MAX,
};

enum reg_srccap_common_triggen_trig_sel {
	SRCCAP_COMMON_TRIGEN_TRIG_SEL_VS = 0,
	SRCCAP_COMMON_TRIGEN_TRIG_SEL_VFDE,
	SRCCAP_COMMON_TRIGEN_TRIG_SEL_MAX,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */
struct reg_srccap_common_triggen_input_src_sel {
	enum reg_srccap_common_triggen_domain domain;
	enum reg_srccap_common_triggen_src_sel src;
};

struct reg_srccap_common_triggen_common_trig_sel{
	enum reg_srccap_common_triggen_domain domain;
	enum reg_srccap_common_triggen_trig_sel trig_sel;
};

struct reg_srccap_common_triggen_trig_sel_sep{
	enum reg_srccap_common_triggen_domain domain;
	bool trig_sel_separate;
};

struct reg_srccap_common_triggen_vs_sw_trig{
	enum reg_srccap_common_triggen_domain domain;
	bool sw_trig_mode;
};

struct reg_srccap_common_triggen_sw_hcnt_ini_val{
	enum reg_srccap_common_triggen_domain domain;
	uint16_t sw_hcnt;
};

struct reg_srccap_common_triggen_sw_htt_size{
	enum reg_srccap_common_triggen_domain domain;
	uint16_t sw_htt;
};

struct reg_srccap_common_triggen_sw_user_mode{
	enum reg_srccap_common_triggen_domain domain;
	bool sw_user_mode_h;
	bool sw_user_mode_v;
};

struct reg_srccap_common_triggen_vcnt_keep_enable{
	enum reg_srccap_common_triggen_domain domain;
	bool vcnt_keep_en;
	uint8_t vcnt_upd_mask_range;
};

struct reg_srccap_common_triggen_rwbank_dly{
	enum reg_srccap_common_triggen_domain domain;
	uint16_t RWBk_ud_dly_h;
	uint16_t RWBk_ud_dly_v;
	enum reg_srccap_common_triggen_trig_sel rwbank_trig_sel;
};

struct reg_srccap_common_triggen_ds_dly{
	enum reg_srccap_common_triggen_domain domain;
	uint16_t ds_dly_v;
	enum reg_srccap_common_triggen_trig_sel ds_trig_sel;
};

struct reg_srccap_common_triggen_ml_dly{
	enum reg_srccap_common_triggen_domain domain;
	uint16_t ml_dly_v;
	enum reg_srccap_common_triggen_trig_sel ml_trig_sel;
};

struct reg_srccap_common_triggen_vs_dly{
	enum reg_srccap_common_triggen_domain domain;
	uint16_t vs_dly_h;
	uint16_t vs_dly_v;
	enum reg_srccap_common_triggen_trig_sel vs_trig_sel;
};

struct reg_srccap_common_triggen_dma_r_dly{
	enum reg_srccap_common_triggen_domain domain;
	uint16_t dma_r_dly_h;
	uint16_t dma_r_dly_v;
	enum reg_srccap_common_triggen_trig_sel dmar_trig_sel;
};

struct reg_srccap_common_triggen_str_dly{
	enum reg_srccap_common_triggen_domain domain;
	uint16_t str_dly_v;
	enum reg_srccap_common_triggen_trig_sel str_trig_sel;
};

struct reg_srccap_common_triggen_sw_irq_dly{
	enum reg_srccap_common_triggen_domain domain;
	uint16_t sw_irq_dly_v;
	enum reg_srccap_common_triggen_trig_sel swirq_trig_sel;
};

struct reg_srccap_common_triggen_pq_irq_dly{
	enum reg_srccap_common_triggen_domain domain;
	uint16_t pq_irq_dly_v;
	enum reg_srccap_common_triggen_trig_sel pqirq_trig_sel;
};

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
int drv_hwreg_srccap_common_triggen_set_inputSrcSel(
	struct reg_srccap_common_triggen_input_src_sel paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_commonTrigSel(
	struct reg_srccap_common_triggen_common_trig_sel paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_trigSelSep(
	struct reg_srccap_common_triggen_trig_sel_sep paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_vsSwTrig(
	struct reg_srccap_common_triggen_vs_sw_trig paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_swHcntIniVal(
	struct reg_srccap_common_triggen_sw_hcnt_ini_val paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_swHttSize(
	struct reg_srccap_common_triggen_sw_htt_size paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_swUserMode(
	struct reg_srccap_common_triggen_sw_user_mode paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_vcntKeepEnable(
	struct reg_srccap_common_triggen_vcnt_keep_enable paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_rwbankTrig(
	struct reg_srccap_common_triggen_rwbank_dly paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_dsTrig(
	struct reg_srccap_common_triggen_ds_dly paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_mlTrig(
	struct reg_srccap_common_triggen_ml_dly paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_vsyncTrig(
	struct reg_srccap_common_triggen_vs_dly paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_dmaRdTrig(
	struct reg_srccap_common_triggen_dma_r_dly paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_strTrig(
	struct reg_srccap_common_triggen_str_dly paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_swIrqTrig(
	struct reg_srccap_common_triggen_sw_irq_dly paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_common_triggen_set_pqIrqTrig(
	struct reg_srccap_common_triggen_pq_irq_dly paramIn,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

#endif // _HWREG_SRCCAP_COMMON_TRIGGER_GEN_H_

