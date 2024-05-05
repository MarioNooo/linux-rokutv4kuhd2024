#include <linux/linkage.h>
//#include <asm/assembler.h>
//#include <asm/cpufeature.h>
//#include <asm/alternative.h>
//#include "cache.h"
#include <linux/kbuild.h>

#define DMA_TO_DEVICE 1
#define DMA_FROM_DEVICE 2
#ifdef CONFIG_ARCH_USE_SYM_ANNOTATIONS
#ifndef ENTRY
#define ENTRY(name) SYM_FUNC_START(name)
#endif
#ifndef ENDPROCWW
#define ENDPROC(name) SYM_FUNC_END(name)
#endif
#endif
//DEFINE(DMA_TO_DEVICE,         DMA_TO_DEVICE);
//DEFINE(DMA_FROM_DEVICE,       DMA_FROM_DEVICE);

/* copy from arch/arm64/include/asm/assembler.h */
	.macro save_and_disable_daif, flags
	mrs	\flags, daif
	msr	daifset, #0xf
	.endm

	.macro disable_daif
	msr	daifset, #0xf
	.endm

	.macro enable_daif
	msr	daifclr, #0xf
	.endm

	.macro	restore_daif, flags:req
	msr	daif, \flags
	.endm

	/* Only on aarch64 pstate, PSR_D_BIT is different for aarch32 */
	.macro	inherit_daif, pstate:req, tmp:req
	and	\tmp, \pstate, #(PSR_D_BIT | PSR_A_BIT | PSR_I_BIT | PSR_F_BIT)
	msr	daif, \tmp
	.endm

	/* IRQ is the lowest priority flag, unconditionally unmask the rest. */
	.macro enable_da_f
	msr	daifclr, #(8 | 4 | 1)
	.endm

/*
 * dcache_line_size - get the safe D-cache line size across all CPUs
 */
	.macro	dcache_line_size, reg, tmp
	read_ctr	\tmp
	ubfm		\tmp, \tmp, #16, #19	// cache line size encoding
	mov		\reg, #4		// bytes per word
	lsl		\reg, \reg, \tmp	// actual cache line size
	.endm

/*
 * read_ctr - read CTR_EL0. If the system has mismatched register fields,
 * provide the system wide safe value from arm64_ftr_reg_ctrel0.sys_val
 */
	.macro	read_ctr, reg
/*
 * FIXME: alternative.h use non exported symbol that we can't include it directly
 * as a loadable kernel module.
 */
#if 0
alternative_if_not ARM64_MISMATCHED_CACHE_TYPE
	mrs	\reg, ctr_el0			// read CTR
	nop
alternative_else
	ldr_l	\reg, arm64_ftr_reg_ctrel0 + ARM64_FTR_SYSVAL
alternative_endif
#else
	mrs	\reg, ctr_el0			// read CTR
	nop
#endif
	.endm

/*
 * Macro to perform a data cache maintenance for the interval
 * [kaddr, kaddr + size)
 *
 * 	op:		operation passed to dc instruction
 * 	domain:		domain used in dsb instruciton
 * 	kaddr:		starting virtual address of the region
 * 	size:		size of the region
 * 	Corrupts:	kaddr, size, tmp1, tmp2
 */
	.macro __dcache_op_workaround_clean_cache, op, kaddr
/*
 * FIXME: alternative.h use non exported symbol that we can't include it directly
 * as a loadable kernel module.
 */
#if 0
alternative_if_not ARM64_WORKAROUND_CLEAN_CACHE
	dc	\op, \kaddr
alternative_else
	dc	civac, \kaddr
alternative_endif
#else
	dc	civac, \kaddr
#endif
	.endm

/*
 * Save/restore interrupts.
 */
	.macro	save_and_disable_irq, flags
	mrs	\flags, daif
	msr	daifset, #2
	.endm

	.macro	restore_irq, flags
	msr	daif, \flags
	.endm

	.macro dcache_by_line_op op, domain, kaddr, size, tmp1, tmp2
	dcache_line_size \tmp1, \tmp2
	add	\size, \kaddr, \size
	sub	\tmp2, \tmp1, #1
	bic	\kaddr, \kaddr, \tmp2
9998:
	.ifc	\op, cvau
	__dcache_op_workaround_clean_cache \op, \kaddr
	.else
	.ifc	\op, cvac
	__dcache_op_workaround_clean_cache \op, \kaddr
	.else
	.ifc	\op, cvap
	sys	3, c7, c12, 1, \kaddr	// dc cvap
	.else
	.ifc	\op, cvadp
	sys	3, c7, c13, 1, \kaddr	// dc cvadp
	.else
	dc	\op, \kaddr
	.endif
	.endif
	.endif
	.endif
	add	\kaddr, \kaddr, \tmp1
	cmp	\kaddr, \size
	b.lo	9998b
	dsb	\domain
	.endm

/* copy from legacy kernel arch/arm64/mm/cache.S */

/*
 *	__flush_dcache_all()
 *
 *	Flush the whole D-cache.
 *
 *	Corrupted registers: x0-x7, x9-x11
 */
__flush_dcache_all:
	dmb	sy				// ensure ordering with previous memory accesses
	mrs	x0, clidr_el1			// read clidr
	and	x3, x0, #0x7000000		// extract loc from clidr
	lsr	x3, x3, #23			// left align loc bit field
	cbz	x3, finished			// if loc is 0, then no need to clean
	mov	x10, #0				// start clean at cache level 0
