#include "who_recognition.hpp"
#include <esp_log.h>
#if !BSP_CONFIG_NO_GRAPHIC_LIB
#include "who_lvgl_utils.hpp"
#endif
#if CONFIG_IDF_TARGET_ESP32P4
#define WHO_REC_RES_SHOW_N_FRAMES (60)
#elif CONFIG_IDF_TARGET_ESP32S3
#define WHO_REC_RES_SHOW_N_FRAMES (30)
#endif
#if !BSP_CONFIG_NO_GRAPHIC_LIB
LV_FONT_DECLARE(montserrat_bold_26);
#endif
namespace who {
namespace recognition {
detect::WhoDetectBase::result_t WhoDetectLCD::get_result()
{
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    result_t result = m_result;
    xSemaphoreGive(m_res_mutex);
    return result;
}

void WhoDetectLCD::on_new_detect_result(const result_t &result)
{
    detect::WhoDetectLCD::on_new_detect_result(result);
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    m_result = result;
    xSemaphoreGive(m_res_mutex);
}

void WhoRecognition::task()
{
    create_btns();
    create_label();
    while (true) {
        set_and_clear_bits(BLOCKING, RUNNING);
        EventBits_t event_bits = xEventGroupWaitBits(
            m_event_group, RECOGNIZE | ENROLL | DELETE | PAUSE | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bits & STOP) {
            printf("Stop received\n");
            set_and_clear_bits(TERMINATE, BLOCKING);
            break;
        } else if (event_bits & PAUSE) {
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, RESUME | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & STOP) {
                set_and_clear_bits(TERMINATE, BLOCKING);
                break;
            } else {
                continue;
            }
        }
        set_and_clear_bits(RUNNING, BLOCKING);
        m_detect->pause();
        if (event_bits & (RECOGNIZE | ENROLL)) {
            auto result = m_detect->get_result();
            auto img = who::cam::fb2img(result.fb);
            if (event_bits & RECOGNIZE) {
                printf("Recognize received\n");
                auto rec_res = m_recognizer->recognize(img, result.det_res);
                char *text = new char[64];
                if (rec_res.empty()) {
                    strcpy(text, "who?");
                } else {
                    snprintf(text, 64, "id: %d, sim: %.2f", rec_res[0].id, rec_res[0].similarity);
                }
                xSemaphoreTake(m_res_mutex, portMAX_DELAY);
                m_results.emplace_back(text);
                xSemaphoreGive(m_res_mutex);
                printf("%s\n", text);
                if (m_result_cb) {
                   m_result_cb(text);
                }
            }
            if (event_bits & ENROLL) {
                printf("Enroll received\n");
                esp_err_t ret = m_recognizer->enroll(img, result.det_res);
                char *text = new char[64];
                if (ret == ESP_FAIL) {
                    strcpy(text, "Failed to enroll.");
                } else {
                    snprintf(text, 64, "id: %d enrolled.", m_recognizer->get_num_feats());
                }
                xSemaphoreTake(m_res_mutex, portMAX_DELAY);
                m_results.emplace_back(text);
                xSemaphoreGive(m_res_mutex);
                printf("%s\n", text);
                if (m_result_cb) {
                   m_result_cb(text);
                }

            }
        }
        if (event_bits & DELETE) {
            printf("Delete received\n");
            esp_err_t ret = m_recognizer->delete_last_feat();
            char *text = new char[64];
            if (ret == ESP_FAIL) {
                strcpy(text, "Failed to delete.");
            } else {
                snprintf(text, 64, "id: %d deleted.", m_recognizer->get_num_feats() + 1);
            }
            xSemaphoreTake(m_res_mutex, portMAX_DELAY);
            m_results.emplace_back(text);
            xSemaphoreGive(m_res_mutex);
            printf("%s\n", text);
            if (m_result_cb) {
                m_result_cb(text);
            }
        }
        m_detect->resume();
    }
    vTaskDelete(NULL);
}

#if !BSP_CONFIG_NO_GRAPHIC_LIB
void WhoRecognition::lvgl_btn_event_handler(lv_event_t *e)
{
    user_data_t *user_data = reinterpret_cast<user_data_t *>(lv_event_get_user_data(e));
    EventGroupHandle_t event_group = user_data->who_rec_ptr->get_event_group();
    EventBits_t event_bits = xEventGroupGetBits(event_group);
    if (event_bits & BLOCKING && !(event_bits & PAUSE)) {
        xEventGroupSetBits(event_group, user_data->event);
    }
}
#endif // !BSP_CONFIG_NO_GRAPHIC_LIB

void WhoRecognition::virtual_btn_event_handler( event_type_t event)
{
    EventGroupHandle_t event_group = get_event_group();
    EventBits_t event_bits = xEventGroupGetBits(event_group);
    if (event_bits & BLOCKING && !(event_bits & PAUSE)) {
        xEventGroupSetBits(event_group, event);
    }
}


void WhoRecognition::iot_btn_event_handler(void *button_handle, void *usr_data)
{
    user_data_t *user_data = reinterpret_cast<user_data_t *>(usr_data);
    EventGroupHandle_t event_group = user_data->who_rec_ptr->get_event_group();
    EventBits_t event_bits = xEventGroupGetBits(event_group);
    if (event_bits & BLOCKING && !(event_bits & PAUSE)) {
        xEventGroupSetBits(event_group, user_data->event);
    }
}

void WhoRecognition::new_result_subscription(const std::function<void(result_t)> &cb) {
    m_result_cb = cb;
}

void WhoRecognition::create_btns()
{
#if CONFIG_IDF_TARGET_ESP32P4
    bsp_display_lock(0);
    lv_obj_t *btn_recognize = create_lvgl_btn("recognize", &montserrat_bold_26);
    lv_obj_t *btn_enroll = create_lvgl_btn("enroll", &montserrat_bold_26);
    lv_obj_t *btn_delete = create_lvgl_btn("delete", &montserrat_bold_26);
    lv_obj_add_event_cb(btn_recognize, lvgl_btn_event_handler, LV_EVENT_CLICKED, (void *)m_btn_user_data);
    lv_obj_add_event_cb(btn_enroll, lvgl_btn_event_handler, LV_EVENT_CLICKED, (void *)(m_btn_user_data + 1));
    lv_obj_add_event_cb(btn_delete, lvgl_btn_event_handler, LV_EVENT_CLICKED, (void *)(m_btn_user_data + 2));
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
    iot_button_register_cb(btns[2], BUTTON_SINGLE_CLICK, nullptr, iot_btn_event_handler, (void *)m_btn_user_data));

