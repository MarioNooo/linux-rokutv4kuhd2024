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

#ifndef _HWREG_SRCCAP_HDMIRX_EDID_H_
#define _HWREG_SRCCAP_HDMIRX_EDID_H_

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

extern MS_BOOL drv_hwreg_srccap_hdmirx_edid_init(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_edid_deinit(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_edid_suspend(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_edid_resume(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_edid_rst(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType);
extern void drv_hwreg_srccap_hdmirx_edid_writeedid(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType,
	MS_U8 *u8EDID,
	MS_U16 u16EDIDSize);
extern void drv_hwreg_srccap_hdmirx_edid_readedid(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType,
	MS_U8 *u8EDID,
	MS_U16 u16EDIDSize);
extern void drv_hwreg_srccap_hdmirx_edid_ddcram_en(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType);
extern void drv_hwreg_srccap_hdmirx_edid_ddcram_dis(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType);
extern void drv_hwreg_srccap_hdmirx_edid_loadedidnewmode(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType,
	MS_U16 u16EDIDSize,
	MS_U8 *pu8EDIDTable);
extern void drv_hwreg_srccap_hdmirx_edid_readedidnewmode(
	E_XC_DDCRAM_PROG_TYPE eDDCRamType,
	MS_U16 u16EDIDSize,
	MS_U8 *pu8EDIDTable);

#endif // _HWREG_SRCCAP_HDMIRX_EDID_H_
