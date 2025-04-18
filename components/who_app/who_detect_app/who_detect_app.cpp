#include "who_detect_app.hpp"
#include "who_yield2idle.hpp"

namespace who {
namespace app {
bool WhoDetectAppBase::run()
{
    who::WhoYield2Idle::run();
    bool ret = m_frame_cap->run(4096, 2, 0);
    return ret & m_detect->run(2560, 2, 1);
}

WhoDetectAppLCD::WhoDetectAppLCD(const std::vector<std::vector<uint8_t>> &palette)
{
    m_frame_cap = new frame_cap::WhoFrameCapLCD("FrameCapLCD");
    m_detect = new detect::WhoDetectLCD(m_frame_cap, "DetectLCD", palette);
    add_element(m_frame_cap);
    add_element(m_detect);
}

WhoDetectAppTerm::WhoDetectAppTerm()
{
    m_frame_cap = new frame_cap::WhoFrameCap("FrameCap");
    m_detect = new detect::WhoDetectTerm(m_frame_cap, "DetectTerm");
    add_element(m_frame_cap);
    add_element(m_detect);
}

} // namespace app
} // namespace who
