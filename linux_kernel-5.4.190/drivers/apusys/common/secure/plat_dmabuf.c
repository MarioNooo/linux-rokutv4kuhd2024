// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/version.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/iommu.h>
#include <linux/dma-buf.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/ion.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include "mtk_iommu_dtv_api.h"
#include "comm_debug.h"
#include "plat_iommu.h"
#include "comm_mem_mgt.h"

#if IS_ENABLED(CONFIG_DMABUF_HEAPS_SYSTEM)
#include <linux/dma-heap.h>
#include <uapi/linux/dma-heap.h>

static struct dma_heap *dmaheap_uncached;
static struct dma_heap *dmaheap_cached;
#endif

#define MDLA_BUFTAG_ID 40
#define BUFTAG_SHIFT		24
#define MDLA_BUFTAG_ATTR (MDLA_BUFTAG_ID << BUFTAG_SHIFT)

#define MDLA_DMAHEAP_UNCACHED	"mtk_mdla_buf_uncached"
#define MDLA_DMAHEAP_CACHED	"mtk_mdla_buf"

#if !IS_ENABLED(CONFIG_DMABUF_HEAPS_SYSTEM)
static int heap_id;

static int _get_ion_heapid(char *heapname)
{
	int i;
	size_t nr_heaps;
	struct ion_heap_data *data;
	unsigned int id = UINT_MAX;

	if (!heapname)
		return -EINVAL;

	nr_heaps = ion_query_heaps_kernel(NULL, 0);
	if (nr_heaps == 0)
		goto err;

	data = kcalloc(nr_heaps, sizeof(struct ion_heap_data), GFP_KERNEL);
	if (!data)
		goto err;

	if (ion_query_heaps_kernel(data, nr_heaps) == 0) {
		kfree(data);
		goto err;
	}

	for (i = 0; i < nr_heaps; i++) {
	comm_drv_debug("%s: [%d] heap name %s id %d/%d\n",
			__func__, i, data[i].name, data[i].heap_id);
		if (strcmp(data[i].name, heapname) == 0) {
			id = data[i].heap_id;
		}
	}
	kfree(data);

	if (id == UINT_MAX)
		goto err;

	comm_drv_debug("%s: found [%s] is heap id %d/%d\n",
			__func__, heapname, i, (int)nr_heaps);

	return id;
err:
	comm_err("%s: cannot find heap [%s]\n", __func__, heapname);
	return -ENOENT;
}
#endif

static int mdla_iommu_cache_ctrl(struct device *dev, struct buf_info *buf, u32 cache_op)
{
	int ret = -EINVAL;

	if (cache_op == COMM_CACHE_OP_FLUSH)
		ret = dma_buf_end_cpu_access(buf->dmabuf, DMA_BIDIRECTIONAL);
	else if (cache_op == COMM_CACHE_OP_INVALIDATE)
		ret = dma_buf_begin_cpu_access(buf->dmabuf, DMA_BIDIRECTIONAL);
	else
		comm_err("%s(): unsupport cache_op:%u\n", __func__, cache_op);

	return ret;
}

static int mdla_iommu_map_uva(struct device *dev, struct vm_area_struct *vma, struct buf_info *buf)
{
	u64 addr;
	unsigned long size;
	unsigned long attrs = 0;
	u32 offset;

	addr = (u64)(vma->vm_pgoff) << PAGE_SHIFT;
	size = vma->vm_end - vma->vm_start;

	offset = (addr & UINT_MAX) - (buf->iova & UINT_MAX);
	vma->vm_flags |= VM_IO | VM_PFNMAP | VM_DONTEXPAND | VM_DONTDUMP | VM_MIXEDMAP;

	vma->vm_pgoff = 0;
	if (buf->kva) {
#if 1
		comm_drv_debug("%s(): dma_buf_mmap(), vma start:%lx end:%lx pgoff:%lx\n", __func__,
			vma->vm_start, vma->vm_end, vma->vm_pgoff);
		return dma_buf_mmap(buf->dmabuf, vma, vma->vm_pgoff);
#else
		if (buf->cache == 0)
			attrs |= DMA_ATTR_WRITE_COMBINE;

		comm_drv_debug("%s(): dma_mmap_attrs(), vma start:%lx end:%lx pgoff:%lx\n", __func__,
			vma->vm_start, vma->vm_end, vma->vm_pgoff);
		return dma_mmap_attrs(dev, vma, (void *)buf->kva + offset,
					buf->iova + offset, size, attrs);
#endif
	} else {
		//non-valid mapping range
		comm_drv_debug("mdla iova map uva  mmap fail! addr %llx size %lu!\n", addr, size);
		return -EINVAL;
	}

}

