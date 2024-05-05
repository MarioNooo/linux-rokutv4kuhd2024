/* drivers/misc/lowmemorykiller.c
 *
 * The lowmemorykiller driver lets user-space specify a set of memory thresholds
 * where processes with a range of oom_score_adj values will get killed. Specify
 * the minimum oom_score_adj values in
 * /sys/module/lowmemorykiller/parameters/adj and the number of free pages in
 * /sys/module/lowmemorykiller/parameters/minfree. Both files take a comma
 * separated list of numbers in ascending order.
 *
 * For example, write "0,8" to /sys/module/lowmemorykiller/parameters/adj and
 * "1024,4096" to /sys/module/lowmemorykiller/parameters/minfree to kill
 * processes with a oom_score_adj value of 8 or higher when the free memory
 * drops below 4096 pages and kill processes with a oom_score_adj value of 0 or
 * higher when the free memory drops below 1024 pages.
 *
 * The driver considers memory used for caches to be free, but if a large
 * percentage of the cached memory is locked this can be very inaccurate
 * and processes may not get killed until the normal oom killer is triggered.
 *
 * Copyright (C) 2007-2008 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/oom.h>
#include <linux/sched/signal.h>
#include <linux/swap.h>
#include <linux/rcupdate.h>
#include <linux/profile.h>
#include <linux/notifier.h>
#include <linux/circ_buf.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/poll.h>


#define CREATE_TRACE_POINTS
#include "trace/lowmemorykiller.h"

// Borrowed from edac_core.h
#if PAGE_SHIFT < 20
#define PAGES_TO_MiB(pages)     ((pages) >> (20 - PAGE_SHIFT))
#define MiB_TO_PAGES(mb)        ((mb) << (20 - PAGE_SHIFT))
#else                           /* PAGE_SHIFT > 20 */
#define PAGES_TO_MiB(pages)     ((pages) << (PAGE_SHIFT - 20))
#define MiB_TO_PAGES(mb)        ((mb) >> (PAGE_SHIFT - 20))
#endif

// Memory available threshold to use when swap is low
// 13M, in pages (256 = 1024*1024/4096)
#define MEM_AVAIL_LOW 13*256

// at 1280 pages(5M) free swap, kill bg apps
#define FREESWAP_KILL_BG  1280

// at 256 pages(1M) free swap, kill fg app
#define FREESWAP_KILL_FG  256

static u32 lowmem_debug_level = 1;
static short lowmem_adj[6] = {
        6,
        6,
        6,
        14,
};
static int lowmem_adj_size = 4;
static int lowmem_minfree[6] = {
        MiB_TO_PAGES(6),        /*  6 MiB */
        MiB_TO_PAGES(8),        /*  8 MiB */
        MiB_TO_PAGES(13),       /* 13 MiB */
        MiB_TO_PAGES(64),       /* 64 MiB */
};

static int lowmem_minfree_size = 4;

static unsigned long lowmem_deathpending_timeout;

#define lowmem_print(level, x...)                        \
        do {                                                \
                if (lowmem_debug_level >= (level))        \
                        pr_info(x);                        \
        } while (0)

static DECLARE_WAIT_QUEUE_HEAD(event_wait);
static DEFINE_SPINLOCK(lmk_event_lock);
static struct circ_buf event_buffer;
#define MAX_BUFFERED_EVENTS 8
#define MAX_TASKNAME 128

struct lmk_event {
        char taskname[MAX_TASKNAME];
        pid_t pid;
        uid_t uid;
        pid_t group_leader_pid;
        unsigned long min_flt;
        unsigned long maj_flt;
        unsigned long rss_in_pages;
        short oom_score_adj;
        short min_score_adj;
        unsigned long long start_time;
        struct list_head list;
};

