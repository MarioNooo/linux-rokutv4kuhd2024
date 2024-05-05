// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2020 MediaTek Inc.
 * Author Owen.Tseng <Owen.Tseng@mediatek.com>
 */

#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include "wdt-mt5896-coda.h"
#include "wdt-mt5896-riu.h"
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/of_address.h>

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
#include <linux/sched/debug.h>
#include "mtk-pm.h"
#include <linux/sysrq.h>
#include <linux/of_device.h>
#include <linux/oom.h>
#include <linux/workqueue.h>
#endif

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
#define WDT_MAX_TIMEOUT		60
#define WDT_MIN_TIMEOUT		0
#else
#define WDT_MAX_TIMEOUT		30
#define WDT_MIN_TIMEOUT		1
#endif

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
#define CHECK_PRETIME_FRQ			(1000 * 1000 * 50)
#define DEFAULT_PRETIMEOUT			6
#endif


#define WDT_CLOCK		12000000
#define WDT_CLOCK_MS	12000
#define REG_0024_PM_POR_STATUS(base)	(base + 0x024)
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
// pm boot status 0x7E
#define REG_01F8_PM_POR_STATUS(base)	(base + 0x1f8)
#endif

#define DRV_NAME		"mtktv-wdt"
#define DRV_VERSION		"1.0"
#define WDT_HEART_BEATS_HZ	(HZ/2)
#define STR_LENSIZE		32
#define WDT_WORD_SHIFT	16
#define PMWDT_BASE		2
#define REG_0008_SYS_WDT	0x8
#define REG_0048_PM51_WDT	0x48
#define REG_0088_CM4_WDT	0x88

static bool nowayout = WATCHDOG_NOWAYOUT;
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
static unsigned int wdt_timeout = WDT_MAX_TIMEOUT;
bool is_roku_application(struct task_struct *t);
static bool printk_time = IS_ENABLED(CONFIG_PRINTK_TIME);

static struct workqueue_struct *watchdog_pretimeout_workqueue;
static void watchdog_pretimeout_work_handler(struct work_struct *work);
static DECLARE_WORK(work, watchdog_pretimeout_work_handler);
#else
static unsigned int timeout = WDT_MAX_TIMEOUT;
#endif
bool bwdtenable = true;

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
static __kernel_time_t wdt_expires;
#endif

#ifdef CONFIG_ROKU_SWUPD
static struct watchdog_device* wdt = NULL;
#endif

struct mtktv_wdt_dev {
	struct watchdog_device wdt_dev;
	void __iomem *wdt_base;
	void __iomem *pm_port_base;
	void __iomem *pm_wdt_base;
	int irq_num;
	struct timer_list timer;	/* The timer that pings the watchdog */
	u32 wdt_ping_self;
	u32 wdt_timeout_ms;
	struct kobject *wdt_kobj;
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	unsigned int feed_watchdog_mode;
	struct hrtimer pretimeout_timer;
#endif
};

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
// update the expire time whenever a new timeout is set or when the watchdog is pinged
// so that we can calculate the time left
static inline void set_expire_time(void)
{
    wdt_expires = ktime_to_timespec(ktime_get()).tv_sec + wdt_timeout;
}
#endif

static int mtktv_wdt_restart(struct watchdog_device *wdt_dev,
			     unsigned long action, void *data)
{
	struct mtktv_wdt_dev *mtktv_wdt = watchdog_get_drvdata(wdt_dev);

	while (1) {
		mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0190_L4_MAIN), 1);
		mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0194_L4_MAIN), 0);
		mdelay(5);
	}

	return 0;
}

static int mtktv_wdt_ping(struct watchdog_device *wdt_dev)
{
	struct mtktv_wdt_dev *mtktv_wdt = watchdog_get_drvdata(wdt_dev);

	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0180_L4_MAIN), 1);
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	set_expire_time();
#endif
	return 0;
}

