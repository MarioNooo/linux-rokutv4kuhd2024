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
/// @file   mdrv_cma_mpool.c
/// @brief  CMA mpool interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <linux/version.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/debugfs.h>
#include <asm/io.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)
#include <linux/uaccess.h>
#else
#include <asm/uaccess.h>
#endif
//#include <asm/dma-contiguous.h>
#include <asm/cacheflush.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/freezer.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/proc_fs.h>
#include <linux/swap.h>
#include <linux/ptrace.h>
//#include <linux/dma-contiguous.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,49)
#include <linux/cma.h>
#include <cma.h>
#endif

#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif
#include <linux/vmalloc.h>

#include "mdrv_types.h"
#include "mst_devid.h"
#include "mdrv_system.h"
#include "mdrv_cma_pool.h"

#include <linux/profile.h>//profile_munmap need this header file
#include <linux/ptrace.h>//ptrace_may_access function need this header file
#include <linux/highmem.h>
#include <asm/fixmap.h>

#include <mach/memory.h>


// new included header
#include "mtk-reserved-memory.h"
#include "mtk-cma.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
struct file *_fileCMAPool = NULL;
EXPORT_SYMBOL(_fileCMAPool);

#define PHYSICAL_START_INIT     UL(-1)
#define PHYSICAL_END_INIT       0
#define INVALID_MIU          0xFF
#define INVALID_PID          0
#define MAX_ALLOC_TRY        30
#define MOD_CMAPOOL_DEVICE_COUNT     1
#define MOD_CMAPOOL_NAME             "cmapool"
#define CMA_POOL_DUMP 1
#define CMA_POOL_KERNEL_PROTECT_DUMP 1
#define DBUG_UTOPIA_TO_KERNEL_CODE 1
#ifdef DBUG_UTOPIA_TO_KERNEL_CODE
#define MCMA_CUST_DEBUG(fmt, args...) printk(CMA_ERR "MCMA debug %s:%d " fmt,__FUNCTION__,__LINE__,## args)
#else
#define MCMA_CUST_DEBUG(fmt, args...)
#endif

#define MCMA_BUG_ON(cond)  \
do { \
      if(cond) {\
        printk(CMA_ERR "MCMA_BUG in %s @ %d \n", __FUNCTION__, __LINE__); \
		Show_CMA_Pool_state(); \
	  }\
      BUG_ON(cond); \
   } while(0)

#define MCMA_CUST_ERR(fmt, args...)     printk(CMA_ERR "MCMA error %s:%d " fmt, __FUNCTION__, __LINE__, ## args)
#define MCMA_CUST_WARNING(fmt, args...) printk(CMA_ERR "MCMA warning %s:%d " fmt, __FUNCTION__, __LINE__, ## args)
//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------
typedef enum
{
    CMA_ALLOC = 0,
    CMA_FREE
} BUFF_OPS;

typedef struct
{
    int                         s32Major;
    int                         s32Minor;
    struct dentry *              debug_root;
    struct cdev                 cDevice;
    struct file_operations      CMAPoolFop;
} CMAPoolModHandle;
//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int CMA_Pool_Open(struct inode *inode, struct file *filp);
static int CMA_Pool_Release(struct inode *inode, struct file *filp);
static int CMA_Pool_Free(unsigned int heap_id, unsigned long offset_in_heap, unsigned long length, filp_private * pdev, bool delay_free);
static long CMA_Pool_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

#if defined(CONFIG_COMPAT)
static long Compat_CMA_Pool_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#endif

static int CMA_Pool_MMap(struct file *filp, struct vm_area_struct *vma);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)
static vm_fault_t _cma_cpu_page_fault_handler(struct vm_fault *vmf);
#else
int _cma_cpu_page_fault_handler(struct vm_area_struct *vma, struct vm_fault *vmf);
#endif
void _cma_vma_close(struct vm_area_struct * vma);
static unsigned long CMA_Pool_Mapping(struct heap_proc_info * proc_info, struct vm_area_struct *vma, unsigned long start_pa, unsigned long length, filp_private *pdev);
static int CMA_Pool_Unmapping(struct heap_proc_info * proc_info, struct vm_area_struct *vma, unsigned long start_pa, unsigned long length, filp_private *pdev);
static void dump_CMA_Pool(struct cma_memory_info * heap_info, bool force_print);
static void free_buffer_list(struct cma_memory_info * heap_info, struct cma_buffer ** release_buf_front , struct cma_buffer ** release_buf_back);
static void free_buffer_list_and_release_special_buf(struct cma_memory_info *heap_info, struct cma_buffer **release_buf_front , struct cma_buffer **release_buf_back);
static struct cma_buffer * find_cma_buffer(struct cma_memory_info * heap_info, unsigned long start_pa, unsigned long length);
static int deleteKRange(struct cma_memory_info * heap_info, struct cma_buffer * buffer);
static int addKRange(struct cma_memory_info * heap_info, struct cma_buffer * buffer);
static void dumpKRange(struct cma_memory_info * heap_info);
static void Show_CMA_Pool_state(void);

//---------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------
#ifdef CONFIG_MP_CMA_PATCH_MBOOT_STR_USE_CMA
extern int mboot_str_heap_id;

#define MBOOT_STR_CMA_ALLOC_CNT 5
struct mboot_str_cma_alloc_log {
	struct cma_buffer str_cma_alloc_buffer;
	unsigned long start_pa;
	int alloc_page_cnt;
	int used;
};
static struct mboot_str_cma_alloc_log mboot_str_cma_alloc[MBOOT_STR_CMA_ALLOC_CNT];
#endif

static DEFINE_MUTEX(heap_info_lock);
static struct list_head heap_info_list; //record heap related info
static unsigned int *kernal_protect_client_id = NULL;
static atomic_t kprotect_enabled = ATOMIC_INIT(1);

unsigned int bootarg_cma_debug_level = 0;
unsigned int temp_cma_debug_level = 0;
struct timer_list cma_debug_timer;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)
static void cma_debug_callback(struct timer_list *t);
#else
static void cma_debug_callback(unsigned long);
#endif

#ifdef CONFIG_MP_CMA_PATCH_COUNT_TIMECOST
extern struct cma_measurement cma_measurement_data[ION_HEAP_ID_RESERVED];
#endif

#ifdef CONFIG_MP_CMA_PATCH_DELAY_FREE
static DEFINE_MUTEX(delay_free_lock);
static int delay_free_size_limite = 52428800; //50M
static int delay_free_time_limite = 0;
extern int lowmem_minfree[6];
extern int lowmem_minfree_size;
static int delay_free_lowmem_minfree = 17 * 1024;
static uint32_t delay_free_last_force_free_jiffies = 0;
static uint32_t delay_free_evict_duration = 0;

static bool is_in_cma_force_free_period(void)
{
	if(jiffies-delay_free_last_force_free_jiffies<delay_free_evict_duration)
		return true;
	return false;
}

LIST_HEAD(delay_list);

struct delay_free_reserved {
	int delay_free_heap_id;
	int delay_free_offset_in_heap;
	int delay_free_length;
	filp_private *delay_free_pdev;
	int delay_free_time_out;
	struct list_head list;
};

static struct task_struct *delay_free_tsk;

static int delay_free(void* arg)
{
	int ret;
	struct list_head *pos, *q;
	struct delay_free_reserved *tmp;
	while(1){
		mutex_lock(&delay_free_lock);
		list_for_each_safe(pos, q, &delay_list){
			tmp = list_entry(pos, struct delay_free_reserved, list);

			if(tmp->delay_free_time_out == 0 && tmp->delay_free_heap_id != -1){
				printk(CMA_DEBUG "\033[0;32;31m [Delay Free] %s %d delay_free_heap_id %d\033[m\n",__func__,__LINE__,tmp->delay_free_heap_id);
				ret = CMA_Pool_Free(tmp->delay_free_heap_id,tmp->delay_free_offset_in_heap,
											tmp->delay_free_length,tmp->delay_free_pdev,false);
				if(ret)
					MCMA_BUG_ON(1);

				list_del(pos);
				kfree(tmp);
			}
			else if(tmp->delay_free_time_out > 0){
				tmp->delay_free_time_out--;
			}

		}
		mutex_unlock(&delay_free_lock);
		msleep(1000);
	}

	return 0;
}

void set_delay_free_min_mem(int min_mem)
{
    delay_free_lowmem_minfree = min_mem;
}
#endif

static struct class *cmapool_class;
static CMAPoolModHandle CMAPoolDev =
{
    .s32Major=               MDRV_MAJOR_CMAPOOL,
    .s32Minor=               MDRV_MINOR_CMAPOOL,
    .debug_root=             NULL,
    .cDevice=
    {
        .kobj=                  {.name= MOD_CMAPOOL_NAME, },
        .owner  =               THIS_MODULE,
    },
    .CMAPoolFop=
    {
        .open=                  CMA_Pool_Open,
        .release=               CMA_Pool_Release,
        .mmap=                  CMA_Pool_MMap,
        .unlocked_ioctl=        CMA_Pool_ioctl,
#if defined(CONFIG_COMPAT)
		.compat_ioctl 	= 		Compat_CMA_Pool_ioctl,
#endif
    },
};

static struct vm_operations_struct cma_vm_ops =
{
    .close = _cma_vma_close,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    .fault = _cma_cpu_page_fault_handler
#else
    .nopfn = _cma_cpu_page_fault_handler
#endif
};

static inline void phy_to_MiuOffset(unsigned long phy_addr, unsigned int *miu, unsigned long *offset)
{
	*miu = INVALID_MIU;

    if(phy_addr >= ARM_MIU2_BUS_BASE)
    {
        *miu = 2;
        *offset = phy_addr - ARM_MIU2_BUS_BASE;
    }
    else if(phy_addr >= ARM_MIU1_BUS_BASE)
    {
        *miu = 1;
        *offset = phy_addr - ARM_MIU1_BUS_BASE;
    }
    else if(phy_addr >= ARM_MIU0_BUS_BASE)
    {
        *miu = 0;
        *offset = phy_addr - ARM_MIU0_BUS_BASE;
    }
	else
		printk(CMA_ERR "\033[35mFunction = %s, Line = %d, Error, Unknown MIU, for phy_addr is 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, phy_addr);
}

static inline void user_phy_to_kernel_va(unsigned long user_phy_addr, unsigned long *kernel_phy_addr)
{
	/*
    such as some chip:
	miu0 addr ARM_MIU0_BUS_BASE=20000000  ARM_MIU0_BASE_ADDR=0
	miu1 addr ARM_MIU1_BUS_BASE=a0000000  ARM_MIU1_BASE_ADDR=80000000
	miu2 addr ARM_MIU2_BUS_BASE=300000000  ARM_MIU2_BASE_ADDR=c0000000
	miu3 addr ARM_MIU3_BUS_BASE=ffffffffffffffff  ARM_MIU3_BASE_ADDR=ffffffffffffffff
	*/

    if(user_phy_addr >= ARM_MIU2_BASE_ADDR)
    {
        *kernel_phy_addr = user_phy_addr + (ARM_MIU2_BUS_BASE-ARM_MIU2_BASE_ADDR);
    }
    else if(user_phy_addr >= ARM_MIU1_BASE_ADDR)
    {
        *kernel_phy_addr = user_phy_addr + (ARM_MIU1_BUS_BASE-ARM_MIU1_BASE_ADDR);
    }
    else if(user_phy_addr >= ARM_MIU0_BASE_ADDR)
    {
        *kernel_phy_addr = user_phy_addr + (ARM_MIU0_BUS_BASE-ARM_MIU0_BASE_ADDR);
    }
    else
    {
        printk(CMA_ERR "\033[35mFunction = %s, Line = %d, Error, Unknown MIU, for user_phy_addr is 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, user_phy_addr);
    }
}

#define SIZE_8K (8192)

struct vm_struct *map_kernel_get_vm_area(unsigned long bus_addr, unsigned long heap_length)
{
    struct vm_struct *area;
    unsigned long flags = VM_MAP;
    area = __get_vm_area(heap_length, flags, VMALLOC_START, VMALLOC_END);

    return area;
}

void map_kernel(struct vm_struct *area, struct heap_proc_info *proc_info, unsigned long offset_in_heap, unsigned long length)
{
    int err;
    pgprot_t pgprot;
    int npages = PAGE_ALIGN(length) / PAGE_SIZE;
    unsigned long addr;
    unsigned long end;
	unsigned long start_pfn;
    struct page **pages = vmalloc(sizeof(struct page *) * npages);
    struct page **tmp = pages;
    int i = 0;

    if(length + offset_in_heap > area->size)
    {
        MCMA_CUST_ERR("over range ,unable to map!!!\n");
		vfree(pages);
        return;
    }

    if(!IS_ALIGNED(offset_in_heap, SIZE_8K) || !IS_ALIGNED(length, SIZE_8K))
    {
		printk(CMA_ERR "input parameter not align to 8K, offset_in_heap %lx, length %lx\n",
		offset_in_heap, length);
		vfree(pages);
		return;
    }

    if(true==proc_info->vma_cached)
        pgprot = PAGE_KERNEL;
    else
         pgprot = pgprot_writecombine(PAGE_KERNEL);

    addr = (unsigned long)area->addr + offset_in_heap;
    end = addr + length;

	start_pfn = __phys_to_pfn(proc_info->base_phy + offset_in_heap);

    for(i = 0; i < npages; ++i)
    {
		*(tmp++) = __pfn_to_page(start_pfn);
		start_pfn++;
    }

    err = map_kernel_range_noflush(addr, length, pgprot, pages);
    flush_cache_vmap(addr, end);
    vfree(pages);

    if (err > 0) {
        if(npages  != err)
            MCMA_CUST_ERR("vmap_page_range not total vmap!!! npages = %d, but err is = %d\n", npages, err);
    } else
        MCMA_CUST_ERR("err = %d, npages = %d\n", err, npages);
}

void unmap_kernel(struct vm_struct *area)
{
    vunmap(area->addr);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)
static vm_fault_t _cma_cpu_page_fault_handler(struct vm_fault *vmf)
#else
int _cma_cpu_page_fault_handler(struct vm_area_struct *vma, struct vm_fault *vmf)
#endif
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)
    MCMA_CUST_ERR("vmf->address = %lX \n", vmf->address);
#else
    void __user *address = vmf->virtual_address;
    MCMA_CUST_ERR("vmf->virtual_address = %lX \n", (unsigned long)address);
#endif
	printk(KERN_EMERG "\033[35mFunction = %s, Line = %d, die\033[m\n", __PRETTY_FUNCTION__, __LINE__);
    return VM_FAULT_SIGBUS;
}

void _cma_vma_close(struct vm_area_struct * vma)
{
}

static int CMA_Pool_Open(struct inode *inode, struct file *filp)
{
    filp_private *pdev = kzalloc(sizeof(filp_private), GFP_KERNEL);
    if (pdev == NULL)
          return -ENOMEM;
    pdev->pid = task_pid_nr(current->group_leader);
    INIT_LIST_HEAD(&pdev->list_heap_proc_info);
    mutex_init(&pdev->lock);
    filp->private_data = pdev;
    return 0;
}

static inline struct cma_memory_info *find_heap_info(unsigned int heap_id)
{
    struct cma_memory_info *heap_info = NULL;
    bool find = false;

    mutex_lock(&heap_info_lock);
    list_for_each_entry(heap_info, &heap_info_list, list_node)
    {
        if(heap_id == heap_info->heap_id)
        {
            find = true;
            break;
        }
    }
    mutex_unlock(&heap_info_lock);

    if(find)
        return heap_info;
    else
        return NULL;
}

static inline struct heap_proc_info *find_heap_proc_info(unsigned int heap_id, filp_private *pdev)
{
    struct heap_proc_info *proc_info = NULL;
    bool find = false;

    mutex_lock(&pdev->lock);
    list_for_each_entry(proc_info, &pdev->list_heap_proc_info, list_node)
    {
        if(proc_info->heap_id == heap_id)
        {
            find = true;
            break;
        }
    }
    mutex_unlock(&pdev->lock);

    if(find)
        return proc_info;
    else
        return NULL;
}

/* release a specified heap with heap_id, all allocated_buffers will be freed */
static int CMA_Pool_Release_Heap(unsigned int heap_id, filp_private *pdev)
{
	pid_t pid = pdev->pid;
	struct cma_memory_info *heap_info = NULL;
	struct cma_buffer *buffer_node, *next;
	struct cma_buffer *release_buf_front = NULL, *release_buf_back = NULL;
	struct cma_allocation_list *alloc_list = NULL;
	struct heap_proc_info *proc_info = NULL;
	struct kernel_cma_pool_vma_item *vma_item = NULL, *vma_item_n = NULL;
#ifdef CONFIG_MP_CMA_PATCH_DELAY_FREE
	struct list_head *pos, *q;
	struct delay_free_reserved *tmp;
#endif

	heap_info = find_heap_info(heap_id);
	if(!heap_info)
		return -EINVAL;

#if CMA_POOL_DUMP
	printk(CMA_ERR "\033[31mFunction = %s, Line = %d, pid is %d, current->pid is %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, pid, current->pid);
	printk(CMA_ERR "\033[31mRelease for heap_id %u\033[m\n", heap_id);
#endif

	mutex_lock(&heap_info->lock);
	alloc_list = &heap_info->alloc_list;
	list_for_each_entry_safe(buffer_node, next, &alloc_list->list_head, list)
	{
		if(buffer_node->pid == pid)
		{
			if(buffer_node->freed == 0)
			{
				printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, ready to freed buffer_start: 0x%lX, length: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, buffer_node->start_pa, buffer_node->length);
				buffer_node->freed = 1;
				buffer_node->pid = INVALID_PID;
				heap_info->alloc_list.using_count--;
				heap_info->alloc_list.freed_count++;
			}
		}
	}

	free_buffer_list_and_release_special_buf(heap_info, &release_buf_front, &release_buf_back);

	dump_CMA_Pool(heap_info, true);
	dumpKRange(heap_info);
	mutex_unlock(&heap_info->lock);

	proc_info = find_heap_proc_info(heap_id, pdev);
	mutex_lock(&pdev->lock);
	if(proc_info != NULL)
	{
		if((proc_info->type_states == TYPE_STATES_KERNEL)
			&& proc_info->kernel_vm)
		{
			printk(KERN_EMERG "\033[35mFunction = %s, unmap kernel va: 0x%lX\033[m\n",
				__PRETTY_FUNCTION__, proc_info->kernel_vm->addr);
			unmap_kernel(proc_info->kernel_vm);
			proc_info->kernel_vm = NULL;

			// list del about vma_item
			mutex_lock(&proc_info->vma_list_lock);
			list_for_each_entry_safe(vma_item, vma_item_n, &proc_info->vma_list, list_node)
			{
				// do what SYSCALL_DEFINE2(munmap, unsigned long, addr, size_t, len) do
				printk(KERN_EMERG "\033[35mFunction = %s, unmap user va: 0x%lX ~ 0x%lX\033[m\n",
					__PRETTY_FUNCTION__, vma_item->vma->vm_start, vma_item->vma->vm_end);
				profile_munmap(vma_item->vma->vm_start);
				vm_munmap(vma_item->vma->vm_start, vma_item->vma->vm_end - vma_item->vma->vm_start);

				vma_item->vma = NULL;

				printk(CMA_ERR "%s: %d delete vma_item\n", __FUNCTION__, __LINE__);
				list_del(&vma_item->list_node);
				kfree(vma_item);
			}
			mutex_unlock(&proc_info->vma_list_lock);
		}
	
		printk(CMA_DEBUG "%s: %d delete proc_info\n", __FUNCTION__, __LINE__);
		list_del(&proc_info->list_node);
		kfree(proc_info);
	}
	mutex_unlock(&pdev->lock);

#ifdef CONFIG_MP_CMA_PATCH_DELAY_FREE
	mutex_lock(&delay_free_lock);
	list_for_each_safe(pos, q, &delay_list)
	{
		tmp = list_entry(pos, struct delay_free_reserved, list);
		if(heap_info->heap_id == tmp->delay_free_heap_id && pid == tmp->delay_free_pdev->pid)
		{
			list_del(pos);
			kfree(tmp);
		}
	}
	mutex_unlock(&delay_free_lock);
#endif

	return 0;
}

