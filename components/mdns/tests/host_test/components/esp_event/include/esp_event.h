/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "stdbool.h"
#include "esp_err.h"
#include "esp_event_base.h"
#include "bsd/string.h"

esp_err_t esp_event_handler_register(const char *event_base, int32_t event_id, void *event_handler, void *event_handler_arg);

esp_err_t esp_event_handler_unregister(const char *event_base, int32_t event_id, void *event_handler);
