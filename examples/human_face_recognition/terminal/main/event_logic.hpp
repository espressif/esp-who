#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * @brief Register button events
 * 
 * @param[in] event_o Queue handle where the events will be added
 */
void register_button_events(const QueueHandle_t event_o);