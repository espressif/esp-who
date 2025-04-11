#include "who_detect_app.hpp"

namespace who {
namespace app {
bool WhoDetectApp::run()
{
    bool ret = m_frame_cap->run(4096, 2, 0);
    return ret & m_detect->run(2560, 2, 1);
}

WhoDetectLCDApp::WhoDetectLCDApp(const std::string &name, const std::vector<std::vector<uint8_t>> &palette)
{
    m_frame_cap = new frame_cap::WhoFrameCapLCD(name + "_cap");
    m_detect = new detect::WhoDetectLCD(m_frame_cap, name + "_det", palette);
    add_element(m_frame_cap);
    add_element(m_detect);
}

WhoDetectTerminalApp::WhoDetectTerminalApp(const std::string &name)
{
    m_frame_cap = new frame_cap::WhoFrameCap(name + "_cap");
    m_detect = new detect::WhoDetectTerminal(m_frame_cap, name + "_det");
    add_element(m_frame_cap);
    add_element(m_detect);
}

} // namespace app
} // namespace who
