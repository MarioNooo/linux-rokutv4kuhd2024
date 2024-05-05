/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_ISDB3_H
#define MTK_DMX_ISDB3_H

#include "mtk_dmx_drv.h"
#include "mtk_dmx_dvrdrv.h"

#define cast_to_isdb3_filter(filter)	\
	((struct mtk_isdb3_filter *)filter)

#define cast_to_isdb3_feed(feed)	\
	((struct mtk_isdb3_feed *)feed)

struct mtk_isdb3_filter {
	struct dmx2_filter base;

	/* derived part */
};

struct mtk_isdb3_feed {
	struct dmx2_feed base;

	/* derived part */
	struct work_struct work;
};

struct mtk_isdb3_dmx_private {
	struct workqueue_struct *work_queue;
};

struct mtk_isdb3_dvr_private {

};

int mtk_dmx_isdb3_init(struct mtk_demux2 *demux);
int mtk_dmx_isdb3_exit(struct mtk_demux2 *demux);

int mtk_dvr_isdb3_init(struct mtk_dvr2 *dvr);
int mtk_dvr_isdb3_exit(struct mtk_dvr2 *dvr);

#endif /* MTK_DMX_ISDB3_H */
