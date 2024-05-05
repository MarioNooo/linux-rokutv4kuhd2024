/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef _MTK_IPAUTH_H_
#define _MTK_IPAUTH_H_


#ifdef __cplusplus
 extern "C" {
#endif

//=============================================================================================

typedef enum
{
    /* New EFUSE ENUM */
    E_EFUSE_IDX_0 = 0x3FF,
    E_EFUSE_IDX_1 = 0x400,
    E_EFUSE_IDX_2 = 0x401,
    E_EFUSE_IDX_3 = 0x402,
    E_EFUSE_IDX_4 = 0x403,
    E_EFUSE_IDX_5 = 0x404, // DVB
    E_EFUSE_IDX_6 = 0x405, // DRA
    E_EFUSE_IDX_7 = 0x406,
    E_EFUSE_IDX_8 = 0x407,
    E_EFUSE_IDX_9 = 0x408,
    E_EFUSE_IDX_10 = 0x409,
    E_EFUSE_IDX_11 = 0x40A,
    E_EFUSE_IDX_12 = 0x40B,
    E_EFUSE_IDX_13 = 0x40C,
    E_EFUSE_IDX_14 = 0x40D,
    E_EFUSE_IDX_15 = 0x40E,
    E_EFUSE_IDX_16 = 0x40F,
    E_EFUSE_IDX_17 = 0x410,
    E_EFUSE_IDX_18 = 0x411,
    E_EFUSE_IDX_19 = 0x412,
    E_EFUSE_IDX_20 = 0x413,
    E_EFUSE_IDX_21 = 0x414,
    E_EFUSE_IDX_22 = 0x415, // E_DRV_EFUSE_ID_MSS_IIS
    E_EFUSE_IDX_23 = 0x416,
    E_EFUSE_IDX_24 = 0x417,
    E_EFUSE_IDX_25 = 0x418,
    E_EFUSE_IDX_26 = 0x419, // E_DRV_EFUSE_ID_ANDROID
    E_EFUSE_IDX_27 = 0x41A,
    E_EFUSE_IDX_28 = 0x41B,
    E_EFUSE_IDX_29 = 0x41C,
    E_EFUSE_IDX_30 = 0x41D,
    E_EFUSE_IDX_31 = 0x41E,
    E_EFUSE_IDX_32 = 0x41F,
    E_EFUSE_IDX_33 = 0x420,
    E_EFUSE_IDX_34 = 0x421,
    E_EFUSE_IDX_35 = 0x422,
    E_EFUSE_IDX_36 = 0x423,
    E_EFUSE_IDX_37 = 0x424,
    E_EFUSE_IDX_38 = 0x425,
    E_EFUSE_IDX_39 = 0x426,
    E_EFUSE_IDX_40 = 0x427,
    E_EFUSE_IDX_41 = 0x428,
    E_EFUSE_IDX_42 = 0x429, // VB1
    E_EFUSE_IDX_43 = 0x42A,
    E_EFUSE_IDX_44 = 0x42B, // AIPQ(GPU)
    E_EFUSE_IDX_45 = 0x42C,
    E_EFUSE_IDX_46 = 0x42D,
    E_EFUSE_IDX_47 = 0x42E,
    E_EFUSE_IDX_48 = 0x42F, // E_DRV_EFUSE_ID_MTK_LOCAL_DIMMING
    E_EFUSE_IDX_49 = 0x430, // AIAQ-Secene Detection
    E_EFUSE_IDX_50 = 0x431,
    E_EFUSE_IDX_51 = 0x432,
    E_EFUSE_IDX_52 = 0x433,
    E_EFUSE_IDX_53 = 0x434,
    E_EFUSE_IDX_54 = 0x435,
    E_EFUSE_IDX_55 = 0x436,
    E_EFUSE_IDX_56 = 0x437,
    E_EFUSE_IDX_57 = 0x438,
    E_EFUSE_IDX_58 = 0x439,
    E_EFUSE_IDX_59 = 0x43A,
    E_EFUSE_IDX_60 = 0x43B,
    E_EFUSE_IDX_61 = 0x43C, // DTS
    E_EFUSE_IDX_62 = 0x43D, // DTS X
    E_EFUSE_IDX_63 = 0x43E, // Dolby Revert
    E_EFUSE_IDX_64 = 0x43F, // DTS Revert
    E_EFUSE_IDX_65 = 0x440, // MEMC_60
    E_EFUSE_IDX_66 = 0x441, // MEMC_120
    E_EFUSE_IDX_67 = 0x442,
    E_EFUSE_IDX_68 = 0x443,
    E_EFUSE_IDX_69 = 0x444,
    E_EFUSE_IDX_70 = 0x445, // WMA
    E_EFUSE_IDX_71 = 0x446, // E_DRV_EFUSE_ID_ANDROID_ANTV
    E_EFUSE_IDX_72 = 0x447, // E_DRV_EFUSE_ID_ANDROID_AOSP
    E_EFUSE_IDX_73 = 0x448,
    E_EFUSE_IDX_74 = 0x449,
    E_EFUSE_IDX_75 = 0x44A,
    E_EFUSE_IDX_76 = 0x44B, // AISR
    E_EFUSE_IDX_77 = 0x44C,
    E_EFUSE_IDX_78 = 0x44D, // LVDS
    E_EFUSE_IDX_79 = 0x44E,
    E_EFUSE_IDX_80 = 0x44F, // MIPI
    E_EFUSE_IDX_81 = 0x450,
    E_EFUSE_IDX_82 = 0x451,
    E_EFUSE_IDX_83 = 0x452,
    E_EFUSE_IDX_84 = 0x453, // DPTZ
    E_EFUSE_IDX_85 = 0x454,
    E_EFUSE_IDX_86 = 0x455,
    E_EFUSE_IDX_87 = 0x456,
    E_EFUSE_IDX_88 = 0x457,
    E_EFUSE_IDX_89 = 0x458,
    E_EFUSE_IDX_90 = 0x459, // MEMC mode
    E_EFUSE_IDX_91 = 0x45A, // DolbyVision(HDR) GFX
    E_EFUSE_IDX_92 = 0x45B, // CM4 SRAM
    E_EFUSE_IDX_93 = 0x45C, // E_DRV_EFUSE_ID_AVS3
    E_EFUSE_IDX_94 = 0x45D,
    E_EFUSE_IDX_95 = 0x45E, // E_DRV_EFUSE_ID_H266
    E_EFUSE_IDX_96 = 0x45F,
    E_EFUSE_IDX_97 = 0x460,
    E_EFUSE_IDX_98 = 0x461, // MTK Orbit(OLED)
    E_EFUSE_IDX_99 = 0x462,
    E_EFUSE_IDX_100 = 0x463, // E_DRV_EFUSE_ID_REGIONAL_PQ
    E_EFUSE_IDX_101 = 0x464, // Natt MC1 core
    E_EFUSE_IDX_102 = 0x465, // Natt MC2 core
    E_EFUSE_IDX_103 = 0x466, // Cortex-A76 core - 1
    E_EFUSE_IDX_104 = 0x467, // Cortex-A76 core - 2
    E_EFUSE_IDX_105 = 0x468, // Cortex-A76 core - 3
    E_EFUSE_IDX_106 = 0x469,
    E_EFUSE_IDX_107 = 0x46A, // E_DRV_EFUSE_ID_CONF_DISABLE_SO_NY_LOCAL_DIMING
    E_EFUSE_IDX_108 = 0x46B,
    E_EFUSE_IDX_109 = 0x46C, // VRR_144
    E_EFUSE_IDX_110 = 0x46D, // Disable SO_NY_OLED
    E_EFUSE_IDX_111 = 0x46E, // E_DRV_EFUSE_ID_LIVE_TONE
    E_EFUSE_IDX_112 = 0x46F, // iView
    E_EFUSE_IDX_113 = 0x470, // PIP or iView
    E_EFUSE_IDX_114 = 0x471, // Negra TVKey cloud
    E_EFUSE_IDX_115 = 0x472, // AI PQ Customize
    E_EFUSE_IDX_116 = 0x473, // AI PQ Depth
    E_EFUSE_IDX_117 = 0x474, // Voice Separation

    /* New EFUSE PLEASE ADD AFTER HERE */
    /* New EFUSE PLEASE ADD BEFORE HERE */
    E_EFUSE_IDX_MAX
} E_EFUSE_IDX;

//=================================================================================================

#define K_EFUSE_R_OK                        0
#define K_EFUSE_R_NG                        -1
#define K_EFUSE_R_ERR_INVALID_PARA          -2
#define K_EFUSE_R_ERR_INIT_NOT_DONE         -3
#define K_EFUSE_R_ERR_EFUSE_ID_NOT_SUPPORT  -4
#define K_EFUSE_R_ERR_WAIT_HW_TIMEOUT       -5
#define K_EFUSE_R_ERR_READ_SUBBANK_FAIL     -6



#define K_EFUSE_R_ERR_HASHKEY_NOT_READY     -10
#define K_EFUSE_R_ERR_HASHKEY_ID_INVALID    -11
#define K_EFUSE_R_ERR_VHASHKEY_NOT_SUPPORT  -12

#define K_EFUSE_R_ERR_IO_CMD_NOT_SUPPORT    -30
//=================================================================================================

#ifndef INTERFACE
    #define INTERFACE
#endif

//=================================================================================================

//----------------------------------------------------------------------
// Desc of mtk_efuse_query:
// input: id should enum => E_EFUSE_IDX
// return:
// if HW only one bit,
//    return 1 => support, return 0 => not support
// else
//    directly return HW value
INTERFACE int mtk_efuse_query(unsigned long int id);
//-----------------------------------------------------------------------

//----------------------------------------------------------------------
// Desc of mtk_efuse_check_IP:
// input:
//  id:  should enum => E_EFUSE_IDX
//  p32Value: the value of HW when function return OK.
//  if HW only one bit,
//      return 1 => support, return 0 => not support
//  else
//      directly return HW value
//
// return: the result of function execute
//
int mtk_efuse_check_IP(unsigned long int id, unsigned int * p32Value);
//-----------------------------------------------------------------------


INTERFACE int mtk_efuse_get_did_pci(unsigned char *val);
INTERFACE int mtk_efuse_get_did_dsc(unsigned char *val);

//------------------------------------------------------------------------------------------------
// Desc: Get chip device id
// input: U16 array[6]
// output: 0 -> OK; others -> NG
int mtk_efuse_DID_Get_DeviceId(unsigned short* pu16_device_id, unsigned char u8BufSize);
//------------------------------------------------------------------------------------------------

//===================================================================================================
void mtk_ipauth_set_debug_flag(unsigned int u32DebugFlag);

//===============================================================================================================
// Hashkey
//=========================================================================================================

//-------------------------------------------------------------------------------------------
// Desc: Get hashkey support bit by IP
// In: u8IP: 0 ~ 127
// return:
//        0 -> not support
//        1 -> support
unsigned char mtk_hashkey_check_IP(unsigned char u8IP);
//#define MTK_HASHKEY_GET_BIT(bit_n)  (mtk_hashkey_check_IP(bit_n))

// ------------------------------------------------------------------------------------------
// Desc: For check IP support or not from hashkey.
// In: u8IP: 0 ~ 127
// out: pu8 result:
//        0 -> not support
//        1 -> support
//
// return: function OK/NG
int mtk_hashkey_check_IP_2(unsigned char u8IP, unsigned char * pu8Result);

//========================================================================================================================
// For read efuse subbank
int mtk_efuse_read_subbank_reg_U32( unsigned char u8SubBank, unsigned int * pu32RegVal);

//========================================================================================================================

#ifdef __cplusplus
}
#endif

#endif //_MTK_IPAUTH_H_