loop1:
	add	x2, x10, x10, lsr #1		// work out 3x current cache level
	lsr	x1, x0, x2			// extract cache type bits from clidr
	and	x1, x1, #7			// mask of the bits for current cache only
	cmp	x1, #2				// see what cache we have at this level
	b.lt	skip				// skip if no cache, or just i-cache
	save_and_disable_irq x9			// make CSSELR and CCSIDR access atomic
	msr	csselr_el1, x10			// select current cache level in csselr
	isb					// isb to sych the new cssr&csidr
	mrs	x1, ccsidr_el1			// read the new ccsidr
	restore_irq x9
	and	x2, x1, #7			// extract the length of the cache lines
	add	x2, x2, #4			// add 4 (line length offset)
	mov	x4, #0x3ff
	and	x4, x4, x1, lsr #3		// find maximum number on the way size
	clz	w5, w4				// find bit position of way size increment
	mov	x7, #0x7fff
	and	x7, x7, x1, lsr #13		// extract max number of the index size
loop2:
	mov	x9, x4				// create working copy of max way size
loop3:
	lsl	x6, x9, x5
	orr	x11, x10, x6			// factor way and cache number into x11
	lsl	x6, x7, x2
	orr	x11, x11, x6			// factor index number into x11
	dc	cisw, x11			// clean & invalidate by set/way
	subs	x9, x9, #1			// decrement the way
	b.ge	loop3
	subs	x7, x7, #1			// decrement the index
	b.ge	loop2
skip:
	add	x10, x10, #2			// increment cache number
	cmp	x3, x10
	b.gt	loop1
finished:
	mov	x10, #0				// swith back to cache level 0
	msr	csselr_el1, x10			// select current cache level in csselr
	dsb	sy
	isb
	ret
ENDPROC(__flush_dcache_all)

/*
 *	flush_cache_all()
 *
 *	Flush the entire cache system.  The data cache flush is now achieved
 *	using atomic clean / invalidates working outwards from L1 cache. This
 *	is done using Set/Way based cache maintenance instructions.  The
 *	instruction cache can still be invalidated back to the point of
 *	unification in a single instruction.
 */
ENTRY(flush_cache_all)
	mov	x12, x30
	bl	__flush_dcache_all
	mov	x0, #0
	ic	ialluis				// I+BTB cache invalidate
	ret	x12
ENDPROC(flush_cache_all)

/*
 *	dmac_flush_range(start, end)
 *
 *	clean & invalidate D / U line
 *
 *	- start   - virtual start address of region
 *	- end    - virtual end address of region
 */
ENTRY(dmac_flush_range)
	sub x1, x1, x0
	dcache_by_line_op civac, sy, x0, x1, x2, x3
	ret
ENDPROC(dmac_flush_range)

/*
 *	__inval_dcache_area(kaddr, size)
 *
 * 	Ensure that any D-cache lines for the interval [kaddr, kaddr+size)
 * 	are invalidated. Any partial lines at the ends of the interval are
 *	also cleaned to PoC to prevent data loss.
 *
 *	- kaddr   - kernel address
 *	- size    - size in question
 */
ENTRY(__inval_dcache_area)
	/* FALLTHROUGH */

/*
 *	__dma_inv_area(start, size)
 *	- start   - virtual start address of region
 *	- size    - size in question
 */
__dma_inv_area:
	add	x1, x1, x0
	dcache_line_size x2, x3
	sub	x3, x2, #1
	tst	x1, x3				// end cache line aligned?
	bic	x1, x1, x3
	b.eq	1f
	dc	civac, x1			// clean & invalidate D / U line
1:	tst	x0, x3				// start cache line aligned?
	bic	x0, x0, x3
	b.eq	2f
	dc	civac, x0			// clean & invalidate D / U line
	b	3f
2:	dc	ivac, x0			// invalidate D / U line
3:	add	x0, x0, x2
	cmp	x0, x1
	b.lo	2b
	dsb	sy
	ret
ENDPROC(__inval_dcache_area)
ENDPROC(__dma_inv_area)

/*
 *	__clean_dcache_area_poc(kaddr, size)
 *
 * 	Ensure that any D-cache lines for the interval [kaddr, kaddr+size)
 * 	are cleaned to the PoC.
 *
 *	- kaddr   - kernel address
 *	- size    - size in question
 */
ENTRY(__clean_dcache_area_poc)
	/* FALLTHROUGH */

/*
 *	__dma_clean_area(start, size)
 *	- start   - virtual start address of region
 *	- size    - size in question
 */
__dma_clean_area:
	dcache_by_line_op cvac, sy, x0, x1, x2, x3
	ret
ENDPROC(__clean_dcache_area_poc)
ENDPROC(__dma_clean_area)

/*
 *	__dma_flush_area(start, size)
 *
 *	clean & invalidate D / U line
 *
 *	- start   - virtual start address of region
 *	- size    - size in question
 */
ENTRY(__dma_flush_area)
	dcache_by_line_op civac, sy, x0, x1, x2, x3
	ret
ENDPROC(__dma_flush_area)

/*
 *	__dma_map_area(start, size, dir)
 *	- start	- kernel virtual start address
 *	- size	- size of region
 *	- dir	- DMA direction
 */
ENTRY(__dma_map_area)
#if 0
	cmp	w2, #DMA_FROM_DEVICE
#else
	cmp	x2, #2
#endif
	b.eq	__dma_inv_area
	b	__dma_clean_area
ENDPROC(__dma_map_area)

/*
 *	__dma_unmap_area(start, size, dir)
 *	- start	- kernel virtual start address
 *	- size	- size of region
 *	- dir	- DMA direction
 */
ENTRY(__dma_unmap_area)
#if 0
	cmp	w2, #DMA_TO_DEVICE
#else
	cmp	x2, #1
#endif
	b.ne	__dma_inv_area
	ret
ENDPROC(__dma_unmap_area)
