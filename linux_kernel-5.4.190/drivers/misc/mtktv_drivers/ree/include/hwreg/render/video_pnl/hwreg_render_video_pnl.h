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

#ifndef _HWREG_RENDER_VIDEO_PNL_H_
#define _HWREG_RENDER_VIDEO_PNL_H_

#include "hwreg_common.h"
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define DRV_PNL_NAME_MAX_NUM (256)
#define DRV_CTRLBIT_MAX_NUM (32)
#define DRV_VBO_MAX_NUM (64)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef enum {
	REF_PULSE = 0,
	RW_BK_UPD_SRC0,
	RW_BK_UPD_SRC1,
	RW_BK_UPD_B2R,
	RW_BK_UPD_B2R_LITE0,
	RW_BK_UPD_B2R_LITE1,
	RW_BK_UPD_IP_IP_DMA_R,
	RW_BK_UPD_IP_PYRA_W,
	RW_BK_UPD_IP_PYRA_R,
	RW_BK_UPD_IP_IPM_W,
	RW_BK_UPD_OP1_OPM_R,
	RW_BK_UPD_OP1_UCM_W,
	RW_BK_UPD_OP1_UCM_R,
	RW_BK_UPD_OP2,
	RW_BK_UPD_DISP,
	RW_BK_UPD_TGEN,
	RW_BK_UPD_FRC1,
	RW_BK_UPD_FRC2,
} drm_en_vttv_source_type;

typedef enum {
	E_DRV_OUTPUT_MODE_NONE,
	E_DRV_8K4K_120HZ,
	E_DRV_8K4K_60HZ,
	E_DRV_8K4K_30HZ,
	E_DRV_4K2K_120HZ,
	E_DRV_4K2K_60HZ,
	E_DRV_4K2K_30HZ,
	E_DRV_FHD_120HZ,
	E_DRV_FHD_60HZ,
	E_DRV_4K2K_144HZ,
	E_DRV_OUTPUT_MODE_MAX,
} en_drv_pnl_output_mode;

typedef enum {
	E_DRV_VBO_NO_LINK = 0,
	E_DRV_VBO_3BYTE_MODE = 3,
	E_DRV_VBO_4BYTE_MODE = 4,
	E_DRV_VBO_5BYTE_MODE = 5,
	E_DRV_VBO_MODE_MAX,
} en_drv_pnl_vbo_bytemode;

typedef enum {
	E_DRV_LINK_IF_NONE,
	E_DRV_LINK_IF_VB1,
	E_DRV_LINK_IF_LVDS,
	E_DRV_LINK_IF_MAX,
} en_drv_pnl_link_if;

typedef enum {
	E_DRV_OUTPUT_RGB,
	E_DRV_OUTPUT_YUV444,
	E_DRV_OUTPUT_YUV422,
	E_DRV_OUTPUT_YUV420,
	E_DRV_OUTPUT_RGBA1010108,
	E_DRV_OUTPUT_RGBA8888,
	E_DRV_PNL_OUTPUT_FORMAT_NUM,
} en_drv_pnl_output_format;

typedef enum {
	E_DRV_TI_10BIT_MODE = 0,
	E_DRV_TI_8BIT_MODE = 2,
	E_DRV_TI_6BIT_MODE = 3,
} en_drv_pnl_tibitmode; //APIPNL_TIBITMODE;

/// Define panel output format bit mode
typedef enum {
	// default is 10bit, because 8bit panel can use 10bit config and
	// 8bit config.
	E_DRV_OUTPUT_10BIT_MODE = 0,
	// but 10bit panel(like PDP panel) can only use 10bit config.
	E_DRV_OUTPUT_6BIT_MODE = 1,
	// and some PDA panel is 6bit.
	E_DRV_OUTPUT_8BIT_MODE = 2,
} en_drv_pnl_outputformat_bitmode; // APIPNL_OUTPUTFORMAT_BITMODE;

