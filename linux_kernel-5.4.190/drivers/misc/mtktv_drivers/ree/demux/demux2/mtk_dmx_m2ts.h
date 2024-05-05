/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_M2TS_H
#define MTK_DMX_M2TS_H

#include <linux/workqueue.h>

#include "mtk_dmx_drv.h"
#include "mtk_dmx_dvrdrv.h"

enum M2TS_PACKET_TYPE {
	M2TS_PACKET_TYPE_NORMAL		= 188,
	M2TS_PACKET_TYPE_WITH_STAMP	= 192
};

#define cast_to_m2ts_filter(filter)	\
	((struct mtk_m2ts_filter *)filter)

#define cast_to_m2ts_feed(feed)	\
	((struct mtk_m2ts_feed *)feed)

struct mtk_m2ts_filter {
	struct dmx2_filter base;

	/* derived part */
};

struct mtk_m2ts_feed {
	struct dmx2_feed base;

	/* derived part */
};

struct mtk_m2ts_dmx_private {
	struct workqueue_struct *work_queue;
};

struct mtk_m2ts_dvr_private {
};

int mtk_dmx_m2ts_init(struct mtk_demux2 *demux);
int mtk_dmx_m2ts_exit(struct mtk_demux2 *demux);

int mtk_dvr_m2ts_init(struct mtk_dvr2 *dvr);
int mtk_dvr_m2ts_exit(struct mtk_dvr2 *dvr);

void m2ts_handle_section_transfer(struct dmx2_feed *feed);

#endif /* MTK_DMX_M2TS_H */
