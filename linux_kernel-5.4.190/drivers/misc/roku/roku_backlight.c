#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/pwm.h>
#include <clocksource/arm_arch_timer.h>
#include "MMAP_MI.h"
#include "mtk-pm.h"

// The following defines should reside within the above chip header file however
// apparently they negatively impact the SCA tool mmap file parsing
#define SPLASH_ON_RESUME  1
#define CLOCK_FREQUENCY                  12000000
#define PWM_CLK_DIVIDER                  3
#define STR_SIZE       16

#include <roku_common_defs.h>

static void __iomem * backlight_status;

static bool backlight_hw_status = false;
static bool model_has_ldm = false;
static unsigned timer_dimming_init_complete=0;

extern void start_toggle_led(void);
extern void pmgammaic_init(bool resume);
extern int roku_ldmled_board_on(bool on);
extern unsigned int get_hw_uptime(void);

void backlight_on_task(bool resume);

static int rokublgpio_pin =- 1, rokualtblgpio_pin =- 1, roku_acdetect_gpio_pin = -1;
static char rokublgpio_pol =- 1, rokualtblgpio_pol =- 1, roku_acdetect_gpio_pol = -1;

static DEFINE_MUTEX(handler_mutex);

// Called from ISR
bool roku_get_backlight_status(void)
{

    bool result = false;
    uint32_t status = 0;
    mutex_lock(&handler_mutex);
    if (backlight_status)
    {
        status = ioread32(backlight_status);
        result = status == ROKU_BACKLIGHT_ON;
    }
    mutex_unlock(&handler_mutex);
    if ((status != ROKU_BACKLIGHT_ON) && (status != ROKU_BACKLIGHT_OFF)) {
        printk(KERN_ERR "Backlight status is unknown value %d", status);
    }
    return result;
}
EXPORT_SYMBOL(roku_get_backlight_status);

// Called from ISR
void roku_set_backlight_status(bool status)
{

    mutex_lock(&handler_mutex);
    if (backlight_status)
    {
        iowrite32((status) ? ROKU_BACKLIGHT_ON : ROKU_BACKLIGHT_OFF, backlight_status);
    }
    mutex_unlock(&handler_mutex);
}
EXPORT_SYMBOL(roku_set_backlight_status);

#define  PERIOD_ATTR            BIT(0)
#define  DUTY_ATTR              BIT(1)
#define  OFFSET_ATTR            BIT(2)
#define  DIV_ATTR               BIT(3)
#define  POL_ATTR               BIT(4)
#define  RESET_VSYNC_ATTR       BIT(5)
#define  OEN_ATTR               BIT(6)
#define  RSTCNT_ATTR            BIT(7)
#define  RSTMUX_ATTR            BIT(8)

struct mtk_scanpwm_dat {
    unsigned int period;
    unsigned int duty_cycle;
    unsigned int shift;
    unsigned int div;
    unsigned int pol;
    bool rst_mux;
    bool rst_vsync;
    bool enable;
    unsigned int rstcnt;
    unsigned int scanpwm_attribute;
    unsigned int sysfs_en;
};

extern int mtk_scanpwm_set_attr(u8 channel, struct mtk_scanpwm_dat *scandat);
extern int mtk_scanpwm_get_attr(u8 channel, struct mtk_scanpwm_dat *scandat);
#if defined(SPLASH_ON_RESUME)

static DEFINE_MUTEX(splash_mutex);
static bool splash_on_resume;
static bool rtcwakeup_backlight_on;
static DEFINE_MUTEX(rtcwakeup_mutex);

static void * panel_cfg_va = NULL;
struct backlight_work_s {
    bool active;
    bool is_resume;
    struct delayed_work backlight_on_work;
} backlight_work;

