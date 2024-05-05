/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef MTK_DMX_REGS_H
#define MTK_DMX_REGS_H

/*-----------------------------*/
#include "mtk_dmx_common.h"

#include "tsp0.h"
#include "tsp1.h"
#include "tsp2.h"
#include "tsp3.h"
#include "tsp4.h"
#include "tsp5.h"
#include "tsp6.h"
#include "tsp7.h"
#include "tsp8.h"

#include "tso0.h"
#include "tso1.h"
#include "tso2.h"

#include "tsp_tlv.h"
#include "tsp_tlv2.h"

#include "tsp_alp.h"

#include "ts_sample.h"

#include "cilink_soc_if.h"

#include "mtk_dmx_rm.h"
/*-----------------------------*/

extern mtk_dmx_riu_bank(*dmx_bank_offsets);

#define REG_CLKGEN0_BASE		(dmx_bank_offsets[RIU_BANK_TYPE_CLKGEN].ptr[0])
#define REG_TS_SAMPLE_BASE		(dmx_bank_offsets[RIU_BANK_TYPE_TS_SAMPLE].ptr[0])

#define REG_TSP0_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS].ptr[0])
#define REG_TSP1_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS].ptr[1])
#define REG_TSP2_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS].ptr[2])
#define REG_TSP3_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS].ptr[3])
#define REG_TSP4_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS].ptr[4])
#define REG_TSP5_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS].ptr[5])
#define REG_TSP6_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS].ptr[6])
#define REG_TSP7_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS].ptr[7])

#define REG_TSO0_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS_OUT].ptr[0])
#define REG_TSO1_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS_OUT].ptr[1])
#define REG_TSO2_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TS_OUT].ptr[2])

#define REG_TLV0_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TLV].ptr[0])
#define REG_TLV1_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_TLV].ptr[1])

#define REG_ALP0_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_ALP].ptr[0])
#define REG_ALP1_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_ALP].ptr[1])

#define REG_PAS0_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_MVD_PARSER].ptr[0])
#define REG_PAS1_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_MVD_PARSER].ptr[1])

#define REG_CILINK_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_CILINK].ptr[0])

#define REG_MAD0_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_MAD_PARSER].ptr[0])
#define REG_MAD1_BASE			(dmx_bank_offsets[RIU_BANK_TYPE_MAD_PARSER].ptr[1])

#define REG_BASE_TS_SAMPLE(n)	(dmx_bank_offsets[RIU_BANK_TYPE_TS_SAMPLE].ptr[n])
#define REG_BASE_TSP(n)			(dmx_bank_offsets[RIU_BANK_TYPE_TS].ptr[n])
#define REG_BASE_TSO(n)			(dmx_bank_offsets[RIU_BANK_TYPE_TS_OUT].ptr[n])
#define REG_BASE_TLV(n)			(dmx_bank_offsets[RIU_BANK_TYPE_TLV].ptr[n])
#define REG_BASE_PAS(n)			(dmx_bank_offsets[RIU_BANK_TYPE_MVD_PARSER].ptr[n])
#define REG_BASE_ALP(n)			(dmx_bank_offsets[RIU_BANK_TYPE_ALP].ptr[n])
#define REG_BASE_CILINK(n)		(dmx_bank_offsets[RIU_BANK_TYPE_CILINK].ptr[n])
#define REG_BASE_MAD(n)		    (dmx_bank_offsets[RIU_BANK_TYPE_MAD_PARSER].ptr[n])

#define REG_BANK_MAX_TS_SAMPLE	(dmx_bank_offsets[RIU_BANK_TYPE_TS_SAMPLE].bank_num)
#define REG_BANK_MAX_TSP		(dmx_bank_offsets[RIU_BANK_TYPE_TS].bank_num)
#define REG_BANK_MAX_CLKGEN		(dmx_bank_offsets[RIU_BANK_TYPE_CLKGEN].bank_num)
#define REG_BANK_MAX_TSO		(dmx_bank_offsets[RIU_BANK_TYPE_TS_OUT].bank_num)
#define REG_BANK_MAX_TLV		(dmx_bank_offsets[RIU_BANK_TYPE_TLV].bank_num)
#define REG_BANK_MAX_PAS		(dmx_bank_offsets[RIU_BANK_TYPE_MVD_PARSER].bank_num)
#define REG_BANK_MAX_ALP		(dmx_bank_offsets[RIU_BANK_TYPE_ALP].bank_num)
#define REG_BANK_MAX_CILINK		(dmx_bank_offsets[RIU_BANK_TYPE_CILINK].bank_num)
#define REG_BANK_MAX_MAD		(dmx_bank_offsets[RIU_BANK_TYPE_MAD_PARSER].bank_num)

#define REG_BANK(base, offset)			((base) + (offset))

#define RIU_READ_BYTE(base, addr)		reg16_mask_read(REG_BANK(base, addr), 0xFF, 0)
#define RIU_WRITE_BYTE(base, addr, val)	reg16_mask_write(REG_BANK(base, addr), 0xFF, val)
#define RIU_READ_WORD(base, addr)		reg16_read(REG_BANK(base, addr))
#define RIU_WRITE_WORD(base, addr, val)	reg16_write(REG_BANK(base, addr), val)

// CLKGEN #0 (bank: 0x100B)
#define CKG_STC_SYNTH                   (REG_CLKGEN0_BASE + 0x10)       // offset: 0x04
    #define STC0_CW_SEL                 (1)                             // bit[1]
    #define STC1_CW_SEL                 (9)                             // bit[9]

#define CKG_STC0_SYNTH_CW               (REG_CLKGEN0_BASE + 0x14)       // offset: 0x05 ~ 0x06

#define CKG_STC1_SYNTH_CW               (REG_CLKGEN0_BASE + 0x1C)       // offset: 0x07 ~ 0x08

// TSP #0 (bank: 0x1015)
#define TSP0_REG_01_02                  (REG_TSP0_BASE + 0x04)          // offset: 0x01 ~ 0x02

#define TSP0_REG_05_06                  (REG_TSP0_BASE + 0x14)          // offset: 0x05 ~ 0x06

#define TSP0_REG_07_08                  (REG_TSP0_BASE + 0x1C)          // offset: 0x07 ~ 0x08

#define TSP0_REG_09_0A                  (REG_TSP0_BASE + 0x24)          // offset: 0x09 ~ 0x0A

#define TSP0_REG_0B                     (REG_TSP0_BASE + 0x2C)          // offset: 0x0B
    #define MOBF_FILEIN_IDX_MASK        (0x001F)
    #define MOBF_FILEIN_IDX_SHIFT       (0)