void handle_lmk_event(struct task_struct *selected, int selected_tasksize,
                        short min_score_adj)
{
        int head;
        int tail;
        struct lmk_event *events;
        struct lmk_event *event;

        spin_lock(&lmk_event_lock);

        head = event_buffer.head;
        tail = READ_ONCE(event_buffer.tail);

       /* Do not continue to log if no space remains in the buffer. */
        if (CIRC_SPACE(head, tail, MAX_BUFFERED_EVENTS) < 1) {
                spin_unlock(&lmk_event_lock);
                return;
        }

        events = (struct lmk_event *) event_buffer.buf;
        event = &events[head];

        strncpy(event->taskname, selected->comm, MAX_TASKNAME);

        event->pid = selected->pid;
        event->uid = from_kuid_munged(current_user_ns(), task_uid(selected));
        if (selected->group_leader)
                event->group_leader_pid = selected->group_leader->pid;
        else
                event->group_leader_pid = -1;
        event->min_flt = selected->min_flt;
        event->maj_flt = selected->maj_flt;
        event->oom_score_adj = selected->signal->oom_score_adj;
        event->start_time = nsec_to_clock_t(selected->real_start_time);
        event->rss_in_pages = selected_tasksize;
        event->min_score_adj = min_score_adj;

        event_buffer.head = (head + 1) & (MAX_BUFFERED_EVENTS - 1);

        spin_unlock(&lmk_event_lock);

        wake_up_interruptible(&event_wait);
}

static int lmk_event_show(struct seq_file *s, void *unused)
{
        struct lmk_event *events = (struct lmk_event *) event_buffer.buf;
        int head;
        int tail;
        struct lmk_event *event;

        spin_lock(&lmk_event_lock);

        head = event_buffer.head;
        tail = event_buffer.tail;

        if (head == tail) {
                spin_unlock(&lmk_event_lock);
                return -EAGAIN;
        }

        event = &events[tail];

        seq_printf(s, "%lu %lu %lu %lu %lu %lu %hd %hd %llu\n%s\n",
                (unsigned long) event->pid, (unsigned long) event->uid,
                (unsigned long) event->group_leader_pid, event->min_flt,
                event->maj_flt, event->rss_in_pages, event->oom_score_adj,
                event->min_score_adj, event->start_time, event->taskname);

        event_buffer.tail = (tail + 1) & (MAX_BUFFERED_EVENTS - 1);

        spin_unlock(&lmk_event_lock);
        return 0;
}

static unsigned int lmk_event_poll(struct file *file, poll_table *wait)
{
        int ret = 0;

        poll_wait(file, &event_wait, wait);
        spin_lock(&lmk_event_lock);
        if (event_buffer.head != event_buffer.tail)
                ret = POLLIN;
        spin_unlock(&lmk_event_lock);
        return ret;
}

static int lmk_event_open(struct inode *inode, struct file *file)
{
        return single_open(file, lmk_event_show, inode->i_private);
}

static const struct file_operations event_file_ops = {
        .open = lmk_event_open,
        .poll = lmk_event_poll,
        .read = seq_read
};

static void lmk_event_init(void)
{
        struct proc_dir_entry *entry;

        event_buffer.head = 0;
        event_buffer.tail = 0;
        event_buffer.buf = kmalloc(
                sizeof(struct lmk_event) * MAX_BUFFERED_EVENTS, GFP_KERNEL);
        if (!event_buffer.buf)
                return;
        entry = proc_create("lowmemorykiller", 0, NULL, &event_file_ops);
        if (!entry)
                pr_err("error creating kernel lmk event file\n");
}

static unsigned long lowmem_count(struct shrinker *s,
                                  struct shrink_control *sc)
{
        return global_node_page_state(NR_ACTIVE_ANON) +
                global_node_page_state(NR_ACTIVE_FILE) +
                global_node_page_state(NR_INACTIVE_ANON) +
                global_node_page_state(NR_INACTIVE_FILE);
}

bool is_roku_application(struct task_struct *);

