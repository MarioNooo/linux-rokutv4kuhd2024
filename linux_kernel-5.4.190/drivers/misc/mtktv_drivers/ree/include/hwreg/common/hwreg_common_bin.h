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

#ifndef _HWREG_COMMON_BIN_H_
#define _HWREG_COMMON_BIN_H_

#include "hwreg_common.h"

#define PQ_NULL_POINTER(pPointer)           ((pPointer == NULL)?(true):(false))

#define PQ_BIN_SOURCELUT_DATA_LEN           (2) // 2 bytes

#define PQ_IP_NULL                          (0xFFFF)
#define PQ_TAB_IP_NULL                      (0xFFFF)
#define PQ_IP_NUM_NULL                      (0xFFFF)

#define PQ_BIN_MAX_IP                       (65535)
#define PQ_BIN_MAX_DOAMIN                   (30)

#define PQ_BIN_IP_ALL                       (0xFFFF)
#define PQ_BIN_IP_NULL                      (0xFFFF)
#define PQ_BIN_IP_COMM                      (0xFFFE)
#define PQ_BIN_ADDR_NULL                    (0xFFFFFFFF)

#define PQ_BIN_BANK_SIZE                    (1)
#define PQ_BIN_BANK_SIZE_2BYTE              (2)
#define PQ_BIN_ADDR_SIZE                    (1)
#define PQ_BIN_MASK_SIZE                    (1)
#define PQ_BIN_ADDR_SIZE_2BYTE              (2)
#define PQ_BIN_MASK_SIZE_2BYTE              (2)

// SkipRule
#define PQ_BIN_SKIPRULE_HEADER_LEN          (16)
#define PQ_BIN_SKIPRULE_IP_NUM_OFFSET       (0)
#define PQ_BIN_SKIPRULE_OFFSET_OFFSET       (PQ_BIN_SKIPRULE_IP_NUM_OFFSET + 2)

// IP & Common
#define PQ_BIN_IP_COMM_HEADER_LEN           (16)
#define PQ_BIN_IP_COMM_IP_NUM_LEN           (2)
#define PQ_BIN_IP_COMM_TABLE_TYPE_OFFSET    (0)
#define PQ_BIN_IP_COMM_COMM_REG_NUM_OFFSET  (PQ_BIN_IP_COMM_TABLE_TYPE_OFFSET + 2)
#define PQ_BIN_IP_COMM_IP_REG_NUM_OFFSET    (PQ_BIN_IP_COMM_COMM_REG_NUM_OFFSET + 2)
#define PQ_BIN_IP_COMM_IP_GROUP_NUM_OFFSET  (PQ_BIN_IP_COMM_IP_REG_NUM_OFFSET + 2)
#define PQ_BIN_IP_COMM_COMMOFFSET_OFFSET    (PQ_BIN_IP_COMM_IP_GROUP_NUM_OFFSET + 2)
#define PQ_BIN_IP_COMM_IPOFFSET_OFFSET      (PQ_BIN_IP_COMM_COMMOFFSET_OFFSET + 4)

// SourceLUT
#define PQ_BIN_SOURCELUT_HEADER_LEN         (16)
#define PQ_BIN_SOURCELUT_PANEL_NUM_LEN      (2)

#define PQ_BIN_SOURCELUT_SOURCE_NUM_OFFSET  (0)
#define PQ_BIN_SOURCELUT_IP_NUM_OFFSET      (PQ_BIN_SOURCELUT_SOURCE_NUM_OFFSET + 2)
#define PQ_BIN_SOURCELUT_OFFSET_OFFSET      (PQ_BIN_SOURCELUT_IP_NUM_OFFSET + 2)

#define PQ_BIN_IP_COMM_DATA_LEN             (1) // 1 byte

//Size of each table
#define PQ_BIN_GRULE_INFO_SIZE              (14)
#define PQ_BIN_GRULE_LEVEL_INFO_SIZE        (6)
#define PQ_BIN_XRULE_INFO_SIZE              (12)
#define PQ_BIN_SKIPRULE_INFO_SIZE           (6)
#define PQ_BIN_IP_COMM_INFO_SIZE            (16)
#define PQ_BIN_SOURCELUT_INFO_SIZE          (8)

