/*
 *
 * Copyright (c) 2020-2024 MediaTek Inc.
 * All rights reserved.
 *
 * Unless otherwise stipulated in writing, any and all information contained
 * herein regardless in any format shall remain the sole proprietary of
 * MediaTek Inc. and be kept in strict confidence
 * ("MediaTek Confidential Information") by the recipient.
 * Any unauthorized act including without limitation unauthorized disclosure,
 * copying, use, reproduction, sale, distribution, modification, disassembling,
 * reverse engineering and compiling of the contents of MediaTek Confidential
 * Information is unlawful and strictly prohibited. MediaTek hereby reserves the
 * rights to any and all damages, losses, costs and expenses resulting therefrom.
 *
 */

/*
 *
 * @author Mediatek Inc.
 *
 */

#ifndef _HWREG_SRCCAP_HDMIRX_PACKETRECEIVER_H_
#define _HWREG_SRCCAP_HDMIRX_PACKETRECEIVER_H_

#include "hwreg_common.h"

#include "drvXC_IOPort.h"
#include "drvXC_HDMI_if.h"
#include "hwreg_srccap_hdmirx_pkt_hdr.h"
#include "hwreg_srccap_hdmirx_pkt_struct.h"
#include "hwreg_srccap_hdmirx_struct.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */

#define HDMI_AUDIO_MAX_FS_HZ ((768+1)*ONE_K_UNIT)
#define HDMI_AUDIO_MIN_FS_HZ ((32-1)*ONE_K_UNIT)

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */
typedef enum
{
    E_EMP_VED_MORE_PKT = 0,
    E_EMP_VRR_MORE_PKT,
    E_EMP_HDR_MORE_PKT,
    E_EMP_DSC_MORE_PKT,
    E_EMP_VED_NOT_EVERY_MTW,
    E_EMP_VRR_NOT_EVERY_MTW,
    E_EMP_HDR_NOT_EVERY_MTW,
    E_EMP_DSC_NOT_EVERY_MTW,
    E_EMP_VED_ON_NON_FAPA,
    E_EMP_VRR_ON_NON_FAPA,
    E_EMP_HDR_ON_NON_FAPA = 10,
    E_EMP_DSC_ON_NON_FAPA,
    E_EMP_DSC_SEQ_WRONG,
    E_EMP_VED_INCOMPLETE_PKT,
    E_EMP_VRR_INCOMPLETE_PKT,
    E_EMP_HDR_INCOMPLETE_PKT,
    E_EMP_DSC_INCOMPLETE_PKT,
    E_EMP_DSC_LAST_EVT,
    E_EMP_VED_FIRST_EVT,
    E_EMP_VRR_FIRST_EVT,
    E_EMP_HDR_FIRST_EVT = 20,
    E_EMP_DSC_FIRST_EVT,
    E_EMP_PKT_DIFF,
    E_EMP_PKT_RECEIVED,
    E_EMP_DSC_PKT_DIFF,
    E_EMP_VRR_CHG_PULSE,
    E_EMP_DSC_CHG_PULSE,
    E_EMP_MTW_PULSE,
    E_EMP_VSYNC_AHEAD,
    E_EMP_VDE_END_TMDS_P,
    E_EMP_HDR_SINGLE_DSF_EVT,
    E_EMP_DSC_SINGLE_DSF_EVT = 31,
}EN_HDMI_EMP_EVT;


/// HDMI reset type
typedef enum
{
	REST_AVMUTE = _BIT(8),    ///<HDMI Reset bit[8]
	REST_Y_COLOR = _BIT(9),    ///<HDMI Reset bit[9]
	REST_PIXEL_REPETITION  = _BIT(10),    ///<HDMI Reset bit[10]
	REST_FRAME_REPETITION  = _BIT(11),    ///<HDMI Reset bit[11]
	REST_GCP_PKT  = _BIT(12),    ///<HDMI Reset bit[12]
	REST_DEEP_COLOR_FIFO  = _BIT(13),    ///<HDMI Reset bit[13]
	REST_RESERVE = _BIT(14),    ///<HDMI Reset bit[14]
	REST_HDMI_STATUS = _BIT(15),    ///<HDMI Reset bit[15]
	REST_PKTQUE = _BIT(31),
}HDMI_REST_t;