static int mdla_iommu_alloc(struct device *dev, struct buf_info *buf)
{
	struct dma_buf *dmabuf;
	struct dma_heap *dmaheap;
	void *kva;
	u64 iova = 0;
	unsigned int iova_size;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
	struct dma_buf_map *map = NULL;
#endif
	int ret;
	struct dma_buf_attachment *att;
	struct sg_table *sgt = NULL;
	int fd;


#if IS_ENABLED(CONFIG_DMABUF_HEAPS_SYSTEM)
	dmaheap = (buf->cache == 0) ? dmaheap_uncached : dmaheap_cached;
	if (!dmaheap) {
		comm_err("%s(): dmaheap_%s is null\n", __func__,
			(buf->cache == 0) ? "uncached" : "cached");
		return -EFAULT;
	}

#ifdef MDLA_DMAHEAP_CLOSEFD_BY_USER
	fd = dma_heap_bufferfd_alloc(dmaheap, buf->size,
			DMA_HEAP_VALID_FD_FLAGS,
			DMA_HEAP_VALID_HEAP_FLAGS);
	if (fd < 0) {
		comm_err("%s(): alloc failed ret: %x\n", __func__, fd);
		goto err_out;

	}

	dmabuf = dma_buf_get(fd);
	if (IS_ERR_OR_NULL(dmabuf)) {
		comm_err("%s(): dma_buf_get failed ret: %ld\n", __func__,
				PTR_ERR(dmabuf));
		goto err_out;
	}
#else
	dmabuf = dma_heap_buffer_alloc(dmaheap, buf->size,
			DMA_HEAP_VALID_FD_FLAGS,
			DMA_HEAP_VALID_HEAP_FLAGS);
	if (IS_ERR_OR_NULL(dmabuf)) {
		comm_err("%s(): alloc dma_buf failed ret: %ld\n", __func__,
				PTR_ERR(dmabuf));
		goto err_out;
	}
#endif
	ret = mtkd_iommu_getiova_kernel(dmabuf, &iova, &iova_size);
	if (ret) {
		comm_err("%s(): mtkd_iommu_getiova failed\n", __func__);
		goto err_buf_put;
	}

	if (iova_size != buf->size) {
		//comm_err("%s(): iova_size:%x is not equal size:%x \n", __func__, iova_size, buf->size);
		//goto err_buf_put;
	}

#else	//CONFIG_DMABUF_HEAPS_SYSTEM
	dmabuf = ion_alloc(buf->size, 1 << heap_id, MDLA_BUFTAG_ATTR);
	if (IS_ERR(dmabuf)) {
		comm_err("%s(): ion_alloc size:%d heap:%x fail\n",
			__func__, buf->size, heap_id);
		return -ENOMEM;
	}

	fd = dma_buf_fd(dmabuf, O_CLOEXEC | O_ACCMODE);
	if (fd < 0) {
		comm_err("%s(): dma_buf_fd fail\n", __func__);
		dma_buf_put(dmabuf);
		return -EFAULT;
	}

	dma_buf_get(fd);
#endif	//CONFIG_DMABUF_HEAPS_SYSTEM

#if 1
	att = dma_buf_attach(dmabuf, dev);
	if (IS_ERR(att)) {
		goto err_buf_put;
	}

	sgt = dma_buf_map_attachment(att, DMA_BIDIRECTIONAL);
	if (IS_ERR_OR_NULL(sgt)) {
		comm_err("%s: get sgt failed\n", __func__);
		goto err_buf_put;
	}

	if (!iova) {
		iova = sg_dma_address(sgt->sgl);
		iova_size = sg_dma_len(sgt->sgl);
		if (iova < 0x200000000) {
			comm_err("%s: sg_dma_address fail, iova: %llx\n",
				__func__, iova);
			goto err_buf_put;
		}
	}
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
	map = kmalloc(sizeof(struct dma_buf_map), GFP_KERNEL);
	if (!map)
		goto err_buf_put;

	ret = dma_buf_vmap(dmabuf, &map);
	if (ret) {
#else
	kva = dma_buf_vmap(dmabuf);
	if (IS_ERR_OR_NULL(kva)) {
#endif
		//dma_buf_end_cpu_access(dmabuf, DMA_BIDIRECTIONAL);
		comm_err("%s(): dma_buf_vmap fail\n", __func__);
		goto err_map_free;
	}

	if (buf->cache) {
		ret = dma_buf_begin_cpu_access(dmabuf, DMA_BIDIRECTIONAL); // invalidate
		if (ret < 0) {
			comm_err("%s(): dma_buf_end_cpu_access fail\n", __func__);
			goto err_map_free;
		}
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
	kva = buf->map.vaddr;
	buf->db_map = (void *)map;
#endif
	buf->attach = att;
	buf->sgt = sgt;
	buf->kva =(uint64_t) kva;
	buf->iova = (uint64_t)iova;
	buf->iova_size = iova_size;
	buf->dmabuf = dmabuf;
	buf->fd = fd;

	return 0;

err_map_free:
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
	if (map)
		kfree(map);
#endif
err_buf_put:
	dma_buf_put(dmabuf);
err_out:

	return -EINVAL;
}

static int mdla_iommu_free(struct device *dev, struct buf_info *buf, int closefd)
{
	if (buf->cache) {
		dma_buf_end_cpu_access(buf->dmabuf, DMA_BIDIRECTIONAL); // invalidate
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,15,0)
	dma_buf_vunmap(buf->dmabuf, (struct dma_buf_map *)buf->db_map);
	if (buf->db_map)
		kfree(buf->db_map);
#else
	dma_buf_vunmap(buf->dmabuf, (void *)buf->kva);
#endif
	dma_buf_unmap_attachment(buf->attach, buf->sgt, DMA_BIDIRECTIONAL);
	dma_buf_detach(buf->dmabuf, buf->attach);
	dma_buf_put(buf->dmabuf);

	return 0;
}

static int mdla_iommu_auth(struct device *dev, struct buf_info *buf)
{
	int ret = 0;
	u32 pipelineID = 0;

	/* get device parameter */
	pipelineID = buf->pipe_id;
	if (pipelineID == 0xFFFFFFFF) {
	#ifdef ALLOC_PIPEID
		mtkd_iommu_allocpipelineid(&pipelineID);
		comm_drv_debug("Alloc pipelineID = %x\n", pipelineID);
		buf->pipe_id = pipelineID;
		if (pipelineID == 0xFFFFFFFF)
			return -EPERM;
	#else
		return -EPERM;
	#endif
	}

	/* authorize buf */
	ret = mtkd_iommu_buffer_authorize(MDLA_BUFTAG_ATTR,
					buf->iova, buf->size, pipelineID);
	if (ret) {
		comm_err("%s(): authorize fail, pipe:%d iova:%llx id:%d\n",
				__func__, pipelineID, buf->iova, buf->size);
		return -EPERM;
	}

	return ret;
}

static int mdla_iommu_init(struct device *dev)
{
#if IS_ENABLED(CONFIG_DMABUF_HEAPS_SYSTEM)
	dmaheap_uncached = dma_heap_find(MDLA_DMAHEAP_UNCACHED);
	if (!dmaheap_uncached) {
		comm_err("%s(): find %s fail\n", __func__, MDLA_DMAHEAP_UNCACHED);
		return -EFAULT;
	}

	dmaheap_cached = dma_heap_find(MDLA_DMAHEAP_CACHED);
	if (!dmaheap_cached) {
		comm_err("%s(): find %s fail\n", __func__, MDLA_DMAHEAP_CACHED);
		return -EFAULT;
	}

	comm_drv_debug("%s(): dmaheap init done.\n", __func__);
#else

	heap_id = _get_ion_heapid("ion_mtkdtv_iommu_heap");
	if (heap_id >> 5) {
		comm_err("%s: heap id must be 0 to 31\n", __func__);
		return -EINVAL;
	}

	comm_drv_debug("%s(): iommuheap init done.\n", __func__);
#endif

	return 0;
}

static void mdla_iommu_uninit(struct device *dev)
{
#if IS_ENABLED(CONFIG_DMABUF_HEAPS_SYSTEM)
	if (dmaheap_uncached)
		dma_heap_put(dmaheap_uncached);

	if (dmaheap_cached)
		dma_heap_put(dmaheap_cached);
#endif
}

static int mdla_iommu_map_iova(struct device *dev, struct buf_info *buf)
{
	int ret = 0;
	struct dma_buf_attachment *attach;
	struct sg_table *sgt;
	struct dma_buf *d = NULL;

	comm_err("%s(): dma_buf_get(%d)\n", __func__, buf->fd);
	d = dma_buf_get(buf->fd);

	if (IS_ERR_OR_NULL(d)) {
		ret = PTR_ERR(d);
		comm_err("dma_buf_get(%d) failed: %d\n", buf->fd, ret);
		return -EBADF;
	}

	attach = dma_buf_attach(d, dev);
	if (IS_ERR(attach)) {
		ret = PTR_ERR(attach);
		comm_err("dma_buf_attach failed: %d\n", ret);
		goto free_import;
	}

	sgt = dma_buf_map_attachment(attach, DMA_BIDIRECTIONAL);
	if (IS_ERR(sgt)) {
		ret = PTR_ERR(sgt);
		comm_err("dma_buf_map_attachment failed: %d\n", ret);
		goto free_attach;
	}

	ret = mtkd_iommu_getiova(buf->fd, &buf->iova, &buf->iova_size);
	if (ret) {
		comm_err("iommu_getiova failed: %d\n", ret);
		goto free_attach;
	}
	buf->attach = attach;
	buf->sgt = sgt;
	buf->dmabuf = d;
	comm_err("%s(): fd(%d) iova(%llx) size(%x)\n", __func__, buf->fd, buf->iova, buf->iova_size);

	return ret;

free_attach:
	dma_buf_detach(d, attach);
free_import:
	dma_buf_put(d);
	return ret;

}

static int mdla_iommu_unmap_iova(struct device *dev, struct buf_info *buf)
{
	dma_buf_unmap_attachment(buf->attach, buf->sgt, DMA_BIDIRECTIONAL);
	dma_buf_detach(buf->dmabuf, buf->attach);
	dma_buf_put(buf->dmabuf);
	buf->sgt = NULL;
	buf->attach = NULL;

	return 0;
}

static int mdla_iommu_map_kva(struct device *dev, struct buf_info *buf)
{
	void *buffer = NULL;
	struct dma_buf *d = NULL;
	int ret = 0;

	d = dma_buf_get(buf->fd);
	if (IS_ERR_OR_NULL(d)) {
		ret = PTR_ERR(d);
		comm_err("dma_buf_get(%d) failed: %d\n", buf->fd, ret);
		return -EBADF;
	}

	ret = dma_buf_begin_cpu_access(d, DMA_FROM_DEVICE);
	if (ret) {
		comm_err("dma_buf_begin_cpu_access fail(%p)\n", d);
		goto err;
	}
	/* map kernel va*/
	buffer = dma_buf_kmap(d, 0);
	if (IS_ERR_OR_NULL(buffer)) {
		comm_err("map kernel va fail(%p)\n", d);
		ret = -ENOMEM;
		goto err;
	}

	buf->dmabuf = d;
	buf->kva = (uint64_t)buffer;

err:
	if (ret)
		dma_buf_put(d);

	return ret;
}

static int mdla_iommu_unmap_kva(struct device *dev, struct buf_info *buf)
{
	int ret = 0;

	if (IS_ERR_OR_NULL(buf->dmabuf))
		return -EINVAL;

	dma_buf_kunmap(buf->dmabuf, 0, (void *)buf->kva);

	ret = dma_buf_end_cpu_access(buf->dmabuf, DMA_FROM_DEVICE);
	if (ret)
		comm_err("dma_buf_end_cpu_access fail(%p): %d\n",
			buf->dmabuf, ret);

	dma_buf_put(buf->dmabuf);

	return ret;
}

static struct plat_mem_callback mem_cb = {
	.alloc = mdla_iommu_alloc,
	.free = mdla_iommu_free,
	.auth = mdla_iommu_auth,
	.map_uva = mdla_iommu_map_uva,
	.cache_ctrl = mdla_iommu_cache_ctrl,
	.init = mdla_iommu_init,
	.uninit = mdla_iommu_uninit,
	.map_iova = mdla_iommu_map_iova,
	.unmap_iova = mdla_iommu_unmap_iova,
	.map_kva = mdla_iommu_map_kva,
	.unmap_kva = mdla_iommu_unmap_kva,
};

struct plat_mem_callback *dmabuf_cb = &mem_cb;
