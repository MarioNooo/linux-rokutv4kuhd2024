/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/*
 * Copyright (c) 2021 MediaTek Inc.
 */

#ifndef MTK_DMX_TEE_COMM_H
#define MTK_DMX_TEE_COMM_H

int mtk_dmx_tee_cmd_open_ctx(void);
int mtk_dmx_tee_cmd_close_ctx(void);
int mtk_dmx_tee_cmd_open_session(u32 *session_id, uuid_t uuid);
int mtk_dmx_tee_cmd_close_session(u32 session_id);
int mtk_dmx_tee_cmd_invoke_cmd(u32 session_id, u32 cmd, void *param, size_t size);

#endif /* MTK_DMX_TEE_COMM_H */
