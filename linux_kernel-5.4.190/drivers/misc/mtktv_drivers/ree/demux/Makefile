#
# Makefile for Mediatek device drivers.
#

.PHONY: all build clean release

all: build release
	@echo "KBUILD_CFLAGS = $(KBUILD_CFLAGS)"

clean:
	make -C $(KDIR) M=$(CURDIR) clean

build:
	make -C $(KDIR) M=$(CURDIR) modules

release: build
	@mkdir -p $(MDPDIR)/out
	@find . -iname *.ko -exec ${CROSS_COMPILE}strip --strip-unneeded {} \;
	@find . -iname *.ko -exec cp {} $(MDPDIR)/out \;
