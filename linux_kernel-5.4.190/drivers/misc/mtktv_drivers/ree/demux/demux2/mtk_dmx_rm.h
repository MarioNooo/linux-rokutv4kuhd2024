/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_RM_H
#define MTK_DMX_RM_H

#include <linux/types.h>

#include <media/dvb_demux2.h>

#include "mtk_dmx_riu.h"

// mmap id
#define FW_MMAP_ID			0
#define VQ_MMAP_ID			1
#define SEC_MMAP_ID			2
#define TSO_SVQ_MMAP_ID		3

// iommu buf id
#define BUFTAG_SHIFT		24
#define DMX_4K_ALIGN		4096

#define VDEC_ES_BUF_ID		2
#define PVR_BUF_ID			16
#define MMFI_BUF_ID			17
#define FQ_BUF_ID			18
#define AESDMA_BUF_ID		48

// dts reg offset
#define MAD0_REG_OFFSET		18
#define MAD1_REG_OFFSET		19

#define FILTER_IDX_INVALID	(-1)
#define PVR_ENG_IDX_INVALID	(-1)

enum irq_type {
	IRQ_TYPE_DMX,
	IRQ_TYPE_MVD_PARSER,
	IRQ_TYPE_MAD_PARSER,
	IRQ_TYPE_MAX
};

enum hwcaps_type {
	HWCAPS_TYPE_PID_FILTER,
	HWCAPS_TYPE_SEC_FILTER,
	HWCAPS_TYPE_PCR_FILTER,
	HWCAPS_TYPE_IP_FILTER,
	HWCAPS_TYPE_NTP_FILTER,
	HWCAPS_TYPE_ALP_FILTER,
	HWCAPS_TYPE_STC,
	HWCAPS_TYPE_VFIFO,
	HWCAPS_TYPE_AFIFO,
	HWCAPS_TYPE_PVR,
	HWCAPS_TYPE_VQ,
	HWCAPS_TYPE_SVQ,
	HWCAPS_TYPE_LIVEIN,
	HWCAPS_TYPE_MERGE_STREAM,
	HWCAPS_TYPE_FILEIN,
	HWCAPS_TYPE_FILEIN_CMDQ,
	HWCAPS_TYPE_MMFI,
	HWCAPS_TYPE_MMFI_FILTER,
	HWCAPS_TYPE_MMFI_CMDQ,
	HWCAPS_TYPE_FIQ,
	HWCAPS_TYPE_TSO,
	HWCAPS_TYPE_TSO_TSIF,
	HWCAPS_TYPE_TLV,
	HWCAPS_TYPE_CILINK,
	HWCAPS_TYPE_ALP,
	HWCPAS_TYPE_CA,
	HWCAPS_TYPE_CK_RATE_START,
	HWCAPS_TYPE_CK_RATE_TSP = HWCAPS_TYPE_CK_RATE_START,
	HWCAPS_TYPE_CK_RATE_PARSER,
	HWCAPS_TYPE_CK_RATE_STC,
	HWCPAS_TYPE_MAX
};

enum mmap_buf_type {
	MMAP_BUF_TYPE_FW,
	MMAP_BUF_TYPE_VQ,
	MMAP_BUF_TYPE_SEC,
	MMAP_BUF_TYPE_PVR,
	MMAP_BUF_TYPE_FILEIN,
	MMAP_BUF_TYPE_FQ,
	MMAP_BUF_TYPE_TSO_SVQ,
	MMAP_BUF_TYPE_AESDMA,
	MMAP_BUF_TYPE_MAX
};

enum mmap_heap_type {
	HEAP_NONE,
	HEAP_IOMMU,
	HEAP_CMA,
	HEAP_NON_MMA,
	HEAP_MAX
};

enum riu_bank_tpye {
	RIU_BANK_TYPE_TS,
	RIU_BANK_TYPE_TS_OUT,
	RIU_BANK_TYPE_TLV,
	RIU_BANK_TYPE_ALP,
	RIU_BANK_TYPE_CILINK,
	RIU_BANK_TYPE_TS_SAMPLE,
	RIU_BANK_TYPE_CLKGEN,
	RIU_BANK_TYPE_MVD_PARSER,
	RIU_BANK_TYPE_MAD_PARSER,
	RIU_BANK_TYPE_MAX
};

enum filter_main_type {
	FILTER_MAIN_TYPE_NONE,
	FILTER_MAIN_TYPE_TS,
	FILTER_MAIN_TYPE_MMTP,
	FILTER_MAIN_TYPE_IP,
	FILTER_MAIN_TYPE_TLV,
	FILTER_MAIN_TYPE_ALP,
	FILTER_MAIN_TPYE_MAX
};

enum filter_sub_type {
	FILTER_SUB_TYPE_MIN = 0,

	FILTER_SUB_TYPE_PID = FILTER_SUB_TYPE_MIN,
	FILTER_SUB_TYPE_SEC,
	FILTER_SUB_TYPE_IP,
	FILTER_SUB_TYPE_PCR,
	FILTER_SUB_TYPE_NTP,
	FILTER_SUB_TYPE_PTP,
	FILTER_SUB_TYPE_ALP,

	FILTER_SUB_TYPE_MAX,
};

enum res_state {
	RES_STATE_FREE,
	RES_STATE_ALLOC,
	RES_STATE_MAX
};

