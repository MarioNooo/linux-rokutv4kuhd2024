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


#ifndef _MHAL_ALSA_DRIVER_HEADER
#define _MHAL_ALSA_DRIVER_HEADER

/*
 * ============================================================================
 * Include Headers
 * ============================================================================
 */
#include "mdrv_public.h"
#include "mhal_version.h"


/*
 * ============================================================================
 * Constant Definitions
 * ============================================================================
 */
#define Capture_decimation_ctrl    0x2D4E
#define PCM_CAPTURE1_FS_MASK    0x000F
#define PCM_CAPTURE2_FS_MASK    0x00F0
#define PCM_CAPTURE3_FS_MASK    0x0F00
#define Decimation_FS_MASK    0xF000
#define PCM_CAPTURE_FS_48KHZ    0
#define PCM_CAPTURE_FS_24KHZ    1
#define PCM_CAPTURE_FS_16KHZ    2
#define PCM_CAPTURE_FS_12KHZ    3
#define PCM_CAPTURE_FS_08KHZ    4
#define DECIM_FILTER_FS_48KHZ    0
#define DECIM_FILTER_FS_24KHZ    1
#define DECIM_FILTER_FS_16KHZ    2

#define DECIM_FILTER_FS_12KHZ    3
#define DECIM_FILTER_FS_08KHZ    4

#define REG_VIVALDI0_BASE 0x950000
#define REG_VIVALDI1_BASE 0x950100
#define REG_VIVALDI2_BASE 0x950200
#define REG_VIVALDI3_BASE 0x950300
#define REG_VIVALDI4_BASE 0x950400
#define REG_VIVALDI5_BASE 0x950500
#define REG_VIVALDI6_BASE 0x950600
#define REG_VIVALDI7_BASE 0x950700
#define REG_VIVALDI8_BASE 0x950800
#define REG_VIVALDI9_BASE 0x950900
#define REG_VIVALDIA_BASE 0x950A00
#define REG_VIVALDID_BASE 0x950D00
#define REG_VIVALDI00_BASE 0x951000
#define REG_VIVALDI01_1_BASE 0x951100
#define REG_VIVALDI05_BASE 0x951500
#define REG_VIVALDI06_BASE 0x951600

#define REG_AUDIO_CH_S1_CFG                      REG_VIVALDI05_BASE + 0x11
#define REG_AUDIO_CH_S2_CFG                      REG_VIVALDI05_BASE + 0x12
#define REG_AUDIO_CH_S3_CFG                      REG_VIVALDI05_BASE + 0x13
#define REG_AUDIO_CH_S4_CFG                      REG_VIVALDI05_BASE + 0x14

#define DMA_READER_V2_1_CTRL                     REG_VIVALDIA_BASE + 0x40 // [0] MIU enable, [1] DMA enable, [4] CPU write trigger, [5] Stop update level cnt,[15] Reset
#define DMA_READER_V2_1_BASE_L                   REG_VIVALDIA_BASE + 0x42 // buf base [15: 0]       (unit: line)
#define DMA_READER_V2_1_BASE_H                   REG_VIVALDIA_BASE + 0x44 // buf base [31:16]       (unit: line)
#define DMA_READER_V2_1_SIZE                     REG_VIVALDIA_BASE + 0x46 // buf size               (unit: line)
#define DMA_READER_V2_1_CPU_TRIGGER_SIZE         REG_VIVALDIA_BASE + 0x48 // CPU write size/once    (unit: line)
#define DMA_READER_V2_1_UNDERRUN_THRESHOLD       REG_VIVALDIA_BASE + 0x4A
#define DMA_READER_V2_1_ENABLE_CTRL              REG_VIVALDIA_BASE + 0x4C // [0][2][6][8]=0x145 enable clock/synth, [5:4] synth N.F. divisor(1/2/4/8)
#define DMA_READER_V2_1_SYNTH                    REG_VIVALDIA_BASE + 0x4E // synth N.F.
#define DMA_READER_V2_1_LEVELCNT                 REG_VIVALDIA_BASE + 0x54 // DRAM level cnt         (unit: line)
#define DMA_READER_V2_1_FLAG                     REG_VIVALDIA_BASE + 0x56
#define DMA_READER_V2_1_OUT_LEFT_CH              REG_VIVALDIA_BASE + 0x5C // output left channel
#define DMA_READER_V2_1_OUT_RIGHT_CH             REG_VIVALDIA_BASE + 0x5E // output right channel

