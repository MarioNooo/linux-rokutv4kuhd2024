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

#ifndef _HWREG_SRCCAP_HDMIRX_DSC_H_
#define _HWREG_SRCCAP_HDMIRX_DSC_H_

#include "hwreg_common.h"

#include "drvXC_IOPort.h"
#include "drvXC_HDMI_if.h"
/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
enum e_dsc_decoder_mux_n {
	E_DSC_D0,
	E_DSC_D1,
	E_DSC_D2,
	E_DSC_D3,
	E_DSC_MAX,
};

enum e_hdmi_dsc_status {
	E_EMP_1_DECODER_1,
	E_EMP_1_DECODER_0,  // abnormal case.....
	E_EMP_0_DECODER_1,
	E_EMP_0_DECODER_0,
};
/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

typedef struct __attribute__((__packed__))
{
    MS_U8 u8_bpc;     // pps3[7:4]
    MS_U8 u8_linebuf_depth; // pps[3:0]
    MS_U16 u16_bpp; // bpp_real = u16_bpp >> 4  // pps4[1:0]<<8|pps5
    MS_U16 u16_pic_height;  // pps6<<8|pps7
    MS_U16 u16_pic_width;  // pps8<<8|pps9
    MS_U16 u16_slice_height; // pps10<<8|pps11
    MS_U16 u16_slice_width; // pps12<<8|pps13
    MS_U16 u16_chunkbytes; // pps14<<8|pps15
    MS_U16 u16_initial_dec_delay;  // pps18<<8|pps19
    MS_U8 u8_native_422;  // pps88[0]
    MS_U8 u8_native_420; // pps[1]
    MS_U16 u16_hfront; // pps129<<8|pps128
    MS_U16 u16_hsync; // pps131<<8|pps130
    MS_U16 u16_hback; // pps133<<8|pps132
    MS_U16 u16_hactivebytes; // pps135<<8|pps134
    MS_U8  u8_slice_num;
// clk
    MS_U32 u32_dsc_clk;  // 1 engine
    MS_U32 u32_pixel_clk;  // 1p
    MS_U8 u8_dsc_n;   // 3
    MS_U8 u8_pixel_n;  // 8
    MS_U8 u8_dsc_420_2_444;  // dependent
// reg fill
    MS_U16 u16_reg_entropy_shf_blk_slice;
    MS_U16 u16_reg_entropy_shf_blk;
    MS_U16 u16_reg_ini_dec_line_delay;
    MS_U16 u16_dsc_420_h_factor;
}ST_HDMI_DSC_INFO;

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */

extern MS_BOOL drv_hwreg_srccap_hdmirx_dsc_init(
    enum e_dsc_decoder_mux_n);
extern MS_BOOL drv_hwreg_srccap_hdmirx_dsc_deinit(
    enum e_dsc_decoder_mux_n);
extern MS_BOOL drv_hwreg_srccap_hdmirx_dsc_suspend(
    enum e_dsc_decoder_mux_n);
extern MS_BOOL drv_hwreg_srccap_hdmirx_dsc_resume(
    enum e_dsc_decoder_mux_n);
extern MS_BOOL drv_hwreg_srccap_hdmirx_dsc_rst(
    enum e_dsc_decoder_mux_n);
extern MS_BOOL drv_hwreg_srccap_hdmirx_hdmi_dsc_init(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_hdmi_dsc_deinit(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_hdmi_dsc_suspend(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_hdmi_dsc_resume(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_hdmi_dsc_rst(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_get_dsc_pps_info(
    E_MUX_INPUTPORT enInputPortType,
    ST_HDMI_DSC_INFO *p_info);
extern MS_BOOL drv_hwreg_srccap_hdmirx_set_dsc_config(
    E_MUX_INPUTPORT enInputPortType,
    enum e_dsc_decoder_mux_n dsc_mux_n,
    ST_HDMI_DSC_INFO *p_info);

#endif // _HWREG_SRCCAP_HDMIRX_DSC_H_