typedef enum {
	///< TTL  type
	E_DRV_LINK_TTL,
	///< LVDS type
	E_DRV_LINK_LVDS,
	///< RSDS type
	E_DRV_LINK_RSDS,
	///< TCON
	E_DRV_LINK_MINILVDS,
	///< Analog TCON
	E_DRV_LINK_ANALOG_MINILVDS,
	///< Digital TCON
	E_DRV_LINK_DIGITAL_MINILVDS,
	///< Ursa (TTL output to Ursa)
	E_DRV_LINK_MFC,
	///< DAC output
	E_DRV_LINK_DAC_I,
	///< DAC output
	E_DRV_LINK_DAC_P,
	///< For PDP(Vsync use Manually MODE)
	E_DRV_LINK_PDPLVDS,
	/// EXT LPLL TYPE
	E_DRV_LINK_EXT,
	/// 10
	E_DRV_LINK_EPI34_8P = E_DRV_LINK_EXT,
	/// 11
	E_DRV_LINK_EPI28_8P,
	/// 12
	E_DRV_LINK_EPI34_6P,
	/// 13
	E_DRV_LINK_EPI28_6P,

	/// replace this with LINK_MINILVDS
	///LINK_MINILVDS_6P_2L,
	/// 14
	E_DRV_LINK_MINILVDS_5P_2L,
	/// 15
	E_DRV_LINK_MINILVDS_4P_2L,
	/// 16
	E_DRV_LINK_MINILVDS_3P_2L,
	/// 17
	E_DRV_LINK_MINILVDS_6P_1L,
	/// 18
	E_DRV_LINK_MINILVDS_5P_1L,
	/// 19
	E_DRV_LINK_MINILVDS_4P_1L,
	/// 20
	E_DRV_LINK_MINILVDS_3P_1L,

	/// 21
	E_DRV_LINK_HS_LVDS,
	/// 22
	E_DRV_LINK_HF_LVDS,

	/// 23
	E_DRV_LINK_TTL_TCON,
	//  2 channel, 3 pair, 8 bits ///24
	E_DRV_LINK_MINILVDS_2CH_3P_8BIT,
	//  2 channel, 4 pair, 8 bits ///25
	E_DRV_LINK_MINILVDS_2CH_4P_8BIT,
	// 2 channel, 5 pair, 8 bits ///26
	E_DRV_LINK_MINILVDS_2CH_5P_8BIT,
	// 2 channel, 6 pair, 8 bits ///27
	E_DRV_LINK_MINILVDS_2CH_6P_8BIT,

	// 1 channel, 3 pair, 8 bits ///28
	E_DRV_LINK_MINILVDS_1CH_3P_8BIT,
	// 1 channel, 4 pair, 8 bits ///29
	E_DRV_LINK_MINILVDS_1CH_4P_8BIT,
	// 1 channel, 5 pair, 8 bits ///30
	E_DRV_LINK_MINILVDS_1CH_5P_8BIT,
	// 1 channel, 6 pair, 8 bits ///31
	E_DRV_LINK_MINILVDS_1CH_6P_8BIT,

	// 2 channel, 3 pari, 6 bits ///32
	E_DRV_LINK_MINILVDS_2CH_3P_6BIT,
	// 2 channel, 4 pari, 6 bits ///33
	E_DRV_LINK_MINILVDS_2CH_4P_6BIT,
	// 2 channel, 5 pari, 6 bits ///34
	E_DRV_LINK_MINILVDS_2CH_5P_6BIT,
	//  2 channel, 6 pari, 6 bits ///35
	E_DRV_LINK_MINILVDS_2CH_6P_6BIT,

	// 1 channel, 3 pair, 6 bits ///36
	E_DRV_LINK_MINILVDS_1CH_3P_6BIT,
	// 1 channel, 4 pair, 6 bits ///37
	E_DRV_LINK_MINILVDS_1CH_4P_6BIT,
	// 1 channel, 5 pair, 6 bits ///38
	E_DRV_LINK_MINILVDS_1CH_5P_6BIT,
	// 1 channel, 6 pair, 6 bits ///39
	E_DRV_LINK_MINILVDS_1CH_6P_6BIT,
	//   HDMI Bypass Mode///40
	E_DRV_LINK_HDMI_BYPASS_MODE,

	/// 41
	E_DRV_LINK_EPI34_2P,
	/// 42
	E_DRV_LINK_EPI34_4P,
	/// 43
	E_DRV_LINK_EPI28_2P,
	/// 44
	E_DRV_LINK_EPI28_4P,

	///45
	E_DRV_LINK_VBY1_10BIT_4LANE,
	///46
	E_DRV_LINK_VBY1_10BIT_2LANE,
	///47
	E_DRV_LINK_VBY1_10BIT_1LANE,
	///48
	E_DRV_LINK_VBY1_8BIT_4LANE,
	///49
	E_DRV_LINK_VBY1_8BIT_2LANE,
	///50
	E_DRV_LINK_VBY1_8BIT_1LANE,

	///51
	E_DRV_LINK_VBY1_10BIT_8LANE,
	///52
	E_DRV_LINK_VBY1_8BIT_8LANE,

	///53
	E_DRV_LINK_EPI28_12P,

	//54
	E_DRV_LINK_HS_LVDS_2CH_BYPASS_MODE,
	//55
	E_DRV_LINK_VBY1_8BIT_4LANE_BYPASS_MODE,
	//56
	E_DRV_LINK_VBY1_10BIT_4LANE_BYPASS_MODE,
	///57
	E_DRV_LINK_EPI24_12P,
	///58
	E_DRV_LINK_VBY1_10BIT_16LANE,
	///59
	E_DRV_LINK_VBY1_8BIT_16LANE,
	///60
	E_DRV_LINK_USI_T_8BIT_12P,
	///61
	E_DRV_LINK_USI_T_10BIT_12P,
	///62
	E_DRV_LINK_ISP_8BIT_12P,
	///63
	E_DRV_LINK_ISP_8BIT_6P_D,

	///64
	E_DRV_LINK_ISP_8BIT_8P,
	///65
	E_DRV_LINK_ISP_10BIT_12P,
	///66
	E_DRV_LINK_ISP_10BIT_6P_D,
	///67
	E_DRV_LINK_ISP_10BIT_8P,
	///68
	E_DRV_LINK_EPI24_16P,
	///69
	E_DRV_LINK_EPI28_16P,
	///70
	E_DRV_LINK_EPI28_6P_EPI3G,
	///71
	E_DRV_LINK_EPI28_8P_EPI3G,

	///72
	E_DRV_LINK_CMPI24_10BIT_12P,
	///73
	E_DRV_LINK_CMPI27_8BIT_12P,
	///74
	E_DRV_LINK_CMPI27_8BIT_8P,
	///75
	E_DRV_LINK_CMPI24_10BIT_8P,
	///76
	E_DRV_LINK_USII_8BIT_12X1_4K,
	///77
	E_DRV_LINK_USII_8BIT_6X1_D_4K,
	///78
	E_DRV_LINK_USII_8BIT_8X1_4K,
	///79
	E_DRV_LINK_USII_8BIT_6X1_4K,
	///80
	E_DRV_LINK_USII_8BIT_6X2_4K,
	///81
	E_DRV_LINK_USII_10BIT_12X1_4K,
	///82
	E_DRV_LINK_USII_10BIT_8X1_4K,

	///83
	E_DRV_LINK_EPI24_8P,
	///84
	E_DRV_LINK_USI_T_8BIT_8P,
	///85
	E_DRV_LINK_USI_T_10BIT_8P,
	///86
	E_DRV_LINK_MINILVDS_4CH_6P_8BIT,
	///87
	E_DRV_LINK_EPI28_12P_EPI3G,
	///88
	E_DRV_LINK_EPI28_16P_EPI3G,

	///89
	E_DRV_LINK_MIPI_DSI_RGB888_4lane,
	///90
	E_DRV_LINK_MIPI_DSI_RGB565_4lane,
	///91
	E_DRV_LINK_MIPI_DSI_RGB666_4lane,
	///92
	E_DRV_LINK_MIPI_DSI_RGB888_3lane,
	///93
	E_DRV_LINK_MIPI_DSI_RGB565_3lane,
	///94
	E_DRV_LINK_MIPI_DSI_RGB666_3lane,
	///95
	E_DRV_LINK_MIPI_DSI_RGB888_2lane,
	///96
	E_DRV_LINK_MIPI_DSI_RGB565_2lane,
	///97
	E_DRV_LINK_MIPI_DSI_RGB666_2lane,
	///98
	E_DRV_LINK_MIPI_DSI_RGB888_1lane,
	///99
	E_DRV_LINK_MIPI_DSI_RGB565_1lane,
	///100
	E_DRV_LINK_MIPI_DSI_RGB666_1lane,

	///101
	E_DRV_LINK_NUM,
} en_drv_pnl_link_type; //APIPNL_LINK_TYPE;


