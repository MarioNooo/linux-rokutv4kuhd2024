/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_DVRDRV_H
#define MTK_DMX_DVRDRV_H

#include <media/dvb_dvr2.h>
#include <uapi/linux/sched.h>

#include "mtk_dmx_riu.h"
#include "mtk_dmx_rm.h"
#include "mtk_dmx_pm.h"
#include "mtk_dmx_bufctrl.h"

#define RECORD_TASK_WAIT_TIME_MAX (2)

#define DEFAULT_RECORD_TASK_POLICY (SCHED_RR)
#define DEFAULT_RECORD_TASK_PRIORITY (1) // MIN:1 MAX:99
#define DEFAULT_RECORD_TASK_WAIT_TIME1 (10000)
#define DEFAULT_RECORD_TASK_WAIT_TIME2 (10500)
#define DEFAULT_RECORD_TASK_FINALIZE_CALLBACK (NULL)
#define DEFAULT_RECORD_TASK_FINALIZE_TIMEOUT (100)

#define IS_OUTPUT_DEVICE(dir)  \
	(dir == DVR_DATA_FROM_DEVICE)

#define IS_INPUT_DEVICE(dir)   \
	(dir == DVR_DATA_TO_DEVICE)

#define fh_to_dvrctx(fhdl)	\
	container_of(fhdl, struct mtk_dvr2_fh_ctx, fh)

#define fh_dvrctx_init(ctx, dvr2)	\
	({ ctx->dvr = dvr2; })

#define fh_dvrctx_get_fh(ctx)	\
	({ &ctx->fh; })

#define fh_dvrctx_get_dvr(ctx)	\
	({ ctx->dvr; })

#define fh_dvrctx_set_dir(ctx, dir)	\
	({ ctx->dir = dir; })

#define fh_dvrctx_get_dir(ctx)	\
	({ ctx->dir; })

#define fh_dvrctx_get_bc_ctx(ctx)	\
	({ &ctx->bc_ctx; })

#define fh_dvrctx_get_buffer_ctx(ctx)	\
	({ &ctx->bc_ctx.d_ctx.buf; })

enum dvr_data_direction {
	DVR_DATA_NONE,
	DVR_DATA_TO_DEVICE,
	DVR_DATA_FROM_DEVICE
};

enum dvr_insert_type {
	DVR_INSERT_NONE,
	DVR_INSERT_TIMESTAMP,
	DVR_INSERT_KEYWORD,
	DVR_INSERT_SIDEBAND
};

enum dvr_encrypt_type {
	DVR_ENCRYPT_TYPE_NONE,
	DVR_ENCRYPT_TYPE_PAYLOAD,
	DVR_ENCRYPT_TYPE_BLOCK
};

struct dvr_rec_config {
	bool record_all, tee_mode;
	enum dvr_insert_type pre_hdr_type;
	enum dvr_encrypt_type encrypt_type;
};

struct dvr_ca_pid {
	u16 pid;
	struct list_head list;
};

struct dvr_crypto_config {
	u32 pid_num;
	struct list_head list_head;
	spinlock_t lock;
};

struct mtk_dvr2_fh_ctx {
	struct dvb_fh fh;

	struct mtk_dvr2 *dvr;
	enum dvr_data_direction dir;
	struct mtk_bc_ctx bc_ctx;
};

struct mtk_dvr2_buf_ops {
	int (*set_rptr)(struct mtk_dvr2 *dvr, u64 rptr);
	u64 (*get_rptr)(struct mtk_dvr2 *dvr);				// reserved
	int (*set_wptr)(struct mtk_dvr2 *dvr, u64 wptr);	// reserved
	u64 (*get_wptr)(struct mtk_dvr2 *dvr);
};

struct mtk_dvr2_crypto_ops {
	int (*attach)(struct mtk_dvr2 *dvr, u8 eng_id);
	int (*detach)(struct mtk_dvr2 *dvr, u8 eng_id);
	int (*encrypt)(struct mtk_dvr2 *dvr, u64 offset, u32 len);
	int (*decrypt)(struct mtk_dvr2 *dvr, u64 offset, u32 len);
};

typedef int (*record_task_finalize_callback)(
		struct mtk_dvr2 *src,
		int timeout);

struct record_task_info {
	int policy;
	int priority;

	unsigned long wait_times[RECORD_TASK_WAIT_TIME_MAX];
	record_task_finalize_callback finalize_callback;
	int finalize_timeout;
};

struct mtk_dvr2 {
	struct dvb_dvr2 base;

	int path_id;
	struct mtk_dmx_rm *rm;
	struct mtk_dmx_pm *pm;

	u32 capabilities; // DMX2_CAP_xxx
	struct dvr_rec_config rec_cfg;
	struct dvr_crypto_config crypto_cfg;

	struct mtk_dvr2_buf_ops *buf_ops;
	struct mtk_dvr2_crypto_ops *crypto_ops;

	struct task_struct *record_task, *feed_task;

	struct record_task_info record_task_info;
};

int mtk_dvr2_init(struct mtk_dvr2 *dvr);
int mtk_dvr2_exit(struct mtk_dvr2 *dvr);

int mtk_dvr2_open(struct mtk_dvr2 *dvr, struct dvb_fh *fh);
int mtk_dvr2_start(struct mtk_dvr2 *dvr);
int mtk_dvr2_stop(struct mtk_dvr2 *dvr);
int mtk_dvr2_flush(struct mtk_dvr2 *dvr);
int mtk_dvr2_close(struct mtk_dvr2 *dvr, struct dvb_fh *fh);

bool mtk_dvr2_is_running(struct mtk_dvr2 *dvr);
bool mtk_dvr2_is_need_ringbuf(struct mtk_dvr2 *dvr);
bool mtk_dvr2_is_tee_mode(struct mtk_dvr2 *dvr);

int mtk_dvr2_set_ext_ctrl(struct mtk_dvr2 *dvr, u32 ctrl_id, void *arg);
int mtk_dvr2_get_ext_ctrl(struct mtk_dvr2 *dvr, u32 ctrl_id, void *arg);

int mtk_dvr2_query_caps(struct mtk_dvr2 *dvr, struct dmx2_capability *info);
int mtk_dvr2_write(struct mtk_dvr2 *dvr, int non_blocking);

int mtk_dvr2_set_secure_mode(struct mtk_dvr2 *dvr, bool enable);
int mtk_dvr2_set_crypto_type(struct mtk_dvr2 *dvr, enum dvr_encrypt_type type);
int mtk_dvr2_attach_ca(struct mtk_dvr2 *dvr, u8 eng_id);
int mtk_dvr2_detach_ca(struct mtk_dvr2 *dvr, u8 eng_id);
int mtk_dvr2_add_ca_pid(struct mtk_dvr2 *dvr, u16 pid);
int mtk_dvr2_remove_ca_pid(struct mtk_dvr2 *dvr, u16 pid);

#endif /* MTK_DMX_DVRDRV_H */
