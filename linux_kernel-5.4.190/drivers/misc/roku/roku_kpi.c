#include <linux/ctype.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/vmalloc.h>

#define MAX_NAME_LEN 30
#define MAX_KPI_LIST_ENTRIES 50
static int kpi_entries = 0;

struct kpi_step_track_t {
    char name[MAX_NAME_LEN];
    ktime_t time;
    struct list_head list;
};

static struct list_head head = LIST_HEAD_INIT(head);
static DEFINE_MUTEX(kpi_mutex);
static ktime_t ref_start_time;
static int roku_kpi_remove(struct platform_device *pdev);

/*
* Usage:
* "[Ts:15335835]|Some record"
*      ^       ^
*      |start  |end
*/
static bool parse_timestamp(const char *start, const char *end, long int *ms)
{
    const char *ptr;
    long int value = 0;

    if (start >= end || start == NULL || end == NULL) {
        /*empty string or invalid pointers */
        return false; 
    }

    for (ptr = start; *ptr != *end; ++ptr) {
        if (!isdigit(*ptr)) {
            return false;
        }
        value *= 10;
        value += *ptr - '0';
    }

    *ms = value;
    return true;
}

static void extract_timestamp_and_event(const char* tbuf, long int *m_sec,
                                char *event_str, size_t count)
{
    const char ts_start_marker[] = "[Ts:";
    const char ts_end_marker[] = "]|";
    const char *ts_end_ptr = tbuf;
    unsigned int evstr_len =  0;

    /*if start marker is at the beginning of the buffer*/
    if (strncmp(tbuf, ts_start_marker, sizeof(ts_start_marker) - 1) == 0) {
        const char *ts_start_ptr = tbuf;
        ts_end_ptr = strstr(ts_start_ptr + sizeof(ts_start_marker) - 1,
                        ts_end_marker);
        if (ts_end_ptr != NULL) {
            ts_start_ptr += sizeof(ts_start_marker) - 1;
            if (parse_timestamp(ts_start_ptr, ts_end_ptr, m_sec)) {
                ts_end_ptr += sizeof(ts_end_marker) - 1;
            } else {
                /*if we don't find timestamp we consider
                everything as event string*/
                ts_end_ptr = tbuf;
            }
        } else {
            ts_end_ptr = tbuf;
        }
    }

    evstr_len = count - (ts_end_ptr - tbuf);
    if (evstr_len >= MAX_NAME_LEN) {
        evstr_len = MAX_NAME_LEN - 1;
    }
    strncpy(event_str, ts_end_ptr, evstr_len);
    /*Remove trailing '\n' if there is any*/
    if (event_str[evstr_len - 1] == '\n') {
        --evstr_len;
    }

    event_str[evstr_len] = 0x0;
}

static ssize_t store_roku_kpi(const char *buf, size_t count)
{
    long int extracted_msec = 0;
    struct kpi_step_track_t *step;
    ktime_t local_ktime = ktime_get();

    if (!buf || !count) {
        return -EIO;
    }

    step = vmalloc(sizeof(struct kpi_step_track_t));
    if (!step) {
        return -ENXIO; // The store operation failed
    }

    pr_info("buf:%s, ref_start_time: %lld\n",buf,
                ktime_to_ms(ref_start_time));

    extract_timestamp_and_event(buf, &extracted_msec, step->name, count);
    if (extracted_msec != 0) {
        step->time = ms_to_ktime(extracted_msec -
                            ktime_to_ms(ref_start_time));
    } else {
        step->time = ktime_sub(local_ktime, ref_start_time);
    }
    mutex_lock(&kpi_mutex);
    list_add(&step->list, &head);
    if (kpi_entries < MAX_KPI_LIST_ENTRIES) {
        kpi_entries++;
    } else {
        /*Remove old entry from the list*/
        struct kpi_step_track_t *tmp_step;
        tmp_step = list_last_entry(&head, struct kpi_step_track_t, list);
        list_del(&tmp_step->list);
        vfree(tmp_step);
    }
    mutex_unlock(&kpi_mutex);

    return count;
}

static void clear_kpi_list(void)
{
    struct kpi_step_track_t *step  = NULL, *tmp_step = NULL;
    struct list_head tmp_head;

    INIT_LIST_HEAD(&tmp_head);
    mutex_lock(&kpi_mutex);
    list_splice_init(&head, &tmp_head);
    kpi_entries = 0;
    mutex_unlock(&kpi_mutex);

    list_for_each_entry_safe(step, tmp_step, &tmp_head, list) {
        list_del(&step->list);
        vfree(step);
    }
}

ssize_t kernel_write_roku_kpi(const char *buf, size_t count)
{
   return store_roku_kpi(buf, count);
}