static unsigned get_backlight_on_wait_time(void) {
    __roku_panel_cfg * cfg = (__roku_panel_cfg *)panel_cfg_va;
    unsigned current_value = get_hw_uptime();
    unsigned panel_init_timer = ioread32(&cfg->panel_init_timer);
    unsigned cp_dimming_on = ioread32(&cfg->panel_timing_dimming_on);
    unsigned cp_bl_delay = ioread32(&cfg->panel_timing_backlight_on);
    unsigned bl_delay = cp_bl_delay;
    unsigned elapsed_bl = 0;

    if (cp_dimming_on) {
        elapsed_bl = current_value - timer_dimming_init_complete; //elapsed times based on dimming init to now
    } else {
        elapsed_bl = current_value - panel_init_timer; //elapsed times based on panel init to now
    }

    if (elapsed_bl > cp_bl_delay) {
        bl_delay = 0;
    } else {
        bl_delay = cp_bl_delay - elapsed_bl;
    }

    pr_info("get_backlight_on_wait_time current %dms, panel_init %dms, dimming_init %dms, bl_dealy %dms\n", current_value, panel_init_timer, timer_dimming_init_complete, bl_delay);
    return bl_delay;
}

static unsigned get_dimming_on_wait_time(void) {
    __roku_panel_cfg * cfg = (__roku_panel_cfg *)panel_cfg_va;
    unsigned current_value = get_hw_uptime();
    unsigned panel_init_timer = ioread32(&cfg->panel_init_timer);
    unsigned cp_dimming_delay = ioread32(&cfg->panel_timing_dimming_on);
    unsigned dim_delay = cp_dimming_delay;
    unsigned elapsed_dim = current_value - panel_init_timer; //elapsed times based on panel init

    if (elapsed_dim > cp_dimming_delay) {
        dim_delay = 0;
    } else {
        dim_delay = cp_dimming_delay - elapsed_dim;
    }
    pr_info("get_dimming_on_wait_time current %dms, panel_init %dms dim_delay %dms\n", current_value, panel_init_timer, dim_delay);
    return dim_delay;
}

static void store_backlight_on_kpi(void) {
    void * kpi_data_addr = (void *)(ROKU_SHARE_MEM_GET_BOOT_KPI_TIMER_DATA);
    void __iomem * kpi_data = ioremap_nocache((unsigned long)kpi_data_addr, sizeof(__roku_boot_resume_kpi));
    if (kpi_data) {
        __roku_boot_resume_kpi * kpi = (__roku_boot_resume_kpi *)kpi_data;
        kpi->backlight_on = get_hw_uptime();
        iounmap(kpi_data);
    }
}

static void store_dimming_complete_kpi(void) {
    void * kpi_data_addr = (void *)(ROKU_SHARE_MEM_GET_BOOT_KPI_TIMER_DATA);
    void __iomem * kpi_data = ioremap_nocache((unsigned long)kpi_data_addr, sizeof(__roku_boot_resume_kpi));
    if (kpi_data) {
        __roku_boot_resume_kpi * kpi = (__roku_boot_resume_kpi *)kpi_data;
        kpi->dimming_init_complete = get_hw_uptime();
        timer_dimming_init_complete = kpi->dimming_init_complete;
        iounmap(kpi_data);
    }
}

