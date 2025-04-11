#pragma once
#include "who_detect_lcd.hpp"
#include "who_detect_terminal.hpp"
#include "who_frame_cap.hpp"

namespace who {
namespace app {
class WhoDetectApp : public WhoTasks {
public:
    void set_cam(cam::WhoCam *cam) { m_frame_cap->set_cam(cam); }
    void set_model(dl::detect::Detect *model) { m_detect->set_model(model); }
    void set_fps(float fps) { m_detect->set_fps(fps); }
    bool run() override;

protected:
    frame_cap::WhoFrameCap *m_frame_cap;
    detect::WhoDetectBase *m_detect;
};

class WhoDetectLCDApp : public WhoDetectApp {
public:
    WhoDetectLCDApp(const std::string &name, const std::vector<std::vector<uint8_t>> &palette);
};

class WhoDetectTerminalApp : public WhoDetectApp {
public:
    WhoDetectTerminalApp(const std::string &name);
};
} // namespace app
} // namespace who
