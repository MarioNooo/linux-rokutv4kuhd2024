#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include <linux/atomic.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/iio/consumer.h>
#include <linux/spinlock.h>

#include "MMAP_MI.h"

#include <roku_common_defs.h>

struct iio_channel *adc_kpd;

extern bool roku_get_backlight_status(void);
extern void roku_set_backlight_status(bool status);
extern void roku_toggle_backlight(bool active);

typedef void (*Roku_early_pwr_mode_handler_notify_fn_t)(u32 data);
extern void MDrv_IR_Roku_Set_Early_Power_Mode_Handler(Roku_early_pwr_mode_handler_notify_fn_t notify_fn);

static DEFINE_SPINLOCK(kpd_sar_spinlock);
static DEFINE_MUTEX(handler_mutex);
static DEFINE_SPINLOCK(toggle_spinlock);
static DEFINE_MUTEX(ir_mutex);

static __roku_wakeup_triggers triggers;

static bool _ir_handler_active = false;
static bool _sar_handler_active = false;
static struct timer_list _kpd_sar_timer;
#define KPD_SAR_CHECK_PERIOD    20 //msec
#define KPD_SAR_PRESS_TIMEOUT   200 //msec
static bool _ir_handler_was_initialized = false;
static bool _sar_handler_was_initialized = false;
static int sar_action_key_press_timeout = 0;
static bool _mask_kpd_sar_event = false;

static int rokuledbrightness=50;
static int rokuledpolarity=1;
static char mfgmode[2];

static struct timer_list _toggle_led_timer;
static bool _toggle_led_init = false, _toggle_led_active = false, _led_on = false;
static uint16_t led_off_duty_cycle;
static uint16_t led_on_duty_cycle;
static bool backlight_control = true;

static void toggle_led(struct timer_list *t);

static atomic_t ir_poweron_key = ATOMIC_INIT(0);
static atomic_t sar_action_key_status = ATOMIC_INIT(0);

static int rescue_firmware = 0;

s64 resume_start_time;

#define PM_REGISTER_BASE 0x1F001C00UL
static void __iomem *_pwm_reg_page;
void start_toggle_led(void)
{
    uint16_t led_period;
    int val;

    if(_toggle_led_active != true) {
//TODO: MTK_R829 need to fix reboot coredump issue
#if 0
        _pwm_reg_page = ioremap(PM_REGISTER_BASE, 1024);
        if (!_pwm_reg_page) {
            printk("Unable to map PWM duty cycle register page");
            return;
        }

        // Set polarity
        iowrite16(((ioread16(_pwm_reg_page + (0x6B<<2)) & 0xfffe) | rokuledpolarity), (_pwm_reg_page + (0x6B<<2)));
        led_period = (int) ioread16(_pwm_reg_page + (0x6A<<2));
        val = led_period - (led_period * rokuledbrightness) / 100;
        led_on_duty_cycle = (uint16_t) val;
        led_off_duty_cycle = (uint16_t) led_period;
        _led_on = (ioread16(_pwm_reg_page + (0x69<<2)) == led_period) ? false : true;
        _toggle_led_active = true;
        toggle_led(0);
#endif
    }
}
#undef PM_REGISTER_BASE

static void toggle_led_init(int ms)
{
    _toggle_led_timer.expires = jiffies + msecs_to_jiffies(ms);
    timer_setup(&_toggle_led_timer, toggle_led, 0);
    add_timer(&_toggle_led_timer);
    _toggle_led_init = true;
}

static void toggle_led(struct timer_list *t)
{
    spin_lock(&toggle_spinlock);
    if(_toggle_led_active) {
        if(_led_on) {
            if (_pwm_reg_page)
                iowrite16(led_off_duty_cycle, (_pwm_reg_page + (0x69<<2)));
            if(_toggle_led_init) {
                mod_timer(&_toggle_led_timer, (jiffies + (msecs_to_jiffies(500))));
            } else {
                toggle_led_init(500);
            }
            _led_on = false;
        } else {
            if (_pwm_reg_page)
                iowrite16(led_on_duty_cycle, (_pwm_reg_page + (0x69<<2)));
            if(_toggle_led_init) {
                mod_timer(&_toggle_led_timer, (jiffies + (msecs_to_jiffies(1000))));
            } else {
                toggle_led_init(1000);
            }
            _led_on = true;
        }
    }
    spin_unlock(&toggle_spinlock);
}

