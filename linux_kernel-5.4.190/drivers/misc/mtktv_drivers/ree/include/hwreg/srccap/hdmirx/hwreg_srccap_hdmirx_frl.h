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

#ifndef _HWREG_SRCCAP_HDMIRX_FRL_H_
#define _HWREG_SRCCAP_HDMIRX_FRL_H_

#include "hwreg_common.h"

#include "drvXC_IOPort.h"
// #include "drvXC_HDMI_if.h"

#include "hwreg_srccap_hdmirx_tmds.h"
#include "hwreg_srccap_hdmirx_mac.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */
#define IsFRL(x) (((x)>HDMI_FRL_MODE_LEGACY) && ((x) <HDMI_FRL_MODE_NONE))
/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
enum HDMI_FRL_MODE_TYPE
{
	HDMI_FRL_MODE_LEGACY = 0,
	HDMI_FRL_MODE_FRL_3G_3CH,
	HDMI_FRL_MODE_FRL_6G_3CH,
	HDMI_FRL_MODE_FRL_6G_4CH,
	HDMI_FRL_MODE_FRL_8G,
	HDMI_FRL_MODE_FRL_10G,
	HDMI_FRL_MODE_FRL_12G,
	HDMI_FRL_MODE_NONE,
	HDMI_FRL_MODE_LEGACY_14,
	HDMI_FRL_MODE_LEGACY_20,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */

extern MS_BOOL drv_hwreg_srccap_hdmirx_frl_init(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_frl_deinit(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_frl_suspend(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_frl_resume(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_frl_rst(
	E_MUX_INPUTPORT enInputPortType);


extern MS_U8 drv_hwreg_srccap_hdmirx_frl_get_frlrate(
	E_MUX_INPUTPORT enInputPortType);


extern MS_BOOL drv_hwreg_srccap_hdmirx_frl_destable(
	E_MUX_INPUTPORT enInputPortType);


extern MS_U32 drv_hwreg_srccap_hdmirx_frl_get_clkratekhz(
	E_MUX_INPUTPORT enInputPortType,
	enum e_hdmi_clk_domain clk_domain);

extern MS_BOOL drv_hwreg_srccap_hdmirx_frl_get_errcntstatus(
	MS_U8 ucFRLRate,
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_ERROR_STATUS *pstErrorStatus);



#endif // _HWREG_SRCCAP_HDMIRX_FRL_H_
