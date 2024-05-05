extern void flush_cache_all(void);
#define __cpuc_flush_kern_all flush_cache_all
extern void dmac_flush_range(const void *, const void *);
extern void __dma_map_area(const void *, size_t, int);
