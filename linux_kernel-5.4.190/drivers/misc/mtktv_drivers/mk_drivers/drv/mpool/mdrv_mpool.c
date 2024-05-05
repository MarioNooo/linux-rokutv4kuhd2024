/* SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause */
/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2019 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2019 MediaTek Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_mpool.c
/// @brief  Memory Pool Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <generated/autoconf.h>
//#include <linux/undefconf.h> //unused header file now
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>  //added
#include <linux/timer.h> //added
#include <linux/device.h>
#include <linux/version.h>
#include <linux/cma.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/cacheflush.h>
#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
#include <linux/mmap_lock.h>
#endif
#include "cma.h"

#include <internal.h>

#include "mst_devid.h"
#include "mdrv_mpool.h"
#include "mhal_mpool.h"
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"
#include "mdrv_mstypes.h"

#include "chip_setup.h"

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
//#define MPOOL_DPRINTK(fmt, args...) printk(KERN_WARNING"%s:%d " fmt,__FUNCTION__,__LINE__,## args)
#define MPOOL_DPRINTK(fmt, args...)

//-------------------------------------------------------------------------------------------------
//  Global variables
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MPOOL_VERSION 1
#define MOD_MPOOL_DEVICE_COUNT     1
#define MOD_MPOOL_NAME             "malloc"

#define KER_CACHEMODE_UNCACHE_NONBUFFERABLE 0
#define KER_CACHEMODE_CACHE   1
#define KER_CACHEMODE_UNCACHE_BUFFERABLE 2

// Define MPOOL Device
U32 linux_base;
U32 linux_size;
U32 linux2_base;
U32 linux2_size;
U32 emac_base;
U32 emac_size;

#if defined(CONFIG_ARM)
typedef  struct
{
   U32 mpool_base;
   U32 mpool_size;
   U32 mmap_offset;
   U32 mmap_size;
   U32 mmap_interval;
   U8  mmap_miusel;
   unsigned int u8MapCached;
   bool setflag;
}MMAP_FileData;
#elif defined(CONFIG_ARM64)
typedef  struct
{
   u64 mpool_base;
   u64 mpool_size;
   u64 mmap_offset;
   u64 mmap_size;
   u64 mmap_interval;
   U8  mmap_miusel;
   unsigned int u8MapCached;
   bool setflag;
}MMAP_FileData;

#endif

//bool setflag;
static struct mutex mpool_iomap_mutex = __MUTEX_INITIALIZER(mpool_iomap_mutex);

unsigned int mpool_version = MPOOL_VERSION;

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------;

typedef struct
{
    int                         s32MPoolMajor;
    int                         s32MPoolMinor;
    void*                       dmaBuf;
    struct cdev                 cDevice;
    struct file_operations      MPoolFop;
} MPoolModHandle;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _MDrv_MPOOL_Open (struct inode *inode, struct file *filp);
static int                      _MDrv_MPOOL_Release(struct inode *inode, struct file *filp);
static int                      _MDrv_MPOOL_MMap(struct file *filp, struct vm_area_struct *vma);

static long                     _MDrv_MPOOL_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);


#if defined(CONFIG_COMPAT)
static long Compat_MDrv_MPOOL_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#endif

//Add by austin, remove linux page from pfn range
static inline int mpool_io_remap_range(struct vm_area_struct *vma, unsigned long addr,
                unsigned long pfn, unsigned long size, pgprot_t prot);

//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------

static struct class *mpool_class;

static MPoolModHandle MPoolDev=
{
	.s32MPoolMajor=               MDRV_MAJOR_MPOOL,
	.s32MPoolMinor=               MDRV_MINOR_MPOOL,
	.cDevice=
	{
		.kobj=                  {.name= MOD_MPOOL_NAME, },
		.owner  =               THIS_MODULE,
	},
	.MPoolFop=
	{
		.open=                  _MDrv_MPOOL_Open,
		.release=               _MDrv_MPOOL_Release,
		.mmap=                  _MDrv_MPOOL_MMap,
		.unlocked_ioctl=        _MDrv_MPOOL_Ioctl,
#if defined(CONFIG_COMPAT)
		.compat_ioctl = Compat_MDrv_MPOOL_Ioctl,
#endif
	},
};