EXPORT_SYMBOL(kernel_write_roku_kpi);

static ssize_t read_kpi_f(struct device *dev,
                            struct device_attribute *attr, char *buf)
{
    int len = 0;
    struct kpi_step_track_t *step  = NULL, *tmp_step = NULL;
    struct list_head tmp_head;
    char dummy[1];
    size_t len_s;

    INIT_LIST_HEAD(&tmp_head);
    mutex_lock(&kpi_mutex);
    list_splice_init(&head, &tmp_head);
    kpi_entries = 0;
    mutex_unlock(&kpi_mutex);

    list_for_each_entry_safe(step, tmp_step, &tmp_head, list) {
        len_s = snprintf(dummy, sizeof(dummy), "%s:%lld,", step->name,
                                                ktime_to_ms(step->time));
        ++len_s; /*scnprintf requires size of buffer including trailing null */
        len += scnprintf(buf + len, len_s, "%s:%lld,", step->name,
                                                ktime_to_ms(step->time));
        list_del(&step->list);
        vfree(step);
    }
    return len;
}

static DEVICE_ATTR(read, S_IRUGO, read_kpi_f, NULL);

static ssize_t write_kpi_step_f(struct device *dev,
                struct device_attribute *attr, const char *buf, size_t count)
{
    return store_roku_kpi(buf, count);
}
static DEVICE_ATTR(write, 0664, NULL, write_kpi_step_f);

static struct attribute *dev_attributes[] = {
        &dev_attr_read.attr,
        &dev_attr_write.attr,
        NULL
};

static const struct attribute_group dev_attr_group = {
        .attrs = dev_attributes,
};

static int add_remove_rw_sysfs_interface(struct platform_device *pdev,
                                            bool is_add_req)
{
    int retval = 0;
    static bool last_addremove = false;
    if (pdev == NULL) {
        pr_err("Invalid platform device pointer\n");
        retval = -1;
    } else if (is_add_req != last_addremove) {
        if (is_add_req) {
            retval = sysfs_create_group(&pdev->dev.kobj, &dev_attr_group);
        } else {
            sysfs_remove_group(&pdev->dev.kobj, &dev_attr_group);
            clear_kpi_list();
        }
        if (!retval) {
            last_addremove = is_add_req;
        }
    }
    return retval;
}

static ssize_t enable_kpi_f(struct device *dev,
                struct device_attribute *attr, const char *buf, size_t count)
{
    int retval = -EINVAL;
    struct platform_device *pdev = to_platform_device(dev);
    if (count == 1 || (count == 2 && buf[1] == '\n')) {
        if (*buf == '0') {
            /*remove sysfs read/write interfaces*/
            retval = add_remove_rw_sysfs_interface(pdev, false);
        } else if (*buf == '1') {
            retval = add_remove_rw_sysfs_interface(pdev, true);
        }
    }

    return retval != 0 ? retval : count;
}
static DEVICE_ATTR(enable, 0664, NULL, enable_kpi_f);

static struct attribute *enable_dev_attributes[] = {
        &dev_attr_enable.attr,
        NULL
};
static const struct attribute_group dev_enable_attr_group = {
        .attrs = enable_dev_attributes,
};

static int __init roku_kpi_probe(struct platform_device *pdev)
{
    return add_remove_rw_sysfs_interface(pdev, true) ||
        sysfs_create_group(&pdev->dev.kobj, &dev_enable_attr_group);
}

static int roku_kpi_remove(struct platform_device *pdev)
{
    clear_kpi_list();
    sysfs_remove_group(&pdev->dev.kobj, &dev_enable_attr_group);
    add_remove_rw_sysfs_interface(pdev, false);
    return 0;
}

static int roku_kpi_suspend(struct platform_device *pdev, pm_message_t state)
{
    clear_kpi_list();
    return 0;
}
static int roku_kpi_resume(struct platform_device *pdev)
{
    ref_start_time = ktime_get();
    return 0;
}

static struct platform_driver roku_kpi_driver = {
        .driver = {
                .owner = THIS_MODULE,
                .name  = "roku_kpi",
        },
        .remove  = roku_kpi_remove,
        .suspend = roku_kpi_suspend,
        .resume  = roku_kpi_resume
};

static int __init roku_kpi_init(void)
{
    ref_start_time = ktime_get();
    return platform_driver_probe(&roku_kpi_driver, roku_kpi_probe);
}

static void __exit roku_kpi_exit(void)
{
    platform_driver_unregister(&roku_kpi_driver);
}

module_init(roku_kpi_init);
module_exit(roku_kpi_exit);

MODULE_AUTHOR("Roku, Inc.");
MODULE_DESCRIPTION("Roku KPI");
MODULE_LICENSE("GPL");
