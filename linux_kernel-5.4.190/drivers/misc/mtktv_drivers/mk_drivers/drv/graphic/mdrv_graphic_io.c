// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

//=============================================================================
// Include Files
//=============================================================================
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/pfn.h>
#include <linux/delay.h>
#include <linux/compat.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/of.h>


//drver header files
#include "mtk-reserved-memory.h"
#include "mdrv_mstypes.h"
#include "chip_int.h"
#include "mst_devid.h"
#include "mdrv_graphic.h"
#include "mdrv_graphic_io.h"
#include "mdrv_system.h"
#include "mhal_graphic.h"

//=============================================================================
/* GOP Definition */
#define BPX                 32    /* bits per pixel */
#define BYTE_PER_BIT        (8)
#define PANEL_H_START       112
#define XRES                1920
#define YRES                1080
#define V_XRES              XRES
//#define V_YRES              YRES*2 /*  x2 for double buffer */
#define V_YRES              YRES /*  x2 for single buffer */
#define VIDEOMEMSIZE        (V_XRES * V_YRES * (BPX >> 3))  /* frame buffer size */
#define ALIGN_LOWER(value,factor) (value & (~(factor-1)))

#if 0 /* FIXME: -Wunused-function */
static int _mstar_fb_ioctl(struct fb_info *pinfo, MS_U32 u32Cmd, unsigned long u32Arg);
#endif

#define MIU0_BUS_OFFSET 	ARM_MIU0_BUS_BASE
#define MIU1_INTERVAL 		(ARM_MIU1_BUS_BASE - ARM_MIU0_BUS_BASE)
static MS_BOOL bGwinEnable=FALSE;
static MS_BOOL bFbdevProbe = FALSE;
static FBDEV_Controller fbControllers;

/* Device independent changeable information */
static struct fb_var_screeninfo mstar_fb_var =
{
    .xoffset = 0,
    .bits_per_pixel = 32, //BPX,
    .red =    { 16, 8, 0 },
    .green =  { 8 , 8, 0 },
    .blue =   { 0 , 8, 0 },
    .transp = { 24, 8, 0 },
    .activate = FB_ACTIVATE_NOW,
    .height = -1, /* =maximum, in fb.h is delcalred as _u32* */
    .width = -1,
    .grayscale = 0,

    /*  timing useless ? use the vfb default */
    .pixclock = 100000000000LLU / (6 *  1920 * 1080), /* pixel clock in ps (pico seconds) */
    .left_margin = 64, /* time from sync to picture */
    .right_margin = 64, /* time from picture to sync */
    .upper_margin = 32, /* time from sync to picture */
    .lower_margin = 32,
    .hsync_len = 64,    /* length of horizontal sync */
    .vsync_len = 2,     /* length of vertical sync */
    .vmode = FB_VMODE_NONINTERLACED,
    /* non interlaced, the most basical framebuffer */
};

/* Device independent unchangeable information */
static struct fb_fix_screeninfo mstar_fb_fix =
{
    .id = "MStar FB",
    .type = FB_TYPE_PACKED_PIXELS,    /* see FB_TYPE_* */
    .visual = FB_VISUAL_TRUECOLOR,    /* see FB_VISUAL_* */
    .xpanstep = (GOP_WordUnit/(BPX >> 3)),  /* zero if no hardware panning */
    .ypanstep = 1,
    .ywrapstep = 1,
    .line_length = (V_XRES * (BPX  >> 3)), /* length of a line in bytes */
    .mmio_start=0x08000000, /* Start of Memory Mapped I/O   */
                            /* (physical address) */
    .mmio_len = 0x10000000, /* Length of Memory Mapped I/O  */
    .smem_start = 0x0,
    .accel = FB_ACCEL_NONE,
    /* Indicate to driver which	specific chip/card we have */
};

