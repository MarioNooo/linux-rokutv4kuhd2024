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

/////////////////////////////////////////////////////////////////////////////////
///
/// @author Mediatek Inc.
///
/////////////////////////////////////////////////////////////////////////////////

#ifndef MDRV_HW_MENULOAD_H
#define MDRV_HW_MENULOAD_H
#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/kernel.h>
#else
#include "pqu_port.h"
#endif
#include "hwreg_common.h"


#ifndef INTERFACE
#define INTERFACE
#endif

#ifndef SYMBOL_WEAK
#define SYMBOL_WEAK
#endif

INTERFACE bool drv_hw_mload_init(void);
INTERFACE bool drv_hw_mload_get_status(__u8 client_id);
INTERFACE bool drv_hw_mload_set_on_off(__u8 client_id, bool enable);
INTERFACE bool drv_hw_mload_set_len(__u8 client_id, __u16 len);
INTERFACE bool drv_hw_mload_set_base_addr(__u8 client_id, size_t addr);
INTERFACE bool drv_hw_mload_set_depth(__u8 client_id, __u16 depth);
INTERFACE __u64 drv_hw_mload_get_endcmd(__u8 client_id);
INTERFACE __u64 drv_hw_mload_get_donecmd(void);
INTERFACE void drv_hw_mload_riu(__u64 cmd);
INTERFACE void drv_hw_mload_set_vsynctype(__u8 vsync_id, bool mw_flag, __u8 client_id);
INTERFACE void drv_hw_mload_no_signal_force_swtrigger(__u8 vsync_id);
INTERFACE void drv_hw_mload_set_riu(bool enable);
INTERFACE bool drv_hw_mload_getmloadreg(__u8 client_id,
	struct reg_info *reg, __u8 cnt);
INTERFACE bool drv_hw_mload_fsc_is_lockop(void);
INTERFACE __u64 drv_hw_mload_gen_64bits_spreadmode(__u32 addr,
	__u32 data, __u32 mask);
INTERFACE __u64 drv_hw_mload_gen_64bits_spreadmode_ext(__u32 addr,__u32 data);
INTERFACE __u64 drv_hw_mload_get_null_cmd(void);
INTERFACE __u16 drv_hw_mload_get_reg_len(void);
INTERFACE __u16 drv_hw_mload_get_align_num(void);
INTERFACE __u8 drv_hw_mload_get_num_per_vsync(void);
INTERFACE __u32 drv_hw_mload_get_cmd_length(void);
INTERFACE __u32 drv_hw_mload_get_bus_width(void);
INTERFACE __u32 drv_hw_mload_get_mem_unit_size(void);
INTERFACE bool drv_hw_mload_parsing_64bits_spreadmode_normal(
	__u64 cmd, __u32 *addr, __u16 *data, __u16 *mask);

#endif