static int mtktv_wdt_set_timeout(struct watchdog_device *wdt_dev,
				  unsigned int timeout)
{
	struct mtktv_wdt_dev *mtktv_wdt = watchdog_get_drvdata(wdt_dev);
	unsigned int wdt_cnt = timeout*WDT_CLOCK;

	wdt_dev->timeout = timeout;
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	wdt_timeout = timeout;
	if (wdt_timeout == 0)
		return 0;
#endif

	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0190_L4_MAIN), wdt_cnt);
	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0194_L4_MAIN), (wdt_cnt >> 16));

	mtk_write_fld((mtktv_wdt->wdt_base + REG_0198_L4_MAIN), 0x1, REG_WDT_ENABLE_1ST);
	mtk_write_fld((mtktv_wdt->wdt_base + REG_0198_L4_MAIN), 0x1, REG_WDT_ENABLE_2ND);
	mtk_write_fld((mtktv_wdt->wdt_base + REG_0198_L4_MAIN), 0x0, REG_WDT_2ND_MAX);

	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0180_L4_MAIN), 1);
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	set_expire_time();
#endif
	return 0;
}

static int mtktv_wdt_set_timeout_ms(struct watchdog_device *wdt_dev,
				  unsigned int timeout)
{
	struct mtktv_wdt_dev *mtktv_wdt = watchdog_get_drvdata(wdt_dev);
	unsigned int wdt_cnt = timeout*WDT_CLOCK_MS;

	mtktv_wdt->wdt_timeout_ms = timeout;
	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0190_L4_MAIN), wdt_cnt);
	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0194_L4_MAIN), (wdt_cnt >> 16));

	mtk_write_fld((mtktv_wdt->wdt_base + REG_0198_L4_MAIN), 0x1, REG_WDT_ENABLE_1ST);
	mtk_write_fld((mtktv_wdt->wdt_base + REG_0198_L4_MAIN), 0x1, REG_WDT_ENABLE_2ND);
	mtk_write_fld((mtktv_wdt->wdt_base + REG_0198_L4_MAIN), 0x0, REG_WDT_2ND_MAX);

	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0180_L4_MAIN), 1);
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	set_expire_time();
#endif
	return 0;
}

static int mtktv_wdt_set_pretimeout(struct watchdog_device *wdt_dev,
				    unsigned int timeout)
{
	struct mtktv_wdt_dev *mtktv_wdt = watchdog_get_drvdata(wdt_dev);
	unsigned int wdt_cnt = timeout*WDT_CLOCK;
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	/* Need more time to dump info */
	if (timeout < DEFAULT_PRETIMEOUT)
		timeout = DEFAULT_PRETIMEOUT;
	wdt_cnt = (wdt_timeout - timeout) * WDT_CLOCK;
#endif

	wdt_dev->pretimeout = timeout;

	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_018C_L4_MAIN), (wdt_cnt >> 16));

	return 0;
}

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
static unsigned int mtktv_wdt_get_timeleft(struct watchdog_device *wdt)
{
	(void) wdt;
	if (wdt_timeout == 0)
		return 0;
	return wdt_expires - ktime_to_timespec(ktime_get()).tv_sec;
}
#endif

static int mtktv_wdt_stop(struct watchdog_device *wdt_dev)
{
	u16 val;
	struct mtktv_wdt_dev *mtktv_wdt = watchdog_get_drvdata(wdt_dev);

	val = mtk_readw_relaxed(REG_0024_PM_POR_STATUS(mtktv_wdt->pm_port_base));
	val |= (BIT(0));
	mtk_writew_relaxed(REG_0024_PM_POR_STATUS(mtktv_wdt->pm_port_base), val);
	mtk_write_fld((mtktv_wdt->wdt_base + REG_0198_L4_MAIN), 0x0, REG_WDT_ENABLE_1ST);
	mtk_write_fld((mtktv_wdt->wdt_base + REG_0198_L4_MAIN), 0x0, REG_WDT_ENABLE_2ND);
	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0190_L4_MAIN), 0);
	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0194_L4_MAIN), 0);
	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_018C_L4_MAIN), 0);

	return 0;
}

