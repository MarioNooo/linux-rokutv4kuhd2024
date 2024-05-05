#ifndef _MDRV_HW_TCH_H_
#define _MDRV_HW_TCH_H_

#ifdef _MDRV_HW_TCH_C_
#ifndef INTERFACE
#define INTERFACE
#endif
#else
#ifndef INTERFACE
#define INTERFACE	extern
#endif
#endif

INTERFACE MS_BOOL MDrv_HW_TCH_IsTCHHWBonded(void);
INTERFACE void MDrv_HW_TCH_EnableTCHHDR(MS_BOOL bEnableTCHHDR,
	ST_HW_REG_TABLES *pstRegs, MS_BOOL bForce);

#undef INTERFACE
#endif		//_MDRV_HW_TCH_H_
