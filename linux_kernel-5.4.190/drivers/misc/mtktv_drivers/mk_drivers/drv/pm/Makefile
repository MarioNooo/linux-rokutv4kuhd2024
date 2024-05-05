#
# Makefile for MStar EMAC device drivers.
#

# general options
EXTRA_CFLAGS      +=  -I$(srctree)/kernel/power
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/gpio
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/pm
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/pm
EXTRA_CFLAGS      +=  -I$(srctree)/include/linux

ccflags-y += -Werror

obj-$(CONFIG_MSTAR_PM) += mdrv-pm.o
mdrv-pm-objs += mdrv_pm.o mdrv_pm_io.o ../../hal/mt5896/pm/mhal_pm.o
