// SPDX-License-Identifier: GPL-2.0
#include <linux/mm.h>
#include "comm_mem_mgt.h"
#include "comm_debug.h"
#include "plat_iommu.h"

#define MDLA_ALLOC_FROM_DMABUF

extern struct plat_mem_callback *dmabuf_cb;
extern struct plat_mem_callback *iommu_cb;

struct plat_mem_callback *mem_cb;

int plat_mem_init(struct comm_mem_mgr *mgr)
{
#if defined(MDLA_ALLOC_FROM_DMABUF)
	mem_cb = dmabuf_cb;
#else
	mem_cb = iommu_cb;
#endif
	if (!mem_cb) {
		comm_err("%s fail, mem_cb is null\n", __func__);
		return -EFAULT;
	}

	mem_cb->init(mgr->dev);

	return 0;
}

int plat_mem_destroy(struct comm_mem_mgr *mgr)
{
	return 0;
}

int plat_mem_alloc(struct comm_mem_mgr *mgr, struct comm_kmem *kmem)
{
	struct buf_info buf;

	memset(&buf, 0, sizeof(buf));
	buf.size = kmem->size;
	buf.cache = kmem->cache;
	buf.pipe_id = kmem->pipe_id;
	buf.secure = kmem->secure;

	if (mem_cb->alloc(mgr->dev, &buf)) {
		comm_err("%s fail, size:%d cache:%d\n",
				__func__, buf.size, buf.cache);
		return -ENOMEM;
	}

	if ((kmem->pipe_id != 0xFFFFFFFF) && kmem->pipe_id) {
		if (mem_cb->auth(mgr->dev, &buf)) {
			comm_err("%s fail, pipe:%d iova:0x%llx size:%d\n",
					__func__, buf.pipe_id, buf.iova, buf.size);
		}
	}

	kmem->kva = buf.kva;
	kmem->iova = buf.iova;
	kmem->iova_size = buf.iova_size;
	kmem->phy = buf.iova;
	kmem->fd = buf.fd;
	kmem->dmabuf = buf.dmabuf;
	kmem->sgt = buf.sgt;
	kmem->db_map = buf.db_map;
	kmem->attach = buf.attach;
	kmem->pid = task_pid_nr(current);
	kmem->tgid = task_tgid_nr(current);
	kmem->status = 0;
	memcpy(kmem->comm, current->comm, 0x10);

	comm_drv_debug("%s: heap:%x pipe:%x iova:%llx fd:%d size:%d cached:%x pid:%x/%x\n",
		__func__,  (mem_cb == iommu_cb) ? 0 : 1, kmem->pipe_id, kmem->iova,
		 kmem->fd, kmem->size, kmem->cache, kmem->pid, kmem->tgid);

	return 0;
}

int plat_mem_free(struct comm_mem_mgr *mgr, struct comm_kmem *kmem, int freefd)
{
	struct buf_info buf;

	buf.size = kmem->size;
	buf.kva = kmem->kva;
	buf.iova = kmem->iova;
	buf.sgt = kmem->sgt;
	buf.attach = kmem->attach;
	buf.dmabuf = kmem->dmabuf;
	buf.db_map = kmem->db_map;
	buf.fd = kmem->fd;

	comm_drv_debug("%s(): pipe:%x iova:%llx size:%d cached:%x pid:%x\n",
		__func__, kmem->pipe_id, kmem->iova,
		 kmem->size, kmem->cache, kmem->pid);

	if (mem_cb->free(mgr->dev, &buf, freefd) < 0) {
		comm_err("%s(): fail, iova:0x%llx size:%x\n",
				__func__, buf.iova, buf.size);
		return -EFAULT;
	}
	return 0;
}

