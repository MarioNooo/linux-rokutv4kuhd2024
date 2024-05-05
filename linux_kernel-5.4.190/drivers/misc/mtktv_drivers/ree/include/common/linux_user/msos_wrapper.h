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

#ifndef __MSOS_WRAPPER_H__
#define __MSOS_WRAPPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define msos_malloc(size)			malloc(size);
#define msos_free(ptr) 				free(ptr);
#define msos_print(format, args...) printf("[%s:%d] "format, __FILE__, __LINE__, ##args)
#define msos_do_div(a, b)           ((a) / (b))
#define msos_do_div_s64(a, b)       ((a) / (b))

#endif // #ifndef __MSOS_WRAPPER_H__