static unsigned long lowmem_scan(struct shrinker *s, struct shrink_control *sc)
{
        struct task_struct *tsk;
        struct task_struct *selected = NULL;
        unsigned long rem = 0;
        int tasksize;
        int i;
        short min_score_adj = OOM_SCORE_ADJ_MAX + 1;
        int minfree = 0;
        int selected_tasksize = 0;
        short selected_oom_score_adj;
        struct task_struct *app_p = NULL;
        short selected_level = -1;
        int array_size = ARRAY_SIZE(lowmem_adj);
        int other_free = global_zone_page_state(NR_FREE_PAGES) - totalreserve_pages;
        int other_file = global_node_page_state(NR_FILE_PAGES) -
                                global_node_page_state(NR_SHMEM) -
                                global_node_page_state(NR_UNEVICTABLE) -
                                total_swapcache_pages();
        int avail_mem = si_mem_available();  /* Kernel's available memory */
        struct sysinfo si;
        si_swapinfo(&si);

        if (lowmem_adj_size < array_size)
                array_size = lowmem_adj_size;
        if (lowmem_minfree_size < array_size)
                array_size = lowmem_minfree_size;
        for (i = 0; i < array_size; i++) {
                minfree = lowmem_minfree[i];
                if (avail_mem < minfree) {
                        min_score_adj = lowmem_adj[i];
                        selected_level = i;
                        lowmem_print(3, "avail_mem: %d, minfree: %d, min_score_adj: %d, selected_level:%d\n",
                                        avail_mem, minfree, min_score_adj, selected_level);
                        break;
                }
        }


        // special case: kill plugins based on swap utilization once memory threshold is crossed
        if (si.totalswap > 0) {
                int level = -1;
                if (si.freeswap <= FREESWAP_KILL_BG) {
                        // threshold crossed, ask App to kill bg apps
                        level = 3;
                }

                // memory is low and swap is low: request fg app kill
                // we shouldn't be able to reach this until bg apps are already killed.
                if (avail_mem < MEM_AVAIL_LOW) {
                        if (si.freeswap == 0) {
                                // completely out of swap, kernel directly SIGKILLs fg app
                                level = 0;
                        } else if (si.freeswap <= FREESWAP_KILL_FG) {
                                // ask App to kill the fg app
                                level = 1;
                        }
                }
                // if selected_level was previously selected based on (non-swap) memory levels,
                // only allow value to decrease.
                // otherwise use the level based on swap thresholds
                if (level != -1 && (level < selected_level || selected_level == -1)) {
                        selected_level = level;
                        min_score_adj = lowmem_adj[level];
                }
        }
        lowmem_print(3, "lowmem_scan %lu, %x, ofree %d %d, ma %hd, mem_avail %dkB freeswap %lukB\n",
                        sc->nr_to_scan, sc->gfp_mask, other_free,
                        other_file, min_score_adj, avail_mem*4, si.freeswap*4);

        if (min_score_adj == OOM_SCORE_ADJ_MAX + 1) {
                lowmem_print(5, "lowmem_scan %lu, %x, return 0\n",
                        sc->nr_to_scan, sc->gfp_mask);
                return 0;
        }

        selected_oom_score_adj = min_score_adj;

        rcu_read_lock();
        for_each_process(tsk) {
                struct task_struct *p;
                short oom_score_adj;

                if (tsk->flags & PF_KTHREAD)
                        continue;

                p = find_lock_task_mm(tsk);
                if (!p)
                        continue;
                if (!uid_eq(p->cred->uid, GLOBAL_ROOT_UID) && is_roku_application(p))  /* Find the Application (non-root) */
                {
                        app_p = p;
                }
                if (task_lmk_waiting(p) &&
                        time_before_eq(jiffies, lowmem_deathpending_timeout)) {
                        task_unlock(p);
                        rcu_read_unlock();
                        return 0;
                }
                oom_score_adj = p->signal->oom_score_adj;
                if (oom_score_adj < min_score_adj) {
                        task_unlock(p);
                        continue;
                }
                tasksize = get_mm_rss(p->mm);
                task_unlock(p);
                if (tasksize <= 0)
                        continue;
                if (selected) {
                        if (oom_score_adj < selected_oom_score_adj)
                                continue;
                        if (oom_score_adj == selected_oom_score_adj &&
                                tasksize <= selected_tasksize)
                                continue;
                }
                selected = p;
                selected_tasksize = tasksize;
                selected_oom_score_adj = oom_score_adj;
                lowmem_print(2, "select '%s' (%d), adj %hd, size %d, to kill\n",
                                p->comm, p->pid, oom_score_adj, tasksize);
        }

        if (selected || app_p) {
                if ((si.totalswap > 0)  && (si.freeswap <= FREESWAP_KILL_BG)) {
                        pr_err("low swap pages: %lu\n", si.freeswap);
                }
                long cache_size = other_file * (long)(PAGE_SIZE / 1024);
                long cache_limit = minfree * (long)(PAGE_SIZE / 1024);
                long free = other_free * (long)(PAGE_SIZE / 1024);

                if ((si.totalswap > 0)  && (si.freeswap <= FREESWAP_KILL_BG)) {
                        pr_err("low swap pages: %lu\n", si.freeswap);
                }
                /* For NDK plugin, directly kill it, oom_score_adj is set to 1000 for NDK apps */
                if (( app_p != selected) && (selected_oom_score_adj == 1000) && (selected_level==0)) {
                        pr_err("Start to send sigkill to %d (%s) avail_mem:%dkB freeswap:%lukB\n",
                                        selected->pid, selected->comm, avail_mem*4, si.freeswap*4);

                task_lock(selected);
                send_sig(SIGKILL, selected, 0);
                if (selected->mm)
                        task_set_lmk_waiting(selected);
                task_unlock(selected);
                trace_lowmemory_kill(selected, cache_size, cache_limit, free);
                lowmem_print(1, "Killing '%s' (%d) (tgid %d), adj %hd,\n"
                                "   to free %ldkB on behalf of '%s' (%d) because\n"
                                "   cache %ldkB is below limit %ldkB for oom_score_adj %hd\n"
                                "   Free memory is %ldkB above reserved\n",
                                selected->comm, selected->pid, selected->tgid,
                                selected_oom_score_adj,
                                selected_tasksize * (long)(PAGE_SIZE / 1024),
                                current->comm, current->pid,
                                cache_size, cache_limit,
                                min_score_adj,
                                free);
                }

                /* Send signal to Application to close/cleanup the active/inactive plugin(s) */
                if (app_p) {
                        int ret;
#if 0//MTK_R829
                        struct siginfo info;
#else
                        struct kernel_siginfo info;
#endif //MTK_R829
                        pr_err("send SIGUSR1 to %d (%s), selected_level = %d avail_mem:%dkB freeswap:%lukB\n",
                                        app_p->pid, app_p->comm, selected_level, avail_mem*4, si.freeswap*4);

                        lowmem_print(2, "send SIGUSR1 to %d (%s), selected_level = %d\n",
                                app_p->pid, app_p->comm, selected_level);
#if 0//MTK_R829
                        memset(&info, 0, sizeof(struct siginfo));
#else
                        memset(&info, 0, sizeof(struct kernel_siginfo));
#endif //MTK_R829
                        info.si_signo = SIGUSR1;
                        info.si_code = SI_QUEUE;
                        info.si_int = selected_level;

                        task_lock(app_p);
                        task_set_lmk_waiting(app_p);
                        ret = send_sig_info(SIGUSR1, &info, app_p);  // send signal
                        if (app_p->mm)
                                task_set_lmk_waiting(app_p);
                        task_unlock(app_p);

                        if (ret < 0)
                                pr_err("send_sig_info error: %d\n", ret);
                        else
                                lowmem_print(2,"send_sig_info sent\n");

                }
                lowmem_deathpending_timeout = jiffies + HZ;
                rem += selected_tasksize;
                if (selected) {
                get_task_struct(selected); 
                }
        }

        lowmem_print(4, "lowmem_scan %lu, %x, return %lu\n",
                        sc->nr_to_scan, sc->gfp_mask, rem); 
        rcu_read_unlock();
        if (selected) {
                handle_lmk_event(selected, selected_tasksize, min_score_adj);
                put_task_struct(selected);
        }
        return rem;
}

