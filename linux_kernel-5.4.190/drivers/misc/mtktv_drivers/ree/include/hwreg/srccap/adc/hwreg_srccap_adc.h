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

#ifndef _HWREG_SRCCAP_ADC_H_
#define _HWREG_SRCCAP_ADC_H_

#include "hwreg_common.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */
#define CLK_NAME_MAX_LENGTH (50)
#define CLK_DATA_MAX_LENGTH (50)

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
enum reg_srccap_adc_input_port {
	REG_SRCCAP_ADC_INPUT_PORT_NONE = 0,
	REG_SRCCAP_ADC_INPUT_PORT_RGB0_DATA = 10,
	REG_SRCCAP_ADC_INPUT_PORT_RGB1_DATA,
	REG_SRCCAP_ADC_INPUT_PORT_RGB2_DATA,
	REG_SRCCAP_ADC_INPUT_PORT_RGB0_SYNC = 30,
	REG_SRCCAP_ADC_INPUT_PORT_RGB1_SYNC,
	REG_SRCCAP_ADC_INPUT_PORT_RGB2_SYNC,
	REG_SRCCAP_ADC_INPUT_PORT_YCVBS0 = 50,
	REG_SRCCAP_ADC_INPUT_PORT_YCVBS1,
	REG_SRCCAP_ADC_INPUT_PORT_YCVBS2,
	REG_SRCCAP_ADC_INPUT_PORT_YCVBS3,
	REG_SRCCAP_ADC_INPUT_PORT_YG0 = 60,
	REG_SRCCAP_ADC_INPUT_PORT_YG1,
	REG_SRCCAP_ADC_INPUT_PORT_YG2,
	REG_SRCCAP_ADC_INPUT_PORT_YR0,
	REG_SRCCAP_ADC_INPUT_PORT_YR1,
	REG_SRCCAP_ADC_INPUT_PORT_YR2,
	REG_SRCCAP_ADC_INPUT_PORT_YB0,
	REG_SRCCAP_ADC_INPUT_PORT_YB1,
	REG_SRCCAP_ADC_INPUT_PORT_YB2,
	REG_SRCCAP_ADC_INPUT_PORT_CCVBS0 = 70,
	REG_SRCCAP_ADC_INPUT_PORT_CCVBS1,
	REG_SRCCAP_ADC_INPUT_PORT_CCVBS2,
	REG_SRCCAP_ADC_INPUT_PORT_CCVBS3,
	REG_SRCCAP_ADC_INPUT_PORT_CG0 = 80,
	REG_SRCCAP_ADC_INPUT_PORT_CG1,
	REG_SRCCAP_ADC_INPUT_PORT_CG2,
	REG_SRCCAP_ADC_INPUT_PORT_CR0,
	REG_SRCCAP_ADC_INPUT_PORT_CR1,
	REG_SRCCAP_ADC_INPUT_PORT_CR2,
	REG_SRCCAP_ADC_INPUT_PORT_CB0,
	REG_SRCCAP_ADC_INPUT_PORT_CB1,
	REG_SRCCAP_ADC_INPUT_PORT_CB2,
	REG_SRCCAP_ADC_INPUT_PORT_DVI0 = 90,
	REG_SRCCAP_ADC_INPUT_PORT_DVI1,
	REG_SRCCAP_ADC_INPUT_PORT_DVI2,
	REG_SRCCAP_ADC_INPUT_PORT_DVI3,
	REG_SRCCAP_ADC_INPUT_PORT_NUM,
};

enum reg_srccap_adc_input_offline_mux {
	REG_SRCCAP_ADC_INPUT_OFFLINE_MUX_CVBS0 = 0,
	REG_SRCCAP_ADC_INPUT_OFFLINE_MUX_CVBS1,
	REG_SRCCAP_ADC_INPUT_OFFLINE_MUX_R0,
	REG_SRCCAP_ADC_INPUT_OFFLINE_MUX_R1,
	REG_SRCCAP_ADC_INPUT_OFFLINE_MUX_G0,
	REG_SRCCAP_ADC_INPUT_OFFLINE_MUX_G1,
	REG_SRCCAP_ADC_INPUT_OFFLINE_MUX_B0,
	REG_SRCCAP_ADC_INPUT_OFFLINE_MUX_B1,
	REG_SRCCAP_ADC_INPUT_OFFLINE_MUX_NUM,
};