int plat_mem_cache_ctrl(struct comm_mem_mgr *mgr, struct comm_kmem *kmem, u32 cache_op)
{
	struct buf_info buf;

	buf.size = kmem->size;
	buf.kva = kmem->kva;
	buf.iova = kmem->iova;
	buf.sgt = kmem->sgt;
	buf.dmabuf = kmem->dmabuf;
	buf.db_map = kmem->db_map;
	buf.fd = kmem->fd;
	buf.cache = kmem->cache;

	if (mem_cb->cache_ctrl(mgr->dev, &buf, cache_op) < 0) {
		comm_err("%s(): flush failed\n", __func__);
		return -ENOMEM;
	}

	return 0;
}

int plat_mem_map_uva(struct comm_mem_mgr *mgr,
			struct vm_area_struct *vma,
			struct comm_kmem *kmem)
{
	struct buf_info buf;
	int ret = -EFAULT;

	switch (kmem->mem_type) {
	case APU_COMM_MEM_VLM:
		vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
		ret = remap_pfn_range(vma, vma->vm_start,
					vma->vm_pgoff,
					vma->vm_end - vma->vm_start,
					vma->vm_page_prot);
		break;
	default:
		buf.fd = kmem->fd;
		buf.dmabuf = kmem->dmabuf;
		buf.size = kmem->size;
		buf.kva = kmem->kva;
		buf.iova = kmem->iova;
		buf.cache = kmem->cache;
		ret = mem_cb->map_uva(mgr->dev, vma, &buf);
		break;
	}

	return ret;
}

int plat_mem_import(struct comm_mem_mgr *mgr, struct comm_kmem *kmem) { return 0; };
int plat_mem_unimport(struct comm_mem_mgr *mgr, struct comm_kmem *kmem) { return 0; };

int plat_mem_map_iova(struct comm_mem_mgr *mgr, struct comm_kmem *kmem)
{
	struct buf_info buf;

	buf.fd = kmem->fd;

	comm_drv_debug("%s(): fd:%d pid:%x\n", __func__, kmem->fd, kmem->pid);

	if (mem_cb->map_iova == NULL)
		return -EFAULT;

	if (mem_cb->map_iova(mgr->dev, &buf)) {
		comm_err("%s(): fd:%d pid:%x\n", __func__, kmem->fd, kmem->pid);
		return -EFAULT;
	}

	kmem->iova = buf.iova;
	kmem->iova_size = buf.iova_size;

	return 0;
}

int plat_mem_unmap_iova(struct comm_mem_mgr *mgr, struct comm_kmem *kmem)
{
	struct buf_info buf;

	buf.fd = kmem->fd;

	comm_drv_debug("%s(): fd:%d pid:%x\n", __func__, kmem->fd, kmem->pid);

	if (mem_cb->unmap_iova == NULL)
		return -EFAULT;

	if (mem_cb->unmap_iova(mgr->dev, &buf)) {
		comm_err("%s(): fd:%d pid:%x\n", __func__, kmem->fd, kmem->pid);
		return -EFAULT;
	}

	return 0;
}

int plat_mem_map_kva(struct comm_mem_mgr *mgr, struct comm_kmem *kmem)
{
	struct buf_info buf;

	buf.fd = kmem->fd;
	buf.size = kmem->size;

	comm_drv_debug("%s(): fd:%d pid:%x\n", __func__, kmem->fd, kmem->pid);

	if (mem_cb->map_kva == NULL)
		return -EFAULT;

	if (mem_cb->map_kva(mgr->dev, &buf)) {
		comm_err("%s(): fd:%d pid:%x\n", __func__, kmem->fd, kmem->pid);
		return -EFAULT;
	}

	return 0;
}

int plat_mem_unmap_kva(struct comm_mem_mgr *mgr, struct comm_kmem *kmem)
{
	struct buf_info buf;

	buf.fd = kmem->fd;

	comm_drv_debug("%s(): fd:%d pid:%x\n", __func__, kmem->fd, kmem->pid);

	if (mem_cb->unmap_kva == NULL)
		return -EFAULT;

	if (mem_cb->unmap_kva(mgr->dev, &buf)) {
		comm_err("%s(): fd:%d pid:%x\n", __func__, kmem->fd, kmem->pid);
		return -EFAULT;
	}

	return 0;
}