/// Define the panel vby1 ctrlbit feature
typedef enum {
	E_DRV_VBO_CTRLBIT_NO_FEATURE,
	E_DRV_VBO_CTRLBIT_GLOBAL_FRAMEID,
	E_DRV_VBO_CTRLBIT_HTOTAL,
	E_DRV_VBO_CTRLBIT_VTOTAL,
	E_DRV_VBO_CTRLBIT_HSTART_POS,
	E_DRV_VBO_CTRLBIT_VSTART_POS,
	E_DRV_VBO_CTRLBIT_NUM,
} en_drv_vbo_ctrlbit_feature;


typedef struct {
	//version control
	uint16_t version;
	uint16_t length;
	char pnlname[DRV_PNL_NAME_MAX_NUM];                ///<  PanelName
	///<  PANEL_LINK
	en_drv_pnl_link_if linkIf;
	uint16_t div_section;

	///////////////////////////////////////////////
	// Board related setting
	///////////////////////////////////////////////
	///   A/B channel swap
	bool swap_port;
	///<  PANEL_SWAP_ODD_ML
	bool swapodd_ml;
	///<  PANEL_SWAP_EVEN_ML
	bool swapeven_ml;
	///<  PANEL_SWAP_ODD_RB
	bool swapodd_rb;
	///<  PANEL_SWAP_EVEN_RB
	bool swapeven_rb;
	///<  PANEL_SWAP_LVDS_POL, for differential P/N swap
	bool swaplvds_pol;
	///<  PANEL_SWAP_LVDS_CH, for pair swap
	bool swaplvds_ch;
	///<  PANEL_PDP_10BIT ,for pair swap
	bool pdp_10bit;
	///<  PANEL_LVDS_TI_MODE
	bool lvds_ti_mode;
	///////////////////////////////////////////////
	// For TTL Only
	///////////////////////////////////////////////
	///<  PANEL_DCLK_DELAY
	uint8_t dclk_dely;
	///<  PANEL_INV_DCLK
	bool inv_dclk;
	///<  PANEL_INV_DE
	bool inv_de;
	///<  PANEL_INV_HSYNC
	bool inv_hsync;
	///<  PANEL_INV_VSYNC
	bool inv_vsync;

	///////////////////////////////////////////////
	// panel on/off timing
	///////////////////////////////////////////////
	///<  time between panel & data while turn on power
	uint16_t ontiming_1;
	///<  time between data & back light while turn on power
	uint16_t ontiming_2;
	///<  time between back light & data while turn off power
	uint16_t offtiming_1;
	///<  time between data & panel while turn off power
	uint16_t offtiming_2;

	///////////////////////////////////////////////
	// panel timing spec.
	///////////////////////////////////////////////
	// sync related
	uint16_t hsync_st;
	///<  PANEL_HSYNC_WIDTH
	uint8_t hsync_w;
	uint8_t hsync_pol;

	uint16_t vsync_st;
	///<  define PANEL_VSYNC_WIDTH
	uint8_t vsync_w;
	///<  define PANEL_VSYNC_BACK_PORCH
	uint8_t vsync_pol;

	// DE related
	///<  PANEL_HSTART, DE H Start
	uint16_t de_hstart;
	///<  PANEL_VSTART, DE V Start
	uint16_t de_vstart;
	///< PANEL_WIDTH, DE width
	uint16_t de_width;
	///< PANEL_HEIGHT, DE height
	uint16_t de_height;

	// DClk related
	///<  PANEL_MAX_HTOTAL. Reserved for future using.
	uint16_t max_htt;
	uint16_t typ_htt;
	///<  PANEL_MIN_HTOTAL. Reserved for future using.
	uint16_t min_htt;

	///<  PANEL_MAX_VTOTAL. Reserved for future using.
	uint16_t max_vtt;
	uint16_t typ_vtt;
	///<  PANEL_MIN_VTOTAL. Reserved for future using.
	uint16_t min_vtt;

	///<  PANEL_MAX_DCLK. Reserved for future using.
	uint16_t max_dclk;
	uint16_t typ_dclk;
	///<  PANEL_MIN_DCLK. Reserved for future using.
	uint16_t min_dclk;

	en_drv_pnl_output_format op_format;

	en_drv_pnl_tibitmode ti_bitmode;
	en_drv_pnl_outputformat_bitmode op_bitmode;
/*
 *
 * Board related params
 *
 *  If a board ( like BD_MST064C_D01A_S ) swap LVDS TX polarity
 *    : This polarity swap value =
 *      (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L from board define,
 *  Otherwise
 *    : The value shall set to 0.
 */
	uint16_t lvds_txswap;
	///<  define PANEL_SWAP_ODD_RG
	bool swapodd_rg;
	///<  define PANEL_SWAP_EVEN_RG
	bool swapeven_rg;
	///<  define PANEL_SWAP_ODD_GB
	bool swapodd_gb;
	///<  define PANEL_SWAP_EVEN_GB
	bool swapeven_gb;
	///<  PAFRC mixed with noise dither disable
	bool noise_dith;

/*
 *###########################
 *#  Panel backlight related params.#
 *###########################
 */
	uint32_t period_pwm;
	uint32_t duty_pwm;
	uint16_t div_pwm;
	bool pol_pwm;
	uint16_t max_pwm_val;
	uint16_t typ_pwm_val;
	uint16_t min_pwm_val;
	uint16_t pwmport;


/**
 *  Others
 */
	//HDMI2.1 VRR
	// variable refresh rate
	bool vrr_en;
	// vrr max frame rate
	uint16_t vrr_max_v;
	// vrr min frame rate
	uint16_t vrr_min_v;

	uint8_t lane_num;
} st_drv_pnl_info;

