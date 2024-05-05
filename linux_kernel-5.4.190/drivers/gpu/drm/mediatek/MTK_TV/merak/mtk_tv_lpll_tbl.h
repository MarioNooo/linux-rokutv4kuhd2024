/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef _MTK_TV_LPLL_TBL_H_
#define _MTK_TV_LPLL_TBL_H_

#define LPLL_REG_NUM         (44)
#define PNL_NUM_VER1         (35)

//======version2 start======
#define LPLL_REG_NUM_VER2    (35)
#define MODA_REG_NUM_VER2    (29)
#define MPLL_REG_NUM_VER2    (7)
#define PNL_NUM_VER2         (21)

typedef enum {
	E_PNL_SUPPORTED_LPLL_LVDS_1ch_450to600MHz,	//0
	E_PNL_SUPPORTED_LPLL_LVDS_1ch_300to450MHz,	//1
	E_PNL_SUPPORTED_LPLL_LVDS_1ch_300to300MHz,	//2

	E_PNL_SUPPORTED_LPLL_LVDS_2ch_450to600MHz,	//3
	E_PNL_SUPPORTED_LPLL_LVDS_2ch_300to450MHz,	//4
	E_PNL_SUPPORTED_LPLL_LVDS_2ch_300to300MHz,	//5

	E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_450to600MHz,	//6
	E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_300to450MHz,	//7
	E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_300to300MHz,	//8

	E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_450to600MHz,	//9
	E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_300to450MHz,	//10
	E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_300to300MHz,	//11

	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_3BYTE_400to600MHz,	//12
	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_3BYTE_300to400MHz,	//13
	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_3BYTE_300to300MHz,	//14

	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_4BYTE_400to600MHz,	//15
	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_4BYTE_300to400MHz,	//16
	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_4BYTE_300to300MHz,	//17

	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_5BYTE_400to600MHz,	//18
	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_5BYTE_300to400MHz,	//19
	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_5BYTE_300to300MHz,	//20

	E_PNL_SUPPORTED_LPLL_Vx1_Video_4BYTE_OSD_3BYTE_400to600MHz,	//21
	E_PNL_SUPPORTED_LPLL_Vx1_Video_4BYTE_OSD_3BYTE_300to400MHz,	//22
	E_PNL_SUPPORTED_LPLL_Vx1_Video_4BYTE_OSD_3BYTE_300to300MHz,	//23

	E_PNL_SUPPORTED_LPLL_Vx1_Video_4BYTE_OSD_4BYTE_300to600MHz,	//24
	E_PNL_SUPPORTED_LPLL_Vx1_Video_4BYTE_OSD_4BYTE_300to300MHz,	//25

	E_PNL_SUPPORTED_LPLL_Vx1_Video_4BYTE_OSD_5BYTE_300to600MHz,	//26
	E_PNL_SUPPORTED_LPLL_Vx1_Video_4BYTE_OSD_5BYTE_300to300MHz,	//27

	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_3BYTE_400to600MHz,	//28
	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_3BYTE_300to400MHz,	//29
	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_3BYTE_300to300MHz,	//30

	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_4BYTE_300to600MHz,	//31
	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_4BYTE_300to300MHz,	//32

	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_5BYTE_300to600MHz,	//33
	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_5BYTE_300to300MHz,	//34
//======version2 end======
	E_PNL_SUPPORTED_LPLL_LVDS_1ch_450to600MHz_VER2,          //0
	E_PNL_SUPPORTED_LPLL_LVDS_1ch_300to450MHz_VER2,          //1
	E_PNL_SUPPORTED_LPLL_LVDS_2ch_450to600MHz_VER2,          //2
	E_PNL_SUPPORTED_LPLL_LVDS_2ch_300to450MHz_VER2,          //3
	E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_450to600MHz_VER2,          //4
	E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_300to450MHz_VER2,          //5
	E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_450to600MHz_VER2,          //6
	E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_300to450MHz_VER2,          //7
	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_3BYTE_340to720MHz_VER2,          //8
	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_4BYTE_340to720MHz_VER2,          //9
	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_5BYTE_400to600MHz_VER2,          //10
	E_PNL_SUPPORTED_LPLL_Vx1_Video_3BYTE_OSD_5BYTE_300to400MHz_VER2,          //11
	E_PNL_SUPPORTED_LPLL_Vx1_Video_4BYTE_OSD_3BYTE_340to720MHz_VER2,          //12
	E_PNL_SUPPORTED_LPLL_Vx1_Video_4BYTE_OSD_4BYTE_340to720MHz_VER2,          //13
	E_PNL_SUPPORTED_LPLL_Vx1_Video_4BYTE_OSD_5BYTE_300to600MHz_VER2,          //14
	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_3BYTE_400to600MHz_VER2,          //15
	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_3BYTE_300to400MHz_VER2,          //16
	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_4BYTE_300to600MHz_VER2,          //17
	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_5BYTE_300to600MHz_VER2,          //18
	E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_Xtal_mode_0to607_5MHz_VER2,          //19
	E_PNL_SUPPORTED_LPLL_Vx1_Video_5BYTE_OSD_5BYTE_Xtal_mode_0to583_2MHz_VER2,          //20
//======version2 end======
	E_PNL_SUPPORTED_LPLL_MAX,
} E_PNL_SUPPORTED_LPLL_TYPE;

#endif				//_LPLL_TBL_H_
