#pragma once
#include "human_face_detect.hpp"
#include "human_face_recognition.hpp"

namespace who {
namespace cam {
class Cam;
}
namespace app {
class WhoHumanFaceDetect {
public:
    WhoHumanFaceDetect(HumanFaceDetect *detect, who::cam::Cam *cam) : m_detect(detect), m_cam(cam) {};
    void run();

private:
    static void task(void *args);
    HumanFaceDetect *m_detect;
    who::cam::Cam *m_cam;
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
    static void btn_event_handler(lv_event_t *e);
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