#define PQ_BIN_HEADER_LEN                   (16)
#define PQ_BIN_VERSION_LEN                  (16)
#define PQ_BIN_DUMMY                        (8)

#define PQ_HM				    (1)
/* ============================================================================================== */
/* ------------------------------------------ Enums --------------------------------------------- */
/* ============================================================================================== */
typedef enum {
	PQ_TABTYPE_GENERAL = 0,
	PQ_TABTYPE_COMB = 1,
	PQ_TABTYPE_SCALER = 2,
	PQ_TABTYPE_SRAM1 = 3,
	PQ_TABTYPE_SRAM2 = 4,
	PQ_TABTYPE_SRAM3 = 5,
	PQ_TABTYPE_SRAM4 = 6,
	PQ_TABTYPE_C_SRAM1 = 7,
	PQ_TABTYPE_C_SRAM2 = 8,
	PQ_TABTYPE_C_SRAM3 = 9,
	PQ_TABTYPE_C_SRAM4 = 10,
	PQ_TABTYPE_SRAM_COLOR_INDEX = 11,
	PQ_TABTYPE_SRAM_COLOR_GAIN_SNR = 12,
	PQ_TABTYPE_SRAM_COLOR_GAIN_DNR = 13,
	PQ_TABTYPE_VIP_IHC_CRD_SRAM = 14,
	PQ_TABTYPE_VIP_ICC_CRD_SRAM = 15,
	PQ_TABTYPE_SRAM_XVYCC_DE_GAMMA = 16,
	PQ_TABTYPE_SRAM_XVYCC_GAMMA = 17,
	PQ_TABTYPE_SWDRIVER = 18,
	PQ_TABTYPE_SRAM_LinearRGB_DE_GAMMA = 19,
	PQ_TABTYPE_SRAM_LinearRGB_GAMMA = 20,
	PQ_TABTYPE_VIP_Peaking_CAG_SRAM = 21,
	PQ_TABTYPE_SCALER_EX = 22,
	PQ_TABTYPE_UFSC_SCALER              = 52,
	PQ_TABTYPE_HFR_SRAM1                = 73,
	PQ_TABTYPE_HFR_SRAM2                = 74,
	PQ_TABTYPE_HFR_SRAM3                = 75,
	PQ_TABTYPE_HFR_SRAM4 = 76,
	PQ_TABTYPE_HFR_C_SRAM1 = 77,
	PQ_TABTYPE_HFR_C_SRAM2 = 78,
	PQ_TABTYPE_HFR_C_SRAM3 = 79,
	PQ_TABTYPE_HFR_C_SRAM4 = 80,
	PQ_TABTYPE_DIP_SRAM1 = 40,
	PQ_TABTYPE_DIP_SRAM2 = 41,
	PQ_TABTYPE_DIP_C_SRAM1 = 44,
	PQ_TABTYPE_DIP_C_SRAM2 = 45,
	PQ_TABTYPE_FO_SRAM1 = 81,
	PQ_TABTYPE_FO_SRAM2 = 82,
	PQ_TABTYPE_FO_SRAM3 = 83,
	PQ_TABTYPE_FO_SRAM4 = 84,
	PQ_TABTYPE_FO_C_SRAM1 = 85,
	PQ_TABTYPE_FO_C_SRAM2 = 86,
	PQ_TABTYPE_FO_C_SRAM3 = 87,
	PQ_TABTYPE_FO_C_SRAM4 = 88,
	PQ_TABTYPE_AI_SRAM1 = 89,
	PQ_TABTYPE_AI_SRAM2 = 90,
	PQ_TABTYPE_AI_C_SRAM1 = 91,
	PQ_TABTYPE_AI_C_SRAM2 = 92,
	PQ_TABTYPE_DIPR_SRAM1 = 93,
	PQ_TABTYPE_DIPR_SRAM2 = 94,
	PQ_TABTYPE_DIPR_C_SRAM1 = 95,
	PQ_TABTYPE_DIPR_C_SRAM2 = 96,
	PQ_TABTYPE_DumF_SERIES = 100,
	PQ_TABTYPE_TWOBYTE_BANK = 110,
	PQ_TABTYPE_TWOBYTE_BANK_MASK = 111,
	PQ_TABTYPE_HWMAP_HWIP = 120,
	PQ_TABTYPE_HWMAP_HWOPF = 121,
	PQ_TABTYPE_HWMAP_HWOPB = 122,
	PQ_TABTYPE_HWMAP_HWIP_TWOBYTE = 123,
	PQ_TABTYPE_HWMAP_HWOPF_TWOBYTE = 124,
	PQ_TABTYPE_HWMAP_HWOPB_TWOBYTE = 125,
} EN_PQ_TABTYPE;

