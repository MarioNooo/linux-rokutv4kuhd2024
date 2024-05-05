#define GOPG_BKA4D9 0x0149B200

//Page GOPG_BKA4D9_1
#define REG_0004_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x004)
    #define REG_0004_GOPG_BKA4D9_REG_HVSP_PATH_SEL Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0004_GOPG_BKA4D9_REG_HVSP_SWITCH Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0004_GOPG_BKA4D9_REG_GOP0_DEST_SEL Fld(2,4,AC_MSKB0)//[5:4]
    #define REG_0004_GOPG_BKA4D9_REG_GOPC_DEST_SEL Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_0004_GOPG_BKA4D9_REG_VG_SEP_PATH Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_0004_GOPG_BKA4D9_REG_ABF_BYPASS Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_0004_GOPG_BKA4D9_REG_VG_PSE_RST_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_0004_GOPG_BKA4D9_REG_VG_PSE_RST_NUM Fld(2,10,AC_MSKB1)//[11:10]
    #define REG_0004_GOPG_BKA4D9_REG_VG_DITH_SEL Fld(2,12,AC_MSKB1)//[13:12]
#define REG_0008_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x008)
    #define REG_0008_GOPG_BKA4D9_REG_GOP0_SW_RST Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0008_GOPG_BKA4D9_REG_AFBC0_SW_RST Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0008_GOPG_BKA4D9_REG_GOP1_SW_RST Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0008_GOPG_BKA4D9_REG_AFBC1_SW_RST Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0008_GOPG_BKA4D9_REG_AFBC_CORE0_SW_RST Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0008_GOPG_BKA4D9_REG_AHB_SW_RST Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_0008_GOPG_BKA4D9_REG_MIU_SW_RST Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_0008_GOPG_BKA4D9_REG_GOP2_SW_RST Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_0008_GOPG_BKA4D9_REG_AFBC2_SW_RST Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_0008_GOPG_BKA4D9_REG_GOP3_SW_RST Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_0008_GOPG_BKA4D9_REG_AFBC3_SW_RST Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_0008_GOPG_BKA4D9_REG_AFBC_CORE1_SW_RST Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_0008_GOPG_BKA4D9_REG_GOPC_SW_RST Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_0008_GOPG_BKA4D9_REG_TG_SW_RST Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_0008_GOPG_BKA4D9_REG_GOP_GLOBAL_SW_RST Fld(1,15,AC_MSKB1)//[15:15]
#define REG_000C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x00C)
    #define REG_000C_GOPG_BKA4D9_REG_HBLK_EXTEND_PERIOD Fld(5,0,AC_MSKB0)//[4:0]
#define REG_0010_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x010)
    #define REG_0010_GOPG_BKA4D9_REG_CHKSUM_FRM_CNT Fld(4,0,AC_MSKB0)//[3:0]
    #define REG_0010_GOPG_BKA4D9_REG_CHKSUM_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0010_GOPG_BKA4D9_REG_CHKSUM_RDY_SEL Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0010_GOPG_BKA4D9_REG_CHKSUM_CLR_TRIG Fld(1,8,AC_MSKB1)//[8:8]
#define REG_0014_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x014)
    #define REG_0014_GOPG_BKA4D9_REG_CHKSUM_RD_0 Fld(16,0,AC_FULLW10)//[15:0]
#define REG_0018_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x018)
    #define REG_0018_GOPG_BKA4D9_REG_CHKSUM_RD_1 Fld(16,0,AC_FULLW10)//[15:0]
#define REG_001C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x01C)
    #define REG_001C_GOPG_BKA4D9_REG_MIX_DMA_TRIG_SEL Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_001C_GOPG_BKA4D9_REG_SCIP_DMA_TRIG_SEL Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_001C_GOPG_BKA4D9_REG_AFBC_CORE0_VS_REF Fld(1,2,AC_MSKB0)//[2:2]
#define REG_0020_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x020)
    #define REG_0020_GOPG_BKA4D9_REG_GOP0_CFG_TRIG Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0020_GOPG_BKA4D9_REG_GOP1_CFG_TRIG Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0020_GOPG_BKA4D9_REG_GOP2_CFG_TRIG Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0020_GOPG_BKA4D9_REG_GOP3_CFG_TRIG Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0020_GOPG_BKA4D9_REG_GOPC_CFG_TRIG Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0020_GOPG_BKA4D9_REG_GOPG_CFG_TRIG Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0020_GOPG_BKA4D9_REG_GOP0_CFG_ACK Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_0020_GOPG_BKA4D9_REG_GOP1_CFG_ACK Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_0020_GOPG_BKA4D9_REG_GOP2_CFG_ACK Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_0020_GOPG_BKA4D9_REG_GOP3_CFG_ACK Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_0020_GOPG_BKA4D9_REG_GOPC_CFG_ACK Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_0020_GOPG_BKA4D9_REG_GOPG_CFG_ACK Fld(1,13,AC_MSKB1)//[13:13]
