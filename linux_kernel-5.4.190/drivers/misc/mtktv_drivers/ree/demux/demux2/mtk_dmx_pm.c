// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/clk-provider.h>
#include <linux/of.h>

#include "mtk_dmx_dev.h"
#include "mtk_dmx_common.h"
#include "mtk_dmx_tm.h"
#include "mtk_dmx_pm.h"
#include "mtk_dmx_io.h"
#include "mtk_dmx_ctrl.h"
#include "mtk_dmx_tee_if.h"
#include "mtk_dmx_lite.h"
#include "clk-dtv.h"

#define MHZ					(1000UL * 1000UL)
#define NTP_BASEPOINT_CNT	(16 * 1024 * 1024)

#ifndef DMX2LITE
static bool bDmxTeeInited;

static int _populate_fw_buffer(struct platform_device *pdev, void __iomem *vaddr, u32 size)
{
	int ret = 0;
	const struct firmware *tsp_fw = NULL;

	ret = request_firmware(&tsp_fw, TSP_FW_NAME, &pdev->dev);
	if (ret == 0) {
		if (size >= tsp_fw->size) {
			memcpy((u8 *) vaddr, tsp_fw->data, tsp_fw->size);
		} else {
			MTK_DMX_ERR("firmware buffer is NOT enough\n");
			ret = -ENOMEM;
		}
	} else {
		MTK_DMX_ERR("request TSP firmware fail\n");
		return ret;
	}

	release_firmware(tsp_fw);
	return ret;
}
#endif

static inline int mtk_dmx_pm_get(struct clk **clk,
	struct platform_device *pdev, const char *name)
{
	*clk = devm_clk_get(&pdev->dev, name);
	if ((IS_ERR(*clk) && PTR_ERR(*clk) != -ENOENT) || *clk == NULL) {
		MTK_DMX_ERR("failed to get %s handle, error: %ld\n", name, PTR_ERR(*clk));
		return 1;
	}
	return 0;
}

static int mtk_dmx_pm_clock_init(struct platform_device *pdev, struct mtk_dmx_pm *pm)
{
	int i, ret = 0, q, r, tmp;
	char name[32] = "";
	struct mtk_dmx2dev *mtkdev = container_of(pm, struct mtk_dmx2dev, pm);
	u16 stc_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_STC);
	struct device_node *np;

	// always on
	ret |= mtk_dmx_pm_get(&pm->tsp_clk,
		pdev, "clk_tsp");
	ret |= mtk_dmx_pm_get(&pm->top_mg_smi_ck,
		pdev, "clk_top_mg_smi_ck");
	ret |= mtk_dmx_pm_get(&pm->tsp_smi_tsp,
		pdev, "clk_smi_tsp");

	// time stamp
	ret |= mtk_dmx_pm_get(&pm->timestamp_clk,
		pdev, "clk_stamp");

	// parser
	ret |= mtk_dmx_pm_get(&pm->parser_clk,
		pdev, "clk_parser");
	ret |= mtk_dmx_pm_get(&pm->vivaldi9_parser_clk,
		pdev, "clk_viva_parser");

	// stc
	pm->stc_synth_clk =
		devm_kzalloc(&pdev->dev, sizeof(struct clk *) * stc_num, GFP_KERNEL);
	if (!pm->stc_synth_clk) {
		MTK_DMX_ERR("allocate clk_sync_stc handle fail\n");
		return -ENOMEM;
	}
	for (i = 0; i < stc_num; i++) {
		if (snprintf(name, sizeof(name), "clk_sync_stc%d", i) < 0) {
			MTK_DMX_ERR("snprintf error - stc index %d\n", i);
			continue;
		}
		ret |= mtk_dmx_pm_get(&pm->stc_synth_clk[i],
			pdev, name);
	}
	pm->stc_clk =
		devm_kzalloc(&pdev->dev, sizeof(struct clk *) * stc_num, GFP_KERNEL);
	if (!pm->stc_clk) {
		MTK_DMX_ERR("allocate clk_stc handle fail\n");
		return -ENOMEM;
	}
	for (i = 0; i < stc_num; i++) {
		if (snprintf(name, sizeof(name), "clk_stc%d", i) < 0) {
			MTK_DMX_ERR("snprintf error - stc index %d\n", i);
			continue;
		}
		ret |= mtk_dmx_pm_get(&pm->stc_clk[i],
			pdev, name);
	}

	// sw_en
	mtk_dmx_pm_swen_set_base();
	for (i = 0; i < SWEN_TOTAL; i++)
		pm->swen2index[i] = -1;
	np = of_find_node_by_name(pdev->dev.of_node, "swen");
	if (np != NULL)
		of_property_read_u32(np, "num", &pm->swen_cnt);

	pm->swen_list = devm_kzalloc(&pdev->dev,
		sizeof(struct mtk_dmx_swen) * pm->swen_cnt, GFP_KERNEL);
	if (!pm->swen_list) {
		MTK_DMX_ERR("allocate swen list fail\n");
		return -ENOMEM;
	}

	for (i = 0; i < (pm->swen_cnt * SWEN_LINE); i++) {
		if (of_property_read_u32_index(np, "list", i, &tmp)) {
			MTK_DMX_ERR("get list fail\n");
			return -ENODEV;
		}
		q = i / SWEN_LINE;
		r = i % SWEN_LINE;
		switch (r) {
		case 0:
			pm->swen_list[q].swen = tmp;
			tmp = mtk_dmx_pm_swen_get_offset(tmp);
			pm->swen2index[tmp] = q;
			break;
		case 1:
			pm->swen_list[q].offset = tmp;
			break;
		case 2:
			pm->swen_list[q].start = tmp;
			break;
		case 3:
			pm->swen_list[q].end = tmp;
			break;
		}
	}

	return (ret == 0) ? 0 : -ENODEV;
}

