/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef MDRV_MVD_PARSER_H

#include <linux/platform_device.h>

#define MDRV_MVD_PARSER_H

#define AUTO_ALLOC_ES_BUF 0

#define MAX_VPAS_NUM      2
#define MAX_META_Q_NUM 32
#define VPAS_BIT_MASK 64
#define PAS_HW_ADDR_UNIT 8
#define PAS_DEFAULT_WPTR 0xFFFFFFFF

typedef enum {
	///failed
	E_VPAS_FAIL = 0,
	///success
	E_VPAS_OK,
	///invalid parameter
	E_VPAS_RET_INVALID_PARAM,
	//illegal parameter
	E_VPAS_RET_ILLEGAL_ACCESS,
	//
	E_VPAS_RET_INIT_ALREADY_EXIST,
	//
	E_VPAS_RET_INIT_FAIL,
} VPAS_Result;

typedef enum {
	E_VPAS_ERR_NONE,
	E_VPAS_ERR_ALLOC_ION,
	E_VPAS_ERR_GET_DQBUF,
	E_VPAS_ERR_BUF_TASK,
	E_VPAS_ERR_INIT_FAIL,
	E_VPAS_ERR_REGBASE,
	E_VPAS_ERR_LOCK_FAIL,
	E_VPAS_ERR_UNLOCK_FAIL,
	E_VPAS_ERR_TASK_END_FAIL,
	E_VPAS_ERR_MAX,
} VPAS_ErrCode;

typedef enum {
	/* parser id  */
	PARSER_ID_VIDEO_BASE = 0x0000,
	PARSER_ID_0 = PARSER_ID_VIDEO_BASE,
	PARSER_ID_1,
	PARSER_ID_2,
	PARSER_ID_3,
	PARSER_ID_AUDIO_BASE = 0x1000,
	PARSER_ID_10,
	PARSER_ID_11,
	PARSER_ID_12,
	PARSER_ID_13,
	PARSER_ID_14,
	PARSER_ID_15,
	PARSER_ID_16,
	PARSER_ID_17,
} parser_id_enum;

///VPAS driver status
typedef struct _VPAS_DrvStatus {
	bool bIsInit;
} VPAS_DrvStatus;

typedef void (*VPAS_EventCb) (u32 eFlag, void *param1, void *param2);

//-------------------------------------------------------------------------------------------------
//	Function param structure for ioctl
//-------------------------------------------------------------------------------------------------
//MDrv_VPAS_Init
typedef struct {
	parser_id_enum parser_id;
	int es_buf_tag;
	int RegBaseAddr;
	struct dvb2_vb2_ctx *ctx;
} parser_init_config;

//MDrv_VPAS_Exit
typedef struct {
	u8 u8ParserID;
} VPAS_Exit_Param;

typedef struct {
	s32 s32Fd;
	u64 phyEntryOffset;
	u32 szEntrySize;
	u64 u64Pts;
	u8 u8ParserID;
} VPAS_Data_Entry;

typedef struct {
	VPAS_EventCb pfn;
	u32 u32EventFlag;
	u8 u8ParserID;
	u32 u32Handle;
} VPAS_CB_param;

//-------------------------------------------------------------------------------------------------
//	Function and Variable
//-------------------------------------------------------------------------------------------------


typedef enum {
	VPAS_STATE_STOP,
	VPAS_STATE_INIT,
	VPAS_STATE_IDLE,
	VPAS_STATE_RUN,
} video_parser_state;

typedef enum {
	VPAS_SOURCE_DTV,
	VPAS_SOURCE_TS_FILE,
} video_parser_source;

typedef enum {
	/* set control */
	VPAS_CMD_SET_CONTROL_BASE = 0x0,
	VPAS_CMD_SET_INPUT_MODE,/* in param: value = video_parser_source, default VPAS_SOURCE_DTV.*/
	VPAS_CMD_SET_SECURE,	/* in param: value = 0 disable, value = 1 enable, default 0. */
	VPAS_CMD_SET_TLV_MODE,	/* in param: value = 0 disable, value = 1 enable, default 0. */
	VPAS_CMD_SET_BYPASS_MODE,/* in param: value = 0 disable, value = 1 enable, default 0. */

	/* get control */
	VPAS_CMD_GET_CONTROL_BASE = 0x1000,
	VPAS_CMD_GET_ES_BUF_INFO,/* In: none.out:ptr=&video_parser_es_buf_info.return 0 fail,1 ok */
} video_parser_control_command;

typedef enum {
	VPAS_CALLBACK_SUCCESS,
	VPAS_CALLBACK_FAIL,
} video_parser_callback_status;

typedef enum video_parser_mode {
	VPAS_INPUT_MODE_OVERWRITTEN,
	VPAS_INPUT_MODE_DATA_STOPPED,
};

typedef struct {
	u64 es_buf_addr;	/* es buffer start address,offset, unit byte */
	u32 es_buf_size;	/* es buffer size, unit byte */
	u64 es_buf_rdptr;	/* es buffer read ptr ,unit byte */
	u64 es_buf_wrptr;	/* es buffer write ptr ,unit byte */
	int es_buf_fd;		/* es buffer dma fd, valid when using dma buf */
	int flag;		/* reverse flag */
} video_parser_es_buf_info;

#define VIDEO_PARSER_ES_MATA_VERSION 0	/* version of es meta structure */
typedef struct {

	u8  version;                    /* version of this structure */
	u8  reserved;                   /* reserved */
	u16 struct_size;                /* size of this structure */
	u32 flag;                       /* extension info of parser */
	u32 pts_flag;                   /* presentation timestamp of current es section */
	u64 pts;                        /* presentation timestamp of current es section */
	u32 es_section_offset;          /* current es section, offset from es_buf_addr, unit byte */
	u32 es_section_length;          /* current es section length, unit byte */
	u64 dataid;                     /* bit 0: secure flag*/
	video_parser_es_buf_info es_buf_info;	/* total es buffer info */
	void *ext_metadata;	/*	 extension meta structure	 */
} parser_es_meta;

typedef struct {
	u32 size;		/* size of control parameter, valid when ptr != NULL */
	union {			/* control parameter */
		u32 value;
		void *ptr;
	};
} vedio_parser_ctrl;

typedef struct {
	video_parser_callback_status status;
} video_parser_callback_param;

int video_parser_init(parser_init_config *p_parser_init, struct platform_device *pdev,
	struct mtk_bc_ctx *buf_ctx);
int video_parser_stop(parser_id_enum parser_id);
int video_parser_flush(parser_id_enum parser_id);
int video_parser_suspend(parser_id_enum parser_id);
int video_parser_resume(parser_id_enum parser_id);
int video_parser_get_entry(parser_id_enum parser_id, struct dmx2_ts_video_metadata *p_meta);
int video_parser_update_es_rptr(parser_id_enum parser_id, parser_es_meta *p_meta);
int video_parser_set_control(parser_id_enum parser_id,
	video_parser_control_command cmd, vedio_parser_ctrl *p_ctrl);
int video_parser_get_control(parser_id_enum parser_id, video_parser_control_command cmd,
	vedio_parser_ctrl *p_ctrl);
int video_parser_deinit(parser_id_enum parser_id);
int video_parser_isr(int irq, void *dev_id);

int mtk_dtv_parser_create_sysfs_attr(struct mtk_dmx2dev *dmxdev);
int mtk_dtv_parser_remove_sysfs_attr(struct mtk_dmx2dev *dmxdev);

#endif				/* MDRV_MVD_PARSER_H */
