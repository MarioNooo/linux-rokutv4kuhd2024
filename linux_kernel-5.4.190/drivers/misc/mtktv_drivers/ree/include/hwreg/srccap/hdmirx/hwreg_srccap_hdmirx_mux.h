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

#ifndef _HWREG_SRCCAP_HDMIRX_MUX_H_
#define _HWREG_SRCCAP_HDMIRX_MUX_H_

#include "hwreg_common.h"

#include "drvXC_IOPort.h"
#include "drvXC_HDMI_if.h"

#include "hwreg_srccap_hdmirx_dsc.h"
#include "hwreg_srccap_hdmirx_struct.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */

#define SRCINPUT_NUM_VAL 2

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
typedef enum
{
    HDMI_TIMING_DET_DTOPDEC_DC_AUTO = 0,
    HDMI_TIMING_DET_DTOPDEC_HDCP_AUTO,
    HDMI_TIMING_DET_DTOPDEC_DC_1P,
    HDMI_TIMING_DET_DTOPDEC_DC_8P,
    HDMI_TIMING_DET_DTOPDEC_HDCP_1P,
    HDMI_TIMING_DET_DTOPDEC_HDCP_8P,
}EN_HDMI_TIMING_DETECT_PATH_DTOPDEC;

typedef enum
{
    HDMI_TIMING_DET_DTOPDEP_AUTO = 0,  // dither_1p(tmds) or rcvtop(frl)
    HDMI_TIMING_DET_DTOPDEP_DITHER_1P,
    HDMI_TIMING_DET_DTOPDEP_RCVTOP,
    HDMI_TIMING_DET_DTOPDEP_SC,
}EN_HDMI_TIMING_DETECT_PATH_DTOPDEP;

typedef enum
{
    HDMI_TIMING_DET_DC_1P = 0,
    HDMI_TIMING_DET_HDCP_1P,
    HDMI_TIMING_DET_DITHER_1P,
    HDMI_TIMING_DET_DC_8P,
    HDMI_TIMING_DET_HDCP_8P,
    HDMI_TIMING_DET_RCV_TOP,
    HDMI_TIMING_DET_SC,
}EN_HDMI_TIMING_DETECT_PATH;

typedef enum
{
    E_CLK_MUX_TIMING_DET_DTOP_DEC,
    E_CLK_MUX_TIMING_DET_DTOP_DEP,
    E_CLK_MUX_TIMING_DET_DTOPMISC,
    E_CLK_MUX_AUDIO_S0,
    E_CLK_MUX_AUDIO_S1,
    E_CLK_MUX_DVI2MIU_S0,
    E_CLK_MUX_DVI2MIU_S1,
    E_CLK_MUX_DSC,
    E_CLK_MUX_TMDS_FRL,
}EN_HDMI_CLK_MUX_SEL;

enum HDMI_SOURCE_VERSION_TYPE
{
    HDMI_SOURCE_VERSION_NOT_SURE = 0,
    HDMI_SOURCE_VERSION_HDMI14,
    HDMI_SOURCE_VERSION_HDMI20,
    HDMI_SOURCE_VERSION_HDMI21,
};

typedef enum
{
    HDMI_RST_INNERMISC_DVI2MIU_S0 = 0,
    HDMI_RST_INNERMISC_DVI2MIU_S1,
    HDMI_RST_INNERMISC_TIMER = 4,
    HDMI_RST_INNERMISC_CLK_DET = 7,
    HDMI_RST_INNERMISC_DSCD,
    HDMI_RST_INNERMISC_DSCD_DST,
    HDMI_RST_INNERMISC_DSCD_SRC,
    HDMI_RST_INNERMISC_PORT_SEL_CDC = 12,
}EN_HDMI_SWRST_INNERMISC;

enum E_PWR_DOMAIN {
	E_HDMI_VIDEO,
	E_HDMI_VIDEO_SHARE,
	E_HDMI_AUDIO,
	E_HDMI_HDCP,
	E_HDMI_PKT,
	E_HDMI_PKT_SHARE,
	E_HDMI_DVI2MIU,
	E_HDMI_DSC,
	E_HDMI_MISC,
};

