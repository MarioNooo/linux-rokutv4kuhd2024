// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/iommu.h>
#include <linux/dma-buf.h>
#include <linux/genalloc.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/ion.h>
#include <linux/syscalls.h>

#ifndef DMX2LITE
#include "mtk_iommu_common.h"
#include "mtk_iommu_ion.h"
#include "mtk_iommu_dtv_api.h"
#include "mtk-reserved-memory.h"
#include "mtk-cma.h"
#endif

#include "mtk_dmx_dev.h"
#include "mtk_dmx_common.h"
#include "mtk_dmx_rm.h"
#ifndef DMX2LITE
#include "mtk_dmx_log.h"
#include "mtk_dmx_filter.h"
#include "mtk_mvd_parser.h"
#include "mtk_mad_parser.h"
#endif

#ifndef DMX2LITE
mtk_dmx_riu_bank *dmx_bank_offsets;

static int rm_hwcaps_type_2_filter_sub_type(
		enum hwcaps_type hwcaps_type,
		enum filter_sub_type *sub_type);
static int rm_get_filter_by_filter_sub_type(
		struct mtk_dmx_rm *res,
		enum filter_sub_type filter_sub_type,
		struct filter **filter);
static int rm_get_filter_by_hwcaps_type(
		struct mtk_dmx_rm *res,
		enum hwcaps_type hwcaps_type,
		struct filter **filter);

static int rm_filter_deinit(
		struct mtk_dmx_rm *res,
		enum hwcaps_type hwcaps_type);
static int rm_filter_init(
		struct mtk_dmx_rm *res,
		enum hwcaps_type hwcaps_type);

static int rm_alloc_pid_filter(struct mtk_dmx_rm *res, enum filter_sub_type sub);
static void rm_release_pid_filter(struct mtk_dmx_rm *res, u16 filter_id);
static int rm_alloc_pcr_filter(struct mtk_dmx_rm *res);
static void rm_release_pcr_filter(struct mtk_dmx_rm *res, u16 filter_id);

static int rm_alloc_filter(
		struct mtk_dmx_rm *res,
		enum filter_sub_type filter_sub_type,
		int *filter_id);
static int rm_release_filter(
		struct mtk_dmx_rm *res,
		enum filter_sub_type filter_sub_type,
		u16 filter_id);

static int rm_deinit(struct mtk_dmx_rm *res);

static int rm_padcfg_init(struct mtk_dmx_rm *res);
static int rm_memmap_init(struct mtk_dmx_rm *res);
static int rm_regmaps_init(struct mtk_dmx_rm *res);
static int rm_hwcaps_init(struct mtk_dmx_rm *res);
static int rm_fifos_init(struct mtk_dmx_rm *res);
static int rm_pvr_init(struct mtk_dmx_rm *res);
static int rm_stc_init(struct mtk_dmx_rm *res);
static int rm_filters_init(struct mtk_dmx_rm *res);

static int rm_hwcaps_type_2_filter_sub_type(
		enum hwcaps_type hwcaps_type,
		enum filter_sub_type *sub_type)
{
	int ret = 0;

	if (MTK_DMX_VERIFY(sub_type == NULL))
		return -EINVAL;

	switch (hwcaps_type) {
	case HWCAPS_TYPE_ALP_FILTER:
		*sub_type = FILTER_SUB_TYPE_ALP;
		break;

	default:
		MTK_DMX_ERR(
			"hwcaps_type(%s) is not supported.\n",
			hwcaps_type_2_str(hwcaps_type));
		*sub_type = FILTER_SUB_TYPE_MAX;
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int rm_get_filter_by_filter_sub_type(
		struct mtk_dmx_rm *res,
		enum filter_sub_type filter_sub_type,
		struct filter **filter)
{
	int ret = 0;

	if (MTK_DMX_VERIFY(res == NULL))
		return -EINVAL;
	if (MTK_DMX_VERIFY(filter == NULL))
		return -EINVAL;
	if (MTK_DMX_VERIFY(filter_sub_type < FILTER_SUB_TYPE_MIN))
		return -EINVAL;
	if (MTK_DMX_VERIFY(filter_sub_type >= FILTER_SUB_TYPE_MAX))
		return -EINVAL;

	*filter = &res->filters[filter_sub_type];

	return ret;
}

static int rm_get_filter_by_hwcaps_type(
		struct mtk_dmx_rm *res,
		enum hwcaps_type hwcaps_type,
		struct filter **filter)
{
	int ret = 0;
	enum filter_sub_type filter_sub_type = FILTER_SUB_TYPE_MAX;

	if (MTK_DMX_VERIFY(res == NULL))
		return -EINVAL;
	if (MTK_DMX_VERIFY(filter == NULL))
		return -EINVAL;
	ret = rm_hwcaps_type_2_filter_sub_type(hwcaps_type, &filter_sub_type);
	if (ret != 0) {
		MTK_DMX_ERR(
			"_hwcaps_type_2_filter_sub_type(%s, ...) is failed.\n",
			hwcaps_type_2_str(hwcaps_type));

		return -EINVAL;
	}

	ret = rm_get_filter_by_filter_sub_type(
			res,
			filter_sub_type,
			filter);
	if (ret != 0)
		MTK_DMX_ERR(
			"_get_filter_by_filter_sub_type(%s, ...) is failed.\n",
			filter_sub_type_2_str(filter_sub_type));

	return ret;
}

static int rm_filter_deinit(
		struct mtk_dmx_rm *res,
		enum hwcaps_type hwcaps_type)
{
	int ret = 0;
	struct filter *filter = NULL;

	MTK_DMX_ENTRY();

	if (MTK_DMX_VERIFY(res == NULL))
		return -EINVAL;

	ret = rm_get_filter_by_hwcaps_type(res, hwcaps_type, &filter);
	if (ret != 0) {
		MTK_DMX_ERR(
			"_get_filter_by_hwcaps_type(%d, ...) is failed.\n",
			hwcaps_type);

		return -EINVAL;
	}

	do {
		if (!filter->init) {
			// Deinit even though previous init is not completed.
			MTK_DMX_WARN("filter is not initialized.\n");
		}

		filter->filter_num = 0;
		MTK_DMX_KVFREE(filter->filter_slot_map);
		filter->init = false;
	} while (0);

	MTK_DMX_EXIT();

	return ret;
}

static int rm_filter_init(
		struct mtk_dmx_rm *res,
		enum hwcaps_type hwcaps_type)
{
	int ret = 0;
	struct filter *filter = NULL;
	struct mtk_dmx2dev *mtkdev = NULL;
	int i = 0;

	MTK_DMX_ENTRY();

	if (MTK_DMX_VERIFY(res == NULL))
		return -EINVAL;

	ret = rm_get_filter_by_hwcaps_type(res, hwcaps_type, &filter);
	if (ret != 0) {
		MTK_DMX_ERR(
			"_get_filter_by_hwcaps_type(%d, ...) is failed.\n",
			hwcaps_type);

		return -EINVAL;
	}
	mtkdev = container_of(res, struct mtk_dmx2dev, rm);

	do {
		if (filter->init) {
			MTK_DMX_WARN("filter is already initialized.\n");

			break;
		}

		filter->filter_num = mtk_dmx_rm_get_hwcap_by_id(res, hwcaps_type);
		if (filter->filter_num == 0) {
			MTK_DMX_ERR(
				"filter_num of hwcaps_type(%d) is 0.\n",
				hwcaps_type);
			ret = -EIO;

			break;
		}

		spin_lock_init(&filter->lock);

		filter->filter_slot_map =
			devm_kzalloc(
				mtkdev->dev,
				filter->filter_num * sizeof(struct filter_status),
				GFP_KERNEL);
		if (filter->filter_slot_map == NULL) {
			MTK_DMX_ERR(
				"Allocate filter_slot_map of hwcaps_type(%d) is failed.\n",
				hwcaps_type);
			ret = -EIO;

			break;
		}
		for (i = 0; i < filter->filter_num; i++)
			filter->filter_slot_map[i].state = RES_STATE_FREE;

		filter->init = true;
	} while (0);

	MTK_DMX_EXIT();

	return ret;
}

static int rm_alloc_pid_filter(struct mtk_dmx_rm *res, enum filter_sub_type sub)
{
	u16 pid_filter_num, sec_filter_num;
	int i;
	unsigned long flags;
	bool bInverse = true;

	if (sub == FILTER_SUB_TYPE_SEC)
		bInverse = false;

	pid_filter_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_PID_FILTER);
	sec_filter_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_SEC_FILTER);