static inline void mtk_dmx_pm_enable(struct clk *target)
{
	if (!IS_ERR_OR_NULL(target))
		if (clk_prepare_enable(target) < 0)
			MTK_DMX_ERR("%s prepare & enable fail\n", __clk_get_name(target));
}

static inline void mtk_dmx_pm_disable(struct clk *target)
{
	if (!IS_ERR_OR_NULL(target) && __clk_is_enabled(target))
		clk_disable_unprepare(target);
}

static inline void mtk_dmx_pm_set_rate(struct clk *target, unsigned long rate)
{
	unsigned long old_rate;
	long round_rate;
	unsigned long rate_patch = (rate > (ULONG_MAX - MHZ)) ? ULONG_MAX : (rate + MHZ);

	if (!IS_ERR_OR_NULL(target)) {
		old_rate = clk_get_rate(target);
		round_rate = clk_round_rate(target, rate_patch);
		if (round_rate < 0) {
			MTK_DMX_ERR("%s set round rate fail\n", __clk_get_name(target));
			return;
		}
		if (old_rate != round_rate)
			if (clk_set_rate(target, round_rate) < 0) {
				MTK_DMX_ERR("%s set rate fail\n", __clk_get_name(target));
				return;
			}
	}
}

static inline void mtk_dmx_pm_set_parent(struct clk *target, struct clk *parent)
{
	if (!IS_ERR_OR_NULL(target) && !IS_ERR_OR_NULL(parent))
		if (clk_set_parent(target, parent) < 0)
			MTK_DMX_ERR("%s set parent fail\n", __clk_get_name(target));
}


static inline void mtk_dmx_pm_swen_ctrl(bool en, bool cond,
	struct mtk_dmx_pm *pm, mtk_tsp_swen swen)
{
	int idx;
	struct mtk_dmx_swen *p;

	idx = mtk_dmx_pm_swen_get_offset(swen);
	idx = pm->swen2index[idx];
	if (idx != -1) {
		p = &pm->swen_list[idx];
		if (cond)
			clock_write(CLK_NONPM, p->offset, en, p->start, p->end);
	}
}

#define mtk_dmx_pm_enable_v2(__PTR, __COND, __SWEN) \
	mtk_dmx_pm_swen_ctrl(true, __COND, __PTR, __SWEN)

#define mtk_dmx_pm_disable_v2(__PTR, __COND, __SWEN) \
	mtk_dmx_pm_swen_ctrl(false, __COND, __PTR, __SWEN)

static int mtk_dmx_pm_clock_ctrl(struct mtk_dmx_pm *pm, bool bEnable)
{
	int ret = 0;
	struct mtk_dmx2dev *mtkdev = container_of(pm, struct mtk_dmx2dev, pm);
	u16 tsp_ck_rate = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_CK_RATE_TSP);
#ifndef DMX2LITE
	mtk_dmx_mcm_ctrl(bEnable);
