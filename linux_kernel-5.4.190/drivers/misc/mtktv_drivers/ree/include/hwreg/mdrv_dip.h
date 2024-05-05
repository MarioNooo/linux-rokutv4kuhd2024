/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef MDRV_DIP_H
#define MDRV_DIP_H

#include "mdrv_types_internal.h"

#ifndef INTERFACE
#define INTERFACE
#endif

#define fmt_fourcc(a, b, c, d)\
	((MS_U32)(a) | ((MS_U32)(b) << 8) | ((MS_U32)(c) << 16) | ((MS_U32)(d) << 24))

#define DIP_PIX_FMT_MT_YUV420_32_32   fmt_fourcc('M', 'T', '3', '3') /* 12  Y/CbCr 4:2:0 32x32 */
#define DIP_PIX_FMT_MT_YUV420_32_16   fmt_fourcc('M', 'T', '3', '1') /* 12  Y/CbCr 4:2:0 32x16 */
#define DIP_PIX_FMT_MT_YUV420_16_32   fmt_fourcc('M', 'T', '1', '3') /* 12  Y/CbCr 4:2:0 16x32 */
#define DIP_PIX_FMT_MT_MFDEC          fmt_fourcc('M', 'M', 'F', 'D') /* Mediatek compressed MFDEC */

