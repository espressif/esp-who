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
    lv_color_t c = cvt_to_lv_color(color);
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, c, LV_PART_MAIN);
    return label;
}
} // namespace who
