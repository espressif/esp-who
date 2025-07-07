#include "who_recognition_button.hpp"
#include "who_lvgl_utils.hpp"
LV_FONT_DECLARE(montserrat_bold_26);

namespace who {
namespace button {
WhoRecognitionButton::WhoRecognitionButton(WhoTask *task)
{
    m_btn_user_data = new btn_user_data_t[3];
    m_btn_user_data[0] = {task, RECOGNIZE};
    m_btn_user_data[1] = {task, ENROLL};
    m_btn_user_data[2] = {task, DELETE};
}

WhoRecognitionButton::~WhoRecognitionButton()
{
    delete[] m_btn_user_data;
}

#if BSP_CAPS_BUTTONS
WhoRecognitionButtonPhysical::WhoRecognitionButtonPhysical(recognition::WhoRecognitionCore *recognition) :
    WhoRecognitionButton(recognition)
{
    m_btns = new button_handle_t[BSP_BUTTON_NUM];
    ESP_ERROR_CHECK(bsp_iot_button_create(m_btns, NULL, BSP_BUTTON_NUM));
#ifdef BSP_BOARD_ESP32_S3_EYE
    int recognize = BSP_BUTTON_PLAY;
    int enroll = BSP_BUTTON_UP;
    int del = BSP_BUTTON_DOWN;
#elif defined(BSP_BOARD_ESP32_S3_KORVO_2)
    int recognize = BSP_BUTTON_PLAY;
    int enroll = BSP_BUTTON_VOLUP;
    int del = BSP_BUTTON_VOLDOWN;
#else
    int recognize = 0;
    int enroll = 1;
    int del = 2;
#endif
    // play  recognize
    ESP_ERROR_CHECK(iot_button_register_cb(
        m_btns[recognize], BUTTON_SINGLE_CLICK, nullptr, btn_event_handler, (void *)m_btn_user_data));
    // up    enroll
    ESP_ERROR_CHECK(iot_button_register_cb(
        m_btns[enroll], BUTTON_SINGLE_CLICK, nullptr, btn_event_handler, (void *)(m_btn_user_data + 1)));
    // down  delete
    ESP_ERROR_CHECK(iot_button_register_cb(
        m_btns[del], BUTTON_SINGLE_CLICK, nullptr, btn_event_handler, (void *)(m_btn_user_data + 2)));
}

WhoRecognitionButtonPhysical::~WhoRecognitionButtonPhysical()
{
    for (int i = 0; i < BSP_BUTTON_NUM; i++) {
        iot_button_delete(m_btns[i]);
    }
    delete[] m_btns;
}

void WhoRecognitionButtonPhysical::btn_event_handler(void *button_handle, void *usr_data)
{
    btn_user_data_t *user_data = reinterpret_cast<btn_user_data_t *>(usr_data);
    if (user_data->task->is_active()) {
        xEventGroupSetBits(user_data->task->get_event_group(), user_data->event);
    }
}
#endif

WhoRecognitionButtonLVGL::WhoRecognitionButtonLVGL(recognition::WhoRecognitionCore *recognition) :
    WhoRecognitionButton(recognition)
{
    bsp_display_lock(0);
    m_btn_recognize = create_lvgl_btn("recognize", &montserrat_bold_26);
    m_btn_enroll = create_lvgl_btn("enroll", &montserrat_bold_26);
    m_btn_delete = create_lvgl_btn("delete", &montserrat_bold_26);
    lv_obj_add_event_cb(m_btn_recognize, btn_event_handler, LV_EVENT_CLICKED, (void *)m_btn_user_data);
    lv_obj_add_event_cb(m_btn_enroll, btn_event_handler, LV_EVENT_CLICKED, (void *)(m_btn_user_data + 1));
    lv_obj_add_event_cb(m_btn_delete, btn_event_handler, LV_EVENT_CLICKED, (void *)(m_btn_user_data + 2));
    lv_obj_update_layout(m_btn_recognize);
    lv_obj_update_layout(m_btn_enroll);
    lv_obj_update_layout(m_btn_delete);
    int32_t w = lv_obj_get_width(m_btn_recognize);
    w = std::max(w, lv_obj_get_width(m_btn_enroll));
    w = std::max(w, lv_obj_get_width(m_btn_delete));
    int32_t h = lv_obj_get_height(m_btn_recognize);
    lv_obj_set_size(m_btn_recognize, w, h);
    lv_obj_set_size(m_btn_enroll, w, h);
    lv_obj_set_size(m_btn_delete, w, h);
    int32_t pad = h / 2;
    lv_obj_align(m_btn_recognize, LV_ALIGN_TOP_RIGHT, -pad, pad);
    lv_obj_align(m_btn_enroll, LV_ALIGN_TOP_RIGHT, -pad, pad + h + pad);
    lv_obj_align(m_btn_delete, LV_ALIGN_TOP_RIGHT, -pad, pad + 2 * (h + pad));
    bsp_display_unlock();
}

WhoRecognitionButtonLVGL::~WhoRecognitionButtonLVGL()
{
    bsp_display_lock(0);
    lv_obj_delete(m_btn_recognize);
    lv_obj_delete(m_btn_enroll);
    lv_obj_delete(m_btn_delete);
    bsp_display_unlock();
}

void WhoRecognitionButtonLVGL::btn_event_handler(lv_event_t *e)
{
    btn_user_data_t *user_data = reinterpret_cast<btn_user_data_t *>(lv_event_get_user_data(e));
    if (user_data->task->is_active()) {
        xEventGroupSetBits(user_data->task->get_event_group(), user_data->event);
    }
}

WhoRecognitionButton *get_recognition_button(recognition_button_type_t btn_type,
                                             recognition::WhoRecognitionCore *recognition)
{
    switch (btn_type) {
    case recognition_button_type_t::PHYSICAL:
#if BSP_CAPS_BUTTONS
        return new WhoRecognitionButtonPhysical(recognition);
#else
        ESP_LOGE("RecognitionButton", "Physical button not supported in BSP.");
        return nullptr;
#endif
    case recognition_button_type_t::LVGL:
        return new WhoRecognitionButtonLVGL(recognition);
    default:
        ESP_LOGE("RecognitionButton", "Wrong recognition button type.");
        return nullptr;
    }
}
} // namespace button
} // namespace who
