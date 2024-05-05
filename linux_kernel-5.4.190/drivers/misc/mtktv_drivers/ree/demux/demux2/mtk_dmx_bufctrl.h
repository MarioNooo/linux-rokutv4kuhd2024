/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_BUFCTRL_H
#define MTK_BUFCTRL_H

#include <linux/kfifo.h>
#include <linux/mutex.h>
#include <linux/dvb/dmx2_mtk.h>

#include <media/dmx2dev.h>

#include "mtk_dmx_rm.h"

#define MTK_BC_METADATA_BUF_NUM		(300)
#define MTK_BC_METADATA_BUF_SIZE	(sizeof(struct metadata))

struct mtk_bc_ctx;

typedef int (*bc_set_rptr_cb)(struct mtk_bc_ctx *ctx, u64 rptr);

enum mtk_bc_memtype {
	MTK_BC_MEMTYPE_RINGBUF,
	MTK_BC_MEMTYPE_VB2
};

enum mem_info_type {
	MEM_INFO_TYPE_MIN,
	MEM_INFO_TYPE_NORMAL = MEM_INFO_TYPE_MIN,
	MEM_INFO_TYPE_CRYPTO,
	MEM_INFO_TYPE_MAX
};

struct metadata {
	union {
		// standard
		struct dmx2_mpu_metadata mpu;
		struct dmx2_ip_payload_metadata ip_payload;
		struct dmx2_ts_video_metadata ts_video;
		struct dmx2_ts_audio_metadata ts_audio;
		struct dmx2_mmtp_video_metadata mmtp_video;
		struct dmx2_mmtp_audio_metadata mmtp_audio;
		// mtk extension
		struct dmx2_record_parser_ts_metadata ts_rec_parser;
		struct dmx2_record_parser_mmtp_metadata mmtp_rec_parser;
		struct dmx2_temi_metadata temi;
	};
};

// @NOTE: currently only support one metadata type per fd
struct metadata_buf_ctx {
	wait_queue_head_t wait;
	struct mutex mutex;
	struct kfifo kfifo;
	u32 size, num;
	u32	navailable;
	bool inited;
};

struct data_buf_ctx {
	struct dmx2_buffer_ctx buf;
	bc_set_rptr_cb cb;

	u32 buf_size;
	struct mem_info mem_info[MEM_INFO_TYPE_MAX];
};

struct mtk_bc_ctx {
	struct data_buf_ctx d_ctx;
	struct metadata_buf_ctx m_ctx;

	// TODO: lock for buffer
};

int mtk_bc_get_data_buf_type(struct mtk_bc_ctx *ctx, enum mtk_bc_memtype *type);

// ring buffer
int mtk_bc_ringbuf_init(struct mtk_bc_ctx *ctx, void *data, size_t len);
int mtk_bc_ringbuf_free(struct mtk_bc_ctx *ctx);
int mtk_bc_ringbuf_read_user(
	struct mtk_bc_ctx *ctx,
	char __user *buf, size_t len,
	int non_blocking);
int mtk_bc_ringbuf_write_user(struct mtk_bc_ctx *ctx, const char __user *buf, size_t len);
int mtk_bc_ringbuf_write(struct mtk_bc_ctx *ctx, const u8 *buf, size_t len);
int mtk_bc_ringbuf_flush(struct mtk_bc_ctx *ctx);
__poll_t mtk_bc_ringbuf_poll(struct mtk_bc_ctx *ctx, struct file *file, poll_table *wait);
int mtk_bc_ringbuf_is_initialized(struct mtk_bc_ctx *ctx);

int mtk_bc_ringbuf_set_rptr_cb(struct mtk_bc_ctx *ctx, bc_set_rptr_cb cb);
int mtk_bc_ringbuf_set_wptr(struct mtk_bc_ctx *ctx, u64 wptr);
u64 mtk_bc_ringbuf_get_wptr(struct mtk_bc_ctx *ctx);

// vb2 buffer
int mtk_bc_vb2_init(struct mtk_bc_ctx *ctx, const char *name, int non_blocking, int wo_memcpy);
int mtk_bc_vb2_release(struct mtk_bc_ctx *ctx);
int mtk_bc_vb2_reqbufs(struct mtk_bc_ctx *ctx, struct dmx2_requestbuffers *req);
int mtk_bc_vb2_querybuf(struct mtk_bc_ctx *ctx, struct dmx2_buffer *b);
int mtk_bc_vb2_expbuf(struct mtk_bc_ctx *ctx, struct dmx2_exportbuffer *exp);
int mtk_bc_vb2_qbuf(struct mtk_bc_ctx *ctx, struct dmx2_buffer *b);
int mtk_bc_vb2_dqbuf(struct mtk_bc_ctx *ctx, struct dmx2_buffer *b);
__poll_t mtk_bc_vb2_poll(struct mtk_bc_ctx *ctx, struct file *file, poll_table *wait);
int mtk_bc_vb2_mmap(struct mtk_bc_ctx *ctx, struct vm_area_struct *vma);
int mtk_bc_vb2_is_streaming(struct mtk_bc_ctx *ctx);
int mtk_bc_vb2_stream_on(struct mtk_bc_ctx *ctx);
int mtk_bc_vb2_stream_off(struct mtk_bc_ctx *ctx);
int mtk_bc_vb2_fill_buffer(struct mtk_bc_ctx *ctx, const u8 *src, int len);

bool mtk_bc_vb2_is_ringbuf_mode(struct mtk_bc_ctx *ctx);
int mtk_bc_vb2_query_dmabuf(struct mtk_bc_ctx *ctx, struct mem_info *mem_info);
int mtk_bc_vb2_set_rptr_cb(struct mtk_bc_ctx *ctx, bc_set_rptr_cb cb);
int mtk_bc_vb2_set_wptr(struct mtk_bc_ctx *ctx, u64 wptr);

// metadata buffer
int mtk_bc_metabuf_init(struct mtk_bc_ctx *ctx, u32 size, u32 num);
int mtk_bc_metabuf_release(struct mtk_bc_ctx *ctx);
int mtk_bc_metabuf_queue(struct mtk_bc_ctx *ctx, void *meta);
int mtk_bc_metabuf_dequeue(struct mtk_bc_ctx *ctx, void *meta, int non_blocking);

// misc
int mtk_bc_set_buf_size(struct mtk_bc_ctx *ctx, u32 buf_size);
int mtk_bc_get_buf_size(struct mtk_bc_ctx *ctx, u32 *buf_size);

int mtk_bc_reset_mem_info(struct mtk_bc_ctx *ctx);
int mtk_bc_set_mem_info(
		struct mtk_bc_ctx *ctx,
		enum mem_info_type mem_info_type,
		struct mem_info *mem_info);
int mtk_bc_get_mem_info(
	struct mtk_bc_ctx *ctx,
	enum mem_info_type mem_info_type,
	struct mem_info *mem_info);

int mtk_bc_get_ctx(struct dvb_fh *fh, struct mtk_bc_ctx **bc_ctx);

#endif /* MTK_BUFCTRL_H */
