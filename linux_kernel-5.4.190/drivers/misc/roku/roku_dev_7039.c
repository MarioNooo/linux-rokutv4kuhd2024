#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/spi/spidev.h>
#include <linux/spi/spi.h>
#include <linux/bitops.h>
#include "roku_spi_ldm_device.h"
#include <linux/gpio.h>

struct spi_device *km_spidev_node_get(char channel);
bool     bfi_on_ = false;
uint16_t bfi_dutygain_ = 0;
bool     ldm_on_ = false;

enum fault_type {
    FAULT_ADJ_SHORT = 0,
    FAULT_SHORT,
    FAULT_OPEN,
    FAULT_SPI_ERR,
    FAULT_MISS_VSYNC,
    FAULT_MISS_HSYNC,
    FAULT_OT,
    FAULT_OH,
    FAULT_EFUSE,
    FAULT_LAST
};

static const char *fault_desc[] = {
    [FAULT_ADJ_SHORT]   = "ADSHRT",
    [FAULT_SHORT]       = "SHORT",
    [FAULT_OPEN]        = "OPEN",
    [FAULT_SPI_ERR]     = "SPIERR",
    [FAULT_MISS_VSYNC]  = "MISSVS",
    [FAULT_MISS_HSYNC]  = "MISSHS",
    [FAULT_OT]          = "OT",
    [FAULT_OH]          = "OH",
    [FAULT_EFUSE]       = "EFUSE"
};

static int dev_7039_read_words(struct spi_ldm_device *dev, int dev_id, int reg,
                             uint16_t *buf, size_t buf_size) {
    mutex_lock(&dev->mutex);
    int dummy = (dev->bus->device_total + 15) / 16 * 2;
    uint8_t w_buf[4 + dummy];
    int w_size = 0;

    w_buf[w_size++] = dev_id | 0x80;
    w_buf[w_size++] = buf_size;

    w_buf[w_size++] = (reg >> 8) | 0x80;
    w_buf[w_size++] = reg & 0xff;

    size_t i = 0;
    for (i = 0; i < dummy; ++i) {
        w_buf[w_size++] = 0x00;
    }

    int status;
    struct spi_transfer x[2];
    struct spi_message message;
    struct spi_device *spi_node;

    spi_message_init(&message);
    memset(x, 0, sizeof(x));

    x[0].tx_buf    = w_buf;
    x[0].len        = w_size;
    x[0].tx_nbits    = SPI_NBITS_SINGLE;
    x[0].rx_nbits    = SPI_NBITS_SINGLE;
    x[0].speed_hz    = 0;
    spi_message_add_tail(&x[0], &message);

    uint8_t r_buf[buf_size * 2];
    x[1].rx_buf    = r_buf;
    x[1].len        = buf_size * 2;
    x[1].tx_nbits    = SPI_NBITS_SINGLE;
    x[1].rx_nbits    = SPI_NBITS_SINGLE;
    x[1].speed_hz    = 0;
    spi_message_add_tail(&x[1], &message);

    spi_node = km_spidev_node_get(dev->bus->bus_num);
    status = spi_sync(spi_node, &message);
    mutex_unlock(&dev->mutex);

    for (i = 0; i < buf_size; i++) {
        buf[i] = (r_buf[i*2] << 8) + r_buf[i*2 + 1];
    }

    return status;
}
static int dev_7039_write_Device_Number(struct spi_ldm_device *dev) {
    mutex_lock(&dev->mutex);
    int dummy = (dev->bus->device_total + 15) / 16 * 2;

    uint8_t w_buf[ 2 + dummy];
    int w_size = 0;
    w_buf[w_size++] = 0x7E | 0x80;
    w_buf[w_size++] = dev->bus->device_total;


    size_t i = 0;
    for (i = 0; i < dummy; ++i) {
        w_buf[w_size++] = 0x00;
    }

    struct spi_device *spi_node;
    int status;
    struct spi_message msg;
    struct spi_transfer spixfer;

    memset(&spixfer, 0, sizeof(struct spi_transfer));
    spixfer.tx_buf = w_buf;
    spixfer.len        = w_size;
    spixfer.tx_nbits    = SPI_NBITS_SINGLE;
    spixfer.rx_nbits    = SPI_NBITS_SINGLE;
    spixfer.speed_hz    = 0;
    spi_node = km_spidev_node_get(dev->bus->bus_num);
    spi_message_init(&msg);
    spi_message_add_tail(&spixfer, &msg);
    status = spi_sync(spi_node, &msg);

    mutex_unlock(&dev->mutex);
    return status;
}


