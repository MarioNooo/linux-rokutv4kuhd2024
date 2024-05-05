/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef MTK_DMX_CTRL_H
#define MTK_DMX_CTRL_H

#define SW_INT_STATUS_FLT_ID_MASK				(0x000000FF)
#define SW_INT_STATUS_FLT_ID_SHIFT				(0)
#define SW_INT_STATUS_EVENT_MASK				(0x7FFF0000)
#define SW_INT_STATUS_EVENT_SHIFT				(16)
	#define SW_INT_STATUS_EVENT_SEC_RDY			(1)
	#define SW_INT_STATUS_EVENT_SEC_CRCERR			(3)
	#define SW_INT_STATUS_EVENT_SEC_OVERFLOW		(6)
#define SW_INT_STATUS_FIRE_INT_MASK				(0x80000000)

#define SW_INT_STATUS_GET_FLT_ID(status) \
	((status & SW_INT_STATUS_FLT_ID_MASK) >> SW_INT_STATUS_FLT_ID_SHIFT)

#define SW_INT_STATUS_GET_EVENT(status) \
	((status & SW_INT_STATUS_EVENT_MASK) >> SW_INT_STATUS_EVENT_SHIFT)

#define SW_INT_STATUS_CHK_STATE(status) \
	(status & SW_INT_STATUS_FIRE_INT_MASK)

#define NTP_INTERRUPT_0			(0x0010)
#define NTP_INTERRUPT_1			(0x0020)
#define NTP_INTERRUPT_2			(0x0040)
#define NTP_INTERRUPT_STATUS_MASK_0	(0x1000)
#define NTP_INTERRUPT_STATUS_MASK_1	(0x2000)
#define NTP_INTERRUPT_STATUS_MASK_2	(0x4000)
#define TLV_INTERRUPT_MASK		(0xFF00)

void mtk_dmx_ctrl_init(bool *bDmxTeeInited);
void mtk_dmx_ctrl_exit(void);

void mtk_dmx_init_hw(void);
bool mtk_dmx_load_fw(u64 paddr, u32 size);
bool mtk_dmx_check_fw_alive(void);
void mtk_dmx_set_vq_buffers(u64 paddr, u32 size, u16 vq_num);
void mtk_dmx_enable_vq_buffers(u16 vq_num);
void mtk_dmx_mcm_ctrl(bool bEnable);
void mtk_dmx_interrupt_ctrl(bool bEnable);
void mtk_dmx_ntp_interrupt_ctrl(int path, bool bEnable);

u32 mtk_dmx_get_sw_interrupt_status(void);
bool mtk_dmx_get_pcr_interrupt_status(u16 filter_id);
bool mtk_dmx_get_ntp_interrupt_status(u16 filter_id);
void mtk_dmx_clear_sw_interrupt_status(void);
void mtk_dmx_clear_pcr_interrupt_status(u16 filter_id);
void mtk_dmx_clear_ntp_interrupt_status(u16 filter_id);

void mtk_dmx_reset_afifo(u16 eng_id, bool bReset);
void mtk_dmx_reset_vfifo(u16 eng_id, bool bReset);
void mtk_dmx_reset_tso(void);

u32 mtk_dmx_get_video_drop_cnt(u16 eng_id);
u32 mtk_dmx_get_audio_drop_cnt(u16 eng_id);
u32 mtk_dmx_get_video_dis_conti_cnt(u16 eng_id);
u32 mtk_dmx_get_audio_dis_conti_cnt(u16 eng_id);
u32 mtk_dmx_get_video_pkt_cnt(u16 eng_id);
u32 mtk_dmx_get_audio_pkt_cnt(u16 eng_id);
u32 mtk_dmx_get_video_fifo_level(u16 eng_id);
u32 mtk_dmx_get_video_fifo_empty_bit(u16 eng_id);
u32 mtk_dmx_get_video_fifo_full_bit(u16 eng_id);
u32 mtk_dmx_get_audio_fifo_level(u16 eng_id);
u32 mtk_dmx_get_audio_fifo_empty_bit(u16 eng_id);
u32 mtk_dmx_get_audio_fifo_full_bit(u16 eng_id);

void mtk_dmx_set_ntp_base_point(u32 base);
void mtk_dmx_set_leap_ind_eq_mode(u16 ind, bool add, bool sub);

#endif /* MTK_DMX_CTRL_H */

