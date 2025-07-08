#pragma once
#include "who_recognition_app_base.hpp"

namespace who {
namespace app {
class WhoRecognitionAppTerm : public WhoRecognitionAppBase {
public:
    WhoRecognitionAppTerm(frame_cap::WhoFrameCap *frame_cap);
    ~WhoRecognitionAppTerm();
    bool run() override;

protected:
    virtual void recognition_result_cb(const std::string &result);

private:
    button::WhoRecognitionButton *m_recognition_button;
};
} // namespace app
} // namespace who