enum pin_control_type {
	PINCTRL_TYPE_CILINK_CLK,
	PINCTRL_TYPE_CILINK_PAD_DATA0,
	PINCTRL_TYPE_CILINK_PAD_DATA1,
	PINCTRL_TYPE_CILINK_DRIVER_CLK,
	PINCTRL_TYPE_CILINK_DRIVER_DATA0,
	PINCTRL_TYPE_EXT2_PMUX,
	PINCTRL_TYPE_EXT1_PMUX,
	PINCTRL_TYPE_EXT_PMUX,
	PINCTRL_TYPE_MAX
};

struct filter_type {
	enum dmx2_main_filter_type main;
	enum filter_sub_type sub;
};

struct hwcaps_entry {
	char *name;
	u16 num;
};

struct dmx_hwcaps {
	struct hwcaps_entry entries[HWCPAS_TYPE_MAX];
};

struct mem_info {
	void __iomem *vaddr;
	u64 paddr;
	u32 size;
	enum mmap_heap_type heaptype;
	int id;
	int dmabuf_fd;
	struct dma_buf *db;
	struct gen_pool *pool;
	unsigned long cma_base;
	unsigned long cma_size;
	char *pool_name;
	u64 offset;
};

struct dmx_memmap_tag {
	char *mmap;
	int buftag_id;
	char *buftag;
};

struct dmx_memmap {
	struct mem_info *list[MMAP_BUF_TYPE_MAX];
};

typedef struct {
	u32 bank_num;
	REG16 __iomem **ptr;
} mtk_dmx_riu_bank;

struct dmx_regmap {
	mtk_dmx_riu_bank banks[RIU_BANK_TYPE_MAX];
};

struct dmx_padcfg {
	struct pinctrl *pin_ctrl;
};

struct pvr_status {
	enum res_state state;
	u16 path_id;
};

struct fifo_status {
	enum res_state state;
};

struct stc_status {
	u16 ref_cnt;
	u16 path_id;
};

struct tso_status {
	enum res_state state;
	u16 path_id;
};

struct filter_status {
	enum res_state state;
};

struct filter {
	bool init;

	spinlock_t lock;
	u16 filter_num;
	struct filter_status *filter_slot_map;
};

struct mtk_dmx_rm {
	struct dmx_hwcaps *hwcap;
	struct dmx_memmap *memmap;
	struct dmx_regmap *regmap;
	struct dmx_padcfg *padcfg;
	struct mem_info fiqmap;
	spinlock_t fifo_lock;
	spinlock_t pvr_lock;
	spinlock_t pid_filter_lock;
	spinlock_t sec_filter_lock;
	spinlock_t pcr_filter_lock;
	spinlock_t ip_filter_lock;
	spinlock_t stc_lock;
	spinlock_t tso_lock;
	struct fifo_status *vfifo_slot_map;
	struct fifo_status *afifo_slot_map;
	struct pvr_status *pvr_eng_slot_map;
	struct filter_status *pid_filter_slot_map;
	struct filter_status *sec_filter_slot_map;
	struct filter_status *pcr_filter_slot_map;
	struct filter_status *ip_filter_slot_map;
	struct stc_status *stc_slot_map;
	struct tso_status *tso_slot_map;

	bool pcr_event_dbg_en;
	bool sec_event_dbg_en;
	bool fiq_rush_en;
	// TODO: Refactor others later
	struct filter filters[FILTER_SUB_TYPE_MAX];
};

int mtk_dmx_rm_init(struct mtk_dmx_rm *res);
int mtk_dmx_rm_exit(struct mtk_dmx_rm *res);

u16 mtk_dmx_rm_get_hwcap_by_name(struct mtk_dmx_rm *res, const char *name);
u16 mtk_dmx_rm_get_hwcap_by_id(struct mtk_dmx_rm *res, u32 id);

int mtk_dmx_rm_get_buf(
	struct mtk_dmx_rm *res,
	enum mmap_buf_type type,
	u32 buf_size,
	struct mem_info *mem_info);
int mtk_dmx_rm_put_buf(struct mtk_dmx_rm *res, struct mem_info *mem_info);

int mtk_dmx_rm_pvr_alloc_engine(struct mtk_dmx_rm *res, u16 path_id);
int mtk_dmx_rm_pvr_free_engine(struct mtk_dmx_rm *res, int eng_id);

int mtk_dmx_rm_get_vfifo_idx(struct mtk_dmx_rm *res);
void mtk_dmx_rm_put_vfifo_idx(struct mtk_dmx_rm *res, int idx);

int mtk_dmx_rm_get_afifo_idx(struct mtk_dmx_rm *res);
void mtk_dmx_rm_put_afifo_idx(struct mtk_dmx_rm *res, int idx);

int mtk_dmx_rm_get_filter_idx(struct mtk_dmx_rm *res, struct filter_type type);
void mtk_dmx_rm_put_filter_idx(struct mtk_dmx_rm *res, struct filter_type type, u16 filter_id);

int mtk_dmx_rm_set_padcfg(struct mtk_dmx_rm *res, enum pin_control_type state);

int mtk_dmx_rm_stc_alloc_engine(struct mtk_dmx_rm *res, u16 path_id);
int mtk_dmx_rm_stc_free_engine(struct mtk_dmx_rm *res, u16 eng_id);

int mtk_dmx_rm_alloc_tso_idx(struct mtk_dmx_rm *res, u16 path_id);
int mtk_dmx_rm_free_tso_idx(struct mtk_dmx_rm *res, u16 path_id);

#endif /* MTK_DMX_RM_H */
