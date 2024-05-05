#ifndef HWREG_SRCCAP_HDMIRX_PKT_HDR_H
#define HWREG_SRCCAP_HDMIRX_PKT_HDR_H

enum e_pkt_type_header{
    HDMI_PKT_HEADER_NULL = 0,
    HDMI_PKT_HEADER_ACR = 1,
    HDMI_PKT_HEADER_AUDIO = 2,
    HDMI_PKT_HEADER_GCP = 3,
    HDMI_PKT_HEADER_ACP = 4,
    HDMI_PKT_HEADER_ISRC1 = 5,
    HDMI_PKT_HEADER_ISRC2 = 6,
    HDMI_PKT_HEADER_ONE_BIT_AUDIO = 7,
    HDMI_PKT_HEADER_DST = 8,
    HDMI_PKT_HEADER_HBR = 9,
    HDMI_PKT_HEADER_GMP = 0xA,
    HDMI_PKT_HEADER_AMP = 0xD,
    HDMI_PKT_HEADER_VSIF = 0x81,
    HDMI_PKT_HEADER_AVI = 0x82,
    HDMI_PKT_HEADER_SPD = 0x83,
    HDMI_PKT_HEADER_AUDIO_IF = 0x84,
    HDMI_PKT_HEADER_MPEG_IF = 0x85,
    HDMI_PKT_HEADER_VBI = 0x86,
    HDMI_PKT_HEADER_HDR = 0x87,
    HDMI_PKT_HEADER_EMP = 0x7F,
    HDMI_PKT_HEADER_RSV = 0xFF,
    HDMI_PKT_HEADER_N,
};

enum e_pkt_type_indx{
    HDMI_PKT_INDEX_ACR = 0,
    HDMI_PKT_INDEX_AUDIO,
    HDMI_PKT_INDEX_GCP,
    HDMI_PKT_INDEX_ACP,
    HDMI_PKT_INDEX_ISRC1,
    HDMI_PKT_INDEX_ISRC2, // 5
    HDMI_PKT_INDEX_ONE_BIT_AUDIO,
    HDMI_PKT_INDEX_DST,
    HDMI_PKT_INDEX_HBR,
    HDMI_PKT_INDEX_GMP,
    HDMI_PKT_INDEX_AMP, // 10
    HDMI_PKT_INDEX_VSIF,
    HDMI_PKT_INDEX_AVI,
    HDMI_PKT_INDEX_SPD,
    HDMI_PKT_INDEX_AUDIO_IF,
    HDMI_PKT_INDEX_MPEG_IF, // 15
    HDMI_PKT_INDEX_VBI,
    HDMI_PKT_INDEX_HDR,
    HDMI_PKT_INDEX_EMP_VENDOR, // 18
    HDMI_PKT_INDEX_EMP_VTEM,
    HDMI_PKT_INDEX_EMP_DSC, //20
    HDMI_PKT_INDEX_EMP_HDR,
    HDMI_PKT_INDEX_RSV,
    HDMI_PKT_INDEX_N,
};

enum E_HDMI_NEXT_TFR {
	E_HDMI_TFR_0 = 0,
	E_HDMI_TFR_23_976,
	E_HDMI_TFR_24,
	E_HDMI_TFR_25,
	E_HDMI_TFR_29_97,
	E_HDMI_TFR_30,
	E_HDMI_TFR_47_95,
	E_HDMI_TFR_48,
	E_HDMI_TFR_50,
	E_HDMI_TFR_59_94,
	E_HDMI_TFR_60 = 10,
	E_HDMI_TFR_100,
	E_HDMI_TFR_119_88,
	E_HDMI_TFR_120 = 13,
	E_HDMI_TFR_RSV
};


struct __attribute__((__packed__))
pkt_prototype_header {
    MS_U8 hb0;       // packet type
    MS_U16 hb2_1;
};

struct __attribute__((__packed__))
pkt_prototype_payload {
    MS_U16 pb1_0;
    MS_U16 pb3_2;
    MS_U16 pb5_4;
    MS_U16 pb7_6;
    MS_U16 pb9_8;
    MS_U16 pb11_10;
    MS_U16 pb13_12;
    MS_U16 pb15_14;
    MS_U16 pb17_16;
    MS_U16 pb19_18;
    MS_U16 pb21_20;
    MS_U16 pb23_22;
    MS_U16 pb25_24;
    MS_U16 pb27_26;
};