#define DIP_PIX_FMT_ABGR32  fmt_fourcc('A', 'R', '2', '4') /* 32, LSB->B(8)->G(8)->R(8)->A(8)->MSB */
#define DIP_PIX_FMT_DIP_ABGR32_SWAP  fmt_fourcc('A', 'R', '4', '2') /* 32bit, LSB->R(8)->G(8)->B(8)->A(8)->MSB */
#define DIP_PIX_FMT_ARGB32  fmt_fourcc('B', 'A', '2', '4') /* 32, LSB->A(8)->R(8)->G(8)->B(8)->MSB */
#define DIP_PIX_FMT_DIP_ARGB32_SWAP  fmt_fourcc('B', 'A', '4', '2') /* 32bit, LSB->A(8)->B(8)->G(8)->R(8)->MSB */
#define DIP_PIX_FMT_DIP_ARGB2101010  fmt_fourcc('M', 'R', '2', '1') /* 32bit, MSB->A(2)->R(10)->G(10)->B(10)->LSB */
#define DIP_PIX_FMT_BGR24   fmt_fourcc('B', 'G', 'R', '3') /* 24, LSB->B(8)->G(8)->R(8)->MSB */
#define DIP_PIX_FMT_RGB565X fmt_fourcc('R', 'G', 'B', 'R') /* 16  LSB->R(5)-G(6)-B(5)->MSB */
#define DIP_PIX_FMT_YUYV    fmt_fourcc('Y', 'U', 'Y', 'V') /* 16  YUV 4:2:2, LSB->Y->U->Y->V->MSB */
#define DIP_PIX_FMT_YVYU    fmt_fourcc('Y', 'V', 'Y', 'U') /* 16  YUV 4:2:2, LSB->Y->V->Y->U->MSB */
#define DIP_PIX_FMT_UYVY    fmt_fourcc('U', 'Y', 'V', 'Y') /* 16  YUV 4:2:2, LSB->U->Y->V->Y->MSB */
#define DIP_PIX_FMT_VYUY    fmt_fourcc('V', 'Y', 'U', 'Y') /* 16  YUV 4:2:2, LSB->V->Y->U->Y->MSB */
#define DIP_PIX_FMT_DIP_YUYV_10BIT    fmt_fourcc('M', '1', 'Y', 'U') /* YUV422 10bit, LSB->Y->U->Y->V->MSB */
#define DIP_PIX_FMT_DIP_YVYU_10BIT    fmt_fourcc('M', '1', 'Y', 'V') /* YVU422 10bit, LSB->Y->V->Y->U->MSB */
#define DIP_PIX_FMT_DIP_UYVY_10BIT    fmt_fourcc('M', '1', 'U', 'Y') /* YUV422 10bit, LSB->U->Y->V->Y->MSB */
#define DIP_PIX_FMT_DIP_VYUY_10BIT    fmt_fourcc('M', '1', 'V', 'Y') /* YUV422 10bit, LSB->V->Y->U->Y->MSB */
#define DIP_PIX_FMT_NV16    fmt_fourcc('N', 'V', '1', '6') /* 16  Y/CbCr 4:2:2, 2 plane, LSB->U->V->MSB */
#define DIP_PIX_FMT_NV61    fmt_fourcc('N', 'V', '6', '1') /* 16  Y/CrCb 4:2:2, 2 plane, LSB->V->U->MSB */
#define DIP_PIX_FMT_DIP_YUV422_8B_BLK_TILE   fmt_fourcc('M', '8', 'B', 'T') /* YC422 8b  BLK_TILE, LSB->U->Y->V->Y->MSB */
#define DIP_PIX_FMT_DIP_YUV422_10B_BLK_TILE  fmt_fourcc('M', 'T', 'B', 'T') /* YC422 10b BLK_TILE, LSB->U->Y->V->Y->MSB */
#define DIP_PIX_FMT_NV12    fmt_fourcc('N', 'V', '1', '2') /* 12  Y/CbCr 4:2:0, 2 plane, LSB->U->V->MSB */
#define DIP_PIX_FMT_NV21    fmt_fourcc('N', 'V', '2', '1') /* 12  Y/CrCb 4:2:0, 2 plane, LSB->V->U->MSB */
#define DIP_PIX_FMT_DIP_MT21S        fmt_fourcc('M', '2', '1', 'S')   /* MTK 8-bit block mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21S10T     fmt_fourcc('M', 'T', 'S', 'T')   /* MTK 10-bit tile block mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21S10R     fmt_fourcc('M', 'T', 'S', 'R')   /* MTK 10-bit raster block mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21S10L     fmt_fourcc('M', 'T', 'S', 'L')   /* MTK 10-bit linear mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21CS       fmt_fourcc('M', '2', 'C', 'S')   /* MTK 8-bit compressed block mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21CSA      fmt_fourcc('M', 'A', 'C', 'S')   /* MTK 8-bit compressed block au offset mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21CS10T    fmt_fourcc('M', 'C', 'S', 'T')   /* MTK 10-bit tile compressed block mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21CS10R    fmt_fourcc('M', 'C', 'S', 'R')   /* MTK 10-bit raster compressed block mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21S10TJ    fmt_fourcc('M', 'J', 'S', 'T')   /* MTK 10-bit tile block jump mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21S10RJ    fmt_fourcc('M', 'J', 'S', 'R')   /* MTK 10-bit raster block jump mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21CS10TJ   fmt_fourcc('J', 'C', 'S', 'T')   /* MTK 10-bit tile compressed block jump mode, two contiguous planes */
#define DIP_PIX_FMT_DIP_MT21CS10RJ   fmt_fourcc('J', 'C', 'S', 'R')   /* MTK 10-bit raster compressed block jump mode, two cont. planes */
#define DIP_PIX_FMT_MT21C            fmt_fourcc('M', 'T', '2', '1')   /* Mediatek compressed block mode  */
#define DIP_PIX_FMT_DIP_VSD8X4       fmt_fourcc('M', 'V', '8', '4')   /* MTK VSD8x4, two contiguous planes */
#define DIP_PIX_FMT_DIP_VSD8X2       fmt_fourcc('M', 'V', '8', '2')   /* MTK VSD8x2, two contiguous planes */

/*
|-----------|-----------|-----------|
|           |    dts    |   hwreg   |
| bank name | reg index | reg index |
|-----------|-----------|-----------|
|  DIPW(0)  |     0     |     0     |
|-----------|-----------|-----------|
|  DIPW(1)  |     0     |     1     |
|-----------|-----------|-----------|
|  DIPW(2)  |     0     |     2     |
|-----------|-----------|-----------|
|   DIPR    |     1     |     3     |
|-----------|-----------|-----------|
|  DIP_TOP  |     2     |     4     |
|-----------|-----------|-----------|
|   DDI1    |     3     |     5     |
|-----------|-----------|-----------|
|   DDNR    |     4     |     6     |
|-----------|-----------|-----------|
|   HVSP    |     5     |     7     |
|-----------|-----------|-----------|
|   UFO     |     6     |     8     |
|-----------|-----------|-----------|
|   B2R     |     7     |     9     |
|-----------|-----------|-----------|
|   AID     |     8     |    10     |
|-----------|-----------|-----------|
*/

