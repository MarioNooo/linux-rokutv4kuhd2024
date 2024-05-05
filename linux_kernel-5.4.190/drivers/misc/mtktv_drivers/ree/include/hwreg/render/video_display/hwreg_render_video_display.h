////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HWREG_RENDER_VIDEO_DISPLAY_H_
#define _HWREG_RENDER_VIDEO_DISPLAY_H_

#include "hwreg_common.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define fourcc_code(a, b, c, d) ((__u32)(a) | ((__u32)(b) << 8) | \
				 ((__u32)(c) << 16) | ((__u32)(d) << 24))

/* 16 bpp RGB */
#define DRM_HWREG_FORMAT_RGB565	fourcc_code('R', 'G', '1', '6') /* [15:0] R:G:B 5:6:5 little endian */
/* 32 bpp RGB */
#define DRM_HWREG_FORMAT_ARGB8888	fourcc_code('A', 'R', '2', '4') /* [31:0] A:R:G:B 8:8:8:8 little endian */
#define DRM_HWREG_FORMAT_ABGR8888	fourcc_code('A', 'B', '2', '4') /* [31:0] A:B:G:R 8:8:8:8 little endian */
#define DRM_HWREG_FORMAT_RGBA8888	fourcc_code('R', 'A', '2', '4') /* [31:0] R:G:B:A 8:8:8:8 little endian */
#define DRM_HWREG_FORMAT_BGRA8888	fourcc_code('B', 'A', '2', '4') /* [31:0] B:G:R:A 8:8:8:8 little endian */
#define DRM_HWREG_FORMAT_ARGB2101010	fourcc_code('A', 'R', '3', '0') /* [31:0] A:R:G:B 2:10:10:10 little endian */
#define DRM_HWREG_FORMAT_ABGR2101010	fourcc_code('A', 'B', '3', '0') /* [31:0] A:B:G:R 2:10:10:10 little endian */
#define DRM_HWREG_FORMAT_RGBA1010102	fourcc_code('R', 'A', '3', '0') /* [31:0] R:G:B:A 10:10:10:2 little endian */
#define DRM_HWREG_FORMAT_BGRA1010102	fourcc_code('B', 'A', '3', '0') /* [31:0] B:G:R:A 10:10:10:2 little endian */
/* packed YCbCr */
#define DRM_HWREG_FORMAT_YVYU		fourcc_code('Y', 'V', 'Y', 'U') /* [31:0] Cb0:Y1:Cr0:Y0 8:8:8:8 little endian */
/*
 * 2 plane YCbCr
 * index 0 = Y plane, [7:0] Y
 * index 1 = Cr:Cb plane, [15:0] Cr:Cb little endian
 * or
 * index 1 = Cb:Cr plane, [15:0] Cb:Cr little endian
 */
#define DRM_HWREG_FORMAT_NV21		fourcc_code('N', 'V', '2', '1') /* 2x2 subsampled Cb:Cr plane */
#define DRM_HWREG_FORMAT_NV61		fourcc_code('N', 'V', '6', '1') /* 2x1 subsampled Cb:Cr plane */
/*
 * 3 plane YCbCr
 * index 0: Y plane, [7:0] Y
 * index 1: Cb plane, [7:0] Cb
 * index 2: Cr plane, [7:0] Cr
 * or
 * index 1: Cr plane, [7:0] Cr
 * index 2: Cb plane, [7:0] Cb
 */
#define DRM_HWREG_FORMAT_YUV444	fourcc_code('Y', 'U', '2', '4') /* non-subsampled Cb (1) and Cr (2) planes */

/*
 * MTK defined modifier
 *
 * BIT 0 ~ 7	: Used to define extended format arrangement
 * BIT 8	: The format is compressed
 * BIT 9	: The format is modified to 6 bit per pixel
 * BIT 10	: The format is modified to 10 bit per pixel
 */
#define DRM_HWREG_FORMAT_MOD_VENDOR_MTK	(0x09)

/* extended arrangment */
#define DRM_HWREG_FORMAT_MOD_MTK_YUV444_VYU	0x01//fourcc_mod_code(MTK, 0x01)

#define MTK_PLANE_DISPLAY_PIPE (3)

