/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef MTK_DMX_ALP_H
#define MTK_DMX_ALP_H

#include <linux/dvb/dmx2.h>

#include "mtk_dmx_regs.h"

//-------------------------------------------------------------------------------------------------
//  Constants
//-------------------------------------------------------------------------------------------------
#define DEMUX_ALP_KEY_WORD_MAX (4)

#define DEMUX_ALP_IP_ADDR_MAX (8)

#define DEMUX_ALP_IP_FILTER_MAX (32)
#define DEMUX_ALP_CID_FILTER_MAX (24)

#define DEMUX_ALP_IP_V4 (4)
#define DEMUX_ALP_IP_V6 (6)

//-------------------------------------------------------------------------------------------------
//  Macros
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Structs and Enums
//-------------------------------------------------------------------------------------------------
typedef enum demux_alp_instance {
	DEMUX_ALP_INSTANCE_MIN = 0,

	DEMUX_ALP_INSTANCE_0 = DEMUX_ALP_INSTANCE_MIN,
	DEMUX_ALP_INSTANCE_1,

	DEMUX_ALP_INSTANCE_MAX,
	DEMUX_ALP_INSTANCE_NULL = -1,
} demux_alp_instance_e;

typedef enum demux_alp_path_type {
	DEMUX_ALP_PATH_TYPE_MIN = 0,

	DEMUX_ALP_PATH_TYPE_LIVE_IN_MIN = DEMUX_ALP_PATH_TYPE_MIN,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_FROM_TS0,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_FROM_TS1,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_FROM_TS2,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_FROM_TS3,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_FROM_TS4,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_FROM_TS5,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_FROM_TSO,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_FROM_CILINK1,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_FROM_CILINK2,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_FROM_DEMOD,
	DEMUX_ALP_PATH_TYPE_LIVE_IN_MAX,

	DEMUX_ALP_PATH_TYPE_FILEIN_MIN = 0x10,
	DEMUX_ALP_PATH_TYPE_FILEIN_FROM_FILEIN0 = DEMUX_ALP_PATH_TYPE_FILEIN_MIN,
	DEMUX_ALP_PATH_TYPE_FILEIN_FROM_FILEIN1,
	DEMUX_ALP_PATH_TYPE_FILEIN_FROM_FILEIN2,
	DEMUX_ALP_PATH_TYPE_FILEIN_FROM_FILEIN3,
	DEMUX_ALP_PATH_TYPE_FILEIN_FROM_MMFI0,
	DEMUX_ALP_PATH_TYPE_FILEIN_FROM_MMFI1,
	DEMUX_ALP_PATH_TYPE_FILEIN_FROM_MMFI2,
	DEMUX_ALP_PATH_TYPE_FILEIN_FROM_MMFI3,
	DEMUX_ALP_PATH_TYPE_FILEIN_MAX,

	DEMUX_ALP_PATH_TYPE_MTK_ALP_TOP_MIN = 0x20,
	DEMUX_ALP_PATH_TYPE_MTK_ALP_TOP_FROM_TSIF_ALP = DEMUX_ALP_PATH_TYPE_MTK_ALP_TOP_MIN,
	DEMUX_ALP_PATH_TYPE_MTK_ALP_TOP_FROM_ALP_FILEIN_TOP,
	DEMUX_ALP_PATH_TYPE_MTK_ALP_TOP_MAX,

	DEMUX_ALP_PATH_TYPE_FIFO_BEFORE_PVR_MIN = 0x30,
	DEMUX_ALP_PATH_TYPE_FIFO_BEFORE_PVR_FROM_TSIF_ALP = DEMUX_ALP_PATH_TYPE_FIFO_BEFORE_PVR_MIN,
	DEMUX_ALP_PATH_TYPE_FIFO_BEFORE_PVR_FROM_ALP_PROCESS,
	DEMUX_ALP_PATH_TYPE_FIFO_BEFORE_PVR_FROM_MTK_ALP_TOP,
	DEMUX_ALP_PATH_TYPE_FIFO_BEFORE_PVR_MAX,

	DEMUX_ALP_PATH_TYPE_ALP_OUT_FIFO_MIN = 0x40,
	DEMUX_ALP_PATH_TYPE_ALP_OUT_FIFO_FROM_FIFO_BEFORE_PVR =
		DEMUX_ALP_PATH_TYPE_ALP_OUT_FIFO_MIN,
	DEMUX_ALP_PATH_TYPE_ALP_OUT_FIFO_FROM_TIMESTAM_KEYWORD_SIDEBAND_INSERT,
	DEMUX_ALP_PATH_TYPE_ALP_OUT_FIFO_MAX,

	DEMUX_ALP_PATH_TYPE_MAX,
	DEMUX_ALP_PATH_TYPE_NULL = -1,
} demux_alp_path_type_e;

