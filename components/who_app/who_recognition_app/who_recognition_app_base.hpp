#pragma once
#include "who_app.hpp"
#include "who_recognition.hpp"
#include "who_recognition_button.hpp"

namespace who {
namespace app {
class WhoRecognitionAppBase : public WhoApp {
public:
    WhoRecognitionAppBase(frame_cap::WhoFrameCap *frame_cap);

protected:
    frame_cap::WhoFrameCap *m_frame_cap;
    recognition::WhoRecognition *m_recognition;
};
} // namespace app
} // namespace who
