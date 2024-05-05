/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef _REG_VPAS_H_
#define _REG_VPAS_H_

#include "mtk_dmx_regs.h"

//----------------------------------------
//	Macro and Define
//----------------------------------------
// RIU macro

static u64 u64RiuVPASBaseAdd;
static u64 u64RiuCLKGENBaseAdd;
#define VPAS_LWORD(x)	 (u16)((x)&0xffff)
#define VPAS_HWORD(x)	 (u16)(((x)>>16)&0xffff)


#define VPAS_RIU_BASE		 u64RiuVPASBaseAdd
#define CLKGENBASE		  u64RiuCLKGENBaseAdd


#define VPAS_RIU_READ_BYTE(addr) \
	(reg16_mask_read((REG16 *)(VPAS_RIU_BASE + (addr)), 0xFF, 0))
#define VPAS_RIU_READ_WORD(addr) \
	(reg16_read((REG16 *)(VPAS_RIU_BASE + (addr))))
#define VPAS_RIU_WRITE_BYTE(addr, val) \
	{reg16_mask_write((REG16 *)(VPAS_RIU_BASE+(addr)), 0xFF, val); }
#define VPAS_RIU_WRITE_WORD(addr, val) \
	{reg16_write((REG16 *)(VPAS_RIU_BASE+(addr)), val); }


#define VPAS_ReadByte(u32Reg) \
	VPAS_RIU_READ_BYTE(((u32Reg) << 1) - ((u32Reg) & 1))

#define VPAS_Read2Byte(u32Reg) \
	(VPAS_RIU_READ_WORD((u32Reg)<<1))

#define VPAS_Read4Byte(u32Reg) \
	((u32)VPAS_RIU_READ_WORD((u32Reg)<<1) | \
	((u32)VPAS_RIU_READ_WORD(((u32Reg)+2)<<1)<<16))

#define VPAS_ReadRegBit(u32Reg, u8Mask) \
	(VPAS_RIU_READ_BYTE(((u32Reg)<<1) - ((u32Reg) & 1)) & (u8Mask))

#define VPAS_ReadWordBit(u32Reg, u16Mask) \
	(VPAS_Read2Byte(u32Reg) & (u16Mask))

#define VPAS_WriteByte(u32Reg, u8Val) \
	VPAS_RIU_WRITE_BYTE(((u32Reg) << 1) - ((u32Reg) & 1), u8Val)

#define VPAS_Write2Byte(u32Reg, u16Val) \
	do { \
		if (((u32Reg) & 0x01)) { \
			VPAS_RIU_WRITE_BYTE(((u32Reg) << 1) - 1, (u8)((u16Val))); \
			VPAS_RIU_WRITE_BYTE(((u32Reg) + 1) << 1, (u8)((u16Val) >> 8)); \
		} else \
			VPAS_RIU_WRITE_WORD(((u32Reg)<<1),	u16Val); \
	} while (0)

#define VPAS_Write4Byte(u32Reg, u32Val) \
	do { \
		if ((u32Reg) & 0x01) { \
			VPAS_RIU_WRITE_BYTE(((u32Reg) << 1) - 1,  u32Val); \
			VPAS_RIU_WRITE_WORD(((u32Reg) + 1)<<1, ((u32Val) >> 8)); \
			VPAS_RIU_WRITE_BYTE((((u32Reg) + 3) << 1),	((u32Val) >> 24)); \
		} else { \
			VPAS_RIU_WRITE_WORD((u32Reg) << 1,	u32Val); \
			VPAS_RIU_WRITE_WORD(((u32Reg) + 2)<<1,	((u32Val) >> 16)); \
		} \
	} while (0)

#define VPAS_WriteByteMask(u32Reg, u8Val, u8Msk) \
	VPAS_RIU_WRITE_BYTE((((u32Reg) << 1) - ((u32Reg) & 1)), \
	(VPAS_RIU_READ_BYTE((((u32Reg) << 1) - ((u32Reg) & 1))) & ~(u8Msk)) | \
	((u8Val) & (u8Msk)))