//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static int is_cma_range(struct cma *cma, void *data)
{
	phys_addr_t start, end, addr;

	addr = *(phys_addr_t *)data;
	start = PFN_PHYS(cma->base_pfn);
	end = start + (cma->count << PAGE_SHIFT);

	if (addr >= start && addr < end)
		return 1;

	return 0;
}

int is_remap_cma_page(unsigned long pfn)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
	if(!pfn_is_map_memory(pfn))
#else
	if(!pfn_valid(pfn))
#endif
		return 0;
	pfn <<= PAGE_SHIFT;
	return cma_for_each_area(is_cma_range, &pfn);
}
EXPORT_SYMBOL(is_remap_cma_page);

static inline int mpool_io_remap_range(struct vm_area_struct *vma, unsigned long addr,
		    unsigned long pfn, unsigned long size, pgprot_t prot)
{
	unsigned long end = addr + PAGE_ALIGN(size);
	int err;

	vma->vm_flags |= VM_PFNMAP;

#if (!defined CONFIG_MTK_TV_SLT) && (!defined TEST_SYZKALLER)
	WARN(1, "%s is trying to mmap mpool buffer via /dev/malloc. \
			And user mod mpool mmap is deprecated, \
			please removed the call and use kernel driver to map the mpool buffer.\n \
			Contact: Mark-PK Tsai, Glenn Lin\n", current->comm);
#endif
	do {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
		if (pfn_is_map_memory(pfn) && !is_remap_cma_page(pfn))
#else
		if (pfn_valid(pfn) && !is_remap_cma_page(pfn))
#endif
			pr_warn_ratelimited("mpool warning: trying to map system ram (pfn: %lx)\n", pfn);

		/* pfn_valid(pfn) means the page is in linux memory
		 * we will also map linux memory to user_space
		 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
		if(!pfn_is_map_memory(pfn) || is_remap_cma_page(pfn))
#else
		if(!pfn_valid(pfn) || is_remap_cma_page(pfn))
#endif
		{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
			err = vmf_insert_pfn(vma, addr, pfn);
#else
			err = vm_insert_pfn(vma, addr, pfn);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
			if (err != VM_FAULT_NOPAGE)
#else
				if (err)
#endif
					break;
		}
	}while(pfn++, addr += PAGE_SIZE, addr != end);

	return 0;
}

static int _MDrv_MPOOL_Open (struct inode *inode, struct file *filp)
{
    MMAP_FileData *mmapData;

    mmapData = kzalloc(sizeof(*mmapData), GFP_KERNEL);
    if (mmapData == NULL)
          return -ENOMEM;

    filp->private_data = mmapData;
    mmapData->u8MapCached = 1;
    mmapData->setflag = false;

    return 0;
}

static int _MDrv_MPOOL_Release(struct inode *inode, struct file *filp)
{
    MMAP_FileData *mmapData = filp->private_data ;
    kfree(mmapData);

    // iounmap(dev->dmaBuf) ;
    return 0;
}


/* used by user mode msos */
static int _MDrv_MPOOL_MMap(struct file *filp, struct vm_area_struct *vma)
{
	MMAP_FileData *mmapData = filp->private_data;
	u32 miu0_len = 0x10000000;
	unsigned long BUS_BASE = 0;

	mutex_lock(&mpool_iomap_mutex);
	if (!mmapData->setflag)
		vma->vm_pgoff = mmapData->mpool_base >> PAGE_SHIFT;
	else {
		if (mmapData->mmap_miusel) {
			pr_err("mpool: miu%d not support\n",mmapData->mmap_miusel);
			WARN_ON(1);
			return -EINVAL;
		}

		vma->vm_pgoff = (mmapData->mmap_offset+ARM_MIU0_BASE_ADDR) >> PAGE_SHIFT;
	}

	/* set page to no cache */
	if(mmapData->u8MapCached == KER_CACHEMODE_CACHE)
	{
#if defined(CONFIG_ARM)
		vma->vm_page_prot=__pgprot_modify(vma->vm_page_prot,L_PTE_MT_MASK,L_PTE_MT_DEV_CACHED);
#elif defined(CONFIG_ARM64)
		/*pgprot_val(vma->vm_page_prot) = pgprot_val(pgprot_cached(vma->vm_page_prot));*/
		vma->vm_page_prot = __pgprot_modify(vma->vm_page_prot, PTE_ATTRINDX_MASK, PTE_ATTRINDX(MT_NORMAL) | PTE_UXN);
#endif
	} else {
		/* (mmapData->u8MapCached == KER_CACHEMODE_UNCACHE_BUFFERABLE) */
#if defined(CONFIG_ARM)
		//pgprot_val(vma->vm_page_prot) = pgprot_dmacoherent(vma->vm_page_prot); //This solution is only to modify mpool mmaping rule,
		//The Correct solution is to enable config_ARM_DMA_MEM_BUFFERABLE, but if enable config_ARM_DMA_MEM_BUFFERABLE , system will trigger the app to crash.
		//if enable config_ARM_DMA_MEM_BUFFERABLE maybe have coherence, so app will crash now. but this solution only modify mpoool mmaping, mpool always have flush pipe. 
		pgprot_val(vma->vm_page_prot)= __pgprot_modify(vma->vm_page_prot, L_PTE_MT_MASK, L_PTE_MT_BUFFERABLE | L_PTE_XN);
#else
		pgprot_val(vma->vm_page_prot) = pgprot_val(pgprot_dmacoherent(vma->vm_page_prot));
#endif
	}

	if(mmapData->setflag)
	{
		if(mmapData->mmap_miusel == 0)
		{
			BUS_BASE = ARM_MIU0_BUS_BASE;
		}
		else
		{
			panic("miu%d not support\n",mmapData->mmap_miusel);
		}

		{
			if(mpool_io_remap_range(vma, vma->vm_start,
						(BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, mmapData->mmap_size,
						vma->vm_page_prot))
			{
				mutex_unlock(&mpool_iomap_mutex);
				return -EAGAIN;
			}
		}
	} else {
		unsigned long u32MIU0_MapStart = 0;
		unsigned long u32MIU0_MapSize = 0;
		unsigned long u32MIU1_MapStart = 0;
		unsigned long u32MIU1_MapSize = 0;


		//calculate map size & start
		if(mmapData->mpool_base<miu0_len)
		{
			u32MIU0_MapStart=mmapData->mpool_base;

			if((mmapData->mpool_base+mmapData->mpool_size)>miu0_len)
			{
				u32MIU0_MapSize=(miu0_len-mmapData->mpool_base);
				u32MIU1_MapSize=mmapData->mpool_size-u32MIU0_MapSize;
				u32MIU1_MapStart=ARM_MIU1_BUS_BASE;
			}
			else
			{
				u32MIU0_MapSize=mmapData->mpool_size;
				u32MIU1_MapSize=0;
				u32MIU1_MapStart=ARM_MIU1_BUS_BASE;
			}
		}
		else
		{
			u32MIU0_MapStart=0;
			u32MIU0_MapSize=0;
			u32MIU1_MapStart=ARM_MIU1_BUS_BASE;
			u32MIU1_MapSize=mmapData->mpool_size;
		}

		if(u32MIU0_MapSize)
		{
			if (mpool_io_remap_range(vma, vma->vm_start+ mmapData->mpool_base,
						u32MIU0_MapStart >> PAGE_SHIFT, u32MIU0_MapSize,
						vma->vm_page_prot))
			{
				mutex_unlock(&mpool_iomap_mutex);
				return -EAGAIN;
			}
		}

		if(u32MIU1_MapSize)
		{
			if(mpool_io_remap_range(vma, vma->vm_start+u32MIU0_MapSize,
						ARM_MIU1_BUS_BASE >> PAGE_SHIFT, u32MIU1_MapSize,
						vma->vm_page_prot))
			{
				mutex_unlock(&mpool_iomap_mutex);
				return -EAGAIN;
			}
		}
	}

	mutex_unlock(&mpool_iomap_mutex);
	return 0;
}

static int is_module_addr(const void *x)
{
#if defined(CONFIG_MODULES) && defined(MODULES_VADDR)
	unsigned long addr = (unsigned long)x;
	if (addr >= MODULES_VADDR && addr < MODULES_END)
		return 1;
#endif

	return 0;
}

static bool is_user_va(const void *p)
{
	if (!virt_addr_valid(p) &&
			!is_vmalloc_addr(p) &&
			!is_module_addr(p))
		return true;
	else
		return false;
}

#if defined(CONFIG_COMPAT)
static long Compat_MDrv_MPOOL_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg){

	switch(cmd)
	{
		case MPOOL_IOC_INFO:
		case MPOOL_IOC_FLUSHDCACHE:
		case MPOOL_IOC_SET:
		case MPOOL_IOC_KERNEL_DETECT:
		case MPOOL_IOC_VERSION:
		case MPOOL_IOC_FLUSHDCACHE_ALL:
		case MPOOL_IOC_GET_BLOCK_OFFSET:
		case MPOOL_IOC_PA2BA:
		case MPOOL_IOC_BA2PA:
		case MPOOL_IOC_SET_MAP_CACHE:
		{
			return filp->f_op->unlocked_ioctl(filp, cmd,(unsigned long)compat_ptr(arg));
		}
		case COMPAT_MPOOL_IOC_FLUSHDCACHE_PAVA:
		{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,13))  //case MPOOL_IOC_FLUSHDCACHE_PAVA:
			return filp->f_op->unlocked_ioctl(filp, MPOOL_IOC_FLUSHDCACHE_PAVA,(unsigned long)compat_ptr(arg));
#else
			compat_u64 u64_data;
			compat_size_t u;
			int	err= 0;

			DrvMPool_Flush_Info_t32 __user *data32;
			DrvMPool_Flush_Info_t __user *data;
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			data32 = compat_ptr(arg);
			err = get_user(u, &data32->u32AddrVirt);
			err |= put_user(u, &data->u32AddrVirt);
			err |= get_user(u64_data, &data32->u32AddrPhys);
			err |= put_user(u64_data, &data->u32AddrPhys);
			err |= get_user(u64_data, &data32->u32Size);
			err |= put_user(u64_data, &data->u32Size);
			if (err)
				return err;

			return filp->f_op->unlocked_ioctl(filp, MPOOL_IOC_FLUSHDCACHE_PAVA,(unsigned long)data);
#endif //#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,13))
		}
		default:
			return -ENOIOCTLCMD;
	}
	return -ENOIOCTLCMD;
}
#endif