static void adjust_duty_cycle(void) {
    void * pwm_cfg_pa = (void *)(ROKU_SHARE_MEM_GET_BACKLIGHT_PWM_DATA);
    void __iomem * pwm_cfg_va = ioremap((unsigned long)pwm_cfg_pa, ROKU_BACKLIGHT_PWM_DATA_SIZE);
    int period;
    int duty;
    struct mtk_scanpwm_dat scanpwmdata;
    int err;

    if (pwm_cfg_va) {
        __roku_backlight_pwm_cfg default_cfg, alt_cfg;
        memcpy_fromio((void *)&default_cfg, (void *)pwm_cfg_va, sizeof(__roku_backlight_pwm_cfg));
        memcpy_fromio((void *)&alt_cfg, (void *)pwm_cfg_va + sizeof(__roku_backlight_pwm_cfg), sizeof(__roku_backlight_pwm_cfg));
        // Configure default PWM
#if 0//MTK_R829
        if (default_cfg.channel < 0) {
            printk("error: invalid pwm channel: %d\n", default_cfg.channel);
            return;
        }
        if (default_cfg.frequency) {
            period = CLOCK_FREQUENCY / PWM_CLK_DIVIDER / default_cfg.frequency;   // expressed in Hz
            duty = ((period * default_cfg.dutycycle) / 100);    // 0-100

            memset(&scanpwmdata, 0, sizeof(struct mtk_scanpwm_dat));

            scanpwmdata.scanpwm_attribute = RESET_VSYNC_ATTR;
            scanpwmdata.rst_vsync = 1;
            err = mtk_scanpwm_set_attr(default_cfg.channel, &scanpwmdata);

            scanpwmdata.scanpwm_attribute = DIV_ATTR;
            scanpwmdata.div = PWM_CLK_DIVIDER - 1;
            err = mtk_scanpwm_set_attr(default_cfg.channel, &scanpwmdata);

            scanpwmdata.scanpwm_attribute = POL_ATTR;
            scanpwmdata.pol = default_cfg.polarity;
            err = mtk_scanpwm_set_attr(default_cfg.channel, &scanpwmdata);

            scanpwmdata.scanpwm_attribute = PERIOD_ATTR;
            scanpwmdata.period = period;
            err = mtk_scanpwm_set_attr(default_cfg.channel, &scanpwmdata);

            scanpwmdata.scanpwm_attribute = DUTY_ATTR;
            scanpwmdata.duty_cycle = duty;
            err = mtk_scanpwm_set_attr(default_cfg.channel, &scanpwmdata);

            scanpwmdata.scanpwm_attribute = OEN_ATTR;
            scanpwmdata.enable = 1;
            err = mtk_scanpwm_set_attr(default_cfg.channel, &scanpwmdata);

        }

        // Configure alternate PWM
        if(alt_cfg.frequency) {
            period = CLOCK_FREQUENCY / alt_cfg.frequency;
            duty = ((period * alt_cfg.dutycycle) / 100); // 0-100

            memset(&scanpwmdata, 0, sizeof(struct mtk_scanpwm_dat));

            scanpwmdata.scanpwm_attribute = RESET_VSYNC_ATTR;
            scanpwmdata.rst_vsync = 1;
            err = mtk_scanpwm_set_attr(alt_cfg.channel, &scanpwmdata);

            scanpwmdata.scanpwm_attribute = POL_ATTR;
            scanpwmdata.pol = default_cfg.polarity;
            err = mtk_scanpwm_set_attr(alt_cfg.channel, &scanpwmdata);

            scanpwmdata.scanpwm_attribute = PERIOD_ATTR;
            scanpwmdata.period = period;
            err = mtk_scanpwm_set_attr(alt_cfg.channel, &scanpwmdata);

            scanpwmdata.scanpwm_attribute = DUTY_ATTR;
            scanpwmdata.duty_cycle = duty;
            err = mtk_scanpwm_set_attr(alt_cfg.channel, &scanpwmdata);

            scanpwmdata.scanpwm_attribute = OEN_ATTR;
            scanpwmdata.enable = 1;
            err = mtk_scanpwm_set_attr(alt_cfg.channel, &scanpwmdata);
        }
#endif
        printk("PWM default_cfg channel %d polarity %d freq %d dutycycle %d\n",
                default_cfg.channel,
                default_cfg.polarity,
                default_cfg.frequency,
                default_cfg.dutycycle);

        printk("PWM alt_cfg channel %d polarity %d freq %d dutycycle %d\n",
                alt_cfg.channel,
                alt_cfg.polarity,
                alt_cfg.frequency,
                alt_cfg.dutycycle);

        iounmap(pwm_cfg_va);
    }
}
#else
static void adjust_duty_cycle(bool active) {
    // Duty cycle will either be 0 if the backlight is turned off, or will be equal
    // to PWM period (max) if backlight is turned on.
    unsigned short duty = 0;
    struct mtk_scanpwm_dat scanpwmdata;
    memset(&scanpwmdata, 0, sizeof(struct mtk_scanpwm_dat));
    if (active) {
        mtk_scanpwm_get_attr(0, &scanpwmdata);
        duty = scanpwmdata.duty;
    }

    scanpwmdata.scanpwm_attribute = DUTY_ATTR;
    scanpwmdata.duty = duty;
    mtk_scanpwm_set_attr(0, &scanpwmdata);
}

#endif
static void backlight_drive_gpios(bool active);
static void backlight_acdet_drive_gpios(bool active);