	spin_lock_irqsave(&res->pid_filter_lock, flags);

	if (bInverse) {
		for (i = pid_filter_num - 1; i >= 0; i--) {
			if (i == SECFLT_NMASK_FLD_ID)
				continue;
			if (res->pid_filter_slot_map[i].state ==
				RES_STATE_FREE) {
				res->pid_filter_slot_map[i].state =
					RES_STATE_ALLOC;
				spin_unlock_irqrestore(&res->pid_filter_lock, flags);
				return i;
			}
		}
	} else {
		for (i = 0; i < sec_filter_num; i++) {
			if (i == SECFLT_NMASK_FLD_ID)
				continue;
			if (res->pid_filter_slot_map[i].state ==
				RES_STATE_FREE) {
				res->pid_filter_slot_map[i].state =
					RES_STATE_ALLOC;
				spin_unlock_irqrestore(&res->pid_filter_lock, flags);
				return i;
			}
		}
	}

	spin_unlock_irqrestore(&res->pid_filter_lock, flags);
	return -ENODEV;
}

static void rm_release_pid_filter(struct mtk_dmx_rm *res, u16 filter_id)
{
	unsigned long flags;

	spin_lock_irqsave(&res->pid_filter_lock, flags);

	res->pid_filter_slot_map[filter_id].state = RES_STATE_FREE;

	spin_unlock_irqrestore(&res->pid_filter_lock, flags);
}

static int rm_alloc_pcr_filter(struct mtk_dmx_rm *res)
{
	int i;
	unsigned long flags;
	u16 pcr_filter_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_PCR_FILTER);

	spin_lock_irqsave(&res->pcr_filter_lock, flags);

	for (i = 0; i < pcr_filter_num; i++) {
		if (res->pcr_filter_slot_map[i].state ==
			RES_STATE_FREE) {
			res->pcr_filter_slot_map[i].state =
				RES_STATE_ALLOC;
			spin_unlock_irqrestore(&res->pcr_filter_lock, flags);
			return i;
		}
	}
	spin_unlock_irqrestore(&res->pcr_filter_lock, flags);
	return -ENODEV;
}

static void rm_release_pcr_filter(struct mtk_dmx_rm *res, u16 filter_id)
{
	unsigned long flags;

	spin_lock_irqsave(&res->pcr_filter_lock, flags);

	res->pcr_filter_slot_map[filter_id].state = RES_STATE_FREE;

	spin_unlock_irqrestore(&res->pcr_filter_lock, flags);
}

static int rm_alloc_ip_filter(struct mtk_dmx_rm *res)
{
	u16 ip_filter_num;
	int i;
	unsigned long flags;

	ip_filter_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_IP_FILTER);

	spin_lock_irqsave(&res->ip_filter_lock, flags);

	for (i = 0; i < ip_filter_num; i++) {
		if (res->ip_filter_slot_map[i].state == RES_STATE_FREE) {
			res->ip_filter_slot_map[i].state = RES_STATE_ALLOC;
			spin_unlock_irqrestore(&res->ip_filter_lock, flags);
			return i;
		}
	}

	spin_unlock_irqrestore(&res->ip_filter_lock, flags);
	return -ENODEV;
}

static void rm_release_ip_filter(struct mtk_dmx_rm *res, u16 filter_id)
{
	unsigned long flags;

	spin_lock_irqsave(&res->ip_filter_lock, flags);

	res->ip_filter_slot_map[filter_id].state = RES_STATE_FREE;

	spin_unlock_irqrestore(&res->ip_filter_lock, flags);
}

static int rm_alloc_filter(
		struct mtk_dmx_rm *res,
		enum filter_sub_type filter_sub_type,
		int *filter_id)
{
	int ret = 0;
	struct filter *filter = NULL;
	int i;
	unsigned long flags;

	MTK_DMX_ENTRY();

	if (MTK_DMX_VERIFY(res == NULL))
		return -EINVAL;
	if (MTK_DMX_VERIFY(filter_id == NULL))
		return -EINVAL;

	ret = rm_get_filter_by_filter_sub_type(
			res,
			filter_sub_type,
			&filter);
	if (ret != 0) {
		MTK_DMX_ERR(
			"_get_filter_by_filter_sub_type(..., %d, ...) is failed.\n",
			filter_sub_type);

		return -EINVAL;
	}

	spin_lock_irqsave(&filter->lock, flags);

	do {
		if (MTK_DMX_VERIFY(!filter->init)) {
			ret = -EIO;

			break;
		}

		for (i = 0; i < filter->filter_num; i++) {
			if (filter->filter_slot_map[i].state == RES_STATE_FREE) {
				filter->filter_slot_map[i].state = RES_STATE_ALLOC;
				*filter_id = i;

				break;
			}
		}
	} while (0);

	spin_unlock_irqrestore(&filter->lock, flags);

	return ret;
}

static int rm_release_filter(
		struct mtk_dmx_rm *res,
		enum filter_sub_type filter_sub_type,
		u16 filter_id)
{
	int ret = 0;
	struct filter *filter = NULL;
	unsigned long flags;

	MTK_DMX_ENTRY();

	if (MTK_DMX_VERIFY(res == NULL))
		return -EINVAL;

	ret = rm_get_filter_by_filter_sub_type(
			res,
			filter_sub_type,
			&filter);
	if (ret != 0) {
		MTK_DMX_ERR(
			"_get_filter_by_filter_sub_type(..., %d, ...) is failed.\n",
			filter_sub_type);

		return -EINVAL;
	}

	spin_lock_irqsave(&filter->lock, flags);

	do {
		// Return 0 even though it is not initialized.
		if (MTK_DMX_VERIFY(!filter->init))
			break;

		if (filter_id >= filter->filter_num) {
			MTK_DMX_WARN(
				"filter_id(%d) >= filter_num(%d)\n",
				filter_id,
				filter->filter_num);

			ret = -EINVAL;
			break;
		}

		filter->filter_slot_map[filter_id].state = RES_STATE_FREE;

	} while (0);

	spin_unlock_irqrestore(&filter->lock, flags);

	MTK_DMX_EXIT();

	return ret;
}

static int rm_deinit(struct mtk_dmx_rm *res)
{
	int ret = 0;
	int tmp_ret = 0;
	int i = 0;

	tmp_ret = rm_filter_deinit(res, HWCAPS_TYPE_ALP_FILTER);
	if (tmp_ret != 0) {
		MTK_DMX_ERR(
			"_filter_deinit(..., %d) is failed.\n",
			HWCAPS_TYPE_ALP_FILTER);

		ret = -EIO;
	}

	// TODO: Release other resources.

	return ret;
}

const struct dmx_memmap_tag dmx_mmap[MMAP_BUF_TYPE_MAX] = { // TODO: DTS?
	{"MI_TSP_FW_BUF", FW_MMAP_ID, "tsp_fw"},
	{"MI_TSP_VQ_BUF", VQ_MMAP_ID, "tsp_vq"},
	{"MI_TSP_SECT_BUF", SEC_MMAP_ID, "tsp_sect"},
	{"MI_PVR_DOWNLOAD_BUF", PVR_BUF_ID, "tsp_pvr"},
	{"MI_PVR_UPLOAD_BUF", MMFI_BUF_ID, "tsp_filein"},
	{"MI_TSP_FQ_BUF", FQ_BUF_ID, "tsp_fiq"},
	{"MI_TSP_SVQ_BUF", TSO_SVQ_MMAP_ID, "tsp_svq"},
	{NULL, AESDMA_BUF_ID, "shm_buf_aesdma"}
};

static void rm_memmap_get(enum mmap_buf_type idx, struct platform_device *pdev,
	struct mem_info *pInfo)
{
#define ADDR_CELLS (2)
#define SIZE_CELLS (2)
	enum mmap_heap_type type = HEAP_NONE;
	struct device_node *mmap_np = NULL, *buftag_np = NULL, *np = NULL, *node = NULL;
	__be32 *p32 = NULL;
	__be64 *p64 = NULL;
	uint32_t len = 0;
	int cma_id = 0, id = 0;
	u64 ba = 0, pa = 0;
	u32 size = 0;
	unsigned long cma_base = 0, cma_size = 0;

