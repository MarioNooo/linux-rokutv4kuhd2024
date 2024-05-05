// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#include "drvNAND.h"
#include "mhal_gpio_reg.h"
#include <linux/gpio.h>

static struct _NAND_DRIVER sg_NandDrv;
static U8 gau8_PartInfo[PARTINFO_BYTE_CNT];
struct _PAIRED_PAGE_MAP ga_tPairedPageMap[PAIRED_PAGE_COUNT] = {{0, 0}};
EXPORT_SYMBOL(ga_tPairedPageMap);

#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
static DECLARE_WAIT_QUEUE_HEAD(fcie_wait);
static U16 u16CurNCMIEEvent;            // Used to store current IRQ state
static U32 SAR5_int;
#endif

extern void roku_set_backlight_status(bool status);

U32 nand_hw_timer_delay(U32 u32_us)
{
	U32 u32_i = u32_us;

	while (u32_i > HW_TIMER_DELAY_1ms) {
		udelay(HW_TIMER_DELAY_1ms);
		u32_i -= HW_TIMER_DELAY_1ms;
	}

	udelay(u32_i);

	return u32_us;
}
EXPORT_SYMBOL(nand_hw_timer_delay);

U32 nand_pads_init(void)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	pNandDrv->u16_Reg40_Signal =
		(BIT_NC_WP_AUTO | BIT_NC_WP_H | BIT_NC_CE_AUTO | BIT_NC_CE_H) &
		~(BIT_NC_CHK_RB_EDGEn | BIT_NC_CE_SEL_MASK);

	return UNFD_ST_SUCCESS;
}

U32 nand_pads_switch(U32 u32EnableFCIE)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	pNandDrv->u16_Reg58_DDRCtrl &= ~(BIT_DDR_MASM|BIT_SDR_DIN_FROM_MACRO|BIT_NC_32B_MODE);

	REG_WRITE_UINT16(NC_DDR_CTRL, pNandDrv->u16_Reg58_DDRCtrl);

	return UNFD_ST_SUCCESS;
}
EXPORT_SYMBOL(nand_pads_switch);

U32 nand_clock_setting(U32 u32ClkParam)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	long rate;
	int ret;

	rate = clk_round_rate(pNandDrv->priv->clk_info.clk,
		pNandDrv->priv->clk_info.clk_table[u32ClkParam].clk_hz);

	ret = clk_set_rate(pNandDrv->priv->clk_info.clk, rate);
	if (ret)
		nand_debug(UNFD_DEBUG_LEVEL, 1, "clk_set_rate(%ld) fail %d\n", rate, ret);

	return UNFD_ST_SUCCESS;
}
EXPORT_SYMBOL(nand_clock_setting);

void nand_DumpPadClk(void)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	nand_debug(0, 1, "clk setting: %ld Hz\n", clk_get_rate(pNandDrv->priv->clk_info.clk));
	nand_debug(0, 0, "\n\n");
}
EXPORT_SYMBOL(nand_DumpPadClk);

#if defined(DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
#define MAX_UNFD(a, b) ((a) > (b) ? (a) : (b))