static int CMA_Pool_Release(struct inode *inode, struct file *filp)
{
	filp_private *pdev = (filp_private *)filp->private_data;
	pid_t pid = pdev->pid;
	struct cma_memory_info *heap_info = NULL, *next_heap = NULL;
	struct cma_buffer *buffer_node, *next;
	struct cma_buffer *release_buf_front = NULL, *release_buf_back = NULL;
	struct cma_allocation_list *alloc_list = NULL;
	struct heap_proc_info *proc_info = NULL, *proc_n = NULL;
#ifdef CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
	struct pid *pid_now;
#endif

#if CMA_POOL_DUMP
	printk(CMA_ERR "\033[31mFunction = %s, Line = %d, pid is %d, current->pid is %d, current->tgid is %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, pid, current->pid, current->tgid);
	printk(CMA_ERR "\033[31mFunction = %s, Line = %d, [%s] do CMA_Pool_Release for /dev/cmapool close\033[m\n", __PRETTY_FUNCTION__, __LINE__, current->comm);
#endif

	mutex_lock(&heap_info_lock);
	list_for_each_entry_safe(heap_info, next_heap, &heap_info_list, list_node)	// search for all heap_info(each heap_id has a heap_info)
	{
#ifdef CONFIG_MP_CMA_PATCH_DELAY_FREE
		struct list_head *pos, *q;
		struct delay_free_reserved *tmp;
		mutex_lock(&delay_free_lock);

		list_for_each_safe(pos, q, &delay_list){
			tmp = list_entry(pos, struct delay_free_reserved, list);
			if(heap_info->heap_id == tmp->delay_free_heap_id && pid == tmp->delay_free_pdev->pid){
				printk(CMA_DEBUG "\033[0;32;31m [Delay Free] %s %d Release %d\033[m\n",__func__,__LINE__,tmp->delay_free_heap_id);
				list_del(pos);
				kfree(tmp);
			}
		}
		mutex_unlock(&delay_free_lock);
#endif
		mutex_lock(&heap_info->lock);
		dump_CMA_Pool(heap_info, true);

		alloc_list = &heap_info->alloc_list;
		list_for_each_entry_safe(buffer_node, next, &alloc_list->list_head, list)	// search this heap's alloc_list, find each buffer for this pid, mark as freed(only free this pid's allocation)
		{
			if(buffer_node->pid == pid)
			{
				if(buffer_node->freed == 0)
				{
					printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, ready to freed buffer_start: 0x%lX, length: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, buffer_node->start_pa, buffer_node->length);
					buffer_node->freed = 1;
					buffer_node->pid = INVALID_PID;
					heap_info->alloc_list.using_count--;
					heap_info->alloc_list.freed_count++;
				}
			}
		}

		free_buffer_list_and_release_special_buf(heap_info, &release_buf_front, &release_buf_back);			// for this heap, check if having free buffer

		dump_CMA_Pool(heap_info, true);
		dumpKRange(heap_info);
		mutex_unlock(&heap_info->lock);
	}
	mutex_unlock(&heap_info_lock);

	mutex_lock(&pdev->lock);
#ifdef CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
	// add this to check kernel mode fd, for removing user_mode va mmaping form pa2kegx
    if(_fileCMAPool)
	{
		filp_private *kernel_mode_pdev = (filp_private *)_fileCMAPool->private_data;
		list_for_each_entry_safe(proc_info, proc_n, &kernel_mode_pdev->list_heap_proc_info, list_node)	// kernel_mode_pdev is kernel mode fd
		{
			struct kernel_cma_pool_vma_item *vma_item = NULL, *vma_item_n = NULL;
			mutex_lock(&proc_info->vma_list_lock);
			list_for_each_entry_safe(vma_item, vma_item_n, &proc_info->vma_list, list_node)
			{
				if(vma_item->pid == pid)	// vma_item->pid use tgid(process_id)
				{
					// do what SYSCALL_DEFINE2(munmap, unsigned long, addr, size_t, len) do
					// we do not do user_mode va unmap here, to prevent the case that cma_free do unmmaping and cma_release do unmapping again
					// if user_mode fd is closed without doing cma_free, the user_mode va will not be unmapped.
					// @ normal case, user_mode fd is closed due to process crashed, without unmaaping is OK
					// @ special case, user_mode fd is closed due to fd_close, without unmaaping is NG(TBD)
					//profile_munmap(vma_item->vma->vm_start);
					//vm_munmap(vma_item->vma->vm_start, vma_item->vma->vm_end - vma_item->vma->vm_start);

					vma_item->vma = NULL;
					pid_now = find_get_pid(pid);
					printk(CMA_ERR "%s: %d delete user_mode vma_item, owner is pid: %d(%s)\n", __FUNCTION__, __LINE__, pid, pid_task(pid_now, PIDTYPE_PID)->comm);
					put_pid(pid_now);
					list_del(&vma_item->list_node);
				}
			}
			mutex_unlock(&proc_info->vma_list_lock);
		}
		proc_info = NULL;
		proc_n = NULL;
	}
#endif

    list_for_each_entry_safe(proc_info, proc_n, &pdev->list_heap_proc_info, list_node)	// pdev is user_mode fd
    {
#ifdef CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
        //list del about vma_item
        struct kernel_cma_pool_vma_item *vma_item = NULL, *vma_item_n = NULL;
        mutex_lock(&proc_info->vma_list_lock);

        list_for_each_entry_safe(vma_item, vma_item_n, &proc_info->vma_list, list_node)
        {
            //do what SYSCALL_DEFINE2(munmap, unsigned long, addr, size_t, len) do
            //refer to find_lively_task_by_vpid()
            struct task_struct *tsk;

            rcu_read_lock();
            if (!vma_item->pid)
                tsk = current;
            else
                tsk = find_task_by_vpid(vma_item->pid);
            if (tsk)
                get_task_struct(tsk);
            rcu_read_unlock();
            if (tsk)
            {
                if(ptrace_may_access(tsk, PTRACE_MODE_READ))
                {
                    struct mm_struct *mm = tsk->mm;
                    unsigned long start=vma_item->vma->vm_start;
                    size_t len=vma_item->vma->vm_end - vma_item->vma->vm_start;
                    if(mm){
                        down_write(&mm->mmap_sem);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)
			do_munmap(mm, start, len, NULL);
#else
			do_munmap(mm, start, len);
#endif
                        up_write(&mm->mmap_sem);
                    }
                }
                put_task_struct(tsk);
            }
            vma_item->vma =NULL;

            printk(CMA_ERR "%s: %d delete vma_item\n", __FUNCTION__, __LINE__);
            list_del(&vma_item->list_node);
            kfree(vma_item);
        }
        mutex_unlock(&proc_info->vma_list_lock);

        //in this CMA_Pool_Release function,we should also unmap user mode user space va!!!
        //do what SYSCALL_DEFINE2(munmap, unsigned long, addr, size_t, len) do
        if(proc_info->vma)
        {
            if(current->mm){
				//profile_munmap(proc_info->vma->vm_start);
				printk("%s:%d WILL user mode user space vma item vm_munmap vm_start=0x%lx,vm_end=0x%lx\n",__FUNCTION__,__LINE__,proc_info->vma->vm_start, proc_info->vma->vm_end);
				vm_munmap(proc_info->vma->vm_start, proc_info->vma->vm_end - proc_info->vma->vm_start);
            }
            proc_info->vma =NULL;
        }
#endif
        printk(CMA_ERR "%s: %d delete proc_info\n", __FUNCTION__, __LINE__);
        list_del(&proc_info->list_node);
        kfree(proc_info);
    }
    mutex_unlock(&pdev->lock);

	kfree(pdev);

	return 0;
}