#define REG_0024_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x024)
    #define REG_0024_GOPG_BKA4D9_REG_GOP0_FORCE_WR Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0024_GOPG_BKA4D9_REG_GOP1_FORCE_WR Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0024_GOPG_BKA4D9_REG_GOP2_FORCE_WR Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0024_GOPG_BKA4D9_REG_GOP3_FORCE_WR Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0024_GOPG_BKA4D9_REG_GOPC_FORCE_WR Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0024_GOPG_BKA4D9_REG_GOPG_FORCE_WR Fld(1,5,AC_MSKB0)//[5:5]
#define REG_0028_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x028)
    #define REG_0028_GOPG_BKA4D9_REG_SCTC_INT_MASK Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0028_GOPG_BKA4D9_REG_SCIP_INT_MASK Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0028_GOPG_BKA4D9_REG_OCTG_INT_MASK Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0028_GOPG_BKA4D9_REG_CLK_GOP_INT_MASK Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0028_GOPG_BKA4D9_REG_CLK_AFBC_INT_MASK Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0028_GOPG_BKA4D9_REG_CLK_MOD_INT_MASK Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_0028_GOPG_BKA4D9_REG_GOP2MOD_UF_CLR Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_0028_GOPG_BKA4D9_REG_SCTC_INT Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_0028_GOPG_BKA4D9_REG_SCIP_INT Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_0028_GOPG_BKA4D9_REG_OCTG_INT Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_0028_GOPG_BKA4D9_REG_CLK_GOP_INT Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_0028_GOPG_BKA4D9_REG_CLK_AFBC_INT Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_0028_GOPG_BKA4D9_REG_CLK_MOD_INT Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_0028_GOPG_BKA4D9_REG_GOP2MOD_UF_FLAG Fld(1,15,AC_MSKB1)//[15:15]
#define REG_0030_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x030)
    #define REG_0030_GOPG_BKA4D9_REG_MIXER2_SRC0_USR_MD Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_0030_GOPG_BKA4D9_REG_MIXER2_SRC0_USR_HSIZE Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0034_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x034)
    #define REG_0034_GOPG_BKA4D9_REG_MIXER2_SRC0_USR_VSIZE Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0038_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x038)
    #define REG_0038_GOPG_BKA4D9_REG_MIXER2_SRC0_USR_HSTR Fld(13,0,AC_MSKW10)//[12:0]
#define REG_003C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x03C)
    #define REG_003C_GOPG_BKA4D9_REG_MIXER2_SRC0_USR_VSTR Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0040_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x040)
    #define REG_0040_GOPG_BKA4D9_REG_MIXER4_DISP_HSIZE Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0044_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x044)
    #define REG_0044_GOPG_BKA4D9_REG_MIXER4_DISP_VSIZE Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0048_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x048)
    #define REG_0048_GOPG_BKA4D9_REG_MIXER4_BYP_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0048_GOPG_BKA4D9_REG_MIXER4_OUT_GFX_ALPHA Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0048_GOPG_BKA4D9_REG_MIXER4_RMA_ALPHA_INV Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0048_GOPG_BKA4D9_REG_MIXER4_RMA_DIV_ADD1 Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0048_GOPG_BKA4D9_REG_MIXER4_RMA_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0048_GOPG_BKA4D9_REG_MIXER4_RMA_REM_FLAG Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_0048_GOPG_BKA4D9_REG_MIXER4_RMA_REM_FLAG_CLR Fld(1,7,AC_MSKB0)//[7:7]
#define REG_004C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x04C)
    #define REG_004C_GOPG_BKA4D9_REG_MIXER4_SRC0_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_004C_GOPG_BKA4D9_REG_MIXER4_SRC1_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_004C_GOPG_BKA4D9_REG_MIXER4_SRC2_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_004C_GOPG_BKA4D9_REG_MIXER4_SRC3_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_004C_GOPG_BKA4D9_REG_MIXER4_LAYER0_SRC_D Fld(2,8,AC_MSKB1)//[9:8]
    #define REG_004C_GOPG_BKA4D9_REG_MIXER4_LAYER1_SRC_D Fld(2,10,AC_MSKB1)//[11:10]
    #define REG_004C_GOPG_BKA4D9_REG_MIXER4_LAYER2_SRC_D Fld(2,12,AC_MSKB1)//[13:12]
    #define REG_004C_GOPG_BKA4D9_REG_MIXER4_LAYER3_SRC_D Fld(2,14,AC_MSKB1)//[15:14]
