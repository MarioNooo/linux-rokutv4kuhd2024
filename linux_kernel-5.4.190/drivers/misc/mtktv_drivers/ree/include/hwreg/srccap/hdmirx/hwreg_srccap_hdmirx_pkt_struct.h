#ifndef HWREG_SRCCAP_HDMIRX_PKT_STRUCT_H
#define HWREG_SRCCAP_HDMIRX_PKT_STRUCT_H


struct  __attribute__((__packed__))
pkt_que_time_s {
// min,avg,max
    MS_U32 u32_vsync_interval[3];
    MS_U32 u32_vsync_handler[3];
    MS_U8 u8_framecnt_interval[3];
};

struct __attribute__((__packed__))
hdmi_pkt_report_s {
    struct pkt_que_time_s time_info;
    MS_U16 u16_latency[4];
    MS_U32 u32_isr_cnt;
    MS_U16 u16_frame_cnt_chg_at_handler;
    MS_U16 u16_handler_late;
    MS_U16 u16_pkt_overwrite;
    MS_U16 u16_crc_mismatch;
};

#endif