#endif
	if (bEnable) {
		mtk_dmx_pm_set_rate(pm->tsp_clk, tsp_ck_rate * MHZ);
		mtk_dmx_pm_enable(pm->tsp_clk);
		mtk_dmx_pm_set_parent(pm->tsp_smi_tsp, pm->top_mg_smi_ck);
		mtk_dmx_pm_enable(pm->tsp_smi_tsp);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_MCU_NONPM2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_SMI_TSP2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TSP2TSP);
		mtk_dmx_pm_runtime(pm, MTK_DMX_PM_STATUS_RESUME, false);
	} else {
		mtk_dmx_pm_runtime(pm, MTK_DMX_PM_STATUS_SUSPEND, false);
		mtk_dmx_pm_disable(pm->tsp_smi_tsp);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_MCU_NONPM2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_SMI_TSP2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TSP2TSP);
		mtk_dmx_pm_disable(pm->tsp_clk);
	}
	return ret;
}

int mtk_dmx_pm_runtime(struct mtk_dmx_pm *pm, mtk_dmx_pm_status eStatus, bool runtime_ops)
{
	int i;
	struct mtk_dmx2dev *mtkdev = container_of(pm, struct mtk_dmx2dev, pm);
	u16 stc_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_STC);
	u16 pvr_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_PVR);
	u16 tlv_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_TLV);
	u16 alp_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_ALP);
	u16 tso_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_TSO);
	u16 cilink_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_CILINK);
	u16 parser_ck_rate = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_CK_RATE_PARSER);
	u16 stc_ck_rate = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_CK_RATE_STC);

	MTK_DMX_INFO("dmx runtime pm(%d): %s\n", runtime_ops,
		(eStatus == MTK_DMX_PM_STATUS_SUSPEND) ? "SUSPEND" :
		(eStatus == MTK_DMX_PM_STATUS_RESUME) ? "RESUME" : "IDLE");
	switch (eStatus) {
	case MTK_DMX_PM_STATUS_SUSPEND:
		if (runtime_ops)
			pm->on = false;
		// tsp
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TSP2DSCRMB);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TS02TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TS12TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TS22TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TSFI2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TSIF0_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_FIQ0_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_MM0_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_MM1_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_STAMP2TSP);
		// time stamp
		mtk_dmx_pm_disable(pm->timestamp_clk);
		// ts sample
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TS_SAMPLE2TS_SAMPLE);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TS0_IN_DFT2TS_SAMPLE);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TS1_IN_DFT2TS_SAMPLE);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_TS2_IN_DFT2TS_SAMPLE);
		// path
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_PATH0_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_PATH1_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_PATH2_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_PATH_FI_27M2TSP);
		// parser
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_PARSER2TSP);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_PARSER2VD_PES_PARSER);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_SMI2VD_PES_PARSER);
		mtk_dmx_pm_disable_v2(pm, true, MTK_DMX_SWEN_VIVA_PARSER2PARSER);
		mtk_dmx_pm_disable(pm->parser_clk);
		mtk_dmx_pm_disable(pm->vivaldi9_parser_clk);
		// stc
		mtk_dmx_pm_disable_v2(pm, (stc_num >= 1), MTK_DMX_SWEN_SYN_STC02TS_SAMPLE);
		mtk_dmx_pm_disable_v2(pm, (stc_num >= 2), MTK_DMX_SWEN_SYN_STC12TS_SAMPLE);
		mtk_dmx_pm_disable_v2(pm, (stc_num >= 3), MTK_DMX_SWEN_SYN_STC22TS_SAMPLE);
		mtk_dmx_pm_disable_v2(pm, (stc_num >= 4), MTK_DMX_SWEN_SYN_STC32TS_SAMPLE);
		mtk_dmx_pm_disable_v2(pm, (stc_num >= 1), MTK_DMX_SWEN_SYS_TIME0_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, (stc_num >= 2), MTK_DMX_SWEN_SYS_TIME1_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, (stc_num >= 3), MTK_DMX_SWEN_SYS_TIME2_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, (stc_num >= 4), MTK_DMX_SWEN_SYS_TIME3_27M2TSP);
		for (i = 0; i < stc_num; i++) {
			mtk_dmx_pm_disable(pm->stc_synth_clk[i]);
			mtk_dmx_pm_disable(pm->stc_clk[i]);
		}
		// pvr
		mtk_dmx_pm_disable_v2(pm, (pvr_num >= 1), MTK_DMX_SWEN_PVR1_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, (pvr_num >= 2), MTK_DMX_SWEN_PVR2_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, (pvr_num >= 3), MTK_DMX_SWEN_PVR3_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, (pvr_num >= 4), MTK_DMX_SWEN_PVR4_27M2TSP);
		// tlv
		mtk_dmx_pm_disable_v2(pm, (tlv_num >= 1), MTK_DMX_SWEN_TS_TLV02TSP);
		mtk_dmx_pm_disable_v2(pm, (tlv_num >= 2), MTK_DMX_SWEN_TS_TLV12TSP);
		mtk_dmx_pm_disable_v2(pm, (tlv_num >= 3), MTK_DMX_SWEN_TS_TLV22TSP);
		mtk_dmx_pm_disable_v2(pm, (tlv_num >= 1), MTK_DMX_SWEN_TLV0_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, (tlv_num >= 2), MTK_DMX_SWEN_TLV1_27M2TSP);
		mtk_dmx_pm_disable_v2(pm, (tlv_num >= 3), MTK_DMX_SWEN_TLV2_27M2TSP);
		// alp
		mtk_dmx_pm_disable_v2(pm, (alp_num >= 1), MTK_DMX_SWEN_TS_ALP02TSP);
		mtk_dmx_pm_disable_v2(pm, (alp_num >= 2), MTK_DMX_SWEN_TS_ALP12TSP);
		// tso
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSP2TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSP2TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_SMI2TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_SMI2TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_STAMP2TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_STAMP2TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH1_27M2TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH12TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH5_27M2TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH52TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH6_27M2TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH62TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_OUT_DIV_MN_SRC2TS_SAMPLE);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_OUT2TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_S2P12TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_S2P22TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_TRACE_IN2TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_TRACE2TSO);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH1_27M2TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH12TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH5_27M2TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH52TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH6_27M2TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH62TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_OUT_DIV_MN_SRC2TS_SAMPLE);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_OUT2TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_S2P12TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_S2P22TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_TRACE_IN2TSO2);
		mtk_dmx_pm_disable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_TRACE2TSO2);
		// cilink
		mtk_dmx_pm_disable_v2(pm, (cilink_num >= 1), MTK_DMX_SWEN_CILINK_IF2CILINK);
		mtk_dmx_pm_disable_v2(pm, (cilink_num >= 1), MTK_DMX_SWEN_CILINK_2B_RX2CILINK);
		mtk_dmx_pm_disable_v2(pm, (cilink_num >= 1), MTK_DMX_SWEN_CILINK_2B_TX2CILINK);
		mtk_dmx_pm_disable_v2(pm, (cilink_num >= 1), MTK_DMX_SWEN_TS_TX_C12CILINK);
		mtk_dmx_pm_disable_v2(pm, (cilink_num >= 2), MTK_DMX_SWEN_TS_TX_C22CILINK);
		break;
	case MTK_DMX_PM_STATUS_RESUME:
		if (runtime_ops)
			pm->on = true;
		// tsp
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TSP2DSCRMB);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TS02TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TS12TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TS22TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TSFI2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TSIF0_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_FIQ0_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_MM0_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_MM1_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_STAMP2TSP);
		// time stamp
		mtk_dmx_pm_set_rate(pm->timestamp_clk, ULONG_MAX);
		mtk_dmx_pm_enable(pm->timestamp_clk);
		// ts sample
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TS_SAMPLE2TS_SAMPLE);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TS0_IN_DFT2TS_SAMPLE);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TS1_IN_DFT2TS_SAMPLE);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_TS2_IN_DFT2TS_SAMPLE);
		// path
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_PATH0_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_PATH1_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_PATH2_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_PATH_FI_27M2TSP);
		// parser
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_PARSER2TSP);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_PARSER2VD_PES_PARSER);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_SMI2VD_PES_PARSER);
		mtk_dmx_pm_enable_v2(pm, true, MTK_DMX_SWEN_VIVA_PARSER2PARSER);
		mtk_dmx_pm_set_rate(pm->parser_clk, parser_ck_rate * MHZ);
		mtk_dmx_pm_enable(pm->parser_clk);
		mtk_dmx_pm_set_parent(pm->vivaldi9_parser_clk, pm->parser_clk);
		mtk_dmx_pm_enable(pm->vivaldi9_parser_clk);
		// stc
		mtk_dmx_pm_enable_v2(pm, (stc_num >= 1), MTK_DMX_SWEN_SYN_STC02TS_SAMPLE);
		mtk_dmx_pm_enable_v2(pm, (stc_num >= 2), MTK_DMX_SWEN_SYN_STC12TS_SAMPLE);
		mtk_dmx_pm_enable_v2(pm, (stc_num >= 3), MTK_DMX_SWEN_SYN_STC22TS_SAMPLE);
		mtk_dmx_pm_enable_v2(pm, (stc_num >= 4), MTK_DMX_SWEN_SYN_STC32TS_SAMPLE);
		mtk_dmx_pm_enable_v2(pm, (stc_num >= 1), MTK_DMX_SWEN_SYS_TIME0_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, (stc_num >= 2), MTK_DMX_SWEN_SYS_TIME1_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, (stc_num >= 3), MTK_DMX_SWEN_SYS_TIME2_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, (stc_num >= 4), MTK_DMX_SWEN_SYS_TIME3_27M2TSP);
		for (i = 0; i < stc_num; i++) {
			mtk_dmx_pm_set_rate(pm->stc_synth_clk[i], (stc_ck_rate * MHZ));
			mtk_dmx_pm_enable(pm->stc_synth_clk[i]);
			mtk_dmx_pm_set_parent(pm->stc_clk[i], pm->stc_synth_clk[i]);
			mtk_dmx_pm_enable(pm->stc_clk[i]);
		}
		// pvr
		mtk_dmx_pm_enable_v2(pm, (pvr_num >= 1), MTK_DMX_SWEN_PVR1_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, (pvr_num >= 2), MTK_DMX_SWEN_PVR2_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, (pvr_num >= 3), MTK_DMX_SWEN_PVR3_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, (pvr_num >= 4), MTK_DMX_SWEN_PVR4_27M2TSP);
		// tlv
		mtk_dmx_pm_enable_v2(pm, (tlv_num >= 1), MTK_DMX_SWEN_TS_TLV02TSP);
		mtk_dmx_pm_enable_v2(pm, (tlv_num >= 2), MTK_DMX_SWEN_TS_TLV12TSP);
		mtk_dmx_pm_enable_v2(pm, (tlv_num >= 3), MTK_DMX_SWEN_TS_TLV22TSP);
		mtk_dmx_pm_enable_v2(pm, (tlv_num >= 1), MTK_DMX_SWEN_TLV0_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, (tlv_num >= 2), MTK_DMX_SWEN_TLV1_27M2TSP);
		mtk_dmx_pm_enable_v2(pm, (tlv_num >= 3), MTK_DMX_SWEN_TLV2_27M2TSP);
		// alp
		mtk_dmx_pm_enable_v2(pm, (alp_num >= 1), MTK_DMX_SWEN_TS_ALP02TSP);
		mtk_dmx_pm_enable_v2(pm, (alp_num >= 2), MTK_DMX_SWEN_TS_ALP12TSP);
		// tso
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSP2TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSP2TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_SMI2TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_SMI2TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_STAMP2TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_STAMP2TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH1_27M2TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH12TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH5_27M2TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH52TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH6_27M2TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_CH62TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_OUT_DIV_MN_SRC2TS_SAMPLE);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_OUT2TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_S2P12TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_S2P22TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_TRACE_IN2TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 1), MTK_DMX_SWEN_TSO_TRACE2TSO);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH1_27M2TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH12TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH5_27M2TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH52TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH6_27M2TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_CH62TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_OUT_DIV_MN_SRC2TS_SAMPLE);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_OUT2TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_S2P12TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_S2P22TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_TRACE_IN2TSO2);
		mtk_dmx_pm_enable_v2(pm, (tso_num >= 2), MTK_DMX_SWEN_TSO2_TRACE2TSO2);
		// cilink
		mtk_dmx_pm_enable_v2(pm, (cilink_num >= 1), MTK_DMX_SWEN_CILINK_IF2CILINK);
		mtk_dmx_pm_enable_v2(pm, (cilink_num >= 1), MTK_DMX_SWEN_CILINK_2B_RX2CILINK);
		mtk_dmx_pm_enable_v2(pm, (cilink_num >= 1), MTK_DMX_SWEN_CILINK_2B_TX2CILINK);
		mtk_dmx_pm_enable_v2(pm, (cilink_num >= 1), MTK_DMX_SWEN_TS_TX_C12CILINK);
		mtk_dmx_pm_enable_v2(pm, (cilink_num >= 2), MTK_DMX_SWEN_TS_TX_C22CILINK);
		// tso sw rst