static int CMA_Pool_Init(unsigned int heap_id, unsigned long flags, unsigned int *miu, u64 *bus_addr, u64 *heap_start_offset, unsigned long *heap_length,
	size_t *virt_addr, filp_private *pdev)
{
    int ret = 0;
    unsigned long pfn = 0, offset = 0;
    unsigned int miu_index = INVALID_MIU;
    struct device * dev = NULL;
    struct cma_memory_info *heap_info;				// for a heap, created once here, and never be freed
    struct heap_proc_info *proc_info = NULL;		// for a process, handling the vma setting, freed when fd_close
    bool isNew_heap_proc_info = false;

#if CMA_POOL_DUMP
	printk("\n");
	printk(CMA_ERR "%s:%d heapid %u, flags = %lu, [%d, %d, name = %s]\n", __FUNCTION__, __LINE__, heap_id, flags, current->pid, current->tgid, current->comm);
	printk("\n");
#endif
	*virt_addr = 0;

    proc_info = find_heap_proc_info(heap_id, pdev);
    if(!proc_info)
    {
		printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, create proc_info\033[m\n", __PRETTY_FUNCTION__, __LINE__);
        proc_info = (struct heap_proc_info *)kzalloc(sizeof(struct heap_proc_info), GFP_KERNEL);
        if(!proc_info)
        {
            MCMA_CUST_ERR("no memory\n");
            return -ENOMEM;
        }

        proc_info->vma = NULL;
        INIT_LIST_HEAD(&proc_info->list_node);

		INIT_LIST_HEAD(&proc_info->vma_list);
		mutex_init(&proc_info->vma_list_lock);

        isNew_heap_proc_info = true;
    }
    else
    {
        printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, already having proc_info, do not init again\033[m\n", __PRETTY_FUNCTION__, __LINE__);
        if(((flags &CMA_FLAG_MAP_KERNEL) && (TYPE_STATES_USER  == proc_info->type_states)) ||
			((!(flags &CMA_FLAG_MAP_KERNEL)) && (TYPE_STATES_KERNEL == proc_info->type_states)))
        {
            printk(CMA_ERR "\033[35mFunction = %s, Line = %d, already having proc_info, but want to use simultaneously in kernel mode and user mode, flags = %lu, proc_info->type_states = %d\033[m\n",
				__PRETTY_FUNCTION__, __LINE__, flags, proc_info->type_states);
            MCMA_BUG_ON(1);
        }
    }

    heap_info = find_heap_info(heap_id);	// heap_info will be created here for only once, never be cleared
    if(heap_info)
    {
        printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, already having heap_info, do not init again\033[m\n", __PRETTY_FUNCTION__, __LINE__);
        *miu = heap_info->miu;
        *heap_length = heap_info->dev->cma_area->count * PAGE_SIZE;
        pdev->temp_heap_id = heap_id;
        proc_info->heap_id = heap_id;
        proc_info->base_phy = PFN_PHYS(heap_info->dev->cma_area->base_pfn);

        phy_to_MiuOffset(proc_info->base_phy, &miu_index, &offset);
        *heap_start_offset = offset;
        *bus_addr = proc_info->base_phy;

        if(isNew_heap_proc_info)
        {
            printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, adding proc_info\033[m\n", __PRETTY_FUNCTION__, __LINE__);
            mutex_lock(&pdev->lock);
			/* for each process, process's heap usage is proc_info
			 * and proc_info is added into process's list_heap_proc_info.
			 * we will nerver change the cache_type after proc_info is created
			 */ 
			if(flags & CMA_FLAG_CACHED)
				proc_info->vma_cached = true;
            list_add(&proc_info->list_node, &pdev->list_heap_proc_info);
            mutex_unlock(&pdev->lock);
        }

        ret = 0;
        goto PARSE_FLAGS_PARAM;
    }

    printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, create heap_info\033[m\n", __PRETTY_FUNCTION__, __LINE__);
	dev = mtkcma_id_device_mapping(heap_id);
    if(dev == NULL)
    {
        MCMA_CUST_ERR("not find the cma device for heap id %d\n", heap_id);
        if(isNew_heap_proc_info)
            kfree(proc_info);

        return -ENXIO;
    }

    heap_info = (struct cma_memory_info *)kzalloc(sizeof(struct cma_memory_info), GFP_KERNEL);
    if(!heap_info)
    {
        MCMA_CUST_ERR("out of memory!\n");
        if(isNew_heap_proc_info)
            kfree(proc_info);

        return -ENOMEM;
    }

    pfn = dev->cma_area->base_pfn;
    MCMA_BUG_ON(!pfn_valid(pfn));

    proc_info->heap_id = heap_id;
    proc_info->base_phy = PFN_PHYS(pfn);

    pdev->temp_heap_id = heap_id;
    phy_to_MiuOffset(proc_info->base_phy, &miu_index, &offset);
    heap_info->miu = miu_index;
    MCMA_BUG_ON(heap_info->miu >= 2);
    heap_info->heap_id = heap_id;
    heap_info->dev = dev;
    heap_info->alloc_list.min_start = PHYSICAL_START_INIT;
    heap_info->alloc_list.max_end = PHYSICAL_END_INIT;
    heap_info->alloc_list.using_count = 0;
    heap_info->alloc_list.freed_count = 0;
    INIT_LIST_HEAD(&heap_info->alloc_list.list_head);

    INIT_LIST_HEAD(&heap_info->list_node);
    mutex_init(&heap_info->lock);

    mutex_lock(&heap_info_lock);
    list_add(&heap_info->list_node, &heap_info_list);
    mutex_unlock(&heap_info_lock);

    *miu = heap_info->miu;
    *heap_length = dev->cma_area->count * PAGE_SIZE;
    *heap_start_offset = offset;
    *bus_addr = proc_info->base_phy;

    if(isNew_heap_proc_info)
    {
		printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, adding proc_info\033[m\n", __PRETTY_FUNCTION__, __LINE__);
        mutex_lock(&pdev->lock);
		if(flags & CMA_FLAG_CACHED)
			proc_info->vma_cached = true;
        list_add(&proc_info->list_node, &pdev->list_heap_proc_info);
        mutex_unlock(&pdev->lock);
    }

    //if for kernel utopia use, should have set CMA_FLAG_MAP_KERNEL flag,
    //but if for user utopia use, must not set that.
PARSE_FLAGS_PARAM:
    if(flags & CMA_FLAG_MAP_KERNEL) {
        proc_info->type_states = TYPE_STATES_KERNEL;
        if(flags & CMA_FLAG_MAP_VMA)
        {
            if (NULL == proc_info->kernel_vm)
            {
                proc_info->kernel_vm = map_kernel_get_vm_area(*bus_addr, *heap_length);
                if (!proc_info->kernel_vm)
                {
                    MCMA_CUST_ERR("get kernel_vm fail flags=%lu\n", flags);
                    return -EFAULT;
                }
            }
            *virt_addr = (unsigned long)(proc_info->kernel_vm->addr);
            MCMA_CUST_DEBUG("va: 0x%lx, length: 0x%lx, 0x%lx~0x%lx\n",
				*virt_addr, proc_info->kernel_vm->size, *virt_addr, (*virt_addr) + proc_info->kernel_vm->size);
        }
    }
    else
        proc_info->type_states = TYPE_STATES_USER;

    dumpKRange(heap_info);
    return ret;
}

//before call this API, lock heap info mutex firstly
static struct cma_buffer *SplitBuffer(unsigned long start_pa, unsigned long length, struct cma_buffer *find,
        struct cma_memory_info *heap_info, BUFF_OPS ops)
{
    struct cma_buffer *buffer_node = NULL;
    struct cma_allocation_list *alloc_list = &heap_info->alloc_list;

    MCMA_BUG_ON(!find || start_pa < find->start_pa || ((start_pa + length) > (find->start_pa + find->length)));

    if(ops == CMA_ALLOC)
        MCMA_BUG_ON(find->freed == 0);
    else
        MCMA_BUG_ON(find->freed == 1);

    if (start_pa == find->start_pa) {
        if(length == find->length) {

			/*   bufffer_node(start_pa, length) = find
			 *  |-------------------------------------|
			 *
			 *  buffer_node is find
			 */
			buffer_node = find;
            if(ops == CMA_ALLOC)
            {
                buffer_node->freed = 0;
                alloc_list->freed_count--;
                alloc_list->using_count++;
            }
            else
            {
                alloc_list->freed_count++;
                alloc_list->using_count--;
                buffer_node->freed = 1;
                buffer_node->cpu_addr = 0;
                buffer_node->pid = INVALID_PID;
            }
        } else {

			/*   bufffer_node(start_pa, length)    find
			 *  |------------------------------|----------|
			 *
			 *  buffer_node is part of find, at the begginning of find
			 */
            buffer_node = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
            MCMA_BUG_ON(!buffer_node);
            buffer_node->start_pa = find->start_pa;	// buffer_node is the to-be-freed buffer
            buffer_node->length = length;
            buffer_node->page = find->page;
			buffer_node->cpu_addr = find->cpu_addr;
            INIT_LIST_HEAD(&buffer_node->list);

			// adjust the find buffer
            find->start_pa = buffer_node->start_pa + buffer_node->length;
            find->length -= buffer_node->length;
            find->page = phys_to_page(find->start_pa);
			find->cpu_addr = buffer_node->cpu_addr + buffer_node->length;
            list_add(&buffer_node->list, find->list.prev);

            if(ops == CMA_ALLOC)
            {
                buffer_node->freed = 0;
                alloc_list->using_count++;
            }
            else
            {
                alloc_list->freed_count++;
                buffer_node->freed = 1;
                buffer_node->cpu_addr = 0;
                buffer_node->pid = INVALID_PID;
            }
        }
    } else if((start_pa + length) == (find->start_pa + find->length)) {

		/*     find   bufffer_node(start_pa, length)
		 *  |--------|-----------------------------|
		 *
		 *  buffer_node is part of find, at the end of find
		 */
		find->length -= length;
		buffer_node = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
		MCMA_BUG_ON(!buffer_node);
		buffer_node->start_pa = find->start_pa + find->length;
		buffer_node->length = length;
		buffer_node->page = phys_to_page(buffer_node->start_pa);
		buffer_node->cpu_addr = find->cpu_addr + find->length;
        INIT_LIST_HEAD(&buffer_node->list);
        list_add(&buffer_node->list, &find->list);

        if(ops == CMA_ALLOC)
        {
            buffer_node->freed = 0;
            alloc_list->using_count++;
        }
        else
        {
            alloc_list->freed_count++;
            buffer_node->freed = 1;
            buffer_node->cpu_addr = 0;
            buffer_node->pid = INVALID_PID;
        }
    } else{

		/*  in the middle(split into 2 find buffers and 1 buffer_node)
		 *    find_1  bufffer_node(start_pa, length) find_2
		 *  |--------|-----------------------------|--------|
		 *
		 *  buffer_node is part of find, at the middle of find
		 */

		struct cma_buffer *buffer_node1 = NULL;
        unsigned long total_len = find->length;

		// adjust find_1 OK
        find->length = start_pa - find->start_pa;

		// adjust bufffer_node
        buffer_node = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
        MCMA_BUG_ON(!buffer_node);
        buffer_node->start_pa = find->start_pa + find->length;
        buffer_node->length = length;
        buffer_node->page = phys_to_page(buffer_node->start_pa);
		buffer_node->cpu_addr = find->cpu_addr + find->length;
        INIT_LIST_HEAD(&buffer_node->list);
        list_add(&buffer_node->list, &find->list);

		// adjust find_2
        buffer_node1 = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
        MCMA_BUG_ON(!buffer_node1);
        buffer_node1->start_pa = buffer_node->start_pa + buffer_node->length;
        buffer_node1->length = total_len - buffer_node->length - find->length;
        buffer_node1->page = phys_to_page(buffer_node1->start_pa);
        buffer_node1->cpu_addr = buffer_node->cpu_addr + buffer_node->length;
        INIT_LIST_HEAD(&buffer_node1->list);
        list_add(&buffer_node1->list, &buffer_node->list);

        if(ops == CMA_ALLOC)
        {
            buffer_node->freed = 0;
            alloc_list->using_count++;
            buffer_node1->freed = 1;
            alloc_list->freed_count++;
        }
        else
        {
            alloc_list->freed_count++;
            buffer_node1->freed = 0;
            alloc_list->using_count++;

            buffer_node->freed = 1;
            buffer_node->cpu_addr = 0;
            buffer_node->pid = INVALID_PID;
        }
    }

    return buffer_node;
}

// before call this API, lock heap info mutex firstly
static struct cma_buffer *_alloc_from_freelist(struct cma_memory_info * heap_info, unsigned long start_pa,
size_t length, unsigned int flags)
{
    struct cma_buffer *buffer_node, *find = NULL;
    struct cma_allocation_list * alloc_list = &heap_info->alloc_list;


    if(alloc_list->freed_count <= 0)
        return NULL;

    // find the start address in free list
    list_for_each_entry(buffer_node, &alloc_list->list_head, list)
    {
        // not freed buffer
        if(buffer_node->freed == 0)
            continue;

		// check if this allocation_range is located at this freed buffer
        if((buffer_node->start_pa <= start_pa)
            && ((start_pa - buffer_node->start_pa + length) <= buffer_node->length))
        {
            find = buffer_node;
            break;
        }
    }

	// if find, split the free buffer to "ready_to_be_allocated buffer" and "free_buffer"
    if(!find)
        buffer_node = NULL;
    else
        buffer_node = SplitBuffer(start_pa, length, find, heap_info, CMA_ALLOC);

    return buffer_node;
}

static void dumpKRange(struct cma_memory_info * heap_info)
{
#if CMA_POOL_KERNEL_PROTECT_DUMP
	printk(KERN_EMERG "\033[35mFunction = %s, we do not support this\033[m\n", __PRETTY_FUNCTION__);
#endif
}

/* when alloc from cma heap, call this API to deleteKRange of this allocted buffer */
static int deleteKRange(struct cma_memory_info *heap_info, struct cma_buffer *buffer)
{
    int ret = 0;

    if(buffer->length == 0)
        return 0;

	printk(KERN_EMERG "\033[35mFunction = %s, Line = %d, delete protect from 0x%llX to 0x%llX\033[m\n",
		__PRETTY_FUNCTION__, __LINE__, (u64)buffer->start_pa - mtkcma_cpu_bus_base, (u64)buffer->start_pa - mtkcma_cpu_bus_base + (u64)buffer->length);
	ret = mtk_miup_req_kprot((u64)buffer->start_pa - mtkcma_cpu_bus_base, (u64)buffer->length);

    return ret;
}

/* when free to cma heap, call this API to add KRange of this allocted buffer */
static int addKRange(struct cma_memory_info *heap_info, struct cma_buffer *buffer)
{
    int ret = 0;

    if(buffer->length == 0)
        return 0;

	printk(KERN_EMERG "\033[35mFunction = %s, Line = %d, add protect from 0x%llX to 0x%llX\033[m\n",
		__PRETTY_FUNCTION__, __LINE__, (u64)buffer->start_pa - mtkcma_cpu_bus_base, (u64)buffer->start_pa - mtkcma_cpu_bus_base + (u64)buffer->length);
	ret = mtk_miup_rel_kprot((u64)buffer->start_pa - mtkcma_cpu_bus_base, (u64)buffer->length);

	return ret;
}

#ifdef CONFIG_MP_CMA_PATCH_COUNT_TIMECOST
extern signed long long Show_Diff_Time(char *caller, ktime_t start_time, bool print);
#endif

static int CMA_Pool_Alloc(unsigned int heap_id, unsigned long offset_in_heap, unsigned long *cpu_addr,
    unsigned long length, unsigned long align, unsigned int flags, filp_private *pdev)
{
    int ret = 0, try_num = 0;
    bool alloc_at_front = false;
    struct page *page = NULL;
    unsigned long phy_addr = 0, start_pa = 0;
    struct cma_memory_info * heap_info = NULL;
    struct cma_buffer *buffer = NULL, *buffer_hole = NULL;
    unsigned long length_buffer_hole = 0;
    bool find_in_freelist = false;
    struct cma * cma_area = NULL;
    struct heap_proc_info * proc_info = NULL;
	struct kernel_cma_pool_vma_item *kernel_cma_pool_vma_item_get = NULL;
	signed long timeout;
#ifdef CONFIG_MP_CMA_PATCH_DELAY_FREE
	struct list_head *pos, *q;
	struct delay_free_reserved *tmp;
#endif
	struct pid *pid_now;
	phys_addr_t flush_base, flush_end;
	size_t flush_size;
	struct page *flush_page;
	unsigned long flush_pfn;
	void *flush_ptr;
	unsigned long va = 0;

    if(!IS_ALIGNED(offset_in_heap, SIZE_8K) || !IS_ALIGNED(length, SIZE_8K))
    {
		printk(CMA_ERR "%s, input parameter not align to 0x%xbytes, offset_in_heap %lx, length %lx, heap_id=%x\n",
			__FUNCTION__, SIZE_8K, offset_in_heap, length, heap_id);
       return -EINVAL;
    }

    proc_info = find_heap_proc_info(heap_id, pdev);
    if(!proc_info)
    {
        MCMA_CUST_ERR("has not find heap_id %u its heap proc info\n", heap_id);
        return -EINVAL;
    }

    if(((flags &CMA_FLAG_MAP_KERNEL) && (TYPE_STATES_USER  == proc_info->type_states))
    ||((! (flags &CMA_FLAG_MAP_KERNEL)) && (TYPE_STATES_KERNEL == proc_info->type_states)))
    {
        printk(CMA_ERR "\033[35mFunction = %s, Line = %d, want to use proc_info simultaneously in kernel mode and user mode,flags=%u   ,proc_info->type_states=%d\033[m\n", __PRETTY_FUNCTION__, __LINE__,flags,proc_info->type_states);
        MCMA_BUG_ON(1);
    }

    heap_info = find_heap_info(heap_id);
    if(!heap_info)
    {
        MCMA_CUST_ERR("not find the heap: heap id %u\n", heap_id);
        return -EINVAL;
    }

	/* if cma_pool_alloc needs a va, it must keep whole va of this cma_buffer
	 * when cma_pool_init
	 */
    if(!proc_info->vma && (flags & CMA_FLAG_VIRT_ADDR)
        && (TYPE_STATES_USER == proc_info->type_states))
    {
        MCMA_CUST_ERR("invalid parameter: no vma but need virtual address\n");
        return -EINVAL;
    }

	/* if cma_pool_alloc needs a va, it must keep whole va of this cma_buffer
	 * when cma_pool_init
	 */
    if((TYPE_STATES_KERNEL == proc_info->type_states)&& (!proc_info->kernel_vm) && (flags & CMA_FLAG_VIRT_ADDR))
    {
        MCMA_CUST_ERR("invalid parameter: no kernel_vm but need virtual address\n");
        return -EINVAL;
    }

#if CMA_POOL_DUMP
    printk(CMA_ERR "%s:%d heapid %u, flags = %u, [%d, %d, name = %s] offset_in_heap = 0x%lx, length = 0x%lx\n",
		__FUNCTION__, __LINE__, heap_id, flags, current->pid, current->tgid, current->comm, offset_in_heap, length);
#endif

    cma_area = heap_info->dev->cma_area;
    start_pa = offset_in_heap + proc_info->base_phy;

#ifdef CONFIG_MP_CMA_PATCH_DELAY_FREE
	//delay free flow
	mutex_lock(&delay_free_lock);
	list_for_each_safe(pos, q, &delay_list){
		tmp = list_entry(pos, struct delay_free_reserved, list);
		printk(CMA_DEBUG "\033[0;32;31m [Delay Free] %s %d heap_id %d\033[m\n",__func__,__LINE__,tmp->delay_free_heap_id);
		if(tmp->delay_free_heap_id == heap_id){
			struct cma_buffer *find = NULL;
			mutex_lock(&heap_info->lock);
			find = find_cma_buffer(heap_info, start_pa, length);
			mutex_unlock(&heap_info->lock);
			if(tmp->delay_free_offset_in_heap == offset_in_heap && tmp->delay_free_length == length &&
					find && find->pid == task_pid_nr(current->group_leader)){
				printk(CMA_DEBUG "\033[0;32;31m [Delay Free] %s %d delay_free_Direct return heap_id %d\033[m\n",__func__,__LINE__,tmp->delay_free_heap_id);
				printk(CMA_ERR "From Delay Free\n");
				cpu_addr = find->cpu_addr;
				pdev = tmp->delay_free_pdev;
				list_del(pos);
				kfree(tmp);
				mutex_unlock(&delay_free_lock);
				return 0;
			}
			else{
				ret = CMA_Pool_Free(tmp->delay_free_heap_id,tmp->delay_free_offset_in_heap,
											tmp->delay_free_length,tmp->delay_free_pdev,false);
				if(ret){
					MCMA_BUG_ON(1);
				}
				printk(CMA_DEBUG "\033[0;32;31m [Delay Free] %s %d delay_free_heap_id %d\033[m\n",__func__,__LINE__,tmp->delay_free_heap_id);
				list_del(pos);
				kfree(tmp);
				break;
			}
		}
	}
	mutex_unlock(&delay_free_lock);
#endif

    mutex_lock(&heap_info->lock);

    // first, find in free list (buffer means a cma_alloc_result => from allocation_start to allocation_end, and mark if freed or not)
    if(heap_info->alloc_list.freed_count > 0)
    {
		buffer = _alloc_from_freelist(heap_info, start_pa, length, flags);
    }

    // if not find in free list, get from cma heap directly
    if(!buffer)
    {
        phy_addr = start_pa;

		// check if input parameters out of cma heap range
        if(((phy_addr + length) > PFN_PHYS(cma_area->base_pfn + cma_area->count))
            || (phy_addr < PFN_PHYS(cma_area->base_pfn)))
        {
			ret = -EINVAL;
            MCMA_CUST_ERR("invalid start address or length:  heap id %u, offset_in_heap 0x%lX ,max end 0x%lX, base_phy 0x%lX\n",
                           heap_id, offset_in_heap, heap_info->alloc_list.max_end, proc_info->base_phy);
            goto ALLOCAT_BUFF_FAIL;
        }

		/* if not find in freelist by _alloc_from_freelist(),
		 * this allocation buffer [start, length] should not locate in the middle of allocation_list
		 */
        if (heap_info->alloc_list.min_start != PHYSICAL_START_INIT)
        {
            if (((phy_addr >= heap_info->alloc_list.min_start) && (phy_addr < heap_info->alloc_list.max_end))
                || (((phy_addr + length) > heap_info->alloc_list.min_start) && ((phy_addr + length) <= heap_info->alloc_list.max_end)))
            {
                ret = -EINVAL;
				pid_now = find_get_pid(pdev->pid);
                MCMA_CUST_ERR("invalid start address or length: heap id %u,	min_start 0x%lX, max_end 0x%lX, phy_addr 0x%lX, length 0x%lX by %s\n\n",
							heap_id, heap_info->alloc_list.min_start, heap_info->alloc_list.max_end,
							phy_addr, length, pid_task(pid_now, PIDTYPE_PID)->comm);
                MCMA_CUST_ERR("Alloc fail!!!\n");
                MCMA_CUST_ERR("case1: Notice buffer may be a co-buffer, and may have already been allocated by others. Do not allocate a buffer simultaneously by different modules though they may co-buffer.\n");
                MCMA_CUST_ERR("case2: may have already been allocated, but not free, or previous free fail by different tgid, but here allocate again.\n");
                MCMA_CUST_ERR("Please refer to following log to get more info:\n");
				put_pid(pid_now);
                dump_CMA_Pool(heap_info, true);
                goto ALLOCAT_BUFF_FAIL;
            }

            if(phy_addr < heap_info->alloc_list.min_start)
                alloc_at_front = true;

            if ((phy_addr + length) < heap_info->alloc_list.min_start)
            {
				/*
				 *     case_1: front allocation, and cause a front hole
				 *             so, we also need to allcate this front hole
				 *             for covering our miu_protect limitation
				 *
				 *        new buffer        hole    min_start             max_end
				 *     |xxxxxxxxxxxxxx|-------------|============================|
				 */

				// we need to adjust allocation length to cover the hole
                length_buffer_hole = heap_info->alloc_list.min_start - (phy_addr + length);
                length = heap_info->alloc_list.min_start - phy_addr;

                buffer_hole = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
                MCMA_BUG_ON(!buffer_hole);
                INIT_LIST_HEAD(&buffer_hole->list);

				/* we allocate both "new buffer" and "hole"
				 * and set this addition cma_buffer(hole) to be freed
				 */
                buffer_hole->freed = 1;
                buffer_hole->start_pa = phy_addr + length - length_buffer_hole;
                buffer_hole->length = length_buffer_hole;
            }
            else if(phy_addr > heap_info->alloc_list.max_end)
            {
				/*
				 *     case_2: tail allocation, and cause a tail hole
				 *             so, we also need to allcate this tail hole
				 *             for covering our miu_protect limitation
				 *
				 *     min_start       max_end       hole        new buffer
				 *     |======================|---------------|xxxxxxxxxxxxxx|
				 */

				// we need to adjust allocation length to cover the hole
                length_buffer_hole = phy_addr - heap_info->alloc_list.max_end;
                length += length_buffer_hole;
                phy_addr = heap_info->alloc_list.max_end;

                buffer_hole = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
                MCMA_BUG_ON(!buffer_hole);
                INIT_LIST_HEAD(&buffer_hole->list);

                buffer_hole->freed = 1;
                buffer_hole->start_pa = heap_info->alloc_list.max_end;
                buffer_hole->length = length_buffer_hole;
            }
        }

ALLOC_RETRY:
		// here, the phy_addr and length is adjusted to cover "hole"
		//printk(KERN_EMERG "\033[35mFunction = %s, Line = %d, go allocate 0x%lX to 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, phy_addr, phy_addr + length);
		page = dma_alloc_at_from_contiguous(dev_get_cma_area(heap_info->dev),
			length >> PAGE_SHIFT, 0, phy_addr);
		if(page)
		{
#ifdef CONFIG_ARM64
			__dma_flush_area(page_address(page), length);
#else
			if (PageHighMem(page))
			{
				flush_pfn = page_to_pfn(page);
				flush_size = length;

				flush_base = flush_pfn << PAGE_SHIFT;
				flush_end = flush_base + flush_size;

				while (flush_size > 0) {
					flush_page = pfn_to_page(flush_pfn);
					flush_ptr = kmap_atomic(flush_page);
					if(!flush_ptr)
					{
						printk(KERN_EMERG "\033[35mFunction = %s, page phy is 0x%lX\033[m\n", __PRETTY_FUNCTION__, page_to_phys(page));
						printk(KERN_EMERG "\033[35mcheck from 0x%lX to ox%lX\033[m\n", flush_base, flush_end);
						printk(KERN_EMERG "\033[35mflush_phys is 0x%lX, no flush_ptr\033[m\n", page_to_phys(flush_page));
						continue;
					}
					dmac_flush_range(flush_ptr, flush_ptr + PAGE_SIZE);
					kunmap_atomic(flush_ptr);
					flush_pfn++;
					flush_size -= PAGE_SIZE;
				}
				outer_flush_range(flush_base, flush_end);
			}
			else
			{
				dmac_flush_range(lowmem_page_address(page), lowmem_page_address(page) + length);
				outer_flush_range(page_to_phys(page), page_to_phys(page) + length);
			}
#endif
		}

#ifdef CONFIG_MP_CMA_PATCH_COUNT_TIMECOST
		mutex_lock(&cma_measurement_data[heap_id].cma_measurement_lock);
		cma_measurement_data[heap_id].total_alloc_size_kb += length >> 10;
		mutex_unlock(&cma_measurement_data[heap_id].cma_measurement_lock);
#endif
        if(!page)
        {
            if(try_num < MAX_ALLOC_TRY)
            {
                try_num++;
                drop_slab();
                timeout = schedule_timeout_interruptible(msecs_to_jiffies(1000));
                goto ALLOC_RETRY;
            }

            MCMA_CUST_ERR("Fail to allocate buffer\n");
			printk(CMA_ERR "\033[35mFunction = %s, Error, Please Check WHY dma_alloc_at_from_contiguous failed (maybe no memory left)!! \033[m\n", __PRETTY_FUNCTION__);
            ret = -ENOMEM;
            goto ALLOCAT_BUFF_FAIL;
        }

		phy_addr = (dma_addr_t)__pfn_to_phys(page_to_pfn(page));
        buffer = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
        MCMA_BUG_ON(!buffer);
        buffer->start_pa = phy_addr;
        buffer->length = length;
        buffer->page = page;
        INIT_LIST_HEAD(&buffer->list);
        buffer->freed = 0;

        // insert the buffer node into alloc_list in order
        if(alloc_at_front)
            list_add(&buffer->list, &heap_info->alloc_list.list_head);
        else
            list_add_tail(&buffer->list, &heap_info->alloc_list.list_head);
        heap_info->alloc_list.using_count++;
    }
    else
    {
        find_in_freelist = true;
    }

    buffer->pid = task_pid_nr(current->group_leader);

    // if find in freelist, alloc_list max_end and min_start does't need to be changed
    if(!find_in_freelist)
    {
		// here, we will update max_end and min_start addr
        if(buffer->start_pa < heap_info->alloc_list.min_start)
            heap_info->alloc_list.min_start = buffer->start_pa;
        if((buffer->start_pa + buffer->length) > heap_info->alloc_list.max_end)
            heap_info->alloc_list.max_end = buffer->start_pa + buffer->length;
    }

	if(!find_in_freelist)
	{
		// this buffer will contain hole_buffer(if hole_buffer exists)
        ret = deleteKRange(heap_info, buffer);
        if(ret)
        {
            MCMA_CUST_ERR("update kernal range fail\n");
            MCMA_CUST_ERR("update kernal range fail\n");
            MCMA_CUST_ERR("update kernal range fail\n");
            MCMA_CUST_ERR("update kernal range fail\n");
            MCMA_CUST_ERR("update kernal range fail\n");
            MCMA_CUST_ERR("update kernal range fail\n");
            MCMA_CUST_ERR("update kernal range fail\n");
            MCMA_CUST_ERR("update kernal range fail\n");
            goto ALLOCAT_BUFF_FAIL;
        }

		/*
		 * If we have hole during this allocation, we will also allocate this hole,
		 * but we do not mapping this hole(CMA_Pool_Mapping).
		 */
        if(length_buffer_hole > 0)
        {
            if (alloc_at_front)
            {
				/* adjust the real addr and length for buffer and buffer_hole */
                buffer->length -= length_buffer_hole;
                buffer_hole->page = pfn_to_page(page_to_pfn(buffer->page) + (buffer->length >> PAGE_SHIFT));
                list_add(&buffer_hole->list, &buffer->list);

				heap_info->alloc_list.freed_count++;
            }
            else
            {
				/* adjust the real addr and length for buffer and buffer_hole */
                buffer_hole->page = buffer->page;

                buffer->start_pa += buffer_hole->length;
				buffer->page = pfn_to_page(page_to_pfn(buffer->page) + (buffer_hole->length >> PAGE_SHIFT));
                buffer->length -= buffer_hole->length;
                list_add(&buffer_hole->list, buffer->list.prev);

				heap_info->alloc_list.freed_count++;
            }
        }
    }
ALLOCAT_BUFF_FAIL:
	if (proc_info->type_states == TYPE_STATES_KERNEL) {
		if(!ret) {
			mutex_lock(&proc_info->vma_list_lock);
			list_for_each_entry(kernel_cma_pool_vma_item_get, &proc_info->vma_list, list_node) {
				/*
				 * Once user mode has called mmap(CMA_Pool_MMAP), it will insert vma_info
				 * into kernel_fd's proc_info->vma_list.
				 *
				 * If user space has called mmap, while kernel space do allocation,
				 * here will do real mapping(CMA_Pool_Mapping) for user space automatically.
				 *
				 * patched user space mapping, allocator is kernel space driver,
				 * but doing user space mapping
				 */
				if(kernel_cma_pool_vma_item_get->vma) {
					unsigned long kernel_mode_map_user_space_va  = 0;

					// kernel_cma_pool_vma_item_get is a user mode vma info
					kernel_mode_map_user_space_va = CMA_Pool_Mapping(proc_info, kernel_cma_pool_vma_item_get->vma, buffer->start_pa, buffer->length, pdev);
					MCMA_CUST_DEBUG("kernel_mode_map_user_space_va: 0x%lx, buffer->length: 0x%lx, 0x%lx~0x%lx\n",
						kernel_mode_map_user_space_va, buffer->length, kernel_mode_map_user_space_va, kernel_mode_map_user_space_va + buffer->length);
					MCMA_CUST_DEBUG("proc_info->type_states == TYPE_STATES_KERNEL: %d, proc_info->type_states == TYPE_STATES_USER: %d\n",
						!!(proc_info->type_states == TYPE_STATES_KERNEL), !!(proc_info->type_states == TYPE_STATES_USER));
					MCMA_CUST_DEBUG("flags & CMA_FLAG_VIRT_ADDR=%d\n", !!(flags & CMA_FLAG_VIRT_ADDR));
				}
			}
		    mutex_unlock(&proc_info->vma_list_lock);
		}
	}

	/* CMA_FLAG_VIRT_ADDR, which means this allocation
	 * will need a va mapping result for user/kernel space
	 */
    if (!ret && (flags & CMA_FLAG_VIRT_ADDR)) {
        if(TYPE_STATES_KERNEL == proc_info->type_states) {
			// pure kernel space mapping, allocator is kernel space driver
			printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, (kernel mode) mapping from 0x%lX to 0x%lX\033[m\n",
				__PRETTY_FUNCTION__, __LINE__, buffer->start_pa, (buffer->start_pa + buffer->length));
			if(proc_info->kernel_vm) {
				map_kernel(proc_info->kernel_vm, proc_info, offset_in_heap, buffer->length);
				va = (unsigned long)(proc_info->kernel_vm->addr) + (buffer->start_pa - proc_info->base_phy);

				MCMA_CUST_DEBUG("va: 0x%lx, length: 0x%lx, 0x%lx~0x%lx\n", va, length, va, va + length);
				MCMA_CUST_DEBUG("proc_info->type_states == TYPE_STATES_KERNEL: %d, proc_info->type_states == TYPE_STATES_USER: %d\n",
					!!(proc_info->type_states == TYPE_STATES_KERNEL),!!(proc_info->type_states == TYPE_STATES_USER));
				MCMA_CUST_DEBUG("flags & CMA_FLAG_VIRT_ADDR = %d\n", !!(flags & CMA_FLAG_VIRT_ADDR));
			}

			if(va <= 0) {
				MCMA_CUST_ERR("map_kernel fail: start_pa %lu, length %lu, flags=%u\n",
					buffer->start_pa, buffer->length, flags);
				MCMA_CUST_ERR("proc_info->type_states == TYPE_STATES_KERNEL: %d, proc_info->type_states == TYPE_STATES_USER: %d\n",
					!!(proc_info->type_states == TYPE_STATES_KERNEL),!!(proc_info->type_states == TYPE_STATES_USER));
				MCMA_CUST_ERR("flags & CMA_FLAG_VIRT_ADDR = %d\n", !!(flags & CMA_FLAG_VIRT_ADDR));
				if((TYPE_STATES_KERNEL == proc_info->type_states) && (!proc_info->kernel_vm))
					MCMA_CUST_ERR("proc_info->kernel_vm is NULL\n");

				mutex_unlock(&heap_info->lock);
				return ret;
			}
		} else {
			// pure user space mapping, allocator is user space driver
			printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, (usr mode) mapping from 0x%lX to 0x%lX\033[m\n",
				__PRETTY_FUNCTION__, __LINE__, buffer->start_pa, (buffer->start_pa + buffer->length));
			va = CMA_Pool_Mapping(proc_info, proc_info->vma, buffer->start_pa, buffer->length, pdev);

			MCMA_CUST_DEBUG("va: 0x%lx, buffer->length: 0x%lx, 0x%lx~0x%lx\n", va, buffer->length, va, va + buffer->length);
			MCMA_CUST_DEBUG("proc_info->type_states == TYPE_STATES_KERNEL: %d, proc_info->type_states == TYPE_STATES_USER: %d\n",
				!!(proc_info->type_states == TYPE_STATES_KERNEL), !!(proc_info->type_states == TYPE_STATES_USER));
			MCMA_CUST_DEBUG("flags & CMA_FLAG_VIRT_ADDR = %d\n", !!(flags & CMA_FLAG_VIRT_ADDR));

			if(va <= 0) {
				MCMA_CUST_ERR("CMA_Pool_Mapping fail: start_pa %lu, length %lu, flags=%u\n",
					buffer->start_pa, buffer->length, flags);
				MCMA_CUST_ERR("proc_info->type_states == TYPE_STATES_KERNEL: %d, proc_info->type_states == TYPE_STATES_USER: %d\n",
					!!(proc_info->type_states == TYPE_STATES_KERNEL),!!(proc_info->type_states == TYPE_STATES_USER));
				MCMA_CUST_ERR("flags & CMA_FLAG_VIRT_ADDR = %d\n", !!(flags & CMA_FLAG_VIRT_ADDR));
				if( (TYPE_STATES_USER == proc_info->type_states ) && (!proc_info->vma))
					MCMA_CUST_ERR("proc_info->vma is NULL\n");
				mutex_unlock(&heap_info->lock);
				return ret;
			}
		}

        *cpu_addr = va;
        buffer->cpu_addr = (void *)va;
        buffer->mapped = 1;
    }

	dump_CMA_Pool(heap_info, true);
    dumpKRange(heap_info);
    mutex_unlock(&heap_info->lock);

    return ret;
}

//before call this API, lock heap info mutex firstly
static void free_buffer_list(struct cma_memory_info *heap_info,
	struct cma_buffer **release_buf_front, struct cma_buffer **release_buf_back)
{
    struct cma_allocation_list *buffer_list = &heap_info->alloc_list;
    struct cma_buffer *buffer = NULL, *next = NULL;
    struct cma_buffer *front = NULL, *back = NULL;
    bool min_max_change = false;

    *release_buf_front = *release_buf_back = NULL;
    if(list_empty(&buffer_list->list_head))
        return;

	// from this heap_info, search every buffer
	list_for_each_entry_safe(buffer, next, &buffer_list->list_head, list) {
		// in current case, buffers must be neighbor, so, merge contiguous freed buffers
		if((buffer->freed == 1) && (next->freed == 1)) {
			if(buffer->start_pa + buffer->length == next->start_pa) {
				printk(CMA_DEBUG "\033[35mFunction = %s, buffer_start: 0x%lX, length: 0x%lX\033[m\n",
					__PRETTY_FUNCTION__, __LINE__, buffer->start_pa, buffer->length);
				printk(CMA_DEBUG "\033[35mFunction = %s, next buffer_start: 0x%lX, length: 0x%lX\033[m\n",
					__PRETTY_FUNCTION__, __LINE__, next->start_pa, next->length);
				printk(CMA_DEBUG "\033[35mFunction = %s, merge it\033[m\n", __PRETTY_FUNCTION__);

				list_del(&buffer->list);
				next->start_pa = buffer->start_pa;
				next->page = buffer->page;
				next->length += buffer->length;
				buffer_list->freed_count--;
				kfree(buffer);
			}
		}
	}

    /* case A: freed buffer locate at beginning of alloc list */
    if(list_empty(&buffer_list->list_head))
        goto UPDATE_MIN_MAX;

	/*
	 * check if 1st buffer is free?
	 * we can only free 1st or last_one buffer because kernel protect limitation
	 */
    front = list_entry(heap_info->alloc_list.list_head.next, struct cma_buffer, list);
    if(front->freed == 1)
    {
		printk(CMA_DEBUG "\033[35mFunction = %s, free front\033[m\n", __PRETTY_FUNCTION__);
        list_del(&front->list);
        buffer_list->freed_count--;
        min_max_change = true;
        *release_buf_front = front;
    }

    /*case B: freed buffer locate at end of kernel range*/
    if(list_empty(&buffer_list->list_head))
        goto UPDATE_MIN_MAX;

	/*
	 * check if last_one buffer is free?
	 * we can only free 1st or last_one buffer because kernel protect limitation
	 */
    back = list_entry(heap_info->alloc_list.list_head.prev, struct cma_buffer, list);
    if(back->freed == 1)
    {
		printk(CMA_DEBUG "\033[35mFunction = %s, free back\033[m\n", __PRETTY_FUNCTION__);
        list_del(&back->list);
        buffer_list->freed_count--;
        min_max_change = true;
        *release_buf_back = back;
    }

UPDATE_MIN_MAX:
    if(list_empty(&buffer_list->list_head))
    {
		printk(CMA_DEBUG "\033[35mFunction = %s, list is empty\033[m\n", __PRETTY_FUNCTION__);
        buffer_list->min_start = PHYSICAL_START_INIT;
        buffer_list->max_end = PHYSICAL_END_INIT;
    }
    else if(min_max_change)
    {
		printk(CMA_DEBUG "\033[35mFunction = %s, update min/max\033[m\n", __PRETTY_FUNCTION__);
        buffer = list_entry(heap_info->alloc_list.list_head.next, struct cma_buffer, list);
        buffer_list->min_start = buffer->start_pa;
        buffer = list_entry(heap_info->alloc_list.list_head.prev, struct cma_buffer, list);
        buffer_list->max_end = buffer->start_pa + buffer->length;
    }
}

//before call this API, lock heap info mutex firstly
static void free_buffer_list_and_release_special_buf(struct cma_memory_info *heap_info, struct cma_buffer **release_buf_front , struct cma_buffer **release_buf_back)
{
	int ret = 0;
	struct cma_buffer *front = NULL, *back = NULL;

	if (!heap_info->dev->cma_area) {
		printk(KERN_EMERG "\033[35mFunction = %s, error, no cma_area\033[m\n", __PRETTY_FUNCTION__);
		return;
	}
		
	free_buffer_list(heap_info, release_buf_front, release_buf_back);
	if (*release_buf_front)
		front = (*release_buf_front);
	if (*release_buf_back)
		back = (*release_buf_back);

	if (front) {
		ret = addKRange(heap_info, front);
		printk(KERN_EMERG "\033[35mcma_release 0x%lX to 0x%lX\033[m\n",
			page_to_phys(front->page), (page_to_phys(front->page) + front->length));
		cma_release(heap_info->dev->cma_area,
			front->page, front->length >> PAGE_SHIFT);
		kfree(front);
	}

	if(back) {
		ret = addKRange(heap_info, back);
		printk(KERN_EMERG "\033[35mcma_release 0x%lX to 0x%lX\033[m\n",
			page_to_phys(front->page), (page_to_phys(front->page) + front->length));
		cma_release(heap_info->dev->cma_area,
			back->page, back->length >> PAGE_SHIFT);
		kfree(back);
	}
}

//Mutex lock outside
static struct cma_buffer *find_cma_buffer(struct cma_memory_info * heap_info, unsigned long start_pa, unsigned long length)
{
    struct cma_allocation_list * alloc_list = &heap_info->alloc_list;
    struct cma_buffer * buffer = NULL, *find = NULL;

    if(list_empty(&alloc_list->list_head))
        goto FIND_CMA_BUFF_DONE;

    list_for_each_entry(buffer, &alloc_list->list_head, list)
    {
        if((start_pa >= buffer->start_pa) && ((start_pa + length) <= (buffer->start_pa + buffer->length)))
        {
            find = buffer;
            break;
        }
    }

FIND_CMA_BUFF_DONE:
    return find;
}

static int CMA_Pool_Free(unsigned int heap_id, unsigned long offset_in_heap, unsigned long length, filp_private *pdev, bool delay_free)
{
    int ret = 0;
    struct cma_memory_info *heap_info = NULL;
    struct cma_buffer *release_buf_front = NULL, *release_buf_back = NULL, *find = NULL;
    struct heap_proc_info *proc_info = NULL;
    unsigned long start_pa = 0;
	unsigned long start;
	unsigned long end;
	struct kernel_cma_pool_vma_item *kernel_cma_pool_vma_item_get = NULL;

#if CMA_POOL_DUMP
	printk(CMA_ERR "%s:%d heapid %u, [%d,%d,name = %s] offset_in_heap = 0x%lx, length = 0x%lx\n",
		__FUNCTION__, __LINE__, heap_id, current->pid, current->tgid, current->comm, offset_in_heap, length);
#endif

    if(!IS_ALIGNED(offset_in_heap, SIZE_8K) || !IS_ALIGNED(length, SIZE_8K))
    {
		printk(CMA_ERR "%s, input parameter not align to 0x%x bytes, offset_in_heap %lx, length %lx, heap_id = %x\n",
			__FUNCTION__, SIZE_8K,offset_in_heap, length,heap_id);
       return -EINVAL;
    }

#ifdef CONFIG_MP_CMA_PATCH_DELAY_FREE
	//delay free flow
	if(delay_free == true && !is_in_cma_force_free_period()){
		mutex_lock(&delay_free_lock);
		if(length >= delay_free_size_limite && delay_free_time_limite != 0){
			struct list_head *pos, *q;
			struct delay_free_reserved *tmp;
			list_for_each_safe(pos, q, &delay_list){
				tmp = list_entry(pos, struct delay_free_reserved, list);
				if(tmp->delay_free_heap_id == heap_id){
					printk(CMA_DEBUG "\033[0;32;31m [Delay Free] %s %d Releae Reserver delay_free_heap_id %d\033[m\n",__func__,__LINE__,tmp->delay_free_heap_id);
					ret = CMA_Pool_Free(tmp->delay_free_heap_id,tmp->delay_free_offset_in_heap,
											tmp->delay_free_length,tmp->delay_free_pdev,false);
					if(ret)
						MCMA_BUG_ON(1);
					tmp->delay_free_heap_id = heap_id;
					tmp->delay_free_offset_in_heap = offset_in_heap;
					tmp->delay_free_length = length;
					tmp->delay_free_pdev = pdev;
					tmp->delay_free_time_out = delay_free_time_limite;
					printk(CMA_DEBUG "\033[0;32;31m [Delay Free] %s %d Reserver heap_id %d\033[m\n",__func__,__LINE__,heap_id);
					mutex_unlock(&delay_free_lock);
					return 0;
				}
			}
			tmp = (struct delay_free_reserved *)kmalloc(sizeof(struct delay_free_reserved),GFP_KERNEL);
			tmp->delay_free_heap_id = heap_id;
			tmp->delay_free_offset_in_heap = offset_in_heap;
			tmp->delay_free_length = length;
			tmp->delay_free_pdev = pdev;
			tmp->delay_free_time_out = delay_free_time_limite;
			list_add(&tmp->list, &delay_list);
			printk(CMA_DEBUG "\033[0;32;31m [Delay Free] %s %d First Reserver heap_id %d\033[m\n",__func__,__LINE__,heap_id);
			list_for_each_safe(pos, q, &delay_list){
				tmp = list_entry(pos, struct delay_free_reserved, list);
				printk(CMA_DEBUG "\033[0;32;31m [Delay Free] %s %d Import %d\033[m\n",__func__,__LINE__,tmp->delay_free_heap_id);
			}

			mutex_unlock(&delay_free_lock);
			return 0;
		}
		mutex_unlock(&delay_free_lock);
	}
#endif

	heap_info = find_heap_info(heap_id);
    if(!heap_info)
	{
		printk(CMA_WARNING "\033[35mFunction = %s, Line = %d, can not find heap_info\033[m\n", __PRETTY_FUNCTION__, __LINE__);
        return -EINVAL;
	}

    proc_info = find_heap_proc_info(heap_id, pdev);
    if(!proc_info)
    {
        MCMA_CUST_ERR("has not find heap_id %u its heap proc info\n", heap_id);
        return -EINVAL;
    }

    mutex_lock(&heap_info->lock);

    start_pa = offset_in_heap + proc_info->base_phy;
    find = find_cma_buffer(heap_info, start_pa, length);
    if(!find)
    {
		printk(CMA_ERR "\033[35mFunction = %s, Line = %d, [Error] [%s] Strange CMA_Pool_Free\033[m\n",
			__PRETTY_FUNCTION__, __LINE__, current->comm);
		printk(CMA_ERR "\033[35mFunction = %s, Line = %d, heap_id %u want to release from 0x%lX to 0x%lX\033[m\n",
			__PRETTY_FUNCTION__, __LINE__, heap_id, start_pa, (start_pa+length));
		printk(CMA_ERR "\033[35mFunction = %s, Check this\033[m\n", __PRETTY_FUNCTION__);
		printk(CMA_ERR "\033[35mFunction = %s, Check this\033[m\n", __PRETTY_FUNCTION__);
		printk(CMA_ERR "\033[35mFunction = %s, Check this\033[m\n", __PRETTY_FUNCTION__);

		mutex_unlock(&heap_info->lock);
		MCMA_BUG_ON(1);
    }

    if(find->freed)
    {
        MCMA_CUST_WARNING("buffer already freed [offset %llX length %llX] heap_id %u\n",
			(u64)offset_in_heap, (u64)length, heap_id);
        mutex_unlock(&heap_info->lock);
        return -EFAULT;
    }

    if(delay_free && find->pid != task_pid_nr(current->group_leader))
    {
        MCMA_CUST_ERR("buffer pid %d current pid %d\n", find->pid, task_pid_nr(current->group_leader));
        mutex_unlock(&heap_info->lock);
        return -EPERM;
    }

    if(TYPE_STATES_KERNEL == proc_info->type_states)
    {
		/* as we realize kernel mode map user space va feature, here also need check proc_info->vma
		 * if kernel mode, kernel space not map, but user space mapped,
		 * should unmap user space va.
		 */
        ret = 0;
        mutex_lock(&proc_info->vma_list_lock);

		// only kernel mode fd has vma_list
		list_for_each_entry(kernel_cma_pool_vma_item_get, &proc_info->vma_list, list_node)
		{
			// for each vma_item,should all unmapping
            ret = CMA_Pool_Unmapping(proc_info, kernel_cma_pool_vma_item_get->vma, start_pa, length, pdev);

            if(ret)
            {
                MCMA_CUST_ERR("CMA_Pool_Unmapping fail: start_pa %llX, length %llX\n", (u64)start_pa, (u64)length);
                mutex_unlock(&proc_info->vma_list_lock);
                mutex_unlock(&heap_info->lock);
                return ret;
            }
		}
		mutex_unlock(&proc_info->vma_list_lock);
    }

    if(find->mapped)
    {
        if(TYPE_STATES_KERNEL == proc_info->type_states)
        {
            start = (unsigned long)(proc_info->kernel_vm->addr) + offset_in_heap;
            end = start + length;
            unmap_kernel_range(start, length);

            ret = 0;
        }
        else
            ret = CMA_Pool_Unmapping(proc_info, proc_info->vma, start_pa, length, pdev);

		if(ret)
        {
            MCMA_CUST_ERR("CMA_Pool_Unmapping fail: start_pa %llX, length %llX\n", (u64)start_pa, (u64)length);
            mutex_unlock(&heap_info->lock);
            return ret;
        }
    }

    MCMA_BUG_ON(heap_info->alloc_list.using_count == 0);

	SplitBuffer(start_pa, length, find, heap_info, CMA_FREE);
	free_buffer_list_and_release_special_buf(heap_info, &release_buf_front, &release_buf_back);

    dump_CMA_Pool(heap_info, true);
    mutex_unlock(&heap_info->lock);
    dumpKRange(heap_info);

    return ret;
}

static unsigned long CMA_Pool_Mapping(struct heap_proc_info *proc_info, struct vm_area_struct *vma, unsigned long start_pa, unsigned long length, filp_private *pdev)
{
    //struct vm_area_struct *vma = NULL;
    unsigned long vaddr = 0, pfn = 0, ret_addr;
    struct mm_struct *mm = NULL;
    int err = -EFAULT;

    if((start_pa & (PAGE_SIZE -1)) || (length & (PAGE_SIZE -1)))
    {
        MCMA_CUST_ERR("start_pa %lu, length %lu\n", start_pa, length);
        return -EINVAL;
    }

    if (!vma)
	{
        MCMA_CUST_ERR("No vma!!\n");
        return -EINVAL;
    }

    //vma = proc_info->vma;
    if(start_pa - proc_info->base_phy + length > (vma->vm_end - vma->vm_start))
    {
        MCMA_CUST_ERR("input length %lu, vma length %lu\n", start_pa - proc_info->base_phy + length,
            vma->vm_end - vma->vm_start);

        MCMA_CUST_ERR("start_pa %lx, base_phy %lx, length %lx, vm_start %lx vm_end %lx\n", start_pa,
            proc_info->base_phy, length, vma->vm_start, vma->vm_end);

        return -EINVAL;
    }

    vaddr = vma->vm_start + (start_pa - proc_info->base_phy);
    pfn = __phys_to_pfn(start_pa);
    ret_addr = vaddr;
    mm = vma->vm_mm;

    if (mm)
        down_write(&mm->mmap_sem);

    err = remap_pfn_range(vma, vaddr, pfn, length, vma->vm_page_prot);

    if (mm)
        up_write(&mm->mmap_sem);

	if (err)
		return err;
	else
		return ret_addr;
}

static int CMA_Pool_Unmapping(struct heap_proc_info *proc_info, struct vm_area_struct *vma,
	unsigned long start_pa, unsigned long length, filp_private *pdev)
{
    unsigned long vaddr = 0;
    struct mm_struct *mm = NULL;
    int err = 0;

    if((start_pa & (PAGE_SIZE - 1)) || (length & (PAGE_SIZE - 1)))
    {
        MCMA_CUST_ERR("start_pa %lu, length %lu\n", start_pa, length);
        return -EINVAL;
    }

    if (!vma)
	{
        MCMA_CUST_ERR("No vma!!\n");
        return -EINVAL;
    }

    if(start_pa - proc_info->base_phy + length > (vma->vm_end - vma->vm_start))
    {
        MCMA_CUST_ERR("input length %lu, vma length %lu\n",
			start_pa - proc_info->base_phy + length, vma->vm_end - vma->vm_start);
        return -EINVAL;
    }

    vaddr = vma->vm_start + (start_pa - proc_info->base_phy);

    mm = vma->vm_mm;
    if (mm)
        down_write(&mm->mmap_sem);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)
    zap_vma_ptes(vma, (uintptr_t)vaddr, length);
#else
    if (zap_vma_ptes(vma, (uintptr_t)vaddr, length)) {
        MCMA_CUST_ERR("!!! zap_vma_ptes failed start = 0x%lX, size = 0x%lX by pid:%d\n",
			vaddr, length, (int)current->pid);
        err = -EFAULT;
    }
#endif

    if (mm)
        up_write(&mm->mmap_sem);

    return err;

}

#ifdef CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#ifdef CONFIG_MP_NEW_UTOPIA_32BIT
static int CMA_Pool_Get_Info_From_Pa(u64 PA, unsigned int *heap_id, unsigned int *miu,
                                                                            u64 *heap_miu_start_offset,
                                                                            unsigned long *heap_length,u64* pa_offset_in_heap, filp_private *user_pdev)
#else
static int CMA_Pool_Get_Info_From_Pa(unsigned long PA, unsigned int *heap_id, unsigned int *miu,
											unsigned long *heap_miu_start_offset,
											unsigned long *heap_length,unsigned long *pa_offset_in_heap, filp_private *user_pdev)
#endif
#elif defined(CONFIG_ARM64)
static int CMA_Pool_Get_Info_From_Pa(u64 PA, u32 *heap_id, u32 *miu,
											u64 *heap_miu_start_offset,
											u64 *heap_length,u64 *pa_offset_in_heap, filp_private *user_pdev)
#endif
{
    filp_private *pdev = NULL;
    struct heap_proc_info * proc_info = NULL;
    struct cma_memory_info *heap_info = NULL;
    unsigned int tmp_heap_length;
    int ret = -1;
    unsigned long kernel_PA;
    unsigned int miu_t = -1;
    unsigned long heap_miu_start_offset_t = -1;

	/*
	 * for a given PA, use kernel space fd to get va
	 *
	 */

    MCMA_CUST_DEBUG("PA=0x%lx,sizeof(unsigned long)=%d\n",PA,sizeof(unsigned long));
    MCMA_CUST_DEBUG("[%d,%d,name=%s]\n", current->pid, current->tgid,current->comm);

    user_phy_to_kernel_va(PA,&kernel_PA);
    PA = kernel_PA;
    MCMA_CUST_DEBUG("after change PA=0x%lx\n",PA);
    //only get heap info from kernel space open file of "/dev/cmapool",
    //not from user space open file of "/dev/cmapool",
    if(!_fileCMAPool)
    {
        //default not enable this printk,for pa which  not in cma may also cause go to here,sometimes thouse case will printk too many times.
        //MCMA_CUST_ERR("kernel mode not open device node. \n");

        return -EFAULT;
    }

    pdev = _fileCMAPool->private_data;
    if(!pdev)
    {
        MCMA_CUST_ERR("device node do not have private_data.\n");
        return -EFAULT;
    }
    MCMA_CUST_DEBUG("PA=0x%lx\n",PA);
    mutex_lock(&pdev->lock);
    MCMA_CUST_DEBUG("PA=0x%lx\n",PA);
    list_for_each_entry(proc_info, &pdev->list_heap_proc_info, list_node)
    {
        MCMA_CUST_DEBUG("PA=0x%lx\n",PA);
        heap_info = find_heap_info(proc_info->heap_id);
        if(!heap_info)
        {
            MCMA_CUST_ERR("not find heap_info. \n");
            return -EFAULT;
        }

        tmp_heap_length = heap_info->dev->cma_area->count * PAGE_SIZE;
        MCMA_CUST_DEBUG("PA=0x%lx  heap[heap_id: %u  range: 0x%lx ~~~~~~0x%lx]\n",
            PA,proc_info->heap_id,proc_info->base_phy,proc_info->base_phy + tmp_heap_length);
        if((PA >= proc_info->base_phy)&&(PA < proc_info->base_phy + tmp_heap_length) )
        {
            *heap_id = proc_info->heap_id;
            *heap_length = tmp_heap_length;
            phy_to_MiuOffset(proc_info->base_phy, &miu_t, &heap_miu_start_offset_t);//use heap start pa to get miu and heap start in miu offset
            *miu = miu_t;
            *heap_miu_start_offset = heap_miu_start_offset_t;
            *pa_offset_in_heap = PA - proc_info->base_phy;//pa offset in heap

            //must set this value to user pdev,for later mmap will use it.
            user_pdev->temp_heap_id = *heap_id;

            ret = 0;
            MCMA_CUST_DEBUG("PA=0x%lx include in heap[heap_id: %u  range: 0x%lx ~~~0x%lx]\n",
               PA,proc_info->heap_id,proc_info->base_phy,proc_info->base_phy + tmp_heap_length);
            break;
        }
    }
    MCMA_CUST_DEBUG("PA=0x%lx ret=%d\n",PA,ret);
    mutex_unlock(&pdev->lock);
    return ret;
}

static int CMA_Pool_Kernel_Mode_Get_User_Va(unsigned int heap_id,unsigned int flags
                                                                 ,enum USER_VA_VALID_FLAG* user_va_valid_flag,unsigned long *heap_start_user_space_virt_addr
                                                                 , filp_private *user_pdev)
{
    filp_private *pdev = NULL;
    struct heap_proc_info * proc_info = NULL;
    int ret = -1;
	struct kernel_cma_pool_vma_item *kernel_cma_pool_vma_item_get = NULL;
	bool find_vma_pid = false;

    MCMA_CUST_DEBUG("heap_id=%u,flags=%u\n",heap_id,flags);
    MCMA_CUST_DEBUG("[%d,%d,name=%s]\n", current->pid, current->tgid,current->comm);

   //should use device node file open by kernel space,not use user space open device node file
    if(!_fileCMAPool)
    {
        MCMA_CUST_ERR("kernel mode not open device node. \n");
        return -EFAULT;
    }
    pdev  = _fileCMAPool->private_data;
    if(!pdev)
    {
        MCMA_CUST_ERR("device node do not have private_data.\n");
        return -EFAULT;
    }
    proc_info = find_heap_proc_info(heap_id, pdev);
    if(proc_info)
    {
        mutex_lock(&proc_info->vma_list_lock);
		kernel_cma_pool_vma_item_get = NULL;
		find_vma_pid = false;
		list_for_each_entry(kernel_cma_pool_vma_item_get, &proc_info->vma_list, list_node)
		{
			if(kernel_cma_pool_vma_item_get->pid == current->tgid)
			{
			    find_vma_pid = true;
			    break;
			}
		}

        if((false == find_vma_pid) || (!kernel_cma_pool_vma_item_get->vma))
        {
            *user_va_valid_flag = NO_USER_VA;
            MCMA_CUST_DEBUG("not have vma_item which means need mmap first, or NO_USER_VA\n");
            *heap_start_user_space_virt_addr = -1;
        }
        else if ( (0 == (kernel_cma_pool_vma_item_get->flags & CMA_FLAG_CACHEATTR_VALID))
			|| ((kernel_cma_pool_vma_item_get->flags & KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE) != (flags & KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE)) )
        {
            //if cache flag not valid,or valid but not match
            *user_va_valid_flag = DIFFERENT_CACHE_TYPE;
            MCMA_CUST_DEBUG("DIFFERENT_CACHE_TYPE \n");
            *heap_start_user_space_virt_addr = -1;
        }
        else
        {
            *user_va_valid_flag = USER_VA_VALID;
            MCMA_CUST_DEBUG("USER_VA_VALID heap_id=%d proc_info->flags=%u\n",heap_id,kernel_cma_pool_vma_item_get->flags);
            *heap_start_user_space_virt_addr = kernel_cma_pool_vma_item_get->vma->vm_start;//user space va of heap start
            MCMA_CUST_DEBUG("*heap_start_user_space_virt_addr=0x%lx\n",*heap_start_user_space_virt_addr);
        }
		mutex_unlock(&proc_info->vma_list_lock);

        ret = 0;
    }
    else
    {
        *user_va_valid_flag = NO_HEAP;
        MCMA_CUST_DEBUG("NO_HEAP \n");
    }

    if(ret)
    {
        MCMA_CUST_ERR("fail  :heap_id=%u,flags=%u.\n",heap_id,flags);
    }
    return ret;

}

static int CMA_Pool_Mmap_User_Va_Page(unsigned long heap_id,unsigned int flags, filp_private *user_pdev)
{
    filp_private *pdev = NULL;
    struct heap_proc_info * proc_info = NULL;
    int ret = -1;
    struct cma_memory_info * heap_info = NULL;
	struct kernel_cma_pool_vma_item *kernel_cma_pool_vma_item_get = NULL;
	bool find_vma_pid = false;
	struct cma_buffer *buffer_node = NULL;
	struct cma_allocation_list *alloc_list;

    MCMA_CUST_DEBUG("[%d,%d,name=%s]\n", current->pid, current->tgid,current->comm);

	//should use device node file open by kernel space,not use user space open device node file
	if(!_fileCMAPool)
	{
		MCMA_CUST_ERR("kernel mode not open device node. \n");
		return -EFAULT;
	}
	pdev  = _fileCMAPool->private_data;
	if(!pdev)
	{
		MCMA_CUST_ERR("device node do not have private_data.\n");
		return -EFAULT;
	}

    MCMA_CUST_DEBUG("\n");
    proc_info = find_heap_proc_info(heap_id, pdev);

	if(proc_info)
    {
        mutex_lock(&proc_info->vma_list_lock);
		list_for_each_entry(kernel_cma_pool_vma_item_get, &proc_info->vma_list, list_node)
		{
			if(kernel_cma_pool_vma_item_get->pid == current->tgid)
			{
			    find_vma_pid =true;
			    break;
			}
		}
	    if(false == find_vma_pid)
	    {
	        MCMA_CUST_ERR("should not goto here. \n");
			mutex_unlock(&proc_info->vma_list_lock);
			return -EFAULT;
	    }

	    if(true == find_vma_pid)
	    {
	        if(!kernel_cma_pool_vma_item_get->vma)
	        {
	            MCMA_CUST_ERR("before calling %s must already mmap in user space!!!.\n",__FUNCTION__);
	            BUG();
	        }
	        if(0 == (kernel_cma_pool_vma_item_get->flags &(CMA_FLAG_CACHEATTR_VALID)) )
	        {
	            MCMA_CUST_DEBUG("\n");
	            kernel_cma_pool_vma_item_get->flags |=(flags & KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE);
	            kernel_cma_pool_vma_item_get->flags |= CMA_FLAG_CACHEATTR_VALID;
	        }
	        else
	        {
	            MCMA_CUST_DEBUG("\n");
	            if((kernel_cma_pool_vma_item_get->flags &KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE)
	                !=(flags & KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE))
	            {
	                MCMA_CUST_ERR("kernel mode user space map cache flags been set : %d,now want %d.\n",
	                        !!(kernel_cma_pool_vma_item_get->flags &KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE),
	                        !!(flags & KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE));
				    mutex_unlock(&proc_info->vma_list_lock);
	                return -EFAULT;
	            }
	        }

	        if(!(flags & KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE))
	        {
	            MCMA_CUST_DEBUG("\n");
#if (defined(MIPS))
	            kernel_cma_pool_vma_item_get->vma->vm_page_prot.pgprot = (kernel_cma_pool_vma_item_get->vma->vm_page_prot.pgprot & ~_CACHE_MASK) | _CACHE_UNCACHED; /* MIPS's equivalents of 'pgprot_writecombine' */
#else
	            kernel_cma_pool_vma_item_get->vma->vm_page_prot = pgprot_writecombine(kernel_cma_pool_vma_item_get->vma->vm_page_prot);
#endif
	        }
	        MCMA_CUST_DEBUG("\n");
	        heap_info = find_heap_info(heap_id);
	        if(!heap_info)
	        {
	            MCMA_CUST_ERR("not find the heap: heap id %lu\n", heap_id);
				mutex_unlock(&proc_info->vma_list_lock);
	            return -EINVAL;
	        }
	        MCMA_CUST_DEBUG("\n");
			mutex_lock(&heap_info->lock);
	        alloc_list = &heap_info->alloc_list;

	        MCMA_CUST_DEBUG("\n");
	        if(alloc_list->using_count > 0)
	        {
	            MCMA_CUST_DEBUG("\n");
	            //find the start address in free list
	            list_for_each_entry(buffer_node, &alloc_list->list_head, list)
	            {
	                MCMA_CUST_DEBUG("\n");
	                //not freed buffer
	                if(buffer_node->freed == 0)
	                {
	                    MCMA_CUST_DEBUG("\n");
	                    //if already been allocated,mapping allocated area.
	                    mutex_lock(&pdev->lock);
	                    MCMA_CUST_DEBUG("buffer_node->start_pa=0x%lx,buffer_node->length=0x%lx\n",buffer_node->start_pa,buffer_node->length);
	                    CMA_Pool_Mapping(proc_info, kernel_cma_pool_vma_item_get->vma, buffer_node->start_pa, buffer_node->length, pdev);
	                    mutex_unlock(&pdev->lock);
	                }
	            }
	        }
			mutex_unlock(&heap_info->lock);
	        MCMA_CUST_DEBUG("\n");
	        ret = 0;
	    }
		mutex_unlock(&proc_info->vma_list_lock);
	}
    if(ret)
    {
        MCMA_CUST_ERR("do not find pos :heap_id = %lu.\n", heap_id);
    }
    return ret;

}
#endif

#if defined(CONFIG_COMPAT)
static long Compat_CMA_Pool_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long ret;

	if (!filp->f_op || !filp->f_op->unlocked_ioctl)
		return -ENOTTY;

    switch (cmd)
	{
		case COMPAT_CMA_POOL_IOC_INIT:
		{
			compat_uint_t uint;
			compat_ulong_t ulong;
			compat_u64 u64_data;
			compat_size_t usize;

			struct compat_cma_heap_info __user *data32;
			struct cma_heap_info __user *data;
			int	err = 0;

			data32 = compat_ptr(arg);						// use 32-bit variable type to point the data(compat is used for 32-bit app and 64-bit kernel, the arg is 32-bit!!
			data = compat_alloc_user_space(sizeof(*data));	// alloc a memory in user space
			if (data == NULL)
				return -EFAULT;

			/*
			 * copy 32_bit data to 64_bit data
			 * we only wanna change data_size for flags, but we still need to copy all data from data32(user data) to data
			 */
			err = get_user(uint, &data32->heap_id);
			err |= put_user(uint, &data->heap_id);

			err = get_user(ulong, &data32->flags);
			err |= put_user(ulong, &data->flags);

			err = get_user(uint, &data32->miu);
			err |= put_user(uint, &data->miu);

			err = get_user(u64_data, &data32->bus_addr);
			err |= put_user(u64_data, &data->bus_addr);

			err = get_user(u64_data, &data32->heap_miu_start_offset);
			err |= put_user(u64_data, &data->heap_miu_start_offset);

			err = get_user(usize, &data32->heap_length);
			err |= put_user(usize, &data->heap_length);
			if(err)
				return err;

			/* call the responding 64_bit ioctl */
			ret = filp->f_op->unlocked_ioctl(filp, CMA_POOL_IOC_INIT, (unsigned long)data);

			/*
			 * copy 64_bit data to 32_bit data for return to 32_bit app
			 */
			err = get_user(uint, &data->heap_id);
			err |= put_user(uint, &data32->heap_id);

			err = get_user(ulong, &data->flags);
			err |= put_user(ulong, &data32->flags);

			err = get_user(uint, &data->miu);
			err |= put_user(uint, &data32->miu);

			err = get_user(u64_data, &data->bus_addr);
			err |= put_user(u64_data, &data32->bus_addr);

			err = get_user(u64_data, &data->heap_miu_start_offset);
			err |= put_user(u64_data, &data32->heap_miu_start_offset);

			err = get_user(usize, &data->heap_length);
			err |= put_user(usize, &data32->heap_length);

			return ret ? ret : err;
		}
		case COMPAT_CMA_POOL_IOC_ALLOC: // alloc a buffer
		{
			compat_uint_t uint;
			compat_ulong_t ulong;
			compat_u64 u64_data;
			compat_size_t usize;

			struct compat_cma_alloc_args __user *data32;
			struct cma_alloc_args __user *data;
			int	err = 0;

			data32 = compat_ptr(arg);						// use 32-bit variable type to point the data(compat is used for 32-bit app and 64-bit kernel, the arg is 32-bit!!
			data = compat_alloc_user_space(sizeof(*data));	// alloc a memory in user space
			if (data == NULL)
				return -EFAULT;

			/*
			 * copy 32_bit data to 64_bit data
			 * we only wanna change data_size for flags, but we still need to copy all data from data32(user data) to data
			 */
			err = get_user(u64_data, &data32->offset_in_heap);
			err |= put_user(u64_data, &data->offset_in_heap);

			err = get_user(ulong, &data32->cpu_addr);
			err |= put_user(ulong, &data->cpu_addr);

			err = get_user(usize, &data32->length);
			err |= put_user(usize, &data->length);

			err = get_user(u64_data, &data32->align);
			err |= put_user(u64_data, &data->align);

			err = get_user(uint, &data32->heap_id);
			err |= put_user(uint, &data->heap_id);

			err = get_user(uint, &data32->flags);
			err |= put_user(uint, &data->flags);
			if(err)
				return err;

			/* call the responding 64_bit ioctl */
			ret = filp->f_op->unlocked_ioctl(filp, CMA_POOL_IOC_ALLOC, (unsigned long)data);
			/*
			 * copy 64_bit data to 32_bit data for return to 32_bit app
			 */
			err = get_user(u64_data, &data->offset_in_heap);
			err |= put_user(u64_data, &data32->offset_in_heap);

			err = get_user(ulong, &data->cpu_addr);
			err |= put_user(ulong, &data32->cpu_addr);

			err = get_user(usize, &data->length);
			err |= put_user(usize, &data32->length);

			err = get_user(u64_data, &data->align);
			err |= put_user(u64_data, &data32->align);

			err = get_user(uint, &data->heap_id);
			err |= put_user(uint, &data32->heap_id);

			err = get_user(uint, &data->flags);
			err |= put_user(uint, &data32->flags);

			return ret ? ret : err;
		}
		case COMPAT_CMA_POOL_IOC_FREE: // free a buffer
		{
			compat_uint_t uint;
			compat_u64 u64_data;
			compat_size_t usize;

			struct compat_cma_free_args __user *data32;
			struct cma_free_args __user *data;
			int	err = 0;

			data32 = compat_ptr(arg);						// use 32-bit variable type to point the data(compat is used for 32-bit app and 64-bit kernel, the arg is 32-bit!!
			data = compat_alloc_user_space(sizeof(*data));	// alloc a memory in user space
			if (data == NULL)
				return -EFAULT;

			/*
			 * copy 32_bit data to 64_bit data
			 * we only wanna change data_size for flags, but we still need to copy all data from data32(user data) to data
			 */
			err = get_user(uint, &data32->heap_id);
			err |= put_user(uint, &data->heap_id);

			err = get_user(u64_data, &data32->offset_in_heap);
			err |= put_user(u64_data, &data->offset_in_heap);

			err = get_user(usize, &data32->length);
			err |= put_user(usize, &data->length);
			if(err)
				return err;

			/* call the responding 64_bit ioctl */
			return filp->f_op->unlocked_ioctl(filp, CMA_POOL_IOC_FREE, (unsigned long)data);
		}
		case CMA_POOL_IOC_RELEASE: // free heap
		{
			return filp->f_op->unlocked_ioctl(filp, cmd, (unsigned long)compat_ptr(arg));
		}
        case  COMPAT_CMA_POOL_IOC_GET_HEAP_INFO_FROM_PA:
        {
			compat_uint_t uint;
            compat_u64 u64_data;
            compat_size_t usize;

            struct compat_get_info_from_pa __user *data32;
            struct get_info_from_pa __user *data;
            int err = 0;
            MCMA_CUST_DEBUG("%s:%d in COMPAT_CMA_POOL_IOC_GET_HEAP_INFO_FROM_PA\n",__FUNCTION__,__LINE__);
            data32 = compat_ptr(arg);                       // use 32-bit variable type to point the data(compat is used for 32-bit app and 64-bit kernel, the arg is 32-bit!!
            data = compat_alloc_user_space(sizeof(*data));  // alloc a memory in user space
            if (data == NULL)
                return -EFAULT;

            /*
	          * copy 32_bit data to 64_bit data
	          * we only wanna change data_size for flags, but we still need to copy all data from data32(user data) to data
	          */
            err = get_user(u64_data, &data32->PA);
            err |= put_user(u64_data, &data->PA);

            if(err)
				return err;
            /* call the responding 64_bit ioctl */
            ret = filp->f_op->unlocked_ioctl(filp, CMA_POOL_IOC_GET_HEAP_INFO_FROM_PA, (unsigned long)data);

            /*
             * copy 64_bit data to 32_bit data for return to 32_bit app
             */

            err = get_user(uint, &data->heap_id);
            err |= put_user(uint, &data32->heap_id);
            MCMA_CUST_DEBUG("data->heap_id=%u, data32->heap_id=%u\n",data->heap_id,data32->heap_id);
            err = get_user(uint, &data->miu);
            err |= put_user(uint, &data32->miu);

            err = get_user(u64_data, &data->heap_miu_start_offset);
            err |= put_user(u64_data, &data32->heap_miu_start_offset);

            err = get_user(usize, &data->heap_length);
            err |= put_user(usize, &data32->heap_length);

            err = get_user(u64_data, &data->pa_offset_in_heap);
            err |= put_user(u64_data, &data32->pa_offset_in_heap);

            return ret ? ret : err;
        }
        case  COMPAT_CMA_POOL_IOC_KERNEL_MODE_GET_USER_VA:
        {
            compat_uint_t uint;
            compat_size_t usize;

            struct compat_cma_heap_get_user_va __user *data32;
            struct cma_heap_get_user_va __user *data;
            int err = 0;
            MCMA_CUST_DEBUG("%s:%d in COMPAT_CMA_POOL_IOC_KERNEL_MODE_GET_USER_VA\n",__FUNCTION__,__LINE__);

            data32 = compat_ptr(arg);                       // use 32-bit variable type to point the data(compat is used for 32-bit app and 64-bit kernel, the arg is 32-bit!!
            data = compat_alloc_user_space(sizeof(*data));  // alloc a memory in user space
            if (data == NULL)
				return -EFAULT;

            /*
             * copy 32_bit data to 64_bit data
             * we only wanna change data_size for flags, but we still need to copy all data from data32(user data) to data
             */
            err = get_user(uint, &data32->heap_id);
            err |= put_user(uint, &data->heap_id);

            MCMA_CUST_DEBUG("data->heap_id=%u, data32->heap_id=%u\n",data->heap_id,data32->heap_id);

            err = get_user(uint, &data32->flags);
            err |= put_user(uint, &data->flags);

            if(err)
				return err;

            /* call the responding 64_bit ioctl */
            ret = filp->f_op->unlocked_ioctl(filp, CMA_POOL_IOC_KERNEL_MODE_GET_USER_VA, (unsigned long)data);

            /*
             * copy 64_bit data to 32_bit data for return to 32_bit app
             */

            err = get_user(uint, &data->user_va_valid_flag);
            err |= put_user(uint, &data32->user_va_valid_flag);

            err = get_user(usize, &data->heap_start_user_space_virt_addr);
            err |= put_user(usize, &data32->heap_start_user_space_virt_addr);

            return ret ? ret : err;
        }
        case  COMPAT_CMA_POOL_IOC_KERNEL_MODE_MAPPING_USER_VA_PAGE:
        {
            compat_uint_t uint;

            struct compat_cma_mmap_user_va_page __user *data32;
            struct cma_mmap_user_va_page __user *data;
            int err = 0;
            MCMA_CUST_DEBUG("%s:%d in COMPAT_CMA_POOL_IOC_KERNEL_MODE_MAPPING_USER_VA_PAGE\n",__FUNCTION__,__LINE__);

            data32 = compat_ptr(arg);                       // use 32-bit variable type to point the data(compat is used for 32-bit app and 64-bit kernel, the arg is 32-bit!!
            data = compat_alloc_user_space(sizeof(*data));  // alloc a memory in user space
            if (data == NULL)
				return -EFAULT;

            /*
             * copy 32_bit data to 64_bit data
             * we only wanna change data_size for flags, but we still need to copy all data from data32(user data) to data
             */
            err = get_user(uint, &data32->heap_id);
            err |= put_user(uint, &data->heap_id);

            err = get_user(uint, &data32->flags);
            err |= put_user(uint, &data->flags);

            if(err)
				return err;

            /* call the responding 64_bit ioctl */
			return filp->f_op->unlocked_ioctl(filp, CMA_POOL_IOC_KERNEL_MODE_MAPPING_USER_VA_PAGE, (unsigned long)data);

        }
		default:
        {
			printk(CMA_ERR "\033[35mFunction = %s, Line = %d, ERROR!!! CAN NOT FIND CMD_TYPE\033[m\n", __PRETTY_FUNCTION__, __LINE__);
			return -ENOTTY;
        }
    }
}
#endif

static long CMA_Pool_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    unsigned int dir;
#ifdef CONFIG_MP_CMA_PATCH_COUNT_TIMECOST
	ktime_t start_time = ktime_get_real();
	s64 cma_alloc_time_cost_ms = 0;
#endif

    union {
        struct cma_heap_info heap_info;
        struct cma_alloc_args allocation;
        struct cma_free_args free;
        unsigned int heap_id;
        struct get_info_from_pa pa_get_info;
        struct cma_heap_get_user_va vma_valid;
        struct cma_mmap_user_va_page mmap_user_va_page;
    } data;

    dir = _IOC_DIR(cmd);

    if (_IOC_SIZE(cmd) > sizeof(data))
        return -EINVAL;

	printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, copy_from_user: %d bytes, cmd is 0x%X\033[m\n", __PRETTY_FUNCTION__, __LINE__, _IOC_SIZE(cmd), cmd);
    if (dir & _IOC_WRITE)
        if (copy_from_user(&data, (void __user *)arg, _IOC_SIZE(cmd)))
            return -EFAULT;

    switch (cmd) {
    case CMA_POOL_IOC_INIT:
    {
        ret = CMA_Pool_Init(data.heap_info.heap_id, data.heap_info.flags, &data.heap_info.miu,
                    &data.heap_info.bus_addr, &data.heap_info.heap_miu_start_offset,
                    &data.heap_info.heap_length
                    ,&data.heap_info.virt_addr, (filp_private *)filp->private_data);

#ifdef CONFIG_MP_CMA_PATCH_COUNT_TIMECOST
		Show_Diff_Time("CMA_POOL_IOC_INIT", start_time, 0);
#endif

        if(ret)
            return -EFAULT;

        if(copy_to_user((void __user *)arg, &data, _IOC_SIZE(cmd)))
        {
            MCMA_CUST_ERR("copy to user error\n");
            ret = -EFAULT;
        }

        break;
    }
    case CMA_POOL_IOC_ALLOC:
    {
		ret = CMA_Pool_Alloc(data.allocation.heap_id, data.allocation.offset_in_heap,
            &data.allocation.cpu_addr, data.allocation.length, data.allocation.align,
            data.allocation.flags, (filp_private *)filp->private_data);

#ifdef CONFIG_MP_CMA_PATCH_COUNT_TIMECOST
		if(!ret)
		{
			cma_alloc_time_cost_ms = Show_Diff_Time("CMA_POOL_IOC_ALLOC", start_time, 0);

			//printk("\033[35mFunction = %s, insert measurement data to %d \033[m\n", __PRETTY_FUNCTION__, data.allocation.heap_id);
			mutex_lock(&cma_measurement_data[data.allocation.heap_id].cma_measurement_lock);
			cma_measurement_data[data.allocation.heap_id].total_alloc_time_cost_ms += cma_alloc_time_cost_ms;
			mutex_unlock(&cma_measurement_data[data.allocation.heap_id].cma_measurement_lock);
		}
#endif

		/* it seems we need to do copy_to_user for data.allocation.cpu_addr(va) */
        if(ret)
		{
			printk(CMA_ERR "\033[35mFunction = %s, Line = %d, CMA_Pool_Alloc Failed, the ret is %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, ret);
            return -EFAULT;
		}
        break;
    }
    case CMA_POOL_IOC_FREE:
    {
        ret = CMA_Pool_Free(data.free.heap_id, data.free.offset_in_heap, data.free.length,
            (filp_private *)filp->private_data, true);

#ifdef CONFIG_MP_CMA_PATCH_COUNT_TIMECOST
		Show_Diff_Time("CMA_POOL_IOC_FREE", start_time, 0);
#endif

        if(ret)
            return -EFAULT;
        break;
    }
    case CMA_POOL_IOC_RELEASE:
    {
        ret = CMA_Pool_Release_Heap(data.heap_id, (filp_private *)filp->private_data);
#ifdef CONFIG_MP_CMA_PATCH_COUNT_TIMECOST
		Show_Diff_Time("CMA_POOL_IOC_RELEASE", start_time, 0);
#endif
        if(ret)
            return -EFAULT;

        break;
    }

	// following 3 cases are not checked for mix mode (used by user space)
#ifdef CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
    case CMA_POOL_IOC_GET_HEAP_INFO_FROM_PA:
    {
        //this step no need alloc info, can delay get alloc info in case CMA_POOL_IOC_KERNEL_MODE_GET_USER_VA
        ret = CMA_Pool_Get_Info_From_Pa(data.pa_get_info.PA, &data.pa_get_info.heap_id,
                                    &data.pa_get_info.miu, &data.pa_get_info.heap_miu_start_offset,
                                    &data.pa_get_info.heap_length, &data.pa_get_info.pa_offset_in_heap, (filp_private *)filp->private_data);
		if(ret)
			return -EFAULT;
        break;
    }
    case CMA_POOL_IOC_KERNEL_MODE_GET_USER_VA:
    {
        unsigned int flags = 0;
        if(data.vma_valid.flags & CMA_FLAG_KERNEL_MODE_USER_SPACE_MAP_CACHED)
            flags |= KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE;

        MCMA_CUST_DEBUG("heap_id=%u,flags=%u\n",data.vma_valid.heap_id,flags);
        ret = CMA_Pool_Kernel_Mode_Get_User_Va(data.vma_valid.heap_id,flags,
                                                                                  &data.vma_valid.user_va_valid_flag,(unsigned long *)&data.vma_valid.heap_start_user_space_virt_addr
                                                                                  , (filp_private *)filp->private_data);
        MCMA_CUST_DEBUG("data.vma_valid.heap_start_user_space_virt_addr=0x%lx",data.vma_valid.heap_start_user_space_virt_addr);
        if(ret)
        {
			printk(CMA_ERR "\033[35mFunction = %s, Line = %d, CMA_POOL_IOC_KERNEL_MODE_FROM_PA_VMA_VA Failed, the ret is %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, ret);
			return -EFAULT;
        }

        break;
    }
    case CMA_POOL_IOC_KERNEL_MODE_MAPPING_USER_VA_PAGE:
    {
        //CMA_POOL_MAPPING those allocated.
        unsigned int flags =0;
        if(data.vma_valid.flags & CMA_FLAG_KERNEL_MODE_USER_SPACE_MAP_CACHED)
            flags |= KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE;
        ret = CMA_Pool_Mmap_User_Va_Page(data.mmap_user_va_page.heap_id, flags, (filp_private *)filp->private_data);
        if(ret)
        {
               printk(CMA_ERR "\033[35mFunction = %s, Line = %d, CMA_POOL_KERNEL_MODE_MAP_USER_VA Failed, the ret is %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, ret);
               return -EFAULT;
        }
        break;
    }
#endif
    default:
        return -ENOTTY;
    }

    if (dir & _IOC_READ)
    {
        if (copy_to_user((void __user *)arg, &data, _IOC_SIZE(cmd)))
        {
            MCMA_CUST_ERR("copy to user fail\n");
            return -EFAULT;
        }
    }

    return ret;
}

static int CMA_Pool_MMap(struct file *filp, struct vm_area_struct *vma)
{
    filp_private *pdev = (filp_private *)filp->private_data;
    struct heap_proc_info* heap_proc_info = NULL;
	bool bKernelProcInfo = false;
	struct kernel_cma_pool_vma_item *kernel_cma_pool_vma_item_get = NULL;
	bool find_vma_pid = false;

    if (NULL == vma) {
        MCMA_CUST_ERR("NULL == vma\n");
        return -EINVAL;
    }

	// temp_heap_id is set by CMA_Pool_Get_Info_From_Pa()
	// find user space heap_proc_info
    heap_proc_info = find_heap_proc_info(pdev->temp_heap_id, pdev);
    if(heap_proc_info == NULL)
    {
#ifdef CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
        // use user space file's pdev to get heap id, for user space mutex can work well.
        // and use kernel space file: _fileCMAPool->private_data
        // as find_heap_proc_info function's pdev.
        heap_proc_info = find_heap_proc_info(pdev->temp_heap_id, _fileCMAPool->private_data);
        if(heap_proc_info)
        {
            bKernelProcInfo = true;
            mutex_lock(&heap_proc_info->vma_list_lock);
            list_for_each_entry(kernel_cma_pool_vma_item_get, &heap_proc_info->vma_list, list_node)
			{
				if(kernel_cma_pool_vma_item_get->pid == current->tgid)
			    {
			        find_vma_pid = true;
			        printk("[%s %d]error ,should not go to here twice\n",__FUNCTION__,__LINE__);
			        BUG();
				}
			}

	        if(false == find_vma_pid)
	        {
	            kernel_cma_pool_vma_item_get = (struct kernel_cma_pool_vma_item *)kzalloc(sizeof(struct kernel_cma_pool_vma_item),GFP_KERNEL);
                if(!kernel_cma_pool_vma_item_get)
                {
			        mutex_unlock(&heap_proc_info->vma_list_lock);
			        return -EINVAL;
				}
                kernel_cma_pool_vma_item_get->pid = current->tgid;
		        kernel_cma_pool_vma_item_get->flags = 0;
		        list_add_tail(&kernel_cma_pool_vma_item_get->list_node, &heap_proc_info->vma_list);
            }
        }
        else
        {
            MCMA_CUST_ERR("not find heap id %u proc info!!!\n", pdev->temp_heap_id);
            return -EINVAL;
        }
#else
        MCMA_CUST_ERR("not find heap id %u proc info!!!\n", pdev->temp_heap_id);
        return -EINVAL;
#endif
    }

#ifdef  CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
    // this function is for user utopia use,
    // so type_states should not to be TYPE_STATES_KERNEL.
    BUG_ON(((heap_proc_info->type_states == TYPE_STATES_KERNEL) != 0) && (false == bKernelProcInfo));
#endif
    vma->vm_flags |= VM_IO;
    vma->vm_flags |= VM_PFNMAP;

    //printk(CMA_ERR "CMA_Pool_MMap vma-start %lx vma-end %lx\n", vma->vm_start, vma->vm_end);
#ifdef CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
    if((!heap_proc_info->vma_cached) && (false == bKernelProcInfo))
#else
	if(!heap_proc_info->vma_cached)
#endif
    {
#if (defined(MIPS))
        vma->vm_page_prot.pgprot = (vma->vm_page_prot.pgprot & ~_CACHE_MASK) | _CACHE_UNCACHED; /* MIPS's equivalents of 'pgprot_writecombine' */
#else
        vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
#endif
    }

    vma->vm_ops = &cma_vm_ops;

#ifdef CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
    if(true == bKernelProcInfo)
	{
		kernel_cma_pool_vma_item_get->vma = vma;
		mutex_unlock(&heap_proc_info->vma_list_lock);
        MCMA_CUST_DEBUG("heap_id=%u\n",pdev->temp_heap_id);
    }
	else
		heap_proc_info->vma = vma;
#else
    heap_proc_info->vma = vma;
#endif
    return 0;
}

static void dump_CMA_Pool(struct cma_memory_info * heap_info, bool force_print)
{
    struct cma_allocation_list *buffer_list = NULL;
    struct cma_buffer *buffer = NULL;
    struct cma *cma_area = NULL;
	struct pid *pid_now;

    if(!force_print)
    {
#if !CMA_POOL_DUMP
        return;
#endif
    }

    cma_area = heap_info->dev->cma_area;
    buffer_list = &heap_info->alloc_list;

    printk(CMA_DEBUG "dump heap_id %u information: min_start 0x%lX, max_end 0x%lX\n", heap_info->heap_id,
        buffer_list->min_start, buffer_list->max_end);
    list_for_each_entry(buffer, &buffer_list->list_head, list)
    {
		pid_now = find_get_pid(buffer->pid);
		printk(CMA_DEBUG "allocated info: cpu address 0x%lX, phy_addr 0x%lX, length 0x%lX, pid %lu, freed %d, mapped %d \"%s\"\n",
            (unsigned long)buffer->cpu_addr, buffer->start_pa, buffer->length,
            (unsigned long)buffer->pid, buffer->freed, buffer->mapped,
			pid_task(pid_now, PIDTYPE_PID)->comm);
		put_pid(pid_now);
    }
}

static void Show_CMA_Pool_state(void)
{
	int heap_id = 0;

	struct cma_memory_info *heap_info = NULL;
	printk("\033[0;32;31m -------------- CMA_Pool State -------------- \033[m\n");
	for(heap_id = 1; heap_id < 50; heap_id++){
		heap_info = find_heap_info(heap_id);
		if(!heap_info)
			continue;
		mutex_lock(&heap_info->lock);
		dump_CMA_Pool(heap_info, true);
		mutex_unlock(&heap_info->lock);
		heap_info = NULL;
	}

	printk("\033[0;32;31m -------------- End -------------- \033[m\n");
}

static int cma_state_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t cma_state_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	Show_CMA_Pool_state();
	return 0;
}

static const struct file_operations cma_state_fops = {
	.owner		= THIS_MODULE,
	.open		= cma_state_open,
	.read		= cma_state_read,
	.llseek		= seq_lseek,
};

static struct platform_device cmapool_dev = {
    .name   = MOD_CMAPOOL_NAME,
    .id     = 0,
};

static int cma_pool_suspend(struct device *dev)
{
    return 0;
}

static int cma_pool_resume(struct device *dev)
{
    return 0;
}

static int cma_pool_freeze(struct device *dev)
{
    return 0;
}

static int cma_pool_thaw(struct device *dev)
{
    return 0;
}

static int cma_pool_restore(struct device *dev)
{
    return 0;
}

static int cma_pool_probe(struct platform_device *pdev)
{
    pdev->dev.platform_data = NULL;
    return 0;
}

static int cma_pool_remove(struct platform_device *pdev)
{
    pdev->dev.platform_data = NULL;
    return 0;
}

static const struct dev_pm_ops cma_pool_pm_ops =
{
    .suspend = cma_pool_suspend,
    .resume_early  = cma_pool_resume,
    .freeze  = cma_pool_freeze,
    .thaw    = cma_pool_thaw,
    .restore = cma_pool_restore,
};

static struct platform_driver Mstar_cmapool_driver = {
    .probe   = cma_pool_probe,
    .remove  = cma_pool_remove,

    .driver = {
        .name   = MOD_CMAPOOL_NAME,
        .owner  = THIS_MODULE,
        .pm     = &cma_pool_pm_ops,
    }
};

#ifdef CONFIG_MP_CMA_PATCH_DELAY_FREE
#define USER_ROOT_DIR "delay_free"
#define USER_ENTRY1   "delay_free1"
static struct proc_dir_entry *delay_free_root;

static int delay_free_size_open(struct inode *inode, struct file *file){
	return 0;
}

static ssize_t delay_free_size_write(struct file *file, const char __user *buffer,
									size_t count, loff_t *ppos)
{
	char local_buf[256];
	if(count>=256)
		return -EINVAL;

	if (copy_from_user(local_buf, buffer, count))
		return -EFAULT;
	local_buf[count] = 0;

	delay_free_size_limite = simple_strtol(local_buf,NULL,10);

	return count;
}

static ssize_t delay_free_size_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	printk("\033[35mFunction = %s, Line = %d, Delay Free Size limite : %d byte\033[m\n", __PRETTY_FUNCTION__, __LINE__, delay_free_size_limite);
		return 0;
}

static int delay_free_timeout_open(struct inode *inode, struct file *file)
{
	return 0;
}

static void force_free_all_cma_cache(void)
{
	int ret = 0;
	struct list_head *pos, *q;
	struct delay_free_reserved *tmp;

	mutex_lock(&delay_free_lock);
	list_for_each_safe(pos, q, &delay_list){
		tmp = list_entry(pos, struct delay_free_reserved, list);
		printk("\033[0;32;31m [Ian] %s %d Shrink delay_free_heap_id %d\033[m\n",__func__,__LINE__,tmp->delay_free_heap_id);
		ret = CMA_Pool_Free(tmp->delay_free_heap_id,tmp->delay_free_offset_in_heap,
									tmp->delay_free_length,tmp->delay_free_pdev,false);
		if(ret)
			MCMA_BUG_ON(1);
		list_del(pos);
		kfree(tmp);
	}
	mutex_unlock(&delay_free_lock);
}

static ssize_t delay_free_timeout_write(struct file *file, const char __user *buffer,
									size_t count, loff_t *ppos)
{
	char local_buf[256];
	struct list_head *pos, *q;
	struct delay_free_reserved *tmp;

	if(count>=256)
		return -EINVAL;
	if (copy_from_user(local_buf, buffer, count))
		return -EFAULT;
	local_buf[count] = 0;

	delay_free_time_limite = simple_strtol(local_buf, NULL, 10);

	if(delay_free_time_limite == 0)
		force_free_all_cma_cache();
	mutex_lock(&delay_free_lock);

	list_for_each_safe(pos, q, &delay_list){
		tmp = list_entry(pos, struct delay_free_reserved, list);
		if(tmp->delay_free_time_out > delay_free_time_limite){
			tmp->delay_free_time_out = delay_free_time_limite;
		}
	}
	mutex_unlock(&delay_free_lock);

	return count;
}

static ssize_t delay_free_timeout_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	printk("\033[35mFunction = %s, Line = %d, Delay Free time out : %d sec\033[m\n", __PRETTY_FUNCTION__, __LINE__, delay_free_time_limite);
		return 0;
}

static int delay_free_lowmem_minfree_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t delay_free_lowmem_minfree_write(struct file *file, const char __user *buffer,
									size_t count, loff_t *ppos)
{
	char local_buf[256];

	if(count>=256)
		return -EINVAL;

	if (copy_from_user(local_buf, buffer, count))
		return -EFAULT;
	local_buf[count] = 0;

	delay_free_lowmem_minfree = simple_strtol(local_buf,NULL,10);


	return count;
}

static ssize_t delay_free_lowmem_minfree_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	printk("\033[35mFunction = %s, Line = %d, Delay Free lowmem minfree : %d byte\033[m\n", __PRETTY_FUNCTION__, __LINE__, delay_free_lowmem_minfree);
		return 0;
}

static int delay_free_force_free_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t delay_free_force_free_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{

   Show_CMA_Pool_state();
   return 0;
}

static ssize_t delay_free_force_free_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long duration_in_ms;
	char local_buf[256];

	if(size>=256)
		return -EINVAL;

	if (copy_from_user(local_buf, buf, size))
		return -EFAULT;

	local_buf[size] = 0;
	duration_in_ms = simple_strtol(local_buf,NULL,10);

	if(duration_in_ms > 30000)
		duration_in_ms = 30000;

	delay_free_evict_duration = (duration_in_ms*HZ)/1000;
	delay_free_last_force_free_jiffies = jiffies;
	force_free_all_cma_cache();

	return size;
}

static const struct file_operations delay_free_fops = {
	.owner = THIS_MODULE,
	.write = delay_free_size_write,
	.read = delay_free_size_read,
	.open = delay_free_size_open,
	.llseek = seq_lseek,
};

static const struct file_operations delay_free_timeout_fops = {
	.owner = THIS_MODULE,
	.write = delay_free_timeout_write,
	.read = delay_free_timeout_read,
	.open = delay_free_timeout_open,
	.llseek = seq_lseek,
};

static const struct file_operations delay_free_lowmem_minfree_fops = {
	.owner = THIS_MODULE,
	.write = delay_free_lowmem_minfree_write,
	.read = delay_free_lowmem_minfree_read,
	.open = delay_free_lowmem_minfree_open,
	.llseek = seq_lseek,
};
static const struct file_operations delay_free_force_free_fops = {
	.owner = THIS_MODULE,
	.write = delay_free_force_free_write,
	.read = delay_free_force_free_read,
	.open = delay_free_force_free_open,
	.llseek = seq_lseek,
};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,31)
static unsigned long delay_free_count_objects(struct shrinker *s, struct shrink_control *sc)
{
	return 0;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,31)
static unsigned long delay_free_shrink(struct shrinker *s, struct shrink_control *sc)
#else
static int delay_free_shrink(struct shrinker *s, struct shrink_control *sc)
#endif
{

	int other_free = global_page_state(NR_FREE_PAGES) - totalreserve_pages;
	int other_file = global_page_state(NR_FILE_PAGES) -
						global_page_state(NR_SHMEM);
	int free_cma = 0;

#ifdef CONFIG_CMA
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,31)
	if (gfpflags_to_migratetype(sc->gfp_mask) != MIGRATE_MOVABLE)
#else
	if (allocflags_to_migratetype(sc->gfp_mask) != MIGRATE_MOVABLE)
#endif
		free_cma = global_page_state(NR_FREE_CMA_PAGES);
#endif

	if(delay_free_lowmem_minfree < lowmem_minfree[lowmem_minfree_size-1])
		delay_free_lowmem_minfree = lowmem_minfree[lowmem_minfree_size-1];

	if ((other_free - free_cma) < delay_free_lowmem_minfree && other_file < delay_free_lowmem_minfree) {
		force_free_all_cma_cache();

	}
	return 0;
}

