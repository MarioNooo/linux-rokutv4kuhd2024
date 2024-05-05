/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_ATSC3_H
#define MTK_DMX_ATSC3_H

#include "mtk_dmx_drv.h"
#include "mtk_dmx_dvrdrv.h"

#define cast_to_atsc3_filter(filter)	\
	container_of(filter, struct mtk_atsc3_filter, base)

#define cast_to_atsc3_feed(feed)	\
	container_of(feed, struct mtk_atsc3_feed, base)

struct mtk_atsc3_filter {
	struct dmx2_filter base;

	/* derived part */
};

struct mtk_atsc3_feed {
	struct dmx2_feed base;

	/* derived part */
};

struct mtk_atsc3_dmx_private {

};

struct mtk_atsc3_pvr_info {
	int pvr_num;
	int *ref_count;
};

struct mtk_atsc3_dvr_private {
	struct mtk_atsc3_pvr_info pvr_info;
};

int mtk_dmx_atsc3_init(struct mtk_demux2 *demux);
int mtk_dmx_atsc3_exit(struct mtk_demux2 *demux);

int mtk_dvr_atsc3_init(struct mtk_dvr2 *dvr);
int mtk_dvr_atsc3_exit(struct mtk_dvr2 *dvr);

#endif /* MTK_DMX_ATSC3_H */
