/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once
#include "esp32_mock.h"
#include "mdns.h"
#include "mdns_private.h"


static inline void *_mdns_get_packet_data(mdns_rx_packet_t *packet)
{
    return packet->pb->payload;
}

static inline size_t _mdns_get_packet_len(mdns_rx_packet_t *packet)
{
    return packet->pb->len;
}

static inline void _mdns_packet_free(mdns_rx_packet_t *packet)
{
    free(packet->pb);
    free(packet);
}