static long _MDrv_MPOOL_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int         err= 0;
	int         ret= 0;

	MMAP_FileData *mmapData = filp->private_data ;

	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (MPOOL_IOC_MAGIC!= _IOC_TYPE(cmd))
	{
		return -ENOTTY;
	}

	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
	{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
#else
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
#endif
	}
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
	{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
		err =  !access_ok((void __user *)arg, _IOC_SIZE(cmd));
#else
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
#endif
	}
	if (err)
	{
		return -EFAULT;
	}

	switch(cmd) {
	case MPOOL_IOC_FLUSHDCACHE:
		{
			DrvMPool_Info_t i;
			struct mm_struct *mm = current->active_mm;
			struct vm_area_struct *vma;
			unsigned long start,end;
#ifdef CONFIG_CPU_SW_DOMAIN_PAN
			unsigned int __ua_flags;
#endif

			ret = copy_from_user(&i, (void __user *)arg, sizeof(i));
			if (ret) {
				pr_err("%s(%d) copy_from_user failed!\n", __func__, __LINE__);
				return ret;
			}

			// check input va is user va or kernel va
			if (!access_ok((void __user *)(unsigned long)i.u32Addr, i.u32Size)) {
				pr_err("mpool: invalid va(%lx) or sz(%lx)\n",
						(unsigned long)i.u32Addr, (unsigned long)i.u32Size);
				return -EINVAL;
			}

			start = i.u32Addr;
			end = i.u32Addr + i.u32Size;
			if (end < start) {
				pr_err("mpool: invalid va(%lx) or sz(%lx)\n",
						(unsigned long)i.u32Addr, (unsigned long)i.u32Size);
				return -EINVAL;
			}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
			mmap_read_lock(mm);
#else
			down_read(&mm->mmap_sem);
#endif
			vma = find_vma(mm, start);
			if (!vma) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
				mmap_read_unlock(mm);
#else
				up_read(&mm->mmap_sem);
#endif
				return -EINVAL;
			}

#ifdef CONFIG_ARM64
			if ((pgprot_val(vma->vm_page_prot) & PTE_ATTRINDX_MASK) != PTE_ATTRINDX(MT_NORMAL)) {
#else /* CONFIG_ARM */
			if ((pgprot_val(vma->vm_page_prot) & L_PTE_MT_MASK) != L_PTE_MT_DEV_CACHED) {
#endif
				pr_err("flush range is not cached map\n");
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
				mmap_read_unlock(mm);
#else
				up_read(&mm->mmap_sem);
#endif
				break;
			}

			if (vma && vma->vm_start < end) {
				if (start < vma->vm_start)
					start = vma->vm_start;
				if (end > vma->vm_end)
					end = vma->vm_end;

				i.u32Addr = start;
				i.u32Size = end - start;

#ifdef CONFIG_ARM64_SW_TTBR0_PAN
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,13))
				uaccess_ttbr0_enable();
#else
				uaccess_enable_not_uao();
#endif
#elif defined(CONFIG_CPU_SW_DOMAIN_PAN)
				__ua_flags = uaccess_save_and_enable();
#endif
				if (is_user_va((void *)start)) {
					/* user va */
#ifdef CONFIG_ARM64
					flush_cache_range(vma, start, end);
#endif
				} else
					MDrv_MPOOL_IOC_FlushDache(i);

#ifdef CONFIG_ARM
				outer_flush_range(start, end);
#endif
#ifdef CONFIG_ARM64_SW_TTBR0_PAN
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,13))
				uaccess_ttbr0_disable();
