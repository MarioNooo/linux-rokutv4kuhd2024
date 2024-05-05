///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020-2024 MediaTek Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MediaTek Inc. and be kept in strict confidence
// ("MediaTek Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MediaTek Confidential
// Information is unlawful and strictly prohibited. MediaTek hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
/// @file   dv_core_if.h
/// @brief  DoVi Core Interface and common definitions
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _DV_CORE_IF_H_
#define _DV_CORE_IF_H_

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#include <linux/module.h>
#include <linux/dma-buf.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/string.h>
#else
#include "pqu_port.h"
#endif

#include "m_pqu_cfd.h"
#include "m_pqu_pq.h"

#include "hwreg_common.h"


//-----------------------------------------------------------------------------
// Driver Capability
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macros and Defines
//-----------------------------------------------------------------------------

#define MAX_OSD_SETTING_LENGTH		(32)
#define MAX_PICTURE_MODE_NAME_LENGTH	(32)
#define MAX_PICTURE_MODE_NUM		(10)


//-----------------------------------------------------------------------------
// Enums and Structures
//-----------------------------------------------------------------------------

enum en_dv_ret_value {
	EN_DV_RET_OK = 0,
	EN_DV_RET_INVALID_PARAM,
	EN_DV_RET_OPERATION_NOT_PERMITTED,
	EN_DV_RET_RESOURCE_NOT_ENOUGH,
	EN_DV_RET_NOT_INITED,
	EN_DV_REG_PERMISSION_DENIED,
};


enum en_dv_core_handler_version {
	EN_DV_CORE_HANDLER_VERSION_V1_INIT = 0,

	EN_DV_CORE_HANDLER_VERSION_MAX,
	EN_DV_CORE_HANDLER_VERSION_CURRENT_VERION = EN_DV_CORE_HANDLER_VERSION_MAX - 1,
};


enum en_dv_input_source {
	EN_DV_INPUT_SRC_HDMI_0 = 0,
	EN_DV_INPUT_SRC_HDMI_1,
	EN_DV_INPUT_SRC_OTT,

	EN_DV_INPUT_SRC_MAX
};


enum en_dolby_source_type {
	EN_DV_NONE = 0,
	EN_DV_HDMI_STD,
	EN_DV_HDMI_LL_YUV,
	EN_DV_HDMI_LL_RGB,
	EN_DV_HDMI_DRM_LL_YUV,
	EN_DV_HDMI_DRM_LL_RGB,
	EN_DV_HDMI_FORM1,
	EN_DV_HDMI_FORM2_YUV,
	EN_DV_HDMI_FORM2_RGB,
	EN_DV_OTT_RPU,
	EN_DV_OTT_SEMPT2094,

	EN_DV_SOURCE_MAX,
};

struct st_dv_core_version_ctrl {
	int version;
	int length;
};

struct st_dolby_core_meta {
	union {
		struct m_pq_dv_hdmi_info stHdmiInfo;
		struct vdec_dd_dolby_desc stVdecDesc;
	}dv_meta;
	struct vdec_dd_color_desc stColorInfo;
	struct m_pq_dv_core_param_info stCoreParam;
};

struct st_dv_core_pq_res {        /* resolution info */
	uint32_t fe_in_width;         /* front end input width */
	uint32_t fe_in_height;        /* front end input height */
	uint32_t fe_crop_width;       /* front end pyr crop output width */
	uint32_t fe_crop_height;      /* front end pyr crop output height */
	uint32_t fe_sd_width;         /* front end pyr sd output width */
	uint32_t fe_sd_height;        /* front end pyr sd output height */
	uint32_t fe_crop2_width;      /* front end pyr2 crop output width */
	uint32_t fe_crop2_height;     /* front end pyr2 crop output height */
	uint32_t be_in_width;         /* back end input width */
	uint32_t be_in_height;        /* back end input height */
	uint32_t be_pre_crop_width;   /* back end pre crop output width */
	uint32_t be_pre_crop_height;  /* back end pre crop output height */
	uint32_t be_post_crop_width;  /* back end post crop output width */
	uint32_t be_post_crop_height; /* back end post crop output height */
};

struct st_dolby_stream_meta {
	struct window_info input_win;
	struct st_dv_core_pq_res res;
	bool hfr_en;
};

