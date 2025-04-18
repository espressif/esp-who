#pragma once
#include "who_detect_lcd.hpp"
#include "who_detect_term.hpp"
#include "who_frame_cap.hpp"

namespace who {
namespace app {
class WhoDetectAppBase : public WhoTasks {
public:
    void set_cam(cam::WhoCam *cam) { m_frame_cap->set_cam(cam); }
    void set_model(dl::detect::Detect *model) { m_detect->set_model(model); }
    void set_fps(float fps) { m_detect->set_fps(fps); }
    bool run() override;

protected:
    frame_cap::WhoFrameCap *m_frame_cap;
    detect::WhoDetectBase *m_detect;
};

class WhoDetectAppLCD : public WhoDetectAppBase {
public:
    WhoDetectAppLCD(const std::vector<std::vector<uint8_t>> &palette);
    void set_lcd(lcd::WhoLCD *lcd) { static_cast<frame_cap::WhoFrameCapLCD *>(m_frame_cap)->set_lcd(lcd); }
};

class WhoDetectAppTerm : public WhoDetectAppBase {
public:
    WhoDetectAppTerm();
};
} // namespace app
} // namespace who
