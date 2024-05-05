# SPDX-License-Identifier: GPL-2.0

KBUILD_EXTRA_SYMBOLS += ${MDRV_MTKDRV_HWREG_SRC_DIR}/Module.symvers
KBUILD_EXTRA_SYMBOLS += ${MDRV_MTKDRV_XCALG_SRC_DIR}/Module.symvers

ifeq (y, $(filter y, $(CONFIG_DRM_MEDIATEK_V2)))
obj-$(CONFIG_DRM_MEDIATEK) += mediatek_v2/
else


mediatek-drm-y := mtk_disp_color.o \
		  mtk_disp_ovl.o \
		  mtk_disp_rdma.o \
		  mtk_drm_crtc.o \
		  mtk_drm_ddp.o \
		  mtk_drm_ddp_comp.o \
		  mtk_drm_drv.o \
		  mtk_drm_fb.o \
		  mtk_drm_gem.o \
		  mtk_drm_plane.o \
		  mtk_dsi.o \
		  mtk_mipi_tx.o \
		  mtk_dpi.o
ccflags-y := -DMSOS_TYPE_LINUX_KERNEL
ccflags-y += -DSTI_PLATFORM_BRING_UP
ccflags-y += -DCONFIG_GOP_AFBC_FEATURE
ccflags-y += -I$(MDRV_MTKDRV_SCRIPTMGT_SRC_DIR)/include
ccflags-y += -I$(MDRV_MTKDRV_HWREG_SRC_DIR)/include
ccflags-y += -I$(HWREG_HEADER_PATH)
ccflags-y += -I$(MDRV_MTKDRV_XCALG_SRC_DIR)/include
ccflags-y += -I$(XCALG_HEADER_PATH)
ccflags-y += -I$(srctree)/drivers/soc/mediatek/mtk-memory
ccflags-y += -I$(KERNEL_PATH)/drivers/soc/mediatek/realtime

obj-$(CONFIG_DRM_MEDIATEK) += mediatek-drm.o

#mediatek-drm-tv-objs += mtk_tv_drm_fb.o
#mediatek-drm-tv-objs += mtk_tv_drm_plane.o
#mediatek-drm-tv-objs += mtk_tv_drm_gem.o

#ifeq ($(CONFIG_DRM_MEDIATEK_TV), m)
#mediatek-drm-tv-objs += ../drm_gem_cma_helper.o
#endif


mediatek-drm-hdmi-objs := mtk_cec.o \
			  mtk_hdmi.o \
			  mtk_hdmi_ddc.o \
                          mtk_mt2701_hdmi_phy.o \
			  mtk_mt8173_hdmi_phy.o \
			  mtk_hdmi_phy.o

obj-$(CONFIG_DRM_MEDIATEK_HDMI) += mediatek-drm-hdmi.o
endif
