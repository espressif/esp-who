#include "who_recognition.hpp"
#include "display_func_manager.hpp"
#include "helper.hpp"
#if CONFIG_IDF_TARGET_ESP32P4
#define WHO_REC_RES_SHOW_N_FRAMES (60)
#elif CONFIG_IDF_TARGET_ESP32S3
#define WHO_REC_RES_SHOW_N_FRAMES (30)
#endif

static const char *TAG = "who_recognition";
LV_FONT_DECLARE(montserrat_bold_26);

namespace who {
namespace app {
using namespace who::lcd;
TaskHandle_t WhoHumanFaceRecognition::s_task_handle = nullptr;

void WhoHumanFaceRecognition::event_handle_task(void *args)
{
    WhoHumanFaceRecognition *self = (WhoHumanFaceRecognition *)args;
    while (true) {
        uint32_t event;
        xTaskNotifyWait(0, 0xffffffff, &event, portMAX_DELAY);
        if (event & static_cast<uint32_t>(fr_event_t::RECOGNIZE)) {
            xSemaphoreTake(self->m_status_mutex, portMAX_DELAY);
            self->m_status = fr_status_t::RECOGNIZE;
            xSemaphoreGive(self->m_status_mutex);
        }
        if (event & static_cast<uint32_t>(fr_event_t::ENROLL)) {
            xSemaphoreTake(self->m_status_mutex, portMAX_DELAY);
            self->m_status = fr_status_t::ENROLL;
            xSemaphoreGive(self->m_status_mutex);
        }
        if (event & static_cast<uint32_t>(fr_event_t::DELETE)) {
            xSemaphoreTake(self->m_status_mutex, portMAX_DELAY);
            self->m_status = fr_status_t::DELETE;
            xSemaphoreGive(self->m_status_mutex);
        }
    }
}

void WhoHumanFaceRecognition::recognition_task(void *args)
{
    WhoHumanFaceRecognition *self = (WhoHumanFaceRecognition *)args;
    self->create_btns();
    self->create_label();
    struct timeval timestamp;
    fr_status_t status;
    int64_t start = esp_timer_get_time();
    int64_t task_wt_interval = 1000000;
    while (true) {
        xSemaphoreTake(self->m_status_mutex, portMAX_DELAY);
        status = self->m_status;
        xSemaphoreGive(self->m_status_mutex);
        switch (status) {
        case fr_status_t::RECOGNIZE: {
            auto *fb = self->m_cam->cam_fb_peek();
            timestamp = fb->timestamp;
            auto img = who::cam::fb2img(fb);
            auto &det_res = self->m_detect->run(img);
            xSemaphoreTake(self->m_det_res_mutex, portMAX_DELAY);
            self->m_det_results.push({det_res, timestamp});
            xSemaphoreGive(self->m_det_res_mutex);
            auto rec_res = self->m_recognizer->recognize(img, det_res);
            char *text = new char[64];
            if (rec_res.empty()) {
                strcpy(text, "who?");
            } else {
                snprintf(text, 64, "id: %d, sim: %.2f", rec_res[0].id, rec_res[0].similarity);
            }
            xSemaphoreTake(self->m_rec_res_mutex, portMAX_DELAY);
            self->m_rec_results.emplace_back(text);
            xSemaphoreGive(self->m_rec_res_mutex);
            xSemaphoreTake(self->m_status_mutex, portMAX_DELAY);
            self->m_status = fr_status_t::DETECT;
            xSemaphoreGive(self->m_status_mutex);
            break;
        }
        case fr_status_t::ENROLL: {
            auto *fb = self->m_cam->cam_fb_peek();
            timestamp = fb->timestamp;
            auto img = who::cam::fb2img(fb);
            auto &det_res = self->m_detect->run(img);
            xSemaphoreTake(self->m_det_res_mutex, portMAX_DELAY);
            self->m_det_results.push({det_res, timestamp});
            xSemaphoreGive(self->m_det_res_mutex);
            esp_err_t ret = self->m_recognizer->enroll(img, det_res);
            char *text = new char[64];
            if (ret == ESP_FAIL) {
                strcpy(text, "Failed to enroll.");
            } else {
                snprintf(text, 64, "id: %d enrolled.", self->m_recognizer->get_num_feats());
            }
            xSemaphoreTake(self->m_rec_res_mutex, portMAX_DELAY);
            self->m_rec_results.emplace_back(text);
            xSemaphoreGive(self->m_rec_res_mutex);
            xSemaphoreTake(self->m_status_mutex, portMAX_DELAY);
            self->m_status = fr_status_t::DETECT;
            xSemaphoreGive(self->m_status_mutex);
            break;
        }
        case fr_status_t::DELETE: {
            esp_err_t ret = self->m_recognizer->delete_last_feat();
            char *text = new char[64];
            if (ret == ESP_FAIL) {
                strcpy(text, "Failed to delete.");
            } else {
                snprintf(text, 64, "id: %d deleted.", self->m_recognizer->get_num_feats() + 1);
            }
            xSemaphoreTake(self->m_rec_res_mutex, portMAX_DELAY);
            self->m_rec_results.emplace_back(text);
            xSemaphoreGive(self->m_rec_res_mutex);
            xSemaphoreTake(self->m_status_mutex, portMAX_DELAY);
            self->m_status = fr_status_t::DETECT;
            xSemaphoreGive(self->m_status_mutex);
            break;
        }
        case fr_status_t::DETECT: {
            auto *fb = self->m_cam->cam_fb_peek();
            timestamp = fb->timestamp;
            auto &det_res = self->m_detect->run(who::cam::fb2img(fb));
            xSemaphoreTake(self->m_det_res_mutex, portMAX_DELAY);
            self->m_det_results.push({det_res, timestamp});
            xSemaphoreGive(self->m_det_res_mutex);
            break;
        }
        }
        int64_t end = esp_timer_get_time();
        if (end - start >= task_wt_interval) {
            vTaskDelay(pdMS_TO_TICKS(10));
            start = esp_timer_get_time();
        }
    }
}

void WhoHumanFaceRecognition::lvgl_btn_event_handler(lv_event_t *e)
{
    fr_event_t fr_event = (fr_event_t)(reinterpret_cast<int>(lv_event_get_user_data(e)));
    btn_event_handler(fr_event);
}

void WhoHumanFaceRecognition::iot_btn_event_handler(void *button_handle, void *usr_data)
{
    fr_event_t fr_event = (fr_event_t)(reinterpret_cast<int>(usr_data));
    btn_event_handler(fr_event);
}

inline void WhoHumanFaceRecognition::btn_event_handler(fr_event_t fr_event)
{
    switch (fr_event) {
    case fr_event_t::RECOGNIZE:
        xTaskNotify(s_task_handle, (uint32_t)fr_event_t::RECOGNIZE, eSetBits);
        break;
    case fr_event_t::ENROLL:
        xTaskNotify(s_task_handle, (uint32_t)fr_event_t::ENROLL, eSetBits);
        break;
    case fr_event_t::DELETE:
        xTaskNotify(s_task_handle, (uint32_t)fr_event_t::DELETE, eSetBits);
        break;
    }
}

void WhoHumanFaceRecognition::create_btns()
{
#if CONFIG_IDF_TARGET_ESP32P4
    bsp_display_lock(0);
    lv_obj_t *btn_recognize = create_lvgl_btn("recognize", &montserrat_bold_26);
    lv_obj_t *btn_enroll = create_lvgl_btn("enroll", &montserrat_bold_26);
    lv_obj_t *btn_delete = create_lvgl_btn("delete", &montserrat_bold_26);
    lv_obj_add_event_cb(btn_recognize, lvgl_btn_event_handler, LV_EVENT_CLICKED, (void *)fr_event_t::RECOGNIZE);
    lv_obj_add_event_cb(btn_enroll, lvgl_btn_event_handler, LV_EVENT_CLICKED, (void *)fr_event_t::ENROLL);
    lv_obj_add_event_cb(btn_delete, lvgl_btn_event_handler, LV_EVENT_CLICKED, (void *)fr_event_t::DELETE);
    lv_obj_update_layout(btn_recognize);
    lv_obj_update_layout(btn_enroll);
    lv_obj_update_layout(btn_delete);
    int32_t w = lv_obj_get_width(btn_recognize);
    w = std::max(w, lv_obj_get_width(btn_enroll));
    w = std::max(w, lv_obj_get_width(btn_delete));
    int32_t h = lv_obj_get_height(btn_recognize);
    lv_obj_set_size(btn_recognize, w, h);
    lv_obj_set_size(btn_enroll, w, h);
    lv_obj_set_size(btn_delete, w, h);
    int32_t pad = h / 2;
    lv_obj_align(btn_recognize, LV_ALIGN_TOP_RIGHT, -pad, pad);
    lv_obj_align(btn_enroll, LV_ALIGN_TOP_RIGHT, -pad, pad + h + pad);
    lv_obj_align(btn_delete, LV_ALIGN_TOP_RIGHT, -pad, pad + 2 * (h + pad));
    bsp_display_unlock();
#elif CONFIG_IDF_TARGET_ESP32S3
    button_handle_t btns[BSP_BUTTON_NUM];
    ESP_ERROR_CHECK(bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM));
    // play  recognize
    ESP_ERROR_CHECK(
        iot_button_register_cb(btns[1], BUTTON_SINGLE_CLICK, iot_btn_event_handler, (void *)fr_event_t::RECOGNIZE));
    // up    enroll
    ESP_ERROR_CHECK(
        iot_button_register_cb(btns[3], BUTTON_SINGLE_CLICK, iot_btn_event_handler, (void *)fr_event_t::ENROLL));
    // down  delete
    ESP_ERROR_CHECK(
        iot_button_register_cb(btns[2], BUTTON_SINGLE_CLICK, iot_btn_event_handler, (void *)fr_event_t::DELETE));
#endif
}

void WhoHumanFaceRecognition::create_label()
{
    bsp_display_lock(0);
    m_label = create_lvgl_label("", &montserrat_bold_26);
    const lv_font_t *font = lv_obj_get_style_text_font(m_label, LV_PART_MAIN);
    lv_obj_align(m_label, LV_ALIGN_TOP_MID, 0, font->line_height);
    bsp_display_unlock();
}

void WhoHumanFaceRecognition::display(who::cam::cam_fb_t *fb)
{
    xSemaphoreTake(m_det_res_mutex, portMAX_DELAY);
    // Try to sync camera frame and result, skip the future result.
    struct timeval t1 = fb->timestamp;
    bool display = false;
    det_result_t det_result;
    while (!m_det_results.empty()) {
        det_result = m_det_results.front();
        if (!compare_timestamp(t1, det_result.timestamp)) {
            m_det_results.pop();
            display = true;
        } else {
            break;
        }
    }
    xSemaphoreGive(m_det_res_mutex);
    if (display) {
        draw_detect_results(fb, det_result.det_res);
    }

    xSemaphoreTake(m_rec_res_mutex, portMAX_DELAY);
    static int cnt = WHO_REC_RES_SHOW_N_FRAMES;
    if (!m_rec_results.empty()) {
        bsp_display_lock(0);
        lv_label_set_text(m_label, m_rec_results.back());
        bsp_display_unlock();
        for (auto iter = m_rec_results.begin(); iter != m_rec_results.end(); iter++) {
            delete[] *iter;
        }
        m_rec_results.clear();
        cnt = 0;
    }
    if (cnt < WHO_REC_RES_SHOW_N_FRAMES && ++cnt == WHO_REC_RES_SHOW_N_FRAMES) {
        bsp_display_lock(0);
        lv_label_set_text(m_label, "");
        bsp_display_unlock();
    }
    xSemaphoreGive(m_rec_res_mutex);
}

void WhoHumanFaceRecognition::run()
{
    auto display_func_manager = DisplayFuncManager::get_instance();
    display_func_manager->register_display_func(
        "WhoRec", std::bind(&WhoHumanFaceRecognition::display, this, std::placeholders::_1));
    if (xTaskCreatePinnedToCore(event_handle_task, "WhoRecEvent", 2560, this, 2, &s_task_handle, 0) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create WhoRecog_event task.\n");
    }
    if (xTaskCreatePinnedToCore(recognition_task, "WhoRec", 3584, this, 2, nullptr, 1) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create WhoRecog task.\n");
    }
}
} // namespace app
} // namespace who
