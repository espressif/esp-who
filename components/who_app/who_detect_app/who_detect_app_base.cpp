#include "who_detect_app_base.hpp"

namespace who {
namespace app {
WhoDetectAppBase::WhoDetectAppBase(frame_cap::WhoFrameCap *frame_cap) :
    m_frame_cap(frame_cap), m_detect(new detect::WhoDetect("Detect", m_frame_cap->get_last_node()))
{
    WhoApp::add_task_group(frame_cap);
    WhoApp::add_task(m_detect);
}

void WhoDetectAppBase::set_model(dl::detect::Detect *model)
{
    m_detect->set_model(model);
}

void WhoDetectAppBase::set_fps(float fps)
{
    m_detect->set_fps(fps);
}
} // namespace app
} // namespace who