struct __attribute__((__packed__))
pkt_prototype_header_if {
    MS_U8 hb0;       // packet type
    MS_U8 version;
    MS_U8 length : 5;
    MS_U8 : 3;
};


struct __attribute__((__packed__))
pkt_prototype_payload_avi_if {
    MS_U8 checksum;
    MS_U8 S1_0:2;
    MS_U8 B1_0:2;
    MS_U8 A0:1;
    MS_U8 Y2_0:3; // Y2 for version 3 above

    MS_U8 R3_0:4;
    MS_U8 M1_0:2;
    MS_U8 C1_0:2;

    MS_U8 SC1_0:2;
    MS_U8 Q1_0:2;
    MS_U8 EC2_0:3;
    MS_U8 ITC:1;

    MS_U8 VIC;  // vic[7] for version 3 above
    MS_U8 PR:4;
    MS_U8 CN:2;
    MS_U8 YQ:2;
    MS_U16 ln_e_top_bar;
    MS_U16 ln_s_btm_bar;
    MS_U16 pn_e_left_bar;
    MS_U16 pn_s_right_bar;
    MS_U8 F143_0:4;  // version 4 above
    MS_U8 ACE3_0:4;  // version 4 above
};


struct __attribute__((__packed__))
pkt_prototype_payload_audio_if {
    MS_U8 checksum;
    MS_U8 CC2_0:3;
    MS_U8 : 1;
    MS_U8 CT3_0:4;

    MS_U8 SS1_0 : 2;
    MS_U8 SF2_0 : 3;
    MS_U8 : 3;

    MS_U8 pb3;

    MS_U8 CA7_0;

    MS_U8 LFEP1_0:2;
    MS_U8 : 1;
    MS_U8 LSV3_0:3;
    MS_U8 DM_INH:1;
};

struct __attribute__((__packed__))
pkt_prototype_payload_vendor_if {
    MS_U8 checksum;
    union{
        struct{
            MS_U8 ieee_lsb;
            MS_U8 ieee_b;
            MS_U8 ieee_msb;
            MS_U8 : 5;
            MS_U8 hdmi_video_format : 3;
        };
        struct{
            MS_U32 u32_ieee : 24;
            MS_U32 : 5;
            MS_U32 u32_hdmi_video_format : 3;
        };
    };
    union{
        MS_U8 hdmi_vic;
        struct{
            MS_U8 : 3;
            MS_U8 d3_meta_present : 1;
            MS_U8 d3_struct : 4;
        };
    };
    MS_U8 : 4;
    MS_U8 d3_extra_data : 4;
    MS_U8 d3_metadata_len : 4;
    MS_U8 d3_metadata_type : 4;
    MS_U8 d3_metadata[20];   //  if metadatay_type = 0, len = 8
};

struct __attribute__((__packed__))
pkt_prototype_payload_vendor_if_hf {
    MS_U8 checksum;
    union{
        struct{
            MS_U8 ieee_lsb;
            MS_U8 ieee_b;
            MS_U8 ieee_msb;
            MS_U8 version;
        };
        struct{
            MS_U32 u32_ieee : 24;
            MS_U32 u32_version : 8;
        };
    };
    MS_U8 d3_valid : 1;
    MS_U8 allm_mode : 1;
    MS_U8 : 2;
    MS_U8 ccbpc : 4;

    MS_U8 : 1;
    MS_U8 d3_meta_present : 1;
    MS_U8 d3_disparity_data_present : 1;
    MS_U8 d3_additional_info_present : 1;
    MS_U8 d3_f_structure : 4;

    MS_U8 : 4;
    MS_U8 d3_f_ext_data : 4;

    MS_U8 d3_preferred2dview : 2;
    MS_U8 d3_viewdependency : 2;
    MS_U8 d3_dualview : 1;
    MS_U8 : 3;

    MS_U8 d3_disparitydata_len : 5;
    MS_U8 d3_disparitydata_version : 3;
    MS_U8 d3_data[18];
    /*
    MS_U8 d3_disparitydata[];

    MS_U8 d3_metadata_len : 5;
    MS_U8 d3_metadata_type : 3;
    MS_U8 d3_metadata[];*/
};


