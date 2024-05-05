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
#ifndef _DRV_HW_DYNAMICSCALING_H_
#define _DRV_HW_DYNAMICSCALING_H_

#include "hwreg_common.h"

enum hw_ds_win_idx {
	E_HW_DS_WIN_0,
	E_HW_DS_WIN_1,
	E_HW_DS_WIN_2,
	E_HW_DS_WIN_3,
	E_HW_DS_WIN_4,
	E_HW_DS_WIN_5,
	E_HW_DS_WIN_6,
	E_HW_DS_WIN_7,
	E_HW_DS_WIN_8,
	E_HW_DS_WIN_9,
	E_HW_DS_WIN_10,
	E_HW_DS_WIN_11,
	E_HW_DS_WIN_12,
	E_HW_DS_WIN_13,
	E_HW_DS_WIN_14,
	E_HW_DS_WIN_15,
	E_HW_DS_WIN_MAX
};

enum hw_ds_sync {
	E_HW_DS_SRC0_SYNC,
	E_HW_DS_SRC1_SYNC,
	E_HW_DS_IP_SYNC,
	E_HW_DS_OPF_SYNC,
	E_HW_DS_OPB_SYNC,
	E_HW_DS_OP2_SYNC,
	E_HW_DS_DISP_SYNC,
	E_HW_DS_SYNC_MAX,
};

enum hw_ds_reg_type {
	E_HW_DS_REG_ENABLE,
	E_HW_DS_REG_IDX_DEPTH,
	E_HW_DS_REG_BASE_ADDR,
	E_HW_DS_REG_MAX,
};

enum hw_ds_disp_reg_type {
	E_HW_DS_DISP_REG_ENABLE,
	E_HW_DS_DISP_REG_IDX_DEPTH,
	E_HW_DS_DISP_REG_BASE_ADDR,
	E_HW_DS_DISP_REG_INDEX_UPDATE_ENABLE,
	E_HW_DS_DISP_REG_INDEX_SEL,
	E_HW_DS_DISP_REG_MAX,
};

enum hw_ds_index_select_type {
	E_HW_DS_INDEX_SELECT_IP,
	E_HW_DS_INDEX_SELECT_OPF,
	E_HW_DS_INDEX_SELECT_OPB,
	E_HW_DS_INDEX_SELECT_OP2,
	E_HW_DS_INDEX_SELECT_DISP,
	E_HW_DS_INDEX_SELECT_MAX,
};

enum hw_ds_source {
	E_HW_DS_SOURCE_SRC0,
	E_HW_DS_SOURCE_SRC1,
	E_HW_DS_SOURCE_B2R,
	E_HW_DS_SOURCE_MAX,
};

void drv_hw_ds_get_index_select_addr(
	enum hw_ds_win_idx win_id,
	enum hw_ds_index_select_type type_id,
	struct reg_info *reg);

void drv_hw_ds_get_dummy_register(
	enum hw_ds_win_idx win_id,
	__u16 mem_index,
	struct reg_info *reg);

void drv_hw_ds_get_null_register(struct reg_info *reg);

void drv_hw_ds_set_register(struct reg_info *reg);

__u16 drv_hw_ds_read_register(struct reg_info *reg);

void drv_hw_ds_get_index_gen_enable_addr(
	enum hw_ds_source src_id,
	struct reg_info *reg);

void drv_hw_ds_get_force_index_addr(
	enum hw_ds_source src_id,
	struct reg_info *reg);

void drv_hw_ds_get_force_index_enable_addr(
	enum hw_ds_source src_id,
	struct reg_info *reg);

void drv_hw_ds_domain_get_addr(
	enum hw_ds_win_idx win_id,
	enum hw_ds_sync sync_id,
	__u16 reg_type,
	struct reg_info *reg,
	__u16 cnt);

void drv_hw_ds_map_disp_domain_reg(
	enum hw_ds_win_idx win_id,
	enum hw_ds_disp_reg_type reg_type,
	struct reg_info *reg,
	__u16 cnt);

void drv_hw_ds_set_domain_max_index(
	__u16 index,
	enum hw_ds_sync sync_id);

void drv_hw_ds_64_bits_command_enable(bool enable);

void drv_hw_ds_spread_mode_enable(bool enable);

void drv_hw_ds_set_miu_bus_sel(void);

void drv_hw_ds_set_dma_threthold(void);

void drv_hw_ds_set_dma_request_length(void);

void drv_hw_ds_set_bit_mast_enable(bool enable);

void drv_hw_ds_set_riu_write_enable(bool enable);

void drv_hw_ds_set_index_restriction_enable(bool enable);

void drv_hw_ds_set_recognize_bk(bool enable);

bool drv_hw_ds_get_recognize_bk(void);

void drv_hw_ds_get_str_ldr_reg(
	enum hw_ds_sync sync_id,
	__u16 index,
	struct reg_info *reg);

bool drv_hw_ds_get_bus_base_addr(
	size_t addr,
	size_t *bus_addr,
	size_t *base_addr);

__u64 drv_hw_ds_gen_64bits_cmd(__u32 addr, __u32 mask, __u32 data);

void drv_hw_ds_get_ip_idx_register(struct reg_info *reg);

void drv_hw_ds_get_opf_idx_register(struct reg_info *reg);

void drv_hw_ds_get_opb_idx_register(struct reg_info *reg);

void drv_hw_ds_get_op2_idx_register(struct reg_info *reg);
#endif //_DRV_HW_DYNAMICSCALING_H_