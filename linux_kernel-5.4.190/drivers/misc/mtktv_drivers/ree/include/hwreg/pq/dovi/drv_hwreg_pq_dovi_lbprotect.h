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
/// @file   drv_hwreg_pq_dovi_lbprotect.h
/// @brief  DoVi Core IP LB Proetect Regsiter driver
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_HWREG_PQ_DOVI_LB_PROTECT_H_
#define _DRV_HWREG_PQ_DOVI_LB_PROTECT_H_

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
struct st_dovi_reg_lbprotect_param_in {
	bool is_lb_enable;
	bool is_lb_hprot_enable;
	uint16_t height;
};

struct st_dovi_reg_lbprotect_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};

//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
int drv_hwreg_pq_dovi_lbprotect_enable(
	struct st_dovi_reg_lbprotect_param_in *param_in,
	struct st_dovi_reg_lbprotect_param_out *param_out);

#endif // _DRV_HWREG_PQ_DOVI_LB_PROTECT_H_
