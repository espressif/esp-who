#include "who_lcd.hpp"
#include "who_app.hpp"
#include "who_cam.hpp"

static const char *TAG = "who_lcd";

namespace who {
namespace lcd {
TaskHandle_t WhoLCD::s_task_handle = nullptr;
QueueHandle_t WhoLCD::s_queue_handle = nullptr;
SemaphoreHandle_t WhoLCD::s_mutex = xSemaphoreCreateMutex();

LCD::LCD()
{
    bsp_display_start();
    ESP_ERROR_CHECK(bsp_display_backlight_on());

    bsp_display_lock(0);
    m_canvas = lv_canvas_create(lv_scr_act());
    lv_obj_set_size(m_canvas, BSP_LCD_H_RES, BSP_LCD_V_RES);
    bsp_display_unlock();

    init_lvgl_dscs();
}

LCD::~LCD()
{
    if (m_canvas) {
        lv_obj_del(m_canvas);
        m_canvas = nullptr;
    }
}

void LCD::set_cam_fb(who::cam::cam_fb_t *fb)
{
    lv_canvas_set_buffer(m_canvas, fb->buf, fb->width, fb->height, LV_COLOR_FORMAT_NATIVE);
}

void LCD::init_lvgl_dscs()
{
    lv_color_t color = get_color_by_palette(LV_PALETTE_RED);

    lv_draw_label_dsc_init(&m_label_dsc);
    m_label_dsc.color = color;
    m_label_dsc.font = &montserrat_bold_26;

    lv_draw_rect_dsc_init(&m_rect_dsc);
    m_rect_dsc.bg_opa = LV_OPA_TRANSP;
    m_rect_dsc.border_width = 2;
    m_rect_dsc.border_color = color;

    lv_draw_arc_dsc_init(&m_arc_dsc);
    m_arc_dsc.color = color;
    m_arc_dsc.width = 5;
    m_arc_dsc.radius = 5;
    m_arc_dsc.start_angle = 0;
    m_arc_dsc.end_angle = 360;
}

void LCD::draw_label(const char *label, lv_area_t coords)
{
    m_label_dsc.text = label;
    lv_draw_label(&m_layer, &m_label_dsc, &coords);
}

void LCD::draw_rectangle(const std::vector<int> &box, const char *label)
{
    static lv_area_t coords_rect;
    coords_rect.x1 = box[0];
    coords_rect.y1 = box[1];
    coords_rect.x2 = box[2];
    coords_rect.y2 = box[3];
    lv_draw_rect(&m_layer, &m_rect_dsc, &coords_rect);
    if (label) {
        int32_t h = montserrat_bold_26.line_height;
        int32_t w = lv_strlen(label) * h / 2;
        draw_label(label, {box[0], box[1] - h, box[0] + w, box[1]});
    }
}

void LCD::draw_landmarks(const std::vector<int> &landmarks)
{
    assert(landmarks.size() == 10);
    for (int i = 0; i < 5; i++) {
        m_arc_dsc.center.x = landmarks[2 * i];
        m_arc_dsc.center.y = landmarks[2 * i + 1];
        lv_draw_arc(&m_layer, &m_arc_dsc);
    }
}

void LCD::draw_detect_result(const std::list<dl::detect::result_t> &detect_res)
{
    lv_canvas_init_layer(m_canvas, &m_layer);
    for (const auto &res : detect_res) {
        draw_rectangle(res.box);
        if (!res.keypoint.empty()) {
            draw_landmarks(res.keypoint);
        }
    }
    lv_canvas_finish_layer(m_canvas, &m_layer);
}

lv_obj_t *LCD::create_btn(const char *text, lv_obj_t *parent)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, &montserrat_bold_26);
    lv_obj_add_style(btn, &style, 0);
    lv_obj_center(label);
    return btn;
}

lv_obj_t *LCD::create_label(const char *text, lv_obj_t *parent)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, &montserrat_bold_26);
    lv_style_set_text_color(&style, get_color_by_palette(LV_PALETTE_RED));
    lv_obj_add_style(label, &style, 0);
    return label;
}

void WhoLCD::display_result(who::cam::cam_fb_t *fb)
{
    xSemaphoreTake(s_mutex, portMAX_DELAY);
    for (int type = 0; type < RESULT_TYPE_MAX + 1; type++) {
        if (type == FACE_DETECT_RESULT_TYPE || type == PEDESTRIAN_DETECT_RESULT_TYPE) {
            // Try to sync camera frame and result, skip the future result.
            auto it =
                std::find_if(m_results[type].rbegin(), m_results[type].rend(), [fb](const result_t &result) -> bool {
                    struct timeval t1 = fb->timestamp;
                    struct timeval t2 = result.timestamp;
                    if (t1.tv_sec == t2.tv_sec) {
                        return t1.tv_usec >= t2.tv_usec;
                    }
                    return t1.tv_sec >= t2.tv_sec;
                });
            // If any result should display, remove all outdated results before.
            if (it != m_results[type].rend()) {
                m_lcd->draw_detect_result(*(it->res.detect_res));
                for (auto iter = m_results[type].begin(); iter != std::prev(it.base());) {
                    delete iter->res.detect_res;
                    iter = m_results[type].erase(iter);
                }
            }
        } else {
            static int cnt = 60;
            if (!m_results[type].empty()) {
                lv_label_set_text(who::app::WhoHumanFaceRecognition::s_label,
                                  m_results[type].back().res.recognition_res);
                for (auto iter = m_results[type].begin(); iter != m_results[type].end(); iter++) {
                    delete[] iter->res.recognition_res;
                }
                m_results[type].clear();
                cnt = 0;
            }
            if (cnt < 60 && ++cnt == 60) {
                lv_label_set_text(who::app::WhoHumanFaceRecognition::s_label, "");
            }
        }
    }
    xSemaphoreGive(s_mutex);
}

void WhoLCD::display_task(void *args)
{
    WhoLCD *self = (WhoLCD *)args;
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        who::cam::cam_fb_t *fb = self->m_cam->cam_fb_peek(false);
        // dl::image::img_t src_img = {
        //     fb->buf,
        //     (int)fb->width,
        //     (int)fb->height,
        //     dl::image::DL_IMAGE_PIX_TYPE_RGB565
        // };
        // dl::image::img_t dst_img = {
        //     .data = fb->buf,
        //     .pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB565
        // };
        // convert_img(src_img, dst_img, DL_IMAGE_CAP_RGB565_BYTE_SWAP);
        bsp_display_lock(0);
        self->m_lcd->set_cam_fb(fb);
        self->display_result(fb);
        bsp_display_unlock();
        self->m_cam->cam_fb_return();
        xTaskNotifyGive(who::cam::WhoCam::s_task_handle);
    }
}

void WhoLCD::recv_task(void *args)
{
    WhoLCD *self = (WhoLCD *)args;
    result_t result;
    while (true) {
        xQueueReceive(s_queue_handle, &result, portMAX_DELAY);
        xSemaphoreTake(s_mutex, portMAX_DELAY);
        self->m_results[result.type].emplace_back(result);
        xSemaphoreGive(s_mutex);
    }
}

void WhoLCD::run()
{
    if (xTaskCreatePinnedToCore(display_task, "WhoLCD_display", 3072, this, 2, &s_task_handle, 0) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create WhoLCD_display task.\n");
    };
    if (xTaskCreatePinnedToCore(recv_task, "WhoLCD_recv", 1536, this, 2, nullptr, 0) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create WhoLCD_recv task.\n");
    };
}

} // namespace lcd
} // namespace who