typedef struct  {
	uint8_t hsync_w;
	uint16_t de_hstart;
	uint8_t lane_numbers;
	en_drv_pnl_output_format op_format;
} st_drv_hprotect_info;

typedef struct  {
	uint32_t sup_lanes;
	uint32_t def_layout[DRV_VBO_MAX_NUM];
	bool usr_defined;
	uint32_t ctrl_lanes;
	uint32_t lane_order[DRV_VBO_MAX_NUM];
} st_drv_out_lane_order;

typedef struct {

	bool	bEnable;
	//is this ctrlbit active default 0
	__u8 u8LaneID;
	// 1~16 (to specify the lane), 0: copy to all lane(for 8KBE)
	__u8 u8Lsb;
	// the least significant bit of ctrlbit, support 0~23
	__u8 u8Msb;
	// the most significant bit of ctrlbit, support 0~23
	__u32 u32Value;
	// the value if specific ctrlbit, if feature was specified, this value will be ignored.
	en_drv_vbo_ctrlbit_feature enFeature;
	// feature_name
}st_drv_ctrlbit_item;


typedef struct {
	__u8 u8ConnectorID;
	// 0: Video; 1: Delta Video; 2: GFX
	__u8 u8CopyType;
	// 0: normal(for Delta Video, GFX) ; 1: copy all lane(for Video)
	st_drv_ctrlbit_item ctrlbit_item[DRV_CTRLBIT_MAX_NUM];
	// ctrl items
}st_drv_ctrlbits;

