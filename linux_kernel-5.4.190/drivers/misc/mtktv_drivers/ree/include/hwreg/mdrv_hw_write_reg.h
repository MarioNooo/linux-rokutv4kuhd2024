////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020-2024 MediaTek Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MediaTek Inc. and be kept in strict confidence
// ("MediaTek Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MediaTek Confidential
// Information is unlawful and strictly prohibited. MediaTek hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MDRV_HW_WRITE_REG_H__
#define __MDRV_HW_WRITE_REG_H__

#ifdef __MDRV_HW_WRITE_REG_C__
#ifndef INTERFACE
#define INTERFACE
#endif
#else
#ifndef INTERFACE
#define INTERFACE	extern
#endif
#endif

typedef enum
{
	E_CFD_REG_IP_IP,
	E_CFD_REG_IP_OP,
	E_CFD_REG_IP_VOP,
	E_CFD_REG_IP_FO,
	E_CFD_REG_IP_VR,
	E_CFD_REG_IP_HDR,
	E_CFD_REG_IP_MAX,
} E_CFD_REG_IP;

typedef struct __attribute__ ((packed)){
	MS_U32 u32Addr;
	MS_U16 u16Value;
	MS_U16 u16Mask;
	MS_U16 u16Client;
} ST_HW_REG_TABLE;

typedef struct __attribute__ ((packed)){
	ST_HW_REG_TABLE *pstTable;
	MS_U16 u16Len;
	MS_U16 u16WriteLen;
} ST_HW_REG_TABLES;

INTERFACE void MDrv_HW_WriteRegTable(MS_U32 u32Addr, MS_U16 u16Value, MS_U16 u16Mask,
	MS_U16 u16Client, ST_HW_REG_TABLE *pstReg, MS_BOOL bForce);
INTERFACE void MDrv_HW_WriteRegsTable(MS_U32 u32Addr, MS_U16 u16Value, MS_U16 u16Mask,
	MS_U16 u16Client, ST_HW_REG_TABLES *pstReg, MS_BOOL bForce);

#undef INTERFACE
#endif		//__MDRV_HW_WRITE_REG_H__