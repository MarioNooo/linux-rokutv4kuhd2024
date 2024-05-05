/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_PVR_H
#define MTK_DMX_PVR_H

#define PVR_FLUSH_TIMEOUT_NEVER (-1)

enum pvr_packet_type {
	PVR_PACKET_TYPE_M2TS,
	PVR_PACKET_TYPE_M2TS_WITH_TIMESTAMP,
	PVR_PACKET_TYPE_TLV,
	PVR_PACKET_TYPE_TLV_WITH_TIMESTAMP,
	PVR_PACKET_TYPE_ALP,
	PVR_PACKET_TYPE_ALP_WITH_TIMESTAMP,
	PVR_PACKET_TYPE_MAX
};

enum pvr_burst_len {
	PVR_BURST_LEN_MIN = 0,
	PVR_BURST_LEN_8 = PVR_BURST_LEN_MIN, ///< 16 bytes * 8
	PVR_BURST_LEN_4, ///< 16 bytes * 4
	PVR_BURST_LEN_12_11_PACKET,
	PVR_BURST_LEN_1, ///< 16 bytes * 1
	PVR_BURST_LEN_MAX,

	PVR_BURST_LEN_NULL = -1,
};

void mtk_pvr_init(void);
void mtk_pvr_exit(void);
int mtk_pvr_suspend(bool suspend, void *parg);

void mtk_pvr_set_buffer(u16 eng_id, bool pingpong_idx, u64 buf_start_addr, u64 buf_end_addr);
void mtk_pvr_pingpong_en(u16 eng_id, bool bEnable);
void mtk_pvr_burst_len(u16 eng_id, enum pvr_burst_len len);
void mtk_pvr_bypass_hdr(u16 eng_id, bool bBypass);
void mtk_pvr_alignment(u16 eng_id, bool bEnable);
void mtk_pvr_set_record_type(u16 eng_id, enum pvr_packet_type type);
void mtk_pvr_set_record_mode(u16 eng_id, bool rec_all);
void mtk_pvr_flush_data(u16 eng_id);
void mtk_pvr_reset_wptr(u16 eng_id);
void mtk_pvr_set_src(u16 eng_id, u16 path_id);
void mtk_pvr_enable(u16 eng_id, bool bEnable);
void mtk_pvr_set_timestamp(u16 eng_id, u64 timestamp);
u64	 mtk_pvr_get_timestamp(u16 eng_id);
u64	 mtk_pvr_get_wptr(u16 eng_id);

#endif /* MTK_DMX_PVR_H */