U32 nand_config_timing(U16 u16_1T)
{
	#define NC_DEFAULT_DELAY	2
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U16 u16_DefaultTRR;
	U16 u16_DefaultTCS;
	U16 u16_DefaultTWW;
	U16 u16_DefaultRX40Cmd;
	U16 u16_DefaultRX40Adr;
	U16 u16_DefaultRX56;
	U16 u16_DefaultTADL;
	U16 u16_DefaultTCWAW;
	#if defined(NC_TWHR_TCLHZ) && NC_TWHR_TCLHZ
	U16 u16_DefaultTCLHZ = NC_SDR_DEFAULT_TCLHZ;
	#endif
	U16 u16_DefaultTWHR;
	U16 u16_Tmp, u16_Cnt;
	U16 u16_Tmp2, u16_Cnt2;

	u16_DefaultTRR = NC_SDR_DEFAULT_TRR;
	u16_DefaultTCS = NC_SDR_DEFAULT_TCS;
	u16_DefaultTWW = NC_SDR_DEFAULT_TWW;
	u16_DefaultRX40Cmd = NC_SDR_DEFAULT_RX40CMD;
	u16_DefaultRX40Adr = NC_SDR_DEFAULT_RX40ADR;
	u16_DefaultRX56 = NC_SDR_DEFAULT_RX56;
	u16_DefaultTADL = NC_SDR_DEFAULT_TADL;
	u16_DefaultTCWAW = NC_SDR_DEFAULT_TCWAW;
	u16_DefaultTWHR = NC_SDR_DEFAULT_TWHR;

	#if defined(NC_INST_DELAY) && NC_INST_DELAY
	// Check CMD_END
	u16_Tmp = MAX_UNFD(pNandDrv->u16_tWHR, pNandDrv->u16_tCWAW);
	u16_Cnt = (u16_Tmp + u16_1T - 1) / u16_1T;

	if (u16_DefaultRX40Cmd >= u16_Cnt)
		u16_Cnt = 0;
	else if (u16_Cnt-u16_DefaultRX40Cmd <= MAX_U8_VALUE)
		u16_Cnt -= u16_DefaultRX40Cmd;
	else
		return UNFD_ST_ERR_INVALID_PARAM;

	// Check ADR_END
	u16_Tmp2 = MAX_UNFD(MAX_UNFD(pNandDrv->u16_tWHR, pNandDrv->u16_tADL), pNandDrv->u16_tCCS);
	u16_Cnt2 = (u16_Tmp2 + u16_1T - 1) / u16_1T;

	if (u16_DefaultRX40Adr >= u16_Cnt2)
		u16_Cnt2 = 0;
	else if (u16_Cnt2-u16_DefaultRX40Adr <= MAX_U8_VALUE)
		u16_Cnt2 -= u16_DefaultRX40Adr;
	else
		return UNFD_ST_ERR_INVALID_PARAM;

	// get the max cnt
	u16_Cnt = MAX_UNFD(u16_Cnt, u16_Cnt2);

	pNandDrv->u16_Reg40_Signal &= ~(LEFT_SHIFT8(MAX_U8_VALUE));
	pNandDrv->u16_Reg40_Signal |= LEFT_SHIFT8(u16_Cnt);
	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "u16_Reg40_Signal =  %X\n",
		pNandDrv->u16_Reg40_Signal);
	#endif

	#if defined(NC_HWCMD_DELAY) && NC_HWCMD_DELAY
	u16_Cnt = ((pNandDrv->u16_tRHW + u16_1T - 1) / u16_1T) + NC_DEFAULT_DELAY;

	if (u16_DefaultRX56 >= u16_Cnt)
		u16_Cnt = 0;
	else if (u16_Cnt-u16_DefaultRX56 <= MAX_U8_VALUE)
		u16_Cnt -= u16_DefaultRX56;
	else
		return UNFD_ST_ERR_INVALID_PARAM;

	pNandDrv->u16_Reg56_Rand_W_Cmd &= ~(LEFT_SHIFT8(MAX_U8_VALUE));
	pNandDrv->u16_Reg56_Rand_W_Cmd |= LEFT_SHIFT8(u16_Cnt);
	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "u16_Reg56_Rand_W_Cmd =  %X\n",
		pNandDrv->u16_Reg56_Rand_W_Cmd);
	#endif

	#if defined(NC_TRR_TCS) && NC_TRR_TCS
	u16_Cnt = ((pNandDrv->u8_tRR + u16_1T - 1) / u16_1T) + NC_DEFAULT_DELAY;

	if (u16_DefaultTRR >= u16_Cnt)
		u16_Cnt = 0;
	else if (u16_Cnt-u16_DefaultTRR <= MAX_U4_VALUE)
		u16_Cnt -= u16_DefaultTRR;
	else
		return UNFD_ST_ERR_INVALID_PARAM;

	u16_Tmp = ((pNandDrv->u8_tCS + u16_1T - 1) / u16_1T) + NC_DEFAULT_DELAY;

	if (u16_DefaultTCS >= u16_Tmp)
		u16_Tmp = 0;
	else if (u16_Tmp-u16_DefaultTCS <= MAX_U4_VALUE)
		u16_Tmp -= u16_DefaultTCS;
	else
		return UNFD_ST_ERR_INVALID_PARAM;

	u16_Tmp2 = ((pNandDrv->u16_tWW + u16_1T - 1) / u16_1T) + NC_DEFAULT_DELAY;

	if (u16_DefaultTWW >= u16_Tmp2)
		u16_Tmp2 = 0;
	else if (u16_Tmp2-u16_DefaultTWW <= MAX_U4_VALUE)
		u16_Tmp2 -= u16_DefaultTWW;
	else
		return UNFD_ST_ERR_INVALID_PARAM;

	u16_Cnt2 = MAX_UNFD(u16_Tmp, u16_Tmp2);

	pNandDrv->u16_Reg59_LFSRCtrl &= ~((U16)MAX_U8_VALUE);
	pNandDrv->u16_Reg59_LFSRCtrl |= (u16_Cnt | LEFT_SHIFT4(u16_Cnt2));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "u16_Reg59_LFSRCtrl =  %X\n",
		pNandDrv->u16_Reg59_LFSRCtrl);
	#endif

	#if defined(NC_TCWAW_TADL) && NC_TCWAW_TADL
	u16_Cnt = ((pNandDrv->u16_tADL + u16_1T - 1) / u16_1T) + NC_DEFAULT_DELAY;

	if (u16_DefaultTADL > u16_Cnt)
		u16_Cnt = 0;
	else if (u16_Cnt-u16_DefaultTADL <= MAX_U8_VALUE)
		u16_Cnt -= u16_DefaultTADL;
	else
		return UNFD_ST_ERR_INVALID_PARAM;

	u16_Cnt2 = ((pNandDrv->u16_tCWAW + u16_1T - 1) / u16_1T) + NC_DEFAULT_DELAY;

	if (u16_DefaultTCWAW > u16_Cnt2)
		u16_Cnt2 = 0;
	else if (u16_Cnt2-u16_DefaultTCWAW <= MAX_U8_VALUE)
		u16_Cnt2 -= u16_DefaultTCWAW;
	else
		return UNFD_ST_ERR_INVALID_PARAM;

	pNandDrv->u16_Reg5D_tCWAW_tADL = (u16_Cnt|LEFT_SHIFT8(u16_Cnt2));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "u16_Reg5D_tCWAW_tADL =  %X\n",
		pNandDrv->u16_Reg5D_tCWAW_tADL);
	#endif

	#if defined(NC_TWHR_TCLHZ) && NC_TWHR_TCLHZ
	u16_Cnt = ((pNandDrv->u8_tCLHZ + u16_1T - 1) / u16_1T) + NC_DEFAULT_DELAY;

	if (u16_DefaultTCLHZ > u16_Cnt)
		u16_Cnt = 0;
	else if (u16_Cnt-u16_DefaultTCLHZ <= MAX_U4_VALUE)
		u16_Cnt -= u16_DefaultTCLHZ;
	else
		return UNFD_ST_ERR_INVALID_PARAM;

	u16_Cnt2 = ((pNandDrv->u16_tWHR + u16_1T - 1) / u16_1T) + NC_DEFAULT_DELAY;

	if (u16_DefaultTWHR > u16_Cnt2)
		u16_Cnt2 = 0;
	else if (u16_Cnt2-u16_DefaultTWHR <= MAX_U8_VALUE)
		u16_Cnt2 -= u16_DefaultTWHR;
	else
		return UNFD_ST_ERR_INVALID_PARAM;

	pNandDrv->u16_Reg5A_tWHR_tCLHZ = ((u16_Cnt & MAX_U4_VALUE) | LEFT_SHIFT8(u16_Cnt2));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH, 1, "u16_Reg5A_tWHR_tCLHZ =  %X\n",
		pNandDrv->u16_Reg5A_tWHR_tCLHZ);
	#endif

	NC_Config();
	return UNFD_ST_SUCCESS;
}

