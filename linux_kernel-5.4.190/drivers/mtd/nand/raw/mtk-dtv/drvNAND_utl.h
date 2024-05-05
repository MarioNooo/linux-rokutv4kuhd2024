/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#ifndef __DRVNAND_UTL_H__
#define __DRVNAND_UTL_H__

#include "drvNAND.h"

#define PAIRED_PAGE_COUNT  0x200
extern struct _PAIRED_PAGE_MAP ga_tPairedPageMap[];

extern U32 drvNAND_ProbeReadSeq(void);
extern U32 drvNAND_CheckPartInfo(U32 u32_PageIdx);
extern U32 drvNAND_SearchCIS(void);
extern void drvNAND_ParseNandInfo(struct _NAND_FLASH_INFO *pNandInfo);
extern void dump_mem(unsigned char *buf, int cnt);
extern void dump_nand_info(struct _NAND_FLASH_INFO *pNandInfo);
extern void dump_part_records(struct _PARTITION_RECORD *pRecord, int cnt);
extern void dump_part_info(struct _PARTITION_INFO *pPartInfo);
extern void dump_nand_driver(struct _NAND_DRIVER *pNandDrv);
extern void dump_nand_PPM(void);

extern U32 drvNAND_CompareCISTag(U8 *tag);
extern U8  drvNAND_CountBits(U32 u32_x);
extern U32 drvNAND_CheckSum(U8 *pu8_Data, U32 u32_ByteCnt);

extern struct _PARTITION_RECORD *drvNAND_SearchPartition(struct _PARTITION_RECORD *pRecord,
	U16 u16_PartType);

extern U32 drvNAND_CheckSpareSpacePattern(U8 *au8_SpareBuf, U32 u32_PageIdx);
extern U32 drvNAND_IsGoodBlk(U16 u16_BlkIdx);
extern U32 drvNAND_MarkBadBlk(U16 u16_BlkIdx);
extern U32 drvNAND_ParseAddrToADMA(U8 *pu8_DMADataBuf,  U32 u32_ByteCnt,
	U8 u8_Direction, struct _ADMA_DESC *ADMADesc, U8 u8_IsEnd);

#endif /* __DRVNAND_UTL_H__ */