#define DMA_READER_V2_2_CTRL                     REG_VIVALDIA_BASE + 0x60 // [0] MIU enable, [1] DMA enable, [4] CPU write trigger, [5] Stop update level cnt,[15] Reset
#define DMA_READER_V2_2_BASE_L                   REG_VIVALDIA_BASE + 0x62 // buf base [15: 0]       (unit: line)
#define DMA_READER_V2_2_BASE_H                   REG_VIVALDIA_BASE + 0x64 // buf base [31:16]       (unit: line)
#define DMA_READER_V2_2_SIZE                     REG_VIVALDIA_BASE + 0x66 // buf size               (unit: line)
#define DMA_READER_V2_2_CPU_TRIGGER_SIZE         REG_VIVALDIA_BASE + 0x68 // CPU write size/once    (unit: line)
#define DMA_READER_V2_2_UNDERRUN_THRESHOLD       REG_VIVALDIA_BASE + 0x6A
#define DMA_READER_V2_2_ENABLE_CTRL              REG_VIVALDIA_BASE + 0x6C // [0][2][6][8]=0x145 enable clock/synth, [5:4] synth N.F. divisor(1/2/4/8)
#define DMA_READER_V2_2_SYNTH                    REG_VIVALDIA_BASE + 0x6E // synth N.F.
#define DMA_READER_V2_2_LEVELCNT                 REG_VIVALDIA_BASE + 0x74 // DRAM level cnt         (unit: line)
#define DMA_READER_V2_2_FLAG                     REG_VIVALDIA_BASE + 0x76
#define DMA_READER_V2_2_OUT_LEFT_CH              REG_VIVALDIA_BASE + 0x7C // output left channel
#define DMA_READER_V2_2_OUT_RIGHT_CH             REG_VIVALDIA_BASE + 0x7E // output right channel

#define REG_SUB_CH_SOURCE_INFO                   REG_VIVALDI2_BASE + 0x67 //Stereo-channel input source info to DSP

#define REG_SE_DSP_ICACHE_BASE_L            (REG_VIVALDI0_BASE + 0x90)
#define REG_SE_BDMA_CFG                     (REG_VIVALDI0_BASE + 0xC0)

#define M2M_ALSA_DDR_SIZE_L                 (REG_VIVALDI3_BASE + 0x82)
#define M2M_ALSA_DDR_SIZE_H                 (REG_VIVALDI3_BASE + 0x84)
#define M2S_MBOX_POWER_DOWN                 (REG_VIVALDI3_BASE + 0x30)
#define M2S_MBOX_SW_DMA_READER_DDR_WtPtr    (REG_VIVALDI3_BASE + 0x34)
#define M2S_MBOX_SW_DMA_READER_DDR_Ctrl     (REG_VIVALDI3_BASE + 0x36)
#define M2S_MBOX_SW_DMA_READER2_DDR_WtPtr   (REG_VIVALDI3_BASE + 0x3C)
#define M2S_MBOX_SW_DMA_READER2_DDR_Ctrl    (REG_VIVALDI3_BASE + 0x3E)
#define M2S_MBOX_SW_DMA_READER3_DDR_WtPtr   (REG_VIVALDI4_BASE + 0x24)
#define M2S_MBOX_SW_DMA_READER3_DDR_Ctrl    (REG_VIVALDI4_BASE + 0x26)
    #define SW_DMA_CTRL_RESET_BIT                   (1 << 0)
    #define SW_DMA_CTRL_START_BIT                   (1 << 1)
    #define SW_DMA_CTRL_FREERUN_BIT                 (1 << 2)           // CIRCL mode, SW_DMA_CTRL_CIRCL_BIT
    #define SW_DMA_CTRL_REPORT_CNT_BIT              (1 << 3)           // 1->disable update when read cnt or level, 0->normal
    #define SW_DMA_CTRL_ENABLE_BIT                  (1 << 4)
    #define SW_DMA_CTRL_CH_NUM_BIT                  (1 << 6)   		// [ 7: 6] ch num, 0= 2ch , 1= 6ch

#define S2M_MBOX_SW_DMA_READER_DDR_Level    (REG_VIVALDI3_BASE + 0xE2)
#define S2M_MBOX_SW_DMA_READER2_DDR_Level   (REG_VIVALDI3_BASE + 0xE8)
#define S2M_MBOX_SW_DMA_READER3_DDR_Level   (REG_VIVALDI4_BASE + 0x12)

#define S2M_MBOX_SW_DMA_READER_DDR_playSmpCnt_H     (REG_VIVALDI4_BASE + 0x02)
#define S2M_MBOX_SW_DMA_READER_DDR_playSmpCnt_L     (REG_VIVALDI4_BASE + 0x04)
#define S2M_MBOX_SW_DMA_READER2_DDR_playSmpCnt_H    (REG_VIVALDI4_BASE + 0x06)
#define S2M_MBOX_SW_DMA_READER2_DDR_playSmpCnt_L    (REG_VIVALDI4_BASE + 0x08)
#define S2M_MBOX_SW_DMA_READER3_DDR_playSmpCnt_H    (REG_VIVALDI4_BASE + 0x0A)
#define S2M_MBOX_SW_DMA_READER3_DDR_playSmpCnt_L    (REG_VIVALDI4_BASE + 0x0C)

