/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef __MTK_DRAMC_H
#define __MTK_DRAMC_H

//####################################
// definition for Register opteration
//####################################
/* Register opteration */
#define READ_REG(offset)   readl_relaxed(base + (offset))
#define READ_REG_16(offset)   readw_relaxed(base + (offset))
#define WRITE_REG(val, offset)   writel_relaxed((val), base + (offset))

//####################################
// definition for EMI
//####################################
/* EMI DISPATCH Register offset */
#define BROC_MODE0   (0x100)
#define BROC_MODE1   (0x104)
/* EMI DISPATCH Register value */
#define DISP_1_EMI_BORC_MODE0   (0x7f7f)
#define DISP_1_EMI_BORC_MODE1   (0x0000)
#define DISP_2_EMI_BORC_MODE0   (0x7f7f)
#define DISP_2_EMI_BORC_MODE1   (0x0002)
#define DISP_3_EMI_BORC_MODE0   (0x7f7f)
#define DISP_3_EMI_BORC_MODE1   (0x7f02)

//DUMMY REG
#define MS_BUS_RIU_ADDR                          0x1C000000
#define DummyRG_BASE_ADDRESS    0x606A00

#define DM_RG_ASYMBOUND					(DummyRG_BASE_ADDRESS + 0x9C)
#define DM_RG_DRAMPKG					(DummyRG_BASE_ADDRESS + 0x9D)

#define DRAM_DREF_IHIGH 0
#define DRAM_DREF_DHIGH 1
#define DRAM_DREF_ILOW 2
#define DRAM_DREF_DLOW 3

typedef enum {

	DRAM_CH_NUM_CH0 = 0,
	DRAM_CH_NUM_CH1,
	DRAM_CH_NUM_CH2,
	DRAM_CH_NUM_CH3,
	DRAM_CH_NUM_CH4,
	DRAM_CH_NUM_CH5,
} DRAM_CH_NUM;

struct DRAMC_RESOURCE_PRIVATE {

	bool bSelfCreateTaskFlag;
	bool bDRAMCTaskProcFlag;
	bool bShuffleDFSTaskFlag;
	s32 sDRAMCPollingTaskID;
	bool bIdleCntCreateTaskFlag;
	bool bIdleCntTaskProcFlag;
	s32 sIdleCntPollingTaskID;
};

extern unsigned int mtk_dramc_get_dfs_freq(void);
extern bool mtk_dramc_set_dfs_freq(unsigned int dram_freq);
extern unsigned int mtk_dramc_get_support_channel_num(void);
extern unsigned int mtk_dramc_get_ddr_type(void);
extern unsigned int mtk_dramc_get_channel_size(int channel);
extern unsigned int mtk_dramc_get_channel_total_size(void);
extern unsigned int mtk_dramc_get_ddr_asymsize(void);
extern unsigned int mtk_dramc_get_ddrpkg(void);
extern unsigned int mtk_dramc_get_freq(int phychannel);
extern unsigned int mtk_dramc_ssc_set(DRAM_CH_NUM eMiuCh, u32 u32SscFrequency,
			u32 u32SscRatio, u8 bEnable);

extern void dramc_dynamic_ref_debug(void);
extern int get_cpu_temperature_bysar(void);
extern bool mtk_dramc_is_ddr_busy(void);
extern void mtk_dramc_set_idlecnt_busy_threshold(u16 threshold);

#if IS_ENABLED(CONFIG_MTK_DRAMC_CA)

#define E_DRAMC_OPTEE_INIT         (0)
#define E_DRAMC_OPTEE_ADJ_REF_RATE (1)
#define E_DRAMC_OPTEE_ADJ_SSC      (2)
#define E_DRAMC_OPTEE_SUSPEND      (3)
#define E_DRAMC_OPTEE_RESUME       (4)

#define TA_SUCCESS   0x00000000
#define TA_ERROR     0xFFFF0006

extern u32 MTK_DRAMC_TEEC_TA_Open(void);
extern u32 MTK_DRAMC_TEEC_Init(void);
extern u32 MTK_DRAMC_TEEC_Set_Ref_Rate(u32 ref_rate);
extern u32 MTK_DRAMC_TEEC_Set_SSC(u32 span, u32 step);
extern u32 MTK_DRAMC_TEEC_Suspend(void);
extern u32 MTK_DRAMC_TEEC_Resume(void);
#endif

#endif

