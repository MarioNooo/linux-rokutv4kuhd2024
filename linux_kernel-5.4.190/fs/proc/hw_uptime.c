/*
 * linux/fs/proc/hw_uptime.c
 *
 * Copyright (C) 2022-2023 Roku Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/kernel.h>
#include <linux/clocksource.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <linux/kernel_stat.h>
#include <roku_common_defs.h>

static unsigned int get_uptime_offset(void)
{
    // The hardware uptime when the kernel uptime is 0. Used to calculate
    // hardware uptime from kernel uptime.
    // unit is ms
    static unsigned int uptime_offset = 0;
    static bool inited = false;
    if (inited) {
        return uptime_offset;
    }
    __roku_boot_resume_kpi kpi;
    void *io_addr = (void *)(ROKU_SHARE_MEM_GET_BOOT_KPI_TIMER_DATA);
    void __iomem *iomem_kpi;
    iomem_kpi = ioremap((unsigned long)io_addr, sizeof(__roku_boot_resume_kpi));
    if (!iomem_kpi) {
        inited = true;
        pr_err("[%s] hw_uptime failed to fetch kpi\n", __func__);
        return 0;
    }
    memcpy_fromio((void *)&kpi, (void *)iomem_kpi, (unsigned int)sizeof(__roku_boot_resume_kpi));
    iounmap(iomem_kpi);
    uptime_offset = kpi.kernel_start;
    inited = true;
    return kpi.kernel_start;
}

static int hw_uptime_proc_show(struct seq_file *m, void *v)
{
	struct timespec64 uptime;
	ktime_get_boottime_ts64(&uptime);
	timespec64_add_ns(&uptime, get_uptime_offset() * NSEC_PER_MSEC);
	if (uptime.tv_sec > 0) {
		seq_printf(m, "%lu%03lu\n",
			   (unsigned long) uptime.tv_sec,
			   (uptime.tv_nsec / (NSEC_PER_SEC / 1000)));
	} else {
		seq_printf(m, "%lu\n",
			   (uptime.tv_nsec / (NSEC_PER_SEC / 1000)));
	}
	return 0;
}

unsigned int get_hw_uptime(void)
{
	return ktime_to_ms(ktime_get()) + get_uptime_offset();
}
EXPORT_SYMBOL(get_hw_uptime);

static int hw_uptime_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, hw_uptime_proc_show, NULL);
}

static const struct file_operations hw_uptime_proc_fops = {
	.open		= hw_uptime_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init hw_proc_uptime_init(void)
{
	proc_create("hw_uptime", 0, NULL, &hw_uptime_proc_fops);
	return 0;
}

fs_initcall(hw_proc_uptime_init);