enum reg_srccap_adc_input_source_type {
	// None ADC source
	REG_SRCCAP_ADC_INPUTSOURCE_NONE = 0x00,

	// Single ADC source
	REG_SRCCAP_ADC_INPUTSOURCE_ONLY_RGB = 0x01,
	REG_SRCCAP_ADC_INPUTSOURCE_ONLY_YPBPR = 0x02,
	REG_SRCCAP_ADC_INPUTSOURCE_ONLY_INT_DMD_ATV = 0x04,
	REG_SRCCAP_ADC_INPUTSOURCE_ONLY_EXT_DMD_ATV = 0x08,
	REG_SRCCAP_ADC_INPUTSOURCE_ONLY_SVIDEO = 0x10,
	REG_SRCCAP_ADC_INPUTSOURCE_ONLY_SCART_RGB = 0x20,
	REG_SRCCAP_ADC_INPUTSOURCE_ONLY_SCART_Y = 0x40,
	REG_SRCCAP_ADC_INPUTSOURCE_ONLY_CVBS = 0x80,

	// Multi ADC source
	REG_SRCCAP_ADC_INPUTSOURCE_MULTI_RGB_ATV =
		(REG_SRCCAP_ADC_INPUTSOURCE_ONLY_RGB | REG_SRCCAP_ADC_INPUTSOURCE_ONLY_EXT_DMD_ATV),
	REG_SRCCAP_ADC_INPUTSOURCE_MULTI_RGB_SCART  =
		(REG_SRCCAP_ADC_INPUTSOURCE_ONLY_RGB | REG_SRCCAP_ADC_INPUTSOURCE_ONLY_SCART_Y),
	REG_SRCCAP_ADC_INPUTSOURCE_MULTI_RGB_CVBS =
		(REG_SRCCAP_ADC_INPUTSOURCE_ONLY_RGB | REG_SRCCAP_ADC_INPUTSOURCE_ONLY_CVBS),
	REG_SRCCAP_ADC_INPUTSOURCE_MULTI_YPBPR_ATV =
		(REG_SRCCAP_ADC_INPUTSOURCE_ONLY_YPBPR | REG_SRCCAP_ADC_INPUTSOURCE_ONLY_EXT_DMD_ATV),
	REG_SRCCAP_ADC_INPUTSOURCE_MULTI_YPBPR_SCART =
		(REG_SRCCAP_ADC_INPUTSOURCE_ONLY_YPBPR | REG_SRCCAP_ADC_INPUTSOURCE_ONLY_SCART_Y),
	REG_SRCCAP_ADC_INPUTSOURCE_MULTI_YPBPR_CVBS =
		(REG_SRCCAP_ADC_INPUTSOURCE_ONLY_YPBPR | REG_SRCCAP_ADC_INPUTSOURCE_ONLY_CVBS),

	// for all OFF cases
	REG_SRCCAP_ADC_INPUTSOURCE_OFF,
};

enum reg_srccap_adc_set_mode_type {
	REG_SRCCAP_ADC_SETMODETYPE_YUV,
	REG_SRCCAP_ADC_SETMODETYPE_RGB,
	REG_SRCCAP_ADC_SETMODETYPE_RGB_SOG,
	REG_SRCCAP_ADC_SETMODETYPE_YUV_Y,
	REG_SRCCAP_ADC_SETMODETYPE_NUM,
};

enum reg_srccap_adc_pws_type {
	// None ADC src
	REG_SRCCAP_ADC_PWS_NONE = 0,

	// Single ADC src power setting
	REG_SRCCAP_ADC_PWS_ATV_VIF,
	REG_SRCCAP_ADC_PWS_ATV_SSIF,
	REG_SRCCAP_ADC_PWS_CVBS,
	REG_SRCCAP_ADC_PWS_RGB,
	REG_SRCCAP_ADC_PWS_SCART,
	REG_SRCCAP_ADC_PWS_YPBPR,
	REG_SRCCAP_ADC_PWS_SVIDEO,

