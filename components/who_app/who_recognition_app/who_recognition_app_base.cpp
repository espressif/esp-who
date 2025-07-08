#include "who_recognition_app_base.hpp"

namespace who {
namespace app {
WhoRecognitionAppBase::WhoRecognitionAppBase(frame_cap::WhoFrameCap *frame_cap) :
    m_frame_cap(frame_cap), m_recognition(new recognition::WhoRecognition(frame_cap->get_last_node()))
{
    WhoApp::add_task_group(m_frame_cap);
    WhoApp::add_task_group(m_recognition);
}
} // namespace app
} // namespace who
