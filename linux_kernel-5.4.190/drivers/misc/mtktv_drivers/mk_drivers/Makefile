#
# Makefile for MStar device drivers.
#

CHIP_NAME:=$(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

obj-$(CONFIG_MSTAR_MPOOL)							+= drv/mpool/
obj-$(CONFIG_MSTAR_SYSTEM)							+= drv/system/
#ifndef CONFIG_ROKU_MSTAR_DRIVER_MODIFY
obj-$(CONFIG_MSTAR_GFLIP)							+= drv/gflip/
obj-$(CONFIG_MSTAR_GRAPHIC)							+= drv/graphic/
#endif
obj-$(CONFIG_MSTAR_SC)								+= drv/sc/
obj-$(CONFIG_MSTAR_SEMUTEX)							+= drv/semutex/
obj-$(CONFIG_MSTAR_CMAPOOL)							+= drv/cma_pool/
obj-$(CONFIG_MSTAR_PM)								+= drv/pm/
obj-$(CONFIG_MSTAR_R2)								+= drv/r2/

#ifdef CONFIG_ROKU_MSTAR_DRIVER_MODIFY
obj-$(CONFIG_MSTAR_ALSA_AUDIO_DRIVER_DRV)			+= drv/alsa_audio_driver/
obj-$(CONFIG_MSTAR_ALSA_AUDIO_DRIVER_HAL)			+= hal/$(CHIP_NAME)/alsa_audio_driver/
#else
obj-$(CONFIG_MSTAR_ALSA_AUDIO_DRIVER_DRV)			+= drv/alsa_audio_driver
obj-$(CONFIG_MSTAR_ALSA_AUDIO_DRIVER_HAL)			+= hal/$(CHIP_NAME)/alsa_audio_driver
#endif

# for midgard platform
obj-$(CONFIG_MSTAR_IOMAP)	+= drv/iomap/
obj-$(CONFIG_MP_ARMCE_TEST)	+= drv/armce_test/
