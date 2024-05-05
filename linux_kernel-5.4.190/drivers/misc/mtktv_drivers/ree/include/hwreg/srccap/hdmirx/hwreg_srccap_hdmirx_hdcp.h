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

#ifndef _HWREG_SRCCAP_HDMIRX_HDCP_H_
#define _HWREG_SRCCAP_HDMIRX_HDCP_H_

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
enum e_hdmi_hdcp_isr_list {
	E_HDMI_HDCP22_CLR_BCH_ERR,
	E_HDMI_HDCP_ISR_MAX,
};

enum e_hdmi_hdcp22_status {
	E_HDMI_HDCP_MSG_FIFO_READ_ABORT = 0,
	E_HDMI_HDCP_AKE_INIT,
	E_HDMI_HDCP_MSG_FIFO_WRITE_DONE,
	E_HDMI_HDCP_MSG_FIFO_READ_DONE,
	E_HDMI_HDCP_MSG_FIFO_WRITE_START,
	E_HDMI_HDCP_MSG_FIFO_READ_START,
	E_HDMI_HDCP2_VER,
	E_HDMI_HDCP_PKT_OVERLAP_KEEPOUT,
	E_HDMI_HDCP_LINK_INTEGRITY_CHK,
	E_HDMI_HDCP_WRITE_AKE,
	E_HDMI_HDCP_WRITE_AN = 10,
	E_HDMI_HDCP_KM_GENERATED,
	E_HDMI_HDCP_READ_BKSV,
	E_HDMI_HDCP_READ_R0,
	E_HDMI_HDCP_READ_RI = 14,
};

enum e_hdmi_hdcp_x74_info_list{
	E_HDMI_HDCP_00_BKSV = 0,
	E_HDMI_HDCP_08_RI = 8,
	E_HDMI_HDCP_0A_RJ = 10,
	E_HDMI_HDCP_10_AKSV = 16,
	E_HDMI_HDCP_15_AINFO = 21,
	E_HDMI_HDCP_18_AN = 24,
	E_HDMI_HDCP_20_VH0 = 32,
	E_HDMI_HDCP_24_VH1 = 36,
	E_HDMI_HDCP_28_VH2 = 40,
	E_HDMI_HDCP_2C_VH3 = 44,
	E_HDMI_HDCP_30_VH4 = 48,
	E_HDMI_HDCP_40_BCAPS = 64,
	E_HDMI_HDCP_41_BSTATUS_L = 65,
	E_HDMI_HDCP_42_BSTATUS_H,
	E_HDMI_HDCP_43_KSVFIFO = 67,
	E_HDMI_HDCP_C0_DBG = 192,
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */

extern MS_BOOL drv_hwreg_srccap_hdmirx_hdcp_init(
    MS_U8 ucPortIdx);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hdcp_deinit(
    MS_U8 ucPortIdx);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hdcp_suspend(
    MS_U8 ucPortIdx);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hdcp_resume(
    MS_U8 ucPortIdx);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hdcp_rst(
    MS_U8 ucPortIdx);

extern void drv_hwreg_srccap_hdmirx_hdcp_isr(
	MS_U8 ucPortIdx,
	enum e_hdmi_hdcp_isr_list e_isr_n,
	void* param1);

extern void drv_hwreg_srccap_hdmirx_hdcp_22_init(
    MS_U8 ucPortIdx);

extern void drv_hwreg_srccap_hdmirx_hdcp_22_setready_bit(
	MS_U8 ucPortIdx,
	MS_BOOL IsReady);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hdcp_22_polling_writedone(
	MS_U8 ucPortIdx);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hdcp_22_polling_readdone(
	MS_U8 ucPortIdx);

extern MS_BOOL
drv_hwreg_srccap_hdmirx_hdcp_22_polling_r0_readdone(
	MS_U8 ucPortIdx);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hdcp_22_recvmsg(
	MS_U8 ucPortIdx, MS_U8 *ucMsgData);

extern void drv_hwreg_srccap_hdmirx_hdcp_22_fetchmsg(
	MS_U8 ucPortIdx,
	MS_U8 *pucData,
	MS_U32 dwDataLen);

extern void drv_hwreg_srccap_hdmirx_hdcp_22_sendmsg(
	MS_U8 ucPortType,
	MS_U8 ucPortIdx,
	MS_U8 *pucData,
	MS_U32 dwDataLen,
	void *pDummy,
	MS_U8 *ubRecIDListSetDone);

extern E_HDMI_HDCP_STATE  drv_hwreg_srccap_hdmirx_hdcp_get_authver(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL bSetFlag);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hdcp_get_encryptionflag(
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_hdcp_clearflag(
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_hdcp_clearstatus(
	E_MUX_INPUTPORT enInputPortType,
	MS_U16 u16_flag);

extern MS_U16 drv_hwreg_srccap_hdmirx_hdcp_getstatus(
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_hdcp_ddc_en(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL b_onoff);

extern MS_BOOL drv_hwreg_srccap_hdmirx_hdcp_writex74(
	E_MUX_INPUTPORT enInputPortType,
	MS_U8 ubOffset,
	MS_U8 ubData);

extern MS_U8 drv_hwreg_srccap_hdmirx_hdcp_readx74(
	E_MUX_INPUTPORT enInputPortType,
	MS_U8 ubOffset);


extern void drv_hwreg_srccap_hdmirx_hdcp_setready(
	E_MUX_INPUTPORT enInputPortType, MS_BOOL bIsReady);

extern void drv_hwreg_srccap_hdmirx_hdcp_write_ksv_list(
	E_MUX_INPUTPORT enInputPortType,
	MS_U8 *pucData,
	MS_U32 ulDataLen);

extern void drv_hwreg_srccap_hdmirx_hdcp_writedone_irq_enable(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL b_en);

extern void drv_hwreg_srccap_hdmirx_hdcp_readr0_irq_enable(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL b_en);

extern MS_U16 drv_hwreg_srccap_hdmirx_hdcp_22_intstatus(
	E_MUX_INPUTPORT enInputPortType,
	enum e_hdmi_hdcp22_status e_status);

extern MS_U32 drv_hwreg_srccap_hdmirx_hdcp_irq_evt(
	E_MUX_INPUTPORT enInputPortType);

#endif // _HWREG_SRCCAP_HDMIRX_HDCP_H_
