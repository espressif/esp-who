#pragma once
#include "who_app.hpp"
#include "who_recognition.hpp"

namespace who {
namespace app {
class WhoRecognitionApp : public WhoApp {
public:
    WhoRecognitionApp(frame_cap::WhoFrameCap *frame_cap, frame_cap::WhoFrameCapNode *lcd_disp_frame_cap_node = nullptr);
    bool run() override;

private:
    lcd_disp::WhoLCDDisp *m_lcd_disp;
    frame_cap::WhoFrameCap *m_frame_cap;
    recognition::WhoDetectLCD *m_detect;
    recognition::WhoRecognition *m_recognition;
};
} // namespace app
} // namespace who