typedef enum
{
	/// Select main window standard PQ setting for each input source/resolution
	E_PQ_BIN_STD_MAIN,
	/// The max number of PQ Bin
	E_PQ_BIN_NUM,
} EN_PQ_BIN_TYPE;

typedef enum
{
	PQ_INPUTSRC_NONE = 0,      ///<NULL input
	PQ_INPUTSRC_VGA,           ///<1   VGA input
	PQ_INPUTSRC_TV,            ///<2   TV input

	PQ_INPUTSRC_CVBS,          ///<3   AV 1
	PQ_INPUTSRC_CVBS2,         ///<4   AV 2
	PQ_INPUTSRC_CVBS3,         ///<5   AV 3
	PQ_INPUTSRC_CVBS4,         ///<6   AV 4
	PQ_INPUTSRC_CVBS5,         ///<7   AV 5
	PQ_INPUTSRC_CVBS6,         ///<8   AV 6
	PQ_INPUTSRC_CVBS7,         ///<9   AV 7
	PQ_INPUTSRC_CVBS8,         ///<10   AV 8
	PQ_INPUTSRC_CVBS_MAX,      ///<11 AV max

	PQ_INPUTSRC_SVIDEO,        ///<12 S-video 1
	PQ_INPUTSRC_SVIDEO2,       ///<13 S-video 2
	PQ_INPUTSRC_SVIDEO3,       ///<14 S-video 3
	PQ_INPUTSRC_SVIDEO4,       ///<15 S-video 4
	PQ_INPUTSRC_SVIDEO_MAX,    ///<16 S-video max

	PQ_INPUTSRC_YPBPR,         ///<17 Component 1
	PQ_INPUTSRC_YPBPR2,        ///<18 Component 2
	PQ_INPUTSRC_YPBPR3,        ///<19 Component 3
	PQ_INPUTSRC_YPBPR_MAX,     ///<20 Component max

	PQ_INPUTSRC_SCART,         ///<21 Scart 1
	PQ_INPUTSRC_SCART2,        ///<22 Scart 2
	PQ_INPUTSRC_SCART_MAX,     ///<23 Scart max

	PQ_INPUTSRC_HDMI,          ///<24 HDMI 1
	PQ_INPUTSRC_HDMI2,         ///<25 HDMI 2
	PQ_INPUTSRC_HDMI3,         ///<26 HDMI 3
	PQ_INPUTSRC_HDMI4,         ///<27 HDMI 4
	PQ_INPUTSRC_HDMI_MAX,      ///<28 HDMI max

	PQ_INPUTSRC_DVI,           ///<29 DVI 1
	PQ_INPUTSRC_DVI2,          ///<30 DVI 2
	PQ_INPUTSRC_DVI3,          ///<31 DVI 2
	PQ_INPUTSRC_DVI4,          ///<32 DVI 4
	PQ_INPUTSRC_DVI_MAX,       ///<33 DVI max

	PQ_INPUTSRC_DTV,           ///<34 DTV
	PQ_INPUTSRC_DTV2,          ///<35 DTV2
	PQ_INPUTSRC_DTV_MAX,       ///<36 DTV max

	// Application source
	PQ_INPUTSRC_STORAGE,       ///<37 Storage
	PQ_INPUTSRC_STORAGE2,      ///<38 Storage2
	PQ_INPUTSRC_STORAGE_MAX,   ///<39 Storage max

	// Support OP capture
	PQ_INPUTSRC_SCALER_OP,     ///<40 scaler OP

	PQ_INPUTSRC_NUM,           ///<number of the source
}pq_input_source_type;