enum HDMI_STATUS_FLAG_TYPE
{
	HDMI_STATUS_MPEG_PACKET_RECEIVE_FLAG = BIT(0),
	HDMI_STATUS_AUDIO_PACKET_RECEIVE_FLAG = BIT(1),
	HDMI_STATUS_SPD_PACKET_RECEIVE_FLAG = BIT(2),
	HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG = BIT(3),
	HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG = BIT(4),
	HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG = BIT(5),
	HDMI_STATUS_ACR_PACKET_RECEIVE_FLAG = BIT(6),
	HDMI_STATUS_VS_PACKET_RECEIVE_FLAG = BIT(7),
	HDMI_STATUS_NULL_PACKET_RECEIVE_FLAG = BIT(8),
	HDMI_STATUS_ISRC2_PACKET_RECEIVE_FLAG = BIT(9),
	HDMI_STATUS_ISRC1_PACKET_RECEIVE_FLAG = BIT(10),
	HDMI_STATUS_ACP_PACKET_RECEIVE_FLAG = BIT(11),
	HDMI_STATUS_DSD_PACKET_RECEIVE_FLAG = BIT(12),
	HDMI_STATUS_GM_PACKET_RECEIVE_FLAG = BIT(13),
	HDMI_STATUS_HBR_PACKET_RECEIVE_FLAG = BIT(14),
	HDMI_STATUS_VBI_PACKET_RECEIVE_FLAG = BIT(15),
	HDMI_STATUS_HDR_PACKET_RECEIVE_FLAG = BIT(16),
	HDMI_STATUS_RESERVED_PACKET_RECEIVE_FLAG = BIT(17),
	HDMI_STATUS_EDR_VALID_FLAG = BIT(18),
	HDMI_STATUS_AUDIO_DST_RECEIVE_FLAG = BIT(19),
	HDMI_STATUS_AUDIO_3D_ASP_RECEIVE_FLAG = BIT(20),
	HDMI_STATUS_AUDIO_3D_DSD_RECEIVE_FLAG = BIT(21),
	HDMI_STATUS_AUDIO_METADATA_RECEIVE_FLAG = BIT(22),
	HDMI_STATUS_AUDIO_MULTI_ASP_RECEIVE_FLAG = BIT(23),
	HDMI_STATUS_AUDIO_MULTI_DSD_RECEIVE_FLAG = BIT(24),
	HDMI_STATUS_EM_PACKET_RECEIVE_FLAG = BIT(25),
	HDMI_STATUS_EM_VENDOR_PACKET_RECEIVE_FLAG = BIT(26),
	HDMI_STATUS_EM_VTEM_PACKET_RECEIVE_FLAG = BIT(27),
	HDMI_STATUS_EM_DSC_PACKET_RECEIVE_FLAG = BIT(28),
	HDMI_STATUS_EM_HDR_PACKET_RECEIVE_FLAG = BIT(29),
};

enum HDMI_BCH_ERROR_TYPE
{
	HDMI_BCH_ERROR_CORRECTED = BIT(0),
	HDMI_BCH_ERROR = BIT(1),
	HDMI_CHECKSUM_ERROR = BIT(2),
	HDMI_PHY_STATE_NOT_READY = BIT(3),
	HDMI_REGEN_NOT_DONE = BIT(4),
	HDMI_UNSUPPORT_PACKET = BIT(5),
};

enum HDMI_INFO_SOURCE_TYPE
{
	HDMI_INFO_SOURCE0 = 0,
	HDMI_INFO_SOURCE1,
	HDMI_INFO_SOURCE2,
	HDMI_INFO_SOURCE3,
	HDMI_INFO_SOURCE_MAX,
	HDMI_INFO_SOURCE_NONE,
};

enum HDMI_PKT_DIFF_BIT
{
	E_HDMI_PKT_GCP_DIFF_BIT = 0,
	E_HDMI_PKT_AVI_DIFF_BIT,
	E_HDMI_PKT_SPD_DIFF_BIT,
	E_HDMI_PKT_RSV_DIFF_BIT,
	E_HDMI_PKT_AUI_DIFF_BIT = 10,
	E_HDMI_PKT_ACR_DIFF_BIT,
	E_HDMI_PKT_ACP_DIFF_BIT,
};


/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

typedef struct __attribute__ ((packed))
{
    // port index from E_MUX_INPUTPORT
    E_MUX_INPUTPORT enInputPortType;
    // indicate which type of packet to reset
    HDMI_REST_t enResetType;
} ST_HDMI_PACKET_RESET;


