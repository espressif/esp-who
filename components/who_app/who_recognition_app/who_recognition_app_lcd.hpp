#pragma once
#include "who_detect_result_handle.hpp"
#include "who_frame_lcd_disp.hpp"
#include "who_recognition_app_base.hpp"
#include "who_text_result_handle.hpp"

namespace who {
namespace app {
class WhoRecognitionAppLCD : public WhoRecognitionAppBase {
public:
    WhoRecognitionAppLCD(frame_cap::WhoFrameCap *frame_cap);
    ~WhoRecognitionAppLCD();
    bool run() override;

protected:
    virtual void recognition_result_cb(const std::string &result);
    virtual void detect_result_cb(const detect::WhoDetect::result_t &result);
    virtual void lcd_disp_cb(who::cam::cam_fb_t *fb);
    virtual void recognition_cleanup();
    virtual void detect_cleanup();

private:
    lcd_disp::WhoFrameLCDDisp *m_lcd_disp;
    button::WhoRecognitionButton *m_recognition_button;
    lcd_disp::WhoTextResultLCDDisp *m_text_result_lcd_disp;
    lcd_disp::WhoDetectResultLCDDisp *m_detect_result_lcd_disp;
    lv_obj_t *m_label;
};
} // namespace app
} // namespace who
