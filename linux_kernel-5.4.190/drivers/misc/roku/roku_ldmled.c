#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <mach/io.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/gpio.h>

#include "MMAP_MI.h"
#include "roku_ldmled.h"
#include "roku_ldmled_debug.h"
#include "roku_spi_ldm_device.h"

#include <roku_common_defs.h>

int ldmled_device_major = 0;
int ldmled_device_minor = 0;

#define DEVICE_NAME "ldmled"
char* ldmled_device_name = DEVICE_NAME;
struct cdev ldmled_dev;
static struct class *ldmled_class;

struct ldmled_runtime {
    unsigned int power_on : 1;
    unsigned int dev_inited : 1;
    unsigned int bfi_on : 1;
    uint32_t bfi_global_current;
    struct spi_ldm_device *dev;
    struct mutex mutex;
    uint16_t *zone_bufs[BUF_LAST];
    uint32_t *delay_buf;
};

__ldm_board_info board_info;

struct ldmled_runtime s_runtime = {
        .power_on = 0,
        .dev_inited = 0,
        .bfi_on = 0,
        .bfi_global_current = 32768,
        .dev = NULL,
        .zone_bufs = {NULL, NULL, NULL},
        .delay_buf = NULL,
};

struct ldmled_debug s_debug = {
        .frame_tick = 0,
        .debug_mode = 0x0,
        .log_frames = {4, 4, 4},
        .demo_mode = {0, 30},
        .buffers = {NULL, NULL, NULL},
        .profiling = NULL,
};

static struct spi_ldm_device* device_list[] = {
    &dev_7038,
    &dev_7039,
};

static struct sysstateregs sys_state = {
        .LDStrength = 0,
        .sceneChange = 0,
        .GlobalBacklight = 1023,
};

static struct configregs boost_sysconf = {
        .vzones = 6,
        .hzones = 8,
        .zones = 48,
        .GlobalDriveCurrent = 0xFFFF,
        .PSCurrentMax  = 3023394,
};


struct spi_ldm_device* probe_spi_ldm_device(char* device_name, __spi_bus *bus) {
    int i = 0;
    for (i = 0; i < ARRAY_SIZE(device_list); ++i) {
        if (strstr(device_name, device_list[i]->name)) {
            struct spi_ldm_device *dev = device_list[i];
            dev->bus = bus;
            mutex_init(&dev->mutex);
            return dev;
        }
    }
    printk(KERN_ERR "Can't match spi device:%s\n", device_name);
    return NULL;
}

int roku_ldmled_board_on(bool on) {
    int ret = 0;
    s_runtime.power_on = on;
    struct spi_ldm_device *dev = s_runtime.dev;
    if (!dev) return -1;
    printk("[ldmled] power %s local dimming board\n", on ? "on" : "off");
    mutex_lock(&s_runtime.mutex);
    if (on) {
        mdelay(300);
        ret = dev->init(dev);
        s_runtime.dev_inited = !ret;

        int zones = board_info.row * board_info.column;
        int i = 0;
        for (i = 0; i < zones; i ++) {
            s_runtime.zone_bufs[BUF_PWM_OUTPUT][i] = 65535;
            s_runtime.zone_bufs[BUF_CURRENT_OUTPUT][i] = boost_sysconf.GlobalDriveCurrent;
        }
        s_runtime.dev->update_device(s_runtime.dev, s_runtime.zone_bufs[BUF_PWM_OUTPUT],
                              s_runtime.zone_bufs[BUF_CURRENT_OUTPUT], zones, &sys_state);
        s_runtime.bfi_global_current = boost_sysconf.GlobalDriveCurrent;
        s_runtime.bfi_on = false;
    } else {
        dev->exit(dev);
        s_runtime.dev_inited = false;
    }
    mutex_unlock(&s_runtime.mutex);
    return ret;
}

EXPORT_SYMBOL(roku_ldmled_board_on);
#define MS_U16 uint16_t
typedef struct
{
    //LDF
    MS_U16 u16LDFWidth;
    MS_U16 u16LDFHeight;
    MS_U16 u16LDFMin[2304];
    MS_U16 u16LDFMean[2304];
    MS_U16 u16LDFMax[2304];
    //Histogram
    MS_U16 u16ShiftCnt;
    MS_U16 u16Histogram[32];
}ST_DRV_LD_GET_LDF_ROKU_DATA;


