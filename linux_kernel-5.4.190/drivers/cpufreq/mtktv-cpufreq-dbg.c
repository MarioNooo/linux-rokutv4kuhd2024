// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ":[%s:%d] " fmt, __func__, __LINE__

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/pm_domain.h>
#include <linux/pm_runtime.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kobject.h>
#include <linux/suspend.h>
#include <linux/cpu.h>
#include <linux/cpu_cooling.h>
#include <linux/cpufreq.h>
#include <linux/cpumask.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>

#include "mtktv-cpufreq.h"
#include "mtktv-cpufreq-dbg.h"

#define MICRO (1000)
/* for procfs used */
static struct proc_dir_entry *mtktv_cpufreq_dir;
static atomic_t mtktv_core_power_voltage_op_is_open = ATOMIC_INIT(0);
static atomic_t mtktv_voltage_op_is_open = ATOMIC_INIT(0);
static atomic_t mtktv_frequency_op_is_open = ATOMIC_INIT(0);
static atomic_t mtktv_temperature_op_is_open = ATOMIC_INIT(0);
static atomic_t mtktv_corner_op_is_open = ATOMIC_INIT(0);
static struct regulator *core_power_regulator;

/* debug corner type */
#define CLK_CORNER_SS (0)
#define CLK_CORNER_FF (1)
static u32 corner_type = 0xFFFFFFFF;
/*********************************************************************
 *                          PROCFS DEBUG                             *
 *********************************************************************/