	mmap_np = of_find_node_by_name(NULL, "mmap_info");
	buftag_np = of_find_node_by_name(NULL, "buf_tag");
	if (mmap_np == NULL || buftag_np == NULL)
		goto FAIL;
	if (dmx_mmap[idx].mmap)
		np = of_find_node_by_name(mmap_np, dmx_mmap[idx].mmap);
	if (np == NULL) { // IOMMU
		np = of_find_node_by_name(buftag_np, dmx_mmap[idx].buftag);
		if (np == NULL)
			goto FAIL;
		p64 = (__be64 *) of_get_property(np, "max_size", &len);
		if (p64 == NULL)
			goto FAIL;
		size = be64_to_cpup(p64);

		type = HEAP_IOMMU;
	} else { // CMA or NON_MMA
		p32 = (__be32 *)of_get_property(np, "cma_id", &len);
		if (p32 == NULL)
			goto FAIL;
		cma_id = be32_to_cpup(p32);

		p32 = (__be32 *)of_get_property(np, "reg", &len);
		if (p32 == NULL ||
			((len / sizeof(__be32)) < (ADDR_CELLS + SIZE_CELLS)))
			goto FAIL;
		ba = of_read_number(p32, ADDR_CELLS);
		size = of_read_number(p32 + ADDR_CELLS, SIZE_CELLS);
		pa = BA_to_PA(ba);

		if (cma_id != 0) {
			type = HEAP_CMA;

			p32 = (__be32 *) of_get_property(pdev->dev.of_node, "cmas", &len);
			if (p32 == NULL)
				goto FAIL;
			id = be32_to_cpup(p32);
			node = of_parse_phandle(pdev->dev.of_node, "memory-region", id);
			p32 = (__be32 *) of_get_property(node, "reg", &len);
			if (p32 == NULL ||
				((len / sizeof(__be32)) < (ADDR_CELLS + SIZE_CELLS)))
				goto FAIL;
			cma_base = of_read_number(p32, ADDR_CELLS);
			cma_size = of_read_number(p32 + ADDR_CELLS, SIZE_CELLS);
		} else
			type = HEAP_NON_MMA;
	}

FAIL:
	if (mmap_np)
		of_node_put(mmap_np);
	if (buftag_np)
		of_node_put(buftag_np);
	if (np)
		of_node_put(np);

	pInfo->paddr = pa;
	pInfo->size = size;
	pInfo->heaptype = type;
	pInfo->id = (type == HEAP_IOMMU) ? dmx_mmap[idx].buftag_id : cma_id;
	pInfo->cma_base = cma_base;
	pInfo->cma_size = cma_size;
	MTK_DMX_INFO("pa:0x%llx, size:0x%x, type:%s, id:%d, cma:0x%x, size:0x%x\n",
		pInfo->paddr, pInfo->size,
		(pInfo->heaptype == HEAP_IOMMU) ? "IOMMU" :
		(pInfo->heaptype == HEAP_CMA) ? "CMA" : "NON_MMA",
		pInfo->id, pInfo->cma_base, pInfo->cma_size);
}