#else
				uaccess_disable_not_uao();
#endif
#elif defined(CONFIG_CPU_SW_DOMAIN_PAN)
				uaccess_restore(__ua_flags);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
				mmap_read_unlock(mm);
#else
				up_read(&mm->mmap_sem);
#endif
			} else {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
				mmap_read_unlock(mm);
#else
				up_read(&mm->mmap_sem);
#endif
				return -EINVAL;
			}
			break;
		}
	case MPOOL_IOC_FLUSHDCACHE_PAVA:
		{

			DrvMPool_Flush_Info_t i;
			struct mm_struct *mm = current->active_mm;
			struct vm_area_struct *vma;
			unsigned long start,end,size;
#ifdef CONFIG_CPU_SW_DOMAIN_PAN
			unsigned int __ua_flags;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0) && defined(CONFIG_COMPAT))
			if(in_compat_syscall()){
				compat_u64 u64_data;
				compat_size_t u;
				DrvMPool_Flush_Info_t32 __user *data32;
				data32 = compat_ptr(arg);
				ret = get_user(u64_data, &data32->u32AddrVirt);
				i.u32AddrVirt = u64_data;
				ret |= get_user(u, &data32->u32Size);
				i.u32Size = u;
			}else{
				ret = copy_from_user(&i, (void __user *)arg, sizeof(i));
			}
