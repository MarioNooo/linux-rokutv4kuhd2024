/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_RIU_H
#define MTK_DMX_RIU_H

#ifdef __KERNEL__
#include <linux/types.h>
#include <asm/io.h>
#else
#include <stdint.h>
#endif

#include "mtk_dmx_common.h"
/*
 * @NOTE: remove volatile qualifier
 * see Documentation/process/volatile-considered-harmful.rst
 */
typedef u8 REG16;

#define UNUSED __attribute__((unused))
/*********************************************
 * ---------------- AC_FULL/MSK ----------------
 * B3[31:24] | B2[23:16] | B1[15:8] | B0[7:0]
 * W32[31:16] | W21[23:8] | W10 [15:0]
 * DW[31:0]
 * ---------------------------------------------
 *********************************************/
#define AC_FULLB0		1
#define AC_FULLB1		2
#define AC_FULLB2		3
#define AC_FULLB3		4
#define AC_FULLW10		5
#define AC_FULLW21		6
#define AC_FULLW32		7
#define AC_FULLDW		8
#define AC_MSKB0		9
#define AC_MSKB1		10
#define AC_MSKB2		11
#define AC_MSKB3		12
#define AC_MSKW10		13
#define AC_MSKW21		14
#define AC_MSKW32		15
#define AC_MSKDW		16

/*********************************************
 * -------------------- Fld --------------------
 * wid[31:16] | shift[15:8] | ac[7:0]
 * ---------------------------------------------
 *********************************************/
#define Fld(wid, shft, ac)		(((uint32_t)wid<<16)|(shft<<8)|ac)
#define Fld_wid(fld)			(uint8_t)((fld)>>16)
#define Fld_shft(fld)			(uint8_t)((fld)>>8)
#define Fld_ac(fld)				(uint8_t)((fld))

#define REG16_MASK_SHIFT_WRITE(offset, mask, lshift, value) \
	reg16_mask_write(offset, mask, ((value) << (lshift)))
#define REG32_MASK_SHIFT_WRITE(offset, mask, lshift, value) \
	reg32_mask_write(offset, mask, ((value) << (lshift)))

#define REG32_SET_BIT(offset, nr) \
	reg32_set_mask(offset, (1 << nr))
#define REG32_CLEAR_BIT(offset, nr) \
	reg32_clear_mask(offset, (1 << nr))

/*********************************************
 * Prototype and Macro
 *********************************************/
static inline uint8_t UNUSED
	u1IO32Read1B(REG16 *reg32);
static inline uint16_t UNUSED
	u2IO32Read2B(REG16 *reg32);
static inline uint32_t UNUSED
	u4IO32Read4B(REG16 *reg32);
static inline uint32_t UNUSED
	u4IO32ReadFld(REG16 *reg32, uint32_t fld);
static inline void UNUSED
	vIO32Write1B(REG16 *reg32, uint8_t val8);
static inline void UNUSED
	vIO32Write2B(REG16 *reg32, uint16_t val16);
static inline void UNUSED
	vIO32Write4B(REG16 *reg32, uint32_t val32);
#define vIO32WriteFld(reg32, val32, fld) \
	__vIO32WriteFldMulti(0, reg32, 1, val32, fld)
#define vIO32WriteFldMulti(reg32, num_fld, ...) \
	__vIO32WriteFldMulti(0, reg32, num_fld, __VA_ARGS__)
#define vIO32WriteFldMultiMask(reg32, num_fld, ...) \
	__vIO32WriteFldMulti(1, reg32, num_fld, __VA_ARGS__)

static void RegWrite4B(REG16 *reg32_l, REG16 *reg32_h, uint32_t val);
static uint32_t RegRead4B(REG16 *reg32_l, REG16 *reg32_h);

enum reg_base {
	REG_BASE_MIN = 0,

	REG_BASE_TSP_MIN = REG_BASE_MIN,
	REG_BASE_TSP_0 = REG_BASE_TSP_MIN,
	REG_BASE_TSP_1,
	REG_BASE_TSP_2,
	REG_BASE_TSP_3,
	REG_BASE_TSP_4,
	REG_BASE_TSP_5,
	REG_BASE_TSP_6,
	REG_BASE_TSP_7,
	REG_BASE_TSP_MAX,

	REG_BASE_TSO_MIN = 0x10,
	REG_BASE_TSO_0 = REG_BASE_TSO_MIN,
	REG_BASE_TSO_1,
	REG_BASE_TSO_2,
	REG_BASE_TSO_MAX,

