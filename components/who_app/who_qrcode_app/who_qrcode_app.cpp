#include "who_qrcode_app.hpp"
#include "who_yield2idle.hpp"

namespace who {
namespace app {
bool WhoQRCodeAppBase::run()
{
    who::WhoYield2Idle::run();
    bool ret = m_frame_cap->run(4096, 2, 0);
    return ret & m_qrcode->run(25600, 2, 1);
}

WhoQRCodeAppLCD::WhoQRCodeAppLCD()
{
    m_frame_cap = new frame_cap::WhoFrameCapLCD("FrameCapLCD");
    m_qrcode = new qrcode::WhoQRCodeLCD(m_frame_cap, "QRCodeLCD");
    add_element(m_frame_cap);
    add_element(m_qrcode);
}

WhoQRCodeAppTerm::WhoQRCodeAppTerm()
{
    m_frame_cap = new frame_cap::WhoFrameCap("FrameCap");
    m_qrcode = new qrcode::WhoQRCodeTerm(m_frame_cap, "QRCodeTerm");
    add_element(m_frame_cap);
    add_element(m_qrcode);
}
} // namespace app
} // namespace who
