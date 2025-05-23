#pragma once
#include "human_face_detect.hpp"
#include "human_face_recognition.hpp"
#include "who_detect_lcd.hpp"
#include "bsp/esp-bsp.h"

namespace who {
namespace recognition {
class WhoRecognition;
class WhoDetectLCD : public detect::WhoDetectLCD {
public:
    WhoDetectLCD(const std::string &name,
                 frame_cap::WhoFrameCapNode *frame_cap_node,
                 lcd_disp::WhoLCDDisp *lcd_disp,
                 dl::detect::Detect *detect,
                 const std::vector<std::vector<uint8_t>> &palette) :
        detect::WhoDetectLCD(name, frame_cap_node, lcd_disp, detect, palette), m_res_mutex(xSemaphoreCreateMutex())
    {
    }
    ~WhoDetectLCD() { vSemaphoreDelete(m_res_mutex); }
    detect::WhoDetectBase::result_t get_result();

private:
    void cleanup() override;
    void on_new_detect_result(const result_t &result) override;
    SemaphoreHandle_t m_res_mutex;
    result_t m_result;
};

class WhoRecognition : public WhoTask, public lcd_disp::IWhoLCDDisp {
public:
    using result_t = char *;
    typedef struct {
        WhoRecognition *who_rec_ptr;
        event_type_t event;
    } user_data_t;

    WhoRecognition(const std::string &name,
                   lcd_disp::WhoLCDDisp *lcd_disp,
                   WhoDetectLCD *detect,
                   HumanFaceRecognizer *recognizer);
    ~WhoRecognition();

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
