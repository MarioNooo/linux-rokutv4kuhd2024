/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_DRV_H
#define MTK_DMX_DRV_H

#include <linux/timer.h>
#include <media/dvb_demux2.h>

#include "mtk_dmx_riu.h"
#include "mtk_dmx_rm.h"
#include "mtk_dmx_tm.h"
#include "mtk_dmx_pm.h"
#include "mtk_dmx_bufctrl.h"
#include "mtk_dmx_filter.h"

struct mtk_demux2_fh_ctx;

#define fh_to_dmxctx(fhdl)	\
	container_of(fhdl, struct mtk_demux2_fh_ctx, fh)

#define fh_dmxctx_init(ctx, dmx2)	\
	({ ctx->demux = dmx2; })

#define fh_dmxctx_get_fh(ctx)	\
	({ &ctx->fh; })

#define fh_dmxctx_get_demux(ctx)	\
	({ ctx->demux; })

#define fh_dmxctx_set_flt_type(ctx, type)	\
	({ ctx->flt_type = type; })

#define fh_dmxctx_get_flt_type(ctx)	\
	({ ctx->flt_type; })

#define fh_dmxctx_get_bc_ctx(ctx)	\
	({ &ctx->bc_ctx; })

#define fh_dmxctx_get_ev_ctx(ctx)	\
	({ &ctx->ev_ctx; })

void fh_dmxctx_set_filter_type(struct mtk_demux2_fh_ctx *fh, struct filter_type type);
int fh_dmxctx_set_filter_info(struct mtk_demux2_fh_ctx *fh, void *data);
int fh_dmxctx_get_filter_info(struct mtk_demux2_fh_ctx *fh, void *data);

int fh_dmxctx_set_filter_event(struct mtk_demux2_fh_ctx *fh, enum sw_interrupt_event event);
int fh_dmxctx_get_filter_event(struct mtk_demux2_fh_ctx *fh, enum sw_interrupt_event *event);

void fh_dmxctx_set_stc_info(struct mtk_demux2_fh_ctx *fh, void *info);
void fh_dmxctx_get_stc_info(struct mtk_demux2_fh_ctx *fh, void *info);

enum signal_info_type {
	SIGNAL_INFO_TYPE_MP2_SECTION,
	SIGNAL_INFO_TYPE_MP2_TTX,
	SIGNAL_INFO_TYPE_MP2_SUBTITLE,
	SIGNAL_INFO_TYPE_MP2_PACKET,
	SIGNAL_INFO_TYPE_MP2_TEMI,
	SIGNAL_INFO_TYPE_TLV_SECTION,
	SIGNAL_INFO_TYPE_MMTP_SECTION,
	SIGNAL_INFO_TYPE_MMTP_SUBTITLE,
	SIGNAL_INFO_TYPE_MMTP_APPDATA,
	SIGNAL_INFO_TYPE_MAX,
	SIGNAL_INFO_TYPE_NULL = -1
};

struct signal_info_mode {
	u32 oneshot:1;
	u32 crc_check:1;
};

struct hw_pcr_ctrl {
	u8 reset_pcr;
	u32 prev_stc_base, prev_pll_ctrl;
};

struct pid_filter_cfg {
	struct dmx2_filter *parent;
	enum filter_dst_type filter_dst_type;
	u16 pid, src_path, dst_eng_id;
};

struct pcr_filter_cfg {
	struct pid_filter_cfg cfg;
	u64 int_cnt, pcr;
	struct hw_pcr_ctrl pcr_ctrl;
};

struct sec_filter_cfg {
	struct pid_filter_cfg cfg;
	enum signal_info_type si_type;
	struct signal_info_mode si_mode;
	struct dvb_demux2 *demux;
	enum sw_interrupt_event event;
};

struct alp_filter_cfg {
	u16 dst_eng_id; // PVR engine id
};

struct filter_cfg {
	struct filter_type type;
	union {
		struct sec_filter_cfg sec_cfg;
		struct pcr_filter_cfg pcr_cfg;
		struct pid_filter_cfg pid_cfg;
		struct alp_filter_cfg alp_cfg;
	};
};

