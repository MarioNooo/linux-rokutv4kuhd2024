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

#ifndef _HWREG_SRCCAP_HDMIRX_IRQ_H_
#define _HWREG_SRCCAP_HDMIRX_IRQ_H_

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
extern MS_BOOL drv_hwreg_srccap_hdmirx_irq_init(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_irq_deinit(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_irq_suspend(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_irq_resume(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_irq_rst(
    E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_isr_phy(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern void drv_hwreg_srccap_hdmirx_isr_pkt_que(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern void drv_hwreg_srccap_hdmirx_isr_scdc(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern void drv_hwreg_srccap_hdmirx_isr_sqh_all_wk(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern void drv_hwreg_srccap_hdmirx_isr_clk_det(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern unsigned int drv_hwreg_srccap_hdmirx_irq_mask_save_global(
	EN_KDRV_HDMIRX_INT e_int);

extern void drv_hwreg_srccap_hdmirx_irq_mask_restore_global(
	EN_KDRV_HDMIRX_INT e_int, unsigned int msk);

extern unsigned int drv_hwreg_srccap_hdmirx_irq_status(
	EN_KDRV_HDMIRX_INT e_int,
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_irq_clr(
	EN_KDRV_HDMIRX_INT e_int,
	E_MUX_INPUTPORT enInputPortType,
	unsigned int clr_bit);

extern void drv_hwreg_srccap_hdmirx_irq_onoff(
	EN_KDRV_HDMIRX_INT e_int,
	E_MUX_INPUTPORT enInputPortType,
	unsigned int msk,
	MS_BOOL b_on);

#endif // _HWREG_SRCCAP_HDMIRX_IRQ_H_

