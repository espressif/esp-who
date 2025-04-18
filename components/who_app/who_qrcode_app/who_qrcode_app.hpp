#pragma once
#include "who_frame_cap.hpp"
#include "who_qrcode_lcd.hpp"
#include "who_qrcode_term.hpp"

namespace who {
namespace app {
class WhoQRCodeAppBase : public WhoTasks {
public:
    void set_cam(cam::WhoCam *cam) { m_frame_cap->set_cam(cam); }
    bool run() override;

protected:
    frame_cap::WhoFrameCap *m_frame_cap;
    qrcode::WhoQRCodeBase *m_qrcode;
};

class WhoQRCodeAppLCD : public WhoQRCodeAppBase {
public:
    WhoQRCodeAppLCD();
    void set_lcd(lcd::WhoLCD *lcd) { static_cast<frame_cap::WhoFrameCapLCD *>(m_frame_cap)->set_lcd(lcd); }
};

class WhoQRCodeAppTerm : public WhoQRCodeAppBase {
public:
    WhoQRCodeAppTerm();
};

} // namespace app
} // namespace who