#define REG_0050_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x050)
    #define REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER0_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER1_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER2_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER3_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER0_SRC_A Fld(2,8,AC_MSKB1)//[9:8]
    #define REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER1_SRC_A Fld(2,10,AC_MSKB1)//[11:10]
    #define REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER2_SRC_A Fld(2,12,AC_MSKB1)//[13:12]
    #define REG_0050_GOPG_BKA4D9_REG_MIXER4_LAYER3_SRC_A Fld(2,14,AC_MSKB1)//[15:14]
#define REG_0054_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x054)
    #define REG_0054_GOPG_BKA4D9_REG_MIXER4_OSD_ROI_THD Fld(8,0,AC_FULLB0)//[7:0]
#define REG_0058_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x058)
    #define REG_0058_GOPG_BKA4D9_REG_MIXER4_OSD_ROI_RECORD_0 Fld(16,0,AC_FULLW10)//[15:0]
#define REG_005C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x05C)
    #define REG_005C_GOPG_BKA4D9_REG_MIXER4_OSD_ROI_RECORD_1 Fld(10,0,AC_MSKW10)//[9:0]
#define REG_0060_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x060)
    #define REG_0060_GOPG_BKA4D9_REG_MIXER2_DISP_HSIZE Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0064_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x064)
    #define REG_0064_GOPG_BKA4D9_REG_MIXER2_DISP_VSIZE Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0068_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x068)
    #define REG_0068_GOPG_BKA4D9_REG_MIXER2_BYP_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0068_GOPG_BKA4D9_REG_MIXER2_OUT_GFX_ALPHA Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0068_GOPG_BKA4D9_REG_MIXER2_RMA_ALPHA_INV Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0068_GOPG_BKA4D9_REG_MIXER2_RMA_DIV_ADD1 Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0068_GOPG_BKA4D9_REG_MIXER2_RMA_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0068_GOPG_BKA4D9_REG_MIXER2_RMA_REM_FLAG Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_0068_GOPG_BKA4D9_REG_MIXER2_RMA_REM_FLAG_CLR Fld(1,7,AC_MSKB0)//[7:7]
#define REG_006C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x06C)
    #define REG_006C_GOPG_BKA4D9_REG_MIXER2_SRC0_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_006C_GOPG_BKA4D9_REG_MIXER2_SRC1_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_006C_GOPG_BKA4D9_REG_MIXER2_LAYER0_SRC_D Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_006C_GOPG_BKA4D9_REG_MIXER2_LAYER1_SRC_D Fld(1,10,AC_MSKB1)//[10:10]
#define REG_0070_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x070)
    #define REG_0070_GOPG_BKA4D9_REG_MIXER2_LAYER0_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0070_GOPG_BKA4D9_REG_MIXER2_LAYER1_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0070_GOPG_BKA4D9_REG_MIXER2_LAYER0_SRC_A Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_0070_GOPG_BKA4D9_REG_MIXER2_LAYER1_SRC_A Fld(1,10,AC_MSKB1)//[10:10]
#define REG_0074_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x074)
    #define REG_0074_GOPG_BKA4D9_REG_MIXER2_OSD_ROI_THD Fld(8,0,AC_FULLB0)//[7:0]
#define REG_0078_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x078)
    #define REG_0078_GOPG_BKA4D9_REG_MIXER2_OSD_ROI_RECORD_0 Fld(16,0,AC_FULLW10)//[15:0]
#define REG_007C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x07C)
    #define REG_007C_GOPG_BKA4D9_REG_MIXER2_OSD_ROI_RECORD_1 Fld(10,0,AC_MSKW10)//[9:0]
#define REG_0080_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x080)
    #define REG_0080_GOPG_BKA4D9_REG_TG_HS_ST Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0084_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x084)
    #define REG_0084_GOPG_BKA4D9_REG_TG_HS_END Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0088_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x088)
    #define REG_0088_GOPG_BKA4D9_REG_TG_HFDE_ST Fld(13,0,AC_MSKW10)//[12:0]
#define REG_008C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x08C)
    #define REG_008C_GOPG_BKA4D9_REG_TG_HFDE_END Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0090_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x090)
    #define REG_0090_GOPG_BKA4D9_REG_TG_HTT Fld(13,0,AC_MSKW10)//[12:0]
#define REG_0094_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x094)
    #define REG_0094_GOPG_BKA4D9_REG_TG_VS_ST Fld(16,0,AC_FULLW10)//[15:0]