static int rm_query_heap_id(bool iommu)
{
	int i;
	size_t nr_heaps;
	struct ion_heap_data *data;
	int heap_id = -ENODEV;
	char* heap_name;

	nr_heaps = ion_query_heaps_kernel(NULL, 0);
	if (nr_heaps == 0)
		return -ENODEV;
	data = kcalloc(nr_heaps, sizeof(struct ion_heap_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	if (ion_query_heaps_kernel(data, nr_heaps) == 0) {
		kfree(data);
		MTK_DMX_ERR("query heaps failed\n" , __func__, __LINE__);
		return -ENODEV;
	}

	heap_name = (iommu) ? "ion_mtkdtv_iommu_heap" : "mtk-demux";

	for (i = 0; i < nr_heaps; i++)
		if (strstr(data[i].name, heap_name)) {
			heap_id = data[i].heap_id;
			break;
		}

	kfree(data);

	return heap_id;
}

static int rm_dma_alloc(int size, int heap, int flags,
	struct dma_buf **db, int *fd, void __iomem **va)
{
	*db = ion_alloc(size, heap, flags);
	if (IS_ERR(*db)) {
		MTK_DMX_ERR("ion_alloc size 0x%x bytes failed\n", size);
		return -ENOMEM;
	}
	*fd = dma_buf_fd(*db, O_CLOEXEC);
	if (*fd < 0) {
		MTK_DMX_ERR("get dma fd failed\n");
		dma_buf_put(*db);
		return -EFAULT;
	}
	dma_buf_get(*fd);
	*va = dma_buf_vmap(*db);
	if (IS_ERR_OR_NULL(*va)) {
		MTK_DMX_ERR("dma_buf_vmap failed\n");
		dma_buf_put(*db);
		ksys_close(*fd);
		return -EFAULT;
	}
	return 0;
}

static inline void rm_dma_free(struct dma_buf *db, void __iomem *va)
{
	dma_buf_vunmap(db, va);
	dma_buf_put(db);
}

static int rm_mma_get_memory(bool iommu, struct mem_info *pMemmapInfo,
				struct mem_info *pInfo, u32 buf_size)
{
	unsigned int req_size;
	int ret;
	int heap_id, fd, get_size;
	struct dma_buf *db;
	void __iomem *va = NULL;
	dma_addr_t iova;

	MTK_DMX_INFO("query %s id: %d size: 0x%x\n",
		(iommu) ? "buftag" : "cma",
		pMemmapInfo->id, buf_size);

	req_size = (buf_size > 0) ? buf_size : pMemmapInfo->size;
	if (iommu)
		req_size = ALIGN(req_size, DMX_4K_ALIGN);

	heap_id = rm_query_heap_id(iommu);
	if (heap_id < 0) {
		MTK_DMX_ERR("query heap fail\n");
		return -EFAULT;
	}
	ret = rm_dma_alloc(req_size, 1 << heap_id,
		(iommu) ? (pMemmapInfo->id << BUFTAG_SHIFT) :
		(PA_to_BA(pInfo->paddr) - pMemmapInfo->cma_base),
		&db, &fd, &va);
	if (ret) {
		MTK_DMX_ERR("dma alloc fail\n");
		return ret;
	}
	if (iommu) {
		ret = mtkd_iommu_getiova(fd, &iova, &get_size);
		if (ret || get_size != req_size) {
			MTK_DMX_ERR("get iova failed\n");
			ksys_close(fd);
			rm_dma_free(db, va);
			return -EFAULT;
		}
	}
	ksys_close(fd);

	pInfo->id = pMemmapInfo->id;
	if (iommu)
		pInfo->paddr = iova;
	pInfo->size = req_size;
	pInfo->vaddr = va;
	pInfo->db = db;
	pInfo->dmabuf_fd = -1;
	pInfo->heaptype = (iommu) ? HEAP_IOMMU : HEAP_CMA;

	MTK_DMX_INFO("%s id: %d paddr: %llx, vaddr: %p, size: %x\n",
		(iommu) ? "buftag" : "cma",
		pInfo->id, pInfo->paddr, pInfo->vaddr, pInfo->size);

	return 0;
}

static inline void rm_mma_put_memory(struct mem_info *pInfo)
{
	rm_dma_free(pInfo->db, pInfo->vaddr);

	MTK_DMX_INFO("%s id: %d paddr: %llx, vaddr: %p, size: %x\n",
		(pInfo->heaptype == HEAP_IOMMU) ? "buftag" : "cma",
		pInfo->id, pInfo->paddr, pInfo->vaddr, pInfo->size);
}

static int rm_mmap_get_memory(struct device *pdev, enum mmap_buf_type buf_type,
				struct mem_info *pInfo)
{
	struct of_mmap_info_data of_mmap_info = { };
	int ret, id;
	char *buf_tag = NULL;

	id = dmx_mmap[buf_type].buftag_id;
	buf_tag = dmx_mmap[buf_type].buftag;

	ret = of_mtk_get_reserved_memory_info_by_idx(pdev->of_node, id, &of_mmap_info);
	if (ret < 0) {
		MTK_DMX_ERR("get reserved mem info fail ret: %d\n", ret);
		return -EFAULT;
	}

	pInfo[0].id = id;
	pInfo[0].paddr = BA_to_PA(of_mmap_info.start_bus_address);
	pInfo[0].size = (u32)of_mmap_info.buffer_size;
	pInfo[0].dmabuf_fd = -1;
	pInfo[0].heaptype = HEAP_NON_MMA;

	MTK_DMX_INFO("[%s] pa = %llx, size = %u\n", buf_tag, pInfo[0].paddr, pInfo[0].size);

	if (!devm_request_mem_region(pdev, PA_to_BA(pInfo[0].paddr),
			pInfo[0].size, DRV2_NAME)) {
		MTK_DMX_ERR("could not reserve memory\n");
		return -ENODEV;
	}

	pInfo[0].vaddr =
		devm_ioremap_wc(pdev, PA_to_BA(pInfo[0].paddr), pInfo[0].size);
	if (!pInfo[0].vaddr) {
		MTK_DMX_ERR("ioremap fail\n");
		return -ENODEV;
	}

	return 0;
}

static inline void rm_mmap_put_memory(struct device *pdev, struct mem_info *pInfo)
{
	devm_iounmap(pdev, pInfo[0].vaddr);
	devm_release_mem_region(pdev, PA_to_BA(pInfo[0].paddr), pInfo[0].size);
}

static int rm_pool_get_memory(struct device *pdev, struct mtk_dmx_rm *res,
	enum mmap_buf_type buf_type, struct mem_info *pInfo, u32 buf_size)
{
	dma_addr_t dma = { 0 };
	struct gen_pool *pool = NULL;
	void *vaddr = NULL;
	int id;
	char *name = NULL;
	enum mmap_heap_type heaptype = res->memmap->list[buf_type]->heaptype;

	name = dmx_mmap[buf_type].mmap;
	id = res->memmap->list[buf_type]->id;

	pool = gen_pool_get(pdev, name);
	if (!pool) {
		MTK_DMX_ERR("gen pool fail\n");
		return -1;
	}

	vaddr = gen_pool_dma_alloc(pool, buf_size, &dma);
	if (!vaddr && (heaptype == HEAP_NON_MMA)) {
		MTK_DMX_ERR("allocate memory by %s pool fail\n", name);
		return -1;
	}

	pInfo->id = id;
	if (heaptype == HEAP_NON_MMA)
		pInfo->paddr = dma;
	else if (heaptype == HEAP_CMA) {
		pInfo->offset = (u64)vaddr;
		pInfo->paddr = res->memmap->list[buf_type]->paddr + pInfo->offset;
	}
	pInfo->size = buf_size;
	if (heaptype == HEAP_NON_MMA)
		pInfo->vaddr = vaddr;
	pInfo->dmabuf_fd = -1;
	pInfo->heaptype = (heaptype == HEAP_NON_MMA) ? HEAP_NON_MMA :
		HEAP_CMA;
	pInfo->pool_name = name;

	MTK_DMX_INFO("[id: %d] paddr: %llx, vaddr: %p, size: %x, offset: %llx\n",
		pInfo->id, pInfo->paddr, pInfo->vaddr, pInfo->size, pInfo->offset);
	return 0;
}

static inline void rm_pool_put_memory(struct device *pdev, struct mem_info *pInfo)
{
	struct gen_pool *pool = NULL;
	char *name = NULL;

	pool = gen_pool_get(pdev, pInfo->pool_name);
	if (!pool) {
		MTK_DMX_ERR("gen pool fail\n");
		return;
	}

	if (pInfo->heaptype == HEAP_NON_MMA)
		gen_pool_free(pool, static_cast(unsigned long, pInfo->vaddr), pInfo->size);
	else if (pInfo->heaptype == HEAP_CMA)
		gen_pool_free(pool, static_cast(unsigned long, pInfo->offset), pInfo->size);

	MTK_DMX_INFO("[id: %d] paddr: %llx, vaddr: %p, size: %x\n, offset: %llx",
		pInfo->id, pInfo->paddr, pInfo->vaddr, pInfo->size, pInfo->offset);
}

static int rm_parse_banks(
	struct platform_device *pdev, unsigned int idx, u32 bank_num, REG16 __iomem ***pppBank)
{
	unsigned int i;
	struct resource *res;
	REG16 __iomem **ppBank;
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;

	if (pdev == NULL) {
		MTK_DMX_ERR("pdev is NULL\n");
		return -EINVAL;
	}
	if (pppBank == NULL) {
		MTK_DMX_ERR("pppBank is NULL\n");
		return -EINVAL;
	}

	ppBank =
		devm_kzalloc(&pdev->dev, bank_num * sizeof(REG16 *), GFP_KERNEL);
	if (!ppBank) {
		MTK_DMX_ERR("allocate bank array fail\n");
		return -ENOMEM;
	}
	*pppBank = ppBank;

	for (i = 0; i < bank_num; i++) {
		res = platform_get_resource(pdev, IORESOURCE_MEM, idx);
		if (!res) {
			MTK_DMX_ERR("failed to get #%u address\n", i);
			return -ENOENT;
		}

		if ((idx == MAD0_REG_OFFSET) || (idx == MAD1_REG_OFFSET)) {
			if (!node)
				return -ENODEV;

			ppBank[i] = of_iomap(node, idx);
		} else
			ppBank[i] = devm_ioremap_resource(&pdev->dev, res);

		if (IS_ERR((const void *)ppBank[i])) {
			MTK_DMX_ERR("failed to ioremap #%u address\n", i);
			return PTR_ERR((const void *)ppBank[i]);
		}
		idx++;
	}

	return 0;
}
#endif

static inline int rm_of_read(int width, const struct device_node *np,
			const char *propname, void *value)
{
	int ret = -1;

	if (width == sizeof(u16))
		ret = of_property_read_u16(np, propname, (u16 *)value);
	else if (width == sizeof(u32))
		ret = of_property_read_u32(np, propname, (u32 *)value);
	if (ret) {
		MTK_DMX_ERR("Failed to get %s\n", propname);
		return -1;
	}
	return 0;
}

#ifndef DMX2LITE
static int rm_pool_create(struct device *dev, enum mmap_buf_type type,
	struct mem_info *info)
{
	struct mem_info data = { 0 };

	switch (info->heaptype) {
	case HEAP_NON_MMA:
		if (rm_mmap_get_memory(dev, type, &data) < 0) {
			MTK_DMX_ERR("allocate memory fail\n");
			return -ENOMEM;
		}
		break;
	case HEAP_CMA:
		data.vaddr = 0;
		data.paddr = info->paddr;
		data.size = info->size;
		break;
	default:
		return 0;
	}
	info->pool = devm_gen_pool_create(dev, 4, -1, dmx_mmap[type].mmap);
	if (gen_pool_add_owner(
		info->pool,
		static_cast(unsigned long, data.vaddr),
		data.paddr,
		data.size,
		-1,
		NULL) < 0) {
		MTK_DMX_ERR("gen pool fail\n");
		return -ENOMEM;
	}
	return 0;
}

static int rm_memmap_init(struct mtk_dmx_rm *res)
{
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	struct platform_device *pdev = to_platform_device(mtkdev->dev);
	s32 i = 0;
	u64 dma_mask;

	res->memmap = devm_kzalloc(mtkdev->dev, sizeof(struct dmx_memmap), GFP_KERNEL);
	if (!res->memmap) {
		MTK_DMX_ERR("allocate memmap struct fail\n");
		return -ENOMEM;
	}

	for (i = 0 ; i < MMAP_BUF_TYPE_MAX ; i++) {
		res->memmap->list[i] = devm_kzalloc(mtkdev->dev, sizeof(struct mem_info),
							GFP_KERNEL);
		if (!res->memmap->list[i]) {
			MTK_DMX_ERR(
				"allocate index %d mem_info of memmap fail\n",
				i);
			return -ENOMEM;
		}
		rm_memmap_get(i, pdev, res->memmap->list[i]);
		// bypass SVQ
		if (i == MMAP_BUF_TYPE_TSO_SVQ) {
			MTK_DMX_INFO("currently bypass %s\n", dmx_mmap[i].buftag);
			continue;
		}
		if (rm_pool_create(mtkdev->dev, i, res->memmap->list[i]) < 0)
			MTK_DMX_ERR("fail to create mem pool\n");
	}

	/* map FIQ at driver initial */
	if (mtk_dmx_rm_get_buf(res, MMAP_BUF_TYPE_FQ, 0, &res->fiqmap) < 0) {
		MTK_DMX_ERR("get FQ buf fail\n");
		return -ENOMEM;
	}

	dma_mask = DMA_BIT_MASK(VPAS_BIT_MASK);
	if (dma_set_mask(&pdev->dev, dma_mask) ||
		dma_set_coherent_mask(&pdev->dev, dma_mask)) {
		MTK_DMX_ERR("[vpas] cannot accept dma mask 0x%llx\n", dma_mask);
		return -EOPNOTSUPP;
	}

	return 0;
}

static int rm_memmap_exit(struct mtk_dmx_rm *res)
{
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	struct platform_device *pdev = to_platform_device(mtkdev->dev);

	mtk_dmx_rm_put_buf(res, res->memmap->list[MMAP_BUF_TYPE_FQ]);

	iommu_fwspec_free(&pdev->dev);

	return 0;
}

static int rm_regmaps_init(struct mtk_dmx_rm *res)
{
	s32 ret = 0;
	u32 idx;
	struct device_node *bank_node;
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	struct platform_device *pdev = to_platform_device(mtkdev->dev);

	res->regmap = devm_kzalloc(mtkdev->dev, sizeof(struct dmx_regmap), GFP_KERNEL);
	if (!res->regmap) {
		MTK_DMX_ERR("allocate regmap struct fail\n");
		return -ENOMEM;
	}

	memset(&res->regmap->banks[0], 0x0, sizeof(mtk_dmx_riu_bank)*RIU_BANK_TYPE_MAX);

	of_node_get(pdev->dev.of_node);
	bank_node = of_find_node_by_name(pdev->dev.of_node, "banks");
	if (bank_node == NULL) {
		MTK_DMX_ERR("Failed to get banks node\n");
		return -ENOENT;
	}

	ret |= rm_of_read(sizeof(u32), bank_node,
		"tsp_bank_num", &res->regmap->banks[RIU_BANK_TYPE_TS].bank_num);
	ret |= rm_of_read(sizeof(u32), bank_node,
		"tso_bank_num", &res->regmap->banks[RIU_BANK_TYPE_TS_OUT].bank_num);
	ret |= rm_of_read(sizeof(u32), bank_node,
		"video_parser_bank_num", &res->regmap->banks[RIU_BANK_TYPE_MVD_PARSER].bank_num);
	ret |= rm_of_read(sizeof(u32), bank_node,
		"ts_sample_bank_num", &res->regmap->banks[RIU_BANK_TYPE_TS_SAMPLE].bank_num);
	ret |= rm_of_read(sizeof(u32), bank_node,
		"cilink_bank_num", &res->regmap->banks[RIU_BANK_TYPE_CILINK].bank_num);
	ret |= rm_of_read(sizeof(u32), bank_node,
		"tlv_bank_num", &res->regmap->banks[RIU_BANK_TYPE_TLV].bank_num);
	ret |= rm_of_read(sizeof(u32), bank_node,
		"alp_bank_num", &res->regmap->banks[RIU_BANK_TYPE_ALP].bank_num);
	ret |= rm_of_read(sizeof(u32), bank_node,
		"mad_parser_bank_num", &res->regmap->banks[RIU_BANK_TYPE_MAD_PARSER].bank_num);
	of_node_put(bank_node);
	if (ret != 0) {
		MTK_DMX_ERR("Get RIU bank number fail\n");
		return -ENOENT;
	}

	// tsp banks
	idx = 0;
	ret = rm_parse_banks(pdev, idx,
			res->regmap->banks[RIU_BANK_TYPE_TS].bank_num,
			&res->regmap->banks[RIU_BANK_TYPE_TS].ptr);
	if (ret != 0) {
		MTK_DMX_ERR("failed to get tsp addresses\n");
		return ret;
	}

	// tso banks
	idx += res->regmap->banks[RIU_BANK_TYPE_TS].bank_num;
	ret = rm_parse_banks(pdev, idx,
			res->regmap->banks[RIU_BANK_TYPE_TS_OUT].bank_num,
			&res->regmap->banks[RIU_BANK_TYPE_TS_OUT].ptr);
	if (ret != 0) {
		MTK_DMX_ERR("failed to get tso addresses\n");
		return ret;
	}

	// video parser_sample
	idx += res->regmap->banks[RIU_BANK_TYPE_TS_OUT].bank_num;
	ret = rm_parse_banks(pdev, idx,
			res->regmap->banks[RIU_BANK_TYPE_MVD_PARSER].bank_num,
			&res->regmap->banks[RIU_BANK_TYPE_MVD_PARSER].ptr);
	if (ret != 0) {
		MTK_DMX_ERR("failed to get video parser_sample addresses\n");
		return ret;
	}

	// tlv banks
	idx += res->regmap->banks[RIU_BANK_TYPE_MVD_PARSER].bank_num;
	ret = rm_parse_banks(pdev, idx,
			res->regmap->banks[RIU_BANK_TYPE_TLV].bank_num,
			&res->regmap->banks[RIU_BANK_TYPE_TLV].ptr);
	if (ret != 0) {
		MTK_DMX_ERR("failed to get tlv addresses\n");
		return ret;
	}

	// ts_sample
	idx += res->regmap->banks[RIU_BANK_TYPE_TLV].bank_num;
	ret = rm_parse_banks(pdev, idx,
			res->regmap->banks[RIU_BANK_TYPE_TS_SAMPLE].bank_num,
			&res->regmap->banks[RIU_BANK_TYPE_TS_SAMPLE].ptr);
	if (ret != 0) {
		MTK_DMX_ERR("failed to get ts_sample bank addr\n");
		return ret;
	}

	// cilink_soc_if
	if (res->regmap->banks[RIU_BANK_TYPE_CILINK].bank_num != 0) {
		idx += res->regmap->banks[RIU_BANK_TYPE_TS_SAMPLE].bank_num;
		ret = rm_parse_banks(pdev, idx,
				res->regmap->banks[RIU_BANK_TYPE_CILINK].bank_num,
				&res->regmap->banks[RIU_BANK_TYPE_CILINK].ptr);
		if (ret != 0) {
			MTK_DMX_ERR("failed to get ts_sample bank addr\n");
			return ret;
		}
	}

	// alp banks
	idx += res->regmap->banks[RIU_BANK_TYPE_CILINK].bank_num;
	ret = rm_parse_banks(pdev, idx,
			res->regmap->banks[RIU_BANK_TYPE_ALP].bank_num,
			&res->regmap->banks[RIU_BANK_TYPE_ALP].ptr);
	if (ret != 0) {
		MTK_DMX_ERR("failed to get alp addresses\n");
		return ret;
	}

	// mad banks
	idx += res->regmap->banks[RIU_BANK_TYPE_ALP].bank_num;
	ret = rm_parse_banks(pdev, idx,
			res->regmap->banks[RIU_BANK_TYPE_MAD_PARSER].bank_num,
			&res->regmap->banks[RIU_BANK_TYPE_MAD_PARSER].ptr);
	if (ret != 0) {
		MTK_DMX_ERR("failed to get mad addresses\n");
		return ret;
	}

	return 0;
}
#endif

static int rm_hwcaps_init(struct mtk_dmx_rm *res)
{
	int ret = 0;
	struct device_node *np;
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	struct platform_device *pdev = to_platform_device(mtkdev->dev);

	res->hwcap = devm_kzalloc(mtkdev->dev, sizeof(struct dmx_hwcaps), GFP_KERNEL);
	if (!res->hwcap) {
		MTK_DMX_ERR("allocate hwcaps struct fail\n");
		return -ENOMEM;
	}

	of_node_get(pdev->dev.of_node);
	np = of_find_node_by_name(pdev->dev.of_node, "hwcaps");
	if (np == NULL) {
		MTK_DMX_ERR("Failed to get hwcaps node\n");
		return -ENOENT;
	}

	ret |= rm_of_read(sizeof(u32), np,
		"pid_filter_num", &res->hwcap->entries[HWCAPS_TYPE_PID_FILTER].num);
	ret |= rm_of_read(sizeof(u32), np,
		"sec_filter_num", &res->hwcap->entries[HWCAPS_TYPE_SEC_FILTER].num);
	ret |= rm_of_read(sizeof(u32), np,
		"pcr_filter_num", &res->hwcap->entries[HWCAPS_TYPE_PCR_FILTER].num);
	ret |= rm_of_read(sizeof(u32), np,
		"mmfi_filter_num", &res->hwcap->entries[HWCAPS_TYPE_MMFI_FILTER].num);
	ret |= rm_of_read(sizeof(u32), np,
		"ip_filter_num", &res->hwcap->entries[HWCAPS_TYPE_IP_FILTER].num);
	ret |= rm_of_read(sizeof(u32), np,
		"ntp_filter_num", &res->hwcap->entries[HWCAPS_TYPE_NTP_FILTER].num);
	ret |= rm_of_read(sizeof(u32), np,
		"alp_filter_num", &res->hwcap->entries[HWCAPS_TYPE_ALP_FILTER].num);
	ret |= rm_of_read(sizeof(u32), np,
		"stc_num", &res->hwcap->entries[HWCAPS_TYPE_STC].num);
	ret |= rm_of_read(sizeof(u32), np,
		"vfifo_num", &res->hwcap->entries[HWCAPS_TYPE_VFIFO].num);
	ret |= rm_of_read(sizeof(u32), np,
		"afifo_num", &res->hwcap->entries[HWCAPS_TYPE_AFIFO].num);
	ret |= rm_of_read(sizeof(u32), np,
		"pvr_num", &res->hwcap->entries[HWCAPS_TYPE_PVR].num);
	ret |= rm_of_read(sizeof(u32), np,
		"vq_num", &res->hwcap->entries[HWCAPS_TYPE_VQ].num);
	ret |= rm_of_read(sizeof(u32), np,
		"svq_num", &res->hwcap->entries[HWCAPS_TYPE_SVQ].num);
	ret |= rm_of_read(sizeof(u32), np,
		"livein_num", &res->hwcap->entries[HWCAPS_TYPE_LIVEIN].num);
	ret |= rm_of_read(sizeof(u32), np,
		"filein_num", &res->hwcap->entries[HWCAPS_TYPE_FILEIN].num);
	ret |= rm_of_read(sizeof(u32), np,
		"filein_cmdQ_num", &res->hwcap->entries[HWCAPS_TYPE_FILEIN_CMDQ].num);
	ret |= rm_of_read(sizeof(u32), np,
		"merge_stream_num", &res->hwcap->entries[HWCAPS_TYPE_MERGE_STREAM].num);
	ret |= rm_of_read(sizeof(u32), np,
		"mmfi_num", &res->hwcap->entries[HWCAPS_TYPE_MMFI].num);
	ret |= rm_of_read(sizeof(u32), np,
		"mmfi_cmdQ_num", &res->hwcap->entries[HWCAPS_TYPE_MMFI_CMDQ].num);
	ret |= rm_of_read(sizeof(u32), np,
		"fiq_num", &res->hwcap->entries[HWCAPS_TYPE_FIQ].num);
	ret |= rm_of_read(sizeof(u32), np,
		"tso_num", &res->hwcap->entries[HWCAPS_TYPE_TSO].num);
	ret |= rm_of_read(sizeof(u32), np,
		"tso_tsif_num", &res->hwcap->entries[HWCAPS_TYPE_TSO_TSIF].num);
	ret |= rm_of_read(sizeof(u32), np,
		"tlv_num", &res->hwcap->entries[HWCAPS_TYPE_TLV].num);
	ret |= rm_of_read(sizeof(u32), np,
		"alp_num", &res->hwcap->entries[HWCAPS_TYPE_ALP].num);
	ret |= rm_of_read(sizeof(u32), np,
		"cilink_num", &res->hwcap->entries[HWCAPS_TYPE_CILINK].num);
	ret |= rm_of_read(sizeof(u32), np,
		"ca_num", &res->hwcap->entries[HWCPAS_TYPE_CA].num);

	of_node_put(np);

	// clk rate
	np = of_find_node_by_name(pdev->dev.of_node, "ck_rate");
	if (np == NULL) {
		MTK_DMX_ERR("Failed to get hwcaps node\n");
		return -ENOENT;
	}

	ret |= rm_of_read(sizeof(u32), np,
		"tsp", &res->hwcap->entries[HWCAPS_TYPE_CK_RATE_TSP].num);
	ret |= rm_of_read(sizeof(u32), np,
		"parser", &res->hwcap->entries[HWCAPS_TYPE_CK_RATE_PARSER].num);
	ret |= rm_of_read(sizeof(u32), np,
		"stc", &res->hwcap->entries[HWCAPS_TYPE_CK_RATE_STC].num);

	of_node_put(np);
	return (ret == 0) ? 0 : -ENOENT;
}

#ifndef DMX2LITE
static int rm_fifos_init(struct mtk_dmx_rm *res)
{
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	u16 vfifo_num, afifo_num;
	u32 map_size;

	vfifo_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_VFIFO);
	afifo_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_AFIFO);

	spin_lock_init(&res->fifo_lock);

	map_size = vfifo_num * sizeof(struct fifo_status);
	res->vfifo_slot_map = devm_kzalloc(mtkdev->dev, map_size, GFP_KERNEL);
	if (!res->vfifo_slot_map) {
		MTK_DMX_ERR("allocate vfifo_slot_map fail\n");
		return -ENOMEM;
	}

	map_size = afifo_num * sizeof(struct fifo_status);
	res->afifo_slot_map = devm_kzalloc(mtkdev->dev, map_size, GFP_KERNEL);
	if (!res->afifo_slot_map) {
		MTK_DMX_ERR("allocate afifo_slot_map fail\n");
		return -ENOMEM;
	}

	return 0;
}