static int mtktv_wdt_start(struct watchdog_device *wdt_dev)
{
	u16 val;
	struct mtktv_wdt_dev *mtktv_wdt = watchdog_get_drvdata(wdt_dev);

	val = mtk_readw_relaxed(REG_0024_PM_POR_STATUS(mtktv_wdt->pm_port_base));
	if (bwdtenable)
		val &= ~(BIT(0));
	mtk_writew_relaxed(REG_0024_PM_POR_STATUS(mtktv_wdt->pm_port_base), val);

	return 0;
}

static irqreturn_t mtktv_wdt_isr(int irq, void *wdt_arg)
{
	struct mtktv_wdt_dev *mtktv_wdt = watchdog_get_drvdata(wdt_arg);
	struct device *dev = mtktv_wdt->wdt_dev.parent;

	sysfs_notify(&dev->kobj, "wdt_extend", "wdtwake");
	return IRQ_HANDLED;
}

static const struct watchdog_info mtktv_wdt_info = {
	.identity	= DRV_NAME,
	.options	= WDIOF_SETTIMEOUT |
			  WDIOF_KEEPALIVEPING |
			  WDIOF_MAGICCLOSE,
};

static const struct watchdog_info mtktv_wdt_pretimeout_info = {
	.identity	= DRV_NAME,
	.options	= WDIOF_SETTIMEOUT |
			  WDIOF_KEEPALIVEPING |
			  WDIOF_MAGICCLOSE |
			  WDIOF_PRETIMEOUT,
};

static const struct watchdog_ops mtktv_wdt_ops = {
	.owner		= THIS_MODULE,
	.start		= mtktv_wdt_start,
	.stop		= mtktv_wdt_stop,
	.ping		= mtktv_wdt_ping,
	.set_timeout	= mtktv_wdt_set_timeout,
	.set_pretimeout	= mtktv_wdt_set_pretimeout,
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	.get_timeleft = mtktv_wdt_get_timeleft,
#endif
	.restart	= mtktv_wdt_restart,
};

#ifdef CONFIG_ROKU_SWUPD
void roku_wdt_keepalive(void)
{
	if (!wdt)
		return;
	mtktv_wdt_ping(wdt);
}
EXPORT_SYMBOL(roku_wdt_keepalive);
#endif

static void wdtdev_inter_ping(struct timer_list *t)
{
	struct mtktv_wdt_dev *mtktv_wdt = from_timer(mtktv_wdt, t, timer);

	mtk_writew_relaxed((mtktv_wdt->wdt_base + REG_0180_L4_MAIN), 1);
	mod_timer(&mtktv_wdt->timer, jiffies + WDT_HEART_BEATS_HZ);
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	set_expire_time();
#endif
}

/* wdt sysfs for wakeup reason poll */
static ssize_t wdtwake_show(struct device *dev,
				  struct device_attribute *attr,
				  char *buf)
{
	struct mtktv_wdt_dev *mtktv_wdt = dev_get_drvdata(dev);

	return scnprintf(buf, STR_LENSIZE, "%d\n", mtktv_wdt->wdt_dev.timeout);
}

static DEVICE_ATTR_RO(wdtwake);

/* wdt timeout path */
static ssize_t wdt_ms_timeout_show(struct device *dev,
				  struct device_attribute *attr,
				  char *buf)
{
	struct mtktv_wdt_dev *mtktv_wdt = dev_get_drvdata(dev);
	u32 val;
	u32 time_tmp;

	val = mtk_readw_relaxed(mtktv_wdt->wdt_base + REG_0194_L4_MAIN);
	time_tmp = (val << WDT_WORD_SHIFT) |
	mtk_readw_relaxed(mtktv_wdt->wdt_base + REG_0190_L4_MAIN);
	mtktv_wdt->wdt_timeout_ms = time_tmp / WDT_CLOCK_MS;
	mtktv_wdt->wdt_dev.timeout = time_tmp /WDT_CLOCK;

	return scnprintf(buf, STR_LENSIZE, "%d\n", mtktv_wdt->wdt_timeout_ms);
}

