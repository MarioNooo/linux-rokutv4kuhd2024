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

#ifndef _HWREG_SRCCAP_HDMIRX_TMDS_H_
#define _HWREG_SRCCAP_HDMIRX_TMDS_H_

#include "hwreg_common.h"
#include "hwreg_srccap_hdmirx_mac.h"

#include "drvXC_IOPort.h"
// #include "drvXC_HDMI_if.h"

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

typedef struct
{
    MS_BOOL bWordBoundaryErrorL0;
    MS_BOOL bWordBoundaryErrorL1;
    MS_BOOL bWordBoundaryErrorL2;
    MS_BOOL bWordBoundaryErrorL3;
    MS_U16 u16DecodeErrorL0;
    MS_U16 u16DecodeErrorL1;
    MS_U16 u16DecodeErrorL2;
    MS_U16 u16DecodeErrorL3;
}stHDMI_ERROR_STATUS;

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
extern MS_BOOL drv_hwreg_srccap_hdmirx_tmds_init(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_tmds_deinit(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_tmds_suspend(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_tmds_resume(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_tmds_rst(
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_tmds_set_autoscramble(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL b_en);
extern void drv_hwreg_srccap_hdmirx_tmds_set_chktxdcbalance(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL b_en);
extern MS_BOOL drv_hwreg_srccap_hdmirx_tmds_get_destable(
	E_MUX_INPUTPORT enInputPortType);

extern MS_U32 drv_hwreg_srccap_hdmirx_tmds_get_clkratekhz(
	E_MUX_INPUTPORT enInputPortType,
	enum e_hdmi_clk_domain e_clk);

extern MS_BOOL drv_hwreg_srccap_hdmirx_tmds_get_errcntstatus(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL bHDMI20Flag,
	stHDMI_ERROR_STATUS *pstErrorStatus);

#endif // _HWREG_SRCCAP_HDMIRX_TMDS_H_