int roku_ldmled_entry(uint16_t* led_buf, uint16_t row, uint16_t column, uint16_t* led_buf_output, ST_DRV_LD_GET_LDF_ROKU_DATA* pstLdfRokuData) {
    s_debug.frame_tick++;
    if (test_bit(BYPASS_CONTROL, &s_debug.debug_mode)) return 0;

    mutex_lock(&s_runtime.mutex);
    if (!s_runtime.dev_inited) {
        mutex_unlock(&s_runtime.mutex);
        return 0;
    }
    //feed data into buffer
    int zones = board_info.row * board_info.column;
    int i = 0;
    if (test_bit(DEMO_MODE, &s_debug.debug_mode)) {
       debug_run_demo_mode(&s_debug, s_runtime.zone_bufs[BUF_LED_INPUT]);
    } else {
        for (i = 0; i < zones; i++)
            s_runtime.zone_bufs[BUF_LED_INPUT][i] = led_buf[i];
    }
    if (test_bit(REPLAY_LED_INPUT, &s_debug.debug_mode)) {
        debug_replay_frame(&s_debug, BUF_LED_INPUT, s_runtime.zone_bufs[BUF_LED_INPUT],
                      &sys_state);
    } else if (test_bit(LOG_LED_INPUT, &s_debug.debug_mode)) {
        debug_log_frame(&s_debug, BUF_LED_INPUT, s_runtime.zone_bufs[BUF_LED_INPUT], &sys_state);
    }

    bool profiling_on = test_bit(PROFILING_ON, &s_debug.debug_mode);
    if (profiling_on) debug_profiling_log(&s_debug, 0, false);
    //processing data.
    if (test_bit(BYPASS_ALGO, &s_debug.debug_mode)) {
        for (i = 0; i < zones; i++) {
            s_runtime.zone_bufs[BUF_PWM_OUTPUT][i] = s_runtime.zone_bufs[BUF_LED_INPUT][i] * sys_state.GlobalBacklight / 1023;
            s_runtime.zone_bufs[BUF_CURRENT_OUTPUT][i] = s_runtime.bfi_global_current;
        }
    } else if(test_bit(PATTERN_MODE, &s_debug.debug_mode)) {
        ;//using s_runtime.zone_bufs which is pre-generated in debug_set_pattern_mode 
    } else {
        if (sys_state.LDStrength == 0) { //Global Dimming
            int i = 0;
            for (i = 0; i < zones; i++) {
                s_runtime.zone_bufs[BUF_PWM_OUTPUT][i] = (int)s_runtime.zone_bufs[BUF_LED_INPUT][i] * sys_state.GlobalBacklight / 1023;
                s_runtime.zone_bufs[BUF_CURRENT_OUTPUT][i] = s_runtime.bfi_global_current;
            }
        } else {
            RokuBoost(s_runtime.zone_bufs[BUF_LED_INPUT], s_runtime.zone_bufs[BUF_PWM_OUTPUT],s_runtime.zone_bufs[BUF_CURRENT_OUTPUT], &sys_state);
        }
    }

    // feed boosted backlight result back to pixel compensation 
    if (sys_state.LDStrength == 0 || test_bit(BYPASS_ALGO, &s_debug.debug_mode) || test_bit(BOOST_BACK_DISABLE, &s_debug.debug_mode)) {
        memcpy(led_buf_output, led_buf, sizeof(uint16_t) * row * column); 
    } else {
        int i = 0; 
        for (i = 0; i < zones; i++) {
            unsigned int temp = s_runtime.zone_bufs[BUF_PWM_OUTPUT][i] * s_runtime.zone_bufs[BUF_CURRENT_OUTPUT][i];
            temp >>= 16;
            led_buf_output[i] = temp;
            s_runtime.zone_bufs[BUF_PWM_OUTPUT][i] = s_runtime.zone_bufs[BUF_PWM_OUTPUT][i] * sys_state.GlobalBacklight / 1023; // apply global gain after feedback to compensation
        }
    }

    if (profiling_on) debug_profiling_log(&s_debug, 1, false);

    if (test_bit(REPLAY_PWM_OUTPUT, &s_debug.debug_mode)) {
        debug_replay_frame(&s_debug, BUF_PWM_OUTPUT, s_runtime.zone_bufs[BUF_PWM_OUTPUT], NULL);
    } else if (test_bit(LOG_PWM_OUTPUT, &s_debug.debug_mode)) {
        debug_log_frame(&s_debug, BUF_PWM_OUTPUT, s_runtime.zone_bufs[BUF_PWM_OUTPUT], &sys_state);
    }
    if (test_bit(REPLAY_CURRENT_OUTPUT, &s_debug.debug_mode)) {
        debug_replay_frame(&s_debug, BUF_CURRENT_OUTPUT, s_runtime.zone_bufs[BUF_CURRENT_OUTPUT], NULL);
    } else if (test_bit(LOG_CURRENT_OUTPUT, &s_debug.debug_mode)) {
        debug_log_frame(&s_debug, BUF_CURRENT_OUTPUT, s_runtime.zone_bufs[LOG_CURRENT_OUTPUT], &sys_state);
    }

    //output data to device
    struct spi_ldm_device *dev = s_runtime.dev;
    dev->update_device(dev, s_runtime.zone_bufs[BUF_PWM_OUTPUT], s_runtime.zone_bufs[BUF_CURRENT_OUTPUT], zones, &sys_state);

    if (profiling_on) debug_profiling_log(&s_debug, 2, true);
    mutex_unlock(&s_runtime.mutex);
    return 0;
}

EXPORT_SYMBOL(roku_ldmled_entry);

int roku_ldmled_bfi_conrol(struct bfi_control * bfi) {
    int total_channels = board_info.row * board_info.column;
    int row = 0, col = 0, ch = 0;

    if (bfi->bfi_on) {
        int delay = bfi->start_delay;
        for (row = 0; row < board_info.row; row++) {
            for (col = 0; col < board_info.column; col++) {
                s_runtime.delay_buf[ch] = (delay + col * bfi->column_delay) & 0xFFFFFF;
                ch++;
            }
            delay += bfi->row_delay;
        }
    } else {
        int delay_step = 0x10000 / total_channels;
        int delay = 0;
        for (row = 0; row < board_info.row; row++) {
            for (col = 0; col < board_info.column; col++) {
                s_runtime.delay_buf[ch] = delay;
                delay += delay_step;
                ch++;
            }
        }
    }

    s_runtime.bfi_on = (bfi->bfi_on != 0);
    uint64_t maxPower = boost_sysconf.PSCurrentMax;
    s_runtime.bfi_global_current = boost_sysconf.GlobalDriveCurrent;
    if (s_runtime.bfi_on) {
        uint32_t compGain = 1024;
        uint32_t compGainLUT[][2] = {{6000, 1024}, {32768, 2048}, {49152, 3072}, {65535, 4096}}; //gain = 1024 means 1
        uint16_t lutsize = sizeof(compGainLUT) / sizeof(uint16_t) / 2;
        int i = 0; 
        for (i = 0; i < lutsize; i++) {
            if (bfi->blank_time <= compGainLUT[i][0]) break;
        }
        if (i == 0) {
            compGain = compGainLUT[0][1];
        } else if (i == lutsize) {
            compGain = compGainLUT[lutsize - 1][1];
        } else { //linear interpolation
            compGain = (bfi->blank_time - compGainLUT[i - 1][0]) * 65536 / (compGainLUT[i][0] - compGainLUT[i - 1][0]);
            compGain = compGainLUT[i - 1][1] + (compGainLUT[i][1] - compGainLUT[i - 1][1]) * compGain / 65536;
        }
        s_runtime.bfi_global_current = (int)boost_sysconf.GlobalDriveCurrent * compGain / 1024;
        if (s_runtime.bfi_global_current > 65535) s_runtime.bfi_global_current = 65535; //limited to maximum value
        maxPower = boost_sysconf.PSCurrentMax * compGain / 1024;
        printk("bfi is set to On, blank_time=%d, compensation_gain = %d, global_current = %d, maxPower = %llu\n",
                bfi->blank_time,
                compGain,
                s_runtime.bfi_global_current,
                maxPower);
    }else {
        printk("bfi is set to Off, global_current = %d, maxPower = %llu\n",
               s_runtime.bfi_global_current,
               maxPower);
    }
    RokuBoost_setMaxPower(maxPower);
    
    s_runtime.dev->write_bfi_control(s_runtime.dev, s_runtime.bfi_on, bfi->blank_time, s_runtime.delay_buf, total_channels);
    return 0;
}