#else
			ret = copy_from_user(&i, (void __user *)arg, sizeof(i));
#endif
			if (ret) {
				pr_err("%s(%d) copy_from_user failed!\n", __func__, __LINE__);
				return ret;
			}

			// check input va is user va or kernel va
			if (!access_ok((void *)i.u32AddrVirt, i.u32Size)) {
				pr_err("error input va\n");
				return -EINVAL;
			}

			start = i.u32AddrVirt;
			end = i.u32AddrVirt + i.u32Size;
			if (end < start)
				return -EINVAL;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
			mmap_read_lock(mm);
#else
			down_read(&mm->mmap_sem);
#endif
			vma = find_vma(mm, start);
			if (vma && vma->vm_start < end) {
				if (start < vma->vm_start)
					start = vma->vm_start;
				if (end > vma->vm_end)
					end = vma->vm_end;
				size = end - start;

				if (is_user_va((void *)start)) {
					/* user va */
#ifdef CONFIG_ARM64
					flush_cache_range(vma, start, end);
#endif
				} else
					/* Actually Chip_Flush_Cache_Range_VA_PA only use va */
					Chip_Flush_Cache_Range_VA_PA(start, (i.u32AddrPhys - ARM_MIU0_BASE_ADDR) + ARM_MIU0_BASE_ADDR , size);

#ifdef CONFIG_ARM64_SW_TTBR0_PAN
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,13))
				uaccess_ttbr0_enable();
