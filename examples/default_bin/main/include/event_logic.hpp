#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

/**
 * @brief 
 * 
 * @param key_state_i 
 * @param event_o 
 */
void register_event(const QueueHandle_t sr_result_i, const QueueHandle_t key_state_i, const QueueHandle_t event_o);