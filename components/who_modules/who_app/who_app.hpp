#pragma once
#include "human_face_detect.hpp"
#include "human_face_recognition.hpp"
#include "pedestrian_detect.hpp"
#include "who_lcd.hpp"

namespace who {
namespace cam {
class Cam;
}
namespace app {
class WhoDetect {
public:
    WhoDetect(dl::detect::Detect *detect, who::cam::Cam *cam, who::lcd::result_type_t type) :
        m_detect(detect), m_cam(cam), m_type(type) {};
    void run();

private:
    static void task(void *args);
    dl::detect::Detect *m_detect;
    who::cam::Cam *m_cam;
    who::lcd::result_type_t m_type;
};

class WhoHumanFaceRecognition {
public:
    WhoHumanFaceRecognition(HumanFaceDetect *detect, HumanFaceRecognizer *recognizer, who::cam::Cam *cam) :
        m_detect(detect), m_recognizer(recognizer), m_cam(cam)
    {
    }
    enum class fr_event_t { RECOGNIZE = 1 << 0, ENROLL = 1 << 1, DELETE = 1 << 2 };
    enum class fr_status_t { RECOGNIZE, ENROLL, DELETE, DETECT };
    void run();
    static TaskHandle_t s_task_handle;
    static lv_obj_t *s_label;

private:
    static void event_handle_task(void *args);
    static void recognition_task(void *args);
    static void lvgl_btn_event_handler(lv_event_t *e);
    static void iot_btn_event_handler(void *button_handle, void *usr_data);
    static void btn_event_handler(fr_event_t fr_event);
    static void create_btns();
    static void create_label();
    HumanFaceDetect *m_detect;
    HumanFaceRecognizer *m_recognizer;
    who::cam::Cam *m_cam;
    static SemaphoreHandle_t s_mutex;
    fr_status_t m_status = fr_status_t::DETECT;
};
} // namespace app
} // namespace who
