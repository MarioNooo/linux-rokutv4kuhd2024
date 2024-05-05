/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_INTERNAL_IF_H
#define MTK_DMX_INTERNAL_IF_H

typedef enum {
	PURPOSE_PLAYBACK,
	PURPOSE_RECORD,
	PURPOSE_MAX
} dmx_purpose_t;

typedef enum {
	CRYPTO_TYPE_PAYLOAD,
	CRYPTO_TYPE_BLOCK,
	CRYPTO_TYPE_MAX,
} dmx_crypto_type;

struct dmx_query_info {
	u16 path_id;
	u16 pid;
	struct dmx2_ip_addr *ip_addr;
	dmx_purpose_t purpose;
};

struct dmx_ca_info {
	u16 path_id;
	u8 eng_id;
	bool pass_through;
};

struct dmx_secure_info {
	bool enable;
	u32 pipeline_id;
	u32 filter_id;
};

struct dmx_crypto_info {
	u16 path_id;
	dmx_crypto_type type;
};

// for [CA DD]
int mtk_dmx_get_filter_id(struct dmx_query_info info);
int mtk_dmx_set_ca_ctrl(struct dmx_ca_info info);
int mtk_dmx_set_secure_mode(struct dmx_secure_info info);
int mtk_dmx_set_crypto_type(struct dmx_crypto_info info);

#endif /* MTK_DMX_INTERNAL_IF_H */
