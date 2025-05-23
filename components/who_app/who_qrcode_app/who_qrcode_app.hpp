#pragma once
#include "who_app.hpp"
#include "who_qrcode_lcd.hpp"
#include "who_qrcode_term.hpp"

namespace who {
namespace app {
class WhoQRCodeAppLCD : public WhoApp {
public:
    WhoQRCodeAppLCD(frame_cap::WhoFrameCap *frame_cap, frame_cap::WhoFrameCapNode *lcd_disp_frame_cap_node = nullptr);
    bool run() override;

private:
    lcd_disp::WhoLCDDisp *m_lcd_disp;
    frame_cap::WhoFrameCap *m_frame_cap;
    qrcode::WhoQRCodeLCD *m_qrcode;
};

class WhoQRCodeAppTerm : public WhoApp {
public:
    WhoQRCodeAppTerm(frame_cap::WhoFrameCap *frame_cap);
    bool run() override;

private:
    frame_cap::WhoFrameCap *m_frame_cap;
    qrcode::WhoQRCodeTerm *m_qrcode;
};

} // namespace app
} // namespace who
