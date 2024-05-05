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

#ifndef _M_PQU_RENDER_H_
#define _M_PQU_RENDER_H_

#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/types.h>
#else
#include "pqu_port.h"
#endif

//-----------------------------------------------------------------------------
//  Driver Capability
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macro and Define
//-----------------------------------------------------------------------------
#define M_PQU_RENDER_PQ_INFO_VERSION (1)
#define M_PQU_RENDER_PQ_INFO_TAG	"M_PQU_RENDER_PQ_INFO"

//-----------------------------------------------------------------------------
//  Type and Structure
//-----------------------------------------------------------------------------
struct meta_render_pnlgamma_info {
	uint16_t pnlgamma_curve_r[1024];
	uint16_t pnlgamma_curve_g[1024];
	uint16_t pnlgamma_curve_b[1024];
	uint32_t pnlgamma_curve_size;
	uint8_t  pnlgamma_version;

	bool     pnlgamma_enable;

	uint16_t pnlgamma_offset_r;
	uint16_t pnlgamma_offset_g;
	uint16_t pnlgamma_offset_b;
	uint16_t pnlgamma_gain_r;
	uint16_t pnlgamma_gain_g;
	uint16_t pnlgamma_gain_b;

	bool     pnlgamma_gainoffset_enbale;

	bool     render_ready;
};

struct meta_render_pqgamma_info {
	uint16_t pqgamma_curve_r[256];
	uint16_t pqgamma_curve_g[256];
	uint16_t pqgamma_curve_b[256];
	uint32_t pqgamma_curve_size;
	uint8_t  pqgamma_version;
	bool     pqgamma_enable;

	uint16_t pqgamma_offset_r;
	uint16_t pqgamma_offset_g;
	uint16_t pqgamma_offset_b;
	uint16_t pqgamma_gain_r;
	uint16_t pqgamma_gain_g;
	uint16_t pqgamma_gain_b;
	bool     pqgamma_gainoffset_enbale;
	bool     pqgamma_pregainoffset;

	uint16_t pqgamma_max_r;
	uint16_t pqgamma_max_g;
	uint16_t pqgamma_max_b;
	bool     pqgamma_maxvalue_enable;
};

struct meta_render_pqu_pq_info {
	struct meta_render_pnlgamma_info pnlgamma;
	struct meta_render_pqgamma_info pqgamma;
};

//-----------------------------------------------------------------------------
//  Function and Variable
//-----------------------------------------------------------------------------

#endif
