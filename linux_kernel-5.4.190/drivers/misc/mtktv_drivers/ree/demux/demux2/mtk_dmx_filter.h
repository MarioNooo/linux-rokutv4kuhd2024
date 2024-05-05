/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_FILTER_H
#define MTK_DMX_FILTER_H

enum filter_dst_type {
	FILTER_DST_TYPE_VIDEO,
	FILTER_DST_TYPE_AUDIO,
	FILTER_DST_TYPE_PVR,
	FILTER_DST_TYPE_SEC,
	FILTER_DST_TYPE_MAX,
	FILTER_DST_TYPE_NULL = -1
};

// signaling information type
enum filter_si_type {
	FILTER_SI_TYPE_SECTION = 0,
	FILTER_SI_TYPE_PES,
	FILTER_SI_TYPE_PKT,
	FILTER_SI_TYPE_SWPCR,
	FILTER_SI_TYPE_TTX,
	FILTER_SI_TYPE_SECTION_VER_CHK,
	FILTER_SI_TYPE_MMT_SI = 0x100,
	FILTER_SI_TYPE_TLV_SI = 0x101,
	FILTER_SI_TYPE_TLV_TIMED_MPU = 0x102,
	FILTER_SI_TYPE_TLV_NON_TIMED_MPU = 0x103,
	FILTER_SI_TYPE_MMT_SI_MESSAGE = 0x104,
	FILTER_SI_TYPE_SECTION_NO_PUSI = 0x1000000,
	FILTER_SI_TYPE_NULL = -1
};

// signaling information control mode
enum filter_si_mode {
	FILTER_SI_MODE_CONTINUE			= 0x0,
	FILTER_SI_MODE_ONESHOT			= 0x1,
	FILTER_SI_MODE_CRC_CHECK		= 0x2,
	FILTER_SI_MODE_AUTO_CRC_CHECK	= 0x10000
};

enum filter_tlv_mode {
	FILTER_TLV_MODE_DISABLE = 0x0,
	FILTER_TLV_MODE_MMTP    = 0x1,
	FILTER_TLV_MODE_SI      = 0x2
};

enum stc_mode {
	STC_MODE_TS,
	STC_MODE_TLV,
};

#define SECFLT_NMASK_FLD_ID			(17)

#define NULL_PID					(0x1FFF)
#define NULL_PATH_ID				((u16)-1)
#define NULL_PACKET_ID				(0xFFFF)
#define NULL_IPFLT_ID				(0x1F)
#define SECFLT_PATTERN_DEPTH		(16)
#define STC_SYNTH_DEFAULT_CW		(0x28000000)
#define STC_SYNTH_DEFAULT_CW_TLV	(0x405F7E00)

#define OR_ADDR_SHIFT			(4)
#define PIDFLT_L_BASE			(0x210000)
#define PIDFLT_H_BASE			(0x210800)
#define PIDFLT_L_BASE1			(0x210400)
#define PIDFLT_H_BASE1			(0x210c00)
#define SECFLT_BASE_1			(0x211000)
#define SECFLT_BASE_2			(0x215000)
#define SECFLT_BASE_3			(0x220000)
#define SECFLT_BASE_EXT			(0x217000)
#define SECFLT_ENTRY_NUM		(16)
#define SECFLT_NMASK_DEPTH		(4)
#define PIDFLT_TLV_BASE			(0x240800)
#define IPFLT_BASE				(0x240000)
#define NTPFLT_BASE				(0x241000)

#define PIDFLT_L_ADDR(id)		\
	((id < 128) ? (PIDFLT_L_BASE + (id) * 0x4) : \
				 (PIDFLT_L_BASE1 + ((id) - 128) * 0x4))

#define PIDFLT_H_ADDR(id)		\
	((id < 128) ? (PIDFLT_H_BASE + (id) * 0x4) : \
	(PIDFLT_H_BASE1 + ((id) - 128) * 0x4))

#define SECFLT_ADDR(id)				\
	({								\
		u16 idx;					\
		u32 addr;					\
									\
		if (id < 64) {				\
			idx = id;				\
			addr = SECFLT_BASE_1;	\
		} else if (id < 128) {		\
			idx = id - 64;			\
			addr = SECFLT_BASE_2;	\
		} else {					\
			idx = id - 128;			\
			addr = SECFLT_BASE_3;	\
		}							\
									\
		addr += (idx * 0x80);		\
	})


#define SECFLT_ADDR_EXT(id)		(SECFLT_BASE_EXT + (id * 0x80))
#define PIDFLT_TLV_ADDR(id)		(PIDFLT_TLV_BASE + (id * 0x4))
#define IPFLT_ADDR(id)			(IPFLT_BASE + (id * 0x80))
#define NTPFLT_ADDR(id)			(NTPFLT_BASE + (id * 0x80))

