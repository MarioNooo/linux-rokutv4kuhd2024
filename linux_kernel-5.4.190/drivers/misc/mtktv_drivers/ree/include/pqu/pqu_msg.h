/* SPDX-License-Identifier: GPL-2.0
 * Copyright (c) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _PQU_MSG_H_
#define _PQU_MSG_H_

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#else
#include "pqu_port.h"
#endif

//-----------------------------------------------------------------------------
//  Macro and Define
//-----------------------------------------------------------------------------
typedef void (*PQU_MSG_REPLY_FP)(int error_cause, pid_t thread_id, uint32_t user_param, void *param);

//-----------------------------------------------------------------------------
//  Type and Structure
//-----------------------------------------------------------------------------
enum PQU_MSG_SEND_ID {
	PQU_MSG_SEND_NEW_WIN,
	PQU_MSG_SEND_DESTROY_WIN,
	PQU_MSG_SEND_STREAM_ON,
	PQU_MSG_SEND_STREAM_OFF,
	PQU_MSG_SEND_QUEUE,
	PQU_MSG_SEND_CONFIG,
	PQU_MSG_SEND_DEMURA,
	PQU_MSG_SEND_PQMAP,
	PQU_MSG_SEND_GLOBAL_TIMING,
	PQU_MSG_SEND_LDM,
	PQU_MSG_SEND_PQMETA,
	PQU_MSG_SEND_PQPARAM,
	PQU_MSG_SEND_RENDER_PQSHAREMEM_INFO,
	PQU_MSG_SEND_RENDER_PQ_FIRE,
	PQU_MSG_SEND_DOLBY_GD,
	PQU_MSG_SEND_RENDER_BE_SHAREMEM_INFO,
	PQU_MSG_SEND_RENDER_BE_SETTING,
	PQU_MSG_SEND_LDM_INIT,
	PQU_MSG_SEND_QUEUE_SLICE,
	PQU_MSG_SEND_ID_MAX,
};

enum PQU_MSG_REPLY_ID {
	PQU_MSG_REPLY_NEW_WIN,
	PQU_MSG_REPLY_DESTROY_WIN,
	PQU_MSG_REPLY_STREAM_ON,
	PQU_MSG_REPLY_STREAM_OFF,
	PQU_MSG_REPLY_QUEUE,
	PQU_MSG_REPLY_DQUEUE,
	PQU_MSG_REPLY_LDM,
	PQU_MSG_REPLY_PQPARAM,

	PQU_MSG_REPLY_NEW_WIN_FOR_TEST,
	PQU_MSG_REPLY_DESTROY_WIN_FOR_TEST,
	PQU_MSG_REPLY_STREAM_ON_FOR_TEST,
	PQU_MSG_REPLY_STREAM_OFF_FOR_TEST,
	PQU_MSG_REPLY_QUEUE_FOR_TEST,
	PQU_MSG_REPLY_DQUEUE_FOR_TEST,

	PQU_MSG_REPLY_ID_MAX,
};

enum PQU_MSG_BUF_TYPE {
	PQU_MSG_BUF_TYPE_INPUT,
	PQU_MSG_BUF_TYPE_OUTPUT,

	PQU_MSG_BUF_TYPE_MAX,
};

enum PQU_MSG_DQ_TYPE {
	PQU_MSG_DQ_TYPE_NORMAL,
	PQU_MSG_DQ_TYPE_ERR,

	PQU_MSG_DQ_TYPE_MAX,
};

// payload of PQU_MSG_SEND_NEW_WIN
struct msg_new_win_info {
	uint8_t win_id;
};

// payload of PQU_MSG_SEND_DESTROY_WIN
struct msg_destroy_win_info {
	uint8_t win_id;
};

// payload of PQU_MSG_SEND_STREAM_ON
struct msg_stream_on_info {
	uint8_t win_id;
	uint8_t stream_type; // refer to PQU_MSG_BUF_TYPE
	uint64_t stream_meta_va;
	uint64_t stream_meta_pa;
	uint32_t stream_meta_size;
};

// payload of PQU_MSG_SEND_STREAM_OFF
struct msg_stream_off_info {
	uint8_t win_id;
	uint8_t stream_type; // refer to PQU_MSG_BUF_TYPE
};

// payload of PQU_MSG_SEND_QUEUE
struct msg_queue_info {
	uint8_t win_id;
	uint8_t frame_type; // refer to PQU_MSG_BUF_TYPE
	uint64_t meta_va;
	uint64_t meta_pa;
	uint32_t meta_size;
	uint64_t frame_iova;
	uint64_t priv;
	uint8_t extra_frame_num; // require extra n frames before calc first queued frame
};

// payload of PQU_MSG_REPLY_NEW_WIN
struct msg_new_win_done_info {
	uint8_t win_id;
	uint8_t status; //0:success, 1:fail
};

// payload of PQU_MSG_REPLY_DESTROY_WIN
struct msg_destroy_win_done_info {
	uint8_t win_id;
	uint8_t status; //0:success, 1:fail
};

// payload of PQU_MSG_REPLY_STREAM_ON
struct msg_stream_on_done_info {
	uint8_t win_id;
	uint8_t stream_type;
	uint8_t status; //0:success, 1:fail
};

// payload of PQU_MSG_REPLY_STREAM_OFF
struct msg_stream_off_done_info {
	uint8_t win_id;
	uint8_t status; //0:success, 1:fail
	uint8_t stream_type; // refer to PQU_MSG_BUF_TYPE
	uint64_t stream_meta_va;
	uint64_t stream_meta_pa;
	uint32_t stream_meta_size;
};

// payload of PQU_MSG_REPLY_QUEUE
struct msg_queue_done_info {
	uint8_t win_id;
	uint8_t frame_type; //0:input 1:output
	uint64_t meta_va;
	int32_t meta_size;
	uint64_t priv;
	uint8_t status; //0:success, 1:fail
};

// payload of PQU_MSG_REPLY_DQUEUE
struct msg_dqueue_info {
	uint8_t win_id;
	uint8_t frame_type; // refer to PQU_MSG_BUF_TYPE
	uint8_t dq_status; //refer to PQU_MSG_DQ_TYPE
	uint64_t meta_va;
	uint32_t meta_size;
	uint64_t meta_host_va;
	uint64_t priv;
};

// payload of PQU_MSG_SEND_CONFIG
struct msg_config_info {
	uint8_t config_en;
	uint64_t config;	//TODO
	uint8_t idx_en;
	uint16_t idx;
	uint8_t dvconfig_en;
	uint64_t dvconfig;
	uint32_t dvconfigsize;
	uint32_t u32IRQ_Version;
	uint32_t u32Config_Version;
	uint32_t u32DV_PQ_Version;
	uint16_t u16HSY_PQ_Version;
};

// payload of PQU_MSG_SEND_DEMURA
struct msg_render_demura_info {
    uint64_t binary_pa;
    uint32_t binary_len;
    uint32_t disable;
    uint32_t hw_index;
};

struct msg_render_pq_sharemem_info {
	uint64_t address;
	u32   size;
};

struct msg_render_pq_update_info {
	bool pnlgamma_enable;
	bool pnlgamma_curve;
	bool pnlgamma_gainoffset;
	bool pnlgamma_gainoffset_enable;
	bool pqgamma_enable;
	bool pqgamma_curve;
	bool pqgamma_gainoffset;
	bool pqgamma_gainoffset_enable;
	bool pqgamma_maxvalue;
	bool pqgamma_maxvalue_enable;
	bool pnlgamma_stub_mode;
};

// payload of PQU_MSG_SEND_RENDER_BE_SHAREMEM_INFO
struct msg_render_be_sharemem_info {
	uint64_t phys;
	uint32_t size;
	uint32_t count;
};

// payload of PQU_MSG_SEND_RENDER_BE_SETTING
struct msg_render_be_setting {
	uint32_t render_mode; // AKA. enum drm_mtk_vplane_buf_mode
};

// payload of PQU_MSG_SEND_PQMAP
struct msg_pqmap_info {
	uint64_t start_addr;
	uint64_t max_size;
	uint32_t u32MainPimLen;
	uint32_t u32MainRmLen;
	uint32_t u32MainExPimLen;
	uint32_t u32MainExRmLen;
};

// payload of PQU_MSG_SEND_GLOBAL_TIMING
struct msg_b2r_global_timing_info {
	uint8_t win_id;
	bool enable;
};

// payload of PQU_MSG_SEND_ld
struct msg_render_ld_info {
	uint8_t u8ld_support_type;
	uint8_t u8ld_version;
	uint8_t u8auto_ld;
	uint8_t u8xtended_range;
	uint8_t u8backlight;

};

// payload of PQU_MSG_SEND_ld_init
struct msg_render_ld_init_info {
	uint64_t u64Compensation_Addr;
	uint32_t u32Compensation_mem_size;
	uint32_t u32Edge2D_mem_size;
	uint32_t u32Edge2D_bin_size;
	uint32_t u32Backlight_Gamma_mem_size;
	uint16_t u16DTS_Mem_Size;
};


// payload of PQU_MSG_SEND_PQMAP
struct msg_pqmeta_info {
	uint32_t fd;
	uint64_t meta_pa;
	uint32_t meta_size;
	uint8_t win_id;
};

// payload of PQU_MSG_SEND_PQPARAM
struct msg_pqparam_info {
	uint8_t win_id;
	uint64_t va;
	uint64_t pa;
	uint64_t da;
	uint64_t size;
};

// payload of PQU_MSG_REPLY_PQPARAM
struct msg_pqparam_done_info {
    uint8_t win_id;
    uint8_t status; //0:success, 1:fail
};

//payload of PQU_MSG_SEND_PQPARAM
struct msg_dv_gd_info {
	uint16_t gd;
};

// payload of PQU_MSG_SEND_QUEUE_SLICE
struct msg_queue_slice_info {
	uint8_t slice_num;
	uint8_t slice_done_id;
};

//-----------------------------------------------------------------------------
//  Function and Variable
//-----------------------------------------------------------------------------
// for PQ DD use
void pqu_msg_send(int32_t msg_id, void *param);
void pqu_msg_register_notify_func(int32_t msg_id, PQU_MSG_REPLY_FP fp);
void pqu_msg_deregister_notify_func(int32_t msg_id);

#endif // _PQU_MSG_H_