	REG_BASE_TLV_MIN = 0x20,
	REG_BASE_TLV_0 = REG_BASE_TLV_MIN,
	REG_BASE_TLV_1,
	REG_BASE_TLV_MAX,

	REG_BASE_PAS_MIN = 0x30,
	REG_BASE_PAS_0 = REG_BASE_PAS_MIN,
	REG_BASE_PAS_1,
	REG_BASE_PAS_MAX,

	REG_BASE_CLKGEN_MIN = 0x40,
	REG_BASE_CLKGEN_0 = REG_BASE_CLKGEN_MIN,
	REG_BASE_CLKGEN_MAX,

	REG_BASE_TS_SAMPLE_MIN = 0x50,
	REG_BASE_TS_SAMPLE_0 = REG_BASE_TS_SAMPLE_MIN,
	REG_BASE_TS_SAMPLE_MAX,

	REG_BASE_CILINK_MIN = 0x60,
	REG_BASE_CILINK_0 = REG_BASE_CILINK_MIN,
	REG_BASE_CILINK_MAX,

	REG_BASE_ALP_MIN = 0x70,
	REG_BASE_ALP_0 = REG_BASE_ALP_MIN,
	REG_BASE_ALP_1,
	REG_BASE_ALP_MAX,

	REG_BASE_MAX = REG_BASE_ALP_MAX,
};

struct reg {
	enum reg_base base;
	u32 offset;
	u32 fld;
};

struct reg_info {
	int reg_type;
	int index; // path, clk_src or pvr_eng_id
	struct reg reg;
};

struct reg_infos {
	u16 size;
	struct reg_info *reg_info;
};

/*********************************************
 * Functions
 *********************************************/
static uint32_t UNUSED __u4IO32AccessFld(
	uint8_t write, uint32_t tmp32, uint32_t val32, uint32_t fld)
{
	uint32_t t = 0;

	switch (Fld_ac(fld)) {
	case AC_FULLB0:
	case AC_FULLB1:
	case AC_FULLB2:
	case AC_FULLB3:
		if (write == 1)
			t =
			(tmp32 & (~((uint32_t) 0xFF <<
			(8 * (Fld_ac(fld) - AC_FULLB0))))) |
			((val32 & 0xFF) <<
			(8 * (Fld_ac(fld) - AC_FULLB0)));
		else
			t =
			(tmp32 & ((uint32_t) 0xFF <<
			(8 * (Fld_ac(fld) - AC_FULLB0)))) >>
			(8 * (Fld_ac(fld) - AC_FULLB0));
		break;
	case AC_FULLW10:
	case AC_FULLW21:
	case AC_FULLW32:
		if (write == 1)
			t =
			(tmp32 & (~((uint32_t) 0xFFFF <<
			(8 * (Fld_ac(fld) - AC_FULLW10))))) |
			((val32 & 0xFFFF) <<
			(8 * (Fld_ac(fld) - AC_FULLW10)));
		else
			t =
			(tmp32 & (((uint32_t) 0xFFFF <<
			(8 * (Fld_ac(fld) - AC_FULLW10))))) >>
			(8 * (Fld_ac(fld) - AC_FULLW10));
		break;
	case AC_FULLDW:
		t = val32;
		break;
	case AC_MSKB0:
	case AC_MSKB1:
	case AC_MSKB2:
	case AC_MSKB3:
		if (write == 1)
			t =
				(tmp32 & (~((((uint32_t) 1 <<
				Fld_wid(fld)) - 1) <<
				Fld_shft(fld)))) |
				((val32 & (((uint32_t) 1 <<
				Fld_wid(fld)) - 1)) <<
				Fld_shft(fld));
		else
			t =
			(tmp32 & ((((uint32_t) 1 <<
			Fld_wid(fld)) - 1) <<
			Fld_shft(fld))) >>
			Fld_shft(fld);
		break;
	case AC_MSKW10:
	case AC_MSKW21:
	case AC_MSKW32:
		if (write == 1)
			t =
			(tmp32 & (~(((((uint32_t) 1 <<
			Fld_wid(fld)) - 1) <<
			Fld_shft(fld))))) |
			(((val32 & (((uint32_t) 1 <<
			Fld_wid(fld)) - 1)) <<
			Fld_shft(fld)));
		else
			t =
			(tmp32 & (((((uint32_t) 1 <<
			Fld_wid(fld)) - 1) <<
			Fld_shft(fld)))) >>
			Fld_shft(fld);
		break;
	case AC_MSKDW:
		if (write == 1)
			t =
			(tmp32 & (~(((((uint32_t) 1 <<
			Fld_wid(fld)) - 1) <<
			Fld_shft(fld))))) |
			(((val32 & (((uint32_t) 1 <<
			Fld_wid(fld)) - 1)) <<
			Fld_shft(fld)));
		else
			t =
			(tmp32 & (((((uint32_t) 1 <<
			Fld_wid(fld)) - 1) <<
			Fld_shft(fld)))) >>
			Fld_shft(fld);
		break;
	default:
		break;
	}
	return t;
}

