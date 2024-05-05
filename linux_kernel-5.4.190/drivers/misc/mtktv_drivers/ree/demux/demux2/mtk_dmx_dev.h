/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DEMUX_DEV_H
#define MTK_DEMUX_DEV_H

#include <media/dvbdev.h>
#include <media/dmx2dev.h>
#include <media/dvb_ctrls.h>

#include "mtk_dmx_drv.h"
#include "mtk_dmx_dvrdrv.h"

#define dmx2dev_to_path(dev)	\
	container_of(dev, struct mtk_dmx_path, dmx2dev)

#define dumex2_to_path(demux)	\
	container_of(demux, struct mtk_dmx_path, demux2)

#define dvr2_to_path(dvr)	\
	container_of(dvr, struct mtk_dmx_path, dvr2)

#define fh_to_path(fh) \
	({ \
		struct dmx2dev *dmx2dev = fh_to_dmx2dev(fh);	\
		dmx2dev_to_path(dmx2dev);	\
	})

enum dmx_path_state {
	DMX_PATH_STATE_IDLE,
	DMX_PATH_STATE_BUSY,
	DMX_PATH_STATE_PAUSE,
	DMX_PATH_STATE_MAX,
};

struct mtk_dmx_path {
	int path_id;
	enum dmx_path_state state;

	struct dmx2dev dmx2dev;
	struct mtk_demux2 demux2;
	struct mtk_dvr2 dvr2;
};

struct mtk_dmx2dev {
	struct device *dev;
	struct dvb_adapter *adapter;
	struct mtk_dmx_rm rm;
	struct mtk_dmx_tm tm;
	struct mtk_dmx_pm pm;

	struct mtk_dmx_path *paths;
};

int mtk_dmx2dev_init(struct mtk_dmx2dev *dmxdev);
int mtk_dmx2dev_exit(struct mtk_dmx2dev *dmxdev);
int mtk_dmx2dev_suspend(bool suspend, struct mtk_dmx2dev *dmxdev);

// TODO: Refactor it to another module
int mtk_dvr2_fh_ctx_init(
		struct mtk_dvr2_fh_ctx *ctx,
		struct mtk_dvr2 *dvr,
		enum dvr_data_direction dir);
// TODO: Refactor it to another module
int mtk_dvr2_fh_ctx_exit(struct mtk_dvr2_fh_ctx *ctx);

#endif /* MTK_DEMUX_DEV_H */