static struct shrinker delay_free_shrinker = {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,31)
	.count_objects = delay_free_count_objects,
	.scan_objects = delay_free_shrink,
#else
	.shrink = delay_free_shrink,
#endif
	.seeks = DEFAULT_SEEKS * 16
};
#endif

#ifdef CONFIG_MP_CMA_PATCH_MBOOT_STR_USE_CMA
void str_reserve_mboot_cma_buffer()
{
	struct cma_memory_info *mboot_str_heap;
	struct cma *mboot_str_cma;
	struct cma_allocation_list *mboot_str_cma_heap_alloc_list;
	struct cma_buffer *still_allocated_mboot_str_cma_buffer, *next = NULL;

	unsigned long mboot_str_ask_cma_start_ba = 0;
	unsigned long mboot_str_ask_cma_end_ba = 0;

	int insert_index = 0;
	int ret = 0;
	struct page *mboot_str_ask_cma_page = NULL;

	if(mboot_str_heap_id == 0)
	{
		printk(CMA_ERR "\033[35mFunction = %s, Line = %d, no mboot_str_cma ...\033[m\n", __PRETTY_FUNCTION__, __LINE__);
		return;
	}

	printk(CMA_ERR "\033[31mFunction = %s, Line = %d, asking mboot_str_cma...\033[m\n", __PRETTY_FUNCTION__, __LINE__);
	memset(mboot_str_cma_alloc, 0, sizeof(struct mboot_str_cma_alloc_log) * MBOOT_STR_CMA_ALLOC_CNT);

	mboot_str_heap = find_heap_info(mboot_str_heap_id);	// get the str used buffer
	if(mboot_str_heap)
	{
		if(!mboot_str_heap->dev)
		{
			printk(CMA_ERR "\033[35mFunction = %s, Line = %d, strange, we can not get mboot_str_dev\033[m\n", __PRETTY_FUNCTION__, __LINE__);
			BUG_ON(1);
		}

		if(!mboot_str_heap->dev->cma_area)
		{
			printk(CMA_ERR "\033[35mFunction = %s, Line = %d, strange, we can not get mboot_str_cma\033[m\n", __PRETTY_FUNCTION__, __LINE__);
			BUG_ON(1);
		}

		printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, asking mboot_str_heap from %s\033[m\n", __PRETTY_FUNCTION__, __LINE__, dev_name(mboot_str_heap->dev));
	}
	else
	{
		printk(CMA_ERR "\033[35mFunction = %s, Line = %d, strange, we can not get mboot_str_heap\033[m\n", __PRETTY_FUNCTION__, __LINE__);
		BUG_ON(1);
	}

	mboot_str_cma = mboot_str_heap->dev->cma_area;
	mboot_str_cma_heap_alloc_list = &mboot_str_heap->alloc_list;
	printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, the mboot_str_cma is form 0x%lX to 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__,
			(mboot_str_cma->base_pfn) << PAGE_SHIFT, (mboot_str_cma->base_pfn + mboot_str_cma->count) << PAGE_SHIFT);

	mboot_str_ask_cma_start_ba = mboot_str_cma->base_pfn << PAGE_SHIFT;
	mboot_str_ask_cma_end_ba = (mboot_str_cma->base_pfn + mboot_str_cma->count) << PAGE_SHIFT;

	/* find if there are any cma_buffers @ alloc_list, which means these cma_buffers are still kept by driver, we do not ask it!! but mboot may touch these kept cma_buffers*/
	list_for_each_entry_safe(still_allocated_mboot_str_cma_buffer, next, &mboot_str_cma_heap_alloc_list->list_head, list)
	{
		printk(CMA_ERR "\033[33mFunction = %s, Line = %d, still allocated cma_buffer from 0x%lX to 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__,
				still_allocated_mboot_str_cma_buffer->start_pa, (still_allocated_mboot_str_cma_buffer->start_pa + still_allocated_mboot_str_cma_buffer->length));

		printk(CMA_ERR "allocated info: cpu address 0x%lX, pid %lu, freed %d, mapped %d\n",
			(unsigned long)still_allocated_mboot_str_cma_buffer->cpu_addr,
			(unsigned long)still_allocated_mboot_str_cma_buffer->pid, still_allocated_mboot_str_cma_buffer->freed, still_allocated_mboot_str_cma_buffer->mapped);

		/* we allcate from mboot_str_ask_cma_start_pfn to still_allocated_mboot_str_cma_buffer->start_pa,
		 * and change mboot_str_ask_cma_start_pfn to "still_allocated_mboot_str_cma_buffer->start_pa + still_allocated_mboot_str_cma_buffer->length"
		 */
		if(still_allocated_mboot_str_cma_buffer->start_pa != mboot_str_ask_cma_start_ba)
		{
			for(insert_index = 0; insert_index < MBOOT_STR_CMA_ALLOC_CNT; insert_index++)
			{
				if(mboot_str_cma_alloc[insert_index].used)
					continue;
				else
					break;
			}

			if(insert_index == MBOOT_STR_CMA_ALLOC_CNT)
			{
				printk(CMA_ERR "\033[31mFunction = %s, Line = %d, [Head Alloc] no enough insert data!!!\033[m\n", __PRETTY_FUNCTION__, __LINE__);
				BUG_ON(1);
			}
			else
			{
			    int length = still_allocated_mboot_str_cma_buffer->start_pa - mboot_str_ask_cma_start_ba;
				/* allocate freed cma_buffer */
				dumpKRange(mboot_str_heap);
				printk(CMA_ERR "\033[34mFunction = %s, Line = %d, [Head Alloc] allocate from 0x%lX to 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__,
						mboot_str_ask_cma_start_ba, still_allocated_mboot_str_cma_buffer->start_pa);

				mboot_str_ask_cma_page = NULL;
				//mboot_str_ask_cma_page = dma_alloc_at_from_contiguous(mboot_str_heap->dev, length >> PAGE_SHIFT, 1, mboot_str_ask_cma_start_ba);
				mboot_str_ask_cma_page = cma_alloc(mboot_str_heap->dev, length>>PAGE_SHIFT, 0, 1);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,31)
                //__dma_flush_buffer(mboot_str_ask_cma_page, (length>>PAGE_SHIFT) << PAGE_SHIFT);	// we dont flush anymore
#else
                __dma_flush_buffer(mboot_str_ask_cma_page, (length>>PAGE_SHIFT) << PAGE_SHIFT);
#endif
				if (!mboot_str_ask_cma_page)
				{
					printk(CMA_ERR "dma_alloc_at_from_contiguous failed\n");
					BUG_ON(1);
				}
				printk(CMA_ERR "\033[34mFunction = %s, Line = %d, [Head Alloc] done\033[m\n", __PRETTY_FUNCTION__, __LINE__);

				/* delete kernel protect range */
				mboot_str_cma_alloc[insert_index].str_cma_alloc_buffer.start_pa = mboot_str_ask_cma_start_ba;
				mboot_str_cma_alloc[insert_index].str_cma_alloc_buffer.length = still_allocated_mboot_str_cma_buffer->start_pa - mboot_str_ask_cma_start_ba;

				ret = deleteKRange(mboot_str_heap, &mboot_str_cma_alloc[insert_index].str_cma_alloc_buffer);  // this buffer will contain hole_buffer(if hole_buffer exists)
				dumpKRange(mboot_str_heap);
				if(ret)
				{
					printk(CMA_ERR "delete kernal range fail\n");
					printk(CMA_ERR "delete kernal range fail\n");
					BUG_ON(1);
				}

				/* if allocate pass */
				mboot_str_cma_alloc[insert_index].used = 1;
				mboot_str_cma_alloc[insert_index].start_pa = mboot_str_ask_cma_start_ba;
				mboot_str_cma_alloc[insert_index].alloc_page_cnt = (still_allocated_mboot_str_cma_buffer->start_pa - mboot_str_ask_cma_start_ba) >> PAGE_SHIFT;
			}
		}

		mboot_str_ask_cma_start_ba = still_allocated_mboot_str_cma_buffer->start_pa + still_allocated_mboot_str_cma_buffer->length;
	}

	/* after above allocation, the tail free cma_memory will still be free, we check if there is any tail free cma_memory, and allocate it */
	if(mboot_str_ask_cma_start_ba != mboot_str_ask_cma_end_ba)
	{
		for(insert_index = 0; insert_index < MBOOT_STR_CMA_ALLOC_CNT; insert_index++)
		{
			if(mboot_str_cma_alloc[insert_index].used)
				continue;
			else
				break;
		}

		if(insert_index == MBOOT_STR_CMA_ALLOC_CNT)
		{
			printk(CMA_ERR "\033[31mFunction = %s, Line = %d, [Tail Alloc] no enough insert data!!!\033[m\n", __PRETTY_FUNCTION__, __LINE__);
			BUG_ON(1);
		}
		else
		{
			/* allocate freed cma_buffer */
            int length = mboot_str_ask_cma_end_ba - mboot_str_ask_cma_start_ba;
			dumpKRange(mboot_str_heap);
			printk(CMA_ERR "\033[34mFunction = %s, Line = %d, [Tail Alloc] allocate from 0x%lX to 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, mboot_str_ask_cma_start_ba, mboot_str_ask_cma_end_ba);
			mboot_str_ask_cma_page = NULL;
			mboot_str_ask_cma_page = dma_alloc_at_from_contiguous(mboot_str_heap->dev, length >> PAGE_SHIFT, 1, mboot_str_ask_cma_start_ba);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,31)
            //__dma_flush_buffer(mboot_str_ask_cma_page, (length>>PAGE_SHIFT) << PAGE_SHIFT);	// we dont flush anymore
#else
            __dma_flush_buffer(mboot_str_ask_cma_page, (length>>PAGE_SHIFT) << PAGE_SHIFT);
#endif
			if (!mboot_str_ask_cma_page)
			{
				printk(CMA_ERR "dma_alloc_at_from_contiguous failed\n");
				BUG_ON(1);
			}
			printk(CMA_ERR "\033[34mFunction = %s, Line = %d, [Tail Alloc] done\033[m\n", __PRETTY_FUNCTION__, __LINE__);

			/* delete kernel protect range */
			mboot_str_cma_alloc[insert_index].str_cma_alloc_buffer.start_pa = mboot_str_ask_cma_start_ba;
			mboot_str_cma_alloc[insert_index].str_cma_alloc_buffer.length = mboot_str_ask_cma_end_ba - mboot_str_ask_cma_start_ba;

			ret = deleteKRange(mboot_str_heap, &mboot_str_cma_alloc[insert_index].str_cma_alloc_buffer);  // this buffer will contain hole_buffer(if hole_buffer exists)
			dumpKRange(mboot_str_heap);
			if(ret)
			{
				printk(CMA_ERR "delete kernal range fail\n");
				printk(CMA_ERR "delete kernal range fail\n");
				BUG_ON(1);
			}

			/* if allocate pass */
			mboot_str_cma_alloc[insert_index].used = 1;
			mboot_str_cma_alloc[insert_index].start_pa = mboot_str_ask_cma_start_ba;
			mboot_str_cma_alloc[insert_index].alloc_page_cnt = (mboot_str_ask_cma_end_ba - mboot_str_ask_cma_start_ba) >> PAGE_SHIFT;
		}
	}

	printk(CMA_ERR "\033[31mFunction = %s, Line = %d, asking mboot_str_cma is done\033[m\n", __PRETTY_FUNCTION__, __LINE__);
}

