/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2019 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2019 MediaTek Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

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

INTERFACE void drv_hw_cmdq_en(bool enable);
INTERFACE void drv_hw_cmdq_wait_bus_config(__u8 wait_type_sel,
	bool wait_bus_sample_en);
INTERFACE void drv_hw_cmdq_req_len(__u8 length);
INTERFACE void drv_hw_cmdq_req_th(__u8 value);
INTERFACE void drv_hw_cmdq_dma_burst_config(bool mode);
INTERFACE void drv_hw_cmdq_dma_mode_config(__u8 buf_mode, bool trig_en);
INTERFACE void drv_hw_cmdq_jump_cmd_en(bool enable);
INTERFACE void drv_hw_cmdq_jump_cmd_amount(__u16 base_amount_0, __u16 base_amount_1, __u16 amount);
INTERFACE void drv_hw_cmdq_poll_wait_time_config(__u16 wait_time, __u8 ratio);
INTERFACE void drv_hw_cmdq_inv_xiu_wmask(bool value);
INTERFACE void drv_hw_cmdq_st_ptr_config(__u32 addr);
INTERFACE void drv_hw_cmdq_end_ptr_config(__u32 addr);
INTERFACE void drv_hw_cmdq_trigger(bool enable);
INTERFACE int drv_hw_cmdq_win_rst(bool enable, struct hwregOut *out);
INTERFACE int drv_hw_cmdq_trig_bus_clr(__u32 value, struct hwregOut *out);
INTERFACE int drv_hw_cmdq_int_trigger(bool enable, struct hwregOut *out);
INTERFACE void drv_hw_cmdq_soft_rst(bool value);
INTERFACE void drv_hw_cmdq_set_win_id(__u8 value, struct hwregOut *out);
INTERFACE void drv_hw_cmdq_set_skip_cmd(__u16 skip_ofst, __u16 skip_id, struct hwregOut *out);
INTERFACE bool drv_hw_cmdq_get_timeout(void);
INTERFACE void drv_hw_cmdq_clear_timeout(void);
INTERFACE void drv_hw_cmdq_set_ipdma_read(__u8 value, struct hwregOut *out);
INTERFACE bool drv_hw_cmdq_get_skip_win(void);
INTERFACE uint16_t drv_hw_cmdq_get_skip_win_num(void);
#endif

