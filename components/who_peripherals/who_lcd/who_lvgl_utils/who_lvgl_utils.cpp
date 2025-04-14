#include "who_lvgl_utils.hpp"

namespace who {
lv_obj_t *create_lvgl_btn(const char *text, const lv_font_t *font, lv_obj_t *parent)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_center(label);
    return btn;
}

lv_obj_t *create_lvgl_label(const char *text,
                            const lv_font_t *font,
                            const std::vector<uint8_t> &color,
                            lv_obj_t *parent)
{
#if CONFIG_IDF_TARGET_ESP32P4
    lv_color_t c = lv_color_make(color[0], color[1], color[2]);
#else
    lv_color_t c = cvt_little_endian_lv_color(lv_color_make(color[0], color[1], color[2]));
#endif
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, c, LV_PART_MAIN);
    return label;
}
} // namespace who