void str_release_mboot_cma_buffer()
{
	struct cma_memory_info *mboot_str_heap;

	int insert_index = 0;
	int ret = 0;

	if(mboot_str_heap_id == 0)
	{
		printk(CMA_ERR "\033[35mFunction = %s, Line = %d, no mboot_str_cma ...\033[m\n", __PRETTY_FUNCTION__, __LINE__);
		return;
	}

	printk(CMA_ERR "\033[31mFunction = %s, Line = %d, release mboot_str_cma...\033[m\n", __PRETTY_FUNCTION__, __LINE__);

	mboot_str_heap = find_heap_info(mboot_str_heap_id);	// get the str used buffer

	if(mboot_str_heap)
	{
		if(!mboot_str_heap->dev)
		{
			printk(CMA_ERR "\033[35mFunction = %s, Line = %d, strange, we can not get mboot_str_dev\033[m\n", __PRETTY_FUNCTION__, __LINE__);
			BUG_ON(1);
		}

		if(!mboot_str_heap->dev->cma_area)
		{
			printk(CMA_ERR "\033[35mFunction = %s, Line = %d, strange, we can not get mboot_str_cma\033[m\n", __PRETTY_FUNCTION__, __LINE__);
			BUG_ON(1);
		}

		printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, release mboot_str_heap from %s\033[m\n", __PRETTY_FUNCTION__, __LINE__, dev_name(mboot_str_heap->dev));
	}
	else
	{
		printk(CMA_ERR "\033[35mFunction = %s, Line = %d, strange, we can not get mboot_str_heap\033[m\n", __PRETTY_FUNCTION__, __LINE__);
		BUG_ON(1);
	}

	for(insert_index = 0; insert_index < MBOOT_STR_CMA_ALLOC_CNT; insert_index++)
	{
		if(mboot_str_cma_alloc[insert_index].used)
		{
			/* free a pre-allocated cma_buffer, which is used for mboot */
			dumpKRange(mboot_str_heap);
			printk(CMA_ERR "\033[34mFunction = %s, Line = %d, [%d] free from 0x%lX to 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, insert_index,
				mboot_str_cma_alloc[insert_index].start_pa, (mboot_str_cma_alloc[insert_index].start_pa + (mboot_str_cma_alloc[insert_index].alloc_page_cnt << PAGE_SHIFT)));
			ret = dma_release_from_contiguous(mboot_str_heap->dev, pfn_to_page(mboot_str_cma_alloc[insert_index].start_pa >> PAGE_SHIFT), mboot_str_cma_alloc[insert_index].alloc_page_cnt);
			if(!ret)
			{
				printk(CMA_ERR "dma_release_from_contiguous failed\n");
				BUG_ON(1);
			}
			printk(CMA_ERR "\033[34mFunction = %s, Line = %d, [%d] free done\033[m\n", __PRETTY_FUNCTION__, __LINE__, insert_index);

			/* add kernel protect range */
			ret = addKRange(mboot_str_heap, &mboot_str_cma_alloc[insert_index].str_cma_alloc_buffer);
			dumpKRange(mboot_str_heap);
			if(ret)
			{
				printk(CMA_ERR "add kernal range fail\n");
				printk(CMA_ERR "add kernal range fail\n");
				BUG_ON(1);
			}

			/* if free pass */
			memset(&mboot_str_cma_alloc[insert_index], 0, sizeof(struct mboot_str_cma_alloc_log));
		}
	}

	printk(CMA_ERR "\033[31mFunction = %s, Line = %d, release mboot_str_cma is done\033[m\n", __PRETTY_FUNCTION__, __LINE__);
}
#endif

MSYSTEM_STATIC int __init mod_cmapool_init(void)
{
    int s32Ret;
    dev_t dev;
    struct dentry * read_file = NULL;
	struct proc_dir_entry *size;
    struct proc_dir_entry *timeout;

    cmapool_class = class_create(THIS_MODULE, "cmapool");
    if (IS_ERR(cmapool_class))
    {
        return PTR_ERR(cmapool_class);
    }

    if (CMAPoolDev.s32Major)
    {
        dev = MKDEV(CMAPoolDev.s32Major, CMAPoolDev.s32Minor);
        s32Ret = register_chrdev_region(dev, MOD_CMAPOOL_DEVICE_COUNT, MOD_CMAPOOL_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, CMAPoolDev.s32Minor, MOD_CMAPOOL_DEVICE_COUNT, MOD_CMAPOOL_NAME);
        CMAPoolDev.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        printk(CMA_ERR "Unable to get major %d\n", CMAPoolDev.s32Major);
        class_destroy(cmapool_class);
        return s32Ret;
    }

    cdev_init(&CMAPoolDev.cDevice, &CMAPoolDev.CMAPoolFop);
    if (0!= (s32Ret= cdev_add(&CMAPoolDev.cDevice, dev, MOD_CMAPOOL_DEVICE_COUNT)))
    {
        printk(CMA_ERR "Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_CMAPOOL_DEVICE_COUNT);
        class_destroy(cmapool_class);
        return s32Ret;
    }
    device_create(cmapool_class, NULL, dev, NULL, MOD_CMAPOOL_NAME);

    INIT_LIST_HEAD(&heap_info_list);

    CMAPoolDev.debug_root = debugfs_create_dir("cma_pool", NULL);
	if (!CMAPoolDev.debug_root)
		printk(CMA_ERR "cma pool: failed to create debugfs root directory.\n");
    else
		printk(CMA_ERR "cma pool: success to create debugfs root directory.\n");
    platform_device_register(&cmapool_dev);
    platform_driver_register(&Mstar_cmapool_driver);

#ifdef CONFIG_MP_CMA_PATCH_DELAY_FREE
	delay_free_tsk = kthread_create(delay_free, NULL, "Delay Free Task");
	if (IS_ERR(delay_free_tsk)) {
		printk("create kthread for delay free fail\n");
		s32Ret = PTR_ERR(delay_free_tsk);
		delay_free_tsk = NULL;
	}else
		wake_up_process(delay_free_tsk);

	delay_free_root = proc_mkdir(USER_ROOT_DIR, NULL);
	if (NULL==delay_free_root)
	{
		printk(KERN_ALERT "Create dir /proc/%s error!\n",USER_ROOT_DIR);
		return -1;
	}

	size = proc_create("cma_delay_free_size_limite", 0644, delay_free_root, &delay_free_fops);
	if (!size){
		printk(KERN_ALERT "Create dir /proc/%s/cma_delay_free_size error!\n",USER_ROOT_DIR);
		return -ENOMEM;
	}

	timeout = proc_create("cma_delay_free_timeout", 0644, delay_free_root, &delay_free_timeout_fops);
	if (!timeout){
		printk(KERN_ALERT "Create dir /proc/%s/cma_delay_free_timeout error!\n",USER_ROOT_DIR);
		return -ENOMEM;
	}

	timeout = proc_create("cma_delay_free_lowmem_minfree", 0644, delay_free_root, &delay_free_lowmem_minfree_fops);
	if (!timeout){
		printk(KERN_ALERT "Create dir /proc/%s/cma_delay_free_lowmem_minfree error!\n",USER_ROOT_DIR);
		return -ENOMEM;
	}
	timeout = proc_create("cma_force_free_cache", 0644, delay_free_root, &delay_free_force_free_fops);
	if (!timeout){
		printk(KERN_ALERT "Create dir /proc/%s/cma_force_free_cache error!\n",USER_ROOT_DIR);
		return -ENOMEM;
	}

	timeout = proc_create("cma_state", 0644, delay_free_root, &cma_state_fops);
	if (!timeout){
		printk(KERN_ALERT "Create dir /proc/%s/cma_state error!\n",USER_ROOT_DIR);
		return -ENOMEM;
	}

	register_shrinker(&delay_free_shrinker);
#endif

	if(temp_cma_debug_level != 0){
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0)
		timer_setup(&cma_debug_timer, cma_debug_callback, 0);
#else
		init_timer(&cma_debug_timer);
		cma_debug_timer.function = &cma_debug_callback;
		cma_debug_timer.data = temp_cma_debug_level;
#endif
                cma_debug_timer.expires = jiffies + 30 * HZ;
		add_timer(&cma_debug_timer);
	}

	return 0;
}

MSYSTEM_STATIC void __exit mod_cmapool_exit(void)
{
    cdev_del(&CMAPoolDev.cDevice);
    debugfs_remove_recursive(CMAPoolDev.debug_root);
    unregister_chrdev_region(MKDEV(CMAPoolDev.s32Major, CMAPoolDev.s32Minor), MOD_CMAPOOL_DEVICE_COUNT);

    device_destroy(cmapool_class, MKDEV(CMAPoolDev.s32Major, CMAPoolDev.s32Minor));
    class_destroy(cmapool_class);
    platform_device_unregister(&cmapool_dev);
    platform_driver_unregister(&Mstar_cmapool_driver);

#ifdef CONFIG_MP_CMA_PATCH_DELAY_FREE
	kthread_stop(delay_free_tsk);
	remove_proc_entry("cma_delay_free_size_limite", delay_free_root);
	remove_proc_entry("cma_delay_free_timeout", delay_free_root);
	remove_proc_entry("cma_delay_free_lowmem_minfree", delay_free_root);
	remove_proc_entry("cma_force_free_cache", delay_free_root);
	remove_proc_entry("cma_state", delay_free_root);
	remove_proc_entry(USER_ROOT_DIR, NULL);
	unregister_shrinker(&delay_free_shrinker);
#endif
}

#ifdef CONFIG_MSTAR_CMAPOOL
module_init(mod_cmapool_init);
module_exit(mod_cmapool_exit);

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,19,0)
static void cma_debug_callback(struct timer_list *t)
{
	pr_info("\033[0;32;31m %s %d to %u\033[m\n",
		__func__, __LINE__, temp_cma_debug_level);
	bootarg_cma_debug_level = temp_cma_debug_level;
}
#else
static void cma_debug_callback(unsigned long data)
{
	printk("\033[0;32;31m [Ian] %s %d to %lu\033[m\n",__func__,__LINE__,data);
	printk("\033[0;32;31m [Ian] %s %d to %lu\033[m\n",__func__,__LINE__,data);
	printk("\033[0;32;31m [Ian] %s %d to %lu\033[m\n",__func__,__LINE__,data);
	printk("\033[0;32;31m [Ian] %s %d to %lu\033[m\n",__func__,__LINE__,data);
	bootarg_cma_debug_level = data;
}
#endif

static int cma_debug(char *str)
{
    if(strcmp(str, "1") == 0)
    {
		printk("\ncma_debug_level enable Level %s\n",str);
		temp_cma_debug_level = 1;
    }
	else if(strcmp(str, "2") == 0)
	{
	    printk("\ncma_debug_level enable Level %s\n",str);
		temp_cma_debug_level = 2;
	}
    else
    {
		bootarg_cma_debug_level = 0;
    }
	return 0;
}
early_param("cma_debug_level", cma_debug);


MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("CMAPOOL driver");
MODULE_LICENSE("GPL");
#endif
