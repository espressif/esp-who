#pragma once
#include "esp_lcd_types.h"
#include "bsp/esp-bsp.h"
#if !BSP_CONFIG_NO_GRAPHIC_LIB
#include "who_lvgl_lcd.hpp"
#else

namespace who {
namespace lcd {
class WhoLCD {
public:
    WhoLCD() { init(); }
    void init();
    esp_lcd_panel_handle_t get_lcd_panel_handle();
    void draw_full_lcd(const void *data);

private:
#if CONFIG_IDF_TARGET_ESP32S3
    esp_lcd_panel_handle_t m_panel_handle;
    esp_lcd_panel_io_handle_t m_io_handle;
    void *m_lcd_buffer;
#elif CONFIG_IDF_TARGET_ESP32P4
    bsp_lcd_handles_t m_lcd_handles;
#endif
};
} // namespace lcd
} // namespace who
#endif