#define REG_0098_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x098)
    #define REG_0098_GOPG_BKA4D9_REG_TG_VS_END Fld(16,0,AC_FULLW10)//[15:0]
#define REG_009C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x09C)
    #define REG_009C_GOPG_BKA4D9_REG_TG_VFDE_ST Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00A0_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0A0)
    #define REG_00A0_GOPG_BKA4D9_REG_TG_VFDE_END Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00A4_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0A4)
    #define REG_00A4_GOPG_BKA4D9_REG_TG_VTT Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00A8_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0A8)
    #define REG_00A8_GOPG_BKA4D9_REG_TG_ML_ST Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00AC_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0AC)
    #define REG_00AC_GOPG_BKA4D9_REG_TG_ML_END Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00B0_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0B0)
    #define REG_00B0_GOPG_BKA4D9_REG_TG_OSDVS_ST Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00B4_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0B4)
    #define REG_00B4_GOPG_BKA4D9_REG_TG_OSDVS_END Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00B8_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0B8)
    #define REG_00B8_GOPG_BKA4D9_REG_TG_DMA_TRIG_ST Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00BC_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0BC)
    #define REG_00BC_GOPG_BKA4D9_REG_TG_DMA_TRIG_END Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00C0_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0C0)
    #define REG_00C0_GOPG_BKA4D9_REG_TG_VCNT_SEL Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_00C0_GOPG_BKA4D9_REG_TG_VG_SYNC_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_00C0_GOPG_BKA4D9_REG_TG_RST_MODE Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_00C0_GOPG_BKA4D9_REG_LR_FLAG_SRC_SEL Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_00C0_GOPG_BKA4D9_REG_LR_FLAG_FORCE Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_00C0_GOPG_BKA4D9_REG_LR_FLAG_VALUE Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_00C0_GOPG_BKA4D9_REG_LR_FLAG_CHG_SEL Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_00C0_GOPG_BKA4D9_REG_FIELD_FORCE Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_00C0_GOPG_BKA4D9_REG_FIELD_VALUE Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_00C0_GOPG_BKA4D9_REG_FIELD_CHG_SEL Fld(1,14,AC_MSKB1)//[14:14]
#define REG_00C4_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0C4)
    #define REG_00C4_GOPG_BKA4D9_REG_TG_RST_TRIG_DCNT Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00D4_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0D4)
    #define REG_00D4_GOPG_BKA4D9_REG_GG_HPEAK_UNDERSHOOT Fld(5,0,AC_MSKB0)//[4:0]
    #define REG_00D4_GOPG_BKA4D9_REG_GG_HPEAK_OVERSHOOT Fld(5,8,AC_MSKB1)//[12:8]
#define REG_00D8_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0D8)
    #define REG_00D8_GOPG_BKA4D9_REG_GG_HPEAK_PEAKING_GAIN Fld(6,0,AC_MSKB0)//[5:0]
    #define REG_00D8_GOPG_BKA4D9_REG_GG_HPEAK_EN Fld(1,7,AC_MSKB0)//[7:7]
#define REG_00DC_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0DC)
    #define REG_00DC_GOPG_BKA4D9_REG_FIX_R2Y_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_00DC_GOPG_BKA4D9_REG_FIX_R2Y_DITHER_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_00DC_GOPG_BKA4D9_REG_FIX_Y2R_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_00DC_GOPG_BKA4D9_REG_FIX_Y2R_DITHER_EN Fld(1,11,AC_MSKB1)//[11:11]
#define REG_00E0_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0E0)
    #define REG_00E0_GOPG_BKA4D9_REG_GG_HSP_RATIO_0 Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00E4_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0E4)
    #define REG_00E4_GOPG_BKA4D9_REG_GG_HSP_RATIO_1 Fld(12,0,AC_MSKW10)//[11:0]
    #define REG_00E4_GOPG_BKA4D9_REG_GG_HSP_EN Fld(1,15,AC_MSKB1)//[15:15]
#define REG_00E8_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0E8)
    #define REG_00E8_GOPG_BKA4D9_REG_GG_VSP_RATIO_0 Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00EC_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0EC)
    #define REG_00EC_GOPG_BKA4D9_REG_GG_VSP_RATIO_1 Fld(8,0,AC_FULLB0)//[7:0]
    #define REG_00EC_GOPG_BKA4D9_REG_GG_VSP_EN Fld(1,15,AC_MSKB1)//[15:15]
