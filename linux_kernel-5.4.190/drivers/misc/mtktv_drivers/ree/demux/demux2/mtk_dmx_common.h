/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_COMMON_H
#define MTK_DMX_COMMON_H

#ifndef DEBUG
#define DEBUG // remove later
#endif //DEBUG

#ifdef pr_fmt
#undef pr_fmt
#endif //pr_fmt
#define pr_fmt(fmt) "mtk_dmx: " fmt

#include <linux/io.h>
#include <linux/bitops.h>
#include <linux/kern_levels.h>

#include <asm-generic/div64.h>

#define DRV2_NAME				"mediatek,demux2"

//-------------------------------------------------------------------------------------------------
//  Debug Macro and Define
//-------------------------------------------------------------------------------------------------
#define LOG_LEVEL_ERROR			(3)
#define LOG_LEVEL_WARNING		(4)
#define LOG_LEVEL_INFO			(6)
#define LOG_LEVEL_DEBUG			(7)

extern int log_level;

#define MTK_DMX_ERR(fmt, ...)	\
		pr_err("[%s][%d]" fmt,	\
				__func__, __LINE__, ##__VA_ARGS__)	\

#define MTK_DMX_WARN(fmt, ...)	\
		pr_warn("[%s][%d]" fmt,	\
				__func__, __LINE__, ##__VA_ARGS__)	\

#define MTK_DMX_INFO(fmt, ...)	\
		pr_info("[%s][%d]" fmt,	\
				__func__, __LINE__, ##__VA_ARGS__)	\

#define MTK_DMX_DEBUG(fmt, ...)	\
		pr_debug("[%s][%d]" fmt,	\
				__func__, __LINE__, ##__VA_ARGS__)	\

#define MTK_DMX_SUSPEND(dmxdev, fmt, ...) \
	do { \
		if (dmxdev->pm.suspend_dbg_en) \
			MTK_DMX_INFO(fmt, ##__VA_ARGS__); \
	} while (0)

#define MTK_DMX_INVALID_PARAMETER                    (-2)
#define MTK_DMX_FAIL                                 (-1)
#define MTK_DMX_SUCCESS                              (0)

#define dmx2dev_to_path(dev)	\
		container_of(dev, struct mtk_dmx_path, dmx2dev)

#define fh_to_demux(fh)	\
	({	\
		struct dmx2dev *dmx2dev = fh_to_dmx2dev(fh);	\
		struct mtk_dmx_path *path = dmx2dev_to_path(dmx2dev);	\
		&path->demux2;	\
	})

#define fh_to_dvr(fh)	\
	({	\
		struct dmx2dev *dmx2dev = fh_to_dmx2dev(fh);	\
		struct mtk_dmx_path *path = dmx2dev_to_path(dmx2dev);	\
		&path->dvr2;	\
	})

#define cast_to_dmx_parent(demux)	((struct dvb_demux2 *)demux)
#define cast_to_dvr_parent(dvr)		((struct dvb_dvr2 *)dvr)

#define cast_to_mtk_demux(demux) \
		container_of(demux, struct mtk_demux2, base)
#define cast_to_mtk_dvr(dvr) \
		container_of(dvr, struct mtk_dvr2, base)

#define cast_to_dmx2_filter(filter)	((struct dmx2_filter *)filter)
#define cast_to_dmx2_feed(feed)		((struct dmx2_feed *)feed)

#define has_op(dev, type, op)	\
	(dev->type##_ops && dev->type##_ops->op)

#define call_op(dev, type, op, args...)	\
	(has_op(dev, type, op) ? dev->type##_ops->op(args) : -ENOTTY)

#define static_cast(type, obj)		((type)(obj))
#define dynamic_cast(type, obj)		((type)(obj))

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define DEMUX_PATH_NULL			((u16)-1)

#define TSIF_0					(0)
#define TSIF_FI					(1)
#define TSIF_1					(2)
#define TSIF_2					(3)
#define TLV_TSIF_0				(4)
#define TLV_TSIF_1				(5)
#define TLV_TSIF_2				(6)
#define TS_ALP_0				(7)
#define TS_ALP_FI				(8)

#define VFIFO_0					(0)
#define VFIFO_1					(1)

#define AFIFO_0					(0)
#define AFIFO_1					(1)
#define AFIFO_2					(2)
#define AFIFO_3					(3)

#define PVR_1					(0)
#define PVR_2					(1)
#define PVR_3					(2)
#define PVR_4					(3)

#define STC_0					(0)
#define STC_1					(1)
#define STC_2					(2)
#define STC_3					(3)

#define PCR_0					(0)
#define PCR_1					(1)

#define TSO_TSIF_1				(0)
#define TSO_TSIF_5				(1)
#define TSO_TSIF_6				(2)

#define NTP_0					(0)
#define NTP_1					(1)
#define NTP_2					(2)
#define NTP_3					(3)

#define MIU_BUS					(4)
#define FQ_PACKET_UNIT_LENGTH			(192)
#define VQ_PACKET_UNIT_LENGTH			(208)

#define MIU0_BUS_BASE			(0x20000000) // MIU0 Low 256MB
#define MIU1_BUS_BASE			(0xA0000000) // MIU1 Low 256MB

#define RIU_BANK_SIZE			(0x80)

#define PA_to_BA(pa)			(pa + MIU0_BUS_BASE)
#define BA_to_PA(ba)			(ba - MIU0_BUS_BASE)

#define ADDR_CAST(t) (t)(size_t)

#define HAS_BITS(val, bits)		((val) & (bits))
#define SET_BITS(val, bits)		((val) |= (bits))
#define CLEAR_BITS(val, bits)	((val) &= (~(bits)))

#define IS_TSIF_TS(path) ((path == TSIF_0) || (path == TSIF_FI) ||\
	(path == TSIF_1) || (path == TSIF_2))
#define IS_TSIF_TLV(path) ((path == TLV_TSIF_0) || (path == TLV_TSIF_1) ||\
	(path == TLV_TSIF_2))
#define IS_TSIF_ALP(path) ((path == TS_ALP_0) || (path == TS_ALP_FI))

#define MTK_DMX_IN_RANGE(position, start, len) \
	(((position) >= (start)) && ((position) < (start) + (len)))
#define MTK_DMX_NELEMS(x) \
	(sizeof(x) / sizeof((x)[0]))

#define MTK_DMX_KVFREE(ptr) \
	do { \
		kvfree(ptr); \
		(ptr) = NULL; \
	} while (0)

/**
 * To support 32 bits,
 * referenced from include/linux/kernel.h and modify it
 */

/**
 * roundup64 - round up to the next specified multiple
 * @x: the value to up
 * @y: multiple to round up to
 *
 * Rounds @x up to next multiple of @y. If @y will always be a power
 * of 2, consider using the faster round_up().
 */
static inline u64 roundup64(u64 x, u32 y)
{
	u64 q = x + (y - 1);

	do_div(q, y);
	return (q * y);
}

/**
 * rounddown64 - round down to next specified multiple
 * @x: the value to round
 * @y: multiple to round down to
 *
 * Rounds @x down to next multiple of @y. If @y will always be a power
 * of 2, consider using the faster round_down().
 */
static inline u64 rounddown64(u64 x, u32 y)
{
	u64 __x = x;

	return (x - do_div(__x, y));
}

#endif /* MTK_DMX_COMMON_H */