static struct fb_ops mstar_fb_ops =
{
    .owner = THIS_MODULE,
    .fb_open = _mstar_fb_open,
    .fb_release = _mstar_fb_release,
    .fb_mmap = _mstar_fb_mmap,
    .fb_set_par = mstar_fb_set_par,
    .fb_check_var = mstar_fb_check_var,
    .fb_blank = mstar_fb_blank,
    .fb_pan_display = mstar_fb_pan_display,
    .fb_setcolreg = mstar_fb_setcolreg,
    .fb_fillrect = mstar_fb_fillrect,
    .fb_copyarea = mstar_fb_copyarea,
    .fb_imageblit = mstar_fb_imageblit,
    .fb_destroy = mstar_fb_destroy,
};

static struct notifier_block mtk_fb_notifier;

static int _mstar_fb_open(struct fb_info *info, int user)
{
    FBDEV_Controller* par;
    par = info->par;

    if(user)
    {
        par->ref_count++;
    }

    return 0;
}

static int _mstar_fb_release(struct fb_info *info, int user)
{
    FBDEV_Controller* par;
    par = info->par;

    if(user)
    {
        if (!par->ref_count)
            return -EINVAL;
        else
            par->ref_count--;
    }
    else
    {
        printk("\33[0;36m fb:%d user=%d \33[m \n",info->node,user);
    }

    return 0;
}

void _fb_gwin_update(struct fb_var_screeninfo *fbvar,  struct fb_info *pinfo)
{
    DRV_GWIN_INFO WinInfo;
    MS_U32 bytes_per_pixel = 0;
    MS_U64 u64PhyAddr = 0;
    MS_U8 u8MiuSel = 0;
    EN_DRV_GOPColorType ColorFmt;

    memset(&WinInfo, 0x0, sizeof(DRV_GWIN_INFO));

    if(fbvar->bits_per_pixel == 1)
       bytes_per_pixel = 1;
    else
       bytes_per_pixel = fbvar->bits_per_pixel/8;

    u64PhyAddr = mstar_fb_fix.smem_start;
    _phy_to_miu_offset(u8MiuSel, WinInfo.u64Addr, u64PhyAddr);

    WinInfo.u64Addr += (fbvar->xoffset + fbvar->xres_virtual*fbvar->yoffset)* bytes_per_pixel;

    _MDrv_GRAPHIC_MIUSel(GOP0, u8MiuSel);
    ColorFmt = get_color_fmt(fbvar);
    if (ColorFmt != E_DRV_GOP_COLOR_INVALID) {
        WinInfo.clrType = ColorFmt;
        WinInfo.u16HStart = 0;
        WinInfo.u16HEnd = fbvar->xres;
        WinInfo.u16VStart = 0;
        WinInfo.u16VEnd = fbvar->yres;
        WinInfo.u16Pitch = fbvar->xres_virtual;
        WinInfo.u16Height = fbvar->yres_virtual;
        _MDrv_GOP_SetGwinInfo(GOP0,0,WinInfo);
        _MDrv_GRAPHIC_UpdateReg(GOP0);
    } else {
        printk("[FBDEV][%s][%d] Invalid color format\n", __FUNCTION__, __LINE__);
    }
}

static int mstar_fb_set_par(struct fb_info *pinfo)
{
    struct fb_var_screeninfo *var = &pinfo->var;
    MS_U32 bits_per_pixel;

    switch (var->bits_per_pixel)
    {
       case 32:
       case 16:
            pinfo->fix.visual = FB_VISUAL_TRUECOLOR;
            bits_per_pixel = var->bits_per_pixel;
            break;
       case 1:
            pinfo->fix.visual = FB_VISUAL_MONO01;
            bits_per_pixel = 8;
            break;
       default:
            pinfo->fix.visual = FB_VISUAL_PSEUDOCOLOR;
            bits_per_pixel = 8;
            break;
    }

    pinfo->fix.line_length = (var->xres_virtual * bits_per_pixel) / 8;
    pinfo->fix.xpanstep = GOP_WordUnit/(bits_per_pixel >>3);
    /* activate this new configuration */

   return 0;
}