typedef enum {
	E_HWREG_DIP0 = 0,
	E_HWREG_DIP1,
	E_HWREG_DIP2,
	E_HWREG_DIPR,
	E_HWREG_DIPTOP,
	E_HWREG_DDI1,
	E_HWREG_DDNR,
	E_HWREG_HVSP,
	E_HWREG_UFO,
	E_HWREG_B2R,
	E_HWREG_AID,
	E_HWREG_MAX
} EN_HWREG_IDX;

typedef enum {
	E_DTS_DIP0 = 0,
	E_DTS_DIP1 = 0,
	E_DTS_DIP2 = 0,
	E_DTS_DIPR,
	E_DTS_DIPTOP,
	E_DTS_DDI1,
	E_DTS_DDNR,
	E_DTS_HVSP,
	E_DTS_UFO,
	E_DTS_B2R,
	E_DTS_AID,
	E_DTS_MAX
} EN_DTS_IDX;

/// Define window type for DIP
typedef enum {
	E_DIP_IDX0 = 0,         ///< DIP window
	E_DIP_IDX1 = 1,
	E_DIP_IDX2 = 2,
	E_DIP_IDX_MAX          /// The max support window
} EN_DIP_IDX;

typedef enum {
	E_TILE_FMT_16_32     = 0x00,
	E_TILE_FMT_32_16     = 0x01,
	E_TILE_FMT_32_32     = 0x02,
} EN_TILE_FMT;

/// Define source type for DIP
typedef enum {
	E_DIP_SRC_FROM_SRCCAP_MAIN = 0,
	E_DIP_SRC_FROM_SRCCAP_SUB,
	E_DIP_SRC_FROM_PQ_START,
	E_DIP_SRC_FROM_PQ_HDR,
	E_DIP_SRC_FROM_PQ_SHARPNESS,
	E_DIP_SRC_FROM_PQ_SCALING,
	E_DIP_SRC_FROM_PQ_CHROMA_COMPENSAT,
	E_DIP_SRC_FROM_PQ_BOOST,
	E_DIP_SRC_FROM_PQ_COLOR_MANAGER,
	E_DIP_SRC_FROM_STREAM_ALL_VIDEO,
	E_DIP_SRC_FROM_STREAM_ALL_VIDEO_OSDB,
	E_DIP_SRC_FROM_STREAM_POST,
	E_DIP_SRC_FROM_OSD,
	E_DIP_SRC_FROM_DIPR,
	E_DIP_SRC_FROM_B2R,
	E_DIP_SRC_FROM_MAX          //< capture point max >
} EN_DIP_SRC_FROM;

typedef enum {
	E_SRC_COLOR_RGB = 0,
	E_SRC_COLOR_YUV444,
	E_SRC_COLOR_YUV422,
	E_SRC_COLOR_YUV420,
	E_SRC_COLOR_MAX,
} EN_DIP_SRC_COLOR;

typedef enum
{
	E_DIP_ROT_0 =0,
	E_DIP_ROT_90,
	E_DIP_ROT_270,
} EN_DIP_ROTATION;

typedef enum {
	E_DIP_NORMAL =0,
	E_DIP_DI,
	E_DIP_DNR,
} EN_DIP_OPMODE;

typedef enum {
	E_B2R_FIELD_PRO =0,
	E_B2R_FIELD_TOP,
	E_B2R_FIELD_BOTTOM,
} EN_B2R_FIELD;

typedef enum {
	E_UDMA_DIPW0 = 0,
	E_UDMA_DIPW1,
	E_UDMA_DIPW2,
	E_UDMA_DIPR,
	E_UDMA_DDIW,
	E_UDMA_DDIR,
	E_UDMA_MAX
} EN_UDMA_CLT;

typedef enum {
	E_DIPCSC_ENC_BT709 = 0,
	E_DIPCSC_ENC_BT2020,
	E_DIPCSC_ENC_BT601,
	E_DIPCSC_ENC_MAX,
} EN_DIP_CSC_ENC;