static int dev_7039_write_words(struct spi_ldm_device *dev, int dev_id, int reg,
                              uint16_t *buf, size_t buf_size) {
    mutex_lock(&dev->mutex);
    int dummy = (dev->bus->device_total + 7) / 8;

    uint8_t w_buf[ 2 + 2 + 2 * buf_size + dummy];
    int w_size = 0;
    w_buf[w_size++] = dev_id | 0x80;
    w_buf[w_size++] = (dev_id == 0x7f)
            ? buf_size / dev->bus->device_total
            : buf_size;

    w_buf[w_size++] = reg >> 8;
    w_buf[w_size++] = reg & 0xff;

    size_t i = 0;
    for (i = 0; i < buf_size; ++i) {
        w_buf[w_size++] = buf[i] >> 8;
        w_buf[w_size++] = buf[i] & 0xff;
    }

    for (i = 0; i < dummy; ++i) {
        w_buf[w_size++] = 0x00;
    }

    struct spi_device *spi_node;
    int status;
    struct spi_message msg;
    struct spi_transfer spixfer;

    memset(&spixfer, 0, sizeof(struct spi_transfer));
    spixfer.tx_buf = w_buf;
    spixfer.len        = w_size;
    spixfer.tx_nbits    = SPI_NBITS_SINGLE;
    spixfer.rx_nbits    = SPI_NBITS_SINGLE;
    spixfer.speed_hz    = 0;
    spi_node = km_spidev_node_get(dev->bus->bus_num);
    spi_message_init(&msg);
    spi_message_add_tail(&spixfer, &msg);
    status = spi_sync(spi_node, &msg);

    mutex_unlock(&dev->mutex);
    return status;
}

static int dev_7039_read_reg(struct spi_ldm_device *dev, int dev_id, int reg,
                             uint32_t *buf, size_t buf_size) {
    uint16_t r_buf[buf_size];
    int ret = dev_7039_read_words(dev, dev_id, reg, r_buf, buf_size);
    size_t i = 0;
    for (i = 0; i < buf_size; i++) {
        buf[i] = r_buf[i];
    }
    return ret;
}

static int dev_7039_write_reg(struct spi_ldm_device *dev, int dev_id, int reg,
                              uint32_t *buf, size_t buf_size) {
    uint16_t w_buf[buf_size];
    size_t i = 0;
    for (i = 0; i < buf_size; i++) {
        w_buf[i] = buf[i];
    }
    return dev_7039_write_words(dev, dev_id, reg, w_buf, buf_size);
}

static int dev_7039_write_pwm(struct spi_ldm_device *dev, uint16_t *pwm_buf,
                              size_t buf_size) {
    // For 7039, we set PWM_PER to fix value 0xFFE, HT = 0xFFF meansing duty = 100%
    uint16_t w_buf[buf_size];

    int i = 0;
    if (bfi_on_)  {//bfi is on
        for (i = 0; i < buf_size; ++i) {
            unsigned int pwm = (pwm_buf[dev->mapping[i]] >> 4);
            pwm = (pwm * bfi_dutygain_) >> 16;
            w_buf[i] = 0xFFF - pwm;
        }
    } else {
        for (i = 0; i < buf_size; ++i) {
            w_buf[i] = pwm_buf[dev->mapping[i]] >> 4;  // 16bits to 12bits
        }
    }
    dev_7039_write_words(dev, 0x7f, 0x60, w_buf, buf_size);
    return 0;
}