static void UNUSED __vIO32WriteFldMulti(
	uint32_t mask, REG16 *reg32, uint32_t num_fld, ...)
{
	uint32_t i, val32, fld, tmp32;
	va_list argp;

	tmp32 = (mask == 0)?u4IO32Read4B(reg32):0;
	va_start(argp, num_fld);
	for (i = 0; i < num_fld; i++) {
		val32 = va_arg(argp, uint32_t);
		fld = va_arg(argp, uint32_t);
		tmp32 = __u4IO32AccessFld(1, tmp32, val32, fld);
	}
	va_end(argp);
	vIO32Write4B(reg32, tmp32);
}

static inline uint8_t UNUSED u1IO32Read1B(REG16 *reg32)
{
	return readb(reg32);
}

static inline uint16_t UNUSED u2IO32Read2B(REG16 *reg32)
{
	return readw(reg32);
}

static inline uint32_t UNUSED u4IO32Read4B(REG16 *reg32)
{
	return readl(reg32);
}

static inline uint32_t UNUSED u4IO32ReadFld(REG16 *reg32, uint32_t fld)
{
	return __u4IO32AccessFld(0, u4IO32Read4B(reg32), 0, fld);
}

static inline void UNUSED vIO32Write1B(REG16 *reg32, uint8_t val8)
{
	writeb(val8, reg32);
}

static inline void UNUSED vIO32Write2B(REG16 *reg32, uint16_t val16)
{
	writew(val16, reg32);
}

static inline void UNUSED vIO32Write4B(REG16 *reg32, uint32_t val32)
{
	writel(val32, reg32);
}

static void UNUSED RegWrite4B(REG16 *reg32_l, REG16 *reg32_h, uint32_t val)
{
	vIO32Write2B(reg32_l, val);
	vIO32Write2B(reg32_h, (val >> 16));
}

static uint32_t UNUSED RegRead4B(REG16 *reg32_l, REG16 *reg32_h)
{
	uint32_t val_low, val_high;

	val_low = (uint32_t) u2IO32Read2B(reg32_l);
	val_high = (uint32_t) u2IO32Read2B(reg32_h);
	return ((val_high << 16) + val_low);
}

bool send_mcu_cmd(u32 cmd, u32 *pmsg0, u32 *pmsg1);
void indirect_write(u32 OR_addr, u32 value);
u32	 indirect_read(u32 OR_addr);
void reg32_write(REG16 __iomem *offset, u32 value);
u32	 reg32_read(REG16 __iomem *offset);
void reg32_set_mask(REG16 __iomem *offset, u32 mask);
void reg32_clear_mask(REG16 __iomem *offset, u32 mask);
void reg32_mask_write(REG16 __iomem *offset, u32 mask, u32 value);
u16 reg32_mask_read(REG16 __iomem *offset, u32 mask, u32 shift);
void reg32_fld_write(struct reg reg, u32 value);
u32 reg32_fld_read(struct reg reg);
void reg16_write(REG16 __iomem *offset, u16 value);
u16	 reg16_read(REG16 __iomem *offset);
void reg16_set_bit(REG16 __iomem *offset, u8 nr);
void reg16_clear_bit(REG16 __iomem *offset, u8 nr);
bool reg16_test_bit(REG16 __iomem *offset, u8 nr);
void reg16_set_mask(REG16 __iomem *offset, u16 mask);
void reg16_clear_mask(REG16 __iomem *offset, u16 mask);
void reg16_mask_write(REG16 __iomem *offset, u16 mask, u16 value);
u16	 reg16_mask_read(REG16 __iomem *offset, u16 mask, u16 shift);

int reg_base_get(enum reg_base reg_base, REG16 __iomem **base);
int reg_save(enum reg_base reg_base);
void reg_restore(enum reg_base reg_base);

int reg_get(struct reg_infos *reg_infos,
		int reg_type,
		u16 index,
		struct reg *reg);

#endif /* MTK_DMX_RIU_H */
