#pragma once
#include "esp_lcd_types.h"
#include "bsp/esp-bsp.h"
#if !BSP_CONFIG_NO_GRAPHIC_LIB
#include "lvgl.h"

namespace who {
namespace lcd {
class WhoLCD {
public:
    WhoLCD(const lvgl_port_cfg_t &lvgl_port_cfg = {4, 6144, 0, 500, MALLOC_CAP_INTERNAL, 5}) { init(lvgl_port_cfg); }
    ~WhoLCD() { deinit(); }
    void init(const lvgl_port_cfg_t &lvgl_port_cfg);
    void deinit();

private:
    lv_display_t *m_disp;
};
} // namespace lcd
} // namespace who
#endif