static int rm_pvr_init(struct mtk_dmx_rm *res)
{
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	u16 pvr_eng_num;
	u32 map_size;

	pvr_eng_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_PVR);

	spin_lock_init(&res->pvr_lock);

	map_size = pvr_eng_num * sizeof(struct pvr_status);
	res->pvr_eng_slot_map = devm_kzalloc(mtkdev->dev, map_size, GFP_KERNEL);
	if (!res->pvr_eng_slot_map) {
		MTK_DMX_ERR("allocate pvr_eng_slot_map fail\n");
		return -ENOMEM;
	}

	return 0;
}

static int rm_stc_init(struct mtk_dmx_rm *res)
{
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	u16 stc_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_STC);
	u32 map_size;

	spin_lock_init(&res->stc_lock);

	map_size = stc_num * sizeof(struct filter_status);
	res->stc_slot_map = devm_kzalloc(mtkdev->dev, map_size, GFP_KERNEL);
	if (!res->stc_slot_map) {
		MTK_DMX_ERR("allocate stc slot map fail\n");
		return -ENOMEM;
	}

	return 0;
}

static int rm_tso_init(struct mtk_dmx_rm *res)
{
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	u16 tso_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_TSO);
	u32 map_size, i;

	spin_lock_init(&res->tso_lock);

	map_size = tso_num * sizeof(struct tso_status);
	res->tso_slot_map = devm_kzalloc(mtkdev->dev, map_size, GFP_KERNEL);
	if (!res->tso_slot_map) {
		MTK_DMX_ERR("allocate tso slot map fail\n");
		return -ENOMEM;
	}

	for (i = 0; i < tso_num ; i++) {
		res->tso_slot_map[i].path_id = DEMUX_PATH_NULL;
		res->tso_slot_map[i].state = RES_STATE_FREE;
	}

	return 0;
}