struct portal_cmd {
    const char *name;
    const char *help;
    int (*cmd)(int argc, char *argv[]);
};

static int portal_init_board(int argc, char *argv[]) {
    struct spi_ldm_device *dev = s_runtime.dev;
    int ret = dev->init(dev);
    printk("[Ldmled] board init result is:%d\n", ret);
    return 0;
}

static int portal_write_dev(int argc, char *argv[]) {
    if (argc < 4) return -EINVAL;
    int reg_id, reg, val;
    kstrtoint(argv[1], 0, &reg_id);
    kstrtoint(argv[2], 0, &reg);
    kstrtoint(argv[3], 0, &val);
    uint32_t buf[1];
    buf[0] = val;
    struct spi_ldm_device *dev = s_runtime.dev;
    int ret = dev->write_reg(dev, reg_id, reg, buf, 1);
    printk("[Ldmled] writing device.%02d [0x%02x]=0x%02x  result is:%d\n", reg_id, reg, val, ret);
    return 0;
}

static int portal_read_dev(int argc, char *argv[]) {
    if (argc < 3) return -EINVAL;
    int reg_id, reg, size;
    size = 1;
    kstrtoint(argv[1], 0, &reg_id);
    kstrtoint(argv[2], 0, &reg);
    if (argc > 3) kstrtoint(argv[3], 0, &size);
    uint32_t buf[size];
    struct spi_ldm_device *dev = s_runtime.dev;
    dev->read_reg(dev, reg_id, reg, buf, size);
    int i = 0;
    for (i = 0; i < size; ++i) {
        printk("[Ldmled] Reading device.%02d [0x%02x]=0x%02x\n", reg_id, reg + i, buf[i]);
    }
    return 0;
}

static int portal_demo(int argc, char *argv[]) {
    if (argc < 2) return -EINVAL;
    int pwm_val;
    kstrtoint(argv[1], 0, &pwm_val);
    struct spi_ldm_device *dev = s_runtime.dev;
    int total_channels = dev->bus->device_total * dev->channels_per_device;
    uint16_t pwm[total_channels];
    int i = 0;
    for (i = 0; i < total_channels; ++i) {
        int j = 0;
        for (j = 0; j < total_channels; ++j) {
            pwm[j] = 0x0;
        }
        pwm[i] = pwm_val;
        dev->write_pwm(dev, pwm, total_channels);
        msleep(500);
    }
    return 0;
}

static struct portal_cmd cmd_list[] = {
        {
                .name = "init",
                .help = "",
                .cmd = &portal_init_board,
        },
        {
                .name = "write_dev",
                .help = "[dev_id] [reg_addr] [val]",
                .cmd = &portal_write_dev,
        },
        {
                .name = "read_dev",
                .help = "[dev_id] [reg_addr] <size=1>",
                .cmd = &portal_read_dev,
        },
        {
                .name = "demo",
                .help = "[pwm]",
                .cmd = &portal_demo,
        },
};

static ssize_t roku_ldmled_get_portal(struct device *dev, struct device_attribute *attr, char *buf) {
    int len = 0;
    int debug_mode = debug_get_debug_mode(&s_debug);
    len += scnprintf(buf + len, PAGE_SIZE - len, "debug_mode   : 0x%04x\n", debug_mode);
    int i = 0;
    len += scnprintf(buf + len, PAGE_SIZE - len, "Supported commands:\n");
    for (i = 0; i < ARRAY_SIZE(cmd_list); ++i) {
        len += scnprintf(buf + len, PAGE_SIZE - len, "\t%s %s\n", cmd_list[i].name, cmd_list[i].help);
    }
    return len;
}

static ssize_t roku_ldmled_set_portal(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    if (!count) return -EIO;

    char in_buf[count];
    strncpy(in_buf, buf, count);
    if (in_buf[count - 1] == '\n') in_buf[count - 1] = '\0';
    char *input, *token;
    input = in_buf;
    const int MAX_ARGS = 7;
    char *argv[MAX_ARGS];
    int argc = 0;
    while ((token = strsep(&input, " ")) != NULL) {
        if (*token == '\0') continue;
        argv[argc++] = token;
        if (argc >= MAX_ARGS) break;
    }
    if (argc == 0) return -EIO;

    int i = 0;
    for (i = 0; i < ARRAY_SIZE(cmd_list); ++i) {
        if (!strcmp(argv[0], cmd_list[i].name)) {
            int ret = cmd_list[i].cmd(argc, argv);
            if (ret) return ret;
            return count;
        }
    }
    pr_err("[Ldmled] portal can't support '%s' command\n", argv[0]);
    return -EINVAL;
}