static void stop_toggle_led(void)
{
    spin_lock_bh(&toggle_spinlock);
    if(_toggle_led_active == true) {
        _toggle_led_active = false;
        if (_pwm_reg_page)
            iounmap(_pwm_reg_page);
    }
    spin_unlock_bh(&toggle_spinlock);
}

// handler_mutex should be held prior to calling this function...
static void early_pwr_on(void)
{
    pr_info("early_pwr_on start");
    // If we match a power key and it's a TOGGLE key or it's DISCRETE
    // and power is currently off (i.e. DISCRETE turns on power only),
    // we handle the condition as an early power mode cycle event...
    if(!roku_get_backlight_status())
    {
        roku_set_backlight_status(!roku_get_backlight_status());
        printk("Handle early pwr button event: --> %s\n", (roku_get_backlight_status() ? "on" : "off"));
        if (backlight_control) {
            roku_toggle_backlight(roku_get_backlight_status());
        }

        // When booting rescue firmware cancel early_key_handler once backlight is turned on
        if (rescue_firmware == 1)
        {
            if (_ir_handler_active == true && _ir_handler_was_initialized == true) {
                MDrv_IR_Roku_Set_Early_Power_Mode_Handler(NULL);
                _ir_handler_active = false;
            }
        }
        else
        {
            // Start the power-up LED toggling...
            start_toggle_led();
        }

    }
}

static void early_pwr_on_worker(struct work_struct *work)
{
    early_pwr_on();
}

// Work queue to get early power handler off the ISR
DECLARE_WORK(early_pwr_work, early_pwr_on_worker);

// Called from ISR
static void ir_handler_notify(u32 data)
{
    int i;
    static u32 last_key = 0;
    u32 keydata = ((data & 0xFF00) << 16) | ((data & 0xFF0000) << 0) | ((data & 0xFF) << 8) | ((data & 0xFF000000) >> 24);
    unsigned long key = keydata; // preserve the actual key value for setting ir_poweron_key below
    // Convert special-case Wifi RC failover IR codes into C7EA codes
    // Remove Roku repeat bit (7)
    if((((keydata>>16) == 0xC7EA) || ((keydata>>16) == 0xC8EA) || ((keydata>>16) == 0xC9EA)))
    {
        key = (key&~0x0F008000)|0x07000000;
    }
    pr_info("data=0x%x , key=0x%x \n", data, key);

    // If keydata repeat, we bail to avoid false key press events
    if(last_key != key)
    {
        for(i=0; i<triggers.num_ir_keycodes; i++)
        {
            if((key>>8) == (triggers.ir_keycodes[i].keycode>>8))
            {
                pr_info("0x%x match keycode \n", key);
                atomic_set(&ir_poweron_key, (long)keydata);
                schedule_work(&early_pwr_work);
                break;
            }
        }
        last_key = key;
    }

    return;
}

static ssize_t get_ir_power_on_key(struct device *dev, struct device_attribute *attr, char *buf)
{
    int len;
    len = scnprintf(buf, PAGE_SIZE, "0x%x\n", atomic_read(&ir_poweron_key));
    return len;
}

static DEVICE_ATTR(ir_poweron_key, S_IRUGO, get_ir_power_on_key, NULL);

typedef struct _kpd_sar_work_t
{
    unsigned char adc_val;
    bool complete;
} kpd_sar_work_t;

static kpd_sar_work_t kpd_sar_work_data = { 0u, true };

