#
# Makefile for the linux kernel.
#

EXTRA_CFLAGS      +=  -I$(srctree)/drivers/irqchip
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS += -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/cpu/$(ARCH)
EXTRA_CFLAGS += -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/cpu/$(ARCH)/include

obj-y += chip_arch.o
