ccflags-y += -I$(srctree)/drivers/tee/optee
ifneq ($(LINUX_ONLY),true)
ccflags-y += -DSET_USER_ID=2944
endif
obj-y += mtk_tee.o