typedef enum {
	E_DIPCSC_RG_F2F = 0,
	E_DIPCSC_RG_F2L,
	E_DIPCSC_RG_L2F,
	E_DIPCSC_RG_L2L,
	E_DIPCSC_RG_MAX,
} EN_DIP_CSC_RANGE;

typedef enum {
	E_AID_NS = 0,
	E_AID_SDC,
	E_AID_S,
	E_AID_CSP,
	E_AID_MAX,
} EN_AID;

typedef enum {
	E_HW_DIPR = 0,
	E_HW_B2R,
	E_HW_DIPW,
	E_HW_MAX,
} EN_HW_TYPE;

struct ST_DIP_MFDEC {
	MS_U16 u16Ena;
	MS_U16 u16Sel;
	MS_U64 u64BitlenBase;
	MS_U64 u64LumaBase;
	MS_U64 u64ChromaBase;
	MS_U16 u16FBPitch;
	MS_U16 u16BitlenPitch;
	MS_U16 u16StartX;
	MS_U16 u16StartY;
	MS_U16 u16HSize;
	MS_U16 u16VSize;
	MS_U16 u16VP9Mode;
	MS_U16 u16HMir;
	MS_U16 u16VMir;
	EN_TILE_FMT enTileFmt;
	MS_U16 u16Bypass;
	MS_U16 u16SimMode;
};

struct ST_CROP_WIN {
	MS_U32 u32XStart;
	MS_U32 u32YStart;
	MS_U32 u32CropWidth;
	MS_U32 u32CropHeight;
};

struct ST_SCALING {
	MS_U32 u32InputWidth;
	MS_U32 u32InputHeight;
	MS_U32 u32OutputWidth;
	MS_U32 u32OutputHeight;
	MS_U8 u8HScalingEna;
	MS_U8 u8VScalingEna;
};

struct ST_RES_WIN {
	MS_U32 u32Width;
	MS_U32 u32Height;
	MS_U32 u32Pitch;
};

struct ST_DIPR_SRC_WIN {
	MS_U16 u16Width;
	MS_U16 u16Height;
	MS_U16 u16Pitch;
	MS_U32 u32fourcc;
};

struct ST_B2R_TIMING{
	uint16_t V_TotalCount;
	uint16_t H_TotalCount;
	uint16_t VBlank0_Start;
	uint16_t VBlank0_End;
	uint16_t VBlank1_Start;
	uint16_t VBlank1_End;
	uint16_t TopField_Start;
	uint16_t BottomField_Start;
	uint16_t TopField_VS;
	uint16_t BottomField_VS;
	uint16_t HActive_Start;
	uint16_t HImg_Start;
	uint16_t VImg_Start0;
	uint16_t VImg_Start1;
};

struct ST_B2R_ADDR {
	MS_U64 u64LumaFb;
	MS_U64 u64ChromaFb;
	MS_U64 u64LumaBlnFb;
	MS_U64 u64ChromaBlnFb;
	MS_U8 u8Addrshift;
};

struct ST_SRC_INFO {
	MS_U8 u8PixelNum;
	EN_DIP_SRC_COLOR enSrcClr;
	MS_U32 u32Width;
	MS_U32 u32Height;
	MS_U32 u32DisplayWidth;
	MS_U32 u32DisplayHeight;
};

struct ST_RWSEP_INFO {
	MS_BOOL bEable;
	MS_BOOL bHVSPScalingDown;
	MS_U16 u16Times;
};

struct ST_OPMODE {
	EN_DIP_OPMODE enOpMode;
	MS_U64 u64Addr;
	MS_U16 u16Width;
	MS_U16 u16Height;
};

struct ST_UDMA_LV {
	MS_U8 u8PreultraLv;
	MS_U8 u8PreultraMask;
	MS_U8 u8UltraLv;
	MS_U8 u8UltraMask;
	MS_U8 u8UrgentLv;
	MS_U8 u8UrgentMask;
};

struct ST_MUX_INFO {
	EN_DIP_SRC_FROM enSrcFrom;
	MS_U32 u32DipPixelNum;
	MS_U32 u32FrontPack;
};