U32 nand_find_timing(U8 *pu8_ClkIdx, U8 u8_find_DDR_timg)
{
	#define NC_LATCH_DATA_CNT	16
	#define VALUE_1G		1000000000
	#define BIT_NC_LATCH_STS_SHIFT	5
	#define NC_LATCH_STS_CNT	8
	U32 u32_Err;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U16 u16_i, u16_j, u16_pass_begin, u16_pass_cnt;
	U16 u16_status;
	U8 au8_ID[NAND_ID_BYTE_CNT];
	U32 u32_Clk;
	U16 u16_SeqAccessTime, u16_Tmp, u16_Tmp2, u16_1T;
	S8 s8_ClkIdx;

	if (u8_find_DDR_timg)
		return UNFD_ST_ERR_PAD_UNSUPPORT_DDR_NAND;

	u16_Tmp = MAX_UNFD(MAX_UNFD(pNandDrv->u8_tRP, pNandDrv->u8_tREH), (pNandDrv->u16_tRC+1)>>1);
	u16_Tmp2 = MAX_UNFD(MAX_UNFD(pNandDrv->u8_tWP, pNandDrv->u8_tWH), (pNandDrv->u16_tWC+1)>>1);
	u16_SeqAccessTime = MAX_UNFD(u16_Tmp, u16_Tmp2);

	u16_Tmp = (pNandDrv->u8_tREA + NAND_SEQ_ACC_TIME_TOL)>>1;
	u16_Tmp2 = u16_SeqAccessTime;
	u16_SeqAccessTime = MAX_UNFD(u16_Tmp, u16_Tmp2);

	u32_Clk = VALUE_1G/((U32)u16_SeqAccessTime);
	for (s8_ClkIdx = 0 ; s8_ClkIdx < pNandDrv->priv->clk_info.clk_cnt ; s8_ClkIdx++) {
		if (u32_Clk < pNandDrv->priv->clk_info.clk_table[s8_ClkIdx].clk_hz)
			break;
	}
	if ((--s8_ClkIdx) < 0)
		return UNFD_ST_ERR_INVALID_PARAM;

	while (s8_ClkIdx >= 0) {
		u16_1T = VALUE_1G / pNandDrv->priv->clk_info.clk_table[s8_ClkIdx].clk_hz; //us
		if (nand_config_timing(u16_1T) != UNFD_ST_SUCCESS)
			s8_ClkIdx--;
		else
			break;
	}
	if (s8_ClkIdx < 0)
		return UNFD_ST_ERR_INVALID_PARAM;

	u32_Err = NC_ReadStatus();
	if (u32_Err != UNFD_ST_SUCCESS)
		return u32_Err;
	REG_READ_UINT16(NC_ST_READ, u16_status);

	pNandDrv->u32_Clk = s8_ClkIdx;
	nand_clock_setting(pNandDrv->u32_Clk);

	//using read id to detect relatch
	memcpy(au8_ID, pNandDrv->au8_ID, NAND_ID_BYTE_CNT);
	u16_pass_begin = MAX_U8_VALUE;
	u16_pass_cnt = 0;

	for (u16_i = 0 ; u16_i < NC_LATCH_DATA_CNT ; u16_i++) {
		pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_DATA_MASK|BIT_NC_PAD_SEL_FAILLING);
		if (u16_i & 1) //select falling edge otherwise rising edge is selected
			pNandDrv->u16_Reg57_RELatch |= BIT_NC_PAD_SEL_FAILLING;
		pNandDrv->u16_Reg57_RELatch |= ((u16_i >> 1) << 1) & BIT_NC_LATCH_DATA_MASK;

		NC_Config();
		u32_Err = NC_ReadID();
		if (u32_Err != UNFD_ST_SUCCESS) {
			nand_debug(0, 1, "ReadID Error with ErrCode 0x%X\n", u32_Err);
			//nand_die();
			return u32_Err;
		}
		for (u16_j = 0 ; u16_j < NAND_ID_BYTE_CNT ; u16_j++) {
			if (pNandDrv->au8_ID[u16_j] != au8_ID[u16_j])
				break;
		}
		if (u16_j == NAND_ID_BYTE_CNT) {
			if (u16_pass_begin == MAX_U8_VALUE)
				u16_pass_begin = u16_i;
			u16_pass_cnt++;
		}
		//break;
	}

	if (u16_pass_cnt != 0) {
		u16_i = u16_pass_begin + (u16_pass_cnt >> 1);
		pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_DATA_MASK|BIT0);
		if (u16_i & 1) //select falling edge otherwise rising edge is selected
			pNandDrv->u16_Reg57_RELatch |= BIT0;
		pNandDrv->u16_Reg57_RELatch |= ((u16_i >> 1) << 1) & BIT_NC_LATCH_DATA_MASK;
		memcpy(pNandDrv->au8_ID, au8_ID, NAND_ID_BYTE_CNT);
	} else {
		nand_debug(0, 1, "Read ID detect timing fails\n");
		pNandDrv->u16_Reg57_RELatch = REG57_ECO_FIX_INIT_VALUE;
		NC_Config();
		s8_ClkIdx = 0;
		*pu8_ClkIdx = (U8)s8_ClkIdx;
		memcpy(pNandDrv->au8_ID, au8_ID, NAND_ID_BYTE_CNT);
		return UNFD_ST_SUCCESS;
	}

	//detect read status
	u16_pass_begin = MAX_U8_VALUE;
	u16_pass_cnt = 0;

	for (u16_i = 0 ; u16_i < NC_LATCH_STS_CNT ; u16_i++) {
		U16 u16_tmpStatus;

		pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_STS_MASK);
		pNandDrv->u16_Reg57_RELatch |=
			((u16_i) << BIT_NC_LATCH_STS_SHIFT) & BIT_NC_LATCH_STS_MASK;

		NC_Config();
		NC_ReadStatus();
		REG_READ_UINT16(NC_ST_READ, u16_tmpStatus);

		if (u16_tmpStatus == u16_status) {
			if (u16_pass_begin == MAX_U8_VALUE)
				u16_pass_begin = u16_i;
			u16_pass_cnt++;
		}
	}

	if (u16_pass_cnt != 0) {
		u16_i = u16_pass_begin + (u16_pass_cnt >> 1);
		pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_STS_MASK);
		pNandDrv->u16_Reg57_RELatch |=
			((u16_i) << BIT_NC_LATCH_STS_SHIFT) & BIT_NC_LATCH_STS_MASK;
	} else {
		nand_debug(0, 1, "Read status detect timing fails\n");
		pNandDrv->u16_Reg57_RELatch = REG57_ECO_FIX_INIT_VALUE;
		NC_Config();
		s8_ClkIdx = 0;
		*pu8_ClkIdx = (U8)s8_ClkIdx;

		return UNFD_ST_SUCCESS;
	}

	*pu8_ClkIdx = (U8)s8_ClkIdx;

	return UNFD_ST_SUCCESS;
}
#endif