static DEVICE_ATTR(portal, 0664, roku_ldmled_get_portal, roku_ldmled_set_portal);

static ssize_t roku_ldmled_get_board_info(struct device* device, struct device_attribute* attr, char* buf) {
    struct spi_ldm_device *dev = s_runtime.dev;
    int len = scnprintf(buf, PAGE_SIZE,
            "row:             %d\n"
            "column:          %d\n"
            "mcu_less:        %s\n"
            "power on:        %s\n"
            "spi initialized: %s\n"
            "device name:     %s\n"
            "\tbus num:             %d\n"
            "\tdevice total:        %d\n"
            "\tchannels per device: %d\n"
            "\tenabled channel per device: %d\n"
            "\tpwm bits:            %d\n"
            "\tcurrent bits:        %d\n"
            "\tgpio_output_pin:     %d\n"
            "\tgpio_input_pin:      %d\n"
            "\tmapping: [row,column]\n\t",
            board_info.row,
            board_info.column,
            board_info.mcu_less ? "true" : "false",
            s_runtime.power_on? "yes" : "no",
            s_runtime.power_on && s_runtime.dev_inited ? "yes" : "no",
            dev->name,
            dev->bus->bus_num,
            dev->bus->device_total,
            dev->channels_per_device,
            dev->enabled_channels_per_device,
            dev->pwm_bits,
            dev->current_bits,
            board_info.gpio_output_pin,
            board_info.gpio_input_pin);

    if (!dev->mapping) return len;
    //print led mapping table
    int zones = board_info.row * board_info.column;
    int i = 0;
    while ( i < zones) {
        int row = dev->mapping[i] / board_info.column + 1;
        int column = dev->mapping[i] % board_info.column + 1;
        len += snprintf(buf + len, PAGE_SIZE - len, "[%02d,%02d] ", row, column);
        if ((++i) % board_info.column == 0) {
            len += snprintf(buf + len, PAGE_SIZE - len, "\n\t");
        }
    }
    len += snprintf(buf + len, PAGE_SIZE - len, "\n");
    return len;
}

static DEVICE_ATTR(board_info, 0444, roku_ldmled_get_board_info, NULL);

static ssize_t roku_ldmled_boost_config(struct device* device, struct device_attribute* attr, char* buf) {
    int len = scnprintf(buf, PAGE_SIZE,
            "hzones:             %d\n"
            "vzones:          %d\n"
            "zones:          %d\n"
            "GlobalDriveCurrent:          %d\n"
            "PSCurrentMax:        %lld\n",
            boost_sysconf.hzones,
            boost_sysconf.vzones,
            boost_sysconf.zones,
            boost_sysconf.GlobalDriveCurrent,
            boost_sysconf.PSCurrentMax);
    return len;
}

static DEVICE_ATTR(boost_config, 0444, roku_ldmled_boost_config, NULL);

static ssize_t roku_ldmled_sys_state(struct device* device, struct device_attribute* attr, char* buf) {
    int len = scnprintf(buf, PAGE_SIZE,
            "LDStregth:             %d\n"
            "SceneChange:          %d\n"
            "GlobalBacklight:        %d\n",
            sys_state.LDStrength,
            sys_state.sceneChange,
            sys_state.GlobalBacklight);
    return len;
}

static DEVICE_ATTR(sys_state, 0444, roku_ldmled_sys_state, NULL);

static ssize_t roku_ldmled_boost_algregs(struct device* device, struct device_attribute* attr, char* buf) {
    struct algoregs * alg_regs = RokuBoost_get_algregs();
    int len = scnprintf(buf, PAGE_SIZE,
            "MinDimmingKnee:    %d\n"
            "MaxBoostKnee:      %d\n"
            "DimmingSlopeGain:  %d\n"
            "BoostSlopeGain:    %d\n"
            "PSCurrentLimit:    %d\n"
            "regMinCurrent:     %d\n"
            "BoostGain:         %d\n"
            "TempFilterDCEnable:%d\n"
            "UpGain:            %d\n"
            "DownGain:          %d\n"
            "ApplyTfilter2PWM:  %d\n"
            "AdaptKneeSel:      %d\n"
            "MaxDimmingKnee:    %d\n"
            "MinBoostKnee:      %d\n"
            "KneeTFEnable:      %d\n"
            "KneeTFGain:        %d\n"
            "BoostSelect:       %d\n"
            "DimmingEnable:     %d\n"
            "PWM_Max:           %d\n"
            "PWM_Min:           %d\n"
            "PWM_MapEnable:     %d\n"
            "GlobalCurrent:     %d\n"
            "MaxCurrent:        %d\n",
            alg_regs->MinDimmingKnee,
            alg_regs->MaxBoostKnee,
            alg_regs->DimmingSlopeGain,
            alg_regs->BoostSlopeGain,
            alg_regs->PSCurrentLimit,
            alg_regs->regMinCurrent,
            alg_regs->BoostGain,
            alg_regs->TempFilterDCEnable,
            alg_regs->UpGain,
            alg_regs->DownGain,
            alg_regs->ApplyTfilter2PWM,
            alg_regs->AdaptKneeSel,
            alg_regs->MaxDimmingKnee,
            alg_regs->MinBoostKnee,
            alg_regs->KneeTFEnable,
            alg_regs->KneeTFGain,
            alg_regs->BoostSelect,
            alg_regs->DimmingEnable,
            alg_regs->PWM_Max,
            alg_regs->PWM_Min,
            alg_regs->PWM_MapEnable,
            alg_regs->GlobalCurrent,
            alg_regs->MaxCurrent);

     len += scnprintf(buf + len, PAGE_SIZE - len,
            "FSGain:           %d\nFSThreshold:      %d\nLDReduceSel:      %d\nBoostMaxAZL:     %d\nBoostMinAZL:       %d\n"
            "DimmingMinAZL:    %d\nDimmingMaxAZL:    %d\n",
            alg_regs->FSGain,
            alg_regs->FSThreshold,
            alg_regs->LDReduceSel,
            alg_regs->BoostMaxAZL,
            alg_regs->BoostMinAZL,
            alg_regs->DimmingMinAZL,
            alg_regs->DimmingMaxAZL
            );
    return len;
}