static int mstar_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
    MS_U32 line_length,bits_per_pixel;
    EN_DRV_GOPColorType ColorFmt;

    /*
     *  FB_VMODE_CONUPDATE and FB_VMODE_SMOOTH_XPAN are equal!
     *  as FB_VMODE_SMOOTH_XPAN is only used internally
     */
     if (var->vmode & FB_VMODE_CONUPDATE)
     {
         var->vmode |= FB_VMODE_YWRAP;
         var->xoffset = info->var.xoffset;
         var->yoffset = info->var.yoffset;
     }
     /*
      *  Some very basic checks
      */

     if (!var->xres)
     {
         var->xres = info->var.xres;
     }
     if (!var->yres)
     {
         var->yres = info->var.yres;
     }
     if (var->xres > var->xres_virtual)
     {
         var->xres_virtual = var->xres;
     }
     if (var->yres > var->yres_virtual)
     {
         var->yres_virtual = var->yres;
     }
     if (var->bits_per_pixel <= 1)
     {
         var->bits_per_pixel = 1;
         bits_per_pixel = 8;
     }else if (var->bits_per_pixel <= 8)
     {
         var->bits_per_pixel = 8;
         bits_per_pixel = 8;
     }else if (var->bits_per_pixel <= 16)
     {
         var->bits_per_pixel = 16;
         bits_per_pixel = 16;
     }else if (var->bits_per_pixel <= 32)
     {
         var->bits_per_pixel = 32;
         bits_per_pixel = 32;
     }else
         return -EINVAL;

     if (var->xres_virtual < var->xoffset + var->xres)
         var->xres_virtual = var->xoffset + var->xres;
     if (var->yres_virtual < var->yoffset + var->yres)
         var->yres_virtual = var->yoffset + var->yres;

     /*
      *  Memory limit
      */
     line_length = get_line_length(var->xres_virtual,bits_per_pixel);
     if(line_length * var->yres_virtual > VIDEOMEMSIZE)
        return -ENOMEM;

     /*
      * Now that we checked it we alter var. The reason being is that the video
      * mode passed in might not work but slight changes to it might make it
      * work. This way we let the user know what is acceptable.
      */
      ColorFmt = get_color_fmt(var);
      if(ColorFmt == E_DRV_GOP_COLOR_INVALID)
         return -EINVAL;

      return 0;
}

static int mstar_fb_blank(int blank, struct fb_info *info)
{
    return 0;
}

static int mstar_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *pinfo)
{
    //update gwin
    FBDEV_Controller* par;
    par = pinfo->par;

    //invoked by linux framebuffer framework fbcon notify
    if(!par->ref_count)
    {
        printk("%s fb:%d invoked by fbcon notify do nothing\n",__FUNCTION__,pinfo->node);
        return 0;
    }

     _fb_strewin_update(var);
     _fb_gwin_update(var,pinfo);

     //enable gwin
     if(bGwinEnable == FALSE)
     {
         _fb_gwin_enable(TRUE);
         bGwinEnable = TRUE;
     }

     //update  mstar_fb_var
     memcpy(&mstar_fb_var, var, sizeof(struct fb_var_screeninfo));

    return 0;
}


static int mstar_fb_setcolreg(unsigned regno, unsigned red, unsigned green,
                              unsigned blue, unsigned transp, struct fb_info *info)
{
    /* grayscale works only partially under directcolor */
    if(info->var.grayscale)
    {
        /* grayscale = 0.30*R + 0.59*G + 0.11*B */
        red = green = blue = (red * 77 + green * 151 + blue * 28) >> 8;
    }

    if(info->fix.visual == FB_VISUAL_TRUECOLOR || info->fix.visual == FB_VISUAL_DIRECTCOLOR)
    {
        MS_U32 v;

        if(regno >= 16)
            return -EINVAL;

        v = (red << info->var.red.offset) | (green << info->var.green.offset) | (blue << info->var.blue.offset) | (transp << info->var.transp.offset);
            ((U32*)(info->pseudo_palette))[regno] = v;
    }
    return 0;
}

static void mstar_fb_fillrect(struct fb_info *p, const struct fb_fillrect *rect)
{
#ifdef CONFIG_FB_VIRTUAL
        //sys_fillrect(p, rect);
#endif
}

static void mstar_fb_copyarea(struct fb_info *p, const struct fb_copyarea *area)
{
#ifdef CONFIG_FB_VIRTUAL
        //sys_copyarea(p, area);
#endif
}