// @TODO: add parameter to indicate remove stc resource or not.
struct stc_info {
	bool is_filter_use_stc;
	s32 eng_id;
};

struct event_ctx_task {
	struct mtk_demux2_fh_ctx *fh_ctx;
	u32 type;
	unsigned long duration;
	struct event_ctx *ev_ctx;
	struct task_struct *task;
	struct list_head list;
};

struct event_ctx {
	union {
		bool status;
		u32 cid;
	} prev;
	struct list_head ev_list_head;
	spinlock_t ev_ctx_lock;
};

struct mtk_demux2_fh_ctx {
	struct dvb_fh fh;

	struct mtk_demux2 *demux;
	enum filter_main_type flt_type;
	struct mtk_bc_ctx bc_ctx;

	struct filter_cfg cfg;
	struct stc_info stc;

	struct event_ctx ev_ctx;
};

struct mtk_demux2_flt_ops {
	int (*allocate_filter)(struct dmx2_filter **filter);
	int (*release_filter)(struct dmx2_filter *filter);
};

struct mtk_demux2_stc_ops {
	int (*set_stc)(struct dvb_fh *fh, struct dmx2_stc *info); // reserved
	int (*get_stc)(struct dvb_fh *fh, struct dmx2_stc *info);
};

struct mtk_demux2_buf_ops {
	int (*set_rptr)(struct dvb_fh *fh, u64 rptr);
	int (*get_rptr)(struct dvb_fh *fh);				// reserved
	int (*set_wptr)(struct dvb_fh *fh, u64 wptr);	// reserved
	int (*get_wptr)(struct dvb_fh *fh);				// reserved
};

struct mtk_demux2 {
	struct dvb_demux2 base;

	int path_id;
	struct mtk_dmx_rm *rm;
	struct mtk_dmx_pm *pm;
	struct mtk_dmx_tm *tm;

	u32 capabilities; // DMX2_CAP_xxx

	struct list_head filter_list;

	struct mtk_demux2_flt_ops *flt_ops;
	struct mtk_demux2_stc_ops *stc_ops;
	struct mtk_demux2_buf_ops *buf_ops;
};

int mtk_demux2_init(struct mtk_demux2 *demux);
int mtk_demux2_exit(struct mtk_demux2 *demux);

int mtk_demux2_open_filter(
	struct mtk_demux2 *demux,
	struct dvb_fh *fh,
	struct dmx2_filter_settings *settings);
int mtk_demux2_start_filter(struct mtk_demux2 *demux, struct dvb_fh *fh);
int mtk_demux2_stop_filter(struct mtk_demux2 *demux, struct dvb_fh *fh);
int mtk_demux2_flush_filter(struct mtk_demux2 *demux, struct dvb_fh *fh);
int mtk_demux2_close_filter(struct mtk_demux2 *demux, struct dvb_fh *fh);

bool mtk_demux2_is_filter_running(struct mtk_demux2 *demux, struct dvb_fh *fh);
bool mtk_demux2_is_filter_need_ringbuf(struct mtk_demux2 *demux, struct dvb_fh *fh);

int mtk_demux2_set_ext_ctrl(struct dvb_fh *fh, u32 ctrl_id, void *arg);
int mtk_demux2_get_ext_ctrl(struct dvb_fh *fh, u32 ctrl_id, void *arg);

int mtk_demux2_get_stc(struct mtk_demux2 *demux, struct dvb_fh *fh, struct dmx2_stc *info);
int mtk_demux2_query_caps(struct mtk_demux2 *demux, struct dmx2_capability *info);

int mtk_demux2_add_filter_list(struct mtk_demux2 *demux
	, struct dvb_fh *fh, int filter_id, enum filter_sub_type type);
int mtk_demux2_remove_filter_list(struct mtk_demux2 *demux
	, struct dvb_fh *fh, int filter_id, enum filter_sub_type type);
struct dvb_fh *mtk_demux2_query_filter_list(struct mtk_demux2 *demux
	, int filter_id, enum filter_sub_type type);

#endif /* MTK_DMX_DRV_H */
