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

void copy_img(const dl::image::img_t &src, std::list<dl::detect::result_t> &detect_res, dl::image::img_t &dst, bool cropped);

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

    if (!result.det_res.empty() && subscriptor_m_event_group_ != nullptr) {
        xEventGroupSetBits(subscriptor_m_event_group_, subscriptor_m_event_type_);
    }
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

            // If a face is detected
            if (!result.det_res.empty()) {
                // Cppy the latest detected result
                img_last_det_res = result.det_res;

                char *text = new char[64];
                char *json = new char[128];

                // Crop the first detected face
                xSemaphoreTake(m_res_mutex, portMAX_DELAY);
                copy_img(img, result.det_res, latest_image_with_a_face, 0);
                copy_img(img, result.det_res, latest_face_cropped, 1);
                xSemaphoreGive(m_res_mutex);

                // Prepare the text for the result
                if (event_bits & RECOGNIZE) {
                    dl::recognition::result_t recognition_result;
                    auto rec_res = m_recognizer->recognize(img, result.det_res);

                    if (rec_res.empty()) {
                        recognition_result.id = 0;
                        recognition_result.similarity = 0.0f;
                        strcpy(text, "who?");
                    } else {
                        recognition_result.id = rec_res[0].id;
                        recognition_result.similarity = rec_res[0].similarity;
                        snprintf(text, 64, "id: %d, sim: %.2f", rec_res[0].id, rec_res[0].similarity);
                    }
                    snprintf(json, 128, "{\"action\":\"recognize\",\"id\":%d,\"sim\":%.2f,\"message\":\"%s\"}", recognition_result.id, recognition_result.similarity, text);

                    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
                    m_results.emplace_back(text);
                    xSemaphoreGive(m_res_mutex);
                }
                if (event_bits & ENROLL) {
                    int enrollId = 0;
                    esp_err_t ret = m_recognizer->enroll(img, result.det_res);
                    if (ret == ESP_FAIL) {
                        strcpy(text, "Failed to enroll.");
                    } else {
                        snprintf(text, 64, "id: %d enrolled.", m_recognizer->get_num_feats());
                        enrollId = m_recognizer->get_num_feats();
                    }
                    snprintf(json, 128, "{\"action\":\"enroll\",\"id\":%d,\"message\":\"%s\"}", enrollId, text);

                    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
                    m_results.emplace_back(text);
                    xSemaphoreGive(m_res_mutex);
                }

                // Display the result
                if (m_result_cb) {
                    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
                    m_result_cb(json, latest_image_with_a_face, latest_face_cropped);
                    xSemaphoreGive(m_res_mutex);
                }
            } else {
                char *text = new char[64];
                char *json = new char[128];
                int enrollId = 0;

                // No face detected -> enroll last detected face
                if (event_bits & RECOGNIZE) {
                    strcpy(text, "No face detected");
                    snprintf(json, 128, "{\"action\":\"recognize\",\"id\":%d,\"sim\":%d,\"message\":\"%s\"}", 0, 0, text);

                } else if (event_bits & ENROLL) {
                    esp_err_t ret = m_recognizer->enroll(latest_image_with_a_face, img_last_det_res);
                    if (ret == ESP_FAIL) {
                        strcpy(text, "Failed to enroll latest detected face.");
                    } else {
                        snprintf(text, 64, "id: %d enrolled.", m_recognizer->get_num_feats());
                        enrollId = m_recognizer->get_num_feats();
                    }
                    snprintf(json, 128, "{\"action\":\"enroll\",\"id\":%d,\"message\":\"%s\"}", enrollId, text);

                    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
                    m_results.emplace_back(text);
                    xSemaphoreGive(m_res_mutex);
                }
                if (m_result_cb) {
                    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
                    m_result_cb(json, latest_image_with_a_face, latest_face_cropped);
                    xSemaphoreGive(m_res_mutex);
                }
            }
        }
        if (event_bits & DELETE) {
            esp_err_t ret = m_recognizer->delete_last_feat();
            char *text = new char[64];
            char *json = new char[128];
            int deletedId = 0;
            if (ret == ESP_FAIL) {
                strcpy(text, "Failed to delete.");
            } else {
                deletedId = m_recognizer->get_num_feats() + 1; // The ID of the deleted face is the current number of faces + 1
                snprintf(text, 64, "id: %d deleted.", deletedId);
            }
            snprintf(json, 128, "{\"action\":\"delete\",\"id\":%d,\"message\":\"%s\"}", deletedId, text);


            xSemaphoreTake(m_res_mutex, portMAX_DELAY);
            m_results.emplace_back(text);
            xSemaphoreGive(m_res_mutex);
            if (m_result_cb) {
                m_result_cb(json, latest_image_with_a_face, latest_face_cropped);
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

void WhoRecognition::virtual_btn_event_handler(event_type_t event)
{
    EventGroupHandle_t event_group = get_event_group();
    EventBits_t event_bits = xEventGroupGetBits(event_group);
    if (event == RECOGNIZE) {
        // Activate autorecognize whenever there is a face detected
        printf("Autorecognize activated\n");
        m_detect->set_subscriptor_event_group(event_group, RECOGNIZE);
    } else {
        // Deactivate autorecognize when not recognizing solicited
        printf("Autorecognize deactivated\n");
        m_detect->set_subscriptor_event_group(nullptr, RECOGNIZE);
    }
    if (event_bits & BLOCKING && !(event_bits & PAUSE)) {
        xEventGroupSetBits(event_group, event);
    }
}

void WhoRecognition::iot_btn_event_handler(void *button_handle, void *usr_data)
{
    user_data_t *user_data = reinterpret_cast<user_data_t *>(usr_data);
    EventGroupHandle_t event_group = user_data->who_rec_ptr->get_event_group();
    EventBits_t event_bits = xEventGroupGetBits(event_group);

    if (user_data->event == RECOGNIZE) {
        // Activate autorecognize whenever there is a face detected
        printf("Autorecognize activated\n");
        user_data->who_rec_ptr->m_detect->set_subscriptor_event_group(event_group, RECOGNIZE);
    } else {
        // Deactivate autorecognize when not recognizing solicited
        printf("Autorecognize deactivated\n");
        user_data->who_rec_ptr->m_detect->set_subscriptor_event_group(nullptr, RECOGNIZE);
    }

    if (event_bits & BLOCKING && !(event_bits & PAUSE)) {
        xEventGroupSetBits(event_group, user_data->event);
    }
}

void WhoRecognition::new_result_subscription(const std::function<void(result_t, dl::image::img_t, dl::image::img_t)> &cb)
{
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
    ESP_ERROR_CHECK(iot_button_register_cb(
        btns[2], BUTTON_DOUBLE_CLICK, nullptr, iot_btn_event_handler, (void *)(m_btn_user_data + 1)));

    // down  delete
    ESP_ERROR_CHECK(iot_button_register_cb(
        btns[2], BUTTON_LONG_PRESS_START, nullptr, iot_btn_event_handler, (void *)(m_btn_user_data + 2)));

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

// Crops an RGB888 image buffer
void copy_img(const dl::image::img_t &src, std::list<dl::detect::result_t> &detect_res, dl::image::img_t &dst, bool cropped)
{
    if (detect_res.empty()) {
        ESP_LOGW("CROP", "Failed to crop. No face detected.");
        return;
    }

    int x1 = INT_MAX;//face.box[0];
    int y1 = INT_MAX;//face.box[1];
    int x2 = 0;//face.box[2];
    int y2 = 0;//face.box[3];

    // Expand bounding box to include all faces
    for (const auto &face : detect_res) {
        x1 = std::min(x1, face.box[0]);
        y1 = std::min(y1, face.box[1]);
        x2 = std::max(x2, face.box[2]);
        y2 = std::max(y2, face.box[3]);
    }

    int crop_w = x2 - x1;
    int crop_h = y2 - y1;
    if (crop_h <= 0 || crop_w <= 0) {
        ESP_LOGE("WhoRecognition", "Invalid crop dimensions: width=%d, height=%d", crop_w, crop_h);
        if (crop_h <= 0) {
            crop_h = y1 - y2;
        }
        if (crop_w <= 0) {
            crop_w = x1 - x2;
        }
    }

    if (dst.data) {
        free(dst.data);
    }
    memset(&dst, 0, sizeof(dl::image::img_t));

    if (cropped) {
        // Ensure the crop does not exceed the source image dimensions
        x1 = std::max(0, x1);
        y1 = std::max(0, y1);
        crop_w = std::min(crop_w, src.width - x1);
        crop_h = std::min(crop_h, src.height - y1);
    } else {
        // If not cropping, use the full image dimensions
        x1 = 0;
        y1 = 0;
        crop_w = src.width;
        crop_h = src.height;
    }
    dst.width = crop_w;
    dst.height = crop_h;
    dst.pix_type = src.pix_type;
    size_t pixel_size =
        get_img_byte_size(dst) / (dst.width * dst.height); // For RGB888 is 3 bytes per pixel

    dst.data = (uint8_t *)malloc(get_img_byte_size(dst));

    for (int y = 0; y < crop_h; ++y) {
        memcpy(dst.data + y * crop_w * pixel_size,
               src.data + ((y + y1) * src.width + x1) * pixel_size,
               crop_w * pixel_size);
    }
}

} // namespace recognition
} // namespace who