#define VPAS_WriteWordMask(u32Reg, u16Val, u16Msk) \
	do { \
		if (((u32Reg) & 0x01)) { \
			VPAS_WriteByteMask(((u32Reg) + 1), \
			(((u16Val) & 0xff00) >> 8), \
			(((u16Msk) & 0xff00) >> 8)); \
			VPAS_WriteByteMask((u32Reg), ((u16Val) & 0x00ff), ((u16Msk) & 0x00ff)); \
		} else { \
			VPAS_RIU_WRITE_WORD(((u32Reg) << 1),\
			(((u16Val) & (u16Msk)) | \
			(VPAS_Read2Byte(u32Reg) & (~(u16Msk))))); \
		} \
	} while (0)


//=======================================================
#define CLKGEN_RIU_READ_BYTE(addr) \
	(reg16_mask_read((REG16 *)(CLKGENBASE + (addr)), 0xFF, 0))

#define CLKGEN_RIU_WRITE_BYTE(addr, val) \
	{reg16_mask_write((REG16 *)(CLKGENBASE+(addr)), 0xFF, val); }

#define CLKGEN_RIU_READ_WORD(addr) \
	(reg16_read((REG16 *)(CLKGENBASE + (addr))))

#define CLKGEN_RIU_WRITE_WORD(addr, val) \
	{ reg16_write((REG16 *)(CLKGENBASE+(addr)), val); }

#define CLKGEN_Read2Byte(u32Reg) \
	(CLKGEN_RIU_READ_WORD((u32Reg)<<1))

#define CLKGEN_WriteByteMask(u32Reg, u8Val, u8Msk) \
	CLKGEN_RIU_WRITE_BYTE((((u32Reg) << 1) - ((u32Reg) & 1)), \
	(CLKGEN_RIU_READ_BYTE((((u32Reg) << 1) - ((u32Reg) & 1))) & ~(u8Msk)) | \
	((u8Val) & (u8Msk)))

#define CLKGEN_WriteWordMask(u32Reg, u16Val, u16Msk) \
	do { \
		if (((u32Reg) & 0x01)) { \
			CLKGEN_WriteByteMask(((u32Reg) + 1), \
			(((u16Val) & 0xff00) >> 8), \
			(((u16Msk) & 0xff00) >> 8)); \
			CLKGEN_WriteByteMask((u32Reg), ((u16Val) & 0x00ff), \
			((u16Msk)&0x00ff)); \
		} else \
			CLKGEN_RIU_WRITE_WORD(((u32Reg) << 1), \
			(((u16Val) & (u16Msk)) | \
			(CLKGEN_Read2Byte(u32Reg) & (~(u16Msk))))); \
	} while (0)

//#define BIT(bit) (1 << (bit))
#define BITS(bits, value) \
	((BIT(((1)?bits) + 1) - BIT(((0)?bits))) & (value << ((0)?bits)))
#define BMASK(bits) \
	(BIT(((1)?bits) + 1) - BIT(((0)?bits)))


//------------------------------------------------------------------------------
// Base Address
//------------------------------------------------------------------------------
//#define VPAS_REG_BASE 0x1100UL
//#define REG_CHIPTOP_BASE 0x0B00UL		  //chiptop CLKGEN0
#define VPAS_REG_BASE 0
#define REG_CHIPTOP_BASE 0	//chiptop CLKGEN0


//------------------------------------------------------------------------------
// VPAS Reg
//------------------------------------------------------------------------------
// Configure PAS
#define REG_VPAS_SW_RST_CTRL                            (VPAS_REG_BASE + ((0x00) << 1))
    #define BIT_REG_SWRST                                               BIT(0)
    #define BIT_REG_SW_DMA_BBU_RST                            BIT(1)
    #define BIT_REG_SW_PAS1_REG_RST                           BIT(2)
    #define BIT_REG_SW_PAS2_REG_RST                           BIT(3)
    #define BIT_REG_SW_PAS3_REG_RST                           BIT(4)
    #define BIT_REG_SW_PAS4_REG_RST                           BIT(5)
    #define BIT_REG_PAS1WADR_MODE                              BIT(12)
    #define BIT_REG_PAS2WADR_MODE                              BIT(13)
    #define BIT_REG_PAS3WADR_MODE                              BIT(14)
    #define BIT_REG_PAS4WADR_MODE                              BIT(15)

