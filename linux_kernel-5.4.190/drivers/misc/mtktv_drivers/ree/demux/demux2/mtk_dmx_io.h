/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef MTK_DMX_IO_H
#define MTK_DMX_IO_H

#define TS_SERIAL_PAD_NUM		1
#define TS_SERIAL_2BITS_MODE_PAD_NUM	2
#define TS_PARALLEL_PAD_NUM		8

enum demux_filein_state {
	DEMUX_FILEIN_STATE_IDLE,
	DEMUX_FILEIN_STATE_BUSY,
	DEMUX_FILEIN_STATE_PAUSE,
	DEMUX_FILEIN_STATE_MAX,
	DEMUX_FILEIN_STATE_NULL = -1
};

enum demux_io_type {
	DEMUX_IO_TYPE_IN_DEMOD,
	DEMUX_IO_TYPE_EXT_DEMOD,
	DEMUX_IO_TYPE_MEM,
	DEMUX_IO_TYPE_CILINK,
	DEMUX_IO_TYPE_TSO,
	DEMUX_IO_TYPE_MAX,
	DEMUX_IO_TYPE_NULL = -1
};

enum demux_port_type {
	DEMUX_PORT_TYPE_PARALLEL,
	DEMUX_PORT_TYPE_SERIAL,
	DEMUX_PORT_TYPE_SERIAL_2BITS,
	DEMUX_PORT_TYPE_MAX,
	DEMUX_PORT_TYPE_NULL = -1
};

enum demux_in_format {
	DEMUX_IN_FORMAT_M2TS,
	DEMUX_IN_FORMAT_M2TS_TIMESTAMP,
	DEMUX_IN_FORMAT_TLV,
	DEMUX_IN_FORMAT_TLV_TIMESTAMP,
	DEMUX_IN_FORMAT_ALP,
	DEMUX_IN_FORMAT_ALP_TIMESTAMP,
	DEMUX_IN_FORMAT_MAX,
	DEMUX_IN_FORMAT_NULL = -1
};

struct demux_io_cfg {
	enum demux_io_type type;
	enum demux_port_type port_type;
	u16	 port_num;
};

struct demux_filein_cfg {
	bool bypass_timestamp;
	enum demux_in_format format;
};

// @TODO: need to modify
struct demux_out_clk_cfg {
	bool bDefault;
	u32 out_clk;
};

enum demux_out_format {
	DEMUX_OUT_FORMAT_CIPLUS,
	DEMUX_OUT_FORMAT_192PKT,
	DEMUX_OUT_FORMAT_OPENCABLE,
	DEMUX_OUT_FORMAT_MAX,
	DEMUX_OUT_FORMAT_NULL = -1
};

struct tso_out_clk {
	u16	out_clk_sel;
	u16	div_src, div_num;
	u16	p_tso_out_clk_sel;
	bool	bClk_invert;
};

struct demux_tso_input_cfg {
	u16	 tso_eng_id, tso_tsif_id;
	struct demux_io_cfg src_cfg;
};

struct demux_tso_output_cfg {
	enum demux_out_format out_format;
	enum demux_port_type port_type;
	struct demux_out_clk_cfg clk_cfg;
};

struct demux_cilink_input_cfg {
	u16	 port_num;
	bool bEnable;
	enum demux_io_type type;
};

// live-in
void mtk_dmx_io_sync_byte(u16 path, u8 idx, u8 *psync_byte, bool bSet);
void mtk_dmx_io_sync_byte_en(u16 path, u8 idx, bool bEnable);
void mtk_dmx_io_src_id(u16 path, u8 idx, u8 *psrc_id, bool bSet);
void mtk_dmx_io_set_input_src(u16 path, struct demux_io_cfg *pcfg);
void mtk_dmx_io_flt_null_pkt(u16 path, bool bEnable);

// file-in
void mtk_dmx_io_feed_filein(u16 path, u64 paddr, u32 size, bool bInitTrust);
enum demux_filein_state mtk_dmx_io_get_filein_state(u16 path, u16 *cmdq_cnt);
void mtk_dmx_io_filein_byte_delay(u16 path, u32 delay);
void mtk_dmx_io_set_filein_cfg(u16 path, enum demux_in_format format, bool timestamp_en);

// fiq
void mtk_dmx_io_init_fiq(void);
void mtk_dmx_io_set_fiq_buf(u16 eng_id, u64 paddr, u32 size);
void mtk_dmx_io_enable_fiq(u16 eng_id, u16 path_id, bool bEnable);
void mtk_dmx_io_rush_fiq(u16 eng_id, u64 rush_addr);
u64 mtk_dmx_io_get_fiq_rptr(u16 eng_id);
u64 mtk_dmx_io_get_fiq_wptr(u16 eng_id);

// tso
void mtk_dmx_io_init_tso(void);
void mtk_dmx_io_tso_input_cfg(struct demux_tso_input_cfg *pcfg);
void mtk_dmx_io_tso_output_cfg(struct demux_tso_output_cfg *pcfg);

// ca ctrl
int mtk_dmx_io_ca_ctrl(u16 path, u8 eng_id, bool bThrough);

// cilink
void mtk_dmx_io_set_cilink_input_cfg(struct demux_cilink_input_cfg *pcfg);
void mtk_dmx_io_set_cilink_loopback_status(u16 eng_id, bool bSet);
u32 mtk_dmx_io_get_cilink_loopback_status(u16 eng_id);

// debug table
u32 mtk_dmx_io_get_locked_cnt(u16 path);

//suspend to ram
int mtk_dmx_io_suspend(bool suspend, void *parg);

#endif /* MTK_DMX_IO_H */