#define TSP0_REG_0C                     (REG_TSP0_BASE + 0x30)          // offset: 0x0C
    #define FILEIN_ADDR_EXT_MASK        (0x0003)
    #define FILEIN_ADDR_EXT_SHIFT       (0)

#define TSP0_REG_0E                     (REG_TSP0_BASE + 0x38)          // offset: 0x0E
    #define PVR2_TIMESTAMP_WLD          (0)                             // bit[0]
    #define PVR2_TIMESTAMP_RLD          (1)                             // bit[1]
    #define PVR2_EN                     (3)                             // bit[3]
    #define PVR2_RESET_WADDR            (4)                             // bit[4]
    #define PVR2_PAUSE                  (6)                             // bit[6]
    #define PVR2_PINGPONG_EN            (7)                             // bit[7]
    #define PVR2_ALIGN_EN               (8)                             // bit[8]
    #define PVR2_TIMESTAMP_EN           (10)                            // bit[10]

#define TSP0_REG_10_11                  (REG_TSP0_BASE + 0x40)          // offset: 0x10 ~ 0x11

#define TSP0_REG_12_13                  (REG_TSP0_BASE + 0x48)          // offset: 0x12 ~ 0x13

#define TSP0_REG_14_15                  (REG_TSP0_BASE + 0x50)          // offset: 0x14 ~ 0x15

#define TSP0_REG_16_17                  (REG_TSP0_BASE + 0x58)          // offset: 0x16 ~ 0x17

#define TSP0_REG_18_19                  (REG_TSP0_BASE + 0x60)          // offset: 0x18 ~ 0x19

#define TSP0_REG_1C_1D                  (REG_TSP0_BASE + 0x70)          // offset: 0x1C ~ 0x1D

#define TSP0_REG_2C                     (REG_TSP0_BASE + 0xB0)          // offset: 0x2C
    #define TSIF0_PARALLEL_SEL          (1)                             // bit[1]
    #define TSIF0_EXT_SYNC_SEL          (2)                             // bit[2]

#define TSP0_REG_38_39                  (REG_TSP0_BASE + 0xE0)          // offset: 0x38 ~ 0x39

#define TSP0_REG_3A_3B                  (REG_TSP0_BASE + 0xE8)          // offset: 0x3A ~ 0x3B

#define TSP0_REG_3C_3D                  (REG_TSP0_BASE + 0xF0)          // offset: 0x3C ~ 0x3D

#define TSP0_REG_3E                     (REG_TSP0_BASE + 0xF8)          // offset: 0x3E
    #define FILEIN_CTRL_START           (0)                             // bit[0]
    #define FILEIN_CTRL_DONE            (1)                             // bit[1]

#define TSP0_REG_3F                     (REG_TSP0_BASE + 0xFC)          // offset: 0x3F
    #define CMDQ_WRITE_CNT_MASK         (0x001F)                        // bit[4:0]
    #define CMDQ_WRITE_CNT_SHIFT        (0)

#define TSP0_REG_40_41                  (REG_TSP0_BASE + 0x100)          // offset: 0x40 ~ 0x41
    #define MUC_CMD_MASK                (0xFF000000)                    // bit[31:28]
    #define MCU_CMD_ALIVE               (0x01000000)
    #define MCU_CMD_NMATCH              (0x02000000)
    #define MCU_CMD_NMATCH_FLT_MASK     (0x000000FF)
    #define MCU_CMD_NMATCH_FLT_SHFT     (0)

#define TSP0_REG_43                     (REG_TSP0_BASE + 0x10C)          // offset: 0x43
    #define TSIF1_PARALLEL_SEL          (8)                             // bit[8]
    #define TSIF1_EXT_SYNC_SEL          (9)                             // bit[9]

#define TSP0_REG_44                     (REG_TSP0_BASE + 0x110)          // offset: 0x44
    #define PVR1_EN                     (1)                             // bit[1]
    #define TSIF_1_EN                   (3)                             // bit[3]
    #define PVR1_RESET_WADDR            (4)                             // bit[4]
    #define PVR1_PAUSE                  (6)                             // bit[6]
    #define TSIF_0_EN                   (8)                             // bit[8]
    #define TSIF_SYNC_RISING_DETECT     (9)                             // bit[9]
    #define TSIF_VALID_FALLING_DETECT   (10)                            // bit[10]

#define TSP0_REG_45                     (REG_TSP0_BASE + 0x114)          // offset: 0x45
    #define BYTE_ADDR_DMA_MASK          (0x000F)                        // bit[3:0]
    #define DATA_CHK_2T                 (15)                            // bit[15]

#define TSP0_REG_48                     (REG_TSP0_BASE + 0x120)          // offset: 0x48
    #define IND_WRITE_MASK              (0x0023)                        // bit[1:0],[5]
    #define IND_READ_MASK               (0x0021)                        // bit[0],[5]

#define TSP0_REG_49_4A                  (REG_TSP0_BASE + 0x124)          // offset: 0x49 ~ 0x4A

#define TSP0_REG_4B_4C                  (REG_TSP0_BASE + 0x12C)          // offset: 0x4B ~ 0x4C

#define TSP0_REG_4D_4E                  (REG_TSP0_BASE + 0x134)          // offset: 0x4D ~ 0x4E

#define TSP0_REG_50_51                  (REG_TSP0_BASE + 0x140)          // offset: 0x50 ~ 0x51

#define TSP0_REG_52_53                  (REG_TSP0_BASE + 0x148)          // offset: 0x52 ~ 0x53

#define TSP0_REG_54_55                  (REG_TSP0_BASE + 0x150)          // offset: 0x55 ~ 0x54

#define TSP0_REG_5A                     (REG_TSP0_BASE + 0x168)          // offset: 0x5A
    #define PID_BYPASS_REC              (3)                             // bit[3]
    #define PID_BYPASS_REC2             (4)                             // bit[4]
    #define BYTE_TIMER_EN               (15)                            // bit[15]

#define TSP0_REG_5B                     (REG_TSP0_BASE + 0x16C)          // offset: 0x5B
    #define PVR1_PINGPONG_EN            (0)                             // bit[0]
    #define STC1_LD_BIT                 (7)                             // bit[7]
    #define STC0_LD_BIT                 (8)                             // bit[8]


#define TSP0_REG_60_61                  (REG_TSP0_BASE + 0x180)          // offset: 0x60 ~ 0x61
    #define NOEA_ADDR_BASE_MASK         (0x01FFFFFF)

#define TSP0_REG_62_63                  (REG_TSP0_BASE + 0x188)          // offset: 0x62 ~ 0x63