static ssize_t wdt_ms_timeout_store(struct device *dev,
			      struct device_attribute *attr,
			      const char *buf, size_t size)
{
	struct mtktv_wdt_dev *mtktv_wdt = dev_get_drvdata(dev);
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	mtktv_wdt_start(&mtktv_wdt->wdt_dev);
	mtktv_wdt_set_timeout_ms(&mtktv_wdt->wdt_dev, val);

	return ret ? : size;
}

static DEVICE_ATTR_RW(wdt_ms_timeout);

/* wdt off path */
static ssize_t wdtpinself_show(struct device *dev,
				  struct device_attribute *attr,
				  char *buf)
{
	struct mtktv_wdt_dev *mtktv_wdt = dev_get_drvdata(dev);

	return scnprintf(buf, STR_LENSIZE, "%d\n", mtktv_wdt->wdt_dev.timeout);
}

static ssize_t wdtpinself_store(struct device *dev,
			      struct device_attribute *attr,
			      const char *buf, size_t size)
{
	struct mtktv_wdt_dev *mtktv_wdt = dev_get_drvdata(dev);
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	if (val) {
		timer_setup(&mtktv_wdt->timer, wdtdev_inter_ping, 0);
		mod_timer(&mtktv_wdt->timer, jiffies + WDT_HEART_BEATS_HZ);
	} else
		mtktv_wdt_stop(&mtktv_wdt->wdt_dev);

	return ret ? : size;
}

static DEVICE_ATTR_RW(wdtpinself);

static ssize_t wdt_reset_chk_show(struct device *dev,
			   struct device_attribute *attr,
			   char *buf)
{
	struct mtktv_wdt_dev *mtktv_wdt = dev_get_drvdata(dev);
	unsigned int val = 0;

	val = mtk_readw_relaxed(mtktv_wdt->pm_wdt_base + REG_0008_SYS_WDT)&0x1;
	val |= (mtk_readw_relaxed(mtktv_wdt->pm_wdt_base + REG_0048_PM51_WDT)&0x1) << 1;
	val |= (mtk_readw_relaxed(mtktv_wdt->pm_wdt_base + REG_0088_CM4_WDT)&0x1) << PMWDT_BASE;
	if (val&0x1)
		return scnprintf(buf, STR_LENSIZE, "%s\n", "sys-wdt-reset");
	else if ((val >> 1)&0x1)
		return scnprintf(buf, STR_LENSIZE, "%s\n", "pm51-wdt-reset");
	else if ((val >> PMWDT_BASE)&0x1)
		return scnprintf(buf, STR_LENSIZE, "%s\n", "cm4-wdt-reset");
	else
		return scnprintf(buf, STR_LENSIZE, "%s\n", "no-pm-wdt-reset");
}
static DEVICE_ATTR_RO(wdt_reset_chk);

static struct attribute *wdt_extend_attrs[] = {
	&dev_attr_wdtwake.attr,
	&dev_attr_wdtpinself.attr,
	&dev_attr_wdt_ms_timeout.attr,
	&dev_attr_wdt_reset_chk.attr,
	NULL,
};
static const struct attribute_group wdt_extend_attr_group = {
	.name = "wdt_extend",
	.attrs = wdt_extend_attrs
};

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
static unsigned int mtktv_wdt_convert_bootstatus(unsigned int boot_reason)
{
	switch (boot_reason) {
	case PM_BR_AC:
		return WDIOF_POWERUNDER;

	case PM_BR_REBOOT:
		return WDIOF_EXTERN1;

	case PM_BR_REBOOT_RESET_BUTTON:
		return WDIOF_EXTERN2;

	case PM_BR_WATCHDOG:
	default:
		return WDIOF_CARDRESET;
	}
	return WDIOF_CARDRESET;
}