#define REG_VPAS_BBU_DMA_CTRL                        (VPAS_REG_BASE + ((0x01) << 1))
    #define BIT_REG_DMA_BBU_BUSY                                 BIT(0)
    #define BIT_REG_MI_BBU_BUSY                                    BIT(1)
    #define BIT_REG_DMA_BBU_STOP_DONE                      BIT(2)
    #define BIT_REG_MI_BBU_BREAK_DONE                       BIT(3)
    #define BIT_REG_DMA_BBU_STOP_TIMEOUT                BIT(4)
    #define BIT_REG_MI_BBU_BREAK_TIMEOUT                 BIT(5)
    #define BIT_REG_DMA_BBU_STOP                                BIT(6)
    #define BIT_REG_MI_BBU_BREAK                                 BIT(7)
    #define BIT_REG_DMA_PAS1_STOP                              BIT(8)
    #define BIT_REG_DMA_PAS2_STOP                              BIT(9)
    #define BIT_REG_DMA_PAS3_STOP                              BIT(10)
    #define BIT_REG_DMA_PAS4_STOP                              BIT(11)
    #define BIT_REG_DMA_PTS1_STOP                              BIT(12)
    #define BIT_REG_DMA_PTS2_STOP                              BIT(13)
    #define BIT_REG_DMA_PTS3_STOP                              BIT(14)
    #define BIT_REG_DMA_PTS4_STOP                              BIT(15)


#define REG_VPAS_VBUF1_WADR_LO          (VPAS_REG_BASE + ((0x08) << 1))
#define REG_VPAS_VBUF1_WADR_HI          (VPAS_REG_BASE + ((0x09) << 1))

#define REG_VPAS_BASE_CTRL                    (VPAS_REG_BASE + ((0x07) << 1))
    #define BIT_REG_VBUF1_WADR_RELOAD                        BIT(0)
    #define BIT_REG_VBUF2_WADR_RELOAD                        BIT(1)
    #define BIT_REG_VBUF3_WADR_RELOAD                        BIT(2)
    #define BIT_REG_VBUF4_WADR_RELOAD                        BIT(3)
    #define BIT_REG_CUR_ESB1_RADR_RELOAD                  BIT(4)
    #define BIT_REG_CUR_ESB2_RADR_RELOAD                  BIT(5)
    #define BIT_REG_CUR_ESB3_RADR_RELOAD                  BIT(6)
    #define BIT_REG_CUR_ESB4_RADR_RELOAD                  BIT(7)
    #define BIT_REG_SECURITY_VBUF1_LOCK                     BIT(8)
    #define BIT_REG_SECURITY_VBUF2_LOCK                     BIT(9)
    #define BIT_REG_SECURITY_VBUF3_LOCK                     BIT(10)
    #define BIT_REG_SECURITY_VBUF4_LOCK                     BIT(11)

#define REG_VPAS_CUR_ESB1_RADR_LO           (VPAS_REG_BASE + ((0x0a) << 1))
#define REG_VPAS_CUR_ESB1_RADR_HI           (VPAS_REG_BASE + ((0x0b) << 1))

#define REG_VPAS_CUR_ESB2_RADR_LO           (VPAS_REG_BASE + ((0x16) << 1))
#define REG_VPAS_CUR_ESB2_RADR_HI           (VPAS_REG_BASE + ((0x17) << 1))

#define REG_VPAS_VBUF1_UBND_LO             (VPAS_REG_BASE + ((0x0C) << 1))
#define REG_VPAS_VBUF1_UBND_HI             (VPAS_REG_BASE + ((0x0D) << 1))
#define REG_VPAS_VBUF1_LBND_LO             (VPAS_REG_BASE + ((0x0E) << 1))
#define REG_VPAS_VBUF1_LBND_HI             (VPAS_REG_BASE + ((0x0F) << 1))

#define REG_VPAS_VBUF1_OVFTH_LO           (VPAS_REG_BASE + ((0x10) << 1))
#define REG_VPAS_VBUF1_OVFTH_HI           (VPAS_REG_BASE + ((0x11) << 1))
#define REG_VPAS_VBUF1_UDFTH_LO           (VPAS_REG_BASE + ((0x12) << 1))
#define REG_VPAS_VBUF1_UDFTH_HI           (VPAS_REG_BASE + ((0x13) << 1))