// [nand_config_clock] the entry for clock auto-config (by DECIDE_CLOCK_BY_NAND)
// [nand_find_timing]   find a FCIE clock
// [nand_config_timing] set other auxiliary parameters (cycle count)
// if timing parameters in nni are 0, would use default cycle count.
U32 nand_config_clock(void)
{
	#define MHZ	1000000
	U32 u32_Err = UNFD_ST_SUCCESS;
	#if defined(DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U8 u8_ClkIdx;

	#if defined(FCIE_LFSR) && FCIE_LFSR
	if (pNandDrv->u8_RequireRandomizer == 1)
		NC_DisableLFSR();
	#endif

	u32_Err = nand_find_timing(&u8_ClkIdx, 0);
	if (u32_Err != UNFD_ST_SUCCESS) {
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, Cannot config nand timing\n");
		//nand_die();
	} else {
		pNandDrv->u32_Clk = u8_ClkIdx;
		pr_info("FCIE is set to %dMHz\n",
			pNandDrv->priv->clk_info.clk_table[u8_ClkIdx].clk_hz/MHZ);

		nand_clock_setting(pNandDrv->u32_Clk);
		REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);
	}

	#if defined(FCIE_LFSR) && FCIE_LFSR
	if (pNandDrv->u8_RequireRandomizer == 1)
		NC_EnableLFSR();
	#endif
	#endif

	return u32_Err;
}
EXPORT_SYMBOL(nand_config_clock);

