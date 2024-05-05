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

#ifndef _HWREG_SRCCAP_HDMIRX_PHY_H_
#define _HWREG_SRCCAP_HDMIRX_PHY_H_

#include "hwreg_common.h"

#include "mdrv_types_internal.h"
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

extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_init(void);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_deinit(void);

extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_init_p(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_deinit_p(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_suspend(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_resume(
	E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_rst(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_get_frltraindone(
	stHDMI_POLLING_INFO *p_info,
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_get_clklosedet(
	E_MUX_INPUTPORT enInputPortType);


extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_get_clkchgflag(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *p_info);

extern void drv_hwreg_srccap_hdmirx_phy_set_eq(
	E_MUX_INPUTPORT enInputPortType,
	MS_HDMI_EQ enEq,
	MS_U8 u8_val);

extern void drv_hwreg_srccap_hdmirx_phy_set_powerdown(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *p_info,
	MS_BOOL b_powerdown);

extern void drv_hwreg_srccap_hdmirx_phy_set_clkpulllow(
	MS_BOOL bPullLow,
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_phy_set_datarterm_ctrl(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL b_onoff);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_get_phydone(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *p_info);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_test_command_parser(
	MS_U16 u16InputCommandType,
	MS_U16 u16InputBuffLength ,
	MS_U8 *u8InputBuff,
	MS_U16 *u16OutputBuffLength,
	MS_U8  *u8OutputBuff,
	int *intStrSize,
	char *buf);
extern MS_U16 drv_hwreg_srccap_hdmirx_phy_getMBXParaFromHDMICPU(
    E_MUX_INPUTPORT enInputPortType, MS_U16 u16InputParamIndex);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_setMBXParaToHDMICPU(
    E_MUX_INPUTPORT enInputPortType, MS_U16 u16InputParamIndex,
    MS_U16 u16InputData);
extern MS_U16 drv_hwreg_srccap_hdmirx_phy_getMBXInstFlag(
    E_MUX_INPUTPORT enInputPortType, MS_U16 u16MBXFlagIdx);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_initMBXPara(void);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_setMBXHKInstFlag(
    E_MUX_INPUTPORT enInputPortType, MS_U16 u16MBXFlagIdx, MS_U16 u16InputData);
extern MS_BOOL drv_hwreg_srccap_hdmirx_phy_updateHKInstantStatus(
    E_MUX_INPUTPORT enInputPortType, stHDMI_POLLING_INFO *pstHDMIPollingInfo);

#endif // _HWREG_SRCCAP_HDMIRX_PHY_H_
