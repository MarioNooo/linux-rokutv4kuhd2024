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

#ifndef _HWREG_RENDER_VIDEO_GLOBALPQ_H_
#define _HWREG_RENDER_VIDEO_GLOBALPQ_H_

#include "hwreg_common.h"
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


enum drm_hwreg_video_ldm_support_type {
	E_LDM_SUPPORT_TYPE_UNSUPPORT = 0,
	E_LDM_SUPPORT_TYPE_TRUNK,
	E_LDM_SUPPORT_TYPE_CUS,
	E_LDM_SUPPORT_TYPEE_MAX,
};

enum drm_hwreg_video_ldm_sw_set_ctrl_type {
	E_LDM_SW_LDC_BYPASS = 0,
	E_LDM_SW_LDC_PATH_SEL,
	E_LDM_SW_LDC_XIU2AHB_SEL0,
	E_LDM_SW_LDC_XIU2AHB_SEL1,
	E_LDM_SW_RESET_LDC_RST,
	E_LDM_SW_RESET_LDC_MRST,
	E_LDM_SW_RESET_LDC_AHB,
	E_LDM_SW_RESET_LDC_AHB_WP,
	E_LDM_SW_SET_LD_SUPPORT,
	E_LDM_SW_SET_CUS_PROFILE,
	E_LDM_SW_SET_TMON_DEBUG,
};

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

struct hwregLDCIn {
	bool RIU;
	enum drm_hwreg_video_ldm_support_type enLDMSupportType;
};


struct hwregSWSetCtrlIn {
	bool RIU;
	enum drm_hwreg_video_ldm_sw_set_ctrl_type enLDMSwSetCtrlType;
	uint8_t u8Value;
};

struct ldmDatebaseIn {
	uint64_t srcAddr;
	uint64_t dstAddr;
	uint32_t dataSize;
};

struct hwregAHBIn {
	bool RIU;
	uint32_t u32AHBReg;
	uint32_t u32AHBData;
	uint8_t u8BitStart;
	uint8_t u8BitLength;
};

struct ldmXIU2AHBSelIn {
	bool xiu2ahb_sel0;
	bool xiu2ahb_sel1;
};

struct hwregSetGlobalPQIn{
	char* globalPQ;
};

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