#define REG_VPAS_VBUF2_WADR_LO          (VPAS_REG_BASE + ((0x14) << 1))
#define REG_VPAS_VBUF2_WADR_HI          (VPAS_REG_BASE + ((0x15) << 1))

#define REG_VPAS_VBUF2_UBND_LO             (VPAS_REG_BASE + ((0x18) << 1))
#define REG_VPAS_VBUF2_UBND_HI             (VPAS_REG_BASE + ((0x19) << 1))
#define REG_VPAS_VBUF2_LBND_LO             (VPAS_REG_BASE + ((0x1a) << 1))
#define REG_VPAS_VBUF2_LBND_HI             (VPAS_REG_BASE + ((0x1b) << 1))

#define REG_VPAS_VBUF2_OVFTH_LO           (VPAS_REG_BASE + ((0x1c) << 1))
#define REG_VPAS_VBUF2_OVFTH_HI           (VPAS_REG_BASE + ((0x1d) << 1))
#define REG_VPAS_VBUF2_UDFTH_LO           (VPAS_REG_BASE + ((0x1e) << 1))
#define REG_VPAS_VBUF2_UDFTH_HI           (VPAS_REG_BASE + ((0x1f) << 1))

#define REG_VPAS_VBUF3_WADR_LO          (VPAS_REG_BASE + ((0x20) << 1))
#define REG_VPAS_VBUF3_WADR_HI          (VPAS_REG_BASE + ((0x21) << 1))

#define REG_VPAS_VBUF3_UBND_LO             (VPAS_REG_BASE + ((0x24) << 1))
#define REG_VPAS_VBUF3_UBND_HI             (VPAS_REG_BASE + ((0x25) << 1))
#define REG_VPAS_VBUF3_LBND_LO             (VPAS_REG_BASE + ((0x26) << 1))
#define REG_VPAS_VBUF3_LBND_HI             (VPAS_REG_BASE + ((0x27) << 1))

#define REG_VPAS_VBUF3_OVFTH_LO           (VPAS_REG_BASE + ((0x28) << 1))
#define REG_VPAS_VBUF3_OVFTH_HI           (VPAS_REG_BASE + ((0x29) << 1))
#define REG_VPAS_VBUF3_UDFTH_LO           (VPAS_REG_BASE + ((0x2a) << 1))
#define REG_VPAS_VBUF3_UDFTH_HI           (VPAS_REG_BASE + ((0x2b) << 1))

#define REG_VPAS_VBUF4_WADR_LO          (VPAS_REG_BASE + ((0x2c) << 1))
#define REG_VPAS_VBUF4_WADR_HI          (VPAS_REG_BASE + ((0x2d) << 1))

#define REG_VPAS_VBUF4_UBND_LO             (VPAS_REG_BASE + ((0x30) << 1))
#define REG_VPAS_VBUF4_UBND_HI             (VPAS_REG_BASE + ((0x31) << 1))
#define REG_VPAS_VBUF4_LBND_LO             (VPAS_REG_BASE + ((0x32) << 1))
#define REG_VPAS_VBUF4_LBND_HI             (VPAS_REG_BASE + ((0x33) << 1))

#define REG_VPAS_VBUF4_OVFTH_LO           (VPAS_REG_BASE + ((0x34) << 1))
#define REG_VPAS_VBUF4_OVFTH_HI           (VPAS_REG_BASE + ((0x35) << 1))
#define REG_VPAS_VBUF4_UDFTH_LO           (VPAS_REG_BASE + ((0x36) << 1))
#define REG_VPAS_VBUF4_UDFTH_HI           (VPAS_REG_BASE + ((0x37) << 1))


#define REG_VPAS_VBUF_CADDR_CTRL        (VPAS_REG_BASE + ((0x38) << 1))
    #define BIT_REG_VBUF_CADDR_RELOAD               BIT(0)
    #define BIT_REG_VBUF_CADDR_SEL                      BITS(5:4, 0)

#define REG_VPAS_VBUF_CADDR_LO        (VPAS_REG_BASE + ((0x39) << 1))
#define REG_VPAS_VBUF_CADDR_HI        (VPAS_REG_BASE + ((0x3a) << 1))