#ifndef DMX2LITE
		mtk_dmx_reset_tso();
#endif
		break;
	case MTK_DMX_PM_STATUS_IDLE:
		break;
	default:
		break;
	}
	return 0;
}

#ifndef DMX2LITE
static int _pm_init_mcu_and_vq(struct mtk_dmx_pm *pm)
{
	struct mem_info *pfw_buffer;
	struct mem_info *pvq_buffer;
	struct mtk_dmx2dev *mtkdev = container_of(pm, struct mtk_dmx2dev, pm);
	struct platform_device *pdev = to_platform_device(mtkdev->dev);
	u16 vq_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_VQ);

	mtk_dmx_ctrl_init(&bDmxTeeInited);
	MTK_DMX_INFO("bDmxTeeInited = %u\n", bDmxTeeInited);

	if (!bDmxTeeInited) {
		pfw_buffer = mtkdev->rm.memmap->list[MMAP_BUF_TYPE_FW];
		if (mtk_dmx_rm_get_buf(&mtkdev->rm, MMAP_BUF_TYPE_FW,
			pfw_buffer->size, pfw_buffer) < 0) {
			MTK_DMX_ERR("get FW buf fail\n");
			return -ENOMEM;
		}

		if (_populate_fw_buffer(pdev, pfw_buffer->vaddr, pfw_buffer->size) < 0) {
			MTK_DMX_ERR("populate firmware buffer fail\n");
			return -EFAULT;
		}

		if (!mtk_dmx_load_fw(pfw_buffer->paddr, pfw_buffer->size)) {
			MTK_DMX_ERR("(REE) load TSP firmware fail\n");
			return -EFAULT;
		}

		pvq_buffer = mtkdev->rm.memmap->list[MMAP_BUF_TYPE_VQ];
		if (mtk_dmx_rm_get_buf(&mtkdev->rm, MMAP_BUF_TYPE_VQ,
			pvq_buffer->size, pvq_buffer) < 0) {
			MTK_DMX_ERR("get VQ buf fail\n");
			return -ENOMEM;
		}

		mtk_dmx_set_vq_buffers(pvq_buffer->paddr, pvq_buffer->size, vq_num);
	} else {
		if (mtk_dmx_tee_if_load_fw() < 0) {
			MTK_DMX_ERR("(TEE) load TSP firmware fail\n");
			return -EFAULT;
		}

		if (!mtk_dmx_check_fw_alive()) {
			MTK_DMX_ERR("TSP firmware is NOT alive\n");
			return -EFAULT;
		}

		if (mtk_dmx_tee_if_set_vq() < 0) {
			MTK_DMX_ERR("(TEE) set VQ buffer fail\n");
			return -EFAULT;
		}
	}

	mtk_dmx_enable_vq_buffers(vq_num);

	mtk_dmx_interrupt_ctrl(true);

	return 0;
}

