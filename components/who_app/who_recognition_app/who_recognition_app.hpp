#pragma once
#include "who_frame_cap.hpp"
#include "who_recognition.hpp"

namespace who {
namespace app {
class WhoRecognitionApp : public WhoTasks {
public:
    WhoRecognitionApp(const std::string &name);
    void set_cam(cam::WhoCam *cam) { m_frame_cap->set_cam(cam); }
    bool run() override;

protected:
    frame_cap::WhoFrameCap *m_frame_cap;
    recognition::WhoDetectLCD *m_detect;
    recognition::WhoRecognition *m_recognition;
};
} // namespace app
} // namespace who
