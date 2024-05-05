//<Mediatek Software>
//******************************************************************************
// Mediatek Software
// Copyright (c) 2020 - 2024 Mediatek Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("Mediatek Software") are
// intellectual property of Mediatek Semiconductor, Inc. ("Mediatek") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of Mediatek Software is expressly prohibited, unless prior written
// permission has been granted by Mediatek.
//
// By accessing, browsing and/or using Mediatek Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. Mediatek shall retain any and all right, ownership and interest to Mediatek
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to Mediatek Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that Mediatek Software might include, incorporate or be
//    supplied together with third party`s software and the use of Mediatek
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. Mediatek Software and any modification/derivatives thereof shall be deemed as
//    Mediatek`s confidential information and you agree to keep Mediatek`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. Mediatek Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by Mediatek, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against Mediatek for any loss, damage, cost or expense that you may
//    incur related to your use of Mediatek Software.
//    In no event shall Mediatek be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if Mediatek Software has been modified by Mediatek in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, Mediatek may from time to time provide technical supports or
//    services in relation with Mediatek Software to you for your use of
//    Mediatek Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use Mediatek name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating Mediatek or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020-2024 MediaTek Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MediaTek Inc. and be kept in strict confidence
// ("MediaTek Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MediaTek Confidential
// Information is unlawful and strictly prohibited. MediaTek hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_HW_PIXELMONITOR_H_
#define _DRV_HW_PIXELMONITOR_H_

//-----------------------------------------------------------------------------
//  Include Files
//-----------------------------------------------------------------------------
#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/kernel.h>
#else
#include "pqu_port.h"
#endif
#include "hwreg_common.h"

//-----------------------------------------------------------------------------
//  Enums
//-----------------------------------------------------------------------------
enum hw_pxm_point {
	E_HW_PXM_POINT_PRE_IP2_0_INPUT = 0,
	E_HW_PXM_POINT_PRE_IP2_1_INPUT,
	E_HW_PXM_POINT_IP2_INPUT,
	E_HW_PXM_POINT_IP2_HDR_INPUT,
	E_HW_PXM_POINT_VOP_INPUT,
	E_HW_PXM_POINT_TCON_AFTER_OSD,
	E_HW_PXM_POINT_TCON_BEFORE_MOD,
	E_HW_PXM_POINT_GFX,
	E_HW_PXM_POINT_TCON_DELTA_PATH,
	E_HW_PXM_POINT_DV_HW5_SCDV,
	E_HW_PXM_POINT_DV_HW5_SCTCON,
	E_HW_PXM_POINT_MAX          //< pixel monitor point max >
};

//-----------------------------------------------------------------------------
//  Structurs
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Interface Functions
//-----------------------------------------------------------------------------
void drv_hw_pxm_write_register(__u32 addr, __u32 mask, __u32 data);
__u32 drv_hw_pxm_read_register(__u32 addr, __u32 mask);
void drv_hw_pxm_get_report_enable_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_report_once_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_report_done_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_display_enable_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_h_pos_in_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_v_pos_in_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_color_r_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_color_g_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_color_b_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_alpha_value_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_r_report_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_g_report_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_b_report_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_alpha_report_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_reset_cnt_soft_mode_addr(enum hw_pxm_point point, struct reg_info *reg);
void drv_hw_pxm_get_h_size_addr(enum hw_pxm_point point, struct reg_info *reg);

#endif //_DRV_HW_PIXELMONITOR_H_