static void mstar_fb_imageblit(struct fb_info *p, const struct fb_image *image)
{
#ifdef CONFIG_FB_VIRTUAL
        //sys_imageblit(p, image);
#endif
}

static void mstar_fb_destroy(struct fb_info *info)
{
    if(info->screen_base)
      iounmap(info->screen_base);
    framebuffer_release(info);
}

static MS_U32 get_line_length(int xres_virtual, int bpp)
{
    MS_U32 length;

    length = xres_virtual * bpp;
    length = (length + 31) & ~31;
    length >>= 3;

    return (length);
}

static EN_DRV_GOPColorType get_color_fmt(struct fb_var_screeninfo *var)
{
    EN_DRV_GOPColorType ColorFmt = E_DRV_GOP_COLOR_INVALID;

    switch (var->bits_per_pixel) {
        case 1:
        case 8:
            var->red.offset = 0;
            var->red.length = 8;
            var->green.offset = 0;
            var->green.length = 8;
            var->blue.offset = 0;
            var->blue.length = 8;
            var->transp.offset = 0;
            var->transp.length = 0;
            ColorFmt = E_DRV_GOP_COLOR_I8;
            break;
        case 16:
            if ( (var->transp.length) && (var->transp.offset == 15))
            {
                var->blue.offset = 0;
                var->blue.length = 5;
                var->green.offset = 5;
                var->green.length = 5;
                var->red.offset = 10;
                var->red.length = 5;
                var->transp.offset = 15;
                var->transp.length = 1;
                ColorFmt = E_DRV_GOP_COLOR_ARGB1555;
            }else if ((var->transp.length) && (var->transp.offset == 12))
            {
                var->blue.offset = 0;
                var->blue.length = 4;
                var->green.offset = 4;
                var->green.length = 4;
                var->red.offset = 8;
                var->red.length = 4;
                var->transp.offset = 12;
                var->transp.length = 4;
                ColorFmt = E_DRV_GOP_COLOR_ARGB4444;
            }else if ( (var->transp.length) && (var->transp.offset == 0))
            {
                var->transp.offset = 0;
                var->transp.length = 1;
                var->blue.offset = 1;
                var->blue.length = 5;
                var->green.offset = 6;
                var->green.length = 5;
                var->red.offset = 11;
                var->red.length = 5;
                ColorFmt = E_DRV_GOP_COLOR_RGBA5551;
            }else
            {
                /* RGB 565 */
                var->blue.offset = 0;
                var->blue.length = 5;
                var->green.offset = 5;
                var->green.length = 6;
                var->red.offset = 11;
                var->red.length = 5;
                var->transp.offset = 0;
                var->transp.length = 0;
                ColorFmt = E_DRV_GOP_COLOR_RGB565;
            }
            break;
            case 32:/* ARGB 8888 */
                if ((var->transp.length) && (var->red.offset == 16))
                {
                    var->blue.offset = 0;
                    var->blue.length = 8;
                    var->green.offset = 8;
                    var->green.length = 8;
                    var->red.offset = 16;
                    var->red.length = 8;
                    var->transp.offset = 24;
                    var->transp.length = 8;
                    ColorFmt = E_DRV_GOP_COLOR_ARGB8888;
                }else
                {
                    var->red.offset = 0;
                    var->red.length = 8;
                    var->green.offset = 8;
                    var->green.length = 8;
                    var->blue.offset = 16;
                    var->blue.length = 8;
                    var->transp.offset = 24;
                    var->transp.length = 8;
                    ColorFmt = E_DRV_GOP_COLOR_ABGR8888;
                }
                break;
      }

      var->red.msb_right = 0;
      var->green.msb_right = 0;
      var->blue.msb_right = 0;
      var->transp.msb_right = 0;
      return ColorFmt;
}

