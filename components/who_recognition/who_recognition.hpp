#pragma once
#include "human_face_detect.hpp"
#include "human_face_recognition.hpp"
#include "who_cam.hpp"
#include "who_detect_lcd.hpp"
#include "who_subscriber.hpp"
#include "bsp/esp-bsp.h"

namespace who {
namespace recognition {
class WhoRecognition;
class WhoDetectLCD : public detect::WhoDetectLCD {
    friend class WhoRecognition;

public:
    WhoDetectLCD(frame_cap::WhoFrameCap *frame_cap,
                 dl::detect::Detect *detect,
                 const std::string &name,
                 const std::vector<std::vector<uint8_t>> &palette);
    detect::WhoDetectBase::result_t get_result();

private:
    void on_new_detect_result(const result_t &result) override;
    SemaphoreHandle_t m_res_mutex;
    result_t m_result;
};

class WhoRecognition : public WhoSubscriber {
public:
    using result_t = char *;
    typedef struct {
        WhoRecognition *who_rec_ptr;
        event_type_t event;
    } user_data_t;

    WhoRecognition(WhoDetectLCD *detect, HumanFaceRecognizer *recognizer, const std::string &name) :
        WhoSubscriber(name), m_detect(detect), m_recognizer(recognizer), m_res_mutex(xSemaphoreCreateMutex())
    {
        m_detect->m_frame_cap->add_element(this);
        m_btn_user_data = new user_data_t[3];
        m_btn_user_data[0] = {this, RECOGNIZE};
        m_btn_user_data[1] = {this, ENROLL};
        m_btn_user_data[2] = {this, DELETE};
    }

    ~WhoRecognition()
    {
        vSemaphoreDelete(m_res_mutex);
        delete[] m_btn_user_data;
    }

    void task() override;
    void lcd_display_cb(who::cam::cam_fb_t *fb) override;

private:
    static void lvgl_btn_event_handler(lv_event_t *e);
    static void iot_btn_event_handler(void *button_handle, void *usr_data);
    void create_btns();
    void create_label();

    WhoDetectLCD *m_detect;
    HumanFaceRecognizer *m_recognizer;

    SemaphoreHandle_t m_res_mutex;
    std::list<result_t> m_results;
    lv_obj_t *m_label;
    user_data_t *m_btn_user_data;
};
} // namespace recognition
} // namespace who