#else
				uaccess_enable_not_uao();
#endif
#elif defined(CONFIG_CPU_SW_DOMAIN_PAN)
				__ua_flags = uaccess_save_and_enable();
#endif

#ifdef CONFIG_ARM
				outer_flush_range(start, end);
#endif

#ifdef CONFIG_ARM64_SW_TTBR0_PAN
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,13))
				uaccess_ttbr0_disable();
#else
				uaccess_disable_not_uao();
#endif
#elif defined(CONFIG_CPU_SW_DOMAIN_PAN)
				uaccess_restore(__ua_flags);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
				mmap_read_unlock(mm);
#else
				up_read(&mm->mmap_sem);
#endif
			} else {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
				mmap_read_unlock(mm);
#else
				up_read(&mm->mmap_sem);
#endif
				return -EINVAL;
			}
		}
		break ;
	case MPOOL_IOC_SET_MAP_CACHE:
		{
			ret= copy_from_user(&mmapData->u8MapCached, (void __user *)arg, sizeof(mmapData->u8MapCached));
		}
		break;
	case MPOOL_IOC_SET:
		{
			DrvMPool_Info_t i;

			memset(&i, 0, sizeof(DrvMPool_Info_t));
			ret= copy_from_user(&i, (void __user *)arg, sizeof(i));
			mmapData->setflag = true;
			mmapData->mmap_offset = i.u32Addr;
			mmapData->mmap_size = i.u32Size;
			mmapData->mmap_interval = i.u32Interval;
			mmapData->mmap_miusel = i.u8MiuSel;
#if (!defined CONFIG_MTK_TV_SLT) && (!defined TEST_SYZKALLER)
			pr_err("comm: %s\n", current->comm);
			WARN(1, "you're trying to map (%lx ~ %lx) miusel (%u) via mpool ioctl\n",
					(unsigned long)i.u32Addr,
					(unsigned long)(i.u32Addr + i.u32Size),
					(unsigned int)i.u8MiuSel);
#endif
		}
		break;

	case MPOOL_IOC_KERNEL_DETECT:
		{
			DrvMPool_Kernel_Info_t i;
			i.u32lxAddr = linux_base;
			i.u32lxSize = linux_size;
			i.u32lx2Addr = linux2_base;
			i.u32lx2Size = linux2_size;

			printk("lxaddr = %08llx, lxsize = %08llx\n", i.u32lxAddr, i.u32lxSize);
			printk("lx2addr = %08llx, lx2size = %08llx\n", i.u32lx2Addr, i.u32lx2Size);
			ret= copy_to_user( (void *)arg, &i, sizeof(i) );
		}
		break;
	case MPOOL_IOC_VERSION:
		{
			ret= copy_to_user( (void *)arg, &mpool_version, sizeof(mpool_version) );
		}
		break;

	case MPOOL_IOC_FLUSHDCACHE_ALL:
		{
			WARN(1, "mpool: don't support flush dcache all any more, use dma_buf instead if you need dma\n");
#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
			Chip_Flush_Cache_All();
#endif
		}
		break ;
	case MPOOL_IOC_PA2BA:
	case MPOOL_IOC_BA2PA:
	case MPOOL_IOC_INFO:
	case MPOOL_IOC_GET_BLOCK_OFFSET:
	default:
		printk("Unknown ioctl command %d\n", cmd);
		return -ENOTTY;
	}
	return 0;
}