typedef enum {
	hwmap_source_ipdma = 0,
	hwmap_source_b2r,
	hwmap_source_max,
} hwmap_source;

typedef enum {
	hwmap_mode_ts = 0,
	hwmap_mode_legacy,
	hwmap_mode_max,
} hwmap_mode;

typedef enum {
	hwmap_buffer_mode_fb = 0,
	hwmap_buffer_mode_fbl,
	hwmap_buffer_mode_rfbl,
	hwmap_buffer_mode_max,
} hwmap_buffer_mode;

typedef enum {
	hwmap_scene_normal = 0,
	hwmap_scene_game,
	hwmap_scene_pc,
	hwmap_scene_memc,
	hwmap_scene_pip,
	hwmap_scene_max,
} hwmap_scene;

typedef enum {
	hwmap_framerate_24 = 0,
	hwmap_framerate_30,
	hwmap_framerate_50,
	hwmap_framerate_60,
	hwmap_framerate_120,
	hwmap_framerate_max,
} hwmap_framerate;

typedef enum {
	hwmap_quality_full = 0,
	hwmap_quality_lite,
	hwmap_quality_zfd,
	hwmap_quality_max,
} hwmap_quality;

typedef enum {
	hwmap_colorformat_rgb = 0,
	hwmap_colorformat_yuv,
	hwmap_colorformat_max,
} hwmap_colorformat;

typedef enum {
	hwmap_ip_sw_scmi_ch0_bit = 0,
	hwmap_ip_sw_scmi_ch1_bit,
	hwmap_ip_sw_scmi_ch2_bit,
	hwmap_ip_sw_scmi_frame_num,
	hwmap_ip_sw_scmi_frame_diff,
	hwmap_ip_sw_ucm_ch0_bit,
	hwmap_ip_sw_ucm_ch1_bit,
	hwmap_ip_sw_ucm_ch2_bit,
	hwmap_ip_sw_ucm_frame_num,
	hwmap_ip_sw_ucm_frame_diff,
	hwmap_ip_sw_znr_ch0_bit,
	hwmap_ip_sw_znr_ch1_bit,
	hwmap_ip_sw_znr_frame_num,
	hwmap_ip_sw_abf_ch0_bit,
	hwmap_ip_sw_abf_ch1_bit,
	hwmap_ip_sw_abf_ch2_bit,
	hwmap_ip_sw_abf_frame_num,
	hwmap_ip_sw_mcdi_frame_num,
	hwmap_ip_sw_max
} hwmap_ip_sw;