#if defined(ENABLE_NAND_POWER_SAVING_MODE) && ENABLE_NAND_POWER_SAVING_MODE
void nand_Prepare_Power_Saving_Mode_Queue(void)
{
	#define FUN_CTL_OFF	(0x63)
	#define FUN_CTL_VAL	(0x0001)
	#define MIE_EVENT_OFF	(0x60)
	#define MIE_EVENT_VAL	(0xffff)
	#define SIGNAL_OFF	(0x40)
	#define SIGNAL_VAL	(0x0024)

	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	int step = 0;

	REG_CLR_BITS_UINT16(NC_PWR_SAVE_CTL, BIT_BAT_SD_PWR_SAVE_MASK);

	/* (1) Clear HW Enable */
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, step), FUN_CTL_VAL);
	step++;
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, step),
			PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | FUN_CTL_OFF);
	step++;

	/* (2) Clear All Interrupt */
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, step), MIE_EVENT_VAL);
	step++;
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, step),
			PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | MIE_EVENT_OFF);
	step++;

	/* (3) CE WP */
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, step), SIGNAL_VAL);
	step++;
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, step),
			PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | SIGNAL_OFF);
	step++;

	/* (4) STOP */
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, step), 0x0000);
	step++;
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, step),
			PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_STOP);
	step++;

	REG_CLR_BITS_UINT16(NC_PWR_SAVE_CTL, BIT_SD_PWR_SAVE_RST);
	REG_SET_BITS_UINT16(NC_PWR_SAVE_CTL, BIT_SD_PWR_SAVE_RST);

	REG_SET_BITS_UINT16(NC_PWR_SAVE_CTL, BIT_PWR_SAVE_MODE | BIT_PWR_SAVE_INT_EN);
}
EXPORT_SYMBOL(nand_Prepare_Power_Saving_Mode_Queue);