static void _pm_init_ntp(struct mtk_dmx_pm *pm, bool bInitSW)
{
	mtk_dmx_filter_init(bInitSW);
	mtk_dmx_set_ntp_base_point(NTP_BASEPOINT_CNT); // 1 second
	mtk_dmx_set_leap_ind_eq_mode(1, true, false);
	mtk_dmx_set_leap_ind_eq_mode(2, false, true);
}

static void _pm_init_io(struct mtk_dmx_pm *pm)
{
	struct mtk_dmx2dev *mtkdev = container_of(pm, struct mtk_dmx2dev, pm);
	u16 fiq_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_FIQ);
	u16 tso_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_TSO);
	u16 tsif_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_LIVEIN);
	u16 merge_stream_num = mtk_dmx_rm_get_hwcap_by_id(&mtkdev->rm, HWCAPS_TYPE_MERGE_STREAM);
	u16 i, j;
	u8 sync_byte, src_id;

	if (fiq_num > 0) {
		mtk_dmx_io_init_fiq();
		mtk_dmx_io_set_fiq_buf(0, mtkdev->rm.fiqmap.paddr, mtkdev->rm.fiqmap.size);
	}

	if (tso_num > 0)
		mtk_dmx_io_init_tso();

	for (i = 0; i < tsif_num; i++) {
		sync_byte = 0x47;
		src_id = 0;
		for (j = 0; j < merge_stream_num; j++, sync_byte++, src_id++) {
			mtk_dmx_io_sync_byte(i, j, &sync_byte, true);
			mtk_dmx_io_src_id(i, j, &src_id, true);
		}
		// only enable idx = 0
		mtk_dmx_io_sync_byte_en(i, 0, true);
	}
}
#endif

