#pragma once
#include "who_frame_lcd_disp.hpp"
#include "who_qrcode_app_term.hpp"
#include "who_text_result_handle.hpp"

namespace who {
namespace app {
class WhoQRCodeAppLCD : public WhoQRCodeAppTerm {
public:
    WhoQRCodeAppLCD(frame_cap::WhoFrameCap *frame_cap);
    ~WhoQRCodeAppLCD();
    bool run() override;

protected:
    virtual void qrcode_result_cb(const std::string &result);
    virtual void lcd_disp_cb(who::cam::cam_fb_t *fb);
    virtual void cleanup();

private:
    lcd_disp::WhoFrameLCDDisp *m_lcd_disp;
    lcd_disp::WhoTextResultLCDDisp *m_result_lcd_disp;
    lv_obj_t *m_label;
};
} // namespace app
} // namespace who
