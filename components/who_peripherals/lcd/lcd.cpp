#include "lcd.hpp"
#include "esp_lcd_panel_ops.h"
#include "bsp/esp-bsp.h"

static const char *TAG = "who_lcd";

namespace who {
namespace lcd {

#if BSP_CONFIG_NO_GRAPHIC_LIB
#pragma message("lcd wo lvgl")
esp_lcd_panel_handle_t LCD::s_panel_handle = nullptr;
LCD::LCD()
{
    esp_lcd_panel_io_handle_t io_handle = nullptr;
#if CONFIG_IDF_TARGET_ESP32P4
    const bsp_display_config_t bsp_disp_cfg{};
    ESP_ERROR_CHECK(bsp_display_new(&bsp_disp_cfg, &s_panel_handle, &io_handle));
#elif CONFIG_IDF_TARGET_ESP32S3
    const bsp_display_config_t bsp_disp_cfg = {
        .max_transfer_sz = BSP_LCD_H_RES * BSP_LCD_V_RES * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(bsp_display_new(&bsp_disp_cfg, &s_panel_handle, &io_handle));
    esp_lcd_panel_disp_on_off(s_panel_handle, true);
#endif
    ESP_ERROR_CHECK(bsp_display_backlight_on());
}

void LCD::set_cam_fb(who::cam::cam_fb_t *fb)
{
    esp_lcd_panel_draw_bitmap(s_panel_handle, 0, 0, fb->width, fb->height, fb->buf);
}
#else
#pragma message("lcd with lvgl")
lv_obj_t *LCD::s_canvas = nullptr;
LCD::LCD()
{
    bsp_display_start();
    ESP_ERROR_CHECK(bsp_display_backlight_on());

    bsp_display_lock(0);
    s_canvas = lv_canvas_create(lv_scr_act());
    lv_obj_set_size(s_canvas, BSP_LCD_H_RES, BSP_LCD_V_RES);
    bsp_display_unlock();
}

void LCD::set_cam_fb(who::cam::cam_fb_t *fb)
{
    lv_canvas_set_buffer(s_canvas, fb->buf, fb->width, fb->height, LV_COLOR_FORMAT_NATIVE);
}
#endif

} // namespace lcd
} // namespace who
