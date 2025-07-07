#pragma once
#include "who_detect_app_base.hpp"

namespace who {
namespace app {
class WhoDetectAppTerm : public WhoDetectAppBase {
public:
    WhoDetectAppTerm(frame_cap::WhoFrameCap *frame_cap);
    bool run() override;

protected:
    virtual void detect_result_cb(const detect::WhoDetect::result_t &result);
};
} // namespace app
} // namespace who