static void kpd_sar_handler_worker(struct work_struct *work)
{
    // MEDIATEK-7747
    // Move reading adc to workqueue because SAR ADC driver has mutex_lock and usleep.
    // In softirq, we should not sleep.
    int ret;
    unsigned int val;
    ret = iio_read_channel_processed(adc_kpd, &val);
    if (ret<0){
        pr_err("iio_read_channel_raw fail");
        return;
    }
    // pr_info("iio_read_channel_raw value = %d \n", val);

    mutex_lock(&handler_mutex);
    if(_sar_handler_active == true)
    {
        if((val >= triggers.sar_keycodes[0].low) && (val < triggers.sar_keycodes[0].high))
        {
            pr_info("adc value in range\n");
            // HACK: if early key handler has been suppressed, assume it was b/c
            // someone futzed with MDrv_SAR_Kpd which can trigger
            // a false reading by MDrv_SAR_Adc_GetValue.
            // Ignore false positive, but pay attention to subsequent ADC readings.
            if (_mask_kpd_sar_event == true) {
                _mask_kpd_sar_event = false;
            } else {
                // Call early power on only one time
                if (sar_action_key_press_timeout == 0) {
                    early_pwr_on();
                }
                if (mfgmode[0] == '1') {
                    sar_action_key_press_timeout += KPD_SAR_CHECK_PERIOD;
                    if (sar_action_key_press_timeout < KPD_SAR_PRESS_TIMEOUT) {
                        mod_timer(&_kpd_sar_timer, jiffies + msecs_to_jiffies(KPD_SAR_CHECK_PERIOD));
                    } else {
                        // write key pressed status.
                        atomic_set(&sar_action_key_status, (long)1);
                        printk("Panel Action key pressed\n");
                    }
                }
            }
        }
        else
        {
            // pr_info("reset timer \n");
            mod_timer(&_kpd_sar_timer, jiffies + msecs_to_jiffies(KPD_SAR_CHECK_PERIOD));
        }
    }
    mutex_unlock(&handler_mutex);
}

DECLARE_WORK(kpd_sar_work, kpd_sar_handler_worker);

static void kpd_sar_handler(struct timer_list *t)
{
    // pr_info("schedule sar work \n");
    schedule_work(&kpd_sar_work);
}

static void kpd_sar_ready_worker(struct work_struct *work)
{
    mutex_lock(&handler_mutex);
    if (_sar_handler_active == true) {
        pr_info(KERN_INFO "SAR KPD channel %ld\n", triggers.sar_keycodes[0].channel);

        // start polling the ADC
        _kpd_sar_timer.expires = jiffies + msecs_to_jiffies(KPD_SAR_CHECK_PERIOD);
        timer_setup(&_kpd_sar_timer, kpd_sar_handler, 0);
        add_timer(&_kpd_sar_timer);
    }
    mutex_unlock(&handler_mutex);
}

// Work queue to start sar handler
DECLARE_WORK(kpd_sar_ready_work, kpd_sar_ready_worker);

static ssize_t get_led_toggler_status(struct device *dev, struct device_attribute *attr, char *buf)
{
    int len;

    if (_toggle_led_active == true)
    {
        len = scnprintf(buf, PAGE_SIZE, "on\n");
    }
    else
    {
        len = scnprintf(buf, PAGE_SIZE, "off\n");
    }

    return len;
}

static DEVICE_ATTR(led_toggler_status, S_IRUGO, get_led_toggler_status, NULL);

static ssize_t get_early_key_handler_status(struct device *dev, struct device_attribute *attr, char *buf)
{
    int len;

    if (_ir_handler_active == true)
    {
        len = scnprintf(buf, PAGE_SIZE, "on\n");
    }
    else
    {
        len = scnprintf(buf, PAGE_SIZE, "off\n");
    }

    return len;
}

static DEVICE_ATTR(early_key_handler_status, S_IRUGO, get_early_key_handler_status, NULL);

static ssize_t get_ir_data(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i, len = 0;
    // check the magic header in case the shared memory has bogus values.
    if(triggers.magic_hdr != ROKU_WAKEUP_TRIGGERS_MAGIC)
    {
        pr_info("get_ir_data: IR info not avail in shared mem. fix me\n");
        return len;
    }

    mutex_lock(&ir_mutex);
    for(i=0; i<triggers.num_ir_keycodes; i++)
    {
        len += scnprintf(buf+len, (PAGE_SIZE-len), "%08x:%08x\n", (int)triggers.ir_keycodes[i].keycode, (int)triggers.ir_keycodes[i].action);
    }
    mutex_unlock(&ir_mutex);

    return len;
}

