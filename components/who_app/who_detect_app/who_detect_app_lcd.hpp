#pragma once
#include "who_detect_app_base.hpp"
#include "who_detect_result_handle.hpp"
#include "who_frame_lcd_disp.hpp"

namespace who {
namespace app {
class WhoDetectAppLCD : public WhoDetectAppBase {
public:
    WhoDetectAppLCD(const std::vector<std::vector<uint8_t>> &palette,
                    frame_cap::WhoFrameCap *frame_cap,
                    frame_cap::WhoFrameCapNode *lcd_disp_frame_cap_node = nullptr);
    ~WhoDetectAppLCD();
    bool run() override;

protected:
    virtual void detect_result_cb(const detect::WhoDetect::result_t &result);
    virtual void lcd_disp_cb(who::cam::cam_fb_t *fb);
    virtual void cleanup();

private:
    lcd_disp::WhoFrameLCDDisp *m_lcd_disp;
    lcd_disp::WhoDetectResultLCDDisp *m_result_lcd_disp;
};
} // namespace app
} // namespace who