static void backlight_on(bool active)
{
    int ret;

    mutex_lock(&handler_mutex);
    __roku_panel_cfg * cfg = (__roku_panel_cfg *)panel_cfg_va;
    // If 'active' is true then configure PWMs and turn on backlight GPIO.
    // If 'active' is false then configure PWMs but don't turn on backlight GPIO.
    // this can be called from interrupt context
    pr_info("Turn backlight %s\n", active ? "on" : "off");
    unsigned int display_type = ioread32(&cfg->display_type);
    int bl_wait = get_backlight_on_wait_time();

    if (display_type == PANEL_OLED && active) {
        printk("setting OLED Panel Backlight_On gpio\n");
        if(bl_wait > 0) {
            msleep(bl_wait);
        }
        backlight_drive_gpios(active);
        backlight_acdet_drive_gpios(active);
    } else {    //LCD Panel
#if defined(SPLASH_ON_RESUME)
        adjust_duty_cycle();
#else
        adjust_duty_cycle(active);
#endif
        store_dimming_complete_kpi();
        if (active) {
            if(bl_wait > 0) {
                msleep(bl_wait);
            }
            backlight_drive_gpios(active);
            if (model_has_ldm) {
                roku_ldmled_board_on(active);
                // TCLTRI-591: roku suggest to configure SPIO_DI,TX,RX as GPIO input
                // when the BL_ON is high before ldm signals starts
                printk("ldm set gpio input after backlight on\n");
//MTK_R829 TODO: fix gpio settings
#if 0//MTK_R829
                #define PAD_GPIO11 65
                #define PAD_GPIO12 66
                #define PAD_LD_SPI_MOSI 87

                ret = gpio_request(PAD_GPIO11, "PAD_GPIO11");
                if ((ret != -EBUSY) && (ret<0)) {
                    pr_err("[%s][%d] gpio_request fail, pin=%d , ret=%d \n", __func__ , __LINE__ , PAD_GPIO11 , ret);
                } else {
                    ret = gpio_direction_input(PAD_GPIO11);
                    if (ret < 0)
                        pr_err("[%s][%d] gpio_direction_input fail, pin=%d , ret=%d \n", __func__ , __LINE__ , PAD_GPIO11 , ret);
                    gpio_free(PAD_GPIO11);
                }

                ret = gpio_request(PAD_GPIO12, "PAD_GPIO12");
                if ((ret != -EBUSY) && (ret<0)) {
                    pr_err("[%s][%d] gpio_request fail, pin=%d , ret=%d \n", __func__ , __LINE__ , PAD_GPIO12 , ret);
                } else {
                    ret = gpio_direction_input(PAD_GPIO12);
                    if (ret < 0)
                        pr_err("[%s][%d] gpio_direction_input fail, pin=%d , ret=%d \n", __func__ , __LINE__ , PAD_GPIO12 , ret);
                    gpio_free(PAD_GPIO12);
                }

                ret = gpio_request(PAD_LD_SPI_MOSI, "PAD_LD_SPI_MOSI");
                if ((ret != -EBUSY) && (ret<0)) {
                    pr_err("[%s][%d] gpio_request fail, pin=%d , ret=%d \n", __func__ , __LINE__ , PAD_LD_SPI_MOSI , ret);
                } else {
                    ret = gpio_direction_input(PAD_LD_SPI_MOSI);
                    if (ret < 0)
                        pr_err("[%s][%d] gpio_direction_input fail, pin=%d , ret=%d \n", __func__ , __LINE__ , PAD_LD_SPI_MOSI , ret);
                    gpio_free(PAD_LD_SPI_MOSI);
                }
#endif
            }
        }
    }
    if (active) {
        store_backlight_on_kpi();
    }
    mutex_unlock(&handler_mutex);
}

