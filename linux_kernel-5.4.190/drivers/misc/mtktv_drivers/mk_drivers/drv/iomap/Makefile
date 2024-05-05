#
# Makefile for MStar Malloc device drivers.
#

# general options
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/common
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/iomap

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_IOMAP)	+= mdrv_iomap.o