static ssize_t set_ir_data(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int i, index = -1, num, keycode, action;

    // check parameters and grab the new keycode:action mapping
    if (buf && (count >= 17))
    {
        sscanf(buf, "%08x:%08x", &keycode, &action);
    }
    else
    {
        pr_info("set_ir_data: Improperly formatted data\n");
        return -EIO;
    }

    // check the magic header in case the shared memory has bogus values.
    if(triggers.magic_hdr != ROKU_WAKEUP_TRIGGERS_MAGIC)
    {
        pr_info("set_ir_data: IR info not avail in shared mem. fix me\n");
        return -EIO;
    }

    mutex_lock(&ir_mutex);
    // check if entry already exists to override
    for(i=0; i<triggers.num_ir_keycodes; i++)
    {
        if(triggers.ir_keycodes[i].keycode == keycode)
        {
            index = i;
            break;
        }
    }

    // if does not exist already, move current entries down a slot (dropping
    // last one off the list if list too big) so as to add new entry to the
    // head of the list
    if(index == -1)
    {
        num = (triggers.num_ir_keycodes < ROKU_WAKEUP_TRIGGERS_MAX_IR_KEYCODES) ? triggers.num_ir_keycodes : (ROKU_WAKEUP_TRIGGERS_MAX_IR_KEYCODES-1);
        for(i=num; i>0; i--)
        {
            triggers.ir_keycodes[i].keycode = triggers.ir_keycodes[i-1].keycode;
            triggers.ir_keycodes[i].action  = triggers.ir_keycodes[i-1].action;
        }
        triggers.num_ir_keycodes = num+1;  // Check above guarantees the new value is <= max
        index = 0;
    }

    // and finally, add the new mapping
    triggers.ir_keycodes[index].keycode = keycode;
    triggers.ir_keycodes[index].action  = action;
    mutex_unlock(&ir_mutex);

    return count;
}

static DEVICE_ATTR(ir_data, 0664, get_ir_data, set_ir_data);

static ssize_t get_sar_data(struct device *dev, struct device_attribute *attr, char *buf)
{
    int i, len = 0;
    // check the magic header in case the shared memory has bogus values.
    if(triggers.magic_hdr != ROKU_WAKEUP_TRIGGERS_MAGIC)
    {
        pr_info("get_sar_data: SAR info not avail in shared mem. fix me\n");
        return len;
    }

    for(i=0; i<triggers.num_sar_keycodes; i++)
    {
        len += scnprintf(buf+len, (PAGE_SIZE-len), "%u:%u-%u:%s\n", triggers.sar_keycodes[i].channel, triggers.sar_keycodes[i].low, triggers.sar_keycodes[i].high, triggers.sar_keycodes[i].btn);
    }

    return len;
}

static DEVICE_ATTR(sar_data, S_IRUGO, get_sar_data, NULL);

static ssize_t stop_led_toggler_f(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    if (!count)
        return -EIO;
    if (!strncmp(buf, "stop", 4))
    {
        // Stop the power-up LED toggling...
        stop_toggle_led();

        printk("Stop kernel LED boot toggling now\n");
    }
    else
        return -EIO;
    return count;
}

static DEVICE_ATTR(stop_led_toggler, 0220, NULL, stop_led_toggler_f);

static ssize_t early_key_handler_f(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    bool changed = false;
    if (!count)
        return -EIO;

    if (!strncmp(buf, "on", 2)) {
        mutex_lock(&handler_mutex);
        // only restore handler if it had been suppressed
        if (_ir_handler_active == false && _ir_handler_was_initialized == true) {

            MDrv_IR_Roku_Set_Early_Power_Mode_Handler(&ir_handler_notify);
            mod_timer(&_kpd_sar_timer, jiffies + msecs_to_jiffies(KPD_SAR_CHECK_PERIOD));
            _ir_handler_active = true;
            changed = true;
        }
        mutex_unlock(&handler_mutex);
        if (changed) {
            printk("Restart early key handler now\n");
        }
    } else if (!strncmp(buf, "off", 3)) {
        mutex_lock(&handler_mutex);
        if (_ir_handler_active == true && _ir_handler_was_initialized == true) {
            MDrv_IR_Roku_Set_Early_Power_Mode_Handler(NULL);
            _ir_handler_active = false;
            changed = true;
        }
        mutex_unlock(&handler_mutex);
        if (changed) {
            printk("Stop early key handler now\n");
        }
    } else if (!strncmp(buf, "sar_on", 6)) {
        mutex_lock(&handler_mutex);
        if (_sar_handler_active == false && _sar_handler_was_initialized == true) {

            mod_timer(&_kpd_sar_timer, jiffies + msecs_to_jiffies(KPD_SAR_CHECK_PERIOD));

            _sar_handler_active = true;
            changed = true;
        }
        mutex_unlock(&handler_mutex);
        if (changed) {
            printk("Restart early sar handler now\n");
        }
    } else if (!strncmp(buf, "sar_off", 7)) {
        mutex_lock(&handler_mutex);
        if (_sar_handler_active == true && _sar_handler_was_initialized == true) {
            _sar_handler_active = false;

            // HACK: if early sar handler is suppressed, assume it is b/c
            // someone is futzing with MDrv_SAR_Kpd which can trigger
            // a false reading by MDrv_SAR_Adc_GetValue.
            // Set flag here to mask next KPD SAR event.
            _mask_kpd_sar_event = true;
            changed = true;
        }
        mutex_unlock(&handler_mutex);
        if (changed) {
            printk("Stop early sar handler now\n");
        }
    } else {
        return -EIO;
    }
    return count;
}

