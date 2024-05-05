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

#ifndef _HWREG_SRCCAP_DV_DESCRB_H_
#define _HWREG_SRCCAP_DV_DESCRB_H_

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------
#include "hwreg_common.h"

//-----------------------------------------------------------------------------
// Driver Capability
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macros and Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Enums and Structures
//-----------------------------------------------------------------------------

/* stub mode ctrl bit */
enum hwreg_srccap_dv_descrb_stub_ctrl {
	HWREG_SRCCAP_DV_DESCRB_STUB_CTRL_NONE		= 0,
	HWREG_SRCCAP_DV_DESCRB_STUB_CTRL_NEG 		= 0x1,
};


//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
int drv_hwreg_srccap_dv_descrb_set_meta_length(
	uint8_t dev_id,
	uint16_t len,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_descrb_set_addr(
	uint8_t dev_id,
	uint16_t addr,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_descrb_set_r_enable(
	uint8_t dev_id,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_descrb_set_r_check(
	uint8_t dev_id,
	uint8_t flag,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_descrb_get_r_data(
	uint8_t dev_id,
	uint16_t *data);

int drv_hwreg_srccap_dv_descrb_get_r_check_rb(
	uint8_t dev_id,
	uint8_t *flag_rb);

int drv_hwreg_srccap_dv_descrb_set_pkt_num(
	uint8_t dev_id,
	uint8_t pkt_num,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_descrb_get_crc_reg(
	uint8_t dev_id,
	uint64_t *crc_reg);

int drv_hwreg_srccap_dv_descrb_set_crc_12b_444_enable(
	uint8_t dev_id,
	bool en,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_descrb_set_1248pack_mode(
	uint8_t dev_id,
	uint8_t np_mode,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_descrb_set_hsize(
	uint8_t dev_id,
	uint16_t hsize,
	bool riu,
	struct reg_info *reg,
	uint16_t *count);

int drv_hwreg_srccap_dv_descrb_set_stub_mode(
	bool enable,
	enum hwreg_srccap_dv_descrb_stub_ctrl stub_ctrl);


#endif // _HWREG_SRCCAP_DV_DESCRB_H_
