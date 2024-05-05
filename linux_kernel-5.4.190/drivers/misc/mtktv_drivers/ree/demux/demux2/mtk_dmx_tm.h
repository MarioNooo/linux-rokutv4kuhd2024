/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_TM_H
#define MTK_DMX_TM_H

#include <linux/spinlock.h>

#define TSP_FW_NAME			"fwTSP.bin"
#define SCMB_MON_TIMEOUT	(20)
#define PCR_INT_MON_TIMEOUT		(10)
#define MAD_INT_ID	(2)

typedef int (*timer_fn_t)(void *data);

enum sw_interrupt_event {
	SW_INT_EVENT_SEC_READY,
	SW_INT_EVENT_SEC_CRC_ERR,
	SW_INT_EVENT_SEC_BUF_OVERFLOW,
	SW_INT_EVENT_MAX,
	SW_INT_EVENT_NULL = -1
};

enum tsp_irq_type {
	TSP_IRQ_NONE,
	TSP_IRQ_PCR,
	TSP_IRQ_SEC,
	TSP_IRQ_NTP
};

struct tsp_irq_pcr {
	u16 idx;
};

struct tsp_irq_sec {
	u16 idx;
	enum sw_interrupt_event event;
};

struct tsp_irq_ntp {
	u16 path;
};

struct tsp_irq_args {
	enum tsp_irq_type type;
	union {
		struct tsp_irq_pcr pcr;
		struct tsp_irq_sec sec;
		struct tsp_irq_ntp ntp;
	};
	struct list_head list;
	int uniq;
};

struct mtk_dmx_tm {
	int irq;
	int pas_irq;
	int mad_irq;
	int mad1_irq;
	int mad2_irq;
	int mad3_irq;
	int mad4_irq;
	struct device *dev;
	struct mtk_dmx_rm *rm;
	rwlock_t fh_lock;
	spinlock_t irq_lock;
	struct list_head irq_list_head;
	atomic_t irq_uniq;
};

int mtk_dmx_tm_pvr_rec_task(void *data);
int mtk_dmx_tm_pvr_feed_task(void *data);
int mtk_dmx_tm_scmb_monitor(void *data);
int mtk_dmx_tm_pcr_int_monitor(void *data);

int mtk_dmx_tm_init(struct device *dev, struct mtk_dmx_tm *tm);
int mtk_dmx_tm_exit(void);

#endif				/* MTK_DMX_TM_H */

