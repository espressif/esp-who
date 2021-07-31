#pragma once
#include <stdint.h>
#include "esp_log.h"
#include "screen_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t app_lcd_init();
    void app_lcd_draw_wallpaper();
    void app_lcd_set_color(int color);
    void app_lcd_draw_bitmap(uint16_t *image_ptr, const uint16_t image_height, const uint16_t image_width);

#ifdef __cplusplus
}
#endif
