// herein regardless in any format shall remain the sole proprietary of
// MediaTek Inc. and be kept in strict confidence
// ("MediaTek Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MediaTek Confidential
// Information is unlawful and strictly prohibited. MediaTek hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
/// @file   drv_hwreg_pq_dovi_hw5_top.h
/// @brief  DoVi Core IP HW5 HW Regsiter driver
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_HWREG_PQ_DOVI_HW5_TOP_H_
#define _DRV_HWREG_PQ_DOVI_HW5_TOP_H_

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
enum en_hwreg_dovi_input_source {
	EN_HWREG_DOVI_INPUT_SRC_HDMI_0 = 0,
	EN_HWREG_DOVI_INPUT_SRC_HDMI_1,
	EN_HWREG_DOVI_INPUT_SRC_B2R,

	EN_HWREG_DOVI_INPUT_SRC_MAX
};

enum reg_pq_dovi_hw5_top_dvfe_input {
	REG_PQ_DOVI_HW5_TOP_DVFE_INPUT_FB = 0,
	REG_PQ_DOVI_HW5_TOP_DVFE_INPUT_FBL_IDW0,
	REG_PQ_DOVI_HW5_TOP_DVFE_INPUT_FBL_IDW1,
	REG_PQ_DOVI_HW5_TOP_DVFE_INPUT_B2R_SUB1,
};

struct st_dovi_reg_hw5_top_param_in {
	enum en_hwreg_dovi_input_source enInputSrc;
	bool bDvEnable;
	bool bC2ResetOn;
	bool bC1ResetOn;
	bool bC1bResetOn;
	bool bC2Tv5ResetOn;
	enum reg_pq_dovi_hw5_top_dvfe_input dvfe_input;
	bool bFEEnable;
	bool bBEEnable;
	bool bIsDvAtIp;
	bool bHFREnable;
	bool bIDKEnable;
};

struct st_dovi_reg_hw5_top_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};

//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
int drv_hwreg_pq_dovi_hw5_top_setDvOp422(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

int drv_hwreg_pq_dovi_hw5_top_setDvPosition(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

int drv_hwreg_pq_dovi_hw5_top_setDvReset(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

int drv_hwreg_pq_dovi_hw5_top_setDebugDvBypass(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

int drv_hwreg_pq_dovi_hw5_top_setDolbyPath(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

int drv_hwreg_pq_dovi_hw5_top_set_dvfe_input(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

int drv_hwreg_pq_dovi_hw5_top_fe_core_en(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

int drv_hwreg_pq_dovi_hw5_top_be_core_en(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

void drv_hwreg_pq_enhance_idk_write2byte(
	uint32_t u32P_Addr,
	uint32_t u32Value);

void drv_hwreg_pq_enhance_idk_write2bytemask(
	uint32_t u32P_Addr,
	uint32_t u32Value,
	uint32_t fld);

int drv_hwreg_pq_dovi_hw5_top_set_pre_ip2_dvpath_disable(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);
int drv_hwreg_pq_dovi_hw5_top_c2_tv5_en(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

int drv_hwreg_pq_dovi_hw5_top_hdr_ltmo_en(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

int drv_hwreg_pq_dovi_hw5_top_pr_1p_en(
	struct st_dovi_reg_hw5_top_param_in *param_in,
	struct st_dovi_reg_hw5_top_param_out *param_out);

#endif // _DRV_HWREG_PQ_DOVI_HW5_TOP_H_