void nand_CheckPowerCut(void)
{
	if (SAR5_int == 1) {
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 0, "SAR5 NAND trap before job start.\n");
		do {
		} while (1);
	}
}
EXPORT_SYMBOL(nand_CheckPowerCut);
#endif

dma_addr_t nand_translate_DMA_address_Ex(unsigned long ulong_DMAAddr,
	U32 u32_ByteCnt, int mode)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	return (ulong_DMAAddr - pNandDrv->priv->miu_base);
}
EXPORT_SYMBOL(nand_translate_DMA_address_Ex);

dma_addr_t nand_translate_Spare_DMA_address_Ex(unsigned long ulong_DMAAddr,
	U32 u32_ByteCnt, int mode)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	return (ulong_DMAAddr - pNandDrv->priv->miu_base);
}
EXPORT_SYMBOL(nand_translate_Spare_DMA_address_Ex);

dma_addr_t nand_DMA_MAP_address(void *p_Buffer, U32 u32_ByteCnt, int mode)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	dma_addr_t dma_addr;

	if (mode == WRITE_TO_NAND) {
		dma_addr = dma_map_single(pNandDrv->priv->dev, p_Buffer, u32_ByteCnt,
			DMA_TO_DEVICE);
	} else {
		dma_addr = dma_map_single(pNandDrv->priv->dev, p_Buffer, u32_ByteCnt,
			DMA_FROM_DEVICE);
	}

	return dma_addr;
}
EXPORT_SYMBOL(nand_DMA_MAP_address);

void nand_DMA_UNMAP_address(dma_addr_t DMAAddr, U32 u32_ByteCnt, int mode)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	if (mode == WRITE_TO_NAND)
		dma_unmap_single(pNandDrv->priv->dev, DMAAddr, u32_ByteCnt, DMA_TO_DEVICE);
	else
		dma_unmap_single(pNandDrv->priv->dev, DMAAddr, u32_ByteCnt, DMA_FROM_DEVICE);
}
EXPORT_SYMBOL(nand_DMA_UNMAP_address);