#define TSP0_REG_64_65                  (REG_TSP0_BASE + 0x190)          // offset: 0x64 ~ 0x65

#define TSP0_REG_66_67                  (REG_TSP0_BASE + 0x198)          // offset: 0x66 ~ 0x67

#define TSP0_REG_68_69                  (REG_TSP0_BASE + 0x1A0)          // offset: 0x68 ~ 0x69

#define TSP0_REG_70                     (REG_TSP0_BASE + 0x1C0)          // offset: 0x70 ~ 0x71
    #define BYTE_TIMER_THRESHOLD_MASK   (0x00FFFFFF)                    // bit[23:0]
    #define BYTE_TIMER_THRESHOLD_SHIFT  (0)

#define TSP0_REG_78                     (REG_TSP0_BASE + 0x1E0)          // offset: 0x78

#define TSP0_REG_79                     (REG_TSP0_BASE + 0x1E4)          // offset: 0x79

#define TSP0_REG_7A                     (REG_TSP0_BASE + 0x1E8)          // offset: 0x7A
    #define NOEA_EN                     (0)                             // bit[0]
    #define SW_RSTZ                     (1)                             // bit[1]
    #define DMA_START                   (2)                             // bit[2]
    #define DMA_DONE                    (3)                             // bit[3]
    #define FILE_INPUT_EN               (4)                             // bit[4]

#define TSP0_REG_7E                     (REG_TSP0_BASE + 0x1F8)          // offset: 0x7E
    #define SW_INT                      (5)

#define TSP0_REG_7F                     (REG_TSP0_BASE + 0x1FC)          // offset: 0x7F
    #define TSP_FILEIN_PAUSE            (2)                             // bit[2]
    #define DISABLE_STANDBY             (7)                             // bit[7]
    #define FILEIN_CMDQ_RST             (11)                            // bit[11]
    #define TSP_FILE_SEGMENT            (13)                            // bit[13]

// TSP #1 (bank: 0x1016)
#define TSP1_REG_00_01                  (REG_TSP1_BASE + 0x00)          // offset: 0x00 ~ 0x01

#define TSP1_REG_02_03                  (REG_TSP1_BASE + 0x08)          // offset: 0x02 ~ 0x03

#define TSP1_REG_04_05                  (REG_TSP1_BASE + 0x10)          // offset: 0x04 ~ 0x05

#define TSP1_REG_06                     (REG_TSP1_BASE + 0x18)          // offset: 0x06
    #define PVR1_TIMESTAMP_WLD          (0)                             // bit[0]
    #define PVR1_TIMESTAMP_RLD          (1)                             // bit[1]
    #define FILEIN_LPCR2_WLD            (2)                             // bit[2]
    #define FILEIN_LPCR2_RLD            (3)                             // bit[3]
    #define PVR1_TIMESTAMP_EN           (4)                             // bit[4]
    #define FILEIN_TIMESTAMP_EN         (5)                             // bit[5]
    #define RM_PINPON_LIMIT             (14)                            // bit[14]
    #define FILEIN_TIMESTAMP_RST        (15)                            // bit[15]

#define TSP1_REG_07                     (REG_TSP1_BASE + 0x1C)          // offset: 0x07
    #define RM_DMA_GLITCH               (7)                             // bit[7]
    #define RESET_VFIFO                 (8)                             // bit[8]
    #define RESET_AFIFO                 (9)                             // bit[9]
    #define RESET_AFIFOB                (12)                            // bit[12]
    #define RESET_VFIFO3D               (13)                            // bit[13]

#define TSP1_REG_08                     (REG_TSP1_BASE + 0x20)          // offset: 0x08
    #define FILEIN_PKT_CHK_SIZE_MASK    (0x00FF)                        // bit[7:0]
    #define FILEIN_PKT_CHK_SIZE_SHIFT   (0)
    #define FILEIN_PKT192_BLOCK_DISABLE (8)                             // bit[8]
    #define HW_STANDBY_MODE             (10)                            // bit[10]

#define TSP1_REG_09                     (REG_TSP1_BASE + 0x24)          // offset: 0x09
    #define SYS_TIME_MODE               (3)                             // bit[3]
    #define SEC_DMA_BURST_EN            (7)                             // bit[7]
    #define DUP_PKT_SKIP_AV_MASK        (0x7E00)                        // bit[14:9]

#define TSP1_REG_0A                     (REG_TSP1_BASE + 0x28)          // offset: 0x0A

#define TSP1_REG_10_11                  (REG_TSP1_BASE + 0x40)          // offset: 0x10 ~ 0x11

#define TSP1_REG_12_13                  (REG_TSP1_BASE + 0x48)          // offset: 0x12 ~ 0x13

#define TSP1_REG_1C                     (REG_TSP1_BASE + 0x70)          // offset: 0x1C
    #define BLK_AF_SCRMB_BIT            (10)                            // bit[10]

#define TSP1_REG_1E                     (REG_TSP1_BASE + 0x78)          // offset: 0x1E
    #define AUDIO2_FIFO_SRC_MASK        (0x0007)                        // bit[2:0]
    #define AUDIO2_FIFO_SRC_SHIFT       (0)
    #define AUDIO3_FIFO_SRC_MASK        (0x0038)                        // bit[5:3]
    #define AUDIO3_FIFO_SRC_SHIFT       (3)

#define TSP1_REG_20_21                  (REG_TSP1_BASE + 0x80)          // offset: 0x20 ~ 0x21

#define TSP1_REG_22                     (REG_TSP1_BASE + 0x88)          // offset: 0x22

#define TSP1_REG_23                     (REG_TSP1_BASE + 0x8C)          // offset: 0x23
    #define VQ_FORCEFIRE_CNT_1K_MASK    (0x0F00)                        // bit[11:8]
    #define VQ_FORCEFIRE_CNT_1K_SHIFT   (8)
    #define VQ_RESET                    (12)                            // bit[12]

#define TSP1_REG_3C_3D                  (REG_TSP1_BASE + 0xF0)          // offset: 0x3C ~ 0x3D

#define TSP1_REG_3E                     (REG_TSP1_BASE + 0xF8)          // offset: 0x3E
    #define PCR_BASE_BIT32_MASK         (0x0001)                        // bit[0]

#define TSP1_REG_44_45                  (REG_TSP1_BASE + 0x110)          // offset: 0x44 ~ 0x45

#define TSP1_REG_46                     (REG_TSP1_BASE + 0x118)          // offset: 0x46