struct ST_CSC {
	MS_BOOL bEnaR2Y;
	MS_BOOL bEnaY2R;
	EN_DIP_CSC_ENC enCscEnc;
	EN_DIP_CSC_RANGE enCscRg;
};

INTERFACE MS_BOOL KDrv_DIP_SetIOBase(MS_U8 u8Idx, MS_U32 u32P_Addr, MS_U64 u32P_Size, MS_U64 u64V_Addr);
INTERFACE MS_BOOL KDrv_DIP_CheckIOBaseInit(MS_U8 u8Idx);
INTERFACE MS_BOOL KDrv_DIP_B2R_Init(void);
INTERFACE MS_BOOL KDrv_DIP_B2R_SetFmt(MS_U32 u32fourcc);
INTERFACE MS_BOOL KDrv_DIP_B2R_SetSize(MS_U16 u16Width, MS_U16 u16Height, MS_U16 u16Pitch);
INTERFACE MS_BOOL KDrv_DIP_B2R_SetTiming(struct ST_B2R_TIMING *pstTiming);
INTERFACE MS_BOOL KDrv_DIP_B2R_SetAddr(struct ST_B2R_ADDR *pstB2rAddr);
INTERFACE MS_BOOL KDrv_DIP_B2R_SetFiled(EN_B2R_FIELD enB2RField);
INTERFACE MS_BOOL KDrv_DIP_B2R_Ena(MS_U8 u8Enable);
INTERFACE MS_BOOL KDrv_DIP_Init(EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetMux(struct ST_MUX_INFO *pstMuxInfo, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_Reset(EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_DIPR_SetSrcWin(struct ST_DIPR_SRC_WIN *pstDiprSrcWin, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetMFDEC(struct ST_DIP_MFDEC *pstMfdec, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetSrcAddr(MS_U32 u32NPlanes, MS_U64 *pu64PhyAddr, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetDstAddr(MS_U32 u32NPlanes, MS_U64 *pu64PhyAddr, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetAlpha(MS_U32 u32alpha, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetMirror(MS_U32 u32HMirror, MS_U32 u32VMirror, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetRotation(EN_DIP_ROTATION enRotation, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetSourceInfo(struct ST_SRC_INFO *pstSrcInfo, EN_DIP_SRC_FROM enSrcFrom, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetWinId(MS_U8 u8Enable, MS_U16 u16WinId, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetCrop(struct ST_CROP_WIN *pstCropWin, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetScaling(struct ST_SCALING *pstScaling, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetHVSP(struct ST_SCALING *pstScaling, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetWriteProperty(struct ST_RES_WIN *pstResWin, MS_U32 u32fourcc, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetRWSeparate(struct ST_RWSEP_INFO *pstRwSep, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetOpMode(struct ST_OPMODE *pstOpMode, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_Ena(MS_U8 u8Enable, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_Cont_Ena(MS_U8 u8Enable, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_FrameCnt(MS_U8 u8FrmCnt, MS_U32 u32frmPixelcnt, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_Trigger(EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_DIPR_Trigger(EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_GetIrqStatus(MS_U16 *pu16IrqSts, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_ClearIrq(MS_U16 u16IrqSts, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_FrameRateCtrl(MS_BOOL bEnable, MS_U16 u16In, MS_U16 u16Out, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_EnableCRC(MS_U8 u8Enable, MS_U8 u8FrmCnt, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_IsEnableCRC(MS_U8 *pu8Enable, MS_U8 *pu8FrmCnt, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_GetCrcValue(MS_U32 *pu32Val, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetSrcHblanking(MS_U32 u32Hblanking, EN_DIP_SRC_FROM enSrcFrom);
INTERFACE MS_BOOL KDrv_DIP_SetUDMALevel(struct ST_UDMA_LV *pstUdmaLv, EN_UDMA_CLT enUdma);
INTERFACE MS_BOOL KDrv_DIP_SetCSC(struct ST_CSC *pstCSC, EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_ShowDevStatus(EN_DIP_IDX enDIPIdx);
INTERFACE MS_BOOL KDrv_DIP_SetAID(EN_AID enAid, EN_HW_TYPE HWType, EN_DIP_IDX enDIPIdx);

#endif //MDRV_DIP_H