#define REG_VPAS_DBG_CTRL                      (VPAS_REG_BASE + ((0x3B) << 1))
    #define BIT_REG_BIST_FAIL                                          BITS(3:0, 0)
    #define BIT_REG_MEM_OFF_DLY_PD                              BITS(7:4, 0)
    #define BIT_REG_MEM_OFF_DIS_QUALIFY                              BIT(8)
    #define BIT_REG_DEBUG_SEL                                         BITS(11:9, 0)
    #define BIT_REG_DEBUG_MUX_SEL                                BITS(15:12 0)


#define REG_VPAS_PTS_BUF      (VPAS_REG_BASE + ((0x50) << 1))
    #define BIT_REG_PTS_BUF1_WADR_RELOAD                BIT(0)
    #define BIT_REG_PTS_BUF2_WADR_RELOAD                BIT(1)
    #define BIT_REG_PTS_BUF3_WADR_RELOAD                BIT(2)
    #define BIT_REG_PTS_BUF4_WADR_RELOAD                BIT(3)
    #define BIT_REG_PTS_BUF1_RADR_RELOAD                 BIT(4)
    #define BIT_REG_PTS_BUF2_RADR_RELOAD                 BIT(5)
    #define BIT_REG_PTS_BUF3_RADR_RELOAD                 BIT(6)
    #define BIT_REG_PTS_BUF4_RADR_RELOAD                 BIT(7)
    #define BIT_REG_PTS_BUF_SEL                                     BITS(9:8, 0)
    #define BIT_REG_PTS_BUF_TYPE                                   BITS(12:10, 0)


#define REG_VPAS_PTS_BUF_DATA_LO      (VPAS_REG_BASE + ((0x51) << 1))
#define REG_VPAS_PTS_BUF_DATA_HI      (VPAS_REG_BASE + ((0x52) << 1))

#define REG_VPAS_INT_CTRL                        (VPAS_REG_BASE + ((0x58) << 1))
    #define BIT_REG_VD_PAS_INT_EN                                BIT(0)
    #define BIT_REG_VD_PAS_INT_CLR                               BIT(1)
    #define BIT_REG_VD_PAS_INT_STATUS                        BIT(2)

#define REG_VPAS_INT_MASK                        (VPAS_REG_BASE + ((0x59) << 1))
    #define BIT_REG_MASK_PAS1_PTS                                 BIT(0)
    #define BIT_REG_MASK_PAS1_PFLAG_MISMATCH          BIT(1)
    #define BIT_REG_MASK_PAS1_TLV_FLAG_MISMATCH    BIT(2)
    #define BIT_REG_MASK_PAS2_PTS                                 BIT(3)
    #define BIT_REG_MASK_PAS2_PFLAG_MISMATCH          BIT(4)
    #define BIT_REG_MASK_PAS2_TLV_FLAG_MISMATCH    BIT(5)
    #define BIT_REG_MASK_PAS3_PTS                                 BIT(6)
    #define BIT_REG_MASK_PAS3_PFLAG_MISMATCH          BIT(7)
    #define BIT_REG_MASK_PAS3_TLV_FLAG_MISMATCH    BIT(8)
    #define BIT_REG_MASK_PAS4_PTS                                 BIT(9)
    #define BIT_REG_MASK_PAS4_PFLAG_MISMATCH          BIT(10)
    #define BIT_REG_MASK_PAS4_TLV_FLAG_MISMATCH    BIT(11)



#define REG_VPAS_PAS1_HK_CTL                 (VPAS_REG_BASE + ((0x61) << 1))
#define REG_VPAS_PAS2_HK_CTL                 (VPAS_REG_BASE + ((0x71) << 1))
    #define BIT_REG_PAS_HK_PES_SCRAMBL_CTL     BITS(1:0, 0)
    #define BIT_REG_PAS_CLR_SW_PAS_RST             BIT(2)
    #define BIT_REG_PAS_PAS_TLV_MODE                 BIT(3)
    #define BIT_REG_PAS_TS_PUSI_DIS                     BIT(4)
    #define BIT_REG_PAS_TS_PUSI1B_CPAS1_HK      BIT(5)
    #define BIT_REG_PAS_PAS_ID_MSB                      BIT(7)
    #define BIT_REG_PAS_PAS_ID                              BITS(9:8, 0)
    #define BIT_REG_PAS_PAS_ID_UPD                      BIT(10)
    #define BIT_REG_PAS_PAS_ID_LOCK                    BIT(11)
    #define BIT_REG_PAS_PAS_SEL_ID                      BITS(14:12, 0)
    #define BIT_REG_PAS_PAS_SEL_ID_MSB              BIT(15)


