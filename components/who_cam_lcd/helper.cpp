#include "helper.hpp"
#include "cam.hpp"

namespace who {
namespace app {
using namespace who::lcd;

#if BSP_CONFIG_NO_GRAPHIC_LIB
void draw_detect_results(who::cam::cam_fb_t *fb,
                         const std::list<dl::detect::result_t> &detect_res,
                         const std::vector<uint8_t> &color)
{
#if CONFIG_IDF_TARGET_ESP32P4
    uint32_t caps = DL_IMAGE_CAP_RGB565_BIG_ENDIAN;
#else
    uint32_t caps = 0;
#endif
    dl::image::img_t img = who::cam::fb2img(fb);
    for (const auto &res : detect_res) {
        dl::image::draw_hollow_rectangle(img, res.box[0], res.box[1], res.box[2], res.box[3], color, 2, caps);
        if (!res.keypoint.empty()) {
            assert(res.keypoint.size() == 10);
            for (int i = 0; i < 5; i++) {
                dl::image::draw_point(img, res.keypoint[2 * i], res.keypoint[2 * i + 1], color, 5, caps);
            }
        }
    }
    LCD::set_cam_fb(fb);
}
#else
void draw_detect_results(who::cam::cam_fb_t *fb,
                         const std::list<dl::detect::result_t> &detect_res,
                         const std::vector<uint8_t> &color)
{
    bsp_display_lock(0);
    LCD::set_cam_fb(fb);
#if CONFIG_IDF_TARGET_ESP32P4
    lv_color_t c = lv_color_make(color[0], color[1], color[2]);
#else
    lv_color_t c = cvt_little_endian_color(lv_color_make(color[0], color[1], color[2]));
#endif

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_opa = LV_OPA_TRANSP;
    rect_dsc.border_width = 2;
    rect_dsc.border_color = c;

    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);
    arc_dsc.color = c;
    arc_dsc.width = 5;
    arc_dsc.radius = 5;
    arc_dsc.start_angle = 0;
    arc_dsc.end_angle = 360;

    lv_layer_t layer;
    lv_canvas_init_layer(LCD::s_canvas, &layer);
    lv_area_t coords_rect;
    for (const auto &res : detect_res) {
        coords_rect = {res.box[0], res.box[1], res.box[2], res.box[3]};
        lv_draw_rect(&layer, &rect_dsc, &coords_rect);
        if (!res.keypoint.empty()) {
            assert(res.keypoint.size() == 10);
            for (int i = 0; i < 5; i++) {
                arc_dsc.center.x = res.keypoint[2 * i];
                arc_dsc.center.y = res.keypoint[2 * i + 1];
                lv_draw_arc(&layer, &arc_dsc);
            }
        }
    }
    lv_canvas_finish_layer(LCD::s_canvas, &layer);
    bsp_display_unlock();
}

lv_obj_t *create_lvgl_btn(const char *text, const lv_font_t *font)
{
    lv_obj_t *btn = lv_button_create(lv_scr_act());
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, font);
    lv_obj_add_style(btn, &style, 0);
    lv_obj_center(label);
    return btn;
}

lv_obj_t *create_lvgl_label(const char *text, const lv_font_t *font, const std::vector<uint8_t> &color)
{
#if CONFIG_IDF_TARGET_ESP32P4
    lv_color_t c = lv_color_make(color[0], color[1], color[2]);
#else
    lv_color_t c = cvt_little_endian_color(lv_color_make(color[0], color[1], color[2]));
#endif
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, text);
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_color(&style, c);
    lv_obj_add_style(label, &style, 0);
    return label;
}
#endif
} // namespace app
} // namespace who
