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

#ifndef _HWREG_SRCCAP_HDMIRX_H_
#define _HWREG_SRCCAP_HDMIRX_H_

#include "hwreg_common.h"
#include "drvXC_HDMI_if.h"
#include "hwreg_srccap_hdmirx_struct.h"

/* ============================================================================================== */
/* ------------------------------------------ Defines ------------------------------------------- */
/* ============================================================================================== */

#define HDMI_DWORD_FUL_MSK 0xffffffff
#define HDMI_WORD_FUL_MSK 0xffff
#define HDMI_WORD_LH_MSK 0x00ff
#define HDMI_WORD_HH_MSK  0xff00
#define HDMI_BYTE_FUL_MSK 0xff
#define HDMI_BYTE_LH_MSK 0x0f
#define HDMI_BYTE_HH_MSK 0xf0

#define HDMI_BIT0 0x1
#define HDMI_BIT1 0x2
#define HDMI_BIT2 0x4
#define HDMI_BIT3 0x8
#define HDMI_BIT4 0x10
#define HDMI_BIT5 0x20
#define HDMI_BIT6 0x40
#define HDMI_BIT7 0x80
#define HDMI_BIT8 0x100
#define HDMI_BIT9 0x200
#define HDMI_BIT10 0x400
#define HDMI_BIT11 0x800
#define HDMI_BIT12 0x1000
#define HDMI_BIT13 0x2000
#define HDMI_BIT14 0x4000
#define HDMI_BIT15 0x8000
#define HDMI_BIT16 0x10000

#define ONE_K_UNIT (1000)
#define ONE_M_UNIT (ONE_K_UNIT*ONE_K_UNIT)

#define HDMI_VAL_0 (0)
#define HDMI_VAL_1 (1)
#define HDMI_VAL_2 (2)
#define HDMI_VAL_3 (3)
#define HDMI_VAL_4 (4)
#define HDMI_VAL_5 (5)
#define HDMI_VAL_6 (6)
#define HDMI_VAL_7 (7)
#define HDMI_VAL_8 (8)
#define HDMI_VAL_9 (9)
#define HDMI_VAL_10 (10)
#define HDMI_VAL_11 (11)
#define HDMI_VAL_12 (12)
#define HDMI_VAL_13 (13)
#define HDMI_VAL_14 (14)
#define HDMI_VAL_15 (15)
#define HDMI_VAL_16 (16)


#define HDMI_BYTE_BITS (8)
#define HDMI_WORD_BITS (16)
#define HDMI_DWORD_BITS (32)

#define HDCP22_REAUTH_ACCU
#define EMP_MTW_IRQ_EN

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Enums -------------------------------------------- */
/* ============================================================================================== */

enum HDMI_CAPABILITY
{
	HDMI_CAPABILITY_FRL = BIT(0),
	HDMI_CAPABILITY_DSC = BIT(1),
};

/* ============================================================================================== */
/* ------------------------------------------ Structs ------------------------------------------- */
/* ============================================================================================== */

typedef struct{
    u32 regbase1;
    u32 regbase2;
    u32 bank_hdmirx_dtop;
    u32 bank_hdmirx_powersave;
    u32 bank_hdmirx_hdcp;
    u32 bank_hdmirx_dtop_pkt;
    u32 bank_hdmirx_ve;
    u32 bank_hdmirx_hdmi;
    u32 bank_hdmirx_hdmi2;
    u32 bank_hdmirx_hdmi3;
    u32 bank_hdmirx_em;
    u32 bank_combo_gp_top;
    u32 bank_chip_gpio_mux;
    u32 bank_phy2p1_0;
    u32 bank_phy2p1_1;
    u32 bank_phy2p1_2;
    u32 bank_phy2p1_3;
    u32 bank_phy2p1_4;
    u32 bank_phy_train;
    u32 bank_clkgen0;
    u32 bank_clkgen1;
    u32 bank_ddc;
    u32 bank_scdc;
    u32 bank_phy21_pm;
    u32 bank_hdcpkey;
    u32 bank_pm_slp;
    u32 bank_sc;

    u32 bank_hdmirx;
    u32 bank_hdmirx_end;
    u32 bank_scdc_p0;
    u32 bank_scdc_xa8_p0;
    u32 bank_scdc_p1;
    u32 bank_scdc_xa8_p1;
    u32 bank_scdc_p2;
    u32 bank_scdc_xa8_p2;
    u32 bank_scdc_p3;
    u32 bank_scdc_xa8_p3;
    u32 bank_hdmirxphy_pm_p0;
    u32 bank_hdmirxphy_pm_p1;
    u32 bank_hdmirxphy_pm_p2;
    u32 bank_hdmirxphy_pm_p3;
    u32 bank_efuse_0;
    u32 bank_pm_top;
    u32 bank_pmux;
    u32 bank_dig_mux;

    u8 	port_num;

    u8 *pExchangeDataPA;
    u32 u32ExchangeDataSize;
    u8 *pMemoryBusBase;
}stHDMIRx_Bank;