static DEVICE_ATTR(boost_algregs, 0444, roku_ldmled_boost_algregs, NULL);

ssize_t roku_ldmled_get_led_input(struct device *dev, struct device_attribute *attr, char *buf) {
    mutex_lock(&s_runtime.mutex);
    int i = 0;
    int len = 0;
    int zones = board_info.row * board_info.column;
    while (i < zones) {
        len += snprintf(buf + len, PAGE_SIZE - len, "0x%04x, ", s_runtime.zone_bufs[BUF_LED_INPUT][i]);
        if ((++i) % board_info.column == 0)
            len += snprintf(buf + len, PAGE_SIZE - len, "\n");
    }
    mutex_unlock(&s_runtime.mutex);
    return len;
}

static DEVICE_ATTR(led_input, 0444, roku_ldmled_get_led_input, NULL);

static ssize_t roku_ldmled_get_pwm_output(struct device *dev, struct device_attribute *attr, char *buf) {
    mutex_lock(&s_runtime.mutex);
    int i = 0;
    int len = 0;
    int zones = board_info.row * board_info.column;
    while (i < zones) {
        len += snprintf(buf + len, PAGE_SIZE - len, "0x%04x, ", s_runtime.zone_bufs[BUF_PWM_OUTPUT][i]);
        if ((++i) % board_info.column == 0)
            len += snprintf(buf + len, PAGE_SIZE - len, "\n");
    }
    mutex_unlock(&s_runtime.mutex);
    return len;
}

static DEVICE_ATTR(pwm_output, 0444, roku_ldmled_get_pwm_output, NULL);

static ssize_t roku_ldmled_get_current_output(struct device *dev, struct device_attribute *attr, char *buf) {
    mutex_lock(&s_runtime.mutex);
    int i = 0;
    int len = 0;
    int zones = board_info.row * board_info.column;
    while (i < zones) {
        len += snprintf(buf + len, PAGE_SIZE - len, "0x%04x, ", s_runtime.zone_bufs[BUF_CURRENT_OUTPUT][i]);
        if ((++i) % board_info.column == 0)
            len += snprintf(buf + len, PAGE_SIZE - len, "\n");
    }
    mutex_unlock(&s_runtime.mutex);
    return len;
}

static DEVICE_ATTR(current_output, 0444, roku_ldmled_get_current_output, NULL);

static ssize_t roku_ldmled_get_delay_output(struct device *dev, struct device_attribute *attr, char *buf) {
    int i = 0;
    int len = 0;

    int total_leds = board_info.row * board_info.column;
    for (i = 0; i < total_leds; ++i) {
        len += snprintf(buf + len, PAGE_SIZE - len, "0x%06x, ", s_runtime.delay_buf[i]);
        if ((i + 1) % board_info.column == 0) {
            len += snprintf(buf + len, PAGE_SIZE - len, "\n");
        }
    }
    return len;
}

static DEVICE_ATTR(delay_output, 0444, roku_ldmled_get_delay_output, NULL);

static int __init roku_ldmled_probe(struct platform_device* pdev) {
    device_create_file(&pdev->dev, &dev_attr_board_info);
    device_create_file(&pdev->dev, &dev_attr_sys_state);
    device_create_file(&pdev->dev, &dev_attr_boost_config);
    device_create_file(&pdev->dev, &dev_attr_boost_algregs);
    device_create_file(&pdev->dev, &dev_attr_portal);
    device_create_file(&pdev->dev, &dev_attr_led_input);
    device_create_file(&pdev->dev, &dev_attr_pwm_output);
    device_create_file(&pdev->dev, &dev_attr_current_output);
    device_create_file(&pdev->dev, &dev_attr_delay_output);
    return 0;
}
static int roku_ldmled_remove(struct platform_device* pdev) {
    device_remove_file(&pdev->dev, &(dev_attr_board_info));
    device_remove_file(&pdev->dev, &(dev_attr_sys_state));
    device_remove_file(&pdev->dev, &(dev_attr_boost_config));
    device_remove_file(&pdev->dev, &(dev_attr_boost_algregs));
    device_remove_file(&pdev->dev, &(dev_attr_portal));
    device_remove_file(&pdev->dev, &(dev_attr_led_input));
    device_remove_file(&pdev->dev, &(dev_attr_pwm_output));
    device_remove_file(&pdev->dev, &(dev_attr_current_output));
    device_remove_file(&pdev->dev, &(dev_attr_delay_output));
    return 0;
}

static void roku_ldmled_shutdown(struct platform_device* pdev) {
}

static int roku_ldmled_resume(struct platform_device* dev) {
    return 0;
}

static struct platform_driver roku_ldmled_driver = {
        .driver = {
                .owner = THIS_MODULE,
                .name = "roku_ldmled",
        },
        .remove = roku_ldmled_remove,
        .shutdown = roku_ldmled_shutdown,
        .resume = roku_ldmled_resume,
};

static int ldmled_open(struct inode * inode, struct file * file)
{
    return 0;
}

static int ldmled_close(struct inode * inode, struct file * file)
{
    return 0;
}