dma_addr_t nand_translate_DMA_MIUSel(unsigned long ulong_DMAAddr, U32 u32_ByteCnt)
{
	return 0;
}
EXPORT_SYMBOL(nand_translate_DMA_MIUSel);

void nand_read_dma_post_flush(unsigned long ulong_DMAAddr, U32 u32_ByteCnt)
{
}
EXPORT_SYMBOL(nand_read_dma_post_flush);

extern void * mmio_base_2423;
extern void * mmio_base_2426;
#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
irqreturn_t NC_FCIE_IRQ(int irq, void *dummy)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U16 u16_Events;
	REG_READ_UINT16(NC_PWR_SAVE_CTL, u16_Events);

	if (u16_Events & BIT_PWR_SAVE_MODE_INT) {
		REG_CLR_BITS_UINT16(NC_PWR_SAVE_CTL, BIT_PWR_SAVE_INT_EN);

		gpio_request(PAD_OPCTRL11, "pad_opctrl11");
		gpio_set_value(PAD_OPCTRL11, 0); // Backlight SOC_BL_ON (NA/SA:AE5, EU:AF8)
		roku_set_backlight_status(false);
		SAR5_int = 1;

		*((uint32_t*)mmio_base_2426 + 0x0E) = *((uint32_t*)mmio_base_2426 + 0x0E) | 0x20; //16bit address TEST_MOD[5]:OP disable=1
		*((uint32_t*)mmio_base_2423 + 0x18) = 0x0000; //16bit address turn off panel output signal
		*((uint32_t*)mmio_base_2423 + 0x19) = 0x0000; //16bit address turn off panel output signal
		*((uint32_t*)mmio_base_2426 + 0x0E) = *((uint32_t*)mmio_base_2426 + 0x0E) | 0x00; //16bit address TEST_MOD[5]=0:OP disable=0

		gpio_request(PAD_OPCTRL12, "pad_sar12");
		gpio_set_value(PAD_OPCTRL12, 0); // Panel SOC_PANEL_ON (NA/SA:AD5, EU:AE7)

		gpio_request(PAD_PWM4, "pad_pwm4");
		gpio_set_value(PAD_PWM4, 0); // Speakers AMP_MUTEn (NA/SA:F1, EU:C5)

		gpio_request(PAD_GPIO5, "pad_gpio5");
		gpio_set_value(PAD_GPIO5, 0); // Headphones HP_MUTEn (NA/SA:M25, EU:U26)

		gpio_request(PAD_I2SOUT_SD2_A, "pad_i2sout_sd2_a");
		gpio_set_value(PAD_I2SOUT_SD2_A, 0); // Speaker Reset AMP_RESETn (NA/SA:H3, EU:E3)

		gpio_request(PAD_SAR0, "pad_sar0");
		gpio_set_value(PAD_SAR0, 0); // USB USB_EN (NA/SA:AF9, EU:AL6)

		gpio_request(PAD_OPCTRL4, "pad_opctrl4");
		gpio_set_value(PAD_OPCTRL4, 0); // Wifi WIFI_RSTn (NA/SA:AH8, EU:AK1)

		gpio_request(PAD_GPIO45, "pad_gpio45");
		gpio_set_value(PAD_GPIO45, 0); // LNB POWER LNB_EN (EU:Y25)

		gpio_request(PAD_GPIO10, "pad_gpio10");
		gpio_set_value(PAD_GPIO10, 0); // PCMCIA POWER PCM_PWR_EN (EU:V25)

		printk(KERN_EMERG "SAR5 NAND WARN.\n");
		nand_hw_timer_delay(DELAY_1s_in_us);
		nand_hw_timer_delay(DELAY_1s_in_us);
		panic("\n");

		return IRQ_HANDLED;
	}

	if ((REG(NC_FUN_CTL) & BIT_NC_EN) != BIT_NC_EN)
		return IRQ_NONE;

	u16CurNCMIEEvent |= REG(NC_MIE_EVENT);
	REG_WRITE_UINT16(NC_MIE_EVENT, u16CurNCMIEEvent);
	wake_up(&fcie_wait);

	return IRQ_HANDLED;
}
EXPORT_SYMBOL(NC_FCIE_IRQ);

