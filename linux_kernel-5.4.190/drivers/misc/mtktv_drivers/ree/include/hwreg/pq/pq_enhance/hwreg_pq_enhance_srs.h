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

////////////////////////////////////////////////////////////////////////////////
///
/// @author Mediatek Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HWREG_PQ_ENHANCE_SRS_H_
#define _HWREG_PQ_ENHANCE_SRS_H_

#include "hwreg_common.h"

//------------------------------------------------------------------------------
//  Driver Capability
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Macro and Define
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Type and Structure
//------------------------------------------------------------------------------
struct srsTimingGenIn {
	uint16_t hfde_st;
	uint16_t hfde_end;
	uint16_t htotal;
	uint16_t vfde_st;
	uint16_t vfde_end;
	uint16_t vtotal;
};

struct srsXIU2AHBSelIn {
	uint8_t xiu2ahb_sel0;
	uint8_t xiu2ahb_sel1;
};

enum srs_reset_signal {
	SRS_RESET_VIDEO,
	SRS_RESET_AHB,
	SRS_RESET_BOTH,
};

struct srsCmdsIn {
	struct reg_info_ex *cmds;
	uint32_t cmdNum;
};

struct srsDatebaseIn {
	uint64_t srcAddr;
	uint64_t dstAddr;
	uint32_t dataSize;
};

enum srs_pattern_index {
	SRS_PATTERN_PRE,
	SRS_PATTERN_POST,
	SRS_PATTERN_MAX,
};

enum srs_pattern_mode {
	SRS_PATTERN_MODE_HCNT,
	SRS_PATTERN_MODE_VCNT,
	SRS_PATTERN_MODE_FIX,
};

struct srsPatternGenIn {
	enum srs_pattern_index pat_idx;
	bool pat_en;
	enum srs_pattern_mode pat_sel_y;
	enum srs_pattern_mode pat_sel_u;
	enum srs_pattern_mode pat_sel_v;
	uint16_t pat_fix_val_y;
	uint16_t pat_fix_val_u;
	uint16_t pat_fix_val_v;
};

struct srsSizeReporterOut {
	uint16_t input_hcnt;
	uint16_t input_vcnt;
	uint16_t output_hcnt;
	uint16_t output_vcnt;
};

//------------------------------------------------------------------------------
//  Function and Variable
//------------------------------------------------------------------------------
bool drv_hwreg_pq_enhance_srs_hw_support(void);
int drv_hwreg_pq_enhance_srs_enable(
	bool srsEnable, struct hwregWriteMethodInfo *writeInfo);
int drv_hwreg_pq_enhance_srs_timing_gen_setting(
	struct srsTimingGenIn paramIn,
	struct hwregWriteMethodInfo *writeInfo);
int drv_hwreg_pq_enhance_srs_xiu2ahb_sel(
	struct srsXIU2AHBSelIn paramIn,
	struct hwregWriteMethodInfo *writeInfo);
int drv_hwreg_pq_enhance_srs_reset(
	enum srs_reset_signal eRstSignal,
	struct hwregWriteMethodInfo *writeInfo);
int drv_hwreg_pq_enhance_srs_update_cmds(
	struct srsCmdsIn paramIn,
	struct hwregWriteMethodInfo *writeInfo);
int drv_hwreg_pq_enhance_srs_update_database(
	struct srsDatebaseIn paramIn,
	struct hwregWriteMethodInfo *writeInfo);
bool drv_hwreg_pq_enhance_srs_get_bdma_status(void);
int drv_hwreg_pq_enhance_srs_pattern_gen(
	struct srsPatternGenIn paramIn,
	struct hwregWriteMethodInfo *writeInfo);
int drv_hwreg_pq_enhance_srs_size_reporter(
	struct srsSizeReporterOut *paramOut);
int drv_hwreg_pq_enhance_srs_get_cmds(
	struct srsCmdsIn * cmdIn);
int drv_hwreg_pq_enhance_srs_get_bdma_config(
	uint8_t channel, struct hwregOut *hwOut);
uint32_t drv_hwreg_pq_enhance_srs_read4byte(uint32_t u32P_Addr);
#endif // _HWREG_PQ_ENHANCE_SRS_H_