#define REG_VPAS_PAS1_HK_ID1                 (VPAS_REG_BASE + ((0x62) << 1))
    #define BIT_REG_PAS1_HK_ID1                      BITS(7:0, 0)
    #define BIT_REG_PAS1_HK_ID1_MASK           BITS(15:8, 0)

#define REG_VPAS_PAS1_HK_ID2                 (VPAS_REG_BASE + ((0x63) << 1))
    #define BIT_REG_PAS1_HK_ID2                      BITS(7:0, 0)
    #define BIT_REG_PAS1_HK_ID2_MASK           BITS(15:8, 0)

#define REG_VPAS_PAS2_HK_ID1                 (VPAS_REG_BASE + ((0x72) << 1))
    #define BIT_REG_PAS2_HK_ID1                      BITS(7:0, 0)
    #define BIT_REG_PAS2_HK_ID1_MASK           BITS(15:8, 0)

#define REG_VPAS_PAS2_HK_ID2                 (VPAS_REG_BASE + ((0x73) << 1))
    #define BIT_REG_PAS2_HK_ID2                      BITS(7:0, 0)
    #define BIT_REG_PAS2_HK_ID2_MASK           BITS(15:8, 0)

#define REG_VPAS_PAS1_CLR_INT           (VPAS_REG_BASE + ((0x68) << 1))
#define REG_VPAS_PAS2_CLR_INT           (VPAS_REG_BASE + ((0x78) << 1))
    #define BIT_REG_TLV_FLAG_SW                                 BIT(3)
    #define BIT_REG_TLV_FLAG_HW                                 BIT(4)
    #define BIT_REG_PAS_HK_TLV_INT_MASK                BIT(5)
    #define BIT_REG_PAS_HK_CLR_TLV_INT                   BIT(6)
    #define BIT_REG_PAS_HK_TLV_INT                           BIT(7)
    #define BIT_REG_PAS_HK_CLR_PTS_INT                   BIT(8)
    #define BIT_REG_PAS_HK_CLR_SNUM_INT                BIT(9)
    #define BIT_REG_PAS_HK_CLR_SID_INT                   BIT(10)
    #define BIT_REG_PAS_HK_CLR_P_FLAG_INT             BIT(11)
    #define BIT_REG_PAS_HK_PTS_INT                           BIT(12)
    #define BIT_REG_PAS_HK_SNUM_INT                        BIT(13)
    #define BIT_REG_PAS_HK_SID_INT                            BIT(14)
    #define BIT_REG_PAS_HK_P_FLAG_INT                     BIT(14)


#define REG_VPAS_PAS1_HK_INT_MASK      (VPAS_REG_BASE + ((0x69) << 1))
#define REG_VPAS_PAS2_HK_INT_MASK      (VPAS_REG_BASE + ((0x79) << 1))
    #define BIT_REG_PAS_HK_PTS_INT_MASK           BIT(0)
    #define BIT_REG_PAS_HK_SNUM_INT_MASK        BIT(1)
    #define BIT_REG_PAS_HK_SID_INT_MASK           BIT(2)
    #define BIT_REG_PAS_HK_P_FLAG_INT_MASK     BIT(3)
    #define BIT_REG_PAS_HK_FF_REQ_THRESH         BITS(7:4, 8)
    #define BIT_REG_PAS_HK_DMA_WCNT                 BITS(9:8, 1)
    #define BIT_REG_PAS_HK_FF_PRI_THRESH         BITS(14:10, 0xc)

#define REG_VPAS_PAS1_OBF               (VPAS_REG_BASE + ((0x6b) << 1))
#define REG_VPAS_PAS2_OBF               (VPAS_REG_BASE + ((0x7b) << 1))
    #define BIT_REG_PAS_HK_H264EN           BIT(0)
    #define BIT_REG_PAS_HK_MOBF_WREN        BIT(1)
    #define BIT_REG_PAS_HK_MOBF_RDEN           BIT(2)
    #define BIT_REG_PAS_HK_PTS_ESBUF_EN     BIT(3)
    #define BIT_REG_PAS_HK_DRAMOBF_WIDX         BITS(8:4, 0)
    #define BIT_REG_PAS_HK_DRAMOBF_RIDX         BITS(13:9, 0)