#define TSP1_REG_50                     (REG_TSP1_BASE + 0x140)          // offset: 0x50
    #define AUDIO_FIFO_SRC_MASK         (0x0007)                        // bit[2:0]
    #define AUDIO_FIFO_SRC_SHIFT        (0)
    #define AUDIO1_FIFO_SRC_MASK        (0x0038)                        // bit[5:3]
    #define AUDIO1_FIFO_SRC_SHIFT       (3)
    #define VIDEO_FIFO_SRC_MASK         (0x01C0)                        // bit[8:6]
    #define VIDEO_FIFO_SRC_SHIFT        (6)
    #define VIDEO1_FIFO_SRC_MASK        (0x0E00)                        // bit[11:9]
    #define VIDEO1_FIFO_SRC_SHIFT       (9)
    #define PVR1_FIFO_SRC_MASK          (0x7000)                        // bit[14:12]
    #define PVR1_FIFO_SRC_SHIFT         (12)

#define TSP1_REG_51                     (REG_TSP1_BASE + 0x144)          // offset: 0x51
    #define PCR0_RESET                  (9)                             // bit[9]
    #define PCR0_READ                   (11)                            // bit[11]
    #define PCR1_RESET                  (13)                            // bit[13]
    #define PCR1_READ                   (15)                            // bit[15]

#define TSP1_REG_55                     (REG_TSP1_BASE + 0x154)          // offset: 0x55
    #define PVR2_FIFO_SRC_MASK          (0x0007)                        // bit[2:0]
    #define PVR2_FIFO_SRC_SHIFT         (0)

#define TSP1_REG_56_57                  (REG_TSP1_BASE + 0x158)          // offset: 0x56 ~ 0x57

#define TSP1_REG_5C                     (REG_TSP1_BASE + 0x170)          // offset: 0x5C

#define TSP1_REG_5D                     (REG_TSP1_BASE + 0x174)          // offset: 0x5D

#define TSP1_REG_5E_5F                  (REG_TSP1_BASE + 0x178)          // offset: 0x5E ~ 0x5F

#define TSP1_REG_64                     (REG_TSP1_BASE + 0x190)          // offset: 0x64

#define TSP1_REG_65                     (REG_TSP1_BASE + 0x194)          // offset: 0x65

#define TSP1_REG_6C                     (REG_TSP1_BASE + 0x1B0)          // offset: 0x6C
    #define PCR_FILTER_1_UPDATE_FINISH  (2)                             // bit[2]
    #define PCR_FILTER_0_UPDATE_FINISH  (3)                             // bit[3]
    #define HW_INT_STATUS_MASK          (0xFF00)                        // bit[15:8]
    #define HW_INT_STATUS_SHIFT         (8)

#define TSP1_REG_70_71                  (REG_TSP1_BASE + 0x1C0)          // offset: 0x70 ~ 0x71

#define TSP1_REG_72                     (REG_TSP1_BASE + 0x1C8)          // offset: 0x72
    #define PVR1_ALIGN_EN               (9)                             // bit[9]
    #define RM_PKT_DEMUX_PIPE           (11)                            // bit[11]
    #define VQ_EN                       (14)                            // bit[14]
    #define VQ2PINGPONG_EN              (15)                            // bit[15]

#define TSP1_REG_73                     (REG_TSP1_BASE + 0x1CC)          // offset: 0x73
    #define REC_ALL_PVR1                (0)                             // bit[0]
    #define REC_ALL_PVR2                (1)                             // bit[1]
    #define RESET_AFIFOC                (6)                             // bit[6]
    #define RESET_AFIFOD                (7)                             // bit[7]
    #define REC_NULL                    (14)                            // bit[14]

#define TSP1_REG_7C_7D                  (REG_TSP1_BASE + 0x1F0)          // offset: 0x7C ~ 0x7D

// TSP #3 (bank: 0x160A)
#define TSP3_REG_14                     (REG_TSP3_BASE + 0x50)          // offset: 0x14
    #define PREVENT_SRAM_COLLISION      (0)                             // bit[0]
    #define PUSI_THREE_BYTE_MODE        (1)                             // bit[1]
    #define PCR0_SRC_MASK               (0x0F00)                        // bit[11:8]
    #define PCR0_SRC_SHIFT              (8)
    #define PCR1_SRC_MASK               (0xF000)                        // bit[15:12]
    #define PCR1_SRC_SHIFT              (12)

#define TSP3_REG_15                     (REG_TSP3_BASE + 0x54)          // offset: 0x15
    #define DROP_SCR_AV_MASK            (0x003F)                        // bit[5:0]

#define TSP3_REG_1A                     (REG_TSP3_BASE + 0x68)          // offset: 0x1A
    #define PCR_PID_MASK                (0x1FFF)                        // bit[12:0]
    #define PCR_PID_SHIFT               (0)
    #define PCR_ENABLE                  (15)                            // bit[15]

#define TSP3_REG_1B                     (REG_TSP3_BASE + 0x6C)          // offset: 0x1B
    #define PCR_SRC_ID_MASK             (0x003F)
    #define PCR_SRC_ID_SHIFT            (0)

#define TSP3_REG_1C                     (REG_TSP3_BASE + 0x70)          // offset: 0x1C

#define TSP3_REG_1D                     (REG_TSP3_BASE + 0x74)          // offset: 0x1D

#define TSP3_REG_20                     (REG_TSP3_BASE + 0x80)          // offset: 0x20

#define TSP3_REG_23                     (REG_TSP3_BASE + 0x8C)          // offset: 0x23
    #define ECO_0_MASK                  (0x4C33)	// bit[1:0],[5:4],[11:10],[14]

#define TSP3_REG_28_2B                  (REG_TSP3_BASE + 0xA0)          // offset: 0x28 ~ 0x2B
    #define SYNC_BYTE_EVEN_MASK         (0x00FF)
    #define SYNC_BYTE_EVEN_SHIFT        (0)
    #define SYNC_BYTE_ODD_MASK          (0xFF00)
    #define SYNC_BYTE_ODD_SHIFT         (8)

#define TSP3_REG_2C_2D                  (REG_TSP3_BASE + 0xB0)          // offset: 0x2C ~ 0x2D
    #define SRC_ID_MASK                 (0x000F)
    #define SRC_ID_SHIFT                (0)

#define TSP3_REG_2E_31                  (REG_TSP3_BASE + 0xB8)          // offset: 0x2E ~ 0x31

#define TSP3_REG_32_33                  (REG_TSP3_BASE + 0xC8)          // offset: 0x32 ~ 0x33

#define TSP3_REG_34_37                  (REG_TSP3_BASE + 0xD0)          // offset: 0x34 ~ 0x37

