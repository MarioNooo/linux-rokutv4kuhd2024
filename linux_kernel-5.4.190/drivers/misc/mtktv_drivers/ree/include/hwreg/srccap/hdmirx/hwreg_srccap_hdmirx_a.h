#ifndef HWREG_SRCCAP_HDMIRX_A_H
#define HWREG_SRCCAP_HDMIRX_A_H

#include "hwreg_common.h"
#include "drvXC_HDMI_if.h"

#include "hwreg_srccap_hdmirx_struct.h"
#include "hwreg_srccap_hdmirx_pkt_hdr.h"

#define H21_SUPPORT

// #define CED_RSCC_ENHS_HW_MODE
#ifdef CED_RSCC_ENHS_HW_MODE
#define HDMI_CTS_SW_PATCH_CED               0
#define HDMI_CTS_SW_PATCH_RS                0
#else
#define HDMI_CTS_SW_PATCH_CED               1
#define HDMI_CTS_SW_PATCH_RS                1
#endif

#ifdef CONFIG_MSTAR_ARM_BD_FPGA
// 9999
// #define FPGA_ENABLE
#else
// 88888
// #define FPGA_ENABLE
#endif

#ifdef CONFIG_MSTAR_ARM_BD_GENERIC
// 7777
#endif

#ifdef FPGA_ENABLE
#define IMMESWITCH_POWER_SAVING 0
#else
#define IMMESWITCH_POWER_SAVING 1
#endif
// #define DUMP_INFO_ENABLE

enum e_hdmi_pkt_isr_list{
	E_HDMI_PKT_ISR_PKTQ,
	E_HDMI_PKT_DEC_MISC,
	E_HDMI_PKT_DEP_MISC,
	E_HDMI_PKT_EMP,
	E_HDMI_PKT_ISR_MAX,
};

enum e_hdmi_frl_isr_list
{
	E_HDMI_FRL_ISR_CEDRSCC,
	E_HDMI_FRL_ISR_MAX,
};

typedef enum
{
	HDMI_RST_FRLDEC_DETECT = 0,
	HDMI_RST_FRLDEC_DEPACKER,
	HDMI_RST_FRLDEC_SRAM,
	HDMI_RST_FRLDEC_TIMING,
	HDMI_RST_FRLDEC_ALL = 7,
}EN_HDMI_SWRST_FRLDEC;

enum E_HDMI_PWR_SCENARIO {
	E_HDMI_PWR_CR_LOCK,
	E_HDMI_PWR_HDMI_DVI,
	E_HDMI_PWR_DSC,
	E_HDMI_PWR_TIMING,
	E_HDMI_PWR_HDCP,
	E_HDMI_PWR_MISC,
};

enum E_AUDIO_FS_TYPE {
	E_HDMI_AUDIO_FS_CTS_N,
	E_HDMI_AUDIO_FS_SYNTH,
	E_HDMI_AUDIO_FS_128FS,
};


extern enum hdmi_hw_module
	drv_hwreg_srccap_hdmirx_a_query_modulever(void);