static void backlight_drive_gpios(bool active)
{
    int ret;

    if (rokublgpio_pin != -1)
    {
        ret = gpio_request(rokublgpio_pin, "bl_on");
        if ((ret != -EBUSY) && (ret<0))
        {
            pr_err("[%s][%d] gpio_request fail, pin=%d , ret=%d \n", __func__ , __LINE__ , rokublgpio_pin-1 , ret);
        }
        else
        {
            if (((rokublgpio_pol == 'L') && (active == false)) ||
                ((rokublgpio_pol == 'H') && (active == true)))
            {
                // For some reason the MStar kernel GPIO drv/hal uses a one-based index
                // whereas the Utopia GPIO drv/hal uses a zero-based index, MSTAR-607
                // Remove the "-1" if this ever gets resolved
                gpio_set_value(rokublgpio_pin, 1);
            }
            else if (((rokublgpio_pol == 'L') && (active == true)) ||
                    ((rokublgpio_pol == 'H') && (active == false)))
            {
                // For some reason the MStar kernel GPIO drv/hal uses a one-based index
                // whereas the Utopia GPIO drv/hal uses a zero-based index, MSTAR-607
                // Remove the "-1" if this ever gets resolved
                gpio_set_value(rokublgpio_pin, 0);
            }
            gpio_free(rokublgpio_pin);
        }
    }

    if (rokualtblgpio_pin != -1)
    {
        ret = gpio_request(rokualtblgpio_pin, "bl_on_alt");
        if ((ret != -EBUSY) && (ret<0))
        {
            pr_err("[%s][%d] gpio_request fail, pin=%d , ret=%d \n", __func__ , __LINE__ , rokualtblgpio_pin-1 , ret);
        }
        else
        {
            if (((rokualtblgpio_pol == 'L') && (active == false)) ||
                ((rokualtblgpio_pol == 'H') && (active == true)))
            {
                // For some reason the MStar kernel GPIO drv/hal uses a one-based index
                // whereas the Utopia GPIO drv/hal uses a zero-based index, MSTAR-607
                // Remove the "-1" if this ever gets resolved
                gpio_set_value(rokualtblgpio_pin, 1);
            }
            else if (((rokualtblgpio_pol == 'L') && (active == true)) ||
                    ((rokualtblgpio_pol == 'H') && (active == false)))
            {
                // For some reason the MStar kernel GPIO drv/hal uses a one-based index
                // whereas the Utopia GPIO drv/hal uses a zero-based index, MSTAR-607
                // Remove the "-1" if this ever gets resolved
                gpio_set_value(rokualtblgpio_pin, 0);
            }
            gpio_free(rokualtblgpio_pin);
        }
    }

    backlight_hw_status = active;

}

static void backlight_acdet_drive_gpios(bool active)
{
    int ret;

    if (roku_acdetect_gpio_pin != -1) {
        ret = gpio_request(roku_acdetect_gpio_pin, "roku_acdetect_gpio_pin");
        if ((ret != -EBUSY) && (ret<0)) {
            pr_err("[%s][%d] gpio_request fail, pin=%d , ret=%d \n", __func__ , __LINE__ , roku_acdetect_gpio_pin-1 , ret);
        } else {
            if (((roku_acdetect_gpio_pol == 'L') && (active == false)) ||
                ((roku_acdetect_gpio_pol == 'H') && (active == true)))
            {
                // For some reason the MStar kernel GPIO drv/hal uses a one-based index
                // whereas the Utopia GPIO drv/hal uses a zero-based index, MSTAR-607
                // Remove the "-1" if this ever gets resolved
                gpio_set_value(roku_acdetect_gpio_pin, 1);
            }
            else if (((roku_acdetect_gpio_pol == 'L') && (active == true)) ||
                    ((roku_acdetect_gpio_pol == 'H') && (active == false)))
            {
                // For some reason the MStar kernel GPIO drv/hal uses a one-based index
                // whereas the Utopia GPIO drv/hal uses a zero-based index, MSTAR-607
                // Remove the "-1" if this ever gets resolved
                gpio_set_value(roku_acdetect_gpio_pin, 0);
            }
            gpio_free(roku_acdetect_gpio_pin);
        }
    }
}

#if defined(SPLASH_ON_RESUME)
static void deferred_backlight_on(struct work_struct * work)
{
    struct backlight_work_s * data =
            container_of(work, struct backlight_work_s, backlight_on_work.work);

    if (data->active == true) {
        int gop;
#if 0   // RNS: Athens u-boot puts the splash on gop2, malone put it on gop0
        if (data->is_resume == false)
        {
            pr_info("Enable GOP0\n");
            gop = 0;
        }
        else
#endif
        {
            pr_info("Enable GOP0\n");
            gop = 0;
        }
    }
    backlight_on(data->active);
}
#endif

void roku_toggle_backlight(bool active)
{
#if defined(SPLASH_ON_RESUME)
    backlight_work.active = active;
    schedule_delayed_work(&backlight_work.backlight_on_work, msecs_to_jiffies(get_dimming_on_wait_time()));
#else
    backlight_on(active);
#endif
}