#define TSP3_REG_38_39                  (REG_TSP3_BASE + 0xE0)          // offset: 0x38 ~ 0x39

#define TSP3_REG_40                     (REG_TSP3_BASE + 0x100)          // offset: 0x40
    #define FIQ_SW_RESETZ               (0)                             // bit[0]
    #define FIQ_PVR_EN                  (1)                             // bit[1]
    #define FIQ_WPTR_RST                (2)                             // bit[2]
    #define FIQ_RW_PTR_LD               (6)                             // bit[6]
    #define FIQ_RUSH_EN                 (14)                            // bit[14]
    #define FIQ_ADDR_MODE               (15)                            // bit[15]

#define TSP3_REG_41_42                  (REG_TSP3_BASE + 0x104)          // offset: 0x41 ~ 0x42

#define TSP3_REG_43_44                  (REG_TSP3_BASE + 0x10C)          // offset: 0x43 ~ 0x44

#define TSP3_REG_45_46                  (REG_TSP3_BASE + 0x114)          // offset: 0x45 ~ 0x46

#define TSP3_REG_47_48                  (REG_TSP3_BASE + 0x11C)          // offset: 0x47 ~ 0x48

#define TSP3_REG_4B                     (REG_TSP3_BASE + 0x12C)          // offset: 0x4B
    #define FIQ_BYPASS                  (0)                             // bit[0]
    #define SKIP_RUSH_DATA_MASK0        (0x13D8)

#define TSP3_REG_51_52                  (REG_TSP3_BASE + 0x144)          // offset: 0x51 ~ 0x52

#define TSP3_REG_53_54                  (REG_TSP3_BASE + 0x14C)          // offset: 0x53 ~ 0x54

#define TSP3_REG_6B                     (REG_TSP3_BASE + 0x1AC)          // offset: 0x6B
    #define SKIP_RUSH_DATA_MASK1        (0x0218)

// TSP #4 (bank: 0x1617)
#define TSP4_REG_05                     (REG_TSP4_BASE + 0x14)          // offset: 0x05

#define TSP4_REG_06                     (REG_TSP4_BASE + 0x18)          // offset: 0x06

#define TSP4_REG_07                     (REG_TSP4_BASE + 0x1C)          // offset: 0x07

#define TSP4_REG_08                     (REG_TSP4_BASE + 0x20)          // offset: 0x08

#define TSP4_REG_09                     (REG_TSP4_BASE + 0x24)          // offset: 0x09

#define TSP4_REG_0A                     (REG_TSP4_BASE + 0x28)          // offset: 0x0A

#define TSP4_REG_0D                     (REG_TSP4_BASE + 0x34)          // offset: 0x0D

#define TSP4_REG_0E                     (REG_TSP4_BASE + 0x38)          // offset: 0x0E

#define TSP4_REG_17                     (REG_TSP4_BASE + 0x5C)          // offset: 0x17

#define TSP4_REG_19                     (REG_TSP4_BASE + 0x64)          // offset: 0x19

#define TSP4_REG_7A                     (REG_TSP4_BASE + 0x1E8)          // offset: 0x7A
    #define LOCK_PKTCNT_0_LOAD          (0)                             // bit[0]
    #define LOCK_PKTCNT_1_LOAD          (1)                             // bit[1]
    #define LOCK_PKTCNT_FI_LOAD         (4)                             // bit[4]
    #define PKTCONT_V_LOAD              (8)                             // bit[8]
    #define PKTCONT_V3D_LOAD            (9)                             // bit[9]
    #define PKTCONT_A_LOAD              (10)                            // bit[10]
    #define PKTCONT_AD_LOAD             (11)                            // bit[11]
    #define PKTCONT_ADC_LOAD            (12)                            // bit[12]
    #define PKTCONT_ADD_LOAD            (13)                            // bit[13]

#define TSP4_REG_7B                     (REG_TSP4_BASE + 0x1EC)          // offset: 0x7B
    #define DROP_PKTCNT_V_LOAD          (0)                             // bit[0]
    #define DROP_PKTCNT_V3D_LOAD        (1)                             // bit[1]
    #define DROP_PKTCNT_A_LOAD          (2)                             // bit[2]
    #define DROP_PKTCNT_AD_LOAD         (3)                             // bit[3]
    #define DROP_PKTCNT_ADC_LOAD        (4)                             // bit[4]
    #define DROP_PKTCNT_ADD_LOAD        (5)                             // bit[5]
    #define DIS_CONTI_PKTCNT_V_LOAD     (8)                             // bit[8]
    #define DIS_CONTI_PKTCNT_V3D_LOAD   (9)                             // bit[9]
    #define DIS_CONTI_PKTCNT_A_LOAD     (10)                            // bit[10]
    #define DIS_CONTI_PKTCNT_AD_LOAD    (11)                            // bit[11]
    #define DIS_CONTI_PKTCNT_ADC_LOAD   (12)                            // bit[12]
    #define DIS_CONTI_PKTCNT_ADD_LOAD   (13)                            // bit[13]

#define TSP4_REG_7F                     (REG_TSP4_BASE + 0x1FC)          // offset: 0x7F
    #define DROP_PKT_MODE               (1)                             // bit[1]
    #define TSIF_SRC_SEL_MASK           (0x00E0)                        // bit[7:5]
    #define TSIF_SRC_SEL_SHIFT          (5)
    #define AV_PKT_SRC_SEL              (8)                             // bit[8]

// TSP #5 (bank: 0x163B)
#define TSP5_REG_02                     (REG_TSP5_BASE + 0x08)          // offset: 0x02
    #define TSIF_FI_EN                  (0)                             // bit[0]
    #define TSIF_FI_PARALLEL_SEL        (2)                             // bit[2]
    #define TSIF_FI_EXT_SYNC_SEL        (3)                             // bit[3]
    #define TSIF_FI_MUX_MASK            (0x0010)                        // bit[4]
    #define TSIF_FI_MUX_SHIFT           (4)
    #define TSIF_FI_MUX_FILE_PATH       (0)
    #define TSIF_FI_MUX_LIVE_PATH       (1)

#define TSP5_REG_04                     (REG_TSP5_BASE + 0x10)          // offset: 0x04
    #define TSIF_TSO_BLK_EN_MASK        (0x0F00)                        // bit[11:8]
    #define FILEIN_WB_FSM_RST           (12)                            // bit[12]

#define TSP5_REG_06                     (REG_TSP5_BASE + 0x18)          // offset: 0x06
    #define FIX_192_TIMER_0_EN          (0)                             // bit[0]
    #define FLUSH_PVR1_DATA             (8)                             // bit[8]
    #define FLUSH_PVR2_DATA             (9)                             // bit[9]

