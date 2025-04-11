#pragma once
#include "esp_lcd_types.h"
#include "bsp/esp-bsp.h"
#if !BSP_CONFIG_NO_GRAPHIC_LIB
#include "lvgl.h"
#endif

namespace who {
namespace lcd {
#if BSP_CONFIG_NO_GRAPHIC_LIB
class LCD {
public:
    LCD();
    static esp_lcd_panel_handle_t s_panel_handle;
};
#else
class LCD {
public:
    LCD();
    static lv_obj_t *s_canvas;
};
#endif

} // namespace lcd
} // namespace who
