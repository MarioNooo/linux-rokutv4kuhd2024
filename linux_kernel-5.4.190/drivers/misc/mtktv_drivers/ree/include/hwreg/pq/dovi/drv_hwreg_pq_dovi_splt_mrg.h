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
/// @file   drv_hwreg_pq_dovi_splt_mrg.h
/// @brief  DoVi Core IP Split & Merge Regsiter driver
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_HWREG_PQ_DOVI_SPLT_MRG_H_
#define _DRV_HWREG_PQ_DOVI_SPLT_MRG_H_

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

struct st_dovi_reg_splt_mrg_param_in {
	bool is_enable;
	uint16_t width;
	uint16_t height;
};


struct st_dovi_reg_splt_mrg_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};


//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------

int drv_hwreg_pq_dovi_splt_mrg_enable(struct st_dovi_reg_splt_mrg_param_in *param_in, struct st_dovi_reg_splt_mrg_param_out *param_out);


#endif // _DRV_HWREG_PQ_DOVI_SPLT_MRG_H_
