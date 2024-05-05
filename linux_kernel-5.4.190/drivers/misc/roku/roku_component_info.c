#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
 
static struct proc_dir_entry *comp_info_entry;
 
static uint16_t register_read(uint32_t reg)
{
    return( *(volatile uint16_t *)(0xfd000000 + ( reg << 1)) );
}

static char * get_ddr_type(void) {
    char *ddr_name = "Unknown";
#if defined(CONFIG_MSTAR_MT5889)
//bank 0x101e offset 0xc0
#define REG_DDR_TYPE 0x101EC0
    uint16_t ddr_id = register_read(REG_DDR_TYPE);
    ddr_id = (ddr_id & 0xff00) >> 8;
    switch(ddr_id) {
        case 0x68:
            ddr_name = "Samsung_DDR4_2666_4x_1024MB";
            break;
        case 0x6c:
            ddr_name = "Samsung_DDR3_2133_4x_1024MB";
            break;
        case 0x6e:
            ddr_name = "Nanya_DDR3_2133_4x_1024MB";
            break;
    };
#endif
    return ddr_name;
}

static int comp_info_proc_show(struct seq_file *m, void *v)
{
    // Don't change these logs as Application reads the printfs.
    seq_printf(m, "%s\n", get_ddr_type());
    return 0;
}

static int comp_info_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, comp_info_proc_show, NULL);
}


static struct file_operations comp_info_ops = {
    .open   = comp_info_proc_open,
    .read   = seq_read,
    .llseek = seq_lseek,
    .release = single_release
};

 
static int __init comp_info_init(void)
{
    int proc_permissions;
    proc_permissions = 0444;
    comp_info_entry=proc_create("component_info", proc_permissions, NULL,&comp_info_ops);
    if (NULL == comp_info_entry){
        pr_warn("comp_info: component_info create proc file failed!\n");
        return -ENOMEM;
    }
    return 0;
}
 
static void __exit comp_info_exit(void)
{
    if(comp_info_entry){
        proc_remove(comp_info_entry);
    }
}

module_init(comp_info_init);
module_exit(comp_info_exit);

MODULE_AUTHOR("Roku");
MODULE_DESCRIPTION("Component Information");
MODULE_LICENSE("GPL");
