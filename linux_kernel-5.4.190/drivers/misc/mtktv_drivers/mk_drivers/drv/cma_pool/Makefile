#
# Makefile for MStar CMA Pool device drivers.
#



# general options
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include/sys/common
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/cma_pool
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/miu
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/mma
# specific options
EXTRA_CFLAGS      +=  -I$(srctree)/drivers/staging/android/ion
EXTRA_CFLAGS      +=  -I$(srctree)/mm

EXTRA_CFLAGS      += -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/miu

ifeq ($(KERNEL_MIXED_BUILD_SUPPORT),true)
EXTRA_CFLAGS      += -I$(KERNEL_MIXED_BUILD_PATH)/drivers/soc/mediatek/mtk-memory/
else
EXTRA_CFLAGS      += -I$(srctree)/drivers/soc/mediatek/mtk-memory/
endif

# files
ifeq ($(CONFIG_CMA),y)
obj-$(CONFIG_MSTAR_CMAPOOL)     += mdrv-cma_pool.o
mdrv-cma_pool-objs                       += mdrv_cma_pool.o
endif
