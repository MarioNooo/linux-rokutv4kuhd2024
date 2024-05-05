/*
 *
 * Copyright (c) 2020-2024 MediaTek Inc.
 * All rights reserved.
 *
 * Unless otherwise stipulated in writing, any and all information contained
 * herein regardless in any format shall remain the sole proprietary of
 * MediaTek Inc. and be kept in strict confidence
 * ("MediaTek Confidential Information") by the recipient.
 * Any unauthorized act including without limitation unauthorized disclosure,
 * copying, use, reproduction, sale, distribution, modification, disassembling,
 * reverse engineering and compiling of the contents of MediaTek Confidential
 * Information is unlawful and strictly prohibited. MediaTek hereby reserves the
 * rights to any and all damages, losses, costs and expenses resulting therefrom.
 *
 */

/*
 *
 * @author Mediatek Inc.
 *
 */

#ifndef _HWREG_COMMON_H_
#define _HWREG_COMMON_H_

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/string.h>
#else
#include "pqu_port.h"
#endif
/* ============================================================================================== */
/* ----------------------------------------- Defines -------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ------------------------------------------- Macros ------------------------------------------- */
/* ============================================================================================== */

/* -------------- define file operations in kernel ----------------*/
#ifdef AN_VTS_LTPKTF_BUILD
	#define KF_OPEN             filp_open
	#define KF_CLOSE            filp_close
	#define KF_LLSEEK           vfs_llseek
	#define KF_READ             kernel_read
	#define KF_WRITE            kernel_write
#else
static inline int hwreg_dummy(void) {return 0;}
	#define KF_OPEN(...)        hwreg_dummy()
	#define KF_CLOSE(...)       hwreg_dummy()
	#define KF_LLSEEK(...)      hwreg_dummy()
	#define KF_READ(...)        hwreg_dummy()
	#define KF_WRITE(...)       hwreg_dummy()
#endif

/* ============================================================================================== */
/* ------------------------------------------ Enums --------------------------------------------- */
/* ============================================================================================== */

/* ============================================================================================== */
/* ----------------------------------------- Structs -------------------------------------------- */
/* ============================================================================================== */
struct reg_info {
	uint32_t addr;
	uint32_t val;
	uint32_t mask;
};

struct reg_info_ex {
	uint32_t addr;
	uint32_t val;
	uint32_t mask;
};

struct hwregOut{
	struct reg_info *reg;
	uint32_t regCount;
};

enum hwregWriteMethod {
	HWREG_WRITE_BY_RIU,
	HWREG_WRITE_BY_ML,
	HWREG_WRITE_BY_DS,
};

struct hwregWriteMethodInfo {
	enum hwregWriteMethod eMethod;
	struct hwregOut hwOut;
	uint32_t maxRegNum;
};
/* ============================================================================================== */
/* ---------------------------------------- Functions ------------------------------------------- */
/* ============================================================================================== */
int drv_hwreg_common_set_stub(bool stub);
int drv_hwreg_common_get_stub(bool *stub);

#endif // _HWREG_COMMON_H_
