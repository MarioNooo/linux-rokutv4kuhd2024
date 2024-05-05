#ifndef _MDRV_HW_RIU_H_
#define _MDRV_HW_RIU_H_

#ifdef _MDRV_HW_RIU_C_
#ifndef INTERFACE
#define INTERFACE
#endif
#else
#ifndef INTERFACE
#define INTERFACE	extern
#endif
#endif

typedef enum {
	EN_HW_REG_TYPE_XC,
	EN_HW_REG_TYPE_XC_DUMMY,
	EN_HW_REG_TYPE_XC_EXT,
	EN_HW_REG_TYPE_XC_EXT2,
	EN_HW_REG_TYPE_XC_IPCLK,
	EN_HW_REG_TYPE_TC,
	EN_HW_REG_TYPE_FRC01,
	EN_HW_REG_TYPE_FRC02,
	EN_HW_REG_TYPE_FSC,
	EN_HW_REG_TYPE_LPLL,
	EN_HW_REG_TYPE_IPMUX,
	EN_HW_REG_TYPE_HDMI_DUAL0,
	EN_HW_REG_TYPE_HDMI_DUAL2,
	EN_HW_REG_TYPE_HDMI2_DUAL0,
	EN_HW_REG_TYPE_HDMI2_DUAL2,
	EN_HW_REG_TYPE_HDMIRX_DTOP_PKT_P0,
	EN_HW_REG_TYPE_CLKGEN0,
	EN_HW_REG_TYPE_CLKGEN2,
	EN_HW_REG_TYPE_MAX,
}EN_HW_REG_TYPE;

INTERFACE MS_BOOL MDrv_HW_RIU_SetBaseAddr(MS_VIRT virtBaseAddr, EN_HW_REG_TYPE eRegType);
INTERFACE void MDrv_HW_RIU_W2BYTE(MS_U32 u32Reg, MS_U16 u16Val);
INTERFACE MS_U16 MDrv_HW_RIU_R2BYTE(MS_U32 u32Reg);
INTERFACE void MDrv_HW_RIU_W2BYTEMSK(MS_U32 u32Reg, MS_U16 u16Val, MS_U16 u16Mask);
INTERFACE MS_U16 MDrv_HW_RIU_R2BYTEMSK(MS_U32 u32Reg, MS_U16 u16Mask);
INTERFACE void MDrv_HW_RIU_WriteByte(MS_U32 u32Reg, MS_U8 u8Val);
INTERFACE void MDrv_HW_RIU_WriteByteMask(MS_U32 u32Reg, MS_U8 u8Val, MS_U16 u16Mask);
INTERFACE MS_U8 MDrv_HW_RIU_ReadByte(MS_U32 u32Reg);

#undef INTERFACE
#endif		//_MDRV_HW_RIU_H_