#define REG_00F0_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0F0)
    #define REG_00F0_GOPG_BKA4D9_REG_GG_HSP_INI_RATIO_0 Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00F4_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0F4)
    #define REG_00F4_GOPG_BKA4D9_REG_GG_HSP_INI_RATIO_1 Fld(12,0,AC_MSKW10)//[11:0]
    #define REG_00F4_GOPG_BKA4D9_REG_GG_HSP_INI_RATIO_M1 Fld(1,15,AC_MSKB1)//[15:15]
#define REG_00F8_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0F8)
    #define REG_00F8_GOPG_BKA4D9_REG_GG_VSP_INI_RATIO_0 Fld(16,0,AC_FULLW10)//[15:0]
#define REG_00FC_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x0FC)
    #define REG_00FC_GOPG_BKA4D9_REG_GG_VSP_INI_RATIO_1 Fld(8,0,AC_FULLB0)//[7:0]
    #define REG_00FC_GOPG_BKA4D9_REG_GG_VSP_INI_RATIO_M1 Fld(1,15,AC_MSKB1)//[15:15]
#define REG_0180_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x180)
    #define REG_0180_GOPG_BKA4D9_REG_G0_HVSP_CLK_DB_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0180_GOPG_BKA4D9_REG_G0_UDMA_FCLK_DB_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0180_GOPG_BKA4D9_REG_G0_UDMA_MCLK_DB_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0180_GOPG_BKA4D9_REG_AFBC_G0_CLK_DB_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0180_GOPG_BKA4D9_REG_GOP_G0_CLK_DB_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0180_GOPG_BKA4D9_REG_MIU_U0_CLK_DB_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0180_GOPG_BKA4D9_REG_AFBC_A0_CLK_DB_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_0180_GOPG_BKA4D9_REG_GOP0_DST_CLK_DB_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_0180_GOPG_BKA4D9_REG_GOP_G0_FCSC_CLK_DB_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_0180_GOPG_BKA4D9_REG_G0_DSC_CLK_DB_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_0180_GOPG_BKA4D9_REG_GOP_G0_DOLBY_CLK_DB_EN Fld(1,10,AC_MSKB1)//[10:10]
#define REG_0184_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x184)
    #define REG_0184_GOPG_BKA4D9_REG_G1_HVSP_CLK_DB_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0184_GOPG_BKA4D9_REG_G1_UDMA_FCLK_DB_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0184_GOPG_BKA4D9_REG_G1_UDMA_MCLK_DB_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0184_GOPG_BKA4D9_REG_AFBC_G1_CLK_DB_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0184_GOPG_BKA4D9_REG_GOP_G1_CLK_DB_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0184_GOPG_BKA4D9_REG_MIU_U1_CLK_DB_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0184_GOPG_BKA4D9_REG_AFBC_A1_CLK_DB_EN Fld(1,6,AC_MSKB0)//[6:6]
#define REG_0188_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x188)
    #define REG_0188_GOPG_BKA4D9_REG_G2_HVSP_CLK_DB_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0188_GOPG_BKA4D9_REG_G2_UDMA_FCLK_DB_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0188_GOPG_BKA4D9_REG_G2_UDMA_MCLK_DB_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0188_GOPG_BKA4D9_REG_AFBC_G2_CLK_DB_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0188_GOPG_BKA4D9_REG_GOP_G2_CLK_DB_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0188_GOPG_BKA4D9_REG_MIU_U2_CLK_DB_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0188_GOPG_BKA4D9_REG_AFBC_A2_CLK_DB_EN Fld(1,6,AC_MSKB0)//[6:6]
#define REG_018C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x18C)
    #define REG_018C_GOPG_BKA4D9_REG_G3_HVSP_CLK_DB_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_018C_GOPG_BKA4D9_REG_G3_UDMA_FCLK_DB_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_018C_GOPG_BKA4D9_REG_G3_UDMA_MCLK_DB_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_018C_GOPG_BKA4D9_REG_AFBC_G3_CLK_DB_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_018C_GOPG_BKA4D9_REG_GOP_G3_CLK_DB_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_018C_GOPG_BKA4D9_REG_MIU_U3_CLK_DB_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_018C_GOPG_BKA4D9_REG_AFBC_A3_CLK_DB_EN Fld(1,6,AC_MSKB0)//[6:6]
