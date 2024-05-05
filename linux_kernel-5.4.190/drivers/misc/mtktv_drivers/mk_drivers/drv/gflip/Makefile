#
# Makefile for MStar GFLIP device drivers.


# general options
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/gflip/
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/gflip/

ccflags-y += -Werror

# files
obj-m += mdrv-gflip.o
mdrv-gflip-objs += mdrv_gflip_io.o mdrv_gflip.o mdrv_gflip_interrupt.o \
	../../hal/mt5896/gflip/mhal_gflip.o

