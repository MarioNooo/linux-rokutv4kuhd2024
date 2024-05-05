/* SPDX-License-Identifier: GPL-2.0*/
#ifndef __APUSYS_PLAT_MEM_WRAP_H__
#define __APUSYS_PLAT_MEM_WRAP_H__

#include <linux/mm.h>
#include "comm_driver.h"
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include "mtk_iommu_dtv_api.h"
#define IOMMU_ENABLE

struct buf_info {
	u64	kva;
	u64	iova;
	u32	size;
	u32	iova_size;
	u32	type;
	pid_t	pid;
	int	fd;
	u32	cache;
	u32	pipe_id;
	u32	secure;
	struct dma_buf *dmabuf;
	struct sg_table *sgt;
	struct dma_buf_attachment *attach;
	void *db_map;
};

struct plat_mem_callback {
	int (*alloc)(struct device *dev, struct buf_info *buf);
	int (*free)(struct device *dev, struct buf_info *buf, int close);
	int (*auth)(struct device *dev, struct buf_info *buf);
	int (*map_uva)(struct device *dev, struct vm_area_struct *vma, struct buf_info *buf);
	int (*cache_ctrl)(struct device *dev, struct buf_info *buf, u32 cache_op);
	int (*init)(struct device *dev);
	void (*uninit)(struct device *dev);
	int (*map_iova)(struct device *dev, struct buf_info *buf);
	int (*unmap_iova)(struct device *dev, struct buf_info *buf);
	int (*map_kva)(struct device *dev, struct buf_info *buf);
	int (*unmap_kva)(struct device *dev, struct buf_info *buf);
};

int plat_mem_init(struct comm_mem_mgr *mgr);
int plat_mem_destroy(struct comm_mem_mgr *mgr);
int plat_mem_alloc(struct comm_mem_mgr *mgr, struct comm_kmem *kmem);
int plat_mem_free(struct comm_mem_mgr *mgr, struct comm_kmem *kmemi, int closefd);
int plat_mem_cache_ctrl(struct comm_mem_mgr *mgr, struct comm_kmem *kmem, u32 cache_op);
int plat_mem_import(struct comm_mem_mgr *mgr, struct comm_kmem *kmem);
int plat_mem_unimport(struct comm_mem_mgr *mgr, struct comm_kmem *kmem);
int plat_mem_map_iova(struct comm_mem_mgr *mgr, struct comm_kmem *kmem);
int plat_mem_unmap_iova(struct comm_mem_mgr *mgr, struct comm_kmem *kmem);
int plat_mem_map_kva(struct comm_mem_mgr *mgr, struct comm_kmem *kmem);
int plat_mem_unmap_kva(struct comm_mem_mgr *mgr, struct comm_kmem *kmem);
int plat_mem_map_uva(struct comm_mem_mgr *mgr, struct vm_area_struct *vma, struct comm_kmem *kmem);
#endif