static long ldmled_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    void __user *argp = (void __user *)arg;
    switch(cmd) {
    case LDMLED_GET_LOG_BUFFER:
    {
        struct ring_buffer buffer;
        if (copy_from_user(&buffer, argp, sizeof(struct ring_buffer)))
            return -EFAULT;
        int buf_size = buffer.buf_size;
        uint8_t *buf = kmalloc(buf_size, GFP_KERNEL);
        if (!buf) return -ENOMEM;
        ret = debug_read_buffer(&s_debug, buffer.buf_type, buf, buf_size);
        if (copy_to_user(argp, buf, buf_size)) ret = -EFAULT;
        kfree(buf);
        break;
    }
    case LDMLED_SET_LOG_BUFFER:
    {
        struct ring_buffer buffer;
        if (copy_from_user(&buffer, argp, sizeof(struct ring_buffer)))
            return -EFAULT;
        int buf_size = buffer.buf_size;
        uint8_t *buf = kmalloc(buf_size, GFP_KERNEL);
        if (!buf) return -ENOMEM;
        if (copy_from_user(buf, argp, buf_size)) {
            kfree(buf);
            return -EFAULT;
        }
        ret = debug_write_buffer(&s_debug, buffer.buf_type, buf, buf_size);
        kfree(buf);
        break;
    }
    case LDMLED_GET_SPI_DEVICE:
    {
        int buf_size;
        if (copy_from_user(&buf_size, argp, sizeof(int))) return -EFAULT;
        if (!buf_size) return -EINVAL;

        char buffer[buf_size];
        if (copy_from_user(buffer, argp, buf_size)) return -EFAULT;
        struct spi_device_io *dev_io = (struct spi_device_io*) buffer;
        struct spi_device_reg *dev_reg = dev_io->dev;
        ret = s_runtime.dev->read_reg(s_runtime.dev, dev_reg->dev_id,
                                dev_reg->dev_addr, dev_reg->payload, dev_reg->payload_size);

        if (copy_to_user(argp, buffer, buf_size)) return -EFAULT;
        break;
    }
    case LDMLED_SET_SPI_DEVICE:
    {
        int buf_size;
        if (copy_from_user(&buf_size, argp, sizeof(int))) return -EFAULT;
        if (!buf_size) return -EINVAL;
        char buffer[buf_size];

        if (copy_from_user(buffer, argp, buf_size)) return -EFAULT;
        struct spi_device_io *dev_io = (struct spi_device_io*) buffer;
        struct spi_device_reg *dev_reg = dev_io->dev;

        ret = s_runtime.dev->write_reg(s_runtime.dev, dev_reg->dev_id,
                                dev_reg->dev_addr, dev_reg->payload,
                                dev_reg->payload_size);
        break;
    }
    case LDMLED_INIT_MAPPING:
    {
        int zones = board_info.row * board_info.column;
        int buf_size = sizeof(int) * zones;
        int *mapping = kmalloc(buf_size, GFP_KERNEL);
        if (!mapping) return -ENOMEM;
        if (copy_from_user(mapping, argp, buf_size)) {
            kfree(mapping);
            return -EFAULT;
        }
        ret = s_runtime.dev->init_mapping(s_runtime.dev, mapping, zones);
        kfree(mapping);
        break;
    }
    case LDMLED_SET_BFI_CONTROL:
    {
        struct bfi_control bfi;
        mutex_lock(&s_runtime.mutex);
        if (copy_from_user(&bfi, argp, sizeof(struct bfi_control))) {
            mutex_unlock(&s_runtime.mutex);
            printk("BFI structure copy error!\n");
            return -EFAULT;
        }
        printk("bfi setting: %u %u %u %u %u\n", bfi.bfi_on, bfi.blank_time, bfi.start_delay, bfi.row_delay, bfi.column_delay);
        ret = roku_ldmled_bfi_conrol(&bfi);
        mutex_unlock(&s_runtime.mutex);
        break;
    }
    case LDMLED_GET_DEDBUG_MODE:
    {
        int debug_mode;
        debug_mode = debug_get_debug_mode(&s_debug);
        if (copy_to_user(argp, &debug_mode, sizeof(int)))
            return -EFAULT;
        break;
    }
    case LDMLED_SET_DEDBUG_MODE:
    {
        int debug_mode;
        if (copy_from_user(&debug_mode, argp, sizeof(int))) return -EFAULT;
        ret = debug_set_debug_mode(&s_debug, debug_mode);
        break;
    }
    case LDMLED_GET_LOG_FRAMES:
    {
        uint16_t log_frames[BUF_LAST];
        ret = debug_get_log_frames(&s_debug, log_frames);
        if (copy_to_user(argp, log_frames, sizeof(log_frames)))
            return -EFAULT;
        break;
    }
    case LDMLED_SET_LOG_FRAMES:
    {
        uint16_t log_frames[BUF_LAST];
        if (copy_from_user(log_frames, argp, sizeof(log_frames)))
            return -EFAULT;
        ret = debug_set_log_frames(&s_debug, log_frames);
        break;
    }
    case LDMLED_SET_SYSSTATE:
    {
        mutex_lock(&s_runtime.mutex);
        if (copy_from_user(&sys_state, argp, sizeof(struct sysstateregs))) ret = -EFAULT;
        mutex_unlock(&s_runtime.mutex);
        break;
    }
    case LDMLED_SET_BOOST_ALGREGS:
    {
        struct algoregs algs;
        mutex_lock(&s_runtime.mutex);
        if (copy_from_user(&algs, argp, sizeof(struct algoregs))) {
            mutex_unlock(&s_runtime.mutex);
            return -EFAULT;
        }
        RokuBoost_set_algregs(&algs);
        mutex_unlock(&s_runtime.mutex);
        break;
    }
    case LDMLED_GET_PROFILE:
    {
        struct profiling_buffer buffer;
        if (copy_from_user(&buffer, argp, sizeof(struct profiling_buffer)))
            return -EFAULT;
        int buf_size = buffer.buf_size;
        uint8_t *buf = kmalloc(buf_size, GFP_KERNEL);
        if (!buf) return -ENOMEM;
        ret = debug_profiling_read(&s_debug, buf, buf_size);
        if (copy_to_user(argp, buf, buf_size)) ret = -EFAULT;
        kfree(buf);
        break;
    }
    case LDMLED_SET_PROFILE:
    {
        struct profiling_buffer buffer;
        if (copy_from_user(&buffer, argp, sizeof(struct profiling_buffer)))
            return -EFAULT;
        ret = debug_profiling_create(&s_debug, buffer.total_frames);
        break;
    }
    case LDMLED_SET_POWER_CONFIG:
    {
        mutex_lock(&s_runtime.mutex);
        struct ldmled_power_config power_config;
        if (copy_from_user(&power_config, argp, sizeof(struct ldmled_power_config))) {
            mutex_unlock(&s_runtime.mutex);
            return -EFAULT;
        }
        s_runtime.dev->current_max = power_config.current_max;
        boost_sysconf.PSCurrentMax  = power_config.power_max;
        boost_sysconf.GlobalDriveCurrent = power_config.current_init;
        mutex_unlock(&s_runtime.mutex);
        break;
    }
    case LDMLED_SET_TESTPATTERN:
    {
        mutex_lock(&s_runtime.mutex);
        struct ldmled_test_pattern test_pattern;
        if (copy_from_user(&test_pattern, argp, sizeof(struct ldmled_test_pattern))) {
            mutex_unlock(&s_runtime.mutex);
            return -EFAULT;
        }
        debug_set_test_pattern(&s_debug, &test_pattern, s_runtime.zone_bufs);
        //output data to device
        int zones = board_info.row * board_info.column;
        struct spi_ldm_device *dev = s_runtime.dev;
        dev->update_device(dev, s_runtime.zone_bufs[BUF_PWM_OUTPUT], s_runtime.zone_bufs[BUF_CURRENT_OUTPUT], zones, &sys_state);
        mutex_unlock(&s_runtime.mutex);
        break;
    }
    case LDMLED_SET_BOARD_ON :
    {
        int onoff;
        if (copy_from_user(&onoff, argp, sizeof(onoff))) {
            return -EFAULT;
        }
        roku_ldmled_board_on(onoff != 0);
        break;
    }
    case LDMLED_SET_OUTPUT_GPIO :
    {
        int onoff;
        if (s_runtime.dev->gpio_output_pin < 0) {
            return -ENODEV;
        }
        if (copy_from_user(&onoff, argp, sizeof(onoff))) {
            return -EFAULT;
        }
        printk("ldmled, ougput gpio %d is set to %d\n", s_runtime.dev->gpio_output_pin, onoff);
        gpio_set_value(s_runtime.dev->gpio_output_pin, onoff ? 1 : 0);
        break;
    }
    case LDMLED_GET_INPUT_GPIO :
    {
        int onoff;
        if (s_runtime.dev->gpio_input_pin < 0) {
            return -ENODEV;
        }
        onoff = gpio_get_value(s_runtime.dev->gpio_input_pin);
        if (copy_to_user(argp, &onoff, sizeof(onoff))) {
            return -EFAULT;
        }
        printk("ldmled, input gpio %d is %d\n", s_runtime.dev->gpio_input_pin, onoff);
        break;
    }
    case LDMLED_CLEAR_FAULT :
    {
        struct spi_ldm_device *dev = s_runtime.dev;
        // clear fault only
        ret = dev->handle_fault(dev, NULL, true);
        if (ret) {
            printk(KERN_ERR "ldmled, failed to process fault info.\n");
        }

        break;
    }
    case LDMLED_PROCESS_FAULT :
    {
        struct spi_ldm_device *dev = s_runtime.dev;
        struct fault_reason reason;
        // invoke full process, including disable LED channels
        ret = dev->handle_fault(dev, &reason, false);
        if (!ret) {
            if (copy_to_user(argp, &reason, sizeof(reason))) {
                return -EFAULT;
            }
        } else {
            printk(KERN_ERR "ldmled, failed to process fault info.\n");
        }

        break;
    }

    default:
        return -EINVAL;
    }
    return ret;
}

