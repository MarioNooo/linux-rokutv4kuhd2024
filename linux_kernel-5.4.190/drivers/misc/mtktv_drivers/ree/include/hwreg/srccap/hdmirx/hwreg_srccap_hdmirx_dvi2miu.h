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

#ifndef _HWREG_SRCCAP_HDMIRX_DVI2MIU_H_
#define _HWREG_SRCCAP_HDMIRX_DVI2MIU_H_

#include "mdrv_types_internal.h"
#include "hwreg_common.h"
#include "hwreg_srccap_hdmirx_struct.h"
#include "hwreg_srccap_hdmirx_pkt_hdr.h"


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

extern MS_BOOL drv_hwreg_srccap_hdmirx_dvi2miu_init(
	enum E_DVI2MIU_MUX dvi2miu_mux,
	phys_addr_t phy_mem_addr_for_arm,
	MS_U32 u32_mem_len);
extern MS_BOOL drv_hwreg_srccap_hdmirx_dvi2miu_deinit(
	enum E_DVI2MIU_MUX dvi2miu_mux);
extern MS_BOOL drv_hwreg_srccap_hdmirx_dvi2miu_suspend(
	enum E_DVI2MIU_MUX dvi2miu_mux);
extern MS_BOOL drv_hwreg_srccap_hdmirx_dvi2miu_resume(
	enum E_DVI2MIU_MUX dvi2miu_mux);
extern MS_BOOL drv_hwreg_srccap_hdmirx_dvi2miu_rst(
	enum E_DVI2MIU_MUX dvi2miu_mux);

extern MS_U16 drv_hwreg_srccap_hdmirx_dvi2miu_get_gnl(
	enum E_DVI2MIU_MUX dvi2miu_mux,
	enum e_pkt_type_indx e_pkt,
	struct pkt_prototype_gnl* gnl_ptr,
	MS_U32 u32_buf_len);

extern void drv_hwreg_srccap_hdmirx_dvi2miu_isr(
	enum E_DVI2MIU_MUX dvi2miu_mux);

extern MS_BOOL drv_hwreg_srccap_hdmirx_dvi2miu_irq_w_msk(
	enum E_DVI2MIU_MUX dvi2miu_mux,
	MS_U32 u32_msk);

extern MS_U32 drv_hwreg_srccap_hdmirx_dvi2miu_irq_r_msk(
	enum E_DVI2MIU_MUX dvi2miu_mux);

extern MS_BOOL drv_hwreg_srccap_hdmirx_dvi2miu_irq_clr_status(
	enum E_DVI2MIU_MUX dvi2miu_mux,
	MS_U32 u32_evt_clr);

extern MS_U32 drv_hwreg_srccap_hdmirx_dvi2miu_irq_r_status(
	enum E_DVI2MIU_MUX dvi2miu_mux);

extern void drv_hwreg_srccap_hdmirx_dvi2miu_report(
	MS_U32 u32_cnt);

#endif // _HWREG_SRCCAP_HDMIRX_DVI2MIU_H_
