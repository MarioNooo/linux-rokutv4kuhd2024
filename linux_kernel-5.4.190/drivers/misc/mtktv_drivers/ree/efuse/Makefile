#MODULE = efuse/efuse_kernel
MODULE = efuse_hashkey/efuse_kernel
export S_E_C_PATCH_1=true

TOP_DIR    := $(shell pwd)
CLOSE_SRC_DIR := $(MDPDIR)/../close_src
MODULE_SRC_DIR = $(CLOSE_SRC_DIR)/$(MODULE)

mtk-efuse-y    += mtk_efuse.o

obj-m            := mtk-efuse.o

KBUILD_EXTRA_SYMBOLS += $(TOP_DIR)/Module.symvers

STRIP := $(CROSS_COMPILE)strip --strip-unneeded

all: _build _release
	
_build:
	@if [ -d $(MODULE_SRC_DIR) ]; then \
		echo "PREBUILT_OBJ_RELEASE_MODE = $(PREBUILT_OBJ_RELEASE_MODE)"; \
		echo "S_E_C_PATCH_1 = $(S_E_C_PATCH_1)"; \
		echo "$(MAKE) -C "$(KDIR)" KERNEL_PATH=$(KDIR) M=$(MODULE_SRC_DIR)"; \
		$(MAKE) -C "$(KDIR)" KERNEL_PATH=$(KDIR) M=$(MODULE_SRC_DIR); \
		echo "$(STRIP)  $(MODULE_SRC_DIR)/mtk_efuse.o" ; \
		$(STRIP)  $(MODULE_SRC_DIR)/mtk_efuse.o; \
		cp $(MODULE_SRC_DIR)/mtk_efuse.o prebuilt_obj/$(PREBUILT_OBJ_RELEASE_MODE)/mtk_efuse.o_shipped; \
	fi
	cp prebuilt_obj/$(PREBUILT_OBJ_RELEASE_MODE)/mtk_efuse.o_shipped mtk_efuse.o
	$(MAKE) -C "$(KDIR)" M=$(TOP_DIR) modules

_release:
	@echo "MDPDIR = $(MDPDIR)"
	@$(STRIP) *.ko
	@mkdir -p $(MDPDIR)/out/
	@echo "cp mtk-efuse.ko $(MDPDIR)/out/"
	@cp mtk-efuse.ko $(MDPDIR)/out/

clean:
	rm -rf $(MDPDIR)/out/mtk-efuse.ko
	@if [ -d $(MODULE_SRC_DIR) ]; then \
		$(MAKE) -C "$(KDIR)" KERNEL_PATH=$(KDIR) M=$(MODULE_SRC_DIR) clean; \
	else \
		echo "Cannot find efuse source directory"; \
	fi
	@if [ -d $(KDIR) ]; then \
		$(MAKE) -C "$(KDIR)" M=$(TOP_DIR) clean; \
		find ../ -name *.o | xargs -i rm -rf {}; \
		find ../ -name *.o.cmd | xargs -i rm -rf {}; \
	else \
		echo "Cannot find kernel directory"; \
	fi
