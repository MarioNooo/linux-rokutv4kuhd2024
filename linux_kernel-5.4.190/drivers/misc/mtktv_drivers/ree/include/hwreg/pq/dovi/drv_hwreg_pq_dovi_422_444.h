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

#ifndef _DRV_HWREG_PQ_DOVI_422_444_H_
#define _DRV_HWREG_PQ_DOVI_422_444_H_

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

//defined for the use of op422to444 and op444to422
struct reg_pq_dovi_422_444_param_in {
	bool is_at_op;
	uint8_t  usr_md;
};

struct reg_pq_dovi_422_444_param_out {
	struct hwregOut *hwOut;
	uint32_t u32RegSize;
};

//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
int drv_hwreg_pq_dovi_set_op1_422to444(
        struct reg_pq_dovi_422_444_param_in *param_in,
        struct reg_pq_dovi_422_444_param_out *param_out);

#endif // _DRV_HWREG_PQ_DOVI_422_444_H_
