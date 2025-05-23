#pragma once
#include "esp_lcd_types.h"
#include "bsp/esp-bsp.h"
#if !BSP_CONFIG_NO_GRAPHIC_LIB
#include "lvgl.h"

namespace who {
namespace lcd {
class WhoLCD {
public:
    WhoLCD(const lvgl_port_cfg_t &lvgl_port_cfg = {4, 6144, 0, 500, 5}) { init(lvgl_port_cfg); }
    ~WhoLCD() { deinit(); }
    void init(const lvgl_port_cfg_t &lvgl_port_cfg);
    void deinit();
    void create_canvas(uint16_t width, uint16_t height);
    lv_obj_t *get_canvas() { return m_canvas; }

private:
    lv_obj_t *m_canvas;
    lv_display_t *m_disp;
};
} // namespace lcd
} // namespace who
#endif