static void _fb_buf_init(struct fb_info *pinfo, unsigned long pa)
{
    pinfo->fix.smem_start = pa;
    //pinfo->fix.smem_len = pinfo->var.xres * pinfo->var.yres * (pinfo->var.bits_per_pixel >> 3)*2/*double buffer*/;
    pinfo->fix.smem_len = pinfo->var.xres * pinfo->var.yres * (pinfo->var.bits_per_pixel >> 3)/*single buffer*/;
    //pinfo->screen_base = (char __iomem *)ioremap(pa + MIU0_BUS_OFFSET, pinfo->fix.smem_len);
    //memset(pinfo->screen_base, 0x0, pinfo->fix.smem_len);
}

static void _fb_gwin_init(void)
{
    MS_BOOL bHMirror_Pnl = FALSE, bVMirror_Pnl = FALSE;

    _MDrv_GRAPHIC_Init(GOP0);

    MDrv_GRAPHIC_GetPanelMirrorMode(&bHMirror_Pnl, &bVMirror_Pnl);
    _MDrv_GRAPHIC_SetHMirror(GOP0, bHMirror_Pnl);
    _MDrv_GRAPHIC_SetVMirror(GOP0, bVMirror_Pnl);

#ifdef Box_Chip
    _MDrv_GRAPHIC_OutputColor_EX(GOP0,DRV_GOPOUT_YUV);
#else
    _MDrv_GRAPHIC_OutputColor_EX(GOP0,DRV_GOPOUT_RGB);
#endif
    _MDrv_GRAPHIC_SetGOPDst(GOP0, E_DRV_GOP_DST_OP0, FALSE);
    _MDrv_GRAPHIC_EnableTransClr_EX(GOP0,GOPTRANSCLR_FMT0, FALSE);
    _MDrv_GRAPHIC_SetBlending(GOP0, 0, FALSE, 0xFF);
}

static void _fb_gwin_enable(MS_BOOL bEnable)
{
    _MDrv_GRAPHIC_GWIN_Enable(GOP0,0,bEnable);
    _MDrv_GRAPHIC_UpdateReg(GOP0);
}

static void _fb_strewin_update(struct fb_var_screeninfo *var)
{
    MS_U16 u16Width = 0;
    MS_U16 u16Height = 0;

    _MDrv_GRAPHIC_GetXCTiming(&u16Width, &u16Height);
    _MDrv_GRAPHIC_SetStretchWin(GOP0,0,0,var->xres,var->yres);
    _MDrv_GRAPHIC_SetHScale(GOP0,TRUE,var->xres,u16Width);
    _MDrv_GRAPHIC_SetVScale(GOP0,TRUE,var->yres,u16Height);
}

//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
static int mtk_fb_notifier_callback(struct notifier_block *self,
	unsigned long event, void *data)
{
    struct fb_event *evdata = data;
    int blank;

    /* If we aren't interested in this event, skip it immediately ... */
    if (event != FB_EVENT_BLANK)
        return 0;

    if (!evdata) {
        printk("[FBDEV][%s][%d] null ptr of evdata\n", __FUNCTION__, __LINE__);
        return 0;
    }
    blank = *(int *)evdata->data;

    switch (blank) {
    case FB_BLANK_UNBLANK:
        if (!evdata->info) {
            printk("[FBDEV][%s][%d] null ptr of evdata->info\n", __FUNCTION__, __LINE__);
            return 0;
        }
        mstar_fb_pan_display(&evdata->info->var, evdata->info);
        break;
    default:
        break;
    }

    return 0;
}

