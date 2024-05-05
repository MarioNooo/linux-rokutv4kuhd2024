/* SPDX-License-Identifier: GPL-2.0
 * Copyright (c) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _PQU_COMMON_TEST_H_
#define _PQU_COMMON_TEST_H_

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#else
#include "pqu_port.h"
#endif
#include "m_pqu_pq.h"

//-----------------------------------------------------------------------------
//  Driver Capability
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macro and Define
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Type and Structure
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Function and Variable
//-----------------------------------------------------------------------------
int pqu_common_alg_pq_ui_process(struct st_pq_common_info *common_info, char* PqParam, int* ui_array,
	struct ST_TypeDesc* TypeDescTab, int TabSize, struct st_osd_ctrl *osd_ctrl, const int osd_num);

#endif