struct __attribute__((__packed__))
pkt_prototype_payload_spd_if {
    MS_U8 checksum;  // pb0
    MS_U8 VN1;
    MS_U8 VN2;
    MS_U8 VN3;
    MS_U8 VN4;
    MS_U8 VN5;
    MS_U8 VN6;
    MS_U8 VN7;
    MS_U8 VN8;
    MS_U8 PD1;
    MS_U8 PD2;
    MS_U8 PD3;
    MS_U8 PD4;
    MS_U8 PD5;
    MS_U8 PD6;
    MS_U8 PD7;
    MS_U8 PD8;
    MS_U8 PD9;
    MS_U8 PD10;
    MS_U8 PD11;
    MS_U8 PD12;
    MS_U8 PD13;
    MS_U8 PD14;
    MS_U8 PD15;
    MS_U8 PD16;
    MS_U8 SourceInfo; // pb25
};


struct __attribute__((__packed__))
pkt_prototype_payload_mpeg_if {
    MS_U8 checksum;
    union{
        struct{
            MS_U8 MB0;
            MS_U8 MB1;
            MS_U8 MB2;
            MS_U8 MB3;
        };
        MS_U32 MB;
    };
    MS_U8 MF0 : 1;
    MS_U8 MF1 : 1;
    MS_U8 : 2;
    MS_U8 FR0 : 1;
    MS_U8 : 3;
};

struct __attribute__((__packed__))
pkt_prototype_payload_hdr_if {
    MS_U8 checksum;
    MS_U8 EOTF : 3;
    MS_U8 : 5;
    MS_U8 metadata_desc_id : 3;
    MS_U8 : 5;
    MS_U16 dis_primaries_x0;
    MS_U16 dis_primaries_y0;
    MS_U16 dis_primaries_x1;
    MS_U16 dis_primaries_y1;
    MS_U16 dis_primaries_x2;
    MS_U16 dis_primaries_y2;
    MS_U16 white_point_x;
    MS_U16 white_point_y;
    MS_U16 max_dis_mastering_luminance;
    MS_U16 min_dis_mastering_luminance;
    MS_U16 max_content_light_level;
    MS_U16 max_frame_avg_light_level;
};


struct __attribute__((__packed__))
pkt_prototype_payload_acr {
    MS_U8 sb0;

    MS_U8 cts19_16:4;
    MS_U8 :4;
    MS_U8 cts15_8;
    MS_U8 cts7_0;

    MS_U8 n19_16:4;
    MS_U8 :4;
    MS_U8 n15_8;
    MS_U8 n7_0;
};

struct __attribute__((__packed__))
pkt_prototype_header_asp {
    MS_U8 hb0;
    MS_U8 sp3_0 : 4;
    MS_U8 layout : 1;
    MS_U8 : 3;
    MS_U8 sp_f3_0 : 4;
    MS_U8 B3_0 : 4;
};

struct __attribute__((__packed__))
pkt_prototype_payload_asp {
    MS_U8 L11_4;
    MS_U8 L19_12;
    MS_U8 L27_20;
    MS_U8 R11_4;
    MS_U8 R19_12;
    MS_U8 R27_20;
    MS_U8 V_L : 1;
    MS_U8 U_L : 1;
    MS_U8 C_L : 1;
    MS_U8 P_L : 1;
    MS_U8 V_R : 1;
    MS_U8 U_R : 1;
    MS_U8 C_R : 1;
    MS_U8 P_R : 1;
};

struct __attribute__((__packed__))
pkt_prototype_payload_gcp {
    MS_U8 Set_AVMUTE : 1;
    MS_U8  : 3;
    MS_U8 Clear_AVMUTE : 1;
    MS_U8  : 3;
    MS_U8 CD3_0 : 4;
    MS_U8 PP3_0 : 4;
    MS_U8 Default_Phase : 1;
    MS_U8 : 7;
};


struct __attribute__((__packed__))
pkt_prototype_header_acp {
    MS_U8 hb0;
    MS_U8 ACP_Type;
    MS_U8 hb2;
};

struct __attribute__((__packed__))
pkt_prototype_header_isrc {
    MS_U8 hb0;
    MS_U8 ISRC_Status : 3;
    MS_U8  : 3;
    MS_U8 ISRC_Valid : 1;
    MS_U8 ISRC_Cont : 1;
    MS_U8 hb2;
};