#define REG_VPAS_PAS1_P_FLAG               (VPAS_REG_BASE + ((0x6c) << 1))
#define REG_VPAS_PAS2_P_FLAG               (VPAS_REG_BASE + ((0x7c) << 1))
    #define BIT_REG_PAS_HK_P_FLAG_SW                                   BMASK(1:0)
    #define BIT_REG_PAS_HK_FILEIN_VALID_IGNORE                 BIT(2)
    #define BIT_REG_PAS_HK_P_FLAG_MISMATCH_DROP_DIS     BIT(3)
    #define BIT_REG_P_FLAG_MISMATCH_SW                                 BITS(9:8, 0)
    #define BIT_REG_P_FLAG_MISMATCH_HW                                 BITS(11:10, 0)

#define REG_VPAS_PAS1_HK_PTS_LO           (VPAS_REG_BASE + ((0x6d) << 1))
#define REG_VPAS_PAS1_HK_PTS_HI           (VPAS_REG_BASE + ((0x6f) << 1))

#define REG_VPAS_PAS2_HK_PTS_LO           (VPAS_REG_BASE + ((0x7d) << 1))
#define REG_VPAS_PAS2_HK_PTS_HI           (VPAS_REG_BASE + ((0x7f) << 1))

#define REG_VPAS_PAS1_CTRL           (VPAS_REG_BASE + ((0x60) << 1))
#define REG_VPAS_PAS2_CTRL           (VPAS_REG_BASE + ((0x70) << 1))
    #define BIT_REG_PAS_HK_PARSER_DIS                BIT(0)
    #define BIT_REG_PAS_HK_PKT_LEN_EN                BIT(1)
    #define BIT_REG_PAS_HK_FLUSH                          BIT(2)
    #define BIT_REG_PAS_HK_PARSER_STOP              BIT(3)
    #define BIT_REG_PAS_HK_SW_PAS_RST               BIT(4)
    #define BIT_REG_PAS_HK_PTS_ERR_RTN              BIT(5)
    #define BIT_REG_PAS_HK_CLR_ES_COUNTER        BIT(6)
    #define BIT_REG_PAS_HK_VPID_CHG                    BIT(7)
    #define BIT_REG_PAS_HK_MPEG1                          BIT(8)
    #define BIT_REG_PAS_HK_CPAS1_HK_HK_ID_MATCH_ON          BIT(9)
    #define BIT_REG_PAS_HK_PARSER_PAUSE             BIT(10)
    #define BIT_REG_PAS_HK_ID_CPAS1_HK_QUALIFY_OFF            BIT(11)
    #define BIT_REG_PAS_HK_AUTO_BYPASS_SWITCH_ON             BIT(12)
    #define BIT_REG_PAS_HK_AUTO_BYPASS_SET       BIT(13)
    #define BIT_REG_PAS_HK_PARSER_DIS_ALWAYS_TSP_ACK       BIT(14)
    #define BIT_REG_PAS_HK_GATE_SCRAMBLE_ES       BIT(15)


//h67	h66	25	0	reg_pas1_hk_ec2reg_pas1_hk_es_counter
//h67	h67	14	14	reg_pas1_hk_ec2reg_pas1_hk_es_counter_reload
//h67	h67	15	15	reg_pas1_hk_ec2reg_pas1_hk_es_counter_sel
#define REG_VPAS_PAS1_ES_CNT_LO           (VPAS_REG_BASE + ((0x66) << 1))
#define REG_VPAS_PAS1_ES_CNT_HI           (VPAS_REG_BASE + ((0x67) << 1))

#define REG_VPAS_PAS2_ES_CNT_LO           (VPAS_REG_BASE + ((0x76) << 1))
#define REG_VPAS_PAS2_ES_CNT_HI           (VPAS_REG_BASE + ((0x77) << 1))



#define REG_VPAS_CKG					(REG_CHIPTOP_BASE+((0x46)<<1))
	#define BIT_REG_PAS_CKG_EN			BIT(4)


#endif // _REG_VPAS_H_