enum drm_hwreg_video_plane_type {
	VPLANE_TYPE_NONE = 0,
	VPLANE_TYPE_MEMC,
	VPLANE_TYPE_MULTI_WIN,
	VPLANE_TYPE_SINGLE_WIN1,
	VPLANE_TYPE_SINGLE_WIN2,
	VPLANE_TYPE_MAX,
};

enum drm_hwreg_dummy_type {
	DUMMY_TYPE_PQU_METADATA_INDEX,
	DUMMY_TYPE_MAX
};

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

struct ARGB {
	uint64_t alpha;
	uint64_t R;
	uint64_t G;
	uint64_t B;
};

struct window {
	uint64_t x;
	uint64_t y;
	uint64_t w;
	uint64_t h;
};

struct windowRect {
	uint64_t h_start;
	uint64_t h_end;
	uint64_t v_start;
	uint64_t v_end;
};

struct baseAddr {
	uint32_t plane0;
	uint32_t plane1;
	uint32_t plane2;
};

struct hwregCropWinIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	uint8_t MGWPlaneNum;
	struct window cropWindow;
};

struct hwregDispWinIn{
	bool RIU;
	uint16_t windowID;
	struct windowRect dispWindowRect;
};

struct hwregPreInsertEnableIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	bool enable;
};

struct hwregPreInsertImageSizeIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	struct windowRect imageSizeRect;
};

struct hwregPreInsertFrameSizeIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t htt;
	uint16_t vtt;
};

struct hwregPostFillEnableIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	bool enable;
};

struct hwregPostFillWinEnableIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	bool enable;
};

struct hwregPostFillFrameSizeIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t htt;
	uint16_t vtt;
};

struct hwregPostFillWindowARGBIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	struct ARGB windowARGB;
};

struct hwregPostFillBgARGBIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	struct ARGB bgARGB;
};

struct hwregPostFillMuteScreenIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	bool muteEn;
	bool muteSWModeEn;
};

struct hwregPostFillSnowScreenIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	bool snowScreenEnable;
};

struct hwregLayerControlIn{
	bool RIU;
	bool layerEn;
	uint8_t srcIndex;
	uint8_t layer;
};

struct hwregVop2memcMuxSelectIn{
	bool RIU;
	bool muxSel;
};

struct hwregThinReadFactorIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint8_t MGWPlaneNum;
	uint8_t thinReadFactor;
};

struct hwregMGWDMAEnIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	bool enable;
};

struct hwregExtWinEnIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	bool enable;
};

struct hwregExtWinAutoEnIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	bool enable;
};

struct hwregMGWDMAWinEnIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	bool enable;
};

struct hwregFrameSizeIn{
	bool RIU;
	uint16_t hSize;
	uint16_t vSize;
};

struct hwregBaseAddrIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	uint8_t MGWPlaneNum;
	struct baseAddr addr;
};

struct hwregFrameNumIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	uint8_t MGWPlaneNum;
	uint32_t frameNum;
};

struct hwregMemConfigIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	uint8_t MGWPlaneNum;
	uint32_t fourcc;
	uint8_t modifier;
	uint8_t modifier_arrange;
	uint8_t modifier_compressed;
	uint8_t modifier_6bit;
	uint8_t modifier_10bit;
};

struct hwregLineOffsetIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	uint8_t MGWPlaneNum;
	uint16_t lineOffset;
};

struct hwregFrameOffsetIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	uint8_t MGWPlaneNum;
	uint32_t frameOffset[MTK_PLANE_DISPLAY_PIPE];
};

struct hwregFreezeIn{
	bool RIU;
	uint16_t windowID;
	bool enable;
};

struct hwregScalingIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	uint8_t MGWPlaneNum;
	uint8_t Hscale_enable;
	uint8_t H_shift_mode;
	uint8_t IsScalingUp_H;
	uint32_t HorIniFactor;
	uint32_t HorRatio;
	uint64_t HInputsize;
	uint64_t HOutputsize;
	uint8_t Vscale_enable;
	uint8_t V_shift_mode;
	uint8_t IsScalingUp_V;
	uint32_t VerIniFactor;
	uint32_t VerRatio;
	uint64_t VInputsize;
	uint64_t VOutputsize;
	uint8_t HVSP_bypass;
};

struct hwregRwDiffIn{
	bool RIU;
	uint16_t windowID;
	uint8_t rwDiff;
};

struct hwregProtectValIn{
	bool RIU;
	uint8_t protectVal;
};