    // up    enroll
    ESP_ERROR_CHECK(
    iot_button_register_cb(btns[2], BUTTON_DOUBLE_CLICK, nullptr, iot_btn_event_handler, (void *)(m_btn_user_data +1)));
    
    // down  delete
    ESP_ERROR_CHECK(
    iot_button_register_cb(btns[2], BUTTON_LONG_PRESS_START, nullptr, iot_btn_event_handler, (void *)(m_btn_user_data +2)));
    
#endif
}

void WhoRecognition::create_label()
{
#if !BSP_CONFIG_NO_GRAPHIC_LIB
    bsp_display_lock(0);
    m_label = create_lvgl_label("", &montserrat_bold_26);
    const lv_font_t *font = lv_obj_get_style_text_font(m_label, LV_PART_MAIN);
    lv_obj_align(m_label, LV_ALIGN_TOP_MID, 0, font->line_height);
    bsp_display_unlock();
#endif // !BSP_CONFIG_NO_GRAPHIC_LIB
}

void WhoRecognition::lcd_display_cb(who::cam::cam_fb_t *fb)
{
#if !BSP_CONFIG_NO_GRAPHIC_LIB
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    static int cnt = WHO_REC_RES_SHOW_N_FRAMES;
    if (!m_results.empty()) {
        lv_label_set_text(m_label, m_results.back());
        for (auto iter = m_results.begin(); iter != m_results.end(); iter++) {
            delete[] *iter;
        }
        m_results.clear();
        cnt = 0;
    }
    if (cnt < WHO_REC_RES_SHOW_N_FRAMES && ++cnt == WHO_REC_RES_SHOW_N_FRAMES) {
        lv_label_set_text(m_label, "");
    }
    xSemaphoreGive(m_res_mutex);
#endif
}

} // namespace recognition
} // namespace who