static int mstar_fb_probe(struct platform_device *dev)
{
    struct fb_info *pinfo = NULL;
    int retval = -ENOMEM;
    MS_U16 u16Width = 0;
    MS_U16 u16Height = 0;
    MS_U16 u16WidthAlign = 0;
    MS_BOOL bSupportPixelMode = FALSE, bIsInitGOPByOthers = FALSE;
    void __iomem *RIUBase1, *RIUBase2;
    struct device_node *np;
    struct of_mmap_info_data mmap_info;

    if (bFbdevProbe)
        return 0;
    else
        bFbdevProbe = TRUE;

    if (NULL == dev)
    {
        printk("ERROR: in mstar_fb_prob: dev is NULL pointer \r\n");
        return -ENOTTY;
    }

    np = of_find_compatible_node(NULL, NULL, MTK_FBDEV_COMPATIBLE_STR);
    if (np == NULL) {
        printk("[FBDEV][%s][%d] can not find node MTK-fb\n", __FUNCTION__, __LINE__);
        return -EINVAL;
    }

    if (of_mtk_get_reserved_memory_info_by_idx(np, 0, &mmap_info)) {
        printk("[FBDEV][%s][%d] of_mtk_get_reserved_memory_info_by_idx fail\n", __FUNCTION__, __LINE__);
        return -EPERM;
    }

    mstar_fb_fix.smem_start = mmap_info.start_bus_address - GOP_GRAPHIC_MIUBUS_OFFSET;
    pinfo = framebuffer_alloc(sizeof(struct fb_info), &dev->dev);
    if (!pinfo)
        return retval;

    RIUBase1 = ioremap(GOP_GRAPHIC_REG_BASE, GOP_GRAPHIC_REG_SIZE);
    RIUBase2 = ioremap(GOP_GRAPHIC_CUROR_REG_BASE, GOP_GRAPHIC_CUROR_REG_SIZE);
    MDrv_GRAPHIC_SetIoBase(0, GOP_GRAPHIC_REG_BASE, GOP_GRAPHIC_REG_SIZE, (uintptr_t)RIUBase1);
    MDrv_GRAPHIC_SetIoBase(1, GOP_GRAPHIC_CUROR_REG_BASE, GOP_GRAPHIC_CUROR_REG_SIZE, (uintptr_t)RIUBase2);

    memset(&fbControllers, 0x0, sizeof(FBDEV_Controller));

    pinfo->fbops = &mstar_fb_ops;
    pinfo->pseudo_palette = fbControllers.pseudo_palette;
    pinfo->par = &fbControllers;
    pinfo->flags = FBINFO_FLAG_DEFAULT;
    if(0 != mstar_fb_fix.smem_start)
    {
        printk("\33[0;36m mstar_fb_fix.smem_start = 0x%lx \33[m \n",mstar_fb_fix.smem_start);

        _MDrv_GRAPHIC_GetXCTiming(&u16Width, &u16Height);
        if(u16Width >= XRES)
        {
            mstar_fb_var.xres = XRES;
            mstar_fb_var.yres = YRES;
            mstar_fb_var.xres_virtual = XRES;
            mstar_fb_var.yres_virtual = YRES;
        }
        else
        {
            bSupportPixelMode = MDrv_GRAPHIC_GetPixelModeSupport();
            if(bSupportPixelMode == FALSE)
            {
                u16WidthAlign = (GOP_WordUnit/(BPX/BYTE_PER_BIT));
                u16Width = ALIGN_LOWER(u16Width,u16WidthAlign);
            }

            mstar_fb_var.xres = u16Width;
            mstar_fb_var.yres = u16Height;
            mstar_fb_var.xres_virtual = u16Width;
            mstar_fb_var.yres_virtual = u16Height;
        }
        pinfo->var = mstar_fb_var;
        pinfo->fix = mstar_fb_fix;

        _fb_buf_init(pinfo, mstar_fb_fix.smem_start);
        _fb_gwin_init();

        bIsInitGOPByOthers = MDrv_GRAPHIC_IsInit(GOP0);
        if(bIsInitGOPByOthers == FALSE)
        {
            MDrv_GRAPHIC_InitGopPart(GOP0);
        }
    }
    else
    {
        printk("[Graphic Driver]parsing recovery buffer address error in bootargs\n");
        retval = -ENOTTY;
        goto err1;
    }
    retval = fb_alloc_cmap(&pinfo->cmap, 256, 0);
    if (retval < 0)
        goto err1;
    retval = register_framebuffer(pinfo);
    if (retval < 0)
        goto err2;

    mtk_fb_notifier.notifier_call = mtk_fb_notifier_callback;
    fb_register_client(&mtk_fb_notifier);

    platform_set_drvdata(dev, pinfo);
    printk("\33[0;36m  fb%d: Mstar frame buffer device \33[m \n",pinfo->node);

    return 0;

err2:
    fb_dealloc_cmap(&pinfo->cmap);
err1:
    framebuffer_release(pinfo);

    return retval;
}

