/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef MTK_DMX_CA_H
#define MTK_DMX_CA_H

typedef struct {
	u32 offset;
	u32 fld;
} reg_fld_t;

// CA in
reg_fld_t ca_in_tsif0[] = {
	{REG_0100_TSP1, TSP1_REG_CA_CTRL_0_0100_0},
	{REG_0104_TSP1, TSP1_REG_CA_CTRL_1_0104_8},
	{REG_010C_TSP1, TSP1_REG_CA_CTRL2_010C_0}
};
reg_fld_t ca_in_tsiffi[] = {
	{REG_0100_TSP1, TSP1_REG_CA_CTRL_0_0100_1},
	{REG_0104_TSP1, TSP1_REG_CA_CTRL_1_0104_9},
	{REG_010C_TSP1, TSP1_REG_CA_CTRL2_010C_1}
};
reg_fld_t ca_in_tsif1[] = {
	{REG_0100_TSP1, TSP1_REG_CA_CTRL_0_0100_2},
	{REG_0104_TSP1, TSP1_REG_CA_CTRL_1_0104_10},
	{REG_010C_TSP1, TSP1_REG_CA_CTRL2_010C_2}
};
reg_fld_t ca_in_tsif2[] = {
	{REG_0100_TSP1, TSP1_REG_CA_CTRL_0_0100_12},
	{REG_0104_TSP1, TSP1_REG_CA_CTRL_1_0104_4},
	{REG_0104_TSP1, TSP1_REG_CA_CTRL_1_0104_7}
};
reg_fld_t ca_in_tlv[] = {
	{REG_0170_TSP_TLV, TSP_TLV_REG_TLV2DSCRMB0_SRC_0170},
	{REG_0170_TSP_TLV, TSP_TLV_REG_TLV2DSCRMB1_SRC_0170},
	{REG_0170_TSP_TLV, TSP_TLV_REG_TLV2DSCRMB2_SRC_0170}
};

// CA out
reg_fld_t ca_out_tsif0[] = {
	{REG_0100_TSP1, TSP1_REG_CA_CTRL_0_0100_4},
	{REG_0104_TSP1, TSP1_REG_CA_CTRL_1_0104_12},
	{REG_010C_TSP1, TSP1_REG_CA_CTRL2_010C_4}
};
reg_fld_t ca_out_tsiffi[] = {
	{REG_0100_TSP1, TSP1_REG_CA_CTRL_0_0100_5},
	{REG_0104_TSP1, TSP1_REG_CA_CTRL_1_0104_13},
	{REG_010C_TSP1, TSP1_REG_CA_CTRL2_010C_5}
};
reg_fld_t ca_out_tsif1[] = {
	{REG_0100_TSP1, TSP1_REG_CA_CTRL_0_0100_6},
	{REG_0104_TSP1, TSP1_REG_CA_CTRL_1_0104_14},
	{REG_010C_TSP1, TSP1_REG_CA_CTRL2_010C_6}
};
reg_fld_t ca_out_tsif2[] = {
	{REG_0100_TSP1, TSP1_REG_CA_CTRL_0_0100_13},
	{REG_0104_TSP1, TSP1_REG_CA_CTRL_1_0104_5},
	{REG_0104_TSP1, TSP1_REG_CA_CTRL_1_0104_8}
};
reg_fld_t ca_out_tlv[] = {
	{REG_0174_TSP_TLV, TSP_TLV_REG_DSCRMB02TLV_SRC_0174},
	{REG_0174_TSP_TLV, TSP_TLV_REG_DSCRMB12TLV_SRC_0174},
	{REG_0174_TSP_TLV, TSP_TLV_REG_DSCRMB22TLV_SRC_0174}
};

// TLV CA en
reg_fld_t ca_en_tlv[] = {
	{REG_016C_TSP_TLV, TSP_TLV_REG_TLV_CA_CTRL_016C_0},
	{REG_016C_TSP_TLV, TSP_TLV_REG_TLV_CA_CTRL_016C_1},
	{REG_016C_TSP_TLV, TSP_TLV_REG_TLV_CA_CTRL_016C_2}
};


#endif /* MTK_DMX_CA_H */