static struct shrinker lowmem_shrinker = {
        .scan_objects = lowmem_scan,
        .count_objects = lowmem_count,
        .seeks = DEFAULT_SEEKS * 16
};

static int __init lowmem_init(void)
{
        register_shrinker(&lowmem_shrinker);
        lmk_event_init();
        return 0;
}
device_initcall(lowmem_init);

#ifdef CONFIG_ANDROID_LOW_MEMORY_KILLER_AUTODETECT_OOM_ADJ_VALUES
static short lowmem_oom_adj_to_oom_score_adj(short oom_adj)
{
        if (oom_adj == OOM_ADJUST_MAX)
                return OOM_SCORE_ADJ_MAX;
        else
               return (oom_adj * OOM_SCORE_ADJ_MAX) / -OOM_DISABLE;
}

static void lowmem_autodetect_oom_adj_values(void)
{
        int i;
        short oom_adj;
        short oom_score_adj;
        int array_size = ARRAY_SIZE(lowmem_adj);

        if (lowmem_adj_size < array_size)
                array_size = lowmem_adj_size;

        if (array_size <= 0)
                return;

        oom_adj = lowmem_adj[array_size - 1];
        if (oom_adj > OOM_ADJUST_MAX)
                return;

        oom_score_adj = lowmem_oom_adj_to_oom_score_adj(oom_adj);
        if (oom_score_adj <= OOM_ADJUST_MAX)
                return;

        lowmem_print(1, "lowmem_shrink: convert oom_adj to oom_score_adj:\n");
        for (i = 0; i < array_size; i++) {
                oom_adj = lowmem_adj[i];
                oom_score_adj = lowmem_oom_adj_to_oom_score_adj(oom_adj);
                lowmem_adj[i] = oom_score_adj;
                lowmem_print(1, "oom_adj %d => oom_score_adj %d\n",
                        oom_adj, oom_score_adj);
        }
}

