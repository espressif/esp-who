#pragma once
#include "cam.hpp"
#include "human_face_detect.hpp"
#include "human_face_recognition.hpp"

namespace who {
namespace app {

class WhoHumanFaceRecognition {
public:
    typedef struct {
        std::list<dl::detect::result_t> det_res;
        struct timeval timestamp;
    } det_result_t;
    using rec_result_t = char *;

    WhoHumanFaceRecognition(HumanFaceDetect *detect, HumanFaceRecognizer *recognizer, who::cam::Cam *cam) :
        m_status_mutex(xSemaphoreCreateMutex()),
        m_det_res_mutex(xSemaphoreCreateMutex()),
        m_rec_res_mutex(xSemaphoreCreateMutex()),
        m_detect(detect),
        m_recognizer(recognizer),
        m_cam(cam)
    {
    }
    enum class fr_event_t { RECOGNIZE = 1 << 0, ENROLL = 1 << 1, DELETE = 1 << 2 };
    enum class fr_status_t { RECOGNIZE, ENROLL, DELETE, DETECT };
    void run();
    void display(who::cam::cam_fb_t *fb);
    static TaskHandle_t s_task_handle;

private:
    static void event_handle_task(void *args);
    static void recognition_task(void *args);
    static void lvgl_btn_event_handler(lv_event_t *e);
    static void iot_btn_event_handler(void *button_handle, void *usr_data);
    static void btn_event_handler(fr_event_t fr_event);
    void create_btns();
    void create_label();
    SemaphoreHandle_t m_status_mutex;
    SemaphoreHandle_t m_det_res_mutex;
    SemaphoreHandle_t m_rec_res_mutex;
    HumanFaceDetect *m_detect;
    HumanFaceRecognizer *m_recognizer;
    who::cam::Cam *m_cam;
    fr_status_t m_status = fr_status_t::DETECT;
    lv_obj_t *m_label;
    std::queue<det_result_t> m_det_results;
    std::list<rec_result_t> m_rec_results;
};
} // namespace app
} // namespace who
