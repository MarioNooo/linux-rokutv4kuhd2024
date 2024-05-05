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
/// @file   drv_hwreg_pq_dovi_hw5_core.h
/// @brief  DoVi Core IP HW5 HW Regsiter driver
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_HWREG_PQ_DOVI_HW5_CORE_H_
#define _DRV_HWREG_PQ_DOVI_HW5_CORE_H_

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

struct st_dovi_reg_hw5_core_l1l4_value {
	uint16_t l1_min_val;
	uint16_t l1_max_val;
	uint16_t l1_mid_val;
	uint16_t l4_std_val;
};


//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------

int drv_hwreg_pq_dovi_hw5_core_getL1L4Output(
	struct st_dovi_reg_hw5_core_l1l4_value *l1l4_out);


#endif // _DRV_HWREG_PQ_DOVI_HW5_CORE_H_