typedef struct {
	bool usr_swing_level;
	bool common_swing;
	uint32_t ctrl_lanes;
	uint32_t swing_level[DRV_VBO_MAX_NUM];
}st_out_swing_level;

typedef struct {
	bool usr_pe_level;
	bool common_pe;
	uint32_t ctrl_lanes;
	uint32_t pe_level[DRV_VBO_MAX_NUM];
}st_out_pe_level;

typedef struct {
	uint32_t ssc_en;
	uint32_t ssc_modulation;
	uint32_t ssc_deviation;
}st_out_ssc_ctrl;


typedef struct {
	uint16_t outconfig_ch0_ch7;
	uint16_t outconfig_ch8_ch15;
	uint16_t outconfig_ch16_ch19;
}st_mod_outconfig;

typedef struct {
	uint16_t freeswap_ch0_ch1;
	uint16_t freeswap_ch2_ch3;
	uint16_t freeswap_ch4_ch5;
	uint16_t freeswap_ch6_ch7;
	uint16_t freeswap_ch8_ch9;
	uint16_t freeswap_ch10_ch11;
	uint16_t freeswap_ch12_ch13;
	uint16_t freeswap_ch14_ch15;
	uint16_t freeswap_ch16_ch17;
	uint16_t freeswap_ch18_ch19;
	uint16_t freeswap_ch20_ch21;
	uint16_t freeswap_ch22_ch23;
	uint16_t freeswap_ch24_ch25;
	uint16_t freeswap_ch26_ch27;
	uint16_t freeswap_ch28_ch29;
	uint16_t freeswap_ch30_ch31;
}st_mod_freeswap;

