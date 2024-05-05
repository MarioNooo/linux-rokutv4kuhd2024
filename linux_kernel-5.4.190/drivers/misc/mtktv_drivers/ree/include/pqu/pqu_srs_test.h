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

#ifndef _PQU_SRS_H_
#define _PQU_SRS_H_

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#else
#include "pqu_port.h"
#endif
#include "hwreg_pq_enhance_srs.h"

//-----------------------------------------------------------------------------
//  Driver Capability
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macro and Define
//-----------------------------------------------------------------------------
#define PER_DB_SIZE 0x18000
//-----------------------------------------------------------------------------
//  Type and Structure
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Function and Variable
//-----------------------------------------------------------------------------
int pqu_srs_test_hw_support(void);
int pqu_srs_test_reset(void);
int pqu_srs_test_get_db_base_address(void);
int pqu_srs_test_enable(void);
int pqu_srs_test_xiu2ahb_selection(void);
int pqu_srs_test_timing_gen_settings(void);
int pqu_srs_test_update_cmds(void);
int pqu_srs_test_update_database(void);
int pqu_srs_test_pattern_gen(void);
int pqu_srs_test_size_reporter(void);
#endif
