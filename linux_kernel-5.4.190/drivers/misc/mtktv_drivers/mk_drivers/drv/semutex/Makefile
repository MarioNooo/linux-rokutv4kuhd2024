#
# Makefile for MStar Semutex device drivers.
#

# general options
EXTRA_CFLAGS                +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS                +=  -I$(KERNEL_PATH_MSTAR2)/drv/semutex
EXTRA_CFLAGS += -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/cpu/$(ARCH)
EXTRA_CFLAGS += -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/cpu/$(ARCH)/include
# specific options

ccflags-y += -Werror
# files
obj-$(CONFIG_MSTAR_SEMUTEX) += mdrv-semutex.o
mdrv-semutex-objs += mdrv_semutex.o mdrv_msos.o