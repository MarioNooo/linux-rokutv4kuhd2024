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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_HWREG_ADL_H_
#define _DRV_HWREG_ADL_H_

#ifdef _DRV_HWREG_ADL_C_

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/kernel.h>
#else
#include "pqu_port.h"
#endif
#include "hwreg_common.h"

#ifndef INTERFACE
#define INTERFACE
#endif
#else
#ifndef INTERFACE
#define INTERFACE	extern
#endif
#endif

enum hw_adl_client {
	/*TCON Auto download*/
	E_HW_TCON_ADL_PANEL_GAMMA_0 = 1,    // 1
	E_HW_TCON_ADL_DEMURA_0 = E_HW_TCON_ADL_PANEL_GAMMA_0+2,
	E_HW_TCON_ADL_GOP_0,
	E_HW_TCON_ADL_GOP_1,
	E_HW_TCON_ADL_GOP_2,
	E_HW_TCON_ADL_GOP_3,
	E_HW_TCON_ADL_LD_3,
	E_HW_TCON_ADL_LD_1,
	E_HW_TCON_ADL_LD_2 = E_HW_TCON_ADL_LD_1 + 2,
	E_HW_TCON_ADL_LD_0,
	E_HW_TCON_ADL_PQ_RESERVE_8, // 13
	E_HW_TCON_ADL_PQ_GAMMA = E_HW_TCON_ADL_PQ_RESERVE_8 + 2, // 15
	E_HW_TCON_ADL_CLIENT_TCON_MAX = E_HW_TCON_ADL_PQ_GAMMA,  // 15


	/*NR Auto download*/
	E_HW_NR_ADL_FILM_GRAIN_0 = E_HW_TCON_ADL_CLIENT_TCON_MAX + 1,  // 16
	E_HW_NR_ADL_UCD_0,
	E_HW_NR_ADL_HDR_0,
	E_HW_NR_ADL_MPEG_NR_0 = E_HW_NR_ADL_HDR_0 + 4,  // 22
	E_HW_NR_ADL_NR_RESERVED_0,
	E_HW_NR_ADL_MPEG_NR_1,                        // 24
	E_HW_NR_ADL_CLIENT_DV_HW5_LUT_C1_V2,       // 25 only for HW version >=M6L_HW
	E_HW_NR_ADL_CLIENT_DV_HW5_LUT_R_V2,        // 26 only for HW version >=M6L_HW
	E_HW_NR_ADL_CLIENT_NR_MAX = E_HW_NR_ADL_CLIENT_DV_HW5_LUT_R_V2, // 26

	/*MW Auto download*/
	E_HW_MW_ADL_PQ_RESERVE_0 = E_HW_NR_ADL_CLIENT_NR_MAX + 1,  // 27
	E_HW_MW_ADL_PQ_RESERVE_1,
	E_HW_MW_ADL_PQ_RESERVE_2,
	E_HW_MW_ADL_HVSP_0,
	E_HW_MW_ADL_PQ_RESERVE_3,
	E_HW_MW_ADL_VIP_1,
	E_HW_MW_ADL_PQ_RESERVE_4,
	E_HW_MW_ADL_PQ_RESERVE_5,
	E_HW_MW_ADL_PQ_RESERVE_6,
	E_HW_MW_ADL_PQ_RESERVE_7,
	E_HW_MW_ADL_SR_0,
	E_HW_MW_ADL_CE_1,
	E_HW_MW_ADL_VOP_0,
	E_HW_MW_ADL_VOP_1,
	E_HW_MW_ADL_PQ_GAMMA_0,
	E_HW_MW_ADL_CE_0,
	E_HW_MW_ADL_VIP_0,
	E_HW_MW_ADL_UCD_2,
	E_HW_MW_ADL_UCD_3,
	E_HW_MW_ADL_UCD_1,
	E_HW_MW_ADL_LCE_0,
	E_HW_MW_ADL_HVSP_1,
	E_HW_MW_ADL_DIP_0,
	E_HW_MW_ADL_DIP_1,     // 50
	E_HW_MW_ADL_CLIENT_MW_MAX = E_HW_MW_ADL_DIP_1,    // 50
	E_HW_ADL_CLIENT_MAX,
};

enum hw_adl_sub_client {
	/* HDR_SUB_CLIENT*/
	E_HW_ADL_HDR_SUB_TMO,
	E_HW_ADL_HDR_SUB_GAMMA,
	E_HW_ADL_HDR_SUB_OOTF,
	E_HW_ADL_HDR_SUB_DEGAMMA,

	/* XVYCC_SUB_CLIENT*/
	E_HW_ADL_XVYCC_SUB_GAMMA,
	E_HW_ADL_XVYCC_SUB_DEGAMMA,

	E_HW_ADL_SUB_CLIENT_MAX,
};

enum hw_adl_work_mode {
	E_HW_ADL_TRIGGER_MODE,
	E_HW_ADL_ENABLE_MODE,
	E_HW_ADL_SW_TRIGGER_MODE,
};

struct hw_adl_setting_info {
	struct reg_info *reginfo;
	__u8 cnt;

	__u8 byte_per_word;
	size_t miu_bus_addr;
};

INTERFACE SYMBOL_WEAK void drv_hwreg_adl_init(void);
INTERFACE SYMBOL_WEAK void drv_hwreg_adl_getreginfo(enum hw_adl_client client_type,
							struct hw_adl_setting_info *pstadlRegInfo);
INTERFACE SYMBOL_WEAK bool drv_hwreg_adl_checkdone(enum hw_adl_client client_type);
INTERFACE SYMBOL_WEAK void drv_hwreg_adl_setclkgate(enum hw_adl_client client_type, bool bEnable);
INTERFACE SYMBOL_WEAK void drv_hwreg_adl_setworkmode(enum hw_adl_client client_type,
							enum hw_adl_work_mode enMode);
INTERFACE SYMBOL_WEAK void drv_hwreg_adl_setswtrig(enum hw_adl_client client_type);
INTERFACE SYMBOL_WEAK void drv_hwreg_adl_setbaseaddr(enum hw_adl_client client_type,
							size_t phyBaseAddr);
INTERFACE SYMBOL_WEAK void drv_hwreg_adl_setdepth(enum hw_adl_client client_type, __u32 u32Depth);
INTERFACE SYMBOL_WEAK void drv_hwreg_adl_setreqlen(enum hw_adl_client client_type, __u32 u32Depth);
INTERFACE SYMBOL_WEAK void drv_hwreg_adl_enable(enum hw_adl_client client_type, bool bEnable);
INTERFACE SYMBOL_WEAK void drv_hwreg_adl_setswtrigen(enum hw_adl_client client_type, enum hw_adl_work_mode enMode);
INTERFACE SYMBOL_WEAK void drv_hwreg_adl_disable_newactivemd(void);

#undef INTERFACE
#endif		//_DRV_HWREG_ADL_H_
