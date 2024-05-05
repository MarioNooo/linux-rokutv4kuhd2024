#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <MMAP_MI.h>
#include <roku_common_defs.h>
#include <mdrv_types.h>

extern void backlight_on_task(bool resume);

struct pmgammaic_work_s {
    bool is_resume;
    struct delayed_work pmgammaic_program_work;
} pmgammaic_work;

struct delayed_work pmgammaic_program_work;


#define _LShift(v, n)       ((v) << (n))
#define _RShift(v, n)       ((v) >> (n))
#define HIGH_BYTE(val)      (U8)_RShift((val), 8)
#define LOW_BYTE(val)       ((U8)((val) & 0xFF))

static BOOL MDrv_HW_IIC_WriteBytes(U8 u8Port, U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData)
{
    int num_xfer;
    struct i2c_adapter *adap;
    struct i2c_msg msg = {
        .addr = (LOW_BYTE(u16SlaveCfg) >> 1),
        .flags = 0,
        .len = (uAddrCnt + uSize),
    };
    U8 *data;

    data = kzalloc(msg.len, GFP_KERNEL);
    if (!data)
        return FALSE;
    memcpy(data, pRegAddr, uAddrCnt);
    memcpy(data + uAddrCnt, pData, uSize);
    msg.buf = data;

    adap = i2c_get_adapter(u8Port);
    if (!adap) {
        kfree(data);
        return FALSE;
    }

    num_xfer = i2c_transfer(adap, &msg, 1);

    i2c_put_adapter(adap);

    kfree(data);

    return (num_xfer == 1) ? TRUE : FALSE;
}

static void write_ic_config(__pmgammaic_info *info) {
    int i;
    U8 port = 3;
    U8 i2c_address = ioread8(&info->i2c_addr);
    U8 rw_size = ioread32(&info->rw_size);
    U8 end_address = ioread32(&info->end_register);
    U8 reg_index;
    U8 buffer[PMGAMMAIC_MAX_BINARY_LEN] = {0};
    S32 status;
    if (strcmp(info->chip_name, "CS602-A0G") == 0 || strcmp(info->chip_name, "CS603-A0R") == 0 || strcmp(info->chip_name, "CS602-A1L") == 0) {
        // Write to mode register to be able to write to other registers. CS603, CS602
        reg_index = 0x64;
        buffer[0] = 0x80;
        status = MDrv_HW_IIC_WriteBytes(port, i2c_address, rw_size, &reg_index, 1, buffer); // 1-Byte size Control register
    }

    for (i = ioread32(&info->start_register); i <= ioread32(&info->end_register) && i < PMGAMMAIC_MAX_BINARY_LEN; i++) {
        buffer[i] = ioread8(&info->binary_data[i]);
    }
    reg_index = ioread32(&info->start_register) + ioread32(&info->reg_offset);

    if (reg_index <= end_address && end_address < PMGAMMAIC_MAX_BINARY_LEN) {
        // Send single I2C command with start register and number of bytes to write.
        status = MDrv_HW_IIC_WriteBytes(port, i2c_address, rw_size, &reg_index, end_address - reg_index + 1, buffer + reg_index);
        if (status < 0) {
            printk("MDrv_HW_IIC_WriteBytes Error!!! = 0x%x\n", status);
        }

        if (info->common_delay) {
            msleep(info->common_delay);
        }
    } else {
        printk("ERROR! %s config illegal reg range [0x%x,0x%x]\n", 
                &info->chip_name, reg_index, end_address);
    }
}

static void write_pmgammaic_registers(struct work_struct * work) {
    int ret;

    void * panel_cfg_va = NULL;
     __roku_panel_cfg * cfg = NULL;
     __pmgammaic_cfg *pmgammaic_cfg = NULL;

    struct pmgammaic_work_s * data =
            container_of(work, struct pmgammaic_work_s, pmgammaic_program_work.work);

    void * panel_cfg_pa = (void *)(ROKU_SHARE_MEM_GET_PANEL_CONFIG_DATA);
    panel_cfg_va = ioremap((unsigned long)panel_cfg_pa, ROKU_PANEL_CONFIG_DATA_SIZE);
    cfg = (__roku_panel_cfg *)panel_cfg_va;
    pmgammaic_cfg = &cfg->pmgammaic_cfg;
    // Apply IC settings if settings weren't applied in u-boot or TV is resuming from suspend
    bool apply_pm_settings = pmgammaic_cfg->pm.present && !pmgammaic_cfg->pm.nvram_present &&
                                (data->is_resume || !pmgammaic_cfg->pm.apply_in_uboot);
    bool apply_ls_settings = pmgammaic_cfg->ls.present && !pmgammaic_cfg->ls.nvram_present &&
                                (data->is_resume || !pmgammaic_cfg->ls.apply_in_uboot);

    printk("panel_type = %d, PM NVRAM present = %d, LS NVRAM present = %d, Apply PM settings = %d, Apply LS settings = %d\n",
        cfg->panel_type, pmgammaic_cfg->pm.nvram_present, pmgammaic_cfg->ls.nvram_present,
        apply_pm_settings, apply_ls_settings);

    if (cfg->panel_type == PANEL_TCONLESS) {
        // If PMGamma/LS IC has NVRAM then handle in Application else here.
        // Write PMIC registers
        if (apply_pm_settings) {
            write_ic_config(&pmgammaic_cfg->pm);
        }

        // Write LevelShifter registers
        if (apply_ls_settings) {
            write_ic_config(&pmgammaic_cfg->ls);

            // Set enable signal after write LevelShifter registers
            if (pmgammaic_cfg->ls.enable_signal_gpio) {
                ret = gpio_request(pmgammaic_cfg->ls.enable_signal_gpio, "pmgammaic_enable_signal");
                if ((ret != -EBUSY) && (ret<0)) {
                    pr_err("[%s][%d] gpio_request fail, pin=%d , ret=%d", __func__ , __LINE__ , pmgammaic_cfg->ls.enable_signal_gpio , ret);
                } else {
                    gpio_set_value(pmgammaic_cfg->ls.enable_signal_gpio, 1);
                    gpio_free(pmgammaic_cfg->ls.enable_signal_gpio);
                }
            }
        }
    }
    iounmap(panel_cfg_va);

    if (!data->is_resume) {
        backlight_on_task(data->is_resume);
    }
}


void pmgammaic_init (bool resume) {
    pmgammaic_work.is_resume = resume;
    INIT_DELAYED_WORK(&pmgammaic_work.pmgammaic_program_work, write_pmgammaic_registers);
    schedule_delayed_work(&pmgammaic_work.pmgammaic_program_work, msecs_to_jiffies(10));
}

EXPORT_SYMBOL(pmgammaic_init);