struct st_dolby_buffer_info {
	uint64_t frame_iova_in; //PR buffer
};

struct st_dolby_user_info {
	bool PR_en;
};

struct st_dolby_md_info {
	enum en_dv_input_source input_source;
	enum en_dolby_source_type dv_type;
	struct st_dolby_core_meta core_meta;
	struct st_dolby_stream_meta stream_meta;
	struct m_pq_dv_info pq_meta;
	struct st_dolby_buffer_info bufferinfo;
	struct st_dolby_user_info userinfo;
	enum en_dolby_source_type pre_dv_type;
	bool is_at_op;
};


struct st_dv_core_reg_out {
	struct hwregOut *pstHwRegOut;
	uint32_t maxCount;
};


struct st_dv_core_reg_info {
	struct st_dv_core_reg_out stRegX16_prio;	// prior
	struct st_dv_core_reg_out stRegX16;		// main
	struct st_dv_core_reg_out stRegX32_prio;	// prior
	struct st_dv_core_reg_out stRegX32;		// main
};


struct st_dv_core_content_info {
	void *pData;
	uint32_t u32Size;	// used length
	uint32_t u32MemSize;	// memory size
};

struct st_dv_core_adl_info {
	struct st_dv_core_content_info stAdlInfoCore1;
	struct st_dv_core_content_info stAdlInfoCore2;
	struct st_dv_core_content_info stAdlInfoCore2Tv5;
};

struct st_dv_core_core2_l1l4 {
	uint16_t L1_min;
	uint16_t L1_mid;
	uint16_t L1_max;
	uint16_t L4_std;
};

/* refer to struct mtk_pq_dv_config_bin_info & v4l2_PQ_dv_config_bin_info_s */
struct st_dv_config_bin_info {
	uint64_t pa;
	uint64_t va;
	uint32_t size;
	uint32_t en;
};

/* refer to struct v4l2_PQ_dv_config_mode_info_s */
struct st_dv_config_mode_info {
	uint32_t en;
	uint32_t num;
	uint8_t name[MAX_PICTURE_MODE_NUM][MAX_PICTURE_MODE_NAME_LENGTH + 1];
};

/* refer to enum v4l2_PQ_dv_support_capability & MI_DISP_DV_SupportCapability_e */
enum en_dv_support_capability {
	EN_DV_CAPABILITY_NONE				= 0,
	EN_DV_CAPABILITY_DOLBY_VISION_SUPPORT		= 0x1,
	EN_DV_CAPABILITY_DOLBY_GFX_SUPPORT		= 0x2,
	EN_DV_CAPABILITY_DOLBY_IQ_SUPPORT		= 0x4,
	EN_DV_CAPABILITY_DOLBY_EUC_SUPPORT		= 0x8,
	EN_DV_CAPABILITY_DOLBY_APO_SUPPORT		= 0x10,
	EN_DV_CAPABILITY_DOLBY_LS_SUPPORT		= 0x20,

	EN_DV_CAPABILITY_MAX
};

/* refer to struct v4l2_PQ_dv_config_capability_info & MI_DISP_DV_ConfigCapabilityInfo_t */
struct st_dv_config_capability_info {
	uint32_t en;
	uint32_t capability;	// enum en_dv_support_capability
};

/* refer to struct v4l2_PQ_dv_config_control_info & MI_DISP_DV_ConfigControlInfo_t */
struct st_dv_config_control_info {
	uint8_t disable_dv;
};

/* refer to struct mtk_pq_dv_config_hw_info */
struct st_dv_config_hw_info {
	bool updatedFlag;
	uint32_t hw_ver;  //ts domain dv hw version (include FE & BE)
};

/* refer to struct v4l2_PQ_dv_config_info_s & MI_DISP_dv_config_info_t */
struct st_dv_config_info {
	struct st_dv_config_bin_info bin_info;
	struct st_dv_config_mode_info mode_info;
	struct st_dv_config_capability_info cap_info;
	struct st_dv_config_control_info ctrl_info;
};

struct st_dv_osd_setting {
	bool valid;
	uint8_t str[MAX_OSD_SETTING_LENGTH + 1];
};

