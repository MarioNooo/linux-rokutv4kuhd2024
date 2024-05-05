// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

#include "mtk_dmx_common.h"
#include "mtk_dmx_riu.h"
#include "mtk_dmx_rm.h"
#include "mtk_dmx_pm.h"
#include "mtk_dmx_dev.h"

#ifndef DMX2LITE
#include "mtk_dscmb.h"
#include "mtk_scrambler.h"
#else
static struct device *__dev = NULL;
int mtk_dmx_runtime_cmd(int cmd, void *args);
#endif

static int mtk_dmx_probe(struct platform_device *pdev)
{
	struct mtk_dmx2dev *dmx_dev;
	unsigned long start_time = jiffies;
	int ret;

	dmx_dev = devm_kzalloc(&pdev->dev, sizeof(struct mtk_dmx2dev), GFP_KERNEL);
	if (!dmx_dev) {
		MTK_DMX_ERR("allocate dmxdev fail\n");
		return -ENOMEM;
	}

	dmx_dev->dev = &pdev->dev;

#ifndef DMX2LITE
	if (mtk_dmx2dev_init(dmx_dev) < 0) {
		MTK_DMX_ERR("init dmxdev fail\n");
		return -EFAULT;
	}

	if (mdrv_dscmb_dev_init(pdev) < 0) {
		MTK_DMX_ERR("init dscmb fail\n");
			return -EFAULT;
	}

	if (mdrv_scrambler_dev_init(pdev) < 0) {
		MTK_DMX_ERR("init scrambler fail\n");
		return -EFAULT;
	}
#else
	__dev = &pdev->dev;
	ret = mtk_dmx_rm_init(&dmx_dev->rm);
	if (ret < 0) {
		MTK_DMX_ERR("resource manager init fail\n");
		return -EFAULT;
	}
	ret = mtk_dmx_pm_init(&dmx_dev->pm, true);
	if (ret < 0) {
		MTK_DMX_ERR("power manager init fail\n");
		return -EFAULT;
	}
#endif
	platform_set_drvdata(pdev, dmx_dev);
#ifndef DMX2LITE
	if (dmx_dev->pm.bRpmEn) {
		pm_runtime_set_active(&pdev->dev);
		pm_runtime_enable(&pdev->dev);
		MTK_DMX_INFO("runtime pm is enabled\n");
	} else
		MTK_DMX_INFO("runtime pm is disabled\n");
#else
	mtk_dmx_runtime_cmd(0, NULL);
	MTK_DMX_INFO("runtime pm is enabled\n");
	MTK_DMX_INFO("mtk_dmx_runtime_cmd(0, NULL)\n");

#endif
	MTK_DMX_INFO("cost %d msec\n", jiffies_to_msecs(jiffies - start_time));
	return 0;
}

static int mtk_dmx_remove(struct platform_device *pdev)
{
	struct mtk_dmx2dev *dmx_dev = platform_get_drvdata(pdev);
	int ret;

#ifndef DMX2LITE
	if (mtk_dmx2dev_exit(dmx_dev) < 0) {
		MTK_DMX_ERR("release dmxdev fail\n");
		return -EFAULT;
	}
#else
	ret = mtk_dmx_pm_exit(&dmx_dev->pm);
	if (ret < 0) {
		MTK_DMX_ERR("clock manager exit fail\n");
		return -EFAULT;
	}
#endif
	return 0;
}

static int mtk_dmx_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct mtk_dmx2dev *dmx_dev = platform_get_drvdata(pdev);
#ifndef DMX2LITE
	if (mtk_dmx2dev_suspend(true, dmx_dev) < 0) {
		MTK_DMX_ERR("suspend dmxdev fail\n");
		return -EFAULT;
	}
#else
	mtk_dmx_pm_suspend(true, dmx_dev);
#endif
	return 0;
}

static int mtk_dmx_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct mtk_dmx2dev *dmx_dev = platform_get_drvdata(pdev);
#ifndef DMX2LITE
	if (mtk_dmx2dev_suspend(false, dmx_dev) < 0) {
		MTK_DMX_ERR("resume dmxdev fail\n");
		return -EFAULT;
	}

	if (dmx_dev->pm.on == false)
		mtk_dmx_pm_runtime(&dmx_dev->pm, MTK_DMX_PM_STATUS_SUSPEND, false);
#else
	mtk_dmx_pm_suspend(false, dmx_dev);
#endif
	return 0;
}

#ifndef DMX2LITE
static int mtk_dmx_runtime_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct mtk_dmx2dev *dmx_dev = platform_get_drvdata(pdev);

	mtk_dmx_pm_runtime(&dmx_dev->pm, MTK_DMX_PM_STATUS_SUSPEND, true);
	return 0;
}

static int mtk_dmx_runtime_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct mtk_dmx2dev *dmx_dev = platform_get_drvdata(pdev);

	mtk_dmx_pm_runtime(&dmx_dev->pm, MTK_DMX_PM_STATUS_RESUME, true);
	return 0;
}
#else
int mtk_dmx_runtime_cmd(int cmd, void *args)
{
	struct mtk_dmx2dev *dmx_dev;

	if (__dev == NULL)
		return -EFAULT;
	dmx_dev = platform_get_drvdata(to_platform_device(__dev));
	switch (cmd) {
	case 0: // clock off
		mtk_dmx_pm_runtime(&dmx_dev->pm, MTK_DMX_PM_STATUS_SUSPEND, true);
		break;
	case 1: // clock on
		mtk_dmx_pm_runtime(&dmx_dev->pm, MTK_DMX_PM_STATUS_RESUME, true);
		break;
	case 2: // stc clock adjustment
		{
			int id = *(int*)args >> 16;
			int rate = *(int*)args & 0xFFFF;
			mtk_dmx_pm_set_stc_clk(&dmx_dev->pm, id, rate);
		}
		break;
	default:
		return -EPERM;
	}
	return 0;
}
EXPORT_SYMBOL(mtk_dmx_runtime_cmd);
#endif

static const struct dev_pm_ops mtk_dmx_pm_ops = {
#ifndef DMX2LITE
	SET_RUNTIME_PM_OPS(mtk_dmx_runtime_suspend,
						mtk_dmx_runtime_resume,
						NULL),
	.suspend = mtk_dmx_suspend,
	.resume = mtk_dmx_resume
#else
	.suspend_late = mtk_dmx_suspend,
	.resume_early = mtk_dmx_resume
#endif
};

static const struct of_device_id mtk_dmx_dt_match[] = {
	{.compatible = DRV2_NAME},
	{},
};

MODULE_DEVICE_TABLE(of, mtk_dmx_dt_match);

static struct platform_driver mtk_dmx_drv = {
	.probe = mtk_dmx_probe,
	.remove = mtk_dmx_remove,
	.driver = {
		.name = DRV2_NAME,
		.owner = THIS_MODULE,
		.of_match_table = mtk_dmx_dt_match,
		.pm = &mtk_dmx_pm_ops,
	}
};

module_platform_driver(mtk_dmx_drv);

MODULE_DESCRIPTION("Mediatek DVB Demux2 Driver");
MODULE_AUTHOR("Mediatek Author");
MODULE_LICENSE("GPL");
