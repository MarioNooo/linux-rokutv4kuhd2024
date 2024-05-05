#ifndef PQU_PORT_OS_H
#define PQU_PORT_OS_H

#define NUM_BASE_10 (10)

#include <linux/types.h>
#include <linux/string.h>
#include <linux/printk.h>
#include <linux/time.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kernel.h>
#define cli_dbg_print(format, args...) {printk(format, args);}
#define str2u8(str, pValue) {if(str&&pValue){kstrtou8(str, NUM_BASE_10, pValue);}}
#define str2u16(str, pValue) {if(str&&pValue){kstrtou16(str, NUM_BASE_10, pValue);}}
#define str2u32(str, pValue) {if(str&&pValue){kstrtou32(str, NUM_BASE_10, pValue);}}
#define pqu_register_alg_mapi_cat(x)
#define pqu_register_alg_mapi_echo(x)
#include <linux/dma-mapping.h>
#include <linux/ctype.h>
#include <linux/version.h>
#endif