int mtk_dmx_pm_init(struct mtk_dmx_pm *pm, bool bInitSW)
{
	struct mtk_dmx2dev *mtkdev = container_of(pm, struct mtk_dmx2dev, pm);
	struct platform_device *pdev = to_platform_device(mtkdev->dev);

	// SW part
	if (bInitSW)
		if (mtk_dmx_pm_clock_init(pdev, pm) != 0)
			return -ENODEV;

	// HW part
	mtk_dmx_pm_clock_ctrl(pm, true);
#ifndef DMX2LITE
	mtk_dmx_init_hw();

	if (_pm_init_mcu_and_vq(pm) < 0)
		return -EFAULT;

	_pm_init_ntp(pm, bInitSW);
	_pm_init_io(pm);
#endif
	return 0;
}

int mtk_dmx_pm_exit(struct mtk_dmx_pm *pm)
{
	struct mtk_dmx2dev *mtkdev = container_of(pm, struct mtk_dmx2dev, pm);
#ifndef DMX2LITE
	if (!bDmxTeeInited) {
		mtk_dmx_rm_put_buf(&mtkdev->rm, mtkdev->rm.memmap->list[MMAP_BUF_TYPE_FW]);
		mtk_dmx_rm_put_buf(&mtkdev->rm, mtkdev->rm.memmap->list[MMAP_BUF_TYPE_VQ]);
	}
#else
	mtk_dmx_pm_clock_ctrl(pm, false);
#endif
	return 0;
}

