
# general options
EXTRA_CFLAGS      += -I$(KERNEL_PATH_MSTAR2)/drv/dvfs/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/system
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/system
ifdef COFNIG_ARM
EXTRA_CFLAGS += -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/cpu/$(ARCH)
EXTRA_CFLAGS += -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/cpu/$(ARCH)/include
endif
ifdef CONFIG_MSTAR_SPI_FLASH
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/flash
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include/sys/common
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include/sys/drv
endif

ccflags-y += -Werror
# files
obj-y += mdrv-system.o

mdrv-system-y := mdrv_system.o mdrv_system_io.o \
	../../hal/mt5896/system/mhal_system.o \
	../../hal/mt5896/cpu/$(ARCH)/chip_arch.o
mdrv-system-y += ../../hal/mt5896/cpu/$(ARCH)/cache.o
