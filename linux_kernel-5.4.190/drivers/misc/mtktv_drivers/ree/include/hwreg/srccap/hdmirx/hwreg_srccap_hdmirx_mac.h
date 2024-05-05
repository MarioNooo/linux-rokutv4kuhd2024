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

#ifndef _HWREG_SRCCAP_HDMIRX_MAC_H_
#define _HWREG_SRCCAP_HDMIRX_MAC_H_

#include "hwreg_common.h"

#include "drvXC_IOPort.h"
#include "drvXC_HDMI_if.h"

#include "hwreg_srccap_hdmirx_struct.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
enum e_hdmi_clk_domain {
	E_HDMI_LINK,
	E_HDMI_CHAR,
	E_HDMI_PIX,
	E_HDMI_PIX_IVS,
};

enum e_hdmi_force_mode_list {
	E_HDMI_F_IMMESWITCH,
	E_HDMI_F_MAX,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */


/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_init(
	MS_BOOL bImmeswitchSupport,
	MS_U8 ucMHLSupportPath);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_deinit(void);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_suspend(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_resume(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_rst(
    E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_mac_initinternalinfo(
	stHDMI_POLLING_INFO *p_info);

extern void drv_hwreg_srccap_hdmirx_mac_rst_software(
	E_MUX_INPUTPORT enInputPortType,
	MS_U16 u16Reset);

extern void drv_hwreg_srccap_hdmirx_mac_proc_stablepolling(
	MS_U8 ucMHLSupportPath,
	MS_U32 *ulPacketStatus,
	stHDMI_POLLING_INFO *stHDMIPollingInfo);

extern void drv_hwreg_srccap_hdmirx_mac_set_powerdown(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL bPowerDown);

extern void drv_hwreg_srccap_hdmirx_mac_set_scdcval(
	E_MUX_INPUTPORT enInputPortType,
	MS_U8 u8Offset,
	MS_U8 u8SCDCValue);

extern void drv_hwreg_srccap_hdmirx_mac_set_scdcintmask(
	E_MUX_INPUTPORT enInputPortType,
	MS_U32 u32StatusMask,
	MS_BOOL bMask);

extern void drv_hwreg_srccap_hdmirx_mac_set_ditheren(
	E_MUX_INPUTPORT enInputPortType,
	E_DITHER_TYPE enDitherType,
	MS_BOOL ubRoundEnable);

extern void drv_hwreg_srccap_hdmirx_mac_set_immeswitch(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo,
	MS_BOOL bPowerSavingOn);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_get_immeswitchstatus(
	E_MUX_INPUTPORT enInputPortType);

extern MS_U8 drv_hwreg_srccap_hdmirx_mac_get_scdcval(
	E_MUX_INPUTPORT enInputPortType,
	MS_U8 u8Offset);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_get_scdccabledet(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_get_scdcintmask(
	E_MUX_INPUTPORT enInputPortType,
	MS_U32 u32StatusMask);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_get_crcval(
	E_MUX_INPUTPORT enInputPortType,
	MS_DVI_CHANNEL_TYPE u8Channel,
	MS_U16 *pu16CRCVal,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_get_destable(
	E_MUX_INPUTPORT enInputPortType);

extern MS_U32 drv_hwreg_srccap_hdmirx_mac_get_datainfo(
	E_HDMI_GET_DATA_INFO enInfo,
	MS_U8 ucHDMIInfoSource,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_U32 drv_hwreg_srccap_hdmirx_mac_getratekhz(
	E_MUX_INPUTPORT enInputPortType,
	enum e_hdmi_clk_domain e_clk_domain,
	MS_BOOL IsFRL);

extern MS_U16 drv_hwreg_srccap_hdmirx_mac_get_hdeinfo(
	MS_U8 ucHDMIInfoSource)	;

extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_islarger166mhz(
	MS_U8 ucHDMIInfoSource,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_force_mode(
	E_MUX_INPUTPORT enInputPortType, enum e_hdmi_force_mode_list force_mode_item,
	void *p_param, MS_U32 u32_param1, MS_U32 u32_param2);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_isr(
	E_MUX_INPUTPORT enInputPortType,
	enum E_MAC_TOP_ISR_EVT evt);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_sw_mute_notify(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

#endif // _HWREG_SRCCAP_HDMIRX_MAC_H_


