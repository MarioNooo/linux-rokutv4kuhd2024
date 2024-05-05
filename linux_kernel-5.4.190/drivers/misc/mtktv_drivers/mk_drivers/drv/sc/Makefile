#
# Makefile for MStar SMC device drivers.
#
# general options
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/sc
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/sc
EXTRA_CFLAGS      +=  -I$(srctree)/include/linux

obj-$(CONFIG_MSTAR_SC)             := mdrv-sc.o

mdrv-sc-objs    += mdrv_sc.o mdrv_sc_io.o
mdrv-sc-objs    += ../../hal/mt5896/sc/mhal_sc.o
