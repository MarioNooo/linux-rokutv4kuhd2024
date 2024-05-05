// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#include "drvNAND.h"

U32 drvNAND_Init(void *priv)
{
	U32 u32_Err = UNFD_ST_SUCCESS;
	struct _NAND_DRIVER *pNandDrv = (struct _NAND_DRIVER *)drvNAND_get_DrvContext_address();

	memset((void *)pNandDrv, 0, sizeof(struct _NAND_DRIVER));
	pNandDrv->priv = priv;

	NC_PlatformInit();

	pNandDrv->pPartInfo = (struct _PARTITION_INFO *)drvNAND_get_DrvContext_PartInfo();
	memset((void *)pNandDrv->pPartInfo, 0, PARTINFO_BYTE_CNT);

	u32_Err = drvNAND_ProbeReadSeq();
	if (u32_Err != UNFD_ST_SUCCESS)
		return u32_Err;

	u32_Err = drvNAND_SearchCIS();
	if (u32_Err != UNFD_ST_SUCCESS)
		return u32_Err;

	u32_Err = nand_config_clock();
	if (u32_Err != UNFD_ST_SUCCESS)
		return u32_Err;

	#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
	REG_WRITE_UINT16(NC_MIE_INT_EN, BIT_NC_JOB_END_EN);
	#endif

	return u32_Err;
}
EXPORT_SYMBOL(drvNAND_Init);