typedef struct {
	bool vby1_locked;
	uint16_t vby1_unlockcnt;
	bool vby1_htpdn;
	bool vby1_lockn;
	st_mod_outconfig outconfig;
	st_mod_freeswap freeswap;
} st_drv_pnl_mod_status;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
int drv_hwreg_render_pnl_set_vttLockModeEnable(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_vttSourceSelect(
	struct hwregOut *paramOut,
	bool RIU,
	drm_en_vttv_source_type ivsSource,
	drm_en_vttv_source_type ovsSource);

int drv_hwreg_render_pnl_set_centralVttChange(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable,
	uint8_t frameCount,
	uint8_t threshold);

int drv_hwreg_render_pnl_set_vttLockPhaseDiff(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable,
	uint16_t phaseDiff);

int drv_hwreg_render_pnl_set_vttLockKeepSequenceEnable(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable,
	uint8_t LockKeepSequenceTh);

int drv_hwreg_render_pnl_set_vttLockVttRange(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable,
	uint16_t vttUpperBound,
	uint16_t vttLowerBound);

int drv_hwreg_render_pnl_set_vttLockDiffLimit(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable,
	uint16_t diffLimit);

int drv_hwreg_render_pnl_set_vttLockVsMaskMode(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_vttLockIvsOvs(
	struct hwregOut *paramOut,
	bool RIU,
	uint16_t ivs,
	uint16_t ovs);

int drv_hwreg_render_pnl_set_vttLockLockedInLimit(
	struct hwregOut *paramOut,
	bool RIU,
	uint16_t lockedLimit);

int drv_hwreg_render_pnl_set_vttLockUdSwModeEnable(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_vttLockIvsShiftEnable(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_vttLockProtectEnable(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_vttLockLastVttLatchEnable(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_vttLockLastVttLatchToggle(
	struct hwregOut *paramOut,
	bool RIU,
	bool toggle);

int drv_hwreg_render_pnl_set_rwDiffTraceEnable(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_rwDiffTraceDepositCount(
	struct hwregOut *paramOut,
	bool RIU,
	uint16_t depositCount);

int drv_hwreg_render_pnl_set_forceInsertBypass(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_InsertRecord(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_fakeInsertPEnable(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_stateSwModeEnable(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_stateSwMode(
	struct hwregOut *paramOut,
	bool RIU,
	uint16_t state);

int drv_hwreg_render_pnl_set_FrameLockVttRange(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable,
	uint16_t mDelta,
	uint16_t vttLowerBound,
	uint16_t vttUpperBound);

int drv_hwreg_render_pnl_set_chaseTotalDiffRange(
	struct hwregOut *paramOut,
	bool RIU,
	uint16_t diffRange);

int drv_hwreg_render_pnl_set_chasePhaseTarget(
	struct hwregOut *paramOut,
	bool RIU,
	uint16_t targetVal);

int drv_hwreg_render_pnl_set_chasePhaseAlmost(
	struct hwregOut *paramOut,
	bool RIU,
	uint16_t almostVal);

int drv_hwreg_render_pnl_set_statusReset(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_framelockVcntEnableDb(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);

int drv_hwreg_render_pnl_set_vtt(
	struct hwregOut *paramOut,
	bool RIU,
	uint16_t vtt);

int drv_hwreg_render_pnl_get_vttvLockedFlag(
	struct hwregOut *paramOut,
	bool RIU);

int drv_hwreg_render_pnl_get_chaseTotalStatus(
	struct hwregOut *paramOut,
	bool RIU);

int drv_hwreg_render_pnl_get_chasePhaseStatus(
	struct hwregOut *paramOut,
	bool RIU);

int drv_hwreg_render_pnl_get_vttLockModeEnable(
	struct hwregOut *paramOut,
	bool RIU);

int drv_hwreg_render_pnl_get_framelockVcntEnableDb(
	struct hwregOut *paramOut,
	bool RIU);

void drv_hwreg_render_pnl_set_cfg_ckg_common(void);

void drv_hwreg_render_pnl_set_cfg_ckg_video(
	uint32_t hw_version,
	en_drv_pnl_vbo_bytemode vbo_bytemode,
	uint8_t in,
	uint8_t out,
	en_drv_pnl_link_if linkIF,
	en_drv_pnl_output_mode timing,
	en_drv_pnl_output_format format);

void drv_hwreg_render_pnl_set_cfg_ckg_deltav(
	uint8_t in,
	uint8_t out,
	en_drv_pnl_output_mode timing);

void drv_hwreg_render_pnl_set_cfg_ckg_gfx(
	uint8_t in,
	uint8_t out,
	en_drv_pnl_output_mode timing,
	en_drv_pnl_output_format format);

void drv_hwreg_render_pnl_set_lvds_v_fhd_60HZ(uint32_t hwversion);
void drv_hwreg_render_pnl_set_vby1_v_fhd_60HZ(uint32_t hwversion);
void drv_hwreg_render_pnl_set_vby1_v_fhd_120HZ(uint32_t hwversion);
void drv_hwreg_render_pnl_set_vby1_v_4k_60HZ(uint32_t hwversion);
void drv_hwreg_render_pnl_set_vby1_v_4k_120HZ(uint32_t hwversion);
void drv_hwreg_render_pnl_set_vby1_v_8k422_60HZ(void);
void drv_hwreg_render_pnl_set_vby1_v_8k_60HZ(void);
void drv_hwreg_render_pnl_set_vby1_deltav_4k_60HZ(void);
void drv_hwreg_render_pnl_set_vby1_deltav_4k_120HZ(void);
void drv_hwreg_render_pnl_set_vby1_gfx_4k_30HZ(void);
void drv_hwreg_render_pnl_set_vby1_gfx_4k_argb8888_60HZ(void);
void drv_hwreg_render_pnl_set_vby1_gfx_4k_argb8101010_60HZ(void);
void drv_hwreg_render_pnl_set_sctcon_misc(uint8_t u8out_format);
void drv_hwreg_render_pnl_set_vby1_seri_data_sel(uint8_t value);
void drv_hwreg_render_pnl_set_vby1_mft_hmirror_video(bool bEn);
void drv_hwreg_render_pnl_set_vby1_mft_hmirror_deltav(bool bEn);
void drv_hwreg_render_pnl_set_vby1_mft_hmirror_gfx(bool bEn);
bool drv_hwreg_render_pnl_get_vby1_mft_hmirror_video(void);
bool drv_hwreg_render_pnl_get_vby1_mft_hmirror_deltav(void);
bool drv_hwreg_render_pnl_get_vby1_mft_hmirror_gfx(void);

void drv_hwreg_render_pnl_set_vby1_fifo_reset(void);
void drv_hwreg_render_pnl_set_lvds_lane_order(uint32_t hwversion, uint8_t value);
void drv_hwreg_render_pnl_set_vby1_pll_powerdown(bool bEn);
void drv_hwreg_render_pnl_set_vby1_byte_mode(en_drv_pnl_vbo_bytemode vbo_byte);

void drv_hwreg_render_pnl_set_render_output_en_video(
	uint32_t hwversion,
	bool bEn,
	en_drv_pnl_output_mode timing,
	en_drv_pnl_output_format format,
	en_drv_pnl_link_if linkIF,
	st_drv_out_lane_order *stDrvOutLaneOrder);
void drv_hwreg_render_pnl_set_render_output_en_deltav(
	bool bEn,
	en_drv_pnl_output_mode timing,
	st_drv_out_lane_order *stDrvOutLaneOrder,
	bool blaneduplicateEn);
void drv_hwreg_render_pnl_set_render_output_en_gfx(bool bEn);
void drv_hwreg_render_pnl_tgen_init(uint32_t hw_version,st_drv_pnl_info *stPnlInfo);
int drv_hwreg_render_pnl_hbkproch_protect_init(st_drv_hprotect_info *stHprotectInfo);
int drv_hwreg_render_pnl_delta_hbkproch_protect_init(st_drv_hprotect_info *stHprotectInfo);
int drv_hwreg_render_pnl_gfx_hbkproch_protect_init(st_drv_hprotect_info *stHprotectInfo);
void drv_hwreg_render_pnl_delta_tgen_init(st_drv_pnl_info *stPnlInfo);
void drv_hwreg_render_pnl_load_lpll_tbl(uint32_t hwVersion,uint16_t tblidx,uint16_t panelNum);
void drv_hwreg_render_pnl_load_mpll_tbl(uint32_t hwVersion,uint16_t tblidx,uint16_t regNum,uint16_t panelNum);
void drv_hwreg_render_pnl_load_moda_tbl(uint32_t hwVersion,uint16_t tblidx,uint16_t regNum,uint16_t panelNum);
void drv_hwreg_render_pnl_set_odclk(uint32_t hwVersion,uint16_t tblidx, uint32_t odclk);
void drv_hwreg_render_pnl_init_clk(void);
void drv_hwreg_render_pnl_init_clk_v2(void);
void drv_hwreg_render_pnl_analog_setting(
	uint32_t hw_version,
	en_drv_pnl_link_if linkIF,
	en_drv_pnl_output_mode timing,
	st_drv_out_lane_order *stDrvOutLaneOrder,
	en_drv_pnl_output_format format);
void drv_hwreg_render_pnl_ext_video_analog_setting(
	en_drv_pnl_link_if linkIF,
	en_drv_pnl_output_mode timing,
	st_drv_out_lane_order *stDrvOutLaneOrder,
	bool blaneduplicateEn);
void drv_hwreg_render_pnl_gfx_analog_setting(
	en_drv_pnl_link_if linkIF,
	en_drv_pnl_output_mode timing,
	st_drv_out_lane_order *stDrvOutLaneOrder);
void drv_hwreg_render_pnl_analog_path_setting(bool path1EN, bool path2EN);
void drv_hwreg_render_pnl_set_pre_emphasis_enable(void);
void drv_hwreg_render_pnl_set_pre_emphasis(st_out_pe_level *pe_info);
void drv_hwreg_render_pnl_get_pre_emphasis(st_out_pe_level *pe_info);
void drv_hwreg_render_pnl_set_swing_biascon(en_drv_pnl_link_if linkIF, uint32_t hwVersion, uint16_t biasConVal, uint16_t biasConOffsetVal);
void drv_hwreg_render_pnl_set_swing_rcon(en_drv_pnl_link_if linkIF);
void drv_hwreg_render_pnl_set_swing_vreg(st_out_swing_level *swing_info);
void drv_hwreg_render_pnl_get_swing_vreg(st_out_swing_level *swing_info);
void drv_hwreg_render_pnl_set_ssc(st_out_ssc_ctrl *ssc_info);
void drv_hwreg_render_pnl_get_ssc(st_out_ssc_ctrl *ssc_info);
void drv_hwreg_render_pnl_set_vby1_mft(
	en_drv_pnl_link_if linkIF,
	uint8_t div_sec,
	uint8_t lanes,
	uint16_t de_width);
void drv_hwreg_render_pnl_set_vby1_lane_order(
	en_drv_pnl_link_if linkIF,
	en_drv_pnl_output_mode timing,
	en_drv_pnl_output_format format,
	uint8_t div_sec,
	uint8_t lanes,
	st_drv_out_lane_order *stDrvOutLaneOrder);
void drv_hwreg_render_pnl_set_vby1_lane_order_delta(
	en_drv_pnl_link_if linkIF,
	uint8_t div_sec,
	uint8_t lanes);
void drv_hwreg_render_pnl_disp_odclk_init(void);

int drv_hwreg_render_pnl_set_vbo_ctrlbit(st_drv_ctrlbits *stCtrlbits);
int drv_hwreg_render_pnl_set_gop_pattern(bool bEn);
int drv_hwreg_render_pnl_set_multiwin_pattern(bool bEn);
int drv_hwreg_render_pnl_set_tcon_pattern(bool bEn);
int drv_hwreg_render_pnl_set_tx_mute_en_video(bool bEn);
int drv_hwreg_render_pnl_set_tx_mute_en_deltav(bool bEn);
int drv_hwreg_render_pnl_set_tx_mute_en_gfx(bool bEn);
int drv_hwreg_render_pnl_set_pixel_mute_en_color_video(bool bEn, uint16_t u16Red, uint16_t u16Green, uint16_t u16Blue);
int drv_hwreg_render_pnl_set_pixel_mute_en_color_deltav(bool bEn, uint16_t u16Red, uint16_t u16Green, uint16_t u16Blue);
int drv_hwreg_render_pnl_set_pixel_mute_en_color_gfx(bool bEn, uint16_t u16Red, uint16_t u16Green, uint16_t u16Blue);
bool drv_hwreg_render_pnl_get_tx_mute_en_video(void);
bool drv_hwreg_render_pnl_get_tx_mute_en_deltav(void);
bool drv_hwreg_render_pnl_get_tx_mute_en_gfx(void);
bool drv_hwreg_render_pnl_get_pixel_mute_en_video(void);
uint16_t drv_hwreg_render_pnl_get_pixel_mute_red_video(void);
uint16_t drv_hwreg_render_pnl_get_pixel_mute_green_video(void);
uint16_t drv_hwreg_render_pnl_get_pixel_mute_blue_video(void);
bool drv_hwreg_render_pnl_get_pixel_mute_en_deltav(void);
uint16_t drv_hwreg_render_pnl_get_pixel_mute_red_deltav(void);
uint16_t drv_hwreg_render_pnl_get_pixel_mute_green_deltav(void);
uint16_t drv_hwreg_render_pnl_get_pixel_mute_blue_deltav(void);
bool drv_hwreg_render_pnl_get_pixel_mute_en_gfx(void);
uint16_t drv_hwreg_render_pnl_get_pixel_mute_red_gfx(void);
uint16_t drv_hwreg_render_pnl_get_pixel_mute_green_gfx(void);
uint16_t drv_hwreg_render_pnl_get_pixel_mute_blue_gfx(void);
int drv_hwreg_render_pnl_tgen_set_gop_ref_mask(
	struct hwregOut *paramOut,
	bool RIU,
	bool enable);
void drv_hwreg_render_pnl_get_mod_status(st_drv_pnl_mod_status *stModStatus);
uint8_t drv_hwreg_render_pnl_get_vby1_swap(uint8_t lanes);

#endif // _HWREG_RENDER_VIDEO_PNL_H_