#define S2M_MBOX_SW_DMA_READER_DDR_synth_nf_h      (REG_VIVALDI2_BASE + 0x24)
#define S2M_MBOX_SW_DMA_READER_DDR_synthrate_h     (REG_VIVALDI2_BASE + 0x26)
#define S2M_MBOX_SW_DMA_READER_DDR_synthrate_l     (REG_VIVALDI2_BASE + 0x28)

#define M2A_MBOX_PCM_CAPTURE_DDR_RdPtr          (REG_VIVALDI3_BASE + 0xD4)
#define M2A_MBOX_PCM_CAPTURE_DDR_Size           (REG_VIVALDI3_BASE + 0xD6)

#define M2A_MBOX_PCM_CAPTURE2_DDR_RdPtr         (REG_VIVALDI3_BASE + 0x38)
#define M2A_MBOX_PCM_CAPTURE2_DDR_Size          (REG_VIVALDI3_BASE + 0x3A)

#define M2A_MBOX_PCM_CAPTURE3_DDR_RdPtr         (REG_VIVALDI3_BASE + 0x94)
#define M2A_MBOX_PCM_CAPTURE3_DDR_Size          (REG_VIVALDI3_BASE + 0x96)

#define M2A_MBOX_PCM_CAPTURE4_DDR_RdPtr         (REG_VIVALDI3_BASE + 0x98)
#define M2A_MBOX_PCM_CAPTURE4_DDR_Size          (REG_VIVALDI3_BASE + 0x9A)

#define A2M_MBOX_PCM_CAPTURE_DDR_WrPtr          (REG_VIVALDI3_BASE + 0xF0)
#define A2M_MBOX_PCM_CAPTURE2_DDR_WrPtr         (REG_VIVALDI3_BASE + 0xF4)
#define A2M_MBOX_PCM_CAPTURE3_DDR_WrPtr         (REG_VIVALDI3_BASE + 0x7E)
#define A2M_MBOX_PCM_CAPTURE4_DDR_WrPtr         (REG_VIVALDI3_BASE + 0x78)

/* D-MIC channel mode */
typedef enum
{
	DMIC_CHANNEL_2 = 0,
	DMIC_CHANNEL_4,
}AudChannel_e;

struct mhal_audio_clock_info {
	unsigned int offset;
	unsigned int start;
	unsigned int end;
	unsigned int val;
};

/* Define a Ring Buffer data structure for MStar Audio DSP */
struct MStar_Ring_Buffer_Struct {
	unsigned char *addr;
	unsigned int size;
	unsigned char *w_ptr;
	unsigned char *r_ptr;
};

/* Define a STR (Suspend To Ram) data structure for MStar Audio DSP */
 struct MStar_STR_MODE_Struct {
	ptrdiff_t physical_addr;
	ptrdiff_t bus_addr;
	ptrdiff_t virtual_addr;
};

/* Define a DMA Reader data structure for MStar Audio DSP */
struct MStar_DMA_Reader_Struct {
	struct MStar_Ring_Buffer_Struct buffer;
	struct MStar_STR_MODE_Struct str_mode_info;
	struct mhal_audio_clock_info clock_info[3];
	unsigned int initialized_status;
	unsigned int channel_mode;
	unsigned int sample_rate;
	unsigned int period_size;
	unsigned int high_threshold;
	unsigned int remain_size;
	unsigned int written_size;
	unsigned int status;
	unsigned char mmap_flag;
	int dev_id;
	unsigned int curr_written_samples;
	unsigned int pre_written_samples;
};

/* Define a DMA Reader data structure for MStar Audio DSP */
struct MStar_PCM_Capture_Struct {
	struct MStar_Ring_Buffer_Struct buffer;
	struct MStar_STR_MODE_Struct str_mode_info;
	unsigned int initialized_status;
	unsigned int channel_mode;
	unsigned int sample_rate;
	unsigned int accuSize; // accumulated PCM size (accumulated size += new available - read size.
	unsigned char mmap_flag;
	unsigned char flush_flag; // Inform PCM DMA to flush DMA data.
	unsigned char bit_width;
	unsigned long preWptr;
	int devId;
	unsigned int status;
};

/* Define a DMA Reader data structure for MStar Audio DSP */
struct MStar_PCM_DMIC_Capture_Struct {
	struct MStar_Ring_Buffer_Struct buffer;
	struct MStar_Ring_Buffer_Struct aec_buffer;
	struct MStar_STR_MODE_Struct str_mode_info;
	struct MStar_STR_MODE_Struct aec_str_mode_info;
	unsigned int initialized_status;
	unsigned int channel_mode;
	unsigned int sample_rate;
	unsigned int status;
	unsigned int bit_width;
	int vdma_channel;
	int aec_channel;
	unsigned char dmic_enable;
	unsigned char hpf_switch;
	unsigned char hpf_config;
	unsigned char sine_gen;
	unsigned int gain;
};

#endif /* _MHAL_ALSA_DRIVER_HEADER */

