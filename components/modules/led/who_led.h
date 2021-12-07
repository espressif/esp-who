#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if CONFIG_LED_ILLUMINATOR_ENABLED
    void app_led_init();
    void app_led_duty(int duty)
#endif

typedef enum
{
    LED_ALWAYS_OFF = 0,
    LED_ALWAYS_ON,
    LED_OFF_1S,
    LED_OFF_2S,
    LED_OFF_4S,
    LED_ON_1S,
    LED_ON_2S,
    LED_ON_4S,
    LED_BLINK_1S,
    LED_BLINK_2S,
    LED_BLINK_4S,
} led_state_t;

void register_led(const gpio_num_t led_io_num, const QueueHandle_t control_i);

#ifdef __cplusplus
}
#endif