U32 nand_WaitCompleteIntr(U16 u16_WaitEvent, U32 u32_MicroSec, U16 *u16_WaitedEvent)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();
	U16 u16_Reg;
	U32 u32_Timeout = (usecs_to_jiffies(u32_MicroSec) > 0) ? usecs_to_jiffies(u32_MicroSec) : 1;

	wait_event_timeout(fcie_wait, ((u16CurNCMIEEvent & u16_WaitEvent) == u16_WaitEvent),
		u32_Timeout);

	if ((u16CurNCMIEEvent & u16_WaitEvent) != u16_WaitEvent) {
		if (SAR5_int == 1) {
			printk(KERN_EMERG "SAR5 NAND trap.\n");
			nand_hw_timer_delay(DELAY_1s_in_us);
			nand_hw_timer_delay(DELAY_1s_in_us);
			panic("\n");
		}
		*u16_WaitedEvent = u16CurNCMIEEvent;
		u16CurNCMIEEvent = 0;
		REG_READ_UINT16(NC_MIE_EVENT, u16_Reg); // Read all events
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Timeout: REG(NC_MIE_EVENT) = 0x%X\n",
			u16_Reg);
		return UNFD_ST_ERR_E_TIMEOUT;
	}

	*u16_WaitedEvent = u16CurNCMIEEvent = 0;
	REG_WRITE_UINT16(NC_MIE_EVENT, u16_WaitEvent);

	return UNFD_ST_SUCCESS;
}
EXPORT_SYMBOL(nand_WaitCompleteIntr);
#endif

void *drvNAND_get_DrvContext_address(void)
{
	return ((void *)&sg_NandDrv);
}
EXPORT_SYMBOL(drvNAND_get_DrvContext_address);

void *drvNAND_get_DrvContext_PartInfo(void)
{
	return ((void *)gau8_PartInfo);
}
EXPORT_SYMBOL(drvNAND_get_DrvContext_PartInfo);

U32 NC_PlatformResetPre(void)
{

	return UNFD_ST_SUCCESS;
}
EXPORT_SYMBOL(NC_PlatformResetPre);

U32 NC_PlatformResetPost(void)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	REG_WRITE_UINT16(NC_FUN_CTL, BIT_NC_EN);

	return UNFD_ST_SUCCESS;
}
EXPORT_SYMBOL(NC_PlatformResetPost);

U32 NC_PlatformInit(void)
{
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	nand_pads_init();
	pNandDrv->u8_WordMode = 0; // TV/Set-Top Box projects did not support x16 NAND flash
	nand_pads_switch(NAND_PAD_BYPASS_MODE);
	pNandDrv->u8_PadMode = NAND_PAD_BYPASS_MODE;

	pNandDrv->u32_Clk = FCIE_SW_SLOWEST_CLK;
	nand_clock_setting(FCIE_SW_SLOWEST_CLK);
	pNandDrv->u16_Reg57_RELatch = REG57_ECO_FIX_INIT_VALUE;
	REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);
	pNandDrv->u16_Reg59_LFSRCtrl = 0;
	REG_WRITE_UINT16(NC_LFSR_CTRL, 0);

	pNandDrv->pu8_PageDataBuf = kmalloc(PAGE_DATA_BUF_SIZE, GFP_KERNEL);
	pNandDrv->pu8_PageSpareBuf = kmalloc(PAGE_SPARE_BUF_SIZE, GFP_KERNEL);
	if (!pNandDrv->pu8_PageDataBuf || !pNandDrv->pu8_PageSpareBuf) {
		nand_debug(0, 1, "kmalloc fail\n");
		return UNFD_ST_ERR_NULL_PTR;
	}

	return UNFD_ST_SUCCESS;
}
EXPORT_SYMBOL(NC_PlatformInit);
