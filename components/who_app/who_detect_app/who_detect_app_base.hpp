#pragma once
#include "who_app.hpp"
#include "who_detect.hpp"

namespace who {
namespace app {
class WhoDetectAppBase : public WhoApp {
public:
    WhoDetectAppBase(frame_cap::WhoFrameCap *frame_cap);
    // inject model after constructor, make it possible to create model after other resources are requested.
    void set_model(dl::detect::Detect *model);
    void set_fps(float fps);

protected:
    frame_cap::WhoFrameCap *m_frame_cap;
    detect::WhoDetect *m_detect;
};
} // namespace app
} // namespace who
