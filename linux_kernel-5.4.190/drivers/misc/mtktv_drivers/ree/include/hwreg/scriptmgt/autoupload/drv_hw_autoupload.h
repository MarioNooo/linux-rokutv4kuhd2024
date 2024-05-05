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

////////////////////////////////////////////////////////////////////////////////
///
/// @author Mediatek Inc.
///
////////////////////////////////////////////////////////////////////////////////


#ifndef MDRV_HW_AUTOUPLOAD_H
#define MDRV_HW_AUTOUPLOAD_H
#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/kernel.h>
#else
#include "pqu_port.h"
#endif
#include "hwreg_common.h"

#define AUL_REG_MAX_SIZE (20)

#ifndef INTERFACE
#define INTERFACE
#endif

#ifndef SYMBOL_WEAK
#define SYMBOL_WEAK
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

enum en_hw_aul_client {
	E_HW_AUL_YHSL,
	E_HW_AUL_OBJ_HIST,
	E_HW_AUL_FREQ_DET,
	E_HW_AUL_YLITE,
	E_HW_AUL_DV_HW5_HIST_W,
	E_HW_AUL_MNC_STAT,
	E_HW_AUL_ZNR_ME,
	E_HW_AUL_ZNR_MI,
	E_HW_AUL_DBK_HW_SP,
	E_HW_AUL_LCE_SCL,
	E_HW_AUL_3D_HIST,
	E_HW_AUL_LCE_ACE,
	E_HW_AUL_AMB,
	E_HW_AUL_LOGO_DET,
	E_HW_AUL_RES_DET,
	E_HW_AUL_DBK,
	E_HW_AUL_MAX,
};

struct aul_hw_reg_info {
	struct reg_info reg_info_list[AUL_REG_MAX_SIZE];
	uint32_t regCount;
	};

struct aul_drv_hw_dts {
	bool support;
	__u32 client_depth;
	__u32 wdepth;
	__u32 buffercnt;
	};

INTERFACE bool drv_hw_aul_clean_done(enum en_hw_aul_client client_id);
INTERFACE bool drv_hw_aul_get_info(enum en_hw_aul_client client_id,
	size_t address, struct aul_hw_reg_info *hw_reg_info);
INTERFACE __u32 drv_hw_aul_get_memsize(enum en_hw_aul_client client_id);
INTERFACE bool drv_hw_aul_setting(enum en_hw_aul_client client_id, size_t address);
INTERFACE bool drv_hw_aul_set_dts_info(enum en_hw_aul_client client_id,struct aul_drv_hw_dts hwdts);
INTERFACE void drv_hw_aul_req_th(uint32_t val);
#endif