#define REG_0190_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x190)
    #define REG_0190_GOPG_BKA4D9_REG_GC_UDMA_FCLK_DB_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0190_GOPG_BKA4D9_REG_GC_UDMA_MCLK_DB_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0190_GOPG_BKA4D9_REG_AFBC_GC_CLK_DB_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0190_GOPG_BKA4D9_REG_MIU_UC_CLK_DB_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0190_GOPG_BKA4D9_REG_GOPC_DST_CLK_DB_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_0190_GOPG_BKA4D9_REG_GG_HVSP_CLK_DB_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_0190_GOPG_BKA4D9_REG_GOP_GG_CLK_DB_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_0190_GOPG_BKA4D9_REG_GOPG_DST_CLK_DB_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_0190_GOPG_BKA4D9_REG_AFBC_CORE0_CLK_DB_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_0190_GOPG_BKA4D9_REG_AFBC_CORE1_CLK_DB_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_0190_GOPG_BKA4D9_REG_ABF_CLK_DB_EN_LBUF Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_0190_GOPG_BKA4D9_REG_ABF_CLK_DB_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_0190_GOPG_BKA4D9_REG_MOD_CLK_DB_EN Fld(1,15,AC_MSKB1)//[15:15]
#define REG_0194_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x194)
    #define REG_0194_GOPG_BKA4D9_REG_G0_HVSP_CLK_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0194_GOPG_BKA4D9_REG_G0_UDMA_FCLK_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0194_GOPG_BKA4D9_REG_G0_UDMA_MCLK_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0194_GOPG_BKA4D9_REG_AFBC_G0_CLK_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0194_GOPG_BKA4D9_REG_GOP_G0_CLK_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0194_GOPG_BKA4D9_REG_MIU_U0_CLK_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0194_GOPG_BKA4D9_REG_AFBC_A0_CLK_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_0194_GOPG_BKA4D9_REG_GOP0_DST_CLK_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_0194_GOPG_BKA4D9_REG_GOP_G0_FCSC_CLK_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_0194_GOPG_BKA4D9_REG_G0_DSC_CLK_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_0194_GOPG_BKA4D9_REG_GOP_G0_DOLBY_CLK_EN Fld(1,10,AC_MSKB1)//[10:10]
#define REG_0198_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x198)
    #define REG_0198_GOPG_BKA4D9_REG_G1_HVSP_CLK_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_0198_GOPG_BKA4D9_REG_G1_UDMA_FCLK_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_0198_GOPG_BKA4D9_REG_G1_UDMA_MCLK_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_0198_GOPG_BKA4D9_REG_AFBC_G1_CLK_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_0198_GOPG_BKA4D9_REG_GOP_G1_CLK_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_0198_GOPG_BKA4D9_REG_MIU_U1_CLK_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_0198_GOPG_BKA4D9_REG_AFBC_A1_CLK_EN Fld(1,6,AC_MSKB0)//[6:6]
#define REG_019C_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x19C)
    #define REG_019C_GOPG_BKA4D9_REG_G2_HVSP_CLK_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_019C_GOPG_BKA4D9_REG_G2_UDMA_FCLK_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_019C_GOPG_BKA4D9_REG_G2_UDMA_MCLK_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_019C_GOPG_BKA4D9_REG_AFBC_G2_CLK_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_019C_GOPG_BKA4D9_REG_GOP_G2_CLK_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_019C_GOPG_BKA4D9_REG_MIU_U2_CLK_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_019C_GOPG_BKA4D9_REG_AFBC_A2_CLK_EN Fld(1,6,AC_MSKB0)//[6:6]
#define REG_01A0_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1A0)
    #define REG_01A0_GOPG_BKA4D9_REG_G3_HVSP_CLK_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_01A0_GOPG_BKA4D9_REG_G3_UDMA_FCLK_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_01A0_GOPG_BKA4D9_REG_G3_UDMA_MCLK_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_01A0_GOPG_BKA4D9_REG_AFBC_G3_CLK_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_01A0_GOPG_BKA4D9_REG_GOP_G3_CLK_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_01A0_GOPG_BKA4D9_REG_MIU_U3_CLK_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_01A0_GOPG_BKA4D9_REG_AFBC_A3_CLK_EN Fld(1,6,AC_MSKB0)//[6:6]
#define REG_01A4_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1A4)
    #define REG_01A4_GOPG_BKA4D9_REG_GC_UDMA_FCLK_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_01A4_GOPG_BKA4D9_REG_GC_UDMA_MCLK_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_01A4_GOPG_BKA4D9_REG_AFBC_GC_CLK_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_01A4_GOPG_BKA4D9_REG_MIU_UC_CLK_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_01A4_GOPG_BKA4D9_REG_GOPC_DST_CLK_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_01A4_GOPG_BKA4D9_REG_GG_HVSP_CLK_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_01A4_GOPG_BKA4D9_REG_GOP_GG_CLK_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_01A4_GOPG_BKA4D9_REG_GOPG_DST_CLK_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_01A4_GOPG_BKA4D9_REG_AFBC_CORE0_CLK_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_01A4_GOPG_BKA4D9_REG_AFBC_CORE1_CLK_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_01A4_GOPG_BKA4D9_REG_ABF_CLK_EN_LBUF Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_01A4_GOPG_BKA4D9_REG_ABF_CLK_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_01A4_GOPG_BKA4D9_REG_MOD_CLK_EN Fld(1,15,AC_MSKB1)//[15:15]

