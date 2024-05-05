#ifndef spi_ldm_device_0744828c9e2ec9f7
#define spi_ldm_device_0744828c9e2ec9f7

#include <roku_common_defs.h>
#include "roku_ldm_boost.h"
#include "roku_ldm_fault.h"

struct spi_ldm_device {
    const char *name;
    const int channels_per_device;
    const int pwm_bits;
    const int current_bits;
    struct mutex mutex;
    __spi_bus *bus;
    int (*init)(struct spi_ldm_device *dev);
    int (*exit)(struct spi_ldm_device *dev);
    int (*write_reg)(struct spi_ldm_device *dev, int dev_id, int reg, uint32_t *buf,
                     size_t buf_size);
    int (*read_reg)(struct spi_ldm_device *dev, int dev_id, int reg, uint32_t *buf,
                    size_t buf_size);
    int (*init_mapping)(struct spi_ldm_device *dev, int *mapping, size_t size);
    int (*write_pwm)(struct spi_ldm_device *dev, uint16_t *pwm_buf, size_t size);
    int (*update_device)(struct spi_ldm_device *dev, uint16_t *pwm_buf, uint16_t *current_buf,size_t size, struct sysstateregs* sysstate);
    int (*write_bfi_control) (struct spi_ldm_device * dev, bool bfi_on, uint16_t blank_time, uint32_t * buf, size_t size);
    int (*handle_fault)(struct spi_ldm_device *dev, struct fault_reason* reason, bool clear_only);
    int enabled_channels_per_device;
    uint16_t *mapping;
    uint16_t current_max;
    int gpio_output_pin; //GPIO from SOC output to driver IC
    int gpio_input_pin;  //GPIO feeback from driver IC to SOC
};

extern struct spi_ldm_device dev_7038;
extern struct spi_ldm_device dev_7039;

#endif // spi_ldm_device_0744828c9e2ec9f7