EXPORT_SYMBOL(roku_toggle_backlight);

// this function ignores custom package delays and drives the GPIOs as fast as
// possible, for use in extreme situations e.g. on power loss where disabling
// the backlight as quick as possible gives the rest of the system more time to
// shutdown cleanly.
void roku_disable_backlight_now(void)
{
    backlight_drive_gpios(false);
}

EXPORT_SYMBOL(roku_disable_backlight_now);

static ssize_t roku_backlight_get_initial_hwstatus(struct device *dev, struct device_attribute *attr, char *buf)
{
    int len;

    if (backlight_hw_status)
    {
        len = scnprintf(buf, PAGE_SIZE, "on\n");
    }
    else
    {
        len = scnprintf(buf, PAGE_SIZE, "off\n");
    }

    return len;
}

static DEVICE_ATTR(initial_hwstatus, 0444, roku_backlight_get_initial_hwstatus, NULL);

static ssize_t roku_backlight_get_status(struct device *dev, struct device_attribute *attr, char *buf)
{
    int len;

    if (roku_get_backlight_status())
    {
        len = scnprintf(buf, PAGE_SIZE, "on\n");
    }
    else
    {
        len = scnprintf(buf, PAGE_SIZE, "off\n");
    }

    return len;
}

static ssize_t roku_backlight_set_status(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    if (!count)
        return -EIO;
    if (!strncmp(buf, "on", 2))
    {
        roku_set_backlight_status(true);
        printk("Backlight on\n");
    }
    else if (!strncmp(buf, "off", 3))
    {
        roku_set_backlight_status(false);
        printk("Backlight off\n");
    }
    else
        return -EIO;
    return count;
}

static DEVICE_ATTR(status, 0664, roku_backlight_get_status, roku_backlight_set_status);

#if defined(SPLASH_ON_RESUME)

static ssize_t roku_set_splash_on_resume(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    if (!count)
        return -EIO;
    mutex_lock(&splash_mutex);
    if (!strncmp(buf, "on", 2))
    {
        splash_on_resume = true;
    }
    else if (!strncmp(buf, "off", 3))
    {
        splash_on_resume = false;
    }
    mutex_unlock(&splash_mutex);
    return count;
}

static ssize_t roku_get_splash_on_resume(struct device *dev, struct device_attribute *attr, char *buf)
{
    int len;
    mutex_lock(&splash_mutex);
    if (splash_on_resume)
    {
        len = scnprintf(buf, PAGE_SIZE, "on\n");
    }
    else
    {
        len = scnprintf(buf, PAGE_SIZE, "off\n");
    }
    mutex_unlock(&splash_mutex);
    return len;
}

static DEVICE_ATTR(splash, 0664, roku_get_splash_on_resume, roku_set_splash_on_resume);

#endif

static ssize_t roku_set_rtc_wakeup_backlight(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    if (!count)
        return -EIO;
    mutex_lock(&rtcwakeup_mutex);
    if (!strncmp(buf, "on", 2))
    {
        rtcwakeup_backlight_on = true;
    }
    else if (!strncmp(buf, "off", 3))
    {
        rtcwakeup_backlight_on = false;
    }
    mutex_unlock(&rtcwakeup_mutex);
    return count;
}

static DEVICE_ATTR(rtcwakeup, 0200, NULL, roku_set_rtc_wakeup_backlight);

static int __init roku_backlight_probe(struct platform_device *pdev)
{
    device_create_file(&pdev->dev, &dev_attr_initial_hwstatus);
    device_create_file(&pdev->dev, &dev_attr_status);
#if defined(SPLASH_ON_RESUME)
    device_create_file(&pdev->dev, &dev_attr_splash);
#endif
    device_create_file(&pdev->dev, &dev_attr_rtcwakeup);
    return 0;
}

static int roku_backlight_remove(struct platform_device *pdev)
{
    device_remove_file(&pdev->dev, &(dev_attr_initial_hwstatus));
    device_remove_file(&pdev->dev, &(dev_attr_status));
#if defined(SPLASH_ON_RESUME)
    device_remove_file(&pdev->dev, &(dev_attr_splash));
#endif
    device_remove_file(&pdev->dev, &(dev_attr_rtcwakeup));
    if (backlight_status)
    {
        iounmap(backlight_status);
    }
#if defined(SPLASH_ON_RESUME)
    if (panel_cfg_va)
    {
         iounmap(panel_cfg_va);
    }
#endif
    return 0;
}

