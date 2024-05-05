/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_SYSFS_H
#define MTK_DMX_SYSFS_H

struct mtk_dmx2dev;

int mtk_dmx_sysfs_init(struct mtk_dmx2dev *dmxdev);
int mtk_dmx_sysfs_exit(struct mtk_dmx2dev *dmxdev);

#endif /* MTK_DMX_SYSFS_H */