#define TSP5_REG_07                     (REG_TSP5_BASE + 0x1C)          // offset: 0x07
    #define INIT_TIMESTAMP_FILEIN       (0)                             // bit[0]
    #define MATCH_CNT_FILEIN_EN         (3)                             // bit[3]
    #define MATCH_CNT_THRESHOLD_MASK    (0x00F0)                        // bit[7:4]
    #define MATCH_CNT_THRESHOLD_SHIFT   (4)

#define TSP5_REG_0B                     (REG_TSP5_BASE + 0x2C)          // offset: 0x0B
    #define FIQ_MUX_SRC_MASK            (0x0007)                        // bit[2:0]
    #define FIQ_MUX_SRC_SHIFT           (0)

#define TSP5_REG_0C                     (REG_TSP5_BASE + 0x30)          // offset: 0x0C
    #define ECO_1_MASK                  (0x03EC)                        // bit[3:2],[7:5],[9:8]

#define TSP5_REG_5F                     (REG_TSP5_BASE + 0x17C)          // offset: 0x5F
    #define LEAF_DROP_DATA_ENABLE       (3)                             // bit[3]

#define TSP5_REG_63_64                  (REG_TSP5_BASE + 0x18C)          // offset: 0x63 ~ 0x64

#define TSP5_REG_7E                     (REG_TSP5_BASE + 0x1F8)          // offset: 0x7E
    #define CHECK_BURST_LEN             (9)                             // bit[9]

#define TSP5_REG_7F                     (REG_TSP5_BASE + 0x1FC)          // offset: 0x7F
    #define MCM_TURN_OFF_ALL            (0)                             // bit[0]

// TSP #6 (bank: 0x161F)
#define TSP6_REG_26                     (REG_TSP6_BASE + 0x98)          // offset: 0x26
    #define RESET_TSIF_0                (8)                             // bit[8]

#define TSP6_REG_41                     (REG_TSP6_BASE + 0x104)          // offset: 0x41
    #define PVR1_WP_LD                  (0)                             // bit[0]
    #define PVR2_WP_LD                  (1)                             // bit[1]

#define TSP6_REG_45_46                  (REG_TSP6_BASE + 0x114)          // offset: 0x45 ~ 0x46

#define TSP6_REG_7D                     (REG_TSP6_BASE + 0x1F4)          // offset: 0x7D
    #define FIX_192_UNLOCK_HANG_EN      (1)                             // bit[1]
    #define PATHID_CHK_IN_SRCID_PAR_DIS (2)                             // bit[2]

// TSO #0 (bank: 0x113A)
#define TSO0_REG_00                     (REG_TSO0_BASE + 0x00)          // offset: 0x00
    #define SW_RESETZ_DISABLE_MASK      (0x0001)                        // bit[0]
    #define SW_RESETZ_0_MASK            (0x00FE)

#define TSO0_REG_01                     (REG_TSO0_BASE + 0x04)          // offset: 0x01
    #define SW_RESETZ_1_MASK            (0x0031)                        // bit[0],[4],[5]

// [need]
#define TSO0_REG_06                     (REG_TSO0_BASE + 0x18)          // offset: 0x06
    #define TSO_TSIF_P_SEL              (0)                             // bit[0]
    #define TSO_TSIF_EXT_SYNC_SEL       (1)                             // bit[1]
    #define TSO_TSIF_SIN_C0             (2)                             // bit[2]
    #define TSO_TSIF_SIN_C1             (3)                             // bit[3]
    #define TSO_TSIF_PDFLT_REC_ALL      (4)                             // bit[4]
    #define TSO_TSIF_PDFLT_REC_NULL     (5)                             // bit[5]

#define TSO0_REG_16                     (REG_TSO0_BASE + 0x58)          // offset: 0x16

#define TSO0_REG_1A                     (REG_TSO0_BASE + 0x68)          // offset: 0x1A

#define TSO0_REG_1C                     (REG_TSO0_BASE + 0x70)          // offset: 0x1C
    #define TSO_CMD_QUEUE_STATUS_LD     (5)                             // bit[5]
    #define TSO_CMD_QUEUE_STATUS_LD1    (13)                            // bit[13]

#define TSO0_REG_1D                     (REG_TSO0_BASE + 0x74)          // offset: 0x1D
    #define TSO_OUT_EN                  (0)                             // bit[0]
    #define TSO_TSIF1_EN                (1)                             // bit[1]
    #define TSO_TSIF5_EN                (5)                             // bit[5]
    #define TSO_TSIF6_EN                (6)                             // bit[6]
    #define TSO_PKT_PARAMETERS_LD       (15)                            // bit[15]

#define TSO0_REG_1E                     (REG_TSO0_BASE + 0x78)          // offset: 0x1E
    #define TSO_VALID_BYTECNT_MASK      (0x00FF)                        // bit[7:0]
    #define TSO_VALID_BYTECNT_SHIFT     (0)
    #define TSO_INVALID_BYTECNT_MASK    (0xFF00)                        // bit[15:8]
    #define TSO_INVALID_BYTECNT_SHIFT   (8)

#define TSO0_REG_1F                     (REG_TSO0_BASE + 0x7C)          // offset: 0x1F

#define TSO0_REG_43                     (REG_TSO0_BASE + 0x10C)          // offset: 0x43
    #define TSO_ENABLE_SYS_TIMESTAMP    (1)                             // bit[1]
    #define TSO_FIX_TIMESTAMP_RING_BACK (5)                             // bit[5]
    #define TSO_FIX_LPCR_RING_BACK      (6)                             // bit[6]
    #define TSO_INIT_STAMP_RSTART_EN    (7)                             // bit[7]

#define TSO0_REG_44                     (REG_TSO0_BASE + 0x110)          // offset: 0x44
    #define TSO_BYPASS_SVQ              (7)                             // bit[7]

// TSO #1 (bank: 0x123D)
#define TSO1_REG_00                     (REG_TSO1_BASE + 0x00)          // offset: 0x00

#define TSO1_REG_10                     (REG_TSO1_BASE + 0x40)          // offset: 0x10

#define TSO1_REG_14                     (REG_TSO1_BASE + 0x50)          // offset: 0x14

#define TSO1_REG_18_19                  (REG_TSO1_BASE + 0x60)          // offset: 0x18 ~ 0x19

#define TSO1_REG_1A                     (REG_TSO1_BASE + 0x68)          // offset: 0x1A

