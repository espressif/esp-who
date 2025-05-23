#pragma once
#include "who_app.hpp"
#include "who_detect_lcd.hpp"
#include "who_detect_term.hpp"

namespace who {
namespace app {
class WhoDetectAppLCD : public WhoApp {
public:
    WhoDetectAppLCD(frame_cap::WhoFrameCap *frame_cap, frame_cap::WhoFrameCapNode *lcd_disp_frame_cap_node = nullptr);
    // inject model after constructor, make it possible to create model after other resources are requested.
    void set_model(dl::detect::Detect *model, const std::vector<std::vector<uint8_t>> &palette);
    void set_fps(float fps) { m_detect->set_fps(fps); }
    bool run() override;

private:
    lcd_disp::WhoLCDDisp *m_lcd_disp;
    frame_cap::WhoFrameCap *m_frame_cap;
    detect::WhoDetectLCD *m_detect;
};

class WhoDetectAppTerm : public WhoApp {
public:
    WhoDetectAppTerm(frame_cap::WhoFrameCap *frame_cap);
    // inject model after constructor, make it possible to create model after other resources are requested.
    void set_model(dl::detect::Detect *model);
    void set_fps(float fps) { m_detect->set_fps(fps); }
    bool run() override;

private:
    frame_cap::WhoFrameCap *m_frame_cap;
    detect::WhoDetectTerm *m_detect;
};

} // namespace app
} // namespace who