int drv_hwreg_render_ldm_set_LDC(
	struct hwregLDCIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_ldm_set_swsetctrl(
	struct hwregSWSetCtrlIn paramIn,
	struct hwregOut *paramOut);

void drv_hwreg_render_ldm_update_database(
	struct ldmDatebaseIn paramIn,
	struct hwregWriteMethodInfo *writeInfo);
void drv_hwreg_render_ldm_write_ahb0(
	struct hwregAHBIn paramIn,
	struct hwregOut *paramOut);

void drv_hwreg_render_ldm_write_ahb1(
	struct hwregAHBIn paramIn,
	struct hwregOut *paramOut);

void drv_hwreg_render_ldm_read_ahb0(
	struct hwregAHBIn paramIn,
	struct hwregOut *paramOut);

void drv_hwreg_render_ldm_read_ahb1(
	struct hwregAHBIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_global_pq(
	struct hwregSetGlobalPQIn paramIn);

/*A3E2 STAT*/
uint16_t drv_hwreg_render_ldm_get_stat_bin(uint8_t u8Idx, uint8_t u8ld_version);

/*A3E3 LDM*/
uint16_t drv_hwreg_render_ldm_get_pre_width(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_pre_height(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_width(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_height(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_lpf_w_pack_md(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_lpf_2byte_pack_md(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_lpf_data_out_sel(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_accu_h_fac_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_accu_h_fac_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_accu_v_fac_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_accu_v_fac_1(uint8_t u8ld_version);

/*edge2d10K*/
uint16_t drv_hwreg_render_ldm_get_edge2d_10k_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_edge2d_10k_bcorrect12b(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_edge2d_10k_led_width(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_edge2d_10k_led_height(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_edge2d_10k_ref_width(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_edge2d_10k_ref_height(uint8_t u8ld_version);

/*LD1*/
uint16_t drv_hwreg_render_ldm_get_local_dimming_en(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_local_dimming_en(uint8_t u8ld_version, bool bEn);

uint16_t drv_hwreg_render_ldm_get_ldf_addr_idx_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_wbank_soft_mode(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldf_bl_width(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldf_bl_height(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_alpha(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldf_10bit_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldf_8byte_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_write_dc_max_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_write_dc_max_of_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_write_wsum_dc_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldf_10bit_pack_md(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldb_4byte_pack_md(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldb_8byte_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ratio_h_lsf_cbmode(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ratio_v_lsf_cbmode(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_cmp_blend(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_r_comp_lut(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_addr_idx_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_read_addr_idx_r(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_read_addr_idx(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_rbank_soft_mode(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_comp_en(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_comp_en(uint8_t u8ld_version, bool bEn);

uint16_t drv_hwreg_render_ldm_get_bl_width(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_bl_height(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_bl_width_led(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_bl_height_led(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_frm_width(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_frm_height(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_lsf_out_mod(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldf_baseaddr0_l_0(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_ldf_baseaddr0_l_0(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_ldf_baseaddr0_l_1(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_ldf_baseaddr0_l_1(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_ldf_baseaddr1_l_0(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_ldf_baseaddr1_l_0(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_ldf_baseaddr1_l_1(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_ldf_baseaddr1_l_1(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_ldf_baseaddr0_r_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldf_baseaddr0_r_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldf_baseaddr1_r_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ldf_baseaddr1_r_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ratio_h_in_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ratio_h_in_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ratio_v_in_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ratio_v_in_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ratio_h_out_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ratio_h_out_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ratio_v_out_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ratio_v_out_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_baseaddr0_l_0(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_baseaddr0_l_0(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_baseaddr0_l_1(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_baseaddr0_l_1(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_baseaddr1_l_0(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_baseaddr1_l_0(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_baseaddr1_l_1(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_baseaddr1_l_1(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_baseaddr0_r_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_baseaddr0_r_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_baseaddr1_r_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_baseaddr1_r_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_edge_2d_baseaddr_0(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_edge_2d_baseaddr_0(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_edge_2d_baseaddr_1(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_edge_2d_baseaddr_1(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_baseaddr_offset_0(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_baseaddr_offset_0(uint8_t u8ld_version, uint16_t u16Data);

uint16_t drv_hwreg_render_ldm_get_baseaddr_offset_1(uint8_t u8ld_version);

void drv_hwreg_render_ldm_set_baseaddr_offset_1(uint8_t u8ld_version, uint16_t u16Data);
/*LD2*/
uint16_t drv_hwreg_render_ldm_get_spi_indpt_read_addr_idx(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_r_comp_lut_2(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ld_stat_miu_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_statistic_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_hist_range_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ld_stat_baseidx(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_total_pixel_weight_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_total_pixel_weight_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_total_pixel_count_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_total_pixel_count_1(uint8_t u8ld_version);

/*LD3*/
uint16_t drv_hwreg_render_ldm_get_mura_comp_gain_en(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_mcg_width(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_mcg_height(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_muragain_rdata(uint8_t u8Idx, uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_mura_comp_gain_hratio_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_mura_comp_gain_hratio_1(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_mura_comp_gain_vratio_0(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_mura_comp_gain_vratio_1(uint8_t u8ld_version);

/*LD4*/
uint16_t drv_hwreg_render_ldm_get_spi_is_done(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_spi_is_busy(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_spi_dma_done(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_spi_base_offset_idx_done(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_spi_base_idx_done(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_spi_is_busy_detail(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_spi_is_busy_err(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_ld_dma_sw_md_when_busy(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_spi_base_idx_done_raw(uint8_t u8ld_version);

uint16_t drv_hwreg_render_ldm_get_spi_base_offset_idx_done_raw(uint8_t u8ld_version);

/*LD5*/
uint16_t drv_hwreg_render_ldm_get_oled_b2p_rdata(uint8_t u8Idx, uint8_t u8ld_version);

/*TGEN*/
uint16_t drv_hwreg_render_ldm_get_tgen_v(uint8_t u8ld_version);
uint16_t drv_hwreg_render_ldm_get_tgen_h(uint8_t u8ld_version);
/*DMA*/
void drv_hwreg_render_ldm_bl_depack16b_mod(uint8_t u8ld_version, bool bEn);

void drv_hwreg_render_ldm_set_spi_new_path(uint8_t u8ld_version, bool bEn);

void drv_hwreg_render_ldm_set_spi_new_path_data_inv(uint8_t u8ld_version, bool bEn);

void drv_hwreg_render_ldm_set_spi_self_trig_offset(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_bl_width_dma(uint8_t u8ld_version, uint8_t u8Data);

void drv_hwreg_render_ldm_set_bl_height_dma(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_bldmar2_hsize_0(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_bldmar2_vsize_0(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_bldmar2_bit_mode_0(uint8_t u8ld_version, uint8_t u8Data);

void drv_hwreg_render_ldm_set_bdmar2_ip_setting_mode_base_en(uint8_t u8ld_version, bool bEn);

void drv_hwreg_render_ldm_set_bldmar2_tr_delay_h_1(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_bldmar2_tr_delay_v_1(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_dma_en(uint8_t u8ld_version, uint8_t u8Idx, bool bEn);

void drv_hwreg_render_ldm_set_miu_pack_offset(uint8_t u8ld_version, uint8_t u8Idx, uint16_t u16Data);

void drv_hwreg_render_ldm_set_miu_pack_length(uint8_t u8ld_version, uint8_t u8Idx, uint16_t u16Data);

void drv_hwreg_render_ldm_set_dma_yoff_end(uint8_t u8ld_version, uint8_t u8Idx, uint16_t u16Data);

void drv_hwreg_render_ldm_set_max_num_spi(uint8_t u8ld_version, uint8_t u8Idx, uint32_t u32Data);
/*PWM*/
void drv_hwreg_render_ldm_set_pwm_clken_bitmap(uint8_t u8ld_version, uint8_t u8Data);

void drv_hwreg_render_ldm_set_pwm0_vdben(uint8_t u8ld_version, bool bEn);

void drv_hwreg_render_ldm_set_pwm0_reset_en(uint8_t u8ld_version, bool bEn);

void drv_hwreg_render_ldm_set_pwm0_vdben_sw(uint8_t u8ld_version, bool bEn);

void drv_hwreg_render_ldm_set_pwm0_period_0(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_pwm0_period_1(uint8_t u8ld_version, uint8_t u8Data);

void drv_hwreg_render_ldm_set_pwm0_duty_0(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_pwm0_duty_1(uint8_t u8ld_version, uint8_t u8Data);

void drv_hwreg_render_ldm_set_ld_vsync_like_st_0(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_ld_vsync_like_st_1(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_ld_vsync_like_end_0(uint8_t u8ld_version, uint16_t u16Data);

void drv_hwreg_render_ldm_set_ld_vsync_like_end_1(uint8_t u8ld_version, uint16_t u16Data);

/*ADL*/
void drv_hwreg_render_ldm_adl_set_comp_dump_debug_mode(bool bEn);
void drv_hwreg_render_ldm_adl_set_mcg_dump_debug_mode(bool bEn);
void drv_hwreg_render_ldm_adl_set_oled_dump_debug_mode(bool bEn);
void drv_hwreg_render_ldm_adl_set_comp1_dump_amod(bool bEn);
void drv_hwreg_render_ldm_adl_set_comp2_dump_amod(bool bEn);
void drv_hwreg_render_ldm_adl_set_mcg_dump_amod(bool bEn);
void drv_hwreg_render_ldm_adl_set_oled_dump_amod(bool bEn);
void drv_hwreg_render_ldm_adl_set_comp1_wd(bool bEn);
void drv_hwreg_render_ldm_adl_set_comp2_wd(bool bEn);
void drv_hwreg_render_ldm_adl_set_mcg_wd(bool bEn);
void drv_hwreg_render_ldm_adl_set_oled_wd(bool bEn);
void drv_hwreg_render_ldm_adl_set_comp1_dump_rd(bool bEn);
void drv_hwreg_render_ldm_adl_set_comp2_dump_rd(bool bEn);
void drv_hwreg_render_ldm_adl_set_mcg_dump_rd(bool bEn);
void drv_hwreg_render_ldm_adl_set_oled_dump_rd(bool bEn);
void drv_hwreg_render_ldm_adl_set_comp1_dump_addr(uint8_t u8Addr);
void drv_hwreg_render_ldm_adl_set_comp2_dump_addr(uint8_t u8Addr);
void drv_hwreg_render_ldm_adl_set_mcg_dump_addr(uint8_t u8Addr);
void drv_hwreg_render_ldm_adl_set_oled_dump_addr(uint8_t u8Addr);
void drv_hwreg_render_ldm_adl_comp_set_enable(bool bEn);
void drv_hwreg_render_ldm_adl_mcg_set_enable(bool bEn);
void drv_hwreg_render_ldm_adl_oled_set_enable(bool bEn);
void drv_hwreg_render_ldm_adl_set_power_enable(bool bEn);
void drv_hwreg_render_ldm_adl_set_comp_low_gain(bool bEn);
void drv_hwreg_render_ldm_adl_set_mcg_width(uint8_t u8Size);
void drv_hwreg_render_ldm_adl_set_mcg_height(uint8_t u8Size);
uint8_t drv_hwreg_render_ldm_adl_get_mcg_width(void);
uint8_t drv_hwreg_render_ldm_adl_get_mcg_height(void);
void drv_hwreg_render_ldm_adl_set_mcg_size(uint8_t u8Size, uint8_t u8OLED_Addr);
void drv_hwreg_render_ldm_adl_set_comp_write_table1(uint16_t u16Data);
void drv_hwreg_render_ldm_adl_set_comp_write_table2(uint16_t u16Data);
uint32_t drv_hwreg_render_ldm_adl_comp_dump_table1(void);
uint32_t drv_hwreg_render_ldm_adl_comp_dump_table2(void);
void drv_hwreg_render_ldm_adl_mcg_write_table(uint8_t u8Idx, uint16_t u16Data);
uint16_t drv_hwreg_render_ldm_adl_mcg_dump_table(uint8_t u8Idx);
uint16_t drv_hwreg_render_ldm_adl_oled_dump_table(uint8_t u8Idx);
void drv_hwreg_render_ldm_aul_swrst(void);
void drv_hwreg_render_ldm_aul_init(uint8_t u8Ver);
void drv_hwreg_render_ldm_aul_set_logo_x_start_left(uint16_t u16Data);
void drv_hwreg_render_ldm_aul_set_logo_x_start_right(uint16_t u16Data);
void drv_hwreg_render_ldm_aul_set_logo_y_start_up(uint16_t u16Data);
void drv_hwreg_render_ldm_aul_set_logo_y_start_down(uint16_t u16Data);

uint32_t drv_hwreg_render_ldm_get_bytes(uint32_t addr,  uint32_t mask);
#endif // _HWREG_RENDER_VIDEO_GLOBALPQ_H_