struct __attribute__((__packed__))
pkt_prototype_header_one_b_asp {
    MS_U8 hb0;
    MS_U8 sp3_0 : 4;
    MS_U8 layout : 1;
    MS_U8 : 3;
    MS_U8 sp_invalid3_0 : 4;
    MS_U8 : 4;
};

struct __attribute__((__packed__))
pkt_prototype_payload_one_b_asp {
    MS_U8 ChA7_0;
    MS_U8 ChA15_8;
    MS_U8 ChA23_16;
    MS_U8 ChB7_0;
    MS_U8 ChB15_8;
    MS_U8 ChB23_16;
    MS_U8 ChB27_24;
};

struct __attribute__((__packed__))
pkt_prototype_header_emp {
    MS_U8 hb0;
    MS_U8  : 6;
    MS_U8 last : 1;
    MS_U8 first : 1;
    MS_U8 seq_index;
};

struct __attribute__((__packed__))
pkt_prototype_payload_emp {
    MS_U8 : 1;
    MS_U8 sync : 1;
    MS_U8 vfr : 1;
    MS_U8 afr : 1;
    MS_U8 ds_type : 2;
    MS_U8 end : 1;
    MS_U8 new : 1;
    MS_U8 pb1;
    MS_U8 organ_id;
    MS_U8 data_set_tag_msb;
    MS_U8 data_set_tag_lsb;
    MS_U8 data_set_len_msb;
    MS_U8 data_set_len_lsb;
    MS_U8 md[21];
};


struct __attribute__((__packed__))
pkt_prototype_avi_if {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_if hdr_if;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_avi_if payld_avi_if;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_audio_if {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_if hdr_if;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_audio_if payld_audio_if;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_vendor_if {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_if hdr_if;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_vendor_if payld_vendor_if;
        struct pkt_prototype_payload_vendor_if_hf payld_vendor_if_hf;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_spd_if {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_if hdr_if;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_spd_if payld_spd_if;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_mpeg_if {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_if hdr_if;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_mpeg_if payld_mpeg_if;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_hdr_if {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_if hdr_if;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_hdr_if payld_hdr_if;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_acr {
    union{
        struct pkt_prototype_header hdr;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_acr payld_acr;
        MS_U8 pb[28];
    };
};


struct __attribute__((__packed__))
pkt_prototype_gcp {
    union{
        struct pkt_prototype_header hdr;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_gcp payld_gcp;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_acp {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_acp hdr_acp;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_isrc {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_isrc hdr_isrc;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_isrc2 {
    union{
        struct pkt_prototype_header hdr;
        MS_U8 hb[3];
    };
    union {
        struct pkt_prototype_payload payld;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_asp {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_asp hdr_asp;
        MS_U8 hb[3];
    };
    union{
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_asp payld_asp;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_one_b_asp {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_one_b_asp hdr_one_b_asp;
        MS_U8 hb[3];
    };
    union{
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_one_b_asp payld_one_b_asp;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_emp {
    union {
        struct pkt_prototype_header hdr;
        struct pkt_prototype_header_emp hdr_emp;
        MS_U8 hb[3];
    };
    union{
        struct pkt_prototype_payload payld;
        struct pkt_prototype_payload_emp payld_emp;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
pkt_prototype_gnl {
    union {
        struct pkt_prototype_header hdr;
        MS_U8 hb[3];
    };
    union{
        struct pkt_prototype_payload payld;
        MS_U8 pb[28];
    };
};

struct __attribute__((__packed__))
s_pkt_emp_vtem {
	MS_U8 vrr_en : 1; // md0, pb7
	MS_U8 m_const : 1;
	MS_U8 qms_en : 1;
	MS_U8 : 1;
	MS_U8 fva_factor_m1 : 4;
	MS_U8 base_vfront; // md1
	MS_U8  : 2; // md2 (we change the format a little bit to make sw handle easily.)
	MS_U8 rb : 1;
	MS_U8 next_tfr : 5;
	MS_U16 base_refresh_rate_9_0 : 10; // md3
	MS_U16 : 6;
};

// dst
// hbr
// gmp
// 3d asp
// one b 3d asp
// amp

#endif