static void roku_backlight_shutdown(struct platform_device *pdev)
{
}

static bool should_turn_on_backlight(void)
{
    int32_t wakeup_reason = pm_get_wakeup_reason();
    bool backlight_on = true;
    mutex_lock(&rtcwakeup_mutex);
    //TODO:MTK_R829 fix wakeup reason
#if 0
    if (wakeup_reason == E_PM_WAKEUPSRC_WOWLAN_AP_LOST)
        backlight_on = false;
    else if (wakeup_reason == E_PM_WAKEUPSRC_RTC && !rtcwakeup_backlight_on)
        backlight_on = false;
#endif
    mutex_unlock(&rtcwakeup_mutex);
    return backlight_on;
}

int roku_backlight_resume(void)
{
    bool backlight_on = should_turn_on_backlight();
    if (backlight_on) {
        timer_dimming_init_complete = 0;
        roku_set_backlight_status(true);
        start_toggle_led();
        backlight_on_task(true);
    }
    return 0;
}
EXPORT_SYMBOL(roku_backlight_resume);

static struct platform_driver roku_backlight_driver = {
        .driver = {
                .owner = THIS_MODULE,
                .name  = "roku_backlight",
        },
        .remove = roku_backlight_remove,
        .shutdown = roku_backlight_shutdown,
};

static int __init roku_backlight_init(void)
{
    pr_info("roku backlight init\n");
    return(platform_driver_probe(&roku_backlight_driver, roku_backlight_probe));
}

static void __exit roku_backlight_exit(void)
{
    platform_driver_unregister(&roku_backlight_driver);
}

void backlight_on_task(bool resume) {
    /*
     * During cold boot backlight is turned on in the driver.
     * During resume only PWMs are configured, backlight is turned on by Application
    */
    bool enable_backlight = resume ? false : true;
    if (roku_get_backlight_status()) {
        roku_toggle_backlight(enable_backlight);
    }
}

static int __init roku_backlight_core_init(void)
{
    void *backlight_addr = (void *)(ROKU_SHARE_MEM_GET_BACKLIGHT_STATUS);
    void * panel_cfg_pa = (void *)(ROKU_SHARE_MEM_GET_PANEL_CONFIG_DATA);

    pr_info("roku_backlight core init\n");

    backlight_status = ioremap((unsigned long)backlight_addr, sizeof(unsigned int));
#if defined(SPLASH_ON_RESUME)
    panel_cfg_va = ioremap_wc((unsigned long)panel_cfg_pa, ROKU_PANEL_CONFIG_DATA_SIZE);

    if (panel_cfg_va) {
        __roku_panel_cfg * cfg = (__roku_panel_cfg *)panel_cfg_va;
        pmgammaic_init(false);
    }

    INIT_DELAYED_WORK(&backlight_work.backlight_on_work, deferred_backlight_on);

#endif
    return 0;
}

static int __init roku_backlight_gpio_setup(char *str)
{
    sscanf(str,"%d:%c,%d:%c", &rokublgpio_pin, &rokublgpio_pol, &rokualtblgpio_pin, &rokualtblgpio_pol);
    return 0;
}

static int __init roku_acdetect_gpio_setup(char *str)
{
    sscanf(str,"%d:%c", &roku_acdetect_gpio_pin, &roku_acdetect_gpio_pol);
    return 0;
}
static int __init model_has_ldm_setup(char *str)
{
    (void)str;

    pr_info("Local dimming enabled\n");
    model_has_ldm = true;
    return 0;
}
__setup("roku.blgpio=", roku_backlight_gpio_setup);
__setup("roku.acdetectgpio=", roku_acdetect_gpio_setup);
__setup("model_has_ldm=true", model_has_ldm_setup);
//have to make sure init after gpio module
subsys_initcall(roku_backlight_core_init);


module_init(roku_backlight_init);
module_exit(roku_backlight_exit);

MODULE_AUTHOR("Roku, Inc.");
MODULE_DESCRIPTION("Roku Backlight");
MODULE_LICENSE("GPL");

