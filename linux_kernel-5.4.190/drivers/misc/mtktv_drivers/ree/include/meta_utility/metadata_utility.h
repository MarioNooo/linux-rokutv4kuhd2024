/* SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause */
/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2019 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2019 MediaTek Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/


#ifndef METADATA_UTILITY_H
#define METADATA_UTILITY_H
#ifdef __cplusplus
extern "C"{
#endif
#ifdef METADATA_SO
#include <stdbool.h>
#endif

/*
 * ktf_tvmetadata.c
 * EN_COMPONENT0_TAG: range 0x00000000 ~ 0x0000FFFF
 * m_pqu_cfd.h
 * EN_COMPONENT1_TAG: range 0x00010000 ~ 0x0001FFFF
 * m_pqu_pattern.h
 * EN_COMPONENT2_TAG: range 0x00020000 ~ 0x0002FFFF
 * m_pqu_pq.h
 * EN_COMPONENT3_TAG: range 0x00030000 ~ 0x0003FFFF
 * m_pqu_render.h
 * EN_COMPONENT4_TAG: range 0x00040000 ~ 0x0004FFFF
 * m_pq.h
 * EN_COMPONENT5_TAG: range 0x00050000 ~ 0x0005FFFF
 * m_render.h
 * EN_COMPONENT6_TAG: range 0x00060000 ~ 0x0006FFFF
 * m_srccap.h
 * EN_COMPONENT7_TAG: range 0x00070000 ~ 0x0007FFFF
 * m_subhal.h
 * EN_COMPONENT8_TAG: range 0x00080000 ~ 0x0008FFFF
 * m_vdec.h
 * EN_COMPONENT9_TAG: range 0x00090000 ~ 0x0009FFFF
 * FrameFormat.h && mi_disp_meta_frameformat.h
 * EN_COMPONENT10_TAG: range 0x000A0000 ~ 0x000AFFFF
 * VtsHalMediatekPQTest.cpp
 * EN_COMPONENT11_TAG: range 0x000B0000 ~ 0x000BFFFF
 */

/* metadata header structure */
struct meta_header {
	unsigned int tag;
	unsigned short ver;
	unsigned short size;
};

struct meta_buffer {
	unsigned char *paddr;
	unsigned int size;
};

// Init metadata buffer,set meta magic header.
//Only the metadata buffer allocator should init buffer.
// [IN] pmetabuf: metadata buffer.
// return false case:
// 1. Invalid Input Arguments.
unsigned char init_metadata(struct meta_buffer *pmetabuf);

// Attach metadata to metadata buffer.
// [IN] pmetabuf: metadata buffer.
// [IN] header: metadata block header.
// [IN] pmetadata: expect metadata.
// return false case:
// 1. Header tag isn’t a string.
// 2. Tag is already attached, but size and ver isn’t equal to old.
// 3. Tag datasize exceeds free space.
// 4. Invalid Input Arguments.
unsigned char attach_metadata(struct meta_buffer *pmetabuf,
				struct meta_header header, void *pmetadata);

// Update specified metadata that already attach to metadata buffer.
// [IN] pmetabuf: metadata buffer.
// [IN] header: metadata block header.
// [IN] pmetadata: update metadata.
// return false case:
// 1. Tag not found.
// 2. Input size or version isn’t equal to olds.
// 3. Invalid Input Arguments.
unsigned char update_metadata(struct meta_buffer *pmetabuf,
				struct meta_header header, void *pmetadata);

// Query metadata header, that used to check metadata version or size that
// attached to metadta buffer.
// [IN] pmetabuf: metadata buffer.
// [IN/OUT] *pheader: specify metadata tag and output the version and size.
// return false case:
// 1. Tag not found.
// 2. Invalid Input Arguments.
unsigned char query_metadata_header(struct meta_buffer *pmetabuf,
				struct meta_header *pheader);

// Query metadata.
// [IN] pmetabuf: metadata buffer.
// [IN/OUT] *pheader: specify metadata tag / version / size and output the
//  minimize metadata version and size.
// [OUT] pmetadata: metadata that exists in metadata buffer.
// return false case:
// 1. Tag not found.
// 2. Invalid Input Arguments.
unsigned char query_metadata_content(struct meta_buffer *pmetabuf,
				struct meta_header *pheader, void *pmetadata);

unsigned char *query_metadata_content_ptr(struct meta_buffer *pmetabuf,
		struct meta_header *pheader);

// Delete metadata
// [IN] pmetabuf: metadata buffer.
// [IN] *pheader: specify metadata tag.
// return false case:
// 1. Invalid Input Arguments.
unsigned char delete_metadata(struct meta_buffer *pmetabuf,
				struct meta_header *pheader);
#ifdef __cplusplus
}
#endif
#endif