static int mstar_fb_remove(struct platform_device *dev)
{
    struct fb_info *pinfo;
    pinfo = 0;

    if (NULL == dev)
    {
        printk("ERROR: mstar_fb_remove: dev is NULL pointer \n");
        return -ENOTTY;
    }
    _fb_gwin_enable(FALSE);
    bGwinEnable = FALSE;
    pinfo = platform_get_drvdata(dev);
    if (pinfo)
    {
        unregister_framebuffer(pinfo);
        framebuffer_release(pinfo);
    }
    return 0;
}

static void mstar_fb_platform_release(struct device *device)
{
    if (NULL == device)
    {
        printk("ERROR: in mstar_fb_platform_release, \
                device is NULL pointer !\r\n");
    }
    else
    {
        printk("in mstar_fb_platform_release, module unload!\n");
    }
}

#if defined (CONFIG_OF)
static struct of_device_id mtk_fb_of_device_ids[] = {
         {.compatible = "MTK-fb"},
};
#endif

/*device .name and driver .name must be the same, then it will call
       probe function */
static struct platform_driver Mstar_fb_driver =
{
    .probe  = mstar_fb_probe,    //initiailize
    .remove = mstar_fb_remove,   /*it free(mem),
                                   release framebuffer, free irq etc. */
    .driver =
    {
#if defined(CONFIG_OF)
	    .of_match_table = mtk_fb_of_device_ids,
#endif
        .name = "Mtk-fb",
    },
};

static u64 mstar_fb_device_lcd_dmamask = 0xffffffffUL;

static struct platform_device Mstar_fb_device =
{
    .name = "Mtk-fb",
    .id = 0,
    .dev =
    {
        .release = mstar_fb_platform_release,
        .dma_mask = &mstar_fb_device_lcd_dmamask,
        .coherent_dma_mask = 0xffffffffUL
    }
};

static int __init mstar_fb_init(void)
{
    int ret = 0;

    ret = platform_driver_register(&Mstar_fb_driver);

    if (!ret)
    {
        /*register driver sucess
          register device*/
        ret = platform_device_register(&Mstar_fb_device);
        if(ret)    /*if register device fail, then unregister the driver.*/
        {
            platform_driver_unregister(&Mstar_fb_driver);
        }
    }
    return ret;
}

static int _mstar_fb_mmap(struct fb_info *pinfo, struct vm_area_struct *vma)
{
      size_t size;
      size = 0;

      if (NULL == pinfo)
      {
         printk("ERROR: mstar_fb_mmap, pinfo is NULL pointer !\n");
         return -ENOTTY;
      }
      if (NULL == vma)
      {
          printk("ERROR: mstar_fb_mmap, vma is NULL pointer !\n");
          return -ENOTTY;
      }
      if (0 == pinfo->fix.smem_start)
      {
          printk("ERROR: mstar_fb_mmap, physical addr is NULL pointer !\n");
          return -ENOTTY;
      }

      size = vma->vm_end - vma->vm_start;
      vma->vm_pgoff = (pinfo->fix.smem_start + MIU0_BUS_OFFSET) >> PAGE_SHIFT;

      printk(
      "\33[0;36m 2 mstar_fb_mmap vma->vm_start=%x\n vma->vm_end=%x\n vma->vm_pgoff =%x \33[m \n",
      (unsigned int) vma->vm_start, (unsigned int)vma->vm_end ,
      (unsigned int)vma->vm_pgoff);

       vma->vm_flags |= VM_IO | VM_DONTEXPAND | VM_DONTDUMP;
       vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
       vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
      // Remap-pfn-range will mark the range VM_IO and VM_RESERVED

      if (remap_pfn_range(vma, vma->vm_start,
          vma->vm_pgoff, size, vma->vm_page_prot))
      return -EAGAIN;

      return 0;
}

#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
#else
module_init(mstar_fb_init);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("GRAPHIC ioctrl driver");
MODULE_LICENSE("GPL");
#endif//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
