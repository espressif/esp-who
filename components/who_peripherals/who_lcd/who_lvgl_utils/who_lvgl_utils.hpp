#pragma once

#include "lvgl.h"
#include <vector>

namespace who {
inline lv_color_t cvt_little_endian_lv_color(const lv_color_t &color)
{
    lv_color_t new_color;
    new_color.red = (color.green & 0x1c) << 3 | (color.blue & 0xc0) >> 3;
    new_color.green = (color.blue & 0x38) << 2 | (color.red & 0xe0) >> 3;
    new_color.blue = (color.red & 0x18) << 3 | (color.green & 0xe0) >> 2;
    return new_color;
}

inline lv_color_t cvt_to_lv_color(const std::vector<uint8_t> &color)
{
#if CONFIG_IDF_TARGET_ESP32P4
    return lv_color_make(color[0], color[1], color[2]);
#else
    return cvt_little_endian_lv_color(lv_color_make(color[0], color[1], color[2]));
#endif
}

inline std::vector<lv_color_t> cvt_to_lv_palette(const std::vector<std::vector<uint8_t>> &palette)
{
    std::vector<lv_color_t> lv_palette;
    for (int i = 0; i < palette.size(); i++) {
        lv_palette.emplace_back(cvt_to_lv_color(palette[i]));
    }
    return lv_palette;
}

lv_obj_t *create_lvgl_btn(const char *text, const lv_font_t *font, lv_obj_t *parent = lv_scr_act());
lv_obj_t *create_lvgl_label(const char *text,
                            const lv_font_t *font,
                            const std::vector<uint8_t> &color = {255, 0, 0},
                            lv_obj_t *parent = lv_scr_act());
} // namespace who