#ifndef DMX2LITE
int mtk_dmx_pm_open(struct mtk_dmx_pm *pm)
{
	struct mtk_dmx2dev *dmxdev = container_of(pm, struct mtk_dmx2dev, pm);

	if (pm->bRpmEn)
		return pm_runtime_get_sync(dmxdev->dev);
	else
		return 0;
}

int mtk_dmx_pm_close(struct mtk_dmx_pm *pm)
{
	struct mtk_dmx2dev *dmxdev = container_of(pm, struct mtk_dmx2dev, pm);

	if (pm->bRpmEn)
		return pm_runtime_put_sync(dmxdev->dev);
	else
		return 0;
}
#endif

int mtk_dmx_pm_suspend(bool suspend, void *parg)
{
	struct mtk_dmx2dev *dmxdev = (struct mtk_dmx2dev *) parg;
#ifndef DMX2LITE
	u16 pcr_filter_num = mtk_dmx_rm_get_hwcap_by_id(&dmxdev->rm, HWCAPS_TYPE_PCR_FILTER);
	u16 vq_num = mtk_dmx_rm_get_hwcap_by_id(&dmxdev->rm, HWCAPS_TYPE_VQ);
	int i;

	if (suspend) {
		mtk_dmx_interrupt_ctrl(false);
		mtk_dmx_clear_sw_interrupt_status();
		for (i = 0; i < pcr_filter_num; i++)
			mtk_dmx_clear_pcr_interrupt_status(i);
		mtk_dmx_pm_clock_ctrl(&dmxdev->pm, false);
	} else {
		mtk_dmx_enable_vq_buffers(vq_num);
	}
#else
	if (suspend)
		mtk_dmx_pm_clock_ctrl(&dmxdev->pm, false);
	else
		mtk_dmx_pm_clock_ctrl(&dmxdev->pm, true);
#endif
	return 0;
}

#ifdef DMX2LITE
int mtk_dmx_pm_set_stc_clk(struct mtk_dmx_pm *pm, int id, int rate) {
	struct mtk_dmx2dev *dmxdev = container_of(pm, struct mtk_dmx2dev, pm);
	struct mtk_dmx_rm *rm = &dmxdev->rm;
	u16 stc_num = mtk_dmx_rm_get_hwcap_by_id(rm, HWCAPS_TYPE_STC);

	if (id < 0 || id >= stc_num) {
		MTK_DMX_ERR("Invalid stc id %d\n", id);
		return -EINVAL;
	}

	mtk_dmx_pm_set_rate(pm->stc_synth_clk[id], (rate * MHZ));
	return 0;
}
#endif