static struct file_operations ldmled_fops = {
    .open = ldmled_open,
    .unlocked_ioctl = ldmled_ioctl,
    .release = ldmled_close,
};

static void ldmled_device_exit(void) {
    dev_t dev_first = MKDEV(ldmled_device_major, ldmled_device_minor);

    cdev_del(&ldmled_dev);
    unregister_chrdev_region(dev_first, 1);
    class_destroy(ldmled_class);

    printk(KERN_INFO "ldmled_device: module unloaded\n");
}

static int __init ldmled_device_init(void) {
    dev_t dev_first = 0;
    int result = 0;
    result = alloc_chrdev_region(&dev_first, ldmled_device_minor, 1, ldmled_device_name);
    ldmled_device_major = MAJOR(dev_first);
    if (result < 0) {
        printk(KERN_WARNING "ldmled_device: can't get major number %d\n", ldmled_device_major);
        return result;
    }
    ldmled_class = class_create(THIS_MODULE, ldmled_device_name);
    struct device * dev = device_create(ldmled_class, NULL, dev_first, NULL, ldmled_device_name);

    cdev_init(&ldmled_dev, &ldmled_fops);
    ldmled_dev.owner = THIS_MODULE;
    ldmled_dev.ops = &ldmled_fops;
    result = cdev_add(&ldmled_dev, dev_first, 1);

    if (result < 0) {
        printk(KERN_WARNING "ldmled_device: error %d adding ldmled_device", result);
        return result;
    }
    printk(KERN_INFO "ldmled_device: module loaded\n");
    return 0;
}