static dev_t mpool_dev;

static int __init mod_mpool_init(void)
{
	int s32Ret;

	mpool_class = class_create(THIS_MODULE, "mpool");
	if (IS_ERR(mpool_class))
	{
		return PTR_ERR(mpool_class);
	}

	if (MPoolDev.s32MPoolMajor)
	{
		mpool_dev = MKDEV(MPoolDev.s32MPoolMajor, MPoolDev.s32MPoolMinor);
		s32Ret = register_chrdev_region(mpool_dev, MOD_MPOOL_DEVICE_COUNT, MOD_MPOOL_NAME);
		if (s32Ret) {
			MPOOL_DPRINTK("Unable to get major %d\n", MPoolDev.s32MPoolMajor);
			class_destroy(mpool_class);
			return s32Ret;
		}
	} else {
		s32Ret = alloc_chrdev_region(&mpool_dev, MPoolDev.s32MPoolMinor, MOD_MPOOL_DEVICE_COUNT, MOD_MPOOL_NAME);
		if (s32Ret) {
			MPOOL_DPRINTK("Unable to get major %d\n", MPoolDev.s32MPoolMajor);
			class_destroy(mpool_class);
			return s32Ret;
		}

		MPoolDev.s32MPoolMajor = MAJOR(mpool_dev);
	}


	cdev_init(&MPoolDev.cDevice, &MPoolDev.MPoolFop);
	if (0!= (s32Ret= cdev_add(&MPoolDev.cDevice, mpool_dev, MOD_MPOOL_DEVICE_COUNT)))
	{
		MPOOL_DPRINTK("Unable add a character device\n");
		unregister_chrdev_region(mpool_dev, MOD_MPOOL_DEVICE_COUNT);
		class_destroy(mpool_class);
		return s32Ret;
	}


	device_create(mpool_class, NULL, mpool_dev, NULL, MOD_MPOOL_NAME);
	return 0;
}

int MDrv_MPOOL_IOC_FlushDache(DrvMPool_Info_t i)
{
        return MHal_MPOOL_IOC_FlushDache(i);
}

static void __exit mod_mpool_exit(void)
{
    cdev_del(&MPoolDev.cDevice);
    unregister_chrdev_region(MKDEV(MPoolDev.s32MPoolMajor, MPoolDev.s32MPoolMinor), MOD_MPOOL_DEVICE_COUNT);

    device_destroy(mpool_class, MKDEV(MPoolDev.s32MPoolMajor, MPoolDev.s32MPoolMinor));
    class_destroy(mpool_class);
}

#if defined(CONFIG_MSTAR_MPOOL) || defined(CONFIG_MSTAR_MPOOL_MODULE)
module_init(mod_mpool_init);
module_exit(mod_mpool_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("MPOOL driver");
MODULE_LICENSE("GPL");
#endif//#if defined(CONFIG_MSTAR_MPOOL) || defined(CONFIG_MSTAR_MPOOL_MODULE)