static int dev_7039_update_device(struct spi_ldm_device *dev, uint16_t *pwm_buf,
                              uint16_t *current_buf, size_t buf_size, struct sysstateregs* sysstate) {
    dev_7039_write_pwm(dev, pwm_buf, buf_size);

    uint16_t w_buf[buf_size];
    int i = 0;
    for (i = 0; i < buf_size; ++i) {
        unsigned int current_val = current_buf[dev->mapping[i]];
        current_val *= dev->current_max;
        w_buf[i] = current_val >> 22;  // 10 bits value
    }
    dev_7039_write_words(dev, 0x7f, 0x20, w_buf, buf_size);

    // change ADP setting base on local dimming on/off
    bool ldm_on_new = sysstate->LDStrength != 0;
    if (ldm_on_new != ldm_on_) {
        ldm_on_ = ldm_on_new;
        uint16_t buf_reg_18[2];
        if (ldm_on_) {
             buf_reg_18[0] = buf_reg_18[1] = 0x0;
        } else {
            buf_reg_18[0] = buf_reg_18[1] = 0xFFFF;
        }
        dev_7039_write_words(dev, 0, 0x18, buf_reg_18, ARRAY_SIZE(buf_reg_18));
    }
    return 0;
}

static int dev_7039_init(struct spi_ldm_device *dev) {
    int ret = 0;
    printk("[7039] init local dimming board spi devices...\n");
    uint16_t buf_device_num[dev->bus->device_total];
    memset(buf_device_num, 0, sizeof(buf_device_num));
    uint16_t buf_reg_00[] = {
        0x0802, 0x03A2, 0x0000, 0x0FFE, 0x3FFB, 0x4327, 0x4585, 0x843D, //0x00 ~ 0x07
        0x0100, 0xA100, 0x0000, 0x0830, 0xCCE4, 0x5840, 0x3335, 0xCC8A, //0x08 ~ 0x0F
        0xFFFF, 0xFFFF, 0x01E1, 0x01E1, 0x0118, 0x0000, 0x0000, 0x0000, //0x10 ~ 0x17
        0xFFFF, 0xFFFF, 0x0000, 0x01FF, 0x6FFF, 0x1FFF, 0xFFFF, 0xFFFF, //0x18 ~ 0x1F
    };
    uint16_t buf_reg_20[] = {
        0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, // 0x20 ~ 0x27
        0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, // 0x28 ~ 0x2F
        0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, // 0x30 ~ 0x37
        0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, // 0x38 ~ 0x3F
    };
    uint16_t buf_reg_40[] = {
        0x0000, 0x0080, 0x0100, 0x0180, 0x0200, 0x0280, 0x0300, 0x0380, //0x40 ~ 0x47
        0x0400, 0x0480, 0x0500, 0x0580, 0x0600, 0x0680, 0x700,  0x0780, //0x48 ~ 0x4F
        0x0800, 0x0880, 0x0900, 0x1980, 0x0A00, 0x0A80, 0x0B00, 0x0B80, //0x50 ~ 0x57
        0x0C00, 0x0C80, 0x0D00, 0x0D80, 0x0E00, 0x0E80, 0x0F00, 0x0F80, //0x58 ~ 0x5F
    };
    uint16_t buf_reg_60[] = {
        0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, // 0x61 ~ 0x67
        0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, // 0x68 ~ 0x6F
        0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, // 0x70 ~ 0x77
        0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, 0x82E, // 0x78 ~ 0x7F
    };

    uint16_t reg_val;
    if (dev->gpio_output_pin >= 0) {
        gpio_set_value(dev->gpio_output_pin, 1);
    }
    //Need enable EN pin first
    mdelay(9);

    if (dev->enabled_channels_per_device < dev->channels_per_device) {
        uint16_t mask = 0xFFFF >> (dev->channels_per_device - dev->enabled_channels_per_device);
        buf_reg_00[0x10] = mask;    //CURR_SW[31:16] Channel enable control
        buf_reg_00[0x15] = ~mask;  //mask_ch
        buf_reg_00[0x1E] = mask;   //FB_ON
    }

    //0x7E, total device number indicate command
    ret |= dev_7039_write_Device_Number(dev);

    ret |= dev_7039_write_words(dev, 0, 0x00, buf_reg_00, ARRAY_SIZE(buf_reg_00));
    reg_val = 0xA5FF;
    ret |= dev_7039_write_words(dev, 0, 0xA0, &reg_val, 1);
    ret |= dev_7039_write_words(dev, 0, 0x20, buf_reg_20, ARRAY_SIZE(buf_reg_20));
    ret |= dev_7039_write_words(dev, 0, 0x40, buf_reg_40, ARRAY_SIZE(buf_reg_40));
    ret |= dev_7039_write_words(dev, 0, 0x60, buf_reg_60, ARRAY_SIZE(buf_reg_60));
    reg_val = 0xA5FF;
    ret |= dev_7039_write_words(dev, 0, 0xA0, &reg_val, 1);

    //Soft resetting
    mdelay(30);
    reg_val = 0x803;
    ret |= dev_7039_write_words(dev, 0, 0x00, &reg_val, 1);

    bfi_on_ = false;
    ldm_on_ = false;
    return ret;
}