/* ============================================================================================== */
/* ----------------------------------------- Functions ------------------------------------------ */
/* ============================================================================================== */
extern MS_BOOL drv_hwreg_srccap_hdmirx_attach(
	int hw_version,
	MS_BOOL b_attach);

MS_BOOL drv_hwreg_srccap_hdmirx_init(
	bool bImmeswitchSupport,
	uint8_t ucMHLSupportPath,
	const stHDMIRx_Bank *pstHDMIRxBank);

MS_BOOL drv_hwreg_srccap_hdmirx_deinit(
	const stHDMIRx_Bank *pstHDMIRxBank);

MS_BOOL drv_hwreg_srccap_hdmirx_setSimData(char *ucSimData);

void drv_hwreg_srccap_hdmirx_setSimMode(MS_BOOL bSimMode);
MS_BOOL drv_hwreg_srccap_hdmirx_isSimMode(void);

void drv_hwreg_srccap_hdmirx_setloglevel(int loglevel);
int drv_hwreg_srccap_hdmirx_getloglevel(void);

extern MS_BOOL drv_hwreg_srccap_hdmirx_ioremap(
	const stHDMIRx_Bank *pstHDMIRxBank);

extern MS_BOOL drv_hwreg_srccap_hdmirx_iounmap(
	const stHDMIRx_Bank *pstHDMIRxBank);

extern void drv_hwreg_srccap_hdmirx_arc_pinctrl(
	E_MUX_INPUTPORT enInputPortType,
	MS_BOOL bEnable ,
	MS_BOOL bDrivingHigh);

extern void drv_hwreg_srccap_hdmirx_setTimingDetectLog(
    E_MUX_INPUTPORT enInputPortType, EN_TIMING_DETECT_STATE enState,
    MS_U8 u8RegenCnt);

extern MS_BOOL drv_hwreg_srccap_hdmirx_getTimingDetectData(
    stHDMIRx_TIMING_LOG *pTimingDetectData);

extern MS_U32 drv_hwreg_srccap_hdmirx_getCapability(
    E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL
drv_hwreg_srccap_hdmirx_a_query_2p1_port(E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL
drv_hwreg_srccap_hdmirx_a_query_dsc_port(E_MUX_INPUTPORT enInputPortType);

extern MS_U32
drv_hwreg_srccap_hdmirx_query_module_hw_param(
	enum hdmi_hw_parameter_list hw_param,
	enum hdmi_param_query_type param_type,
	E_MUX_INPUTPORT enInputPortType);

extern MS_BOOL
drv_hwreg_srccap_hdmirx_set_module_hw_param_sw_prog(
	enum hdmi_hw_parameter_list hw_param,
	E_MUX_INPUTPORT enInputPortType,
	MS_U32 u32_val);

extern MS_U16
drv_hwreg_srccap_hdmirx_query_video_n(void);
extern MS_U16
drv_hwreg_srccap_hdmirx_query_audio_n(void);
extern MS_U16
drv_hwreg_srccap_hdmirx_query_dvi2miu_n(void);
extern MS_U16
drv_hwreg_srccap_hdmirx_query_dsc_n(void);

extern MS_U32
drv_hwreg_srccap_hdmirx_query_dram_ofs_r2load(void);
extern MS_U32
drv_hwreg_srccap_hdmirx_query_dram_ofs_pktq(void);
extern MS_U32
drv_hwreg_srccap_hdmirx_query_dram_ofs_dvi2miu(void);
extern MS_U32
drv_hwreg_srccap_hdmirx_query_dram_ofs_end(void);
extern MS_U8
drv_hwreg_srccap_hdmirx_a_query_max_port_n(void);
#endif // _HWREG_SRCCAP_HDMIRX_H_
