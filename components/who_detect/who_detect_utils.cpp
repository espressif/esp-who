#include "who_detect_utils.hpp"

static const char *TAG = "WhoDetect";

namespace who {
namespace detect {

void draw_detect_results_on_fb(who::cam::cam_fb_t *fb,
                               const std::list<dl::detect::result_t> &detect_res,
                               const std::vector<std::vector<uint8_t>> &palette)
{
#if CONFIG_IDF_TARGET_ESP32P4
    uint32_t caps = 0;
#else
    uint32_t caps = DL_IMAGE_CAP_RGB565_BIG_ENDIAN;
#endif
    dl::image::img_t img = *fb;
    for (const auto &res : detect_res) {
        dl::image::draw_hollow_rectangle(
            img, res.box[0], res.box[1], res.box[2], res.box[3], palette[res.category], 2, caps);
        if (!res.keypoint.empty()) {
            assert(res.keypoint.size() == 10);
            for (int i = 0; i < 5; i++) {
                dl::image::draw_point(
                    img, res.keypoint[2 * i], res.keypoint[2 * i + 1], palette[res.category], 5, caps);
            }
        }
    }
}

#if !BSP_CONFIG_NO_GRAPHIC_LIB
void draw_detect_results_on_canvas(lv_obj_t *canvas,
                                   const std::list<dl::detect::result_t> &detect_res,
                                   const std::vector<lv_color_t> &palette)
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_opa = LV_OPA_TRANSP;
    rect_dsc.border_width = 2;

    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);
    arc_dsc.width = 5;
    arc_dsc.radius = 5;
    arc_dsc.start_angle = 0;
    arc_dsc.end_angle = 360;

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_area_t coords_rect;
    for (const auto &res : detect_res) {
        coords_rect = {res.box[0], res.box[1], res.box[2], res.box[3]};
        rect_dsc.border_color = palette[res.category];
        lv_draw_rect(&layer, &rect_dsc, &coords_rect);
        if (!res.keypoint.empty()) {
            arc_dsc.color = palette[res.category];
            assert(res.keypoint.size() == 10);
            for (int i = 0; i < 5; i++) {
                arc_dsc.center.x = res.keypoint[2 * i];
                arc_dsc.center.y = res.keypoint[2 * i + 1];
                lv_draw_arc(&layer, &arc_dsc);
            }
        }
    }
    lv_canvas_finish_layer(canvas, &layer);
}
#endif

void print_detect_results(const std::list<dl::detect::result_t> &detect_res)
{
    int i = 0;
    if (!detect_res.empty()) {
        if (detect_res.begin()->keypoint.empty()) {
            ESP_LOGI(TAG, "----------------------------------------");
        } else {
            ESP_LOGI(
                TAG,
                "---------------------------------------------------------------------------------------------------"
                "---------------------------------------------------");
        }
    }
    for (const auto &r : detect_res) {
        if (r.keypoint.empty()) {
            ESP_LOGI(TAG, "%d, bbox: [%f, %d, %d, %d, %d]", i, r.score, r.box[0], r.box[1], r.box[2], r.box[3]);
        } else {
            assert(r.keypoint.size() == 10);
            ESP_LOGI(TAG,
                     "%d, bbox: [%f, %d, %d, %d, %d], left_eye: [%d, %d], left_mouth: [%d, %d], nose: [%d, %d], "
                     "right_eye: [%d, %d], right_mouth: [%d, %d]",
                     i,
                     r.score,
                     r.box[0],
                     r.box[1],
                     r.box[2],
                     r.box[3],
                     r.keypoint[0],
                     r.keypoint[1],
                     r.keypoint[2],
                     r.keypoint[3],
                     r.keypoint[4],
                     r.keypoint[5],
                     r.keypoint[6],
                     r.keypoint[7],
                     r.keypoint[8],
                     r.keypoint[9]);
        }
        i++;
    }
}

} // namespace detect
} // namespace who
