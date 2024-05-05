/* SPDX-License-Identifier: GPL-2.0
 * Copyright (c) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _M_PQU_CFD_H_
#define _M_PQU_CFD_H_

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#else
#include "pqu_port.h"
#endif

//-----------------------------------------------------------------------------
//  Driver Capability
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Macro and Define
//-----------------------------------------------------------------------------
#define M_PQ_CFD_INPUT_FORMAT_META_TAG     "m_pq_cfd_input_format"
#define M_PQ_CFD_OUTPUT_FORMAT_META_TAG    "m_pq_cfd_output_format"
#define MTK_VDEC_DD_COLOR_DESC_TAG         "MTK_VDEC_DD_COLORDESC"
#define MTK_VDEC_DD_HDR10PLUS_DESC_TAG     "MTK_VDEC_DD_HDR10PLUSDESC"
#define MTK_VDEC_DD_DOLBY_DESC_TAG         "MTK_VDEC_DD_DOLBYDESC"
#define META_SRCCAP_HDR_PKT_TAG            "srccap_hdr_pkt"
#define META_SRCCAP_HDR_VSIF_PKT_TAG       "srccap_hdr_vsif_pkt"
#define META_SRCCAP_HDR_EMP_PKT_TAG        "srccap_hdr_emp_pkt"
#define M_PQ_PQPARAM_TAG                   "m_pq_pqparam_pqu_info"
#define META_CFD_TEST_RULELIST_TAG         "META_CFD_TEST_RULELIST_TAG"

#define M_PQ_CFD_INPUT_FORMAT_VERSION      (0)
#define M_PQ_CFD_OUTPUT_FORMAT_VERSION     (0)
#define M_PQ_VDEC_COLOR_DESC_VERSION       (0)
#define M_PQ_VDEC_HDR10PLUS_VERSION        (0)
#define M_PQ_VDEC_DV_INFO_VERSION          (0)
#define META_SRCCAP_HDR_PKT_VERSION        (1)
#define META_SRCCAP_HDR_VSIF_PKT_VERSION   (1)
#define META_SRCCAP_HDR_EMP_PKT_VERSION    (1)
#define META_PQ_PQPARAM_VERSION (0)

//-----------------------------------------------------------------------------
//  Enum
//-----------------------------------------------------------------------------
enum vdec_dolby_type {
	VDEC_DOLBY_NONE = 0,
	VDEC_DOLBY_RPU = 1,
	VDEC_DOLBY_SEMPT2094 = 2,
};

//-----------------------------------------------------------------------------
//  Structure
//-----------------------------------------------------------------------------
struct m_pq_cfd_input_format {
	uint8_t source;

	uint8_t data_format;
	uint8_t bit_depth;
	uint8_t data_range;
	uint8_t colour_primaries;
	uint8_t transfer_characteristics;
	uint8_t matrix_coefficients;

	uint8_t hdr_mode;
	uint32_t u32SamplingMode;
};

struct m_pq_cfd_output_format {
	uint8_t source;
	uint8_t data_format;
	uint8_t bit_depth;
	uint8_t data_range;
	uint8_t colour_primaries;
	uint8_t transfer_characteristics;
	uint8_t matrix_coefficients;
};

struct vdec_dd_color_desc {
	__u32 max_luminance;
	__u32 min_luminance;
	__u16 primaries[3][2];
	__u16 white_point[2];
	__u16 max_content_light_level;
	__u16 max_pic_average_light_level;
	__u8 colour_primaries;
	__u8 transfer_characteristics;
	__u8 matrix_coefficients;
	__u8 video_full_range;
	__u8 is_hdr;
};

struct vdec_dd_hdr10plus_desc {
	__u8 u8Itu_T_T35_Country_Code;
	__u16 u16Terminal_Provider_Code;
	__u16 u16Terminal_Provider_Oriented_Code;
	__u8 u8Application_Identifier;
	__u8 u8Application_Version;
	__u8 u8Num_Windows;
	__u32 u32Targeted_System_Display_Maximum_Luminance;
	__u8 u8Targeted_System_Display_Actual_Peak_Luminance_Flag;
	__u32 u32Maxscl[3];
	__u32 u32Average_Maxrgb;
	__u8 u8Num_Distribution_Maxrgb_Percentiles;
	__u8 u8Distribution_Maxrgb_Percentages[9];
	__u32 u32Distribution_Maxrgb_Percentiles[9];
	__u16 u16Fraction_Bright_Pixels;
	__u8 u8Mastering_Display_Actual_Peak_Luminance_Flag;
	__u8 u8Tone_Mapping_Flag;
	__u16 u16Knee_Point_X;
	__u16 u16Knee_Point_Y;
	__u8 u8Num_Bezier_Curve_Anchors;
	__u16 u16Bezier_Curve_Anchors[9];
	__u8 u8Color_Saturation_Mapping_Flag;
};

struct vdec_dd_dolby_desc {
	enum vdec_dolby_type dolby_type;
	__u32 data_size;
	union {
		__u8 rpu_data[1024];
		__u8 sei_2094_data[256];
	};
};

struct meta_srccap_hdr_pkt {
	__u16 u16Version;
	__u16 u16Size;
	__u8 u8EOTF;
	__u8 u8Static_Metadata_ID;
	__u16 u16Display_Primaries_X[3];
	__u16 u16Display_Primaries_Y[3];
	__u16 u16White_Point_X;
	__u16 u16White_Point_Y;
	__u16 u16Max_Display_Mastering_Luminance;
	__u16 u16Min_Display_Mastering_Luminance;
	__u16 u16Maximum_Content_Light_Level;
	__u16 u16Maximum_Frame_Average_Light_Level;
	__u8 u8Version;
	__u8 u8Length;
};

struct meta_srccap_hdr_vsif_pkt {
	__u16 u16Version;      /// Version.
	__u16 u16Size;         /// Structure size.
	__u8 bValid;         /// if vsif info frame is valid
	__u8 u8VSIFTypeCode;
	__u8 u8VSIFVersion;
	__u8 u8Length;
	__u32 u32IEEECode;
	__u8 u8ApplicationVersion;
	__u8 u8TargetSystemDisplayMaxLuminance;
	__u8 u8AverageMaxRGB;
	__u8 au8DistributionValues[9];
	__u16 u16KneePointX;
	__u16 u16KneePointY;
	__u8 u8NumBezierCurveAnchors;
	__u8 au8BezierCurveAnchors[9];
	__u8 u8GraphicsOverlayFlag;
	__u8 u8NoDelayFlag;
};

struct meta_srccap_hdr_emp_pkt {
	__u32 u32Version;
	__u8 bValid;
	__u8 bSync;
	__u8 bVFR;
	__u8 bAFR;
	__u8 u8DSType;
	__u8 u8OrganizationID;
	__u16 u16DataSetTag;
	__u16 u16DataSetLength;
	__u8 u8ItuTT35CountryCode; // 0xB5 as HDR10+ SEI defined
	__u16 u16ItuTT35TerminalProviderCode; //0x003C as HDR10+ SEI defined
	__u16 u16ItuTT35TerminalProviderOrientedCode; //0x0001 as HDR10+ SEI defined
	__u8 u8ApplicationIdentifier; // 4 as HDR10+ SEI defined
	__u8 u8ApplicationVersion; // 1 as HDR10+ SEI defined
	__u8 u8NumWindows;// 1 as HDR10+ SEI defined
	__u8 u8TargetedSystemDisplayMaximumLuminance;
	__u8 bTargetSystemDisplayActualPeakLuminanceFlag; //0 as HDR10+ SEI defined
	__u32 au32Maxscl[3];
	__u32 au32Average_maxrgb[3];
	__u8 u8NumDistributions; // 9 as HDR10+ SEI defined
	__u8 au8DistributionValues[9];
	__u8 u8FractionBrightPixels;
	__u8 bMasteringDisplayActualPeakLuminanceFlag;
	__u8 bToneMappingFlag;
	__u8 u8KneePointX;
	__u8 u8KneePointY;
	__u8 u8NumBezierCurveAnchors;
	__u8 au8BezierCurveAnchors[9];
	__u8 u8ColorSaturationMappingFlag; //0 as HDR10+ SEI defined
};

struct meta_pq_pqparam {
	__u8  data[1024*3];
};

struct m_pq_cfd_test_rulelist_info {
    __u8 transfer_characteristics;
    __u8 tmo_range;
    __u8 tmo_mode;
    __u8 tmo_manual_mode;
    __u8 xvvcc_manual_mode;
    __u8 gamut_mapping_mode;
    __u8 hdr_mode;
};

//-----------------------------------------------------------------------------
//  Function and Variable
//-----------------------------------------------------------------------------

#endif