struct st_dv_osd_ctrl {
	struct st_dv_osd_setting picture_mode;
	uint8_t view_id;
	struct st_dv_osd_setting pr_osd;
	bool pr_en;
	struct st_dv_osd_setting iq_apo_osd;
	bool iq_apo_en;
	struct st_dv_osd_setting iq_ls_osd;
	bool iq_ls_en;
	struct st_dv_osd_setting dark_detail_osd;
	bool dark_detail_en;
    struct st_dv_osd_setting global_dimming_osd;
    bool global_dimming_en;
};


struct st_dv_cfg_input_param {
	/* data - 3dlut */
	bool data_en;
	uint8_t *p8data;
	uint32_t u32Size;

	/* picture mode name */
	bool pic_mode_en;
	uint8_t pic_mode_num;
	uint8_t pic_mode_name[MAX_PICTURE_MODE_NUM][MAX_PICTURE_MODE_NAME_LENGTH + 1];

	/* data - hw ver */
	bool hw_ver_en;
	uint32_t dv_hwver;
};


struct st_dv_core_input_param {
	// version control
	struct st_dv_core_version_ctrl stVerCtrl;

	/* version EN_DV_CORE_HANDLER_VERSION_V1_INIT start */
	// primary meta
	struct st_dolby_md_info *pstDvMdInfoPrimary;
	// secondary meta
	struct st_dolby_md_info *pstDvMdInfoSecondary;
	/* version EN_DV_CORE_HANDLER_VERSION_V1_INIT end */

	/* Debug Mode */
	struct m_pq_dv_debug_force_control *debug_force_ctrl;

	/* OSD */
	struct st_dv_osd_ctrl *osd_ctrl;

	/* HW Version */
	struct st_dv_config_hw_info *hw_info;
};


struct st_dv_core_output_param {
	// version control
	struct st_dv_core_version_ctrl stVerCtrl;

	/* version EN_DV_CORE_HANDLER_VERSION_V1_INIT start*/
	// ML registers
	struct st_dv_core_reg_info stRegInfoMl;
	// RIU registers
	struct st_dv_core_reg_info stRegInfoRiu;
	// ADL data
	struct st_dv_core_adl_info stAdlInfo;
	// AUL data
	struct st_dv_core_content_info stAulInfo;
	/* version EN_DV_CORE_HANDLER_VERSION_V1_INIT end*/

	struct st_dv_core_core2_l1l4 stCore2L1L4; // Legacy mode only
};

struct st_dv_core_l11_info {
	uint8_t u8WPValid;
	uint8_t u8ContentType;
	uint8_t u8WhitePoint;
	uint8_t u8L11Byte2;
	uint8_t u8L11Byte3;
};

struct st_dv_core_status_info {
	/* debug level */
	uint32_t debug_level;

	/* 3DLUT */
	uint8_t* lut_bin_data;
	uint32_t lut_bin_size;

	/* picture mode */
	uint8_t pic_mode_num;
	uint8_t pic_mode_name[MAX_PICTURE_MODE_NUM][MAX_PICTURE_MODE_NAME_LENGTH + 1];
	uint8_t cur_view_id;

	/* global dimming */
	struct dv_gd_info gd_info;

	/* hw version */
	uint32_t dv_hw_ver;

	/* pr status */
	bool cur_frame_pr_en;
	bool next_frame_pr_en;

	/* resolution info */
	struct st_dv_core_pq_res res;
	struct st_dv_core_pq_res next_res;

	/* Level 11 info */
	struct st_dv_core_l11_info l_11_Data;
};


struct st_dv_core_hw_status_info {
	/* L1L4 */
	uint16_t l1_min_val;
	uint16_t l1_max_val;
	uint16_t l1_mid_val;
	uint16_t l4_std_val;
};


//-----------------------------------------------------------------------------
// Variables and Functions
//-----------------------------------------------------------------------------
int dv_core_handler_registerDvCoreInterface(void);
int dv_core_handler_hw_info(void *pParamIn);
int dv_core_handler_cfg(void *pParamIn, void *pParamOut);
int dv_core_handler_calc(void *pParamIn, void *pParamOut);
int dv_core_handler_rst(void *pParamIn, void *pParamOut);
int dv_core_handler_get_hw_status(void *pParamIn, void *pParamOut);

// debug
int dv_core_set_debug_level(void *pParamIn, void *pParamOut);
int dv_core_get_status_info(void *pParamIn, void *pParamOut);
#endif // _DV_CORE_IF_H_