static int rm_filters_init(struct mtk_dmx_rm *res)
{
	s32 ret = 0;
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	u16 pid_filter_num, sec_filter_num, pcr_filter_num, ip_filter_num;
	u32 map_size;

	pid_filter_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_PID_FILTER);
	sec_filter_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_SEC_FILTER);
	pcr_filter_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_PCR_FILTER);
	ip_filter_num  = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_IP_FILTER);

	spin_lock_init(&res->pid_filter_lock);
	spin_lock_init(&res->sec_filter_lock);
	spin_lock_init(&res->pcr_filter_lock);
	spin_lock_init(&res->ip_filter_lock);

	map_size = pid_filter_num * sizeof(struct filter_status);
	res->pid_filter_slot_map = devm_kzalloc(mtkdev->dev, map_size, GFP_KERNEL);
	if (!res->pid_filter_slot_map) {
		MTK_DMX_ERR("allocate pid filter slot map fail\n");
		return -ENOMEM;
	}

	map_size = sec_filter_num * sizeof(struct filter_status);
	res->sec_filter_slot_map = devm_kzalloc(mtkdev->dev, map_size, GFP_KERNEL);
	if (!res->sec_filter_slot_map) {
		MTK_DMX_ERR("allocate sec filter slot map fail\n");
		return -ENOMEM;
	}

	map_size = pcr_filter_num * sizeof(struct filter_status);
	res->pcr_filter_slot_map = devm_kzalloc(mtkdev->dev, map_size, GFP_KERNEL);
	if (!res->pcr_filter_slot_map) {
		MTK_DMX_ERR("allocate pcr filter slot map fail\n");
		return -ENOMEM;
	}

	map_size = ip_filter_num * sizeof(struct filter_status);
	res->ip_filter_slot_map = devm_kzalloc(mtkdev->dev, map_size, GFP_KERNEL);
	if (!res->ip_filter_slot_map) {
		MTK_DMX_ERR("allocate ip filter slot map fail\n");
		return -ENOMEM;
	}

	// TODO: Refactor others later
	ret = rm_filter_init(res, HWCAPS_TYPE_ALP_FILTER);
	if (ret != 0) {
		MTK_DMX_ERR(
			"filter_init(..., %d) is failed.\n",
			HWCAPS_TYPE_ALP_FILTER);

		ret = rm_deinit(res);

		return -ENOMEM;
	}

	return 0;
}