struct hwregBlendOutPatternIn{
	bool RIU;
	bool blackEn;
	bool snowEn;
};

struct hwregBlendOutSizeIn{
	bool RIU;
	uint16_t Hsize;
};

struct hwregSetWindowPQIn{
	uint16_t windowID;
	char* windowPQ;
};

struct hwregAidTableIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	uint8_t access_id;
};

struct hwregDummyValueIn{
	bool RIU;
	enum drm_hwreg_dummy_type dummy_type;
	uint16_t dummy_value;
};

struct hwregLinebuffersizeIn{
	bool RIU;
	uint8_t MGWPlaneNum;
	enum drm_hwreg_video_plane_type planeType;
	bool enable;
	uint64_t Linebuffersize;
};

struct hwregMemSWmodeIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	uint16_t windowID;
	bool bSWmode;//0:HW mode, 1:SW mode
};

struct hwregAutoNoSignalIn{
	bool RIU;
	enum drm_hwreg_video_plane_type planeType;
	bool bEnable;
};
//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

int drv_hwreg_render_display_set_cropWindow(
	struct hwregCropWinIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_dispWindow(
	struct hwregDispWinIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_preInsertEnable(
	struct hwregPreInsertEnableIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_preInsertImageSize(
	struct hwregPreInsertImageSizeIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_preInsertFrameSize(
	struct hwregPreInsertFrameSizeIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_postFillEnable(
	struct hwregPostFillEnableIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_postFillWinEnable(
	struct hwregPostFillWinEnableIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_postFillFrameSize(
	struct hwregPostFillFrameSizeIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_postFillWindowAlpha(
	struct hwregPostFillWindowARGBIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_postFillWindowRGB(
	struct hwregPostFillWindowARGBIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_postFillBgAlpha(
	struct hwregPostFillBgARGBIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_postFillBgRGB(
	struct hwregPostFillBgARGBIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_postFillMuteScreen(
	struct hwregPostFillMuteScreenIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_postFillSnowScreen(
	struct hwregPostFillSnowScreenIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_layerControl(
	struct hwregLayerControlIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_vop2memcMuxSelect(
	struct hwregVop2memcMuxSelectIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_thinReadFactor(
	struct hwregThinReadFactorIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_mgwdmaEnable(
	struct hwregMGWDMAEnIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_extWinEnable(
	struct hwregExtWinEnIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_extwin_auto_en(
	struct hwregExtWinAutoEnIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_mgwdmaWinEnable(
	struct hwregMGWDMAWinEnIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_frameSize(
	struct hwregFrameSizeIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_baseAddr(
	struct hwregBaseAddrIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_frameNum(
	struct hwregFrameNumIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_memConfig(
	struct hwregMemConfigIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_lineOffset(
	struct hwregLineOffsetIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_frameOffset(
	struct hwregFrameOffsetIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_freeze(
	struct hwregFreezeIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_scalingratio(
	struct hwregScalingIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_rwDiff(
	struct hwregRwDiffIn paramIn,
        struct hwregOut *paramOut);

int drv_hwreg_render_display_get_rwDiff(
	uint16_t windowID,
	uint16_t *value);

int drv_hwreg_render_display_set_rbankRefProtect(
	struct hwregProtectValIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_blendOut_pattern(
	struct hwregBlendOutPatternIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_blendOut_size(
	struct hwregBlendOutSizeIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_window_pq(
	struct hwregSetWindowPQIn paramIn);

int drv_hwreg_render_display_set_aidtable(
	struct hwregAidTableIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_dummy_value(
	struct hwregDummyValueIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_get_dummy_value(
	enum drm_hwreg_dummy_type dummy_type,
	uint16_t *value);

int drv_hwreg_render_display_set_linebuffersize(
	struct hwregLinebuffersizeIn paramIn,
	struct hwregOut *paramOut);

void drv_hwreg_render_display_set_mgw_version(uint32_t mgw_version);
void drv_hwreg_render_display_set_vb_version(uint32_t vb_version);

int drv_hwreg_render_display_set_mem_sw_mode(
	struct hwregMemSWmodeIn paramIn,
	struct hwregOut *paramOut);

int drv_hwreg_render_display_set_auto_no_signal(
	struct hwregAutoNoSignalIn paramIn,
	struct hwregOut *paramOut);

#endif // _HWREG_RENDER_VIDEO_DISPLAY_H_
