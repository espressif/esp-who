#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

esp_err_t register_lcd(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb);

void app_lcd_draw_wallpaper();
void app_lcd_set_color(int color);

#ifdef __cplusplus
}
#endif