static int rm_padcfg_init(struct mtk_dmx_rm *res)
{
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);

	res->padcfg = devm_kzalloc(mtkdev->dev, sizeof(struct dmx_padcfg), GFP_KERNEL);
	if (!res->padcfg) {
		MTK_DMX_ERR("allocate pad config fail\n");
		return -ENOMEM;
	}

	res->padcfg->pin_ctrl = devm_pinctrl_get(mtkdev->dev);
	if (!res->padcfg->pin_ctrl) {
		MTK_DMX_ERR("devm_pinctrl_get fail\n");
		return -EINVAL;
	}

	return 0;
}
#endif

int mtk_dmx_rm_init(struct mtk_dmx_rm *res)
{
	s32 ret = 0;
#ifndef DMX2LITE
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	struct mem_info info = { 0 };

	//Init pin control
	ret = rm_padcfg_init(res);
	if (ret < 0) {
		MTK_DMX_ERR("rm_padcfg_init fail\n");
		return ret;
	}

	//process memmap
	ret = rm_memmap_init(res);
	if (ret < 0) {
		MTK_DMX_ERR("init memmap struct fail\n");
		return ret;
	}
#endif
	//process hwcap
	ret = rm_hwcaps_init(res);
	if (ret < 0) {
		MTK_DMX_ERR("init hwcaps struct fail\n");
		return ret;
	}
#ifndef DMX2LITE
	//process regmaps
	ret = rm_regmaps_init(res);
	if (ret < 0) {
		MTK_DMX_ERR("init regmaps struct fail\n");
		return ret;
	}
	dmx_bank_offsets = res->regmap->banks;

	ret = rm_fifos_init(res);
	if (ret < 0) {
		MTK_DMX_ERR("init fifos struct fail\n");
		return ret;
	}

	ret = rm_pvr_init(res);
	if (ret < 0) {
		MTK_DMX_ERR("init pvr struct fail\n");
		return ret;
	}

	ret = rm_stc_init(res);
	if (ret < 0) {
		MTK_DMX_ERR("init stc struct fail\n");
		return ret;
	}

	ret = rm_filters_init(res);
	if (ret < 0) {
		MTK_DMX_ERR("init filters' struct fail\n");
		return ret;
	}

	ret = rm_tso_init(res);
	if (ret < 0) {
		MTK_DMX_ERR("init tso' struct fail\n");
		return ret;
	}
#endif
	return 0;
}

#ifndef DMX2LITE
int mtk_dmx_rm_exit(struct mtk_dmx_rm *res)
{
	int ret = 0;

	dmx_bank_offsets = NULL;
	rm_memmap_exit(res);

	ret = rm_deinit(res);
	if (ret != 0)
		MTK_DMX_ERR(
			"rm_deinit(...) is failed.\n");

	return ret;
}

u16 mtk_dmx_rm_get_hwcap_by_name(struct mtk_dmx_rm *res, const char *name)
{
	return 0;
}
#endif

u16 mtk_dmx_rm_get_hwcap_by_id(struct mtk_dmx_rm *res, u32 id)
{
	const u16 max_scmb_detect_flt_num = 144;
	u16 num;

	if (id >= HWCPAS_TYPE_MAX)
		return 0;

	num = res->hwcap->entries[id].num;

	if (id == HWCAPS_TYPE_PID_FILTER || id == HWCAPS_TYPE_SEC_FILTER)
		return min(num, max_scmb_detect_flt_num);
	else
		return num;
}

#ifndef DMX2LITE
int mtk_dmx_rm_get_buf(
	struct mtk_dmx_rm *res,
	enum mmap_buf_type type,
	u32 buf_size,
	struct mem_info *mem_info)
{
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);

	int ret = 0;
	enum mmap_heap_type heaptype = res->memmap->list[type]->heaptype;
	u32 req_size = 0;

	if (!mem_info) {
		MTK_DMX_ERR("mem_info is null pointer\n");
		return -ENOMEM;
	}

	switch (heaptype) {
	case HEAP_IOMMU:
		return rm_mma_get_memory(true, res->memmap->list[type],
			&mem_info[0], buf_size);
	case HEAP_CMA:
		req_size = ALIGN(buf_size, DMX_4K_ALIGN);
		break;
	case HEAP_NON_MMA:
		req_size = buf_size;
		break;
	default:
		MTK_DMX_ERR("type: %x is not support\n", type);
		return -EINVAL;
	}

	ret = rm_pool_get_memory(mtkdev->dev, res, type, &mem_info[0], req_size);
	if (ret == 0 && heaptype == HEAP_CMA)
		ret = rm_mma_get_memory(false, res->memmap->list[type],
			&mem_info[0], req_size);
	return ret;
}


int mtk_dmx_rm_put_buf(struct mtk_dmx_rm *res, struct mem_info *mem_info)
{
	struct mtk_dmx2dev *mtkdev = container_of(res, struct mtk_dmx2dev, rm);
	int ret = 0;

	switch (mem_info[0].heaptype) {
	case HEAP_IOMMU:
		rm_mma_put_memory(&mem_info[0]);
		break;
	case HEAP_CMA:
		rm_mma_put_memory(&mem_info[0]);
		rm_pool_put_memory(mtkdev->dev, &mem_info[0]);
		break;
	case HEAP_NON_MMA:
		rm_pool_put_memory(mtkdev->dev, &mem_info[0]);
		break;
	default:
		return -EINVAL;
	}
	return ret;
}

int mtk_dmx_rm_pvr_alloc_engine(struct mtk_dmx_rm *res, u16 path_id)
{
	u16 i;
	unsigned long flags;
	u16 pvr_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_PVR);

	spin_lock_irqsave(&res->pvr_lock, flags);

	for (i = 0; i < pvr_num; i++) {
		if ((res->pvr_eng_slot_map[i].state != RES_STATE_FREE) &&
			res->pvr_eng_slot_map[i].path_id == path_id) {
			spin_unlock_irqrestore(&res->pvr_lock, flags);
			return i;
		}
	}

	for (i = 0; i < pvr_num; i++) {
		if (res->pvr_eng_slot_map[i].state == RES_STATE_FREE) {
			res->pvr_eng_slot_map[i].state = RES_STATE_ALLOC;
			res->pvr_eng_slot_map[i].path_id = path_id;
			spin_unlock_irqrestore(&res->pvr_lock, flags);
			return i;
		}
	}

	spin_unlock_irqrestore(&res->pvr_lock, flags);
	return -1;
}

int mtk_dmx_rm_pvr_free_engine(struct mtk_dmx_rm *res, int eng_id)
{
	unsigned long flags;

	if (res->pvr_eng_slot_map[eng_id].state == RES_STATE_FREE)
		return 0;

	spin_lock_irqsave(&res->pvr_lock, flags);
	res->pvr_eng_slot_map[eng_id].state = RES_STATE_FREE;
	spin_unlock_irqrestore(&res->pvr_lock, flags);

	return 0;
}

int mtk_dmx_rm_get_vfifo_idx(struct mtk_dmx_rm *res)
{
	u16 i;
	unsigned long flags;
	u16 vfifo_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_VFIFO);

	spin_lock_irqsave(&res->fifo_lock, flags);

	for (i = 0; i < vfifo_num; i++) {
		if (res->vfifo_slot_map[i].state == RES_STATE_FREE) {
			res->vfifo_slot_map[i].state = RES_STATE_ALLOC;
			spin_unlock_irqrestore(&res->fifo_lock, flags);
			return i;
		}
	}

	spin_unlock_irqrestore(&res->fifo_lock, flags);
	return -1;
}

void mtk_dmx_rm_put_vfifo_idx(struct mtk_dmx_rm *res, int idx)
{
	unsigned long flags;

	spin_lock_irqsave(&res->fifo_lock, flags);
	res->vfifo_slot_map[idx].state = RES_STATE_FREE;
	spin_unlock_irqrestore(&res->fifo_lock, flags);
}

int mtk_dmx_rm_get_afifo_idx(struct mtk_dmx_rm *res)
{
	u16 i;
	unsigned long flags;
	u16 afifo_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_AFIFO);

	spin_lock_irqsave(&res->fifo_lock, flags);

	for (i = 0; i < afifo_num; i++) {
		if (res->afifo_slot_map[i].state == RES_STATE_FREE) {
			res->afifo_slot_map[i].state = RES_STATE_ALLOC;
			spin_unlock_irqrestore(&res->fifo_lock, flags);
			return i;
		}
	}

	spin_unlock_irqrestore(&res->fifo_lock, flags);
	return -1;
}

