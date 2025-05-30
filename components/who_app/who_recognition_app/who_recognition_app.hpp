#pragma once
#include "who_frame_cap.hpp"
#include "who_recognition.hpp"

namespace who {
namespace app {
class WhoRecognitionApp : public WhoTasks {
public:
    WhoRecognitionApp();
    void set_cam(cam::WhoCam *cam) { m_frame_cap->set_cam(cam); }
    void set_lcd(lcd::WhoLCDiface *lcd) { m_frame_cap->set_lcd(lcd); }
    bool run() override;

protected:
    frame_cap::WhoFrameCapLCD *m_frame_cap;
    recognition::WhoDetectLCD *m_detect;
    recognition::WhoRecognition *m_recognition;
};
} // namespace app
} // namespace who
