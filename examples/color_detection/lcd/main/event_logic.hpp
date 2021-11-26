#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

/**
 * @brief 
 * 
 * @param key_state_i_adc 
 * @param key_state_i_gpio 
 * @param event_o 
 */
void register_event(const QueueHandle_t key_state_i_adc, const QueueHandle_t key_state_i_gpio, const QueueHandle_t event_o);