typedef enum demux_alp_key_word_type {
	DEMUX_ALP_KEY_WORD_TYPE_MIN = 0,

	DEMUX_ALP_KEY_WORD_TYPE_NO_INSERT = DEMUX_ALP_KEY_WORD_TYPE_MIN,
	DEMUX_ALP_KEY_WORD_TYPE_KEY_WORD_1B_TIMESTAMP_4B,
	DEMUX_ALP_KEY_WORD_TYPE_KEY_WORD_4B_TIMESTAMP_4B,
	DEMUX_ALP_KEY_WORD_TYPE_KEY_WORD_4B,
	DEMUX_ALP_KEY_WORD_TYPE_TIMESTAMP_4B,

	DEMUX_ALP_KEY_WORD_TYPE_MAX,
	DEMUX_ALP_KEY_WORD_TYPE_NULL = -1,
} demux_alp_key_word_type_e;

typedef struct demux_alp_key_words {
	demux_alp_key_word_type_e eKeyWordType;
	u8 u8keyWords[DEMUX_ALP_KEY_WORD_MAX];

	bool bSidebandEnable;
} demux_alp_key_words_t;

typedef struct ip_addr {
	union {
		u8 u8Addrs[DEMUX_ALP_IP_ADDR_MAX * 2];
		u16 u16Addrs[DEMUX_ALP_IP_ADDR_MAX];
		u32 u32Addr; // IPV4
	} uAddr;
	union {
		u8 u8Masks[DEMUX_ALP_IP_ADDR_MAX * 2];
		u16 u16Masks[DEMUX_ALP_IP_ADDR_MAX];
		u32 u32Mask; // IPV4
	} uMask;
} ip_addr_t;

typedef struct udp_port {
	union {
		u8 u8Ports[2];
		u16 u16Port;
	} uPort;
	union {
		u8 u8Masks[2];
		u16 u16Mask;
	} uMask;
} udp_port_t;

typedef struct demux_alp_plp {
	bool bIsPlp; // depreciated
	bool bSinglePlpMode;
	u8 u8PlpId;
} demux_alp_plp_t;

typedef struct demux_alp_sid {
	bool bIsSid;
	u8 u8Sid;
} demux_alp_sid_t;

typedef struct demux_alp_cid {
	bool bIsCid;
	u8 u8Cid;
} demux_alp_cid_t;

typedef enum demux_alp_udp_port {
	DEMUX_ALP_UDP_PORT_MIN = 0,
	DEMUX_ALP_UDP_PORT_DESTINATION = DEMUX_ALP_UDP_PORT_MIN,
	DEMUX_ALP_UDP_PORT_SOURCE,
	DEMUX_ALP_UDP_PORT_MAX,
	DEMUX_ALP_UDP_PORT_NULL = -1,
} demux_alp_udp_port_e;

typedef struct demux_alp_ip_filter {
	s8 s8Index;

	u8 u8IpVersion; // use DEMUX_ALP_IPv4 or DEMUX_ALP_IPv6
	ip_addr_t stSrcAddr;
	ip_addr_t stDesAddr;
	udp_port_t stSrcPort;
	udp_port_t stDesPort;

	demux_alp_plp_t stPlp;
	demux_alp_sid_t stSid;
	demux_alp_cid_t stCid;

	u8 u8FilterId;
} demux_alp_filter_t;

//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------
int mtk_dmx_alp_init(demux_alp_instance_e eInstance);
int mtk_dmx_alp_exit(demux_alp_instance_e eInstance);

demux_alp_instance_e mtk_dmx_alp_tsio_path_2_alp_instance(int path_id);

int mtk_dmx_alp_set_path(
		demux_alp_instance_e eInstance,
		demux_alp_path_type_e ePathType);

int mtk_dmx_alp_set_key_words(
		demux_alp_instance_e eInstance,
		demux_alp_key_words_t *pstKeyWords);
int mtk_dmx_alp_is_timestamp_keyword_sideband_inserted(bool *pbIsInserted);
int mtk_dmx_alp_get_keywords(demux_alp_key_words_t *pstKeyWords);

int mtk_dmx_alp_get_filter(
		demux_alp_instance_e eInstance,
		demux_alp_filter_t *pstFilter);
int mtk_dmx_alp_clear_filter(
		demux_alp_instance_e eInstance,
		demux_alp_filter_t *pstFilter);
int mtk_dmx_alp_set_filter(
		demux_alp_instance_e eInstance,
		demux_alp_filter_t *pstFilter);
void mtk_dmx_alp_print_filter(demux_alp_filter_t *pstFilter);

#endif /* MTK_DMX_ALP_H */
