#
# Makefile for MStar Malloc device drivers.
#

CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
ifeq ($(KERNEL_MIXED_BUILD_SUPPORT),true)
EXTRA_CFLAGS      +=  -I$(KERNEL_MIXED_BUILD_PATH)/include
endif
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/alsa_audio_driver

ccflags-y += -DMSOS_TYPE_LINUX_KERNEL

obj-$(CONFIG_MSTAR_ALSA_AUDIO_DRIVER_HAL) += mhal-alsa_driver.o

mhal-alsa_driver-objs += mhal_audio.o