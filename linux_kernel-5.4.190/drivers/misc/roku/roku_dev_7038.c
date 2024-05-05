#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/spi/spidev.h>
#include <linux/spi/spi.h>
#include "roku_spi_ldm_device.h"

struct spi_device *km_spidev_node_get(char channel);

static int dev_7038_read_bytes(struct spi_ldm_device *dev, int dev_id, int reg,
                             uint8_t *buf, size_t buf_size) {
    mutex_lock(&dev->mutex);
    uint8_t w_buf[4 + dev->bus->device_total];
    int w_size = 0;
    w_buf[w_size++] = dev_id;
    w_buf[w_size++] = buf_size;
    w_buf[w_size++] = 0x80 + (reg >> 8);
    w_buf[w_size++] = reg & 0xff;
    int i = 0;
    for (i = 0; i < dev->bus->device_total; ++i) {
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

    x[1].rx_buf    = buf;
    x[1].len        = buf_size;
    x[1].tx_nbits    = SPI_NBITS_SINGLE;
    x[1].rx_nbits    = SPI_NBITS_SINGLE;
    x[1].speed_hz    = 0;
    spi_message_add_tail(&x[1], &message);

    spi_node = km_spidev_node_get(dev->bus->bus_num);
    status = spi_sync(spi_node, &message);
    mutex_unlock(&dev->mutex);

    return status;
}

static int dev_7038_write_bytes(struct spi_ldm_device *dev, int dev_id, int reg,
                              uint8_t *buf, size_t buf_size) {
    mutex_lock(&dev->mutex);
    uint8_t w_buf[4 + dev->bus->device_total + buf_size];
    int w_size = 0;
    size_t reg_size = buf_size;
    if (dev_id == 0x3f) reg_size = buf_size / dev->bus->device_total;
    w_buf[w_size++] = (dev_id > 0 && dev_id < 0x3F)
            ? dev_id
            : (dev_id == 0) ? 0x80 : 0xBF;
    w_buf[w_size++] = reg_size;
    w_buf[w_size++] = reg >> 8;
    w_buf[w_size++] = reg & 0xff;
    size_t i = 0;
    for (i = 0; i < buf_size; ++i) {
        w_buf[w_size++] = buf[i];
    }

    size_t extra_bytes = (dev_id > 0 && dev_id < 0x3F)
            ? dev_id - 1
            : dev->bus->device_total - 1;
    for (i = 0; i < extra_bytes; ++i) {
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

static int dev_7038_read_reg(struct spi_ldm_device *dev, int dev_id, int reg,
                             uint32_t *buf, size_t buf_size) {
    uint8_t r_buf[buf_size];
    int ret = dev_7038_read_bytes(dev, dev_id, reg, r_buf, buf_size);
    size_t i = 0;
    for (i = 0; i < buf_size; i++) {
        buf[i] = r_buf[i];
    }
    return ret;
}

static int dev_7038_write_reg(struct spi_ldm_device *dev, int dev_id, int reg,
                              uint32_t *buf, size_t buf_size) {
    uint8_t w_buf[buf_size];
    size_t i = 0;
    for (i = 0; i < buf_size; i++) {
        w_buf[i] = buf[i];
    }
    return dev_7038_write_bytes(dev, dev_id, reg, w_buf, buf_size);
}

static int dev_7038_write_pwm(struct spi_ldm_device *dev, uint16_t *pwm_buf,
                              size_t buf_size) {
    // read PWM_PERIOD from register
    uint16_t PWM_PER[dev->bus->device_total];
    int devid = 0;
    for (devid = 0; devid < dev->bus->device_total; devid++) {
        uint8_t buffer[2];
        dev_7038_read_bytes(dev, devid + 1, 0x150, buffer, 2);
        PWM_PER[devid] = buffer[0] & 0x1F;
        PWM_PER[devid] <<= 8;
        PWM_PER[devid] += buffer[1];
        if (PWM_PER[devid] == 0) {
            printk("PWM_PERIOD is 0, use default 2048\n");
            PWM_PER[devid] = 2048;
        }
    }
    int total_channels = dev->bus->device_total * dev->channels_per_device;
    if (buf_size != total_channels) return -1;
    int reg = 0x40;
    uint8_t w_buf[total_channels * 2];
    int i = 0;
    for (i = 0; i < total_channels; ++i) {
        int logic = dev->mapping[i];
        int pwm_val = pwm_buf[dev->mapping[i]];
        int dev_id = i / dev->channels_per_device;
        pwm_val *= PWM_PER[dev_id];
        pwm_val >>= 16;
        w_buf[i * 2] = pwm_val >> 8;
        w_buf[i * 2 + 1] = pwm_val & 0xff;
        //pwm_buf[dev->mapping[i]] = pwm_val;
    }
    dev_7038_write_bytes(dev, 0x3f, reg, w_buf, total_channels * 2);
    return 0;
}

static int dev_7038_update_device(struct spi_ldm_device *dev, uint16_t *pwm_buf,
                              uint16_t *current_buf, size_t buf_size, struct sysstateregs* sysstate) {
    dev_7038_write_pwm(dev, pwm_buf, buf_size);

    int total_size = dev->bus->device_total * 16;
    if (buf_size != total_size) return -1;

    total_size += dev->bus->device_total * 2;
    uint8_t out_buf[total_size];
    int dev_id = 0;
    int ch = 0;
    int phy_ch = 0;
    int buf_start = 0;
    for (dev_id = 0; dev_id < dev->bus->device_total; dev_id++) {
        out_buf[buf_start] = out_buf[buf_start + 1] = 0;
        for (ch = 0; ch < 16; ch++, phy_ch++) {
            int current_val = current_buf[dev->mapping[phy_ch]];
            current_val = current_val * dev->current_max + (1 << 22);
            current_val >>= 23;  // 9 bits value
            //bit[7:0]
            out_buf[buf_start + 2 + ch] = current_val & 0xFF;
            //bit[8]
            if (current_val & 0x100) {
                if (ch < 8) {
                    out_buf[buf_start] |=  (0x80 >> ch);
                } else {
                    out_buf[buf_start + 1] |=  (0x80 >> (ch - 8));
                }
            }
        }
        buf_start += 18;
    }
    
    int reg = 0x66;
    dev_7038_write_bytes(dev, 0x3f, reg, out_buf, total_size);
    return 0;
}

static int dev_7038_init(struct spi_ldm_device *dev) {
    int ret;
    printk("[7038] init local dimming board spi devices...\n");
    uint8_t buf_reg_a1[] = {0xa3};
    uint8_t buf_reg_00[] = {0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x07, 0x0F, 0x17, 0x1F,
                            0x26, 0x2E, 0x36, 0x3E, 0x45, 0x4D, 0x55,
                            0x5D, 0x65, 0x6C, 0x74, 0xFF, 0xFF};
    uint8_t buf_reg_21[] = {0x02, 0x13, 0x3E, 0x61, 0x6F, 0x8A,
                            0xF7, 0xCF, 0xFF, 0xFF, 0x80, 0x00,
                            0x00, 0x5B, 0x2A, 0x61, 0x00, 0x23};
    uint8_t buf_reg_34[] = {0x01};
    uint8_t buf_reg_60[] = {0x00, 0x00};
    uint8_t buf_reg_82[] = {0x10};
    uint8_t buf_reg_89[] = {0x00, 0x18, 0x11, 0x0A, 0x02, 0x1B, 0x14, 0x0C,
                            0x05, 0x1E, 0x16, 0x0F, 0x08, 0x00, 0x19, 0x12};
    uint8_t buf_reg_40[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0xff, 0xff};
    uint8_t buf_reg_0[] = {0x03};

    ret = dev_7038_write_bytes(dev, 0, 0xa1, buf_reg_a1, ARRAY_SIZE(buf_reg_a1));
    ret = dev_7038_write_bytes(dev, 0, 0x00, buf_reg_00, ARRAY_SIZE(buf_reg_00));
    ret = dev_7038_write_bytes(dev, 0, 0x21, buf_reg_21, ARRAY_SIZE(buf_reg_21));
    ret = dev_7038_write_bytes(dev, 0, 0x34, buf_reg_34, ARRAY_SIZE(buf_reg_34));
    ret = dev_7038_write_bytes(dev, 0, 0x60, buf_reg_60, ARRAY_SIZE(buf_reg_60));
    ret = dev_7038_write_bytes(dev, 0, 0x82, buf_reg_82, ARRAY_SIZE(buf_reg_82));
    ret = dev_7038_write_bytes(dev, 0, 0x89, buf_reg_89, ARRAY_SIZE(buf_reg_89));
    ret = dev_7038_write_bytes(dev, 0, 0x40, buf_reg_40, ARRAY_SIZE(buf_reg_40));

    ret = dev_7038_write_bytes(dev, 0, 0x0, buf_reg_0, ARRAY_SIZE(buf_reg_0));
    return ret;
}

static int dev_7038_exit(struct spi_ldm_device *dev) {
    return 0;
}


static int dev_7038_init_mapping(struct spi_ldm_device *dev, int *mapping,
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

int dev_7038_write_bfi_control(struct spi_ldm_device* dev, bool bfi_on, uint16_t blank_time, uint32_t* buf, size_t buf_size) {
    int devid = 0;
    uint8_t reg01_18[24];
    uint8_t reg89_98[16];
    int ch = 0;
    uint8_t reg_27, reg_34;
    uint8_t pwm_freq;
    uint8_t scan_num;

    dev_7038_read_bytes(dev, 1, 0x27, &reg_27, 1);
    pwm_freq = (reg_27 & 0xF) + 1;

    int pwm_one_period_delay = (1 << 24) / pwm_freq;
    for (devid = 0; devid < dev->bus->device_total; devid ++) {
        int i = 0;
        int regScanNum = 0;
        int regDelayHigh = 0x8;
        int regDelayLow = 0x0;

        // read PWM_PER for each device
        uint8_t buffer[2];
        dev_7038_read_bytes(dev, devid + 1, 0x150, buffer, 2);
        int pwm_per = buffer[0] & 0x1F;
        pwm_per <<= 8;
        pwm_per += buffer[1];

        for (i = 0; i < dev->channels_per_device; i += 2) {
            int delay0 = buf[dev->mapping[ch]];
            int delay1 = buf[dev->mapping[ch + 1]];
            int scannum0 = delay0 / pwm_one_period_delay;
            int scannum1 = delay1 / pwm_one_period_delay;
            delay0  = (delay0 % pwm_one_period_delay) * pwm_per / pwm_one_period_delay;
            delay1 = (delay1 % pwm_one_period_delay) * pwm_per / pwm_one_period_delay;
            if (scannum0 >= pwm_freq) {
                scannum0 = pwm_freq - 1;
            }
            if (scannum1 >= pwm_freq) {
                scannum1 = pwm_freq - 1;
            }
            reg01_18[regScanNum ++] = (scannum0 << 4) + scannum1;
            reg01_18[regDelayHigh ++] = delay0 >> 5;
            reg01_18[regDelayHigh ++] = delay1 >> 5;
            reg89_98[regDelayLow ++] = delay0 & 0x1F;
            reg89_98[regDelayLow ++] = delay1 & 0x1F;
            ch += 2;
        }
        dev_7038_write_bytes(dev, devid + 1, 0x01, reg01_18, 24);
        dev_7038_write_bytes(dev, devid + 1, 0x89, reg89_98, 16);
    }

    if (bfi_on) {
        scan_num = (blank_time * pwm_freq) >> 16;
        scan_num = pwm_freq - scan_num - 1;
        reg_27 = (scan_num << 4) + (pwm_freq - 1);
        reg_34 = 0x2;
        dev_7038_write_bytes(dev, 0, 0x27, &reg_27, 1);
        dev_7038_write_bytes(dev, 0, 0x34, &reg_34, 1);
    } else {
        reg_27 = 0xF0 | reg_27;
        reg_34 = 0x1;
        dev_7038_write_bytes(dev, 0, 0x34, &reg_34, 1);
        dev_7038_write_bytes(dev, 0, 0x27, &reg_27, 1);
    }
    return 0;
}

static int dev_7038_handle_fault(struct spi_ldm_device *dev, struct fault_reason* reason, bool clear_only) {
    return -EFAULT;
}

struct spi_ldm_device dev_7038 = {
        .name = "7038",
        .channels_per_device = 16,
        .pwm_bits = 13,
        .current_bits = 13,
        .bus = NULL,
        .init = dev_7038_init,
        .exit = dev_7038_exit,
        .write_reg = dev_7038_write_reg,
        .read_reg = dev_7038_read_reg,
        .init_mapping = dev_7038_init_mapping,
        .write_pwm = dev_7038_write_pwm,
        .update_device = dev_7038_update_device,
        .write_bfi_control = dev_7038_write_bfi_control,
        .handle_fault = dev_7038_handle_fault,
        .mapping = NULL,
        .gpio_output_pin = -1, //GPIO from SOC output to driver IC
        .gpio_input_pin = -1,  //GPIO feeback from driver IC to SOC
};
