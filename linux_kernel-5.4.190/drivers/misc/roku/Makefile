# Roku drivers

obj-$(CONFIG_ROKU_BACKLIGHT) += roku_backlight.o
obj-$(CONFIG_ROKU_BACKLIGHT) += roku_pmgammaic.o
obj-$(CONFIG_ROKU_EARLY_PWR_MODE_HANDLER) += roku_early_pwr_mode_handler.o
obj-$(CONFIG_ROKU_KPI) += roku_kpi.o
obj-$(CONFIG_ROKU_BOARD_ID) += roku_board_id.o
obj-$(CONFIG_ROKU_COMPONENT_INFO) += roku_component_info.o

obj-$(CONFIG_ROKU_LDMLED) += roku_ldmled.o
obj-$(CONFIG_ROKU_LDMLED) += roku_ldmled_debug.o
obj-$(CONFIG_ROKU_LDMLED) += roku_dev_7038.o
obj-$(CONFIG_ROKU_LDMLED) += roku_dev_7039.o
obj-$(CONFIG_ROKU_LDMLED) += roku_ldm_boost.o
EXTRA_CFLAGS += -I$(KERNEL_PATH_MSTAR2)/include
EXTRA_CFLAGS += -I$(srctree)/include/soc/mediatek