enum E_HDMI_INNER_MUX {
	E_HDMI_MUX_ADUIO,
	E_HDMI_MUX_DVI2MIU,
	E_HDMI_MUX_DSC,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

struct __attribute__((__packed__))
hwreg_srccap_hdmirx_mux_ioctl_s {
	E_MUX_INPUTPORT enInputPortType;
	enum e_dsc_decoder_mux_n dsc_mux_n;
	MS_U32 u32_param[4];
	MS_BOOL bRet;
	union{
	        MS_U64 addr1;
	        void* p_buf;
	};
};


/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_init(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_deinit(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_suspend(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_resume(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_rst(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_sel_clk(
	EN_HDMI_CLK_MUX_SEL mux_sel,
	E_MUX_INPUTPORT enInputPortType,
	MS_U32 u32_p0,
	MS_U32 u32_p1);
extern MS_U32 drv_hwreg_srccap_hdmirx_mux_query_clk(
	EN_HDMI_CLK_MUX_SEL mux_sel,
	E_MUX_INPUTPORT enInputPortType,
	MS_U32 u32_p0);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_sel_dsc(
	E_MUX_INPUTPORT enInputPortType,
	enum e_dsc_decoder_mux_n dsc_mux_n);
extern E_MUX_INPUTPORT drv_hwreg_srccap_hdmirx_mux_query_dsc(
	enum e_dsc_decoder_mux_n dsc_mux_n);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_sel_audio(
	enum HDMI_AUDIO_PATH_TYPE audio_mux_n,
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL bFRL);

extern  E_MUX_INPUTPORT
	drv_hwreg_srccap_hdmirx_mux_query_audio(
	enum HDMI_AUDIO_PATH_TYPE audio_mux_n);

extern enum HDMI_AUDIO_PATH_TYPE
	drv_hwreg_srccap_hdmirx_mux_query_audio_inverse(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_sw_init(void);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_sw_deinit(void);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_sw_rst(void);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_sel_video(
	MS_U8 u8InputID,
	E_MUX_INPUTPORT enInputPortType);

extern MS_U8
	drv_hwreg_srccap_hdmirx_mux_query_video(
	MS_U8 inputID);

extern MS_U8
	drv_hwreg_srccap_hdmirx_mux_query_video_inverse(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL
	drv_hwreg_srccap_hdmirx_mux_sel_timingdet(
	E_MUX_INPUTPORT enInputPortType,
	MS_U8 ucSourceVersion);

extern void drv_hwreg_srccap_hdmirx_mux_inner_rst(
	E_MUX_INPUTPORT enInputPortType,
	EN_HDMI_SWRST_INNERMISC rstbit);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_pwr_onoff(
	enum E_PWR_DOMAIN e_domain,
	MS_U32 u32_target,
	MS_U32 u32_p0,
	MS_U32 u32_p1,
	MS_BOOL b_on);

extern MS_U8 drv_hwreg_srccap_hdmirx_mux_inport_2_src(
	E_MUX_INPUTPORT enInputPortType);


extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_sel(
	enum E_HDMI_INNER_MUX e_mux,
	E_MUX_INPUTPORT enInputPortType,
	MS_U32 u32_p0,
	EN_HDMI_SIGNAL_TYPE e_sgl,
	MS_U32 u32_p1);


extern E_MUX_INPUTPORT drv_hwreg_srccap_hdmirx_mux_query(
	enum E_HDMI_INNER_MUX e_mux,
	MS_U32 u32_p0,
	MS_U32 u32_p1);

extern MS_BOOL
drv_hwreg_srccap_hdmirx_mux_set_onoff(
	enum E_HDMI_INNER_MUX e_mux,
	MS_U32 u32_p0,
	MS_BOOL b_on);

extern MS_BOOL
drv_hwreg_srccap_hdmirx_mux_dvi2miu_rd_clk_config(
	enum HDMI_DVI2MIU_PATH_TYPE dvi2miu_mux_n,
	E_MUX_INPUTPORT enInputPortType,
	MS_U16 u16_tmds_clk_mhz);

extern MS_BOOL
drv_hwreg_srccap_hdmirx_mux_Is_active_port(
	E_MUX_INPUTPORT enInputPortType,
	MS_U8 *p_active_port_n);

#endif // _HWREG_SRCCAP_HDMIRX_MUX_H_
