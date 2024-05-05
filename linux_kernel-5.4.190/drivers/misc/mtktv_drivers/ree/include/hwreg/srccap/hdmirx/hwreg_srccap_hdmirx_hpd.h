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

#ifndef _HWREG_SRCCAP_HDMIRX_HPD_H_
#define _HWREG_SRCCAP_HDMIRX_HPD_H_

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

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */

extern MS_BOOL drv_hwreg_srccap_hdmirx_hpd_init(
	E_MUX_INPUTPORT enInputPortType);

extern  MS_BOOL drv_hwreg_srccap_hdmirx_hpd_deinit(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hpd_suspend(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hpd_resume(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hpd_rst(
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_hpd_pull(
	MS_BOOL bHighLow,
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL bInverse,
	MS_U8 ucMHLSupportPath);


#endif // _HWREG_SRCCAP_HDMIRX_HPD_H_