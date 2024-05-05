/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_TEE_IF_H
#define MTK_DMX_TEE_IF_H

#define PID_INFO_MAX_NUM		(16)
#define PVR_METADATA_MAX_NUM	(300)

typedef enum {
	AESDMA_DATA_FORMAT_TS_188,
	AESDMA_DATA_FORMAT_TS_192
} aesdma_data_format_t;

typedef enum {
	AESDMA_TYPE_NONE,
	AESDMA_ENCRYPT_TYPE_PAYLOAD,
	AESDMA_ENCRYPT_TYPE_BLOCK,
	AESDMA_DECRYPT_TYPE_PAYLOAD,
	AESDMA_DECRYPT_TYPE_BLOCK
} aesdma_type_t;

typedef enum {
	PVR_STREAM_TYPE_NONE,
	PVR_STREAM_TYPE_MPEG_VIDEO,
	PVR_STREAM_TYPE_AVC_VIDEO,
	PVR_STREAM_TYPE_HEVC_VIDEO,
	PVR_STREAM_TYPE_AUDIO
} pvr_stream_type_t;

typedef enum {
	PVR_TAG_MASK_PUSI = (1 << 0),
	PVR_TAG_MASK_I_FRAME = (1 << 1),
	PVR_TAG_MASK_P_FRAME = (1 << 2),
	PVR_TAG_MASK_B_FRAME = (1 << 3),
	PVR_TAG_MASK_SEQ = (1 << 4),
	//AVC
	PVR_TAG_MASK_I_SLICE = (1 << 5),
	PVR_TAG_MASK_P_SLICE = (1 << 6),
	PVR_TAG_MASK_B_SLICE = (1 << 7),
	PVR_TAG_MASK_SI_SLICE = (1 << 8),
	PVR_TAG_MASK_SP_SLICE = (1 << 9),
	//HEVC
	PVR_TAG_MASK_SPS = (1 << 10),
	PVR_TAG_MASK_AUD = (1 << 11),
	PVR_TAG_MASK_SLICE_CE_BLA_W_LP = (1 << 12),
	PVR_TAG_MASK_SLICE_BLA_W_RADL = (1 << 13),
	PVR_TAG_MASK_SLICE_BLA_N_LP = (1 << 14),
	PVR_TAG_MASK_SLICE_IDR_W_RADL = (1 << 15),
	PVR_TAG_MASK_SLICE_IDR_N_LP = (1 << 16),
	PVR_TAG_MASK_SLICE_TRAIL_CRA = (1 << 17),
} pvr_tag_mask_t;

struct pvr_pid_info {
	s16 pid;
	s32 type;
	u32 mask;
};

struct pvr_encrypt_config {
	aesdma_data_format_t format;
	aesdma_type_t type;
	u32 pid_num;
	struct pvr_pid_info *pid_info;
};

struct pvr_metadata {
	s16 pid;
	u32 mask;
	u32 offset;
	u64 pts;
};

struct pvr_encrypt_param {
	u64 src_addr, dst_addr;
	u32 size;
	u32 index_num;
	struct pvr_metadata *metadata;
};

int mtk_dmx_tee_if_load_fw(void);
int mtk_dmx_tee_if_set_vq(void);

int mtk_dmx_tee_if_lock_pvr_buffer(int pvr_eng_id, struct mem_info *pvr_buffer);
int mtk_dmx_tee_if_unlock_pvr_buffer(int pvr_eng_id, struct mem_info *pvr_buffer);

int mtk_dmx_tee_if_pvr_encrypt_config(int pvr_eng_id, struct pvr_encrypt_config *cfg);
int mtk_dmx_tee_if_pvr_encrypt_buffer(int pvr_eng_id, struct pvr_encrypt_param *param);

#endif /* MTK_DMX_TEE_IF_H */