#define TSO1_REG_1B                     (REG_TSO1_BASE + 0x6C)          // offset: 0x1B
    #define SVQ_TX_FORCE_FIRE_CNT_MASK  (0x0F00)                        // bit[11:8]
    #define SVQ_TX_FORCE_FIRE_CNT_SHIFT (8)
    #define SVQ_TX_RESET                (12)                            // bit[12]
    #define SVQ_TX_ENABLE               (15)                            // bit[15]

#define TSO1_REG_28_29                  (REG_TSO1_BASE + 0xA0)          // offset: 0x28 ~ 0x29

#define TSO1_REG_2A                     (REG_TSO1_BASE + 0xA8)          // offset: 0x2A

#define TSO1_REG_2B                     (REG_TSO1_BASE + 0xAC)          // offset: 0x2B

#define TSO1_REG_2C_2D                  (REG_TSO1_BASE + 0xB0)          // offset: 0x2C ~ 0x2D

#define TSO1_REG_2E                     (REG_TSO1_BASE + 0xE8)          // offset: 0x2E

#define TSO1_REG_2F                     (REG_TSO1_BASE + 0xBC)          // offset: 0x2F

#define TSO1_REG_30                     (REG_TSO1_BASE + 0xC0)          // offset: 0x30
    #define SVQ_RX_MODE_MASK            (0x0003)
    #define SVQ_RX_MODE_SHIFT           (0)
    #define SVQ_RX_MODE_OPENCABLE       (0)
    #define SVQ_RX_MODE_CIPLUS          (1)
    #define SVQ_RX_MODE_192_PKT         (2)
    #define SVQ_RX_MODE_DONGLE          (3)

// TSO #2 (bank: 0x1539)
#define TSO2_REG_21                     (REG_TSO2_BASE + 0x84)          // offset: 0x21
    #define SW_RESET_TS_MMT             (2)                             // bit[2]

// TS_SAMPLE (bank: 0x110A)
#define TSO_OUT_CLK_SAMPLE              (REG_TS_SAMPLE_BASE + 0x0C)     // offset: 0x03
    #define TSO_OUT_SAMPLE_CNT_MASK     (0x001F)                        // bit[4:0]
    #define TSO_OUT_SAMPLE_CNT_SHIFT    (0)
    #define TSO_OUT_CLK_PHASE_ADJUST_EN (5)                             // bit[5]

//[need]
#define CLK_TSIF_CKG                    (REG_TS_SAMPLE_BASE + 0x138)     // offset: 0x4E
    #define TSIF0_CLK_DISABLE           (0)                             // bit[0]
    #define TSIF0_CLK_INVERT            (1)                             // bit[1]
    #define TSIF0_CLK_SRC_MASK          (0x003C)                        // bit[5:2]
    #define TSIF0_CLK_SRC_SHIFT         (2)
    #define TSIF_CLK_PAD0               (0)
    #define TSIF_CLK_PAD1               (1)
    #define TSIF_CLK_PAD2               (2)
    #define TSIF_CLK_PAD3               (3)
    #define TSIF_CLK_TSO                (10)
    #define TSIF_CLK_CILINK0            (12)
    #define TSIF_CLK_CILINK1            (13)
    #define TSIF_CLK_DEMOD              (15)

    #define TSIF1_CLK_DISABLE           (8)                             // bit[8]
    #define TSIF1_CLK_INVERT            (9)                             // bit[9]
    #define TSIF1_CLK_SRC_MASK          (0x3C00)                        // bit[13:10]
    #define TSIF1_CLK_SRC_SHIFT         (10)

#define CLK_TSIF_FI_CKG                 (REG_TS_SAMPLE_BASE + 0x140)     // offset: 0x50
    #define TSIF_FI_CLK_DISABLE         (8)                             // bit[8]
    #define TSIF_FI_CLK_INVERT          (9)                             // bit[9]
    #define TSIF_FI_CLK_SRC_MASK        (0x3C00)                        // bit[13:10]
    #define TSIF_FI_CLK_SRC_SHIFT       (10)

#define CLK_TSO_IN_CKG                  (REG_TS_SAMPLE_BASE + 0x14C)     // offset: 0x53
    #define TSO_TSIF1_CLK_DISABLE       (8)                             // bit[8]
    #define TSO_TSIF1_CLK_INVERT        (9)                             // bit[9]
    #define TSO_TSIF1_CLK_SRC_MASK      (0x3C00)                        // bit[13:10]
    #define TSO_TSIF1_CLK_SRC_SHIFT     (10)

#define CLK_TSO1_IN_CKG                 (REG_TS_SAMPLE_BASE + 0x150)     // offset: 0x54
    #define TSO_TSIF2_CLK_DISABLE       (0)                             // bit[0]
    #define TSO_TSIF2_CLK_INVERT        (1)                             // bit[1]
    #define TSO_TSIF2_CLK_SRC_MASK      (0x003C)                        // bit[5:2]
    #define TSO_TSIF2_CLK_SRC_SHIFT     (2)
    #define TSO_TSIF3_CLK_DISABLE       (8)                             // bit[8]
    #define TSO_TSIF3_CLK_INVERT        (9)                             // bit[9]
    #define TSO_TSIF3_CLK_SRC_MASK      (0x3C00)                        // bit[13:10]
    #define TSO_TSIF3_CLK_SRC_SHIFT     (10)

#define CLK_TS_TX_CILINK_CKG            (REG_TS_SAMPLE_BASE + 0x164)    // offset: 0x59
    #define TS_TX_C1_CLK_DISABLE        (0)                             // bit[0]
    #define TS_TX_C1_CLK_INVERT         (1)                             // bit[1]
    #define TS_TX_C1_CLK_SRC_MASK       (0x003C)                        // bit[5:2]
    #define TS_TX_C1_CLK_SRC_SHIFT      (2)
    #define TS_TX_C1_CLK_DIV8_MASK      (0x00C0)                        // bit[7:6]
    #define TS_TX_C1_CLK_DIV8_SHIFT     (5)
    #define TS_TX_C2_CLK_DISABLE        (0)                             // bit[8]
    #define TS_TX_C2_CLK_INVERT         (1)                             // bit[9]
    #define TS_TX_C2_CLK_SRC_MASK       (0x003C)                        // bit[13:10]
    #define TS_TX_C2_CLK_SRC_SHIFT      (2)
    #define TS_TX_C2_CLK_DIV8_MASK      (0x00C0)                        // bit[15:14]
    #define TS_TX_C2_CLK_DIV8_SHIFT     (5)
    #define TS_TX_CL_CLK_SRC_PAD0       (0)
    #define TS_TX_CL_CLK_SRC_PAD1       (1)
    #define TS_TX_CL_CLK_SRC_PAD2       (2)
    #define TS_TX_CL_CLK_SRC_PAD3       (3)
    #define TS_TX_CL_CLK_SRC_PAD4       (4)
    #define TS_TX_CL_CLK_SRC_PAD5       (5)
    #define TS_TX_CL_CLK_SRC_TSO        (10)
    #define TS_TX_CL_CLK_SRC_C1         (12)
    #define TS_TX_CL_CLK_SRC_C2         (13)
    #define TS_TX_CL_CLK_SRC_DEMOD      (15)
    #define TS_TX_CL_CLK_XTAL_24M       (0)
    #define TS_TX_CLCLK_TS_TX_C1        (1)
    #define TS_TX_CLCLK_TS_TX_C2        (1)
    #define TS_TX_CLCLK_TS_TX_C1_DIV8   (2)
    #define TS_TX_CLCLK_TS_TX_C2_DIV8   (2)