//Page GOPG_BKA4D9_2
#define REG_01A8_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1A8)
    #define REG_01A8_GOPG_BKA4D9_REG_G0_HVSP_CLK_SW_MD_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_01A8_GOPG_BKA4D9_REG_G0_UDMA_FCLK_SW_MD_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_01A8_GOPG_BKA4D9_REG_G0_UDMA_MCLK_SW_MD_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_01A8_GOPG_BKA4D9_REG_AFBC_G0_CLK_SW_MD_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_01A8_GOPG_BKA4D9_REG_GOP_G0_CLK_SW_MD_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_01A8_GOPG_BKA4D9_REG_MIU_U0_CLK_SW_MD_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_01A8_GOPG_BKA4D9_REG_AFBC_A0_CLK_SW_MD_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_01A8_GOPG_BKA4D9_REG_GOP0_DST_CLK_SW_MD_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_01A8_GOPG_BKA4D9_REG_GOP_G0_FCSC_CLK_SW_MD_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_01A8_GOPG_BKA4D9_REG_G0_DSC_CLK_SW_MD_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_01A8_GOPG_BKA4D9_REG_GOP_G0_DOLBY_CLK_SW_MD_EN Fld(1,10,AC_MSKB1)//[10:10]
#define REG_01AC_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1AC)
    #define REG_01AC_GOPG_BKA4D9_REG_G1_HVSP_CLK_SW_MD_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_01AC_GOPG_BKA4D9_REG_G1_UDMA_FCLK_SW_MD_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_01AC_GOPG_BKA4D9_REG_G1_UDMA_MCLK_SW_MD_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_01AC_GOPG_BKA4D9_REG_AFBC_G1_CLK_SW_MD_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_01AC_GOPG_BKA4D9_REG_GOP_G1_CLK_SW_MD_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_01AC_GOPG_BKA4D9_REG_MIU_U1_CLK_SW_MD_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_01AC_GOPG_BKA4D9_REG_AFBC_A1_CLK_SW_MD_EN Fld(1,6,AC_MSKB0)//[6:6]
#define REG_01B0_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1B0)
    #define REG_01B0_GOPG_BKA4D9_REG_G2_HVSP_CLK_SW_MD_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_01B0_GOPG_BKA4D9_REG_G2_UDMA_FCLK_SW_MD_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_01B0_GOPG_BKA4D9_REG_G2_UDMA_MCLK_SW_MD_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_01B0_GOPG_BKA4D9_REG_AFBC_G2_CLK_SW_MD_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_01B0_GOPG_BKA4D9_REG_GOP_G2_CLK_SW_MD_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_01B0_GOPG_BKA4D9_REG_MIU_U2_CLK_SW_MD_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_01B0_GOPG_BKA4D9_REG_AFBC_A2_CLK_SW_MD_EN Fld(1,6,AC_MSKB0)//[6:6]
#define REG_01B4_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1B4)
    #define REG_01B4_GOPG_BKA4D9_REG_G3_HVSP_CLK_SW_MD_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_01B4_GOPG_BKA4D9_REG_G3_UDMA_FCLK_SW_MD_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_01B4_GOPG_BKA4D9_REG_G3_UDMA_MCLK_SW_MD_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_01B4_GOPG_BKA4D9_REG_AFBC_G3_CLK_SW_MD_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_01B4_GOPG_BKA4D9_REG_GOP_G3_CLK_SW_MD_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_01B4_GOPG_BKA4D9_REG_MIU_U3_CLK_SW_MD_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_01B4_GOPG_BKA4D9_REG_AFBC_A3_CLK_SW_MD_EN Fld(1,6,AC_MSKB0)//[6:6]
