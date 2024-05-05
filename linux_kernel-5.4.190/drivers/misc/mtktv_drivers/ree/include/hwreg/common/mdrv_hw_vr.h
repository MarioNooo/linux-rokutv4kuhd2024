#ifndef _MDRV_HW_VR_H_
#define _MDRV_HW_VR_H_

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#else
#include "pqu_port.h"
#endif

struct HeaderContent {
	uint32_t NumOfWIn;
	uint32_t miu;
	uint32_t VersionInfoSize;
	uint32_t sizePerWin;
	uint64_t Header_BaseAddr;
};

int MDrv_Set_DummyReg(struct HeaderContent header_content);

#endif		//_MDRV_HW_VR_H_