static int __init ldmled_runtime_init(void) {
    void * panel_cfg_pa = (void *)(ROKU_SHARE_MEM_GET_PANEL_CONFIG_DATA);
    void * panel_cfg_va = NULL;
    panel_cfg_va = ioremap((unsigned long)panel_cfg_pa, ROKU_PANEL_CONFIG_DATA_SIZE);
    if (panel_cfg_va) {
        __roku_panel_cfg * cfg = (__roku_panel_cfg *)panel_cfg_va;
        memcpy_fromio(&board_info, &cfg->ldm_board_info, sizeof(__ldm_board_info));
        pr_info("[ldmled] init row=%d, column=%d, mcu_less=%d\n", board_info.row, board_info.column, board_info.mcu_less);
        pr_info("[ldmled] init device_name=%s, bus_num=%d, device_total=%d\n", board_info.device_name, board_info.bus.bus_num, board_info.bus.device_total);
        iounmap(panel_cfg_va);
    }
    if (!board_info.mcu_less || !board_info.row || !board_info.column) {
        printk(KERN_ERR "Wrong board info\n");
        return -EINVAL;
    }
    mutex_init(&s_runtime.mutex);
    s_runtime.dev = probe_spi_ldm_device(board_info.device_name, &board_info.bus);
    if (!s_runtime.dev) return -EXDEV;
    s_runtime.dev->current_max = board_info.current_max;
    s_runtime.dev->current_max = board_info.current_max;
    int zones = board_info.row * board_info.column;

    if (s_runtime.dev->bus->device_total) {
        s_runtime.dev->enabled_channels_per_device =  (zones + s_runtime.dev->bus->device_total - 1) / s_runtime.dev->bus->device_total;
    } else {
        s_runtime.dev->enabled_channels_per_device = s_runtime.dev->channels_per_device;
    }

    //GPIO
    s_runtime.dev->gpio_output_pin = board_info.gpio_output_pin;
    if (s_runtime.dev->gpio_output_pin >= 0) {
        int ret = gpio_request(s_runtime.dev->gpio_output_pin, "ldmled_gpio_output");
        if (ret == 0) {
            gpio_direction_output(s_runtime.dev->gpio_output_pin, board_info.gpio_output_pin_ini_level);
            gpio_free(s_runtime.dev->gpio_output_pin);
        } else {
            pr_err("[ldmled] gpio_request fail, pin=%d, ret=%d\n", s_runtime.dev->gpio_output_pin, ret);
        }
    }
    s_runtime.dev->gpio_input_pin = board_info.gpio_input_pin;
    if (s_runtime.dev->gpio_input_pin >= 0) {
        int ret = gpio_request(s_runtime.dev->gpio_input_pin, "ldmled_gpio_input");
        if (ret == 0) {
            gpio_direction_input(s_runtime.dev->gpio_input_pin);
            gpio_free(s_runtime.dev->gpio_input_pin);
        } else {
            pr_err("[ldmled] gpio_request fail, pin=%d, ret=%d\n", s_runtime.dev->gpio_input_pin, ret);
        }
    }

    //default mapping
    if (s_runtime.dev->init_mapping(s_runtime.dev, NULL, zones)) {
        return -ENOMEM;
    }

    uint16_t *buf = kmalloc(sizeof(uint16_t) * zones * BUF_LAST, GFP_KERNEL);
    if (!buf) return -ENOMEM;
    int i;
    for (i = 0; i < BUF_LAST; ++i) s_runtime.zone_bufs[i] = buf + zones * i;
    uint32_t *delay_buf = kmalloc(sizeof(uint32_t) * zones, GFP_KERNEL);
    if (!delay_buf) return -ENOMEM;
    s_runtime.delay_buf = delay_buf;
    debug_init(&s_debug, board_info.row, board_info.column);

    boost_sysconf.hzones =  board_info.column;
    boost_sysconf.vzones =  board_info.row;
    boost_sysconf.zones = board_info.column * board_info.row;
    boost_sysconf.GlobalDriveCurrent = board_info.current_init;
    boost_sysconf.PSCurrentMax  = (uint64_t)board_info.power_max;
    RokuBoost_init(&boost_sysconf);
    return 0;
}

static void ldmled_runtime_exit(void) {
    if(s_runtime.dev->mapping) kfree(s_runtime.dev->mapping);
    if (s_runtime.zone_bufs[0]) kfree(s_runtime.zone_bufs[0]);
    debug_free_buffer(&s_debug);
}

static int __init roku_ldmled_init(void) {
    pr_info("roku local dimming led driver init\n");
    return (platform_driver_probe(&roku_ldmled_driver, roku_ldmled_probe));
}

static void __exit roku_ldmled_exit(void) {
    ldmled_runtime_exit();
    ldmled_device_exit();
    platform_driver_unregister(&roku_ldmled_driver);
}

static int __init roku_ldmled_core_init(void) {
    pr_info("roku_ldmled core init\n");
    int result = ldmled_runtime_init();
    if (result < 0) {
        pr_err("roku local dimming led driver init fail: ldmled_runtime_init :%d\n", result);
        s_runtime.dev = NULL;
        return result;
    }
    result = ldmled_device_init();
    if (result < 0) {
        pr_err("roku local dimming led driver init fail: ldmled_device_init :%d\n", result);
        return result;
    }
    return 0;
}

int __init roku_ldmled_init_after_spi_inited(int bus_num) {
    if (s_runtime.power_on) {
        if (s_runtime.dev && s_runtime.dev->bus->bus_num == bus_num) {
            pr_info("[ldmled] AC power on to turn on ldmled board\n");
            roku_ldmled_board_on(true);
            return 0;
        }
    }
    return -1;
}
EXPORT_SYMBOL(roku_ldmled_init_after_spi_inited);

subsys_initcall(roku_ldmled_core_init);

module_init(roku_ldmled_init);
module_exit(roku_ldmled_exit);

MODULE_AUTHOR("Roku, Inc.");
MODULE_DESCRIPTION("Roku ldmled");
MODULE_LICENSE("GPL");
