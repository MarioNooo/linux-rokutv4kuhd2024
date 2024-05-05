////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020-2024 MediaTek Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MediaTek Inc. and be kept in strict confidence
// ("MediaTek Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MediaTek Confidential
// Information is unlawful and strictly prohibited. MediaTek hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __PORTABLE_ENDIAN_H__
#define __PORTABLE_ENDIAN_H__

static inline int16_t payload_to_le16(const uint8_t *payload)
{
    const int s16_size = sizeof(int16_t);
    int idx = 0;
    union {
        int16_t s16;
        uint8_t data[s16_size];
    } le16;

    for(idx = 0; idx < s16_size; idx++) {
        le16.data[idx] = payload[idx];
    }

    return le16.s16;
}

static inline void le16_to_payload(uint8_t *payload, int16_t value)
{
    const int s16_size = sizeof(int16_t);
    int idx = 0;
    union {
        int16_t s16;
        uint8_t data[s16_size];
    } le16;

    le16.s16 = value;

    for(idx = 0; idx < s16_size; idx++) {
        payload[idx] = le16.data[idx];
    }
}

static inline int32_t payload_to_le32(const uint8_t *payload)
{
    const int s32_size = sizeof(int32_t);
    int idx = 0;
    union {
        int32_t s32;
        uint8_t data[s32_size];
    } le32;

    for(idx = 0; idx < s32_size; idx++) {
        le32.data[idx] = payload[idx];
    }

    return le32.s32;
}

static inline void le32_to_payload(uint8_t *payload, int32_t value)
{
    const int s32_size = sizeof(int32_t);
    int idx = 0;
    union {
        int32_t s32;
        uint8_t data[s32_size];
    } le32;

    le32.s32 = value;

    for(idx = 0; idx < s32_size; idx++) {
        payload[idx] = le32.data[idx];
    }
}

static inline uint64_t payload_to_le64(const uint8_t *payload)
{
    const int u64_size = sizeof(uint64_t);
    int idx = 0;
    union {
        uint64_t u64;
        uint8_t data[u64_size];
    } le64;

    for(idx = 0; idx < u64_size; idx++) {
        le64.data[idx] = payload[idx];
    }

    return le64.u64;
}

static inline void le64_to_payload(uint8_t *payload, uint64_t value)
{
    const int u64_size = sizeof(uint64_t);
    int idx = 0;
    union {
        uint64_t u64;
        uint8_t data[u64_size];
    } le64;

    le64.u64 = value;

    for(idx = 0; idx < u64_size; idx++) {
        payload[idx] = le64.data[idx];
    }
}

#endif // #ifndef __PORTABLE_ENDIAN_H__