	// Multi ADC src power setting
	REG_SRCCAP_ADC_PWS_CVBS_RGB,
	REG_SRCCAP_ADC_PWS_CVBS_YPBPR,
	REG_SRCCAP_ADC_PWS_SCART_RGB,
	REG_SRCCAP_ADC_PWS_SCART_YPBPR,
	REG_SRCCAP_ADC_PWS_NUM,
};

enum reg_srccap_adc_sog_online_mux {
	REG_SRCCAP_ADC_SOG_ONLINE_GIN0P = 0,
	REG_SRCCAP_ADC_SOG_ONLINE_GIN1P,
	REG_SRCCAP_ADC_SOG_ONLINE_GIN2P,
	REG_SRCCAP_ADC_SOG_ONLINE_GND,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */
struct reg_srccap_adc_clk_info {
	char clk_name[CLK_NAME_MAX_LENGTH];
	char clk_data[CLK_DATA_MAX_LENGTH];
};

struct reg_srccap_adc_rgb_gain {
	uint16_t r_gain;
	uint16_t g_gain;
	uint16_t b_gain;
};

struct reg_srccap_adc_rgb_offset {
	uint16_t r_offset;
	uint16_t g_offset;
	uint16_t b_offset;
};

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
int drv_hwreg_srccap_adc_init(void);
int drv_hwreg_srccap_adc_set_freerun(bool enable);
int drv_hwreg_srccap_adc_set_porst(bool enable);
int drv_hwreg_srccap_adc_set_mux(
	enum reg_srccap_adc_input_port input_port);
int drv_hwreg_srccap_adc_set_source(
	enum reg_srccap_adc_input_source_type input_src);
int drv_hwreg_srccap_adc_set_rgb_cal(
	bool use_hw_cal,
	bool use_band_gap);
int drv_hwreg_srccap_adc_set_av_cal(void);
int drv_hwreg_srccap_adc_set_sv_cal(void);
int drv_hwreg_srccap_adc_set_mode(
	enum reg_srccap_adc_set_mode_type type,
	uint16_t pix_clk,
	uint16_t *iclamp_clk_rate,
	bool iclamp_delay_div_two_enable);
int drv_hwreg_srccap_adc_set_pws(
	enum reg_srccap_adc_pws_type pws_type);
int drv_hwreg_srccap_adc_get_clk_setting(
	enum reg_srccap_adc_input_source_type input_src,
	uint16_t max_clk_nums,
	struct reg_srccap_adc_clk_info *clk_info,
	uint16_t *clk_cnt);
int drv_hwreg_srccap_adc_get_clk_swen(
	enum reg_srccap_adc_input_source_type input_src,
	uint16_t max_clk_nums,
	struct reg_srccap_adc_clk_info *clk_info,
	uint16_t *clk_cnt);
int drv_hwreg_srccap_adc_get_scart_type(
	bool *is_scart_rgb);
int drv_hwreg_srccap_adc_get_rgb_gain(
	struct reg_srccap_adc_rgb_gain *gain);
int drv_hwreg_srccap_adc_set_rgb_gain(
	struct reg_srccap_adc_rgb_gain gain,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_adc_get_rgb_offset(
	struct reg_srccap_adc_rgb_offset *offset);
int drv_hwreg_srccap_adc_set_rgb_offset(
	struct reg_srccap_adc_rgb_offset offset,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_adc_get_phase(
	uint16_t *phase,
	uint16_t adcpllmod);
int drv_hwreg_srccap_adc_set_phase(
	uint16_t phase,
	uint16_t adcpllmod,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_adc_set_plldiv(
	uint16_t plldiv,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_adc_set_hpol(
	bool high_hpol,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_adc_get_isog_enable(
	bool *enable);
int drv_hwreg_srccap_adc_set_isog_reset_width(
	uint8_t width,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_adc_set_iclamp_rgb_powerdown(
	bool powerdown,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_adc_set_plla_reset(
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_adc_enable_offline_sog(
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_adc_set_offline_sog_mux(
	enum reg_srccap_adc_input_offline_mux src,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
int drv_hwreg_srccap_adc_get_adcpllmod(
	uint16_t *adcpllmod);
int drv_hwreg_srccap_adc_set_hw_version(
	uint32_t version);
int drv_hwreg_srccap_adc_set_sog_online_mux(
	enum reg_srccap_adc_sog_online_mux muxsel,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);
#endif // _HWREG_SRCCAP_ADC_H_
