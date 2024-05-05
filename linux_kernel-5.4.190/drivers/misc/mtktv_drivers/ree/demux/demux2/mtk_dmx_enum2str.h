/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#undef ENUM_ELEMENT
#undef ENUM_ELEMENT_WITH_VALUE

#ifndef ENUM_GENERATE_STRINGS
#define ENUM_ELEMENT(element) element
//#define ENUM_ELEMENT_WITH_VALUE(element, value) element = (value)
#else
#define ENUM_ELEMENT(element) { element, #element }
//#define ENUM_ELEMENT_WITH_VALUE(element, value) { value, #element }
#endif //#ifndef ENUM_GENERATE_STRINGS

#ifndef MTK_DMX_ENUM_2_STR_H
#define MTK_DMX_ENUM_2_STR_H

#define ENUM_GET_NAME(ENUM_NAME, value) GetEnumName_##ENUM_NAME(value)
#define ENUM_DECLARE_GET_NAME_FUNC(ENUM_TYPE, ENUM_NAME, ENUMS) \
	static MTK_DMX_ENUM_NAME_MAPPING_T gs_##ENUM_NAME[] = ENUMS; \
	static char sz##ENUM_NAME##NotFound[256]; \
	static const char *GetEnumName_##ENUM_NAME(ENUM_TYPE eEnum) \
	{ \
		s32 i; \
		const char *szEnumName = sz##ENUM_NAME##NotFound; \
		memset(sz##ENUM_NAME##NotFound, \
			0, \
			sizeof(sz##ENUM_NAME##NotFound)); \
		snprintf( \
			sz##ENUM_NAME##NotFound, \
			sizeof(sz##ENUM_NAME##NotFound), \
			"Enum %d", \
			eEnum); \
		for (i = 0; i < ARRAY_SIZE(gs_##ENUM_NAME); i++) { \
			if (gs_##ENUM_NAME[i].i4EnumValue == eEnum) { \
				szEnumName = gs_##ENUM_NAME[i].pszEnumName; \
				break; \
			} \
		} \
		return szEnumName; \
	}

typedef struct {
	s32 i4EnumValue;
	const char *pszEnumName;
} MTK_DMX_ENUM_NAME_MAPPING_T;

#endif				/* MTK_DMX_ENUM_2_STR_H */