typedef enum
{
	hwmap_ip_sw_bit_bypass,
	hwmap_ip_sw_bit_0b,
	hwmap_ip_sw_bit_1b,
	hwmap_ip_sw_bit_2b,
	hwmap_ip_sw_bit_3b,
	hwmap_ip_sw_bit_4b,
	hwmap_ip_sw_bit_5b,
	hwmap_ip_sw_bit_6b,
	hwmap_ip_sw_bit_7b,
	hwmap_ip_sw_bit_8b,
	hwmap_ip_sw_bit_9b,
	hwmap_ip_sw_bit_10b,
	hwmap_ip_sw_bit_11b,
	hwmap_ip_sw_bit_12b,
	hwmap_ip_sw_bit_13b,
	hwmap_ip_sw_bit_14b,
	hwmap_ip_sw_bit_15b,
	hwmap_ip_sw_bit_16b,
	hwmap_ip_sw_bit_17b,
	hwmap_ip_sw_bit_18b,
	hwmap_ip_sw_bit_19b,
	hwmap_ip_sw_bit_20b,
	hwmap_ip_sw_bit_21b,
	hwmap_ip_sw_bit_22b,
	hwmap_ip_sw_bit_23b,
	hwmap_ip_sw_bit_24b,
	hwmap_ip_sw_bit_25b,
	hwmap_ip_sw_bit_26b,
	hwmap_ip_sw_bit_27b,
	hwmap_ip_sw_bit_28b,
	hwmap_ip_sw_bit_29b,
	hwmap_ip_sw_bit_30b,
	hwmap_ip_sw_bit_31b,
	hwmap_ip_sw_bit_32b,
	hwmap_ip_sw_bit_33b,
	hwmap_ip_sw_bit_34b,
	hwmap_ip_sw_bit_35b,
	hwmap_ip_sw_bit_36b,
	hwmap_ip_sw_bit_37b,
	hwmap_ip_sw_bit_38b,
	hwmap_ip_sw_bit_39b,
	hwmap_ip_sw_bit_40b,
	hwmap_ip_sw_bit_41b,
	hwmap_ip_sw_bit_42b,
	hwmap_ip_sw_bit_43b,
	hwmap_ip_sw_bit_44b,
	hwmap_ip_sw_bit_45b,
	hwmap_ip_sw_bit_46b,
	hwmap_ip_sw_bit_47b,
	hwmap_ip_sw_bit_48b,
	hwmap_ip_sw_bit_49b,
	hwmap_ip_sw_bit_50b,
	hwmap_ip_sw_bit_51b,
	hwmap_ip_sw_bit_52b,
	hwmap_ip_sw_bit_53b,
	hwmap_ip_sw_bit_54b,
	hwmap_ip_sw_bit_55b,
	hwmap_ip_sw_bit_56b,
	hwmap_ip_sw_bit_57b,
	hwmap_ip_sw_bit_58b,
	hwmap_ip_sw_bit_59b,
	hwmap_ip_sw_bit_60b,
	hwmap_ip_sw_bit_61b,
	hwmap_ip_sw_bit_62b,
	hwmap_ip_sw_bit_63b,
	hwmap_ip_sw_bit_64b,
	hwmap_ip_sw_bit_65b,
	hwmap_ip_sw_bit_66b,
	hwmap_ip_sw_bit_67b,
	hwmap_ip_sw_bit_68b,
	hwmap_ip_sw_bit_69b,
	hwmap_ip_sw_bit_70b,
	hwmap_ip_sw_bit_71b,
	hwmap_ip_sw_bit_72b,
	hwmap_ip_sw_bit_default,
	hwmap_ip_sw_bit_max
} hwmap_ip_sw_bit;


/* ============================================================================================== */
/* ----------------------------------------- Structs -------------------------------------------- */
/* ============================================================================================== */
typedef struct
{
	uint8_t  u8BinID;
	size_t   u32BinStartAddress;
	char     u8Header[PQ_BIN_HEADER_LEN];
	char     u8Version[PQ_BIN_VERSION_LEN];
	uint32_t u32StartOffset;
	uint32_t u32EndOffset;
	uint8_t  u8Dummy[PQ_BIN_DUMMY];

	// GRule
	char     u8GRuleHader[PQ_BIN_HEADER_LEN];
	uint16_t u16GRule_RuleNum;
	uint16_t u16GRule_PnlNum;
	uint32_t u32GRule_Offset;
	uint32_t u32GRule_Pitch;
	uint32_t u32GRule_Lvl_Offset;
	uint32_t u32GRule_Lvl_Pitch;

	// XRule
	char     u8XRuleHader[PQ_BIN_HEADER_LEN];
	uint16_t u16XRuleNum;
	uint32_t u32XRule_Offset;
	uint32_t u32XRUle_Pitch;

	// SkipRule
	char     u8SkipRuleHader[PQ_BIN_HEADER_LEN];
	uint16_t u16SkipRule_IPNum;
	uint32_t u32SkipRule_Offset;

	// IP & Common
	char     u8IP_Comm_Header[PQ_BIN_HEADER_LEN];
	uint16_t u16IP_Comm_Num;
	uint32_t u32IP_Comm_Offset;
	uint32_t u32IP_Comm_Pitch;

	// SourceLUT
	uint8_t  u8SourceLUT_Header[PQ_BIN_HEADER_LEN];
	uint16_t u16SourceLUT_PnlNum;
	uint32_t u32SourceLUT_Offset;
	uint32_t u32SourceLUT_Pitch;
} MS_PQBin_Header_Info;

typedef struct
{
	size_t   tabOffset;
	uint16_t regNum;
	uint16_t tableType;
	uint16_t groupNum;
	uint16_t groupIdx;
} MS_PQBin_IP_Table_Info;


