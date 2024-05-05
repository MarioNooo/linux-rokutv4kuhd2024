#ifndef _MDRV_HW_DOLBY_H_
#define _MDRV_HW_DOLBY_H_

#include "mdrv_hw_pq_st.h"
#include "mhal_hw_pq_st.h"

#ifdef _MDRV_HW_DOLBY_C_
#ifndef INTERFACE
#define INTERFACE
#endif
#else
#ifndef INTERFACE
#define INTERFACE	extern
#endif
#endif

INTERFACE void MDrv_HW_DV_EnableDescrbSramExt(MS_BOOL bEnable,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE void MDrv_HW_DV_EnableDescrbReadBack(MS_BOOL bEnable,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE void MDrv_HW_DV_SetDescrbReadBackAddr(MS_U16 u16Addr,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE MS_U16 MDrv_HW_DV_GetDescrbCheckFlag(void);
INTERFACE MS_U16 MDrv_HW_DV_GetDescrbData(void);
INTERFACE MS_BOOL MDrv_HW_DV_IsCRCPass(MS_U8 u8PkgIdx);
INTERFACE MS_BOOL MDrv_HW_DV_IsCRCPacketPass(MS_U8 u8PkgNum);
#if DOLBY_GD_ENABLE
INTERFACE MS_BOOL MDrv_HW_DV_GetPWMDuty(MS_U8 u8PWMPort, MS_U32 *pu32Duty);
INTERFACE void MDrv_HW_DV_SetPWMDuty(MS_U8 u8PWMPort, MS_U32 u32Duty);
INTERFACE MS_BOOL MDrv_HW_DV_GetPWMPeroid(MS_U8 u8PWMPort, MS_U32 *pu32Period);
INTERFACE MS_U16 MDrv_HW_DV_ReadDummyGdValue(void);
INTERFACE void MDrv_HW_DV_SetDummyGdValue(MS_U16 u16GdValue,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE MS_U16 MDrv_HW_DV_IsDummyGdValueReady(void);
INTERFACE void MDrv_HW_DV_SetDummyGdValueReady(MS_U16 u16GdReadyValue,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
#endif
INTERFACE void MDrv_HW_DV_DolbyHDR10Enable(MS_BOOL bIsOn, MS_BOOL bIsHDMI,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);
INTERFACE MS_BOOL MDrv_HW_DV_IsHwBonded(void);
INTERFACE void MDrv_HW_DV_Init(ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);

#undef INTERFACE
#endif		//_MDRV_HW_DOLBY_H_
