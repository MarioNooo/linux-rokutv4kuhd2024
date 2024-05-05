/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef MSTAR_FB_GRAPHIC_H
#define MSTAR_FB_GRAPHIC_H

typedef struct
{
    MS_U32 ref_count;
    MS_U32 pseudo_palette[16];
}FBDEV_Controller;

#define MTK_FBDEV_COMPATIBLE_STR "MTK-fb"

/* ========================================================================= */
/* prototype of function */
static int _mstar_fb_open(struct fb_info *info, int user);
static int _mstar_fb_release(struct fb_info *info, int user);
static int mstar_fb_set_par(struct fb_info *pinfo);
static int mstar_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);
static int mstar_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info);
static int mstar_fb_blank(int blank, struct fb_info *info);
static int mstar_fb_setcolreg(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp, struct fb_info *info);
static int _mstar_fb_mmap(struct fb_info *pinfo, struct vm_area_struct *vma);
static void mstar_fb_destroy(struct fb_info *info);
static void mstar_fb_fillrect(struct fb_info *p, const struct fb_fillrect *rect);
static void mstar_fb_copyarea(struct fb_info *p, const struct fb_copyarea *area);
static void mstar_fb_imageblit(struct fb_info *p, const struct fb_image *image);
static MS_U32 get_line_length(int xres_virtual, int bpp);
static void _fb_strewin_update(struct fb_var_screeninfo *var);
static void _fb_gwin_enable(MS_BOOL bEnable);
static void _fb_buf_init(struct fb_info *pinfo, unsigned long pa);
static EN_DRV_GOPColorType get_color_fmt(struct fb_var_screeninfo *var);
#endif