void mtk_dmx_filter_init(bool bInitSW);
void mtk_dmx_filter_exit(void);
//suspend to ram
int mtk_dmx_filter_suspend(bool suspend, void *parg);

void mtk_dmx_pid_filter_init(u16 filter_id);
void mtk_dmx_pid_filter_set_pid(u16 filter_id, u16 pid);
u16 mtk_dmx_pid_filter_get_pid(u16 filter_id);
void mtk_dmx_pid_filter_sel_src(u16 filter_id, u16 path);
u16 mtk_dmx_pid_filter_get_src(u16 filter_id);
void mtk_dmx_pid_filter_sel_dst(u16 filter_id, enum filter_dst_type type, u16 idx);
enum filter_dst_type mtk_dmx_pid_filter_get_dst_type(u16 filter_id);
void mtk_dmx_pid_filter_rush_pass(u16 filter_id, bool bPass);
void mtk_dmx_pid_filter_set_srcid(u16 filter_id, u8 srcid);
void mtk_dmx_pid_filter_sel_secflt(u16 filter_id, u16 sec_filter_id);
void mtk_dmx_pid_filter_set_tlv_mode(u16 filter_id, enum filter_tlv_mode mode);
void mtk_dmx_pid_filter_set_ip_filter_id(u16 filter_id, u16 ip_filter_id);
bool mtk_dmx_pid_filter_get_scmb_status(u16 filter_id, bool *pstatus);
void mtk_dmx_pid_filter_clear_scmb_status(u16 filter_id);

void mtk_dmx_sec_filter_init(u16 filter_id);
bool mtk_dmx_sec_filter_set_pattern(u16 filter_id, u8 *pmask, u8 *pNmask, u8 *pmatch);
bool mtk_dmx_sec_filter_clear_pattern(u16 filter_id);
void mtk_dmx_sec_filter_set_type(u16 filter_id, enum filter_si_type type);
void mtk_dmx_sec_filter_set_mode(u16 filter_id, enum filter_si_mode mode);
void mtk_dmx_sec_filter_clear_ctrl(u16 filter_id);
void mtk_dmx_sec_filter_clear_req(u16 filter_id);

void mtk_dmx_sec_filter_start_addr(u16 filter_id, u64 *paddr, bool bSet);
void mtk_dmx_sec_filter_end_addr(u16 filter_id, u64 *paddr, bool bSet);
void mtk_dmx_sec_filter_read_addr(u16 filter_id, u64 *paddr, bool bSet);
void mtk_dmx_sec_filter_write_addr(u16 filter_id, u64 *paddr, bool bSet);
void mtk_dmx_sec_filter_reset_buffer(u16 filter_id);

void mtk_dmx_pcr_filter_init(u16 filter_id);
void mtk_dmx_pcr_filter_sel_src(u16 filter_id, u16 path);
u16 mtk_dmx_pcr_filter_get_src(u16 filter_id);
void mtk_dmx_pcr_filter_set_srcid(u16 filter_id, u8 srcid);
void mtk_dmx_pcr_filter_pid(u16 filter_id, u16 *pPID, bool bSet);
void mtk_dmx_pcr_filter_get_PCR(u16 filter_id, u32 *pPCR_base, u32 *pPCR_base_32);
void mtk_dmx_pcr_filter_enable(u16 filter_id, bool bEnable);

void mtk_dmx_tlv_ip_filter_init(int filter_id);
void mtk_dmx_tlv_ip_filter_set_ip(int filter_id, struct dmx2_ip_addr *ip_addr);
void mtk_dmx_tlv_ip_filter_set_src(int filter_id, int path);
void mtk_dmx_tlv_ip_filter_set_enable(int filter_id, bool enable);
void mtk_dmx_tlv_ip_filter_set_cid(int filter_id, bool enable, bool hw_update, int cid);

void mtk_dmx_tlv_ntp_filter_init(int path);
void mtk_dmx_tlv_ntp_filter_set_ip(int path, struct dmx2_ip_addr *ip_addr);
u64  mtk_dmx_tlv_get_ntp(int path, u8 *leap_ind);

void mtk_dmx_stc_cw(u16 eng_id, u32 *pcw, bool bSet);
void mtk_dmx_stc(u16 eng_id, u32 *pSTC_L, u32 *pSTC_H, bool bSet);
void mtk_dmx_stc_set_mode(u16 eng_id, enum stc_mode mode);

void mtk_dmx_set_fifo_src(enum filter_dst_type type, u16 idx, u16 path_id);

#endif /* MTK_DMX_FILTER_H */
