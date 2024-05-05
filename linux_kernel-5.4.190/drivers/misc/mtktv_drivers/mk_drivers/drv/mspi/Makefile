#
# Makefile for MStar GFLIP device drivers.


# general options
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/mspi/
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/hal/common/mspi

# files
obj-$(CONFIG_MSTAR_MSPI)   += spi-mstar.o
obj-$(CONFIG_MSTAR_QSPI)   += qpi-mstar.o
obj-y	+=  mdrv_mspi.o
obj-y   += ../../hal/common/mspi/mhal_mspi.o
