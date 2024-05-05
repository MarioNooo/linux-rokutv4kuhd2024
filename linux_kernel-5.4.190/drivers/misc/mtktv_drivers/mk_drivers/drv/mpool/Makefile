#
# Makefile for MStar Malloc device drivers.
#

# general options
ifeq ($(TEST_SYZKALLER), true)
EXTRA_CFLAGS      +=  -DTEST_SYZKALLER
endif
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/drv/mpool
EXTRA_CFLAGS      +=  -I$(KERNEL_PATH_MSTAR2)/hal/mt5896/mpool
EXTRA_CFLAGS      +=  -I$(srctree)/mm

ccflags-y += -Werror

# files
obj-$(CONFIG_MSTAR_MPOOL)	+= mdrv-mpool.o
mdrv-mpool-objs			+= mdrv_mpool.o \
	../../hal/mt5896/mpool/mhal_mpool.o