extern MS_BOOL
	drv_hwreg_srccap_hdmirx_a_issupportimmeswitchps(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_a_issupportsrampd(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_a_yuv420need2xclk(
	E_MUX_INPUTPORT enInputPortType);

extern MS_U32 drv_hwreg_srccap_hdmirx_a_query_macdivider(
	MS_U32 p0);
extern MS_U32 drv_hwreg_srccap_hdmirx_a_query_phydivider(
	MS_U32 p0);
extern MS_U32 drv_hwreg_srccap_hdmirx_a_query_paralleldtop2mac(
	MS_U32 p0);

extern MS_BOOL drv_hwreg_srccap_hdmirx_a_query_2p1_port(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_a_query_dsc_port(
	E_MUX_INPUTPORT enInputPortType);

extern MS_U32 drv_hwreg_srccap_hdmirx_a_get_loop_cnt(void);

extern void drv_hwreg_srccap_hdmirx_frl_isr(
	E_MUX_INPUTPORT enInputPortType,
	enum e_hdmi_frl_isr_list e_isr_n,
	void* param1);

extern void drv_hwreg_srccap_hdmirx_pkt_isr(
	E_MUX_INPUTPORT enInputPortType,
	enum e_hdmi_pkt_isr_list e_isr_n,
	void* param1);

extern void drv_hwreg_srccap_hdmirx_mac_set_pausepolling(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL bPauseEN);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mac_get_pausepolling(
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_mac_pwr_switch(
	enum E_HDMI_PWR_SCENARIO e_pwr_scenario,
	E_MUX_INPUTPORT enInputPortType,
	MS_U32 u32_p0,
	MS_U32 u32_p1);

extern void drv_hwreg_srccap_hdmirx_mac_pwr_off_proc(
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern void drv_hwreg_srccap_hdmirx_phy_set_yuv420setting(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL bYUV420Flag,
	MS_U8 u8SourceVersion,
	MS_U8 u8Colordepth);

extern MS_BOOL drv_hwreg_srccap_hdmirx_mux_macmuxsetting(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern void drv_hwreg_srccap_hdmirx_pkt_clr_edrvalidflag(
	MS_U8 u8HDMIInfoSource,
	MS_U8 ucSourceVersion);

extern MS_U16 drv_hwreg_srccap_hdmirx_pkt_get_empinfo(
	E_MUX_INPUTPORT enInputPortType,
	MS_U8 u8HDMIInfoSource,
	MS_U32 u32PacketStatus,
	MS_U8 ucSourceVersion);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_is_diff(
	E_MUX_INPUTPORT enInputPortType,
	MS_HDMI_PACKET_STATE_t e_pkt);

extern MS_U32 drv_hwreg_srccap_hdmirx_pkt_get_audio_fs_hz_adv(
	enum HDMI_AUDIO_PATH_TYPE audio_mux_n,
	E_MUX_INPUTPORT enInputPortType,
	enum E_AUDIO_FS_TYPE e_type,
	EN_HDMI_SIGNAL_TYPE e_sgl);

extern MS_U8 drv_hwreg_srccap_hdmirx_pkt_get_dsc_pps(
	MS_U8 ucHDMIInfoSource,
	MS_U16 u16_pps_n);

extern void drv_hwreg_srccap_hdmirx_phy_set_powerdownphdac(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL b_powerdown);

extern void drv_hwreg_srccap_hdmirx_frl_rst_srssb(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL b_rst);

extern void drv_hwreg_srccap_hdmirx_frl_rst_symerrcnt(
	E_MUX_INPUTPORT enInputPortType,
	MS_U8 symerr_type);

extern void drv_hwreg_srccap_hdmirx_frl_rst_rserrcnt(
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_frl_rst_sramsel(
	MS_BOOL bhw_mode,
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_frl_rst_fec(
	E_MUX_INPUTPORT enInputPortType);


extern void drv_hwreg_srccap_hdmirx_frl_rst_ftlnoretrain(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *p_info);

extern void drv_hwreg_srccap_hdmirx_frl_get_rserrcnt(
	E_MUX_INPUTPORT enInputPortType,
	MS_U16 *p_buf);



extern void drv_hwreg_srccap_hdmirx_frl_set_dschvregen(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL b_en);


extern void drv_hwreg_srccap_hdmirx_frl_proc_scdc_noretrain(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *p_info);

extern void drv_hwreg_srccap_hdmirx_frl_decoder_rst(
	E_MUX_INPUTPORT enInputPortType,
	EN_HDMI_SWRST_FRLDEC enRstBit);

extern void drv_hwreg_srccap_hdmirx_phy_busypolling2p1(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *p_info);
extern void drv_hwreg_srccap_hdmirx_phy_busypolling(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *p_info);
extern void drv_hwreg_srccap_hdmirx_phy_fasttraining2p1(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *p_info);
extern void drv_hwreg_srccap_hdmirx_phy_fasttraining(
	E_MUX_INPUTPORT enInputPortType,
	stHDMI_POLLING_INFO *p_info);


extern unsigned int drv_hwreg_srccap_hdmirx_irq_mask_save(
	EN_KDRV_HDMIRX_INT e_int,
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_irq_mask_restore(
	EN_KDRV_HDMIRX_INT e_int,
	E_MUX_INPUTPORT enInputPortType,
	unsigned int msk);

extern MS_BOOL drv_hwreg_srccap_hdmirx_tmds_set_errcntmaxlevel(
	E_MUX_INPUTPORT enInputPortType,
	MS_U32 u32_tmds_rate_mhz,
	MS_BOOL b_ove);

extern enum e_pkt_type_header
drv_hwreg_srccap_hdmirx_query_pkt_header_by_pktindex(
	enum e_pkt_type_indx);

enum e_pkt_type_header
drv_hwreg_srccap_hdmirx_query_pkt_header_by_pktstate(
	MS_HDMI_PACKET_STATE_t src_pkt);

extern enum e_pkt_type_indx
drv_hwreg_srccap_hdmirx_pktstate_2_pktindex(
	MS_HDMI_PACKET_STATE_t src_pkt);

#endif
