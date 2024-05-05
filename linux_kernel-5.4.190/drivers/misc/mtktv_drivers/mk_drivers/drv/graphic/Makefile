#
# Makefile for MStar GFLIP device drivers.


# general options
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/graphic/
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/graphic/

ccflags-y += -Werror

# files
obj-m	+= mdrv-graphic.o
mdrv-graphic-objs += mdrv_graphic_io.o mdrv_graphic.o \
	../../hal/mt5896/graphic/mhal_graphic.o