static int lowmem_adj_array_set(const char *val, const struct kernel_param *kp)
{
        int ret;

        ret = param_array_ops.set(val, kp);

        /* HACK: Autodetect oom_adj values in lowmem_adj array */
        lowmem_autodetect_oom_adj_values();

        return ret;
}

static int lowmem_adj_array_get(char *buffer, const struct kernel_param *kp)
{
        return param_array_ops.get(buffer, kp);
}

static void lowmem_adj_array_free(void *arg)
{
        param_array_ops.free(arg);
}

static struct kernel_param_ops lowmem_adj_array_ops = {
        .set = lowmem_adj_array_set,
        .get = lowmem_adj_array_get,
        .free = lowmem_adj_array_free,
};

static const struct kparam_array __param_arr_adj = {
        .max = ARRAY_SIZE(lowmem_adj),
        .num = &lowmem_adj_size,
        .ops = &param_ops_short,
        .elemsize = sizeof(lowmem_adj[0]),
        .elem = lowmem_adj,
};
#endif

/*
 * not really modular, but the easiest way to keep compat with existing
 * bootargs behaviour is to continue using module_param here.
 */
module_param_named(cost, lowmem_shrinker.seeks, int, 0644);
#ifdef CONFIG_ANDROID_LOW_MEMORY_KILLER_AUTODETECT_OOM_ADJ_VALUES
module_param_cb(adj, &lowmem_adj_array_ops,
                .arr = &__param_arr_adj,
                0644);
__MODULE_PARM_TYPE(adj, "array of short");
#else
module_param_array_named(adj, lowmem_adj, short, &lowmem_adj_size, 0644);
#endif
module_param_array_named(minfree, lowmem_minfree, uint, &lowmem_minfree_size,
                        0644);
module_param_named(debug_level, lowmem_debug_level, uint, 0644);