#define CLK_CILINK_2B_RX_TX_CKG         (REG_TS_SAMPLE_BASE + 168)      // offset: 0x5A
    #define CLK_CILINK_2B_RX_DISABLE    (0)
    #define CLK_CILINK_2B_RX_INVERT     (1)
    #define CLK_CILINK_2B_RX_MASK       (0x0F00)                        // bit[11:8]
    #define CLK_CILINK_2B_RX_SHIFT      (7)
    #define CLK_CILINK_2B_RX_XTAL_24M   (0)
    #define CLK_CILINK_2B_RX_PAD        (1)
    #define CLK_CILINK_2B_RX_144Mhz     (2)
    #define CLK_CILINK_2B_RX_160Mhz     (3)
    #define CLK_CILINK_2B_TX_DISABLE    (0)
    #define CLK_CILINK_2B_TX_INVERT     (1)
    #define CLK_CILINK_2B_TX_MASK       (0xF000)                        // bit[15:12]
    #define CLK_CILINK_2B_TX_SHIFT      (11)
    #define CLK_CILINK_2B_TX_XTAL_24M   (0)
    #define CLK_CILINK_2B_TX_PAD        (1)
    #define CLK_CILINK_2B_TX_144M       (2)
    #define CLK_CILINK_2B_TX_160M       (3)

#define CLK_CILINK_IF_CKG               (REG_TS_SAMPLE_BASE + 16C)      // offset 0x5B
    #define CLK_CILINK_IF_DISABLE       (0)                             // bit[0]
    #define CLK_CILINK_IF_INVERT        (1)                             // bit[1]
    #define CLK_CILINK_IF_SRC_MASK      (0x000c)                        // bit[2-3]
    #define CLK_CILINK_IF_SRC_SHIFT     (2)
    #define CLK_CILINK_IF_XTAL_24M      (0)
    #define CLK_CILINK_IF_36M           (1)
    #define CLK_CILINK_IF_27M           (2)

#define CLK_2B_TX_EXT_INV_CKG           (REG_TS_SAMPLE_BASE + 170)       // offset: 0x5C
    #define CLK_2B_TX_EXT_INV           (1)                             // bit[0]

#define CLK_TSO_OUT_CKG                 (REG_TS_SAMPLE_BASE + 0x178)     // offset: 0x5E
    #define TSO_OUT_CLK_DISABLE         (0)                             // bit[0]
    #define TSO_OUT_CLK_INVERT          (1)                             // bit[1]
    #define TSO_OUT_CLK_SRC_MASK        (0x001C)                        // bit[4:2]
    #define TSO_OUT_CLK_SRC_SHIFT       (2)
    #define TSO_OUT_CLK_SRC_XTAL_24M    (0)
    #define TSO_OUT_CLK_DIV_2N          (2)
    #define TSO_OUT_CLK_P_TSO_OUT       (3)
    #define TSO_OUT_CLK_P_TSO_OUT_DIV8  (4)
    #define TSO_OUT_CLK_27MHZ           (5)
    #define TSO_OUT_CLK_DIV_MN          (6)
    #define TSO_OUT_CLK_INT_DEMOD       (7)
    #define TSO_OUT_CLK_P_TSO_DISABLE   (8)                             // bit[8]
    #define TSO_OUT_CLK_P_TSO_INVERT    (9)                             // bit[9]
    #define TSO_OUT_CLK_P_TSO_SRC_MASK  (0x1C00)                        // bit[12:10]
    #define TSO_OUT_CLK_P_TSO_SRC_SHIFT (10)
    #define TSO_OUT_CLK_P_TSO_SRC_TS0   (0)
    #define TSO_OUT_CLK_P_TSO_SRC_TS1   (1)
    #define TSO_OUT_CLK_P_TSO_SRC_TS2   (2)

#define CLK_TSO_OUT_PHASE_CFG           (REG_TS_SAMPLE_BASE + 0x17C)     // offset: 0x5F
    #define TSO_OUT_CLK_DIV_NUM_MASK    (0x00FF)                        // bit[7:0]
    #define TSO_OUT_CLK_DIV_NUM_SHIFT   (0)
    #define TSO_OUT_CLK_PHTUNE_MASK     (0xFF00)                        // bit[15:8]
    #define TSO_OUT_CLK_PHTUNE_SHIFT    (8)

#define CLK_TSO_OUT_DIV_CFG             (REG_TS_SAMPLE_BASE + 0x180)     // offset: 0x60
    #define TSO_OUT_DIV_SEL_SRC_MASK    (0x0003)                        // bit[0]
    #define TSO_OUT_DIV_SEL_SRC_SHIFT       (0)
    #define TSO_OUT_DIV_SEL_SRC_XTAL_24MHZ  (0)
    #define TSO_OUT_DIV_SEL_SRC_172MHZ      (1)
    #define TSO_OUT_DIV_SEL_SRC_288MHZ      (2)
    #define TSO_OUT_PH_TUNE_EN              (2)                         // bit[2]

#define CLK_TSO_OUT_TO_PAD              (REG_TS_SAMPLE_BASE + 0x18C)     // offset: 0x63
    #define TSO_TO_PAD_INVERT           (0)                             // bit[0]
    #define TSO_TS_SAMPLE_CLK_MASK      (0x0C00)                        // bit[11:10]
    #define TSO_TS_SAMPLE_CLK_SHIFT     (9)
    #define TSO_TS_SAMPLE_CLK_216MHz    (0)
    #define TSO_TS_SAMPLE_CLK_432MHz    (1)

#endif /* MTK_DMX_REGS_H */