#define REG_01B8_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1B8)
    #define REG_01B8_GOPG_BKA4D9_REG_GC_UDMA_FCLK_SW_MD_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_01B8_GOPG_BKA4D9_REG_GC_UDMA_MCLK_SW_MD_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_01B8_GOPG_BKA4D9_REG_AFBC_GC_CLK_SW_MD_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_01B8_GOPG_BKA4D9_REG_MIU_UC_CLK_SW_MD_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_01B8_GOPG_BKA4D9_REG_GOPC_DST_CLK_SW_MD_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_01B8_GOPG_BKA4D9_REG_GG_HVSP_CLK_SW_MD_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_01B8_GOPG_BKA4D9_REG_GOP_GG_CLK_SW_MD_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_01B8_GOPG_BKA4D9_REG_GOPG_DST_CLK_SW_MD_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_01B8_GOPG_BKA4D9_REG_AFBC_CORE0_CLK_SW_MD_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_01B8_GOPG_BKA4D9_REG_AFBC_CORE1_CLK_SW_MD_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_01B8_GOPG_BKA4D9_REG_ABF_CLK_SW_MD_EN_LBUF Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_01B8_GOPG_BKA4D9_REG_ABF_CLK_SW_MD_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_01B8_GOPG_BKA4D9_REG_MOD_CLK_SW_MD_EN Fld(1,15,AC_MSKB1)//[15:15]
#define REG_01BC_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1BC)
    #define REG_01BC_GOPG_BKA4D9_REG_ALL_AUTO_CLK_EN Fld(1,0,AC_MSKB0)//[0:0]
#define REG_01E0_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1E0)
    #define REG_01E0_GOPG_BKA4D9_REG_OSDB_LAYER_SEL Fld(3,0,AC_MSKB0)//[2:0]
    #define REG_01E0_GOPG_BKA4D9_REG_OSDB_METHOD_SEL Fld(2,4,AC_MSKB0)//[5:4]
    #define REG_01E0_GOPG_BKA4D9_REG_OSDB_VIDEO_DE_SEL Fld(2,6,AC_MSKB0)//[7:6]
    #define REG_01E0_GOPG_BKA4D9_REG_OSDB_CAP_STAGE Fld(2,8,AC_MSKB1)//[9:8]
    #define REG_01E0_GOPG_BKA4D9_REG_OSDB_CAP_DE_SEL Fld(2,12,AC_MSKB1)//[13:12]
    #define REG_01E0_GOPG_BKA4D9_REG_OSDB_CAP_EN Fld(1,15,AC_MSKB1)//[15:15]
#define REG_01E4_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1E4)
    #define REG_01E4_GOPG_BKA4D9_REG_OSDB_OSD_ROI_THD Fld(8,0,AC_FULLB0)//[7:0]
    #define REG_01E4_GOPG_BKA4D9_REG_OSDB_VIDEO_ALPHA Fld(8,8,AC_FULLB1)//[15:8]
#define REG_01E8_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1E8)
    #define REG_01E8_GOPG_BKA4D9_REG_OSDB_OSD_ROI_RECORD_0 Fld(16,0,AC_FULLW10)//[15:0]
#define REG_01EC_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1EC)
    #define REG_01EC_GOPG_BKA4D9_REG_OSDB_OSD_ROI_RECORD_1 Fld(10,0,AC_MSKW10)//[9:0]
#define REG_01F0_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1F0)
    #define REG_01F0_GOPG_BKA4D9_REG_OSDB0_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_01F0_GOPG_BKA4D9_REG_OSDB0_MODE1_ALPHA_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_01F0_GOPG_BKA4D9_REG_OSDB0_MODE1_PX_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_01F0_GOPG_BKA4D9_REG_OSDB0_MODE1_SUM_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_01F0_GOPG_BKA4D9_REG_OSDB0_ALPHA_INV Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_01F0_GOPG_BKA4D9_REG_OSDB1_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_01F0_GOPG_BKA4D9_REG_OSDB1_MODE1_ALPHA_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_01F0_GOPG_BKA4D9_REG_OSDB1_MODE1_PX_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_01F0_GOPG_BKA4D9_REG_OSDB1_MODE1_SUM_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_01F0_GOPG_BKA4D9_REG_OSDB1_ALPHA_INV Fld(1,12,AC_MSKB1)//[12:12]
#define REG_01FC_GOPG_BKA4D9 (GOPG_BKA4D9 + 0x1FC)
    #define REG_01FC_GOPG_BKA4D9_REG_OSDB_SRC0_UF_CLR Fld(1,0,AC_MSKB0)//[0:0]
    #define REG_01FC_GOPG_BKA4D9_REG_OSDB_SRC1_UF_CLR Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_01FC_GOPG_BKA4D9_REG_OSDB_SRC0_UF_FLAG Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_01FC_GOPG_BKA4D9_REG_OSDB_SRC1_UF_FLAG Fld(1,9,AC_MSKB1)//[9:9]