static int dev_7039_exit(struct spi_ldm_device *dev) {
    int ret = 0;
    uint16_t buf_reg_60[] = {
        0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, // 0x60 ~ 0x67
        0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, // 0x68 ~ 0x6F
        0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, // 0x70 ~ 0x77
        0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, // 0x78 ~ 0x7F
    };
    ret |= dev_7039_write_words(dev, 0, 0x60, buf_reg_60, ARRAY_SIZE(buf_reg_60));
    if (dev->gpio_output_pin >= 0) {
        gpio_set_value(dev->gpio_output_pin, 0);
    }

    return ret;
}


static int dev_7039_init_mapping(struct spi_ldm_device *dev, int *mapping,
                                 size_t size) {
    int buf_size = sizeof(uint16_t) * dev->enabled_channels_per_device * dev->bus->device_total;
    if (dev->mapping == NULL) {
        dev->mapping = kmalloc(buf_size, GFP_KERNEL);
    }
    if (!dev->mapping) return -ENOMEM;
    memset(dev->mapping, 0, buf_size);
    int i = 0;
    if (mapping == NULL) {
        for (i = 0; i < size; ++i) {
            dev->mapping[i] = i;
        }
    } else {
        for (i = 0; i < size; ++i) {
            int dev_id = (mapping[i] >> 8) & 0xff;
            int dev_channel = mapping[i] & 0xff;
            int phy = dev_id * dev->enabled_channels_per_device + dev_channel;
            if (phy >= size) return -EACCES;
            dev->mapping[phy] = i;
        }
    }
    return 0;
}

int dev_7039_write_bfi_control(struct spi_ldm_device* dev, bool bfi_on, uint16_t blank_time,
                               uint32_t* buf, size_t buf_size) {
    uint16_t buf_reg_04[2];
    uint16_t buf_reg_00[1];
    if (bfi_on) {
        //PWM freq change to 120hz, 1 period  of PWM in  one VSYNC
        buf_reg_04[0] = 0x0FFE;
        buf_reg_04[1] = 0x4027;
        buf_reg_00[0] = 0x080B; //using invert mode, which blank first and then high time.
        bfi_dutygain_ = 0xFFFF - blank_time;
    } else {
        //PWM freq change to 480hz, 4 periods of PWM one VSYNC 
        buf_reg_04[0] = 0x3FFB;
        buf_reg_04[1] = 0x4327;
        buf_reg_00[0] = 0x0803;
        bfi_dutygain_ = 0;
    }
    bfi_on_ = bfi_on;
    dev_7039_write_words(dev, 0, 0x04, buf_reg_04, ARRAY_SIZE(buf_reg_04));
    dev_7039_write_words(dev, 0, 0x00, buf_reg_00, ARRAY_SIZE(buf_reg_00));

    uint16_t w_buf[buf_size];
    int i = 0;
    for (i = 0; i < buf_size; ++i) {
        w_buf[i] = buf[dev->mapping[i]] >> 4;  // 16bits to 12bits
    }
    dev_7039_write_words(dev, 0x7f, 0x40, w_buf, buf_size);
    return 0;
}