static DEVICE_ATTR(early_key_handler, 0220, NULL, early_key_handler_f);

static ssize_t early_backlight_control_f(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    if (!count)
        return -EIO;

    if (!strncmp(buf, "on", 2)) {
        mutex_lock(&handler_mutex);
        backlight_control = true;
        mutex_unlock(&handler_mutex);
        printk("Start early backlight control now\n");
    } else if (!strncmp(buf, "off", 3)) {
        mutex_lock(&handler_mutex);
        backlight_control = false;
        mutex_unlock(&handler_mutex);
        printk("Stop early backlight control now\n");
    } else {
        return -EIO;
    }
    return count;
}

static DEVICE_ATTR(early_backlight_control, 0220, NULL, early_backlight_control_f);

static ssize_t get_sar_action_key_status(struct device *dev, struct device_attribute *attr, char *buf)
{
    int len = 0;
    // check the magic header in case the shared memory has bogus values.
    if(triggers.magic_hdr != ROKU_WAKEUP_TRIGGERS_MAGIC)
    {
        pr_info("get_sar_key_pressed: SAR info not avail in shared mem. fix me\n");
        return len;
    }
    len = scnprintf(buf, PAGE_SIZE, "0x%x\n", atomic_read(&sar_action_key_status));
    return len;
}

static DEVICE_ATTR(sar_action_key_status, S_IRUGO, get_sar_action_key_status, NULL);

static ssize_t get_resume_start_time(struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t len;
    len = scnprintf(buf, PAGE_SIZE, "%lld", resume_start_time);
    return len;
}

static DEVICE_ATTR(resume_start_time, S_IRUGO, get_resume_start_time, NULL);

static struct attribute *dev_attributes[] = {
        &dev_attr_led_toggler_status.attr,
        &dev_attr_ir_poweron_key.attr,
        &dev_attr_early_key_handler_status.attr,
        &dev_attr_ir_data.attr,
        &dev_attr_sar_data.attr,
        &dev_attr_stop_led_toggler.attr,
        &dev_attr_early_key_handler.attr,
        &dev_attr_early_backlight_control.attr,
        &dev_attr_sar_action_key_status.attr,
        &dev_attr_resume_start_time.attr,
        NULL
};

static const struct attribute_group dev_attr_group = {
        .attrs = dev_attributes,
};

