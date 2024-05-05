#ifndef _MDRV_HW_CFD_H_
#define _MDRV_HW_CFD_H_

#include "mdrv_hw_pq_st.h"
#include "mhal_hw_pq_st.h"

#ifdef _MDRV_HW_CFD_C_
#ifndef INTERFACE
#define INTERFACE
#endif
#else
#ifndef INTERFACE
#define INTERFACE	extern
#endif
#endif

INTERFACE void MDrv_HW_CFD_SetDMPQBypass(MS_BOOL bEnable,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE MS_U16 MDrv_HW_CFD_GetHue(MS_U32 winSize);
INTERFACE MS_U16 MDrv_HW_CFD_GetSat(MS_U32 winSize);
INTERFACE void MDrv_HW_CFD_GetHDRFrameCount(MS_U8 *pu8FrameCount);
INTERFACE MS_BOOL MDrv_HW_CFD_IsDramBWRBypass(MS_U8 u8Win);
INTERFACE MS_BOOL MDrv_HW_CFD_IsDram444Format(MS_U8 u8Win);
INTERFACE void MDrv_HW_CFD_GetHDRDMAInfo(ST_HW_DMA_INFO *pstHDRDMAInfo);
INTERFACE void MDrv_HW_CFD_ConfigEmpDma(ST_HW_EMP_DMA_INFO *_apstEmpDmaInfo,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE void MDrv_HW_CFD_SetHDRPosition(EN_HW_CFD_HDR_POSITION enHDRPosition,
	ST_HW_REG_TABLES * pstRegs,MS_BOOL bForce);
INTERFACE EN_HW_CFD_HDR_POSITION MDrv_HW_CFD_GetHDRPosition(void);
INTERFACE void MDrv_HW_CFD_SetHDRPath(EN_HW_CFD_HDR_PATH enHDRPath,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE void MDrv_HW_CFD_SetRGB3DLutAdlProtect(MS_BOOL bEnable,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE void MDrv_HW_CFD_EnableHDRCLK(MS_BOOL bEnable,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE void MDrv_HW_CFD_SetHDROutRange(MS_BOOL bBypass,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE void MDrv_HW_CFD_InitHDR(ST_HW_REG_TABLES * pstRegs,MS_BOOL bForce);

#undef INTERFACE
#endif		//_MDRV_HW_CFD_H_