/* core power voltage operation */
ssize_t mtktv_core_power_voltage_op_write(struct file *file,
		const char __user *buf,
		size_t count, loff_t *ppos)
{
	unsigned int set_voltage = 0;
	char buffer[MAX_BUFFER_SIZE];

	if (!count)
		return count;

	if (count >= MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE-1;

	if (copy_from_user(buffer, buf, count))
		return -EFAULT;

	buffer[count] = '\0';

	if (kstrtouint(buffer, 0, &set_voltage) == 0) {
		regulator_set_voltage(core_power_regulator, set_voltage*MICRO, set_voltage*MICRO);
		return count;
	}

	return -EFAULT;
}

static int mtktv_core_power_voltage_op_show(struct seq_file *s, void *v)
{
	unsigned int  vol = 0;
	unsigned int  j = 0, total = 0;

	if (core_power_regulator != NULL) {
		total = 0;

		for (j = 0; j < SAMPLE_TIMES; j++)
			total += regulator_get_voltage(core_power_regulator);

		vol = total/SAMPLE_TIMES;
		seq_printf(s, "core_power:%d\n", vol/MICRO); // mV
	}

	return 0;
}

static int mtktv_core_power_voltage_op_open(struct inode *inode, struct file *file)
{
	if (atomic_read(&mtktv_core_power_voltage_op_is_open))
		return -EACCES;

	atomic_set(&mtktv_core_power_voltage_op_is_open, 1);
	return single_open(file, &mtktv_core_power_voltage_op_show, NULL);
}

static int mtktv_core_power_voltage_op_release(struct inode *inode, struct file *file)
{
	WARN_ON(!atomic_read(&mtktv_core_power_voltage_op_is_open));
	atomic_set(&mtktv_core_power_voltage_op_is_open, 0);
	return single_release(inode, file);
}

/* end of core power voltage operation */


/* voltage operation */
ssize_t mtktv_voltage_op_write(struct file *file,
		const char __user *buf,
		size_t count, loff_t *ppos)
{
	unsigned int set_voltage = 0, set_cpu = 0, temp = 0;
	char buffer[MAX_BUFFER_SIZE];
	struct mtktv_cpu_dvfs_info *info;

	if (!count)
		return count;

	if (count >= MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE-1;

	if (copy_from_user(buffer, buf, count))
		return -EFAULT;

	buffer[count] = '\0';

	if (!auto_measurement)
		return count;

	if (sscanf(buffer, "%d %d %d", &set_cpu, &set_voltage, &temp) == 3) {
		return -EFAULT;
	} else if (sscanf(buffer, "%d %d", &set_cpu, &set_voltage) == 2) {
		if (set_cpu >= CONFIG_NR_CPUS)
			return -EINVAL;

		/* set_voltage: XXX0mV */
		info = mtktv_cpu_dvfs_info_lookup(set_cpu);
		mtktv_cpufreq_set_voltage(info, set_voltage*MICRO);
		return count;
	}

	return -EFAULT;
}

static int mtktv_voltage_op_show(struct seq_file *s, void *v)
{
	unsigned int  vol = 0;
	unsigned int  i = 0, j = 0, total = 0;
	struct mtktv_cpu_dvfs_info *info;

#if CPU_HOTPLUG == 1
	for_each_online_cpu(i) {
		info = mtktv_cpu_dvfs_info_lookup(i);
		if (info == NULL) {
			seq_printf(s, "CPU_%d: not found\n", i);
			continue;
		}

		total = 0;

		for (j = 0; j < SAMPLE_TIMES; j++)
			total += regulator_get_voltage(info->reg);

		vol = total/SAMPLE_TIMES;
		seq_printf(s, "CPU_%d_vol:%d\n", i, vol/MICRO); // mV
	}
#else
		info = mtktv_cpu_dvfs_info_lookup(i);
		if (info == NULL) {
			seq_printf(s, "CPU_%d: not found\n", i);
			return 0;
		}

		total = 0;

		for (j = 0; j < SAMPLE_TIMES; j++)
			total += regulator_get_voltage(info->reg);

		vol = total/SAMPLE_TIMES;
		seq_printf(s, "CPU_%d_vol:%d\n", i, vol/MICRO); // mV

#endif
	return 0;
}

static int mtktv_voltage_op_open(struct inode *inode, struct file *file)
{
	if (atomic_read(&mtktv_voltage_op_is_open))
		return -EACCES;

	atomic_set(&mtktv_voltage_op_is_open, 1);
	return single_open(file, &mtktv_voltage_op_show, NULL);
}

static int mtktv_voltage_op_release(struct inode *inode, struct file *file)
{
	WARN_ON(!atomic_read(&mtktv_voltage_op_is_open));
	atomic_set(&mtktv_voltage_op_is_open, 0);
	return single_release(inode, file);
}

/* end of voltage operation */

/* frequency operation */
ssize_t mtktv_frequency_op_write(struct file *file,
		const char __user *buf,
		size_t count, loff_t *ppos)
{
	char buffer[MAX_BUFFER_SIZE];
	unsigned int set_cpu = 0, set_frequency = 0, temp = 0;

	if (!count)
		return count;

	if (count >= MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE-1;

	if (copy_from_user(buffer, buf, count))
		return -EFAULT;

	buffer[count] = '\0';

	if (sscanf(buffer, "%d %d %d", &set_cpu, &set_frequency, &temp) == 3) {
		return -EFAULT;
	} else if (sscanf(buffer, "%d %d", &set_cpu, &set_frequency) == 2) {
		if (set_cpu >= CONFIG_NR_CPUS)
			return -EINVAL;

		/* set_frequency: XXXX MHz */
		mtktv_cpufreq_set_clk(mtktv_cpufreq_get_clk(set_cpu),
				set_frequency * MICRO);

		return count;
	}
	return -EFAULT;
}

static int mtktv_frequency_op_show(struct seq_file *s, void *v)
{
	u32 clock = 0, i = 0;

#if CPU_HOTPLUG == 1
	for_each_online_cpu(i) {
		clock = mtktv_cpufreq_get_clk(i);
		seq_printf(s, "CPU_%d_freq:%u\n", i, clock/MICRO);
	}
#else
		clock = mtktv_cpufreq_get_clk(i);
		seq_printf(s, "CPU_%d_freq:%u\n", i, clock/MICRO);
#endif
	return 0;
}

static int mtktv_frequency_op_open(struct inode *inode, struct file *file)
{
	if (atomic_read(&mtktv_frequency_op_is_open))
		return -EACCES;
	atomic_set(&mtktv_frequency_op_is_open, 1);
	return single_open(file, &mtktv_frequency_op_show, NULL);
}

static int mtktv_frequency_op_release(struct inode *inode, struct file *file)
{
	WARN_ON(!atomic_read(&mtktv_frequency_op_is_open));
	atomic_set(&mtktv_frequency_op_is_open, 0);
	return single_release(inode, file);
}

/* end of frequency operation */

/* temperature operation */
ssize_t mtktv_temperature_op_write(struct file *file,
		const char __user *buf,
		size_t count, loff_t *ppos)
{
	unsigned int set_temperature_offset = 0;
	char buffer[MAX_BUFFER_SIZE];
	int res = 0;

	if (!count)
		return count;

	if (count >= MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE-1;

	if (copy_from_user(buffer, buf, count))
		return -EFAULT;

	buffer[count] = '\0';

	res = kstrtoint(buffer, 10, &set_temperature_offset);
	if (res == 0)
		temperature_offset = set_temperature_offset;

	return count;
}

extern int get_cpu_temperature_bysar(void);
static int mtktv_temperature_op_show(struct seq_file *s, void *v)
{
	s32 dwTempData = 0;
	u32 i = 0;

#if CPU_HOTPLUG == 1
	for_each_online_cpu(i) {
		dwTempData = get_cpu_temperature_bysar();
		if (temperature_offset == 0)
			seq_printf(s, "CPU_%d_temp:%d\n", i, dwTempData);
		else
			seq_printf(s, "CPU_%d_temp:%d (offset: %d)\n", i,
					dwTempData, temperature_offset);
	}
#else
		dwTempData = get_cpu_temperature_bysar();
		if (temperature_offset == 0)
			seq_printf(s, "CPU_%d_temp:%d\n", i, dwTempData);
		else
			seq_printf(s, "CPU_%d_temp:%d (offset: %d)\n", i,
					dwTempData, temperature_offset);
#endif
	return 0;
}

static int mtktv_temperature_op_open(struct inode *inode, struct file *file)
{
	if (atomic_read(&mtktv_temperature_op_is_open))
		return -EACCES;
	atomic_set(&mtktv_temperature_op_is_open, 1);
	return single_open(file, &mtktv_temperature_op_show, NULL);
}

static int mtktv_temperature_op_release(struct inode *inode,
		struct file *file)
{
	WARN_ON(!atomic_read(&mtktv_temperature_op_is_open));
	atomic_set(&mtktv_temperature_op_is_open, 0);
	return single_release(inode, file);
}
/* end of temperature operation */

static const struct file_operations mtktv_core_power_voltage_fileops = {
	.owner      = THIS_MODULE,
	.open       = mtktv_core_power_voltage_op_open,
	.write      = mtktv_core_power_voltage_op_write,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = mtktv_core_power_voltage_op_release,
};

/* corner operation */
extern __weak u32 MTKTV_CPUFREQ_TEEC_Simulate_Corner(u32 corner_type);
ssize_t mtktv_corner_op_write(struct file *file,
		const char __user *buf,
		size_t count, loff_t *ppos)
{
	char buffer[MAX_BUFFER_SIZE];
	unsigned int ret = 0;

	if (!count)
		return count;

	if (count >= MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE-1;

	if (copy_from_user(buffer, buf, count))
		return -EFAULT;

	buffer[count] = '\0';

	if(0 == strncmp(buffer, "fast", strlen(buffer) - 1))
	{
		corner_type = CLK_CORNER_FF;
	}else if(0 == strncmp(buffer, "slow", strlen(buffer) -1))
	{
		corner_type = CLK_CORNER_SS;
	}else{
		pr_err("simulate core corner bad input\n");
		return -EFAULT;
	}

	pr_info("Set Simulate Corner Speed: %s Corner\n", corner_type ? "FAST":"SLOW");

	ret = MTKTV_CPUFREQ_TEEC_Simulate_Corner(corner_type);

	if(ret != 0)
	{
		pr_err("simulate core corner fail\n");
		return -EFAULT;
	}

	return count;
}

static int mtktv_corner_op_show(struct seq_file *s, void *v)
{
	if(corner_type != 0xFFFFFFFF)
	seq_printf(s, "Simulate Corner Speed: %s Corner\n", corner_type ? "FAST":"SLOW");
	else
	seq_printf(s, "Simulate Corner haven't set\n");

	return 0;
}

static int mtktv_corner_op_open(struct inode *inode, struct file *file)
{
	if (atomic_read(&mtktv_corner_op_is_open))
		return -EACCES;
	atomic_set(&mtktv_corner_op_is_open, 1);
	return single_open(file, &mtktv_corner_op_show, NULL);
}

static int mtktv_corner_op_release(struct inode *inode, struct file *file)
{
	WARN_ON(!atomic_read(&mtktv_corner_op_is_open));
	atomic_set(&mtktv_corner_op_is_open, 0);
	return single_release(inode, file);
}

/* end of corner operation */
static const struct file_operations mtktv_voltage_fileops = {
	.owner      = THIS_MODULE,
	.open       = mtktv_voltage_op_open,
	.write      = mtktv_voltage_op_write,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = mtktv_voltage_op_release,
};

static const struct file_operations mtktv_frequency_fileops = {
	.owner      = THIS_MODULE,
	.open       = mtktv_frequency_op_open,
	.write      = mtktv_frequency_op_write,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = mtktv_frequency_op_release,
};

static const struct file_operations mtktv_temperature_fileops = {
	.owner      = THIS_MODULE,
	.open       = mtktv_temperature_op_open,
	.write      = mtktv_temperature_op_write,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = mtktv_temperature_op_release,
};

static const struct file_operations mtktv_corner_fileops = {
	.owner      = THIS_MODULE,
	.open       = mtktv_corner_op_open,
	.write      = mtktv_corner_op_write,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = mtktv_corner_op_release,
};

static int mtvtv_cpufreq_procfs_init(void)
{
	struct proc_dir_entry *entry;

	mtktv_cpufreq_dir = proc_mkdir("mtk_dvfs", NULL);

	if (!mtktv_cpufreq_dir) {
		pr_err("mtk_dvfs procfs initialization fail\n");
		return -ENOMEM;
	}

	entry = proc_create("voltage", 0644, mtktv_cpufreq_dir,
					&mtktv_voltage_fileops);
	if (!mtktv_cpufreq_dir) {
		pr_err("voltage procfs initialization fail\n");
		return -ENOMEM;
	}

	core_power_regulator = regulator_get(NULL, "mtk-core-power-regulator");
	if (IS_ERR(core_power_regulator)) {
		if (PTR_ERR(core_power_regulator) == -EPROBE_DEFER)
			pr_err("proc regulator\n"
					"for core power not ready\n"
					"retry.\n");
		else
			pr_err("failed to\n"
					"get proc regulator\n"
					"for core power\n");
	} else {
		entry = proc_create("core_power_voltage", 0600, mtktv_cpufreq_dir,
						&mtktv_core_power_voltage_fileops);
		if (!mtktv_cpufreq_dir) {
			pr_err("voltage procfs initialization fail\n");
			return -ENOMEM;
		}
	}

	entry = proc_create("frequency", 0644, mtktv_cpufreq_dir,
					&mtktv_frequency_fileops);
	if (!mtktv_cpufreq_dir) {
		pr_err("frequency procfs initialization fail\n");
		return -ENOMEM;
	}

	entry = proc_create("temperature", 0644, mtktv_cpufreq_dir,
					&mtktv_temperature_fileops);
	if (!mtktv_cpufreq_dir) {
		pr_err("temperature procfs initialization fail\n");
		return -ENOMEM;
	}

	entry = proc_create("corner", 0644, mtktv_cpufreq_dir,
					&mtktv_corner_fileops);
	if (!mtktv_cpufreq_dir) {
		pr_err("corner procfs initialization fail\n");
		return -ENOMEM;
	}
	return 0;
}

static int __init mtktv_cpufreq_debug_init(void)
{
	mtvtv_cpufreq_procfs_init();
	return 0;
}

#ifdef MODULE
module_init(mtktv_cpufreq_debug_init);
MODULE_DESCRIPTION("dbg for cpufreq");
MODULE_LICENSE("GPL");
#else
module_init(mtktv_cpufreq_debug_init);
#endif