static void dump_tasks(void)
{
	struct task_struct *p;
	struct task_struct *task;

	pr_info("[ pid ]   uid  tgid total_vm      rss name\n");

	rcu_read_lock();
	for_each_process(p) {
		task = find_lock_task_mm(p);
		if (!task)
			continue;

		pr_info("[%5d] %5d %5d %8lu %8lu %s\n",
			task->pid, from_kuid(&init_user_ns, task_uid(task)),
			task->tgid, task->mm->total_vm, get_mm_rss(task->mm),
			task->comm);
			task_unlock(task);
	}
	rcu_read_unlock();
}

static void dump_info(void)
{
	dump_tasks();
	dump_stack();
}

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
extern void normalize_rt_tasks(void);
static void watchdog_pretimeout_work_handler(struct work_struct *work)
{
	// FW-36485
	// do this in a workqueue
	// it calls __sched_setscheduler which can't be called from an interrupt context
	normalize_rt_tasks();
}
#endif

static enum hrtimer_restart mtktv_wdt_pretimeout_timer(struct hrtimer *timer)
{
	struct mtktv_wdt_dev *data = container_of(timer, struct mtktv_wdt_dev, pretimeout_timer);
	unsigned int timeleft;
	static int is_pretimeout;

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	// Send the signal when application doesn't response
	struct task_struct *p;
	bool save_printk_time = printk_time;
	int save_loglevel = console_loglevel;
#endif

	hrtimer_forward_now(timer, ktime_set(0, CHECK_PRETIME_FRQ));
	timeleft = mtktv_wdt_get_timeleft(&data->wdt_dev);
	if (data->wdt_dev.pretimeout && timeleft < data->wdt_dev.pretimeout) {
		if (!is_pretimeout) {
			pr_emerg("Watchdog Timeout Warning\n");
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
			console_loglevel = 0;
			printk_time = false;
			rcu_sysrq_start();

			/* dump scheduler state */
			pr_info("Watchdog Pretimeout: Show State\n");
			show_state();
			show_workqueue_state();

			/* dump memory stats */
			pr_info("Watchdog Pretimeout: Show Memory\n");
			show_mem(0, NULL);

			/* dump uninterruptible tasks */
			pr_info("Watchdog Pretimeout: Show Blocked State\n");
			show_state_filter(TASK_UNINTERRUPTIBLE);
			rcu_sysrq_end();
#endif

			dump_info();
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
			printk_time = save_printk_time;
			console_loglevel = save_loglevel;
			rcu_read_lock();
			for_each_process(p) {
				task_lock(p);
				if (is_roku_application(p)) {
					pr_emerg("killing pid: %d name: %s\n",
					p->pid, p->comm);
					send_sig(SIGABRT, p, 1);
					task_unlock(p);
					break;
				}
				task_unlock(p);
			}
			rcu_read_unlock();
			queue_work(watchdog_pretimeout_workqueue, &work);
#endif
			is_pretimeout = 1;
			pr_emerg("Watchdog Timeout Warning Done\n");
		}
	}

	return HRTIMER_RESTART;
}
#endif // end of CONFIG_ROKU_WATCHDOG_MODIFY

static int mtktv_wdt_probe(struct platform_device *pdev)
{
	struct mtktv_wdt_dev *mtktv_wdt;
	int err;
	int reset_sts;
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	unsigned int boot_sts;
#endif

	mtktv_wdt = devm_kzalloc(&pdev->dev, sizeof(*mtktv_wdt), GFP_KERNEL);
	if (!mtktv_wdt)
		return -ENOMEM;

	platform_set_drvdata(pdev, mtktv_wdt);

	mtktv_wdt->wdt_base = of_iomap(pdev->dev.of_node, 0);
	mtktv_wdt->pm_port_base = of_iomap(pdev->dev.of_node, 1);
	mtktv_wdt->pm_wdt_base = of_iomap(pdev->dev.of_node, PMWDT_BASE);

	if (IS_ERR(mtktv_wdt->wdt_base))
		return PTR_ERR(mtktv_wdt->wdt_base);

	if (IS_ERR(mtktv_wdt->pm_port_base)) {
		dev_err(&pdev->dev, "err pm_port_base=%#lx\n",
			(unsigned long)mtktv_wdt->pm_port_base);
		return PTR_ERR(mtktv_wdt->pm_port_base);
	}

	if (IS_ERR(mtktv_wdt->pm_wdt_base)) {
		dev_err(&pdev->dev, "err pm_wdt_base=%#lx\n",
			(unsigned long)mtktv_wdt->pm_wdt_base);
		return PTR_ERR(mtktv_wdt->pm_wdt_base);
	}

	err = of_property_read_u32(pdev->dev.of_node,
				"wdt-ping-self", &mtktv_wdt->wdt_ping_self);
	if (err) {
		dev_info(&pdev->dev, "could not get pingself resource\n");
		mtktv_wdt->wdt_ping_self = 0;
	}

	mtktv_wdt->wdt_dev.info = &mtktv_wdt_pretimeout_info;

	mtktv_wdt->irq_num = platform_get_irq(pdev, 0);
	if (!devm_request_threaded_irq(&pdev->dev, mtktv_wdt->irq_num,
				NULL,
				mtktv_wdt_isr,
				IRQF_ONESHOT, dev_name(&pdev->dev), &mtktv_wdt->wdt_dev))
		mtktv_wdt->wdt_dev.info = &mtktv_wdt_pretimeout_info;

	mtktv_wdt->wdt_dev.ops = &mtktv_wdt_ops;
	mtktv_wdt->wdt_dev.timeout = WDT_MAX_TIMEOUT;
	mtktv_wdt->wdt_dev.max_timeout = WDT_MAX_TIMEOUT;
	mtktv_wdt->wdt_dev.min_timeout = WDT_MIN_TIMEOUT;
	mtktv_wdt->wdt_dev.parent = &pdev->dev;
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	watchdog_init_timeout(&mtktv_wdt->wdt_dev, wdt_timeout, &pdev->dev);
#else
	watchdog_init_timeout(&mtktv_wdt->wdt_dev, timeout, &pdev->dev);
#endif
	watchdog_set_nowayout(&mtktv_wdt->wdt_dev, nowayout);
	watchdog_set_restart_priority(&mtktv_wdt->wdt_dev, 128);

	reset_sts = mtk_readw_relaxed((mtktv_wdt->wdt_base + REG_0198_L4_MAIN));
	if ((reset_sts) & 1)
		mtktv_wdt->wdt_dev.bootstatus = WDIOF_CARDRESET;
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	/* Set the reboot reason to bootstatus */
	if (mtktv_wdt->wdt_dev.bootstatus != WDIOF_CARDRESET) {
		boot_sts = mtk_readw_relaxed(REG_01F8_PM_POR_STATUS(mtktv_wdt->pm_port_base));
		mtktv_wdt->wdt_dev.bootstatus = mtktv_wdt_convert_bootstatus(boot_sts);
	}
#endif
	watchdog_set_drvdata(&mtktv_wdt->wdt_dev, mtktv_wdt);
	if (mtktv_wdt->wdt_ping_self == 1) {
		timer_setup(&mtktv_wdt->timer, wdtdev_inter_ping, 0);
		mod_timer(&mtktv_wdt->timer, jiffies + WDT_HEART_BEATS_HZ);
		mtktv_wdt_start(&mtktv_wdt->wdt_dev);
	}

	err = watchdog_register_device(&mtktv_wdt->wdt_dev);
	if (unlikely(err))
		return err;

	err = sysfs_create_group(&pdev->dev.kobj, &wdt_extend_attr_group);
	if (err < 0)
		dev_err(&pdev->dev, "wdt sysfs_create_group failed: %d\n", err);

	dev_info(&pdev->dev, "Watchdog enabled (timeout=%d sec, nowayout=%d)\n",
			mtktv_wdt->wdt_dev.timeout, nowayout);

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	/* Setup a hrtimer */
	hrtimer_init(&mtktv_wdt->pretimeout_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	mtktv_wdt->pretimeout_timer.function = mtktv_wdt_pretimeout_timer;
	hrtimer_start(&mtktv_wdt->pretimeout_timer, ktime_set(0, CHECK_PRETIME_FRQ), HRTIMER_MODE_REL);
	mtktv_wdt->wdt_dev.pretimeout = DEFAULT_PRETIMEOUT;
#endif

#ifdef CONFIG_ROKU_SWUPD
	wdt = &mtktv_wdt->wdt_dev;
#endif
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	watchdog_pretimeout_workqueue = alloc_workqueue("watchdog_pretimeout", WQ_HIGHPRI, 0);
#endif
	return 0;
}

static void mtktv_wdt_shutdown(struct platform_device *pdev)
{
	struct mtktv_wdt_dev *mtktv_wdt = platform_get_drvdata(pdev);

	if (watchdog_active(&mtktv_wdt->wdt_dev))
		mtktv_wdt_stop(&mtktv_wdt->wdt_dev);
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	hrtimer_cancel(&mtktv_wdt->pretimeout_timer);
#endif
}

static int mtktv_wdt_remove(struct platform_device *pdev)
{
	struct mtktv_wdt_dev *mtktv_wdt = platform_get_drvdata(pdev);

	watchdog_unregister_device(&mtktv_wdt->wdt_dev);
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	if (watchdog_pretimeout_workqueue)
		destroy_workqueue(watchdog_pretimeout_workqueue);
	watchdog_pretimeout_workqueue = NULL;
#endif
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int mtktv_wdt_suspend(struct device *dev)
{
	struct mtktv_wdt_dev *mtktv_wdt = dev_get_drvdata(dev);

	if (watchdog_active(&mtktv_wdt->wdt_dev))
		mtktv_wdt_stop(&mtktv_wdt->wdt_dev);
#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	hrtimer_cancel(&mtktv_wdt->pretimeout_timer);
#endif
	return 0;
}

static int mtktv_wdt_resume(struct device *dev)
{
	struct mtktv_wdt_dev *mtktv_wdt = dev_get_drvdata(dev);

	if (watchdog_active(&mtktv_wdt->wdt_dev)) {
		mtktv_wdt_start(&mtktv_wdt->wdt_dev);
		mtktv_wdt_set_timeout(&mtktv_wdt->wdt_dev, mtktv_wdt->wdt_dev.timeout);
	}

#ifdef CONFIG_ROKU_WATCHDOG_MODIFY
	hrtimer_start(&mtktv_wdt->pretimeout_timer, ktime_set(0, CHECK_PRETIME_FRQ),
				HRTIMER_MODE_REL);
#endif

	return 0;
}
#endif

static const struct of_device_id mtktv_wdt_dt_ids[] = {
	{ .compatible = "mediatek,mt5896-wdt" },
	{},
};
MODULE_DEVICE_TABLE(of, mtktv_wdt_dt_ids);

static const struct dev_pm_ops mtktv_wdt_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(mtktv_wdt_suspend,
				mtktv_wdt_resume)
};

static struct platform_driver mtktv_wdt_driver = {
	.probe		= mtktv_wdt_probe,
	.remove		= mtktv_wdt_remove,
	.shutdown	= mtktv_wdt_shutdown,
	.driver		= {
		.name		= DRV_NAME,
		.pm		= &mtktv_wdt_pm_ops,
		.of_match_table	= mtktv_wdt_dt_ids,
	},
};

module_platform_driver(mtktv_wdt_driver);

module_param_named(wdt_enable, bwdtenable, bool, 0444);
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default="
			__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Owen Tseng");
MODULE_DESCRIPTION("Mediatek WatchDog Timer Driver");
MODULE_VERSION(DRV_VERSION);