void mtk_dmx_rm_put_afifo_idx(struct mtk_dmx_rm *res, int idx)
{
	unsigned long flags;

	spin_lock_irqsave(&res->fifo_lock, flags);
	res->afifo_slot_map[idx].state = RES_STATE_FREE;
	spin_unlock_irqrestore(&res->fifo_lock, flags);
}

int mtk_dmx_rm_stc_alloc_engine(struct mtk_dmx_rm *res, u16 path_id)
{
	u16 i;
	unsigned long flags;
	u16 stc_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_STC);

	spin_lock_irqsave(&res->stc_lock, flags);

	for (i = 0; i < stc_num; i++) {
		if (res->stc_slot_map[i].ref_cnt != 0 &&
			res->stc_slot_map[i].path_id == path_id) {
			spin_unlock_irqrestore(&res->stc_lock, flags);
			return i;
		}
	}

	for (i = 0; i < stc_num; i++) {
		if (res->stc_slot_map[i].ref_cnt == 0) {
			res->stc_slot_map[i].ref_cnt++;
			res->stc_slot_map[i].path_id = path_id;
			spin_unlock_irqrestore(&res->stc_lock, flags);
			return i;
		}
	}

	spin_unlock_irqrestore(&res->stc_lock, flags);
	return -1;
}

// @TODO: when dmx close and remove parameter indicated, need to call it
int mtk_dmx_rm_stc_free_engine(struct mtk_dmx_rm *res, u16 eng_id)
{
	unsigned long flags;

	spin_lock_irqsave(&res->stc_lock, flags);
	if (res->stc_slot_map[eng_id].ref_cnt == 0) {
		spin_unlock_irqrestore(&res->stc_lock, flags);
		return 0;
	}

	res->stc_slot_map[eng_id].ref_cnt--;
	if (res->stc_slot_map[eng_id].ref_cnt != 0) {
		MTK_DMX_ERR("stc used, eng_id = %d, ref_cnt = %d\n",
			eng_id, res->stc_slot_map[eng_id].ref_cnt);
		spin_unlock_irqrestore(&res->stc_lock, flags);
		return -1;
	}

	spin_unlock_irqrestore(&res->stc_lock, flags);

	return 0;
}

int mtk_dmx_rm_alloc_tso_idx(struct mtk_dmx_rm *res, u16 path_id)
{
	u16 i;
	unsigned long flags;
	u16 tso_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_TSO);

	spin_lock_irqsave(&res->tso_lock, flags);

	for (i = 0; i < tso_num; i++) {
		if (res->tso_slot_map[i].state == RES_STATE_ALLOC &&
			res->tso_slot_map[i].path_id == path_id) {
			spin_unlock_irqrestore(&res->tso_lock, flags);
			return i;
		}
	}

	for (i = 0; i < tso_num; i++) {
		if (res->tso_slot_map[i].state == RES_STATE_FREE) {
			res->tso_slot_map[i].state = RES_STATE_ALLOC;
			res->tso_slot_map[i].path_id = path_id;
			spin_unlock_irqrestore(&res->tso_lock, flags);
			return i;
		}
	}
	spin_unlock_irqrestore(&res->tso_lock, flags);

	return -1;
}

int mtk_dmx_rm_free_tso_idx(struct mtk_dmx_rm *res, u16 path_id)
{
	u16 i;
	unsigned long flags;
	u16 tso_num = mtk_dmx_rm_get_hwcap_by_id(res, HWCAPS_TYPE_TSO);

	spin_lock_irqsave(&res->tso_lock, flags);

	for (i = 0; i < tso_num; i++) {
		if (res->tso_slot_map[i].state == RES_STATE_ALLOC &&
			res->tso_slot_map[i].path_id == path_id) {
			res->tso_slot_map[i].state = RES_STATE_FREE;
			spin_unlock_irqrestore(&res->tso_lock, flags);
			return i;
		}
	}
	spin_unlock_irqrestore(&res->tso_lock, flags);

	return -1;
}

int mtk_dmx_rm_get_filter_idx(struct mtk_dmx_rm *res, struct filter_type type)
{
	int ret = 0;
	int filter_idx = FILTER_IDX_INVALID;

	switch (type.main) {
	case DMX2_FILTER_TYPE_TS: {
		switch (type.sub) {
		case FILTER_SUB_TYPE_PID:
		case FILTER_SUB_TYPE_SEC:
			return rm_alloc_pid_filter(res, type.sub);
		case FILTER_SUB_TYPE_PCR:
			return rm_alloc_pcr_filter(res);
		default:
			return -1;
		}
		break;
	}
	case DMX2_FILTER_TYPE_TLV:
	case DMX2_FILTER_TYPE_MMTP:
		return rm_alloc_pid_filter(res, type.sub);
	case DMX2_FILTER_TYPE_ALP:
		ret = rm_alloc_filter(res, type.sub, &filter_idx);
		if (ret != 0) {
			MTK_DMX_ERR(
				"_alloc_filter(..., %d, ...) is failed.\n",
				type.sub);

			return FILTER_IDX_INVALID;
		}
		return filter_idx;
	case DMX2_FILTER_TYPE_IP:
		switch (type.sub) {
		case FILTER_SUB_TYPE_ALP:
			ret = rm_alloc_filter(res, type.sub, &filter_idx);
			if (ret != 0) {
				MTK_DMX_ERR(
					"_alloc_filter(..., %d, ...) is failed.\n",
					type.sub);

				return FILTER_IDX_INVALID;
			}
			return filter_idx;
		case FILTER_SUB_TYPE_IP:
			return rm_alloc_ip_filter(res);
		default:
			MTK_DMX_ERR(
				"type.sub(%d) is not supported.\n",
				type.sub);

			return FILTER_IDX_INVALID;
		}
	default:
		break;
	}
	return -1;
}

void mtk_dmx_rm_put_filter_idx(struct mtk_dmx_rm *res, struct filter_type type, u16 filter_id)
{
	int ret = 0;

	switch (type.main) {
	case DMX2_FILTER_TYPE_TS: {
		switch (type.sub) {
		case FILTER_SUB_TYPE_PID:
		case FILTER_SUB_TYPE_SEC:
			rm_release_pid_filter(res, filter_id);
			break;
		case FILTER_SUB_TYPE_PCR:
			rm_release_pcr_filter(res, filter_id);
			break;
		default:
			break;
		}
		break;
	}
	case DMX2_FILTER_TYPE_TLV:
	case DMX2_FILTER_TYPE_MMTP:
		rm_release_pid_filter(res, filter_id);
		break;
	case DMX2_FILTER_TYPE_ALP:
		ret = rm_release_filter(res, type.sub, filter_id);
		if (ret != 0) {
			MTK_DMX_ERR(
				"_release_filter(..., %d, %d) is failed.\n",
				type.sub,
				filter_id);

			return;
		}
		break;
	case DMX2_FILTER_TYPE_IP:
		switch (type.sub) {
		case FILTER_SUB_TYPE_ALP:
			ret = rm_release_filter(res, type.sub, filter_id);
			if (ret != 0) {
				MTK_DMX_ERR(
					"_release_filter(..., %d, %d) is failed.\n",
					type.sub,
					filter_id);

				return;
			}
			break;
		case FILTER_SUB_TYPE_IP:
			rm_release_ip_filter(res, filter_id);
			break;
		default:
			MTK_DMX_ERR(
				"type.sub(%d) is not supported.\n",
				type.sub);
			return;
		}
		break;
	default:
		break;
	}
}

int mtk_dmx_rm_set_padcfg(struct mtk_dmx_rm *res, enum pin_control_type type)
{
	static char * const pinctrl_names[] = {
						"cilink_pad_clk",
						"cilink_pad_data0",
						"cilink_pad_data1",
						"cilink_drive_clk",
						"cilink_drive_data0",
						"ext2_ts_demod_pad_mux",
						"ext1_ts_demod_pad_mux",
						"ext_ts_demod_pad_mux"};
	struct pinctrl_state *state;

	state = pinctrl_lookup_state(res->padcfg->pin_ctrl, pinctrl_names[type]);
	if (IS_ERR(state)) {
		MTK_DMX_ERR("pinctrl_lookup_state couldn't find state of type %d\n", type);
		return PTR_ERR(state);
	}

	if (pinctrl_select_state(res->padcfg->pin_ctrl, state)) {
		MTK_DMX_ERR("pinctrl_select_state failed\n");
		return -EINVAL;
	}

	return 0;
}
#endif
