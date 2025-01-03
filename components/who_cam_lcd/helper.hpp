#pragma once
#include "cam_define.hpp"
#include "dl_detect_define.hpp"
#include "dl_image.hpp"
#include "lcd.hpp"
#include <list>

namespace who {
namespace app {

inline bool compare_timestamp(const struct timeval &t1, const struct timeval &t2)
{
    if (t1.tv_sec == t2.tv_sec) {
        return t1.tv_usec < t2.tv_usec;
    }
    return t1.tv_sec < t2.tv_sec;
}

void draw_detect_results(who::cam::cam_fb_t *fb,
                         const std::list<dl::detect::result_t> &detect_res,
                         const std::vector<uint8_t> &color = {255, 0, 0});
#if !BSP_CONFIG_NO_GRAPHIC_LIB
inline lv_color_t cvt_little_endian_color(const lv_color_t &color)
{
    lv_color_t new_color;
    new_color.red = (color.green & 0x1c) << 3 | (color.blue & 0xc0) >> 3;
    new_color.green = (color.blue & 0x38) << 2 | (color.red & 0xe0) >> 3;
    new_color.blue = (color.red & 0x18) << 3 | (color.green & 0xe0) >> 2;
    return new_color;
}
lv_obj_t *create_lvgl_btn(const char *text, const lv_font_t *font);
lv_obj_t *create_lvgl_label(const char *text, const lv_font_t *font, const std::vector<uint8_t> &color = {255, 0, 0});
#endif

} // namespace app
} // namespace who