static int __init roku_early_pwr_mode_handler_core_init(void)
{
    void *io_addr = (void *)(ROKU_SHARE_MEM_GET_WAKEUP_TRIGGERS_DATA);
    void __iomem *iomem_triggers;

    pr_info("roku_early_pwr_mode_handler core init\n");

    // Fetch the boot-side custom pkg IR and keypad (SAR) button data
    // from u-boot via the E_ROKU_SHARE_MEM_ADR memory area.  We use
    // this instead of kernel bootarfs due to the volume of data passed
    // as the kernel cmdline is limited to 1024 bytes total.
    iomem_triggers = ioremap((unsigned long)io_addr, sizeof(__roku_wakeup_triggers));
    if (!iomem_triggers)
    {
        return -ENOMEM;
    }
    memcpy_fromio((void *)&triggers, (void *)iomem_triggers, (unsigned int)sizeof(__roku_wakeup_triggers));
    iounmap(iomem_triggers);

    
    // check the magic header in case the shared memory has bogus values.
    if(triggers.magic_hdr != ROKU_WAKEUP_TRIGGERS_MAGIC)
    {
        pr_info("roku_early_pwr_mode_handler: IR/SAR info not avail in shared mem at 0x%x. fix me\n", (unsigned int)io_addr);
        return 0;
    }

    // If the backlight is already on, start the LED toggling now...
    // If backlight not on, we need to monitor IR and SAR for power/partner
    // button presses early enough to turn on the backlight while booting...
    if(roku_get_backlight_status())
    {
        start_toggle_led();
    }
    else
    {
        // Ensure we received valid custom pkg IR and SAR data from uboot...
        if(triggers.num_ir_keycodes)
        {
            mutex_lock(&handler_mutex);
            _ir_handler_active = true;
            MDrv_IR_Roku_Set_Early_Power_Mode_Handler(&ir_handler_notify);
            _ir_handler_was_initialized = true;
            mutex_unlock(&handler_mutex);
            printk("roku_early_pwr_mode_handler_core_init(): IR handler installed\n");
        }
    }
    // Install SAR handler if 'key codes and mfg mode' or 'keycodes and backlight off'
    // Ensure we received valid custom pkg IR and SAR data from uboot.
    if(triggers.num_sar_keycodes && (mfgmode[0] == '1' || !roku_get_backlight_status()))
    {
        mutex_lock(&handler_mutex);
        _sar_handler_active = true;
            // Kernel doesn't have SAR api's. MSTAR-3687 to address missing SAR apis.

        schedule_work(&kpd_sar_ready_work);

        _sar_handler_was_initialized = true;
        mutex_unlock(&handler_mutex);
        printk("roku_early_pwr_mode_handler_core_init(): SAR Handler installed\n");
    }
    return 0;
}

static int __init roku_early_pwr_mode_handler_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;

    pr_info("roku_early_pwr_mode_handler_probe\n");

    adc_kpd = devm_iio_channel_get(dev, "adc0");
	if (IS_ERR(adc_kpd))
    {
		return PTR_ERR(adc_kpd);
    }

	roku_early_pwr_mode_handler_core_init();
    return(sysfs_create_group(&pdev->dev.kobj, &dev_attr_group));
}

static int roku_early_pwr_mode_handler_remove(struct platform_device *pdev)
{
    sysfs_remove_group(&pdev->dev.kobj, &dev_attr_group);
    return 0;
}

static void roku_early_pwr_mode_handler_shutdown(struct platform_device *pdev)
{
}

static struct platform_driver roku_early_pwr_mode_handler_driver = {
        .driver = {
                .owner = THIS_MODULE,
                .name  = "roku_early_pwr_mode_handler",
        },
        .remove = roku_early_pwr_mode_handler_remove,
        .shutdown = roku_early_pwr_mode_handler_shutdown,
};

static int __init roku_early_pwr_mode_handler_init(void)
{
    pr_info("roku early_pwr_mode handler init\n");
    return platform_driver_probe(&roku_early_pwr_mode_handler_driver, roku_early_pwr_mode_handler_probe);
}

static void __exit roku_early_pwr_mode_handler_exit(void)
{
    pr_info("roku early_pwr_mode handler exit\n");
    mutex_lock(&handler_mutex);
    if(_ir_handler_active == true)
    {
        MDrv_IR_Roku_Set_Early_Power_Mode_Handler(NULL);
        _ir_handler_active = false;
    }
    if(_sar_handler_active == true)
    {
        _sar_handler_active = false;
    }
    mutex_unlock(&handler_mutex);

    platform_driver_unregister(&roku_early_pwr_mode_handler_driver);
}

static int __init roku_led_brightness_setup(char *str)
{
    sscanf(str,"%d,%d", &rokuledbrightness, &rokuledpolarity);
    rokuledbrightness = (rokuledbrightness)>100?100:rokuledbrightness;
    rokuledpolarity = !!rokuledpolarity;
    return 0;
}

static int __init mfgmode_setup(char *str)
{
    sscanf(str,"%s", mfgmode);
    return 0;
}

__setup("rokuled=", roku_led_brightness_setup);
__setup("manuimage=", mfgmode_setup);

static int __init roku_rescue_boot_setup(char *str)
{
    (void)str;
    // Mark as booting rescue firmware
    rescue_firmware = 1;
    return 0;
}

__setup("rescueboot=", roku_rescue_boot_setup);

module_init(roku_early_pwr_mode_handler_init);
module_exit(roku_early_pwr_mode_handler_exit);

MODULE_AUTHOR("Roku, Inc.");
MODULE_DESCRIPTION("Roku Early Pwr Mode Handler");
MODULE_LICENSE("GPL");
