#ifndef PQU_PORT_OS_H
#define PQU_PORT_OS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>
#include <errno.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "msos_types.h"
#include "dtv_mrv55.h"

#define METADATA_FREERTOS 1

//#include <time.h>
#ifndef __timespec_defined
#define __timespec_defined
struct timespec {
	long	tv_sec;			// seconds
	long	tv_nsec;		// nanoseconds
};
#endif

struct device{
	int dev;
};

struct platform_device{
	int dev;
};

#define ioremap_cache(a,b)  mtk_io_pa2da(a)
#define dma_direct_sync_single_for_device(a,b,c,d)
#define dma_direct_sync_single_for_cpu(a,b,c,d)

// https://elixir.bootlin.com/linux/latest/source/include/linux/interrupt.h
#define IRQF_SHARED		0x00000080
#define IRQF_ONESHOT		0x00002000

enum irqreturn {
    IRQ_NONE        = (0 << 0),
    IRQ_HANDLED     = (1 << 0),
    IRQ_WAKE_THREAD = (1 << 1),
};
typedef enum irqreturn irqreturn_t;

//TODO: Below may replace by MS_U8, MS_U16, ... and etc
typedef uint64_t phys_addr_t;
typedef uint8_t  __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef uint64_t u64;
typedef uint32_t u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef signed long long  s64;
typedef signed int  s32;
typedef signed short s16;
typedef signed char  s8;
typedef int pid_t;


// =========================================================
// Porting with dummy function

#define __iomem
#define GFP_ATOMIC   0
#define GFP_KERNEL   0
#define KERN_ERR
#define PROT_NORMAL  0
#define __pgprot(x)  x
#define KERN_CONT
#define KERN_CRIT
#define KERN_DEBUG
#define KERN_INFO

#ifndef BIT
#define BIT(_bit_)                  (1 << (_bit_))
#endif


//TODO: All pr_xxx, printk should be replaced with log_output() function
// =========================================================
// Porting with real function
#ifdef SIMULATOR
#define pr_err(format, ...)   fprintf(stderr, format, ## __VA_ARGS__)
#define pr_debug(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#define pr_devel(fmt, args...) fprintf(stderr, format, ## __VA_ARGS__)
#define pr_info(format, ...)  fprintf(stderr, format, ## __VA_ARGS__)
#define printk(format, ...)   fprintf(stderr, format, ## __VA_ARGS__)
#else
#include "log.h"
#define pr_err(fmt, args...)    log_output("SYS", LOG_LEVEL_ERROR,  fmt, ## args)
#define pr_debug(fmt, args...)  log_output("SYS", LOG_LEVEL_TRACE,  fmt, ## args)
#define pr_devel(fmt, args...)  log_output("SYS", LOG_LEVEL_ERROR,  fmt, ## args)
#define pr_info(fmt, args...)   log_output("SYS", LOG_LEVEL_INFO,  fmt, ## args)
#define printk(fmt, args...)    log_output("SYS", LOG_LEVEL_INFO,  fmt, ## args)
#endif

#define str2u8(str, pValue) {if(str&&pValue){*pValue = atoi(str);}}
#define str2u16(str, pValue) {if(str&&pValue){*pValue = atoi(str);}}
#define str2u32(str, pValue) {if(str&&pValue){*pValue = atoi(str);}}

#define cli_dbg_print(format, args...) {pr_err(format, args);}

#define msleep(x)           vTaskDelay( x/portTICK_RATE_MS )
#define kmalloc(x, type)    pvPortMalloc(x)
#define vmalloc(x)          pvPortMalloc(x)
#define malloc(x)           pvPortMalloc(x)
#define vfree(x)            vPortFree(x)
#define kfree(x)            vPortFree(x)
#define free(x)             vPortFree(x)

#define simple_strtol       strtol
#define simple_strtoul      strtoul
#define strlcpy             strncpy


//TODO: Below inline function may moved to C file.
static inline void *kmalloc_array(size_t n, size_t size, int flags)
{
      if (size != 0 && n > SIZE_MAX / size)
             return NULL;
      return pvPortMalloc(n * size);
}

static inline void getrawmonotonic(struct timespec *x)
{
    TickType_t tick = xTaskGetTickCount();
    long ms = tick/portTICK_PERIOD_MS;

    x->tv_sec  = ms / 1000;
    x->tv_nsec = (ms % 1000) * 1000000;
}

static inline void usleep_range(unsigned long min, unsigned long max)
{
    const portTickType xDelay = pdMS_TO_TICKS(min/1000);
    vTaskDelay( xDelay );
}

static inline unsigned long msleep_interruptible(unsigned int msecs)
{
    const portTickType xDelay = pdMS_TO_TICKS(msecs);
    vTaskDelay( xDelay );
    return xDelay;
}

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
#define panic(x) configASSERT(0)

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2; })

#include "mtk/io.h"
#define readb(x)   mtk_io_read8((x))
#define readw(x)   mtk_io_read16((x))
#define readl(x)   mtk_io_read32((x))
#define writeb(x, y)  mtk_io_write8((y), (x))
#define writew(x, y)  mtk_io_write16((y), (x))
#define writel(x, y)  mtk_io_write32((y), (x))

#define module_init(x)
#define module_exit(x)

#define MODULE_AUTHOR(x)
#define MODULE_DESCRIPTION(x)
#define MODULE_LICENSE(x)
#define EXPORT_SYMBOL(x)

#endif // end of PQU_PORT_WRAPPER_H