typedef struct {
	uint32_t source;
	uint32_t size;
	uint32_t mode;
	uint32_t scene;
	uint32_t framerate;
	uint32_t scan;
	uint32_t lowlatency;
	uint32_t dynamicstream;
	uint32_t pq;
	uint32_t colorformat;
} hwmap_condition;

typedef struct  {
	uint32_t width;
	uint32_t height;
	bool interlace;
	bool adaptive_stream;
	bool low_latency;
	hwmap_source source;
	hwmap_mode mode;
	hwmap_scene scene;
	hwmap_framerate framerate;
	hwmap_quality quality;
	hwmap_colorformat colorformat;
} hwmap_stream_info;
//-----------------------------------------------------------------------------
//  Enum
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Structure
//-----------------------------------------------------------------------------

/* ---------------------------------------------------------------------------------------------- */
/*	Function										  */
/* ---------------------------------------------------------------------------------------------- */
void drv_hwreg_common_config_get_ori_version(char *ver, int size);

void drv_hwreg_common_config_get_version(char *ver, int size);

uint8_t drv_hwreg_common_config_get_byte(void *pAddrVirt,uint32_t *offset);

uint16_t drv_hwreg_common_config_get_2byte(void *pAddrVirt, uint32_t *offset);

uint32_t drv_hwreg_common_config_get_4byte(void *pAddrVirt, uint32_t *offset);

uint16_t drv_hwreg_common_config_get_ip_num(MS_PQBin_Header_Info *pPQBinHeader);

uint16_t drv_hwreg_common_config_get_table_num(uint16_t ipIdx,
						MS_PQBin_Header_Info *pPQBinHeader);

uint16_t drv_hwreg_common_config_get_table_idx(uint16_t src_type,
						uint16_t ipIdx,
						uint16_t domain_idx,
						MS_PQBin_Header_Info *pPQBinHeader);

void drv_hwreg_common_config_get_table_info(uint16_t domainIdx,
						uint16_t tabIdx,
						uint16_t ipIdx,
						MS_PQBin_IP_Table_Info *pTableInfo);

uint16_t drv_hwreg_common_config_get_table_level(uint16_t src_type,
						uint16_t ipIdx,
						uint16_t domain_idx);

void drv_hwreg_common_config_get_table(uint16_t domainIdx,
					uint16_t tabIdx,
					uint16_t ipIdx,
					MS_PQBin_IP_Table_Info *pTableInfo,
					MS_PQBin_Header_Info *pPQBinHeader);

uint16_t drv_hwreg_common_config_get_skip(uint16_t ipIdx,
					MS_PQBin_Header_Info *pPQBinHeader);

uint32_t drv_hwreg_common_config_get_buffer_mode(uint32_t src_type);

void drv_hwreg_common_config_get_table_data(uint16_t domainIdx,
					uint16_t tabIdx,
					uint16_t ipIdx,
					uint8_t *pTable,
					uint16_t tablesize,
					MS_PQBin_Header_Info *pPQBinHeader);

void drv_hwreg_common_config_parsing(MS_PQBin_Header_Info *pPQBinHeader);

bool drv_hwreg_common_config_dump_reg_table(MS_PQBin_IP_Table_Info *pTabInfo,
					bool bRIU, struct hwregOut *hwOut);

void drv_hwreg_common_config_dump_table(MS_PQBin_IP_Table_Info *pTabInfo,
					bool bRIU, struct hwregOut *hwOut);

void drv_hwreg_common_config_load_common(uint16_t domain_idx,
					bool bRIU);

void drv_hwreg_common_config_load_tab_by_src(uint16_t src_type,
					uint16_t ip_idx,
					uint16_t domain_idx,
					bool bRIU,
					struct hwregOut *hwOut);

uint16_t drv_hwreg_common_config_mapping_ip_level(hwmap_ip_sw ipIdx, hwmap_ip_sw_bit swBit);

uint16_t drv_hwreg_common_config_mapping_ip(hwmap_ip_sw ipIdx);

void drv_hwreg_common_config_init(void *config_file);

int drv_hwreg_common_mapping_transition(uint16_t src_type);

#endif // _HWREG_COMMON_BIN_H_