static int dev_7039_handle_fault(struct spi_ldm_device *dev, struct fault_reason* reason, bool clear_only) {
    int ret = 0;
    int id = 0, i = 0;
    uint16_t buf[1];

    size_t fault_cnt = 0;
    unsigned long fault_mask = 0;

    for (id=1; id <= dev->bus->device_total; ++id) {
        uint16_t ch_mask[2] = {0};
        ret = dev_7039_read_words(dev, id, 0x342, buf, 1);
        if (ret) {
            printk(KERN_ALERT "Failed to read fault status of dev %x\n", id);
            continue;
        }
        fault_mask = buf[0];
        if (fault_mask) {
            if (!clear_only) {
                uint16_t disable_mask[2] = {0};
                ret = dev_7039_read_words(dev, id, 0x10, disable_mask, 2);
                for ( i=0; i<FAULT_LAST; ++i) {
                    if (test_bit(i, &fault_mask)) {
                        switch(i) {
                            case FAULT_OPEN:
                                ret = dev_7039_read_words(dev, id, 0x33c, ch_mask, 2);
                                break;
                            case FAULT_SHORT:
                                ret = dev_7039_read_words(dev, id, 0x33e, ch_mask, 2);
                                break;
                            case FAULT_ADJ_SHORT:
                                ret = dev_7039_read_words(dev, id, 0x340, ch_mask, 2);
                                break;
                            default:
                                break;
                        }

                        // aggregate fault'd channels
                        disable_mask[0] &= ~ch_mask[0];
                        disable_mask[1] &= ~ch_mask[1];

                        if (ret) {
                            printk(KERN_ALERT "Failed to read fault info\n");
                            return ret;
                        }

                        if (reason) {
                            strncpy(reason->records[fault_cnt].desc, fault_desc[i],
                                    FAULT_DESC_LEN);
                            reason->records[fault_cnt].dev_id = id;
                            reason->records[fault_cnt].ch_mask =
                                ((ch_mask[0] << 16) | ch_mask[1]);
                        }

                        ++fault_cnt;
                        if (fault_cnt == MAX_FAULTS_AT_ONCE) {
                            printk(KERN_WARNING "Exceeded maximum fault size, some"
                                    " faults are not reported\n");
                        }
                    }
                }

                // disable channels that has been fault'd
                ret = dev_7039_write_words(dev, id,  0x10, disable_mask, 2);
            }

            // clear fault
            ret &= dev_7039_write_words(dev, id, 0x17, buf, 1);

            if (ret) {
                printk(KERN_ALERT "Failed to clear fault of dev %d\n", id);
            }
        }

        if (fault_cnt == MAX_FAULTS_AT_ONCE) break;
    }

    if (!clear_only && reason) {
        reason->size = fault_cnt;
    }

    return ret;
}




struct spi_ldm_device dev_7039 = {
        .name = "7039",
        .channels_per_device = 32,
        .pwm_bits = 12,
        .current_bits = 10,
        .bus = NULL,
        .init = dev_7039_init,
        .exit = dev_7039_exit,
        .write_reg = dev_7039_write_reg,
        .read_reg = dev_7039_read_reg,
        .init_mapping = dev_7039_init_mapping,
        .write_pwm = dev_7039_write_pwm,
        .update_device = dev_7039_update_device,
        .write_bfi_control = dev_7039_write_bfi_control,
        .handle_fault = dev_7039_handle_fault,
        .mapping = NULL,
        .gpio_output_pin = -1, //GPIO from SOC output to driver IC
        .gpio_input_pin = -1,  //GPIO feeback from driver IC to SOC
};