/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_init(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_deinit(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_suspend(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_resume(
    E_MUX_INPUTPORT enInputPortType);
extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_rst(
	E_MUX_INPUTPORT enInputPortType,
	MS_U8 u8HDMIInfoSource,
	HDMI_REST_t enResetType,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_U16 drv_hwreg_srccap_hdmirx_pkt_get_gnl(
    E_MUX_INPUTPORT enInputPortType,
    MS_HDMI_PACKET_STATE_t enPacketType,
    struct pkt_prototype_gnl* buf_ptr,
    MS_U32 u32_n);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_get_avi(
    E_MUX_INPUTPORT enInputPortType,
    struct pkt_prototype_avi_if* buf_ptr,
    MS_U32 u32_n);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_get_gcp(
    E_MUX_INPUTPORT enInputPortType,
    struct pkt_prototype_gcp* buf_ptr,
    MS_U32 u32_n);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_get_hdrif(
    E_MUX_INPUTPORT enInputPortType,
    struct pkt_prototype_hdr_if* buf_ptr,
    MS_U32 u32_n);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_get_report(
    E_MUX_INPUTPORT enInputPortType,
    MS_HDMI_PACKET_STATE_t enPacketType,
    struct hdmi_pkt_report_s* buf_ptr);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_is_dsc(
    E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_is_vrr(
    E_MUX_INPUTPORT enInputPortType);

extern MS_U16 drv_hwreg_srccap_hdmirx_pkt_get_emp_evt(
    E_MUX_INPUTPORT enInputPortType,
    EN_HDMI_EMP_EVT evt);

extern void drv_hwreg_srccap_hdmirx_pkt_clr_emp_evt(
    E_MUX_INPUTPORT enInputPortType,
    EN_HDMI_EMP_EVT evt);


extern MS_U8
drv_hwreg_srccap_hdmirx_pkt_acp_info(
	MS_U8 ucHDMIInfoSource, stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_U8
drv_hwreg_srccap_hdmirx_pkt_audio_channel_status(
	MS_U8 ucHDMIInfoSource,
	MS_U8 u8byte,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern EN_AVI_INFOFRAME_VERSION
drv_hwreg_srccap_hdmirx_pkt_avi_infoframe_info_ver(
	MS_U8 ucHDMIInfoSource,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_U8 drv_hwreg_srccap_hdmirx_pkt_avi_infoframe_info(
	MS_U8 ucHDMIInfoSource,
	MS_U8 u8byte,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_U32 drv_hwreg_srccap_hdmirx_pkt_packet_info(
	MS_U8 ucHDMIInfoSource,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern E_HDMI_ADDITIONAL_VIDEO_FORMAT
	drv_hwreg_srccap_hdmirx_pkt_check_additional_format(
		MS_U8 ucHDMIInfoSource,
		stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern
MS_U8 drv_hwreg_srccap_hdmirx_pkt_getpacketreceivercount(
	MS_U8 ucHDMIInfoSource,
	MS_U8 u8PrePacketReceiveCount,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_get_packet_value(
	MS_U8 ucHDMIInfoSource,
	MS_HDMI_PACKET_STATE_t u8state,
	MS_U8 u8byte_idx,
	MS_U8 *pu8Value,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_U8 drv_hwreg_srccap_hdmirx_pkt_get_errstatus(
	MS_U8 ucHDMIInfoSource,
	MS_U8 u8value,
	MS_BOOL bread,
	stHDMI_POLLING_INFO *p_info);

extern E_XC_3D_INPUT_MODE
	drv_hwreg_srccap_hdmirx_pkt_get_3d_structure(
		MS_U8 ucHDMIInfoSource,
		stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern E_HDMI_3D_EXT_DATA_T
	drv_hwreg_srccap_hdmirx_pkt_get_3d_ext_data(
		MS_U8 ucHDMIInfoSource,
		stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern void drv_hwreg_srccap_hdmirx_pkt_get_3d_meta_field(
		MS_U8 ucHDMIInfoSource,
		sHDMI_3D_META_FIELD *p_buf,
		stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_U8 drv_hwreg_srccap_hdmirx_pkt_getisrc1headerinfo(
	MS_U8 ucHDMIInfoSource,
	stHDMI_POLLING_INFO *pstHDMIPollingInfo);

extern MS_U16 drv_hwreg_srccap_hdmirx_pkt_gcontrol_info(
	HDMI_GControl_INFO_t gcontrol, MS_U8 ucHDMIInfoSource);

extern MS_U8 drv_hwreg_srccap_hdmirx_pkt_getgcpcolordepth(
	E_MUX_INPUTPORT enInputPortType, MS_U32 u32PacketStatus);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_audio_init_setting(
	enum HDMI_AUDIO_PATH_TYPE audio_mux_n);


extern  MS_BOOL drv_hwreg_srccap_hdmirx_pkt_audio_mute_en(
	enum HDMI_AUDIO_PATH_TYPE audio_mux_n,
	MS_U16 u16MuteEvent,
	MS_U16 u16MuteMask);


extern  MS_BOOL
drv_hwreg_srccap_hdmirx_pkt_audio_mute_status_clr(
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_pkt_pktq_dump(
	E_MUX_INPUTPORT enInputPortType, MS_U32 u32_cnt);


extern MS_U32 drv_hwreg_srccap_hdmirx_pkt_get_audio_fs_hz(
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_is_fva(
	E_MUX_INPUTPORT enInputPortType);

extern void drv_hwreg_srccap_hdmirx_pkt_set_emp_irq_onoff(
	E_MUX_INPUTPORT enInputPortType,
	EN_HDMI_EMP_EVT evt,
	MS_BOOL b_on);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_get_emp_irq_onoff(
	E_MUX_INPUTPORT enInputPortType,
	EN_HDMI_EMP_EVT evt);

extern MS_BOOL drv_hwreg_srccap_hdmirx_pkt_get_emp_vtem_info(
	E_MUX_INPUTPORT enInputPortType,
	struct s_pkt_emp_vtem *p_info);

#endif // _HWREG_SRCCAP_HDMIRX_PACKETRECEIVER_H_
