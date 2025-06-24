#include "who_qrcode_app.hpp"
#include "who_yield2idle.hpp"

namespace who {
namespace app {
WhoQRCodeAppLCD::WhoQRCodeAppLCD(frame_cap::WhoFrameCap *frame_cap,
                                 frame_cap::WhoFrameCapNode *lcd_disp_frame_cap_node) :
    m_frame_cap(frame_cap)
{
    if (!lcd_disp_frame_cap_node) {
        lcd_disp_frame_cap_node = frame_cap->get_last_node();
    }
    m_lcd_disp = new lcd_disp::WhoLCDDisp("LCDDisp", lcd_disp_frame_cap_node);
    m_qrcode = new qrcode::WhoQRCodeLCD("QRCode", frame_cap->get_last_node(), m_lcd_disp);
    WhoApp::add_task(m_lcd_disp);
    WhoApp::add_task_group(m_frame_cap);
    WhoApp::add_task(m_qrcode);
}

bool WhoQRCodeAppLCD::run()
{
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_lcd_disp->run(2560, 2, 0);
    ret &= m_qrcode->run(25600, 2, 1);
    return ret;
}

WhoQRCodeAppTerm::WhoQRCodeAppTerm(frame_cap::WhoFrameCap *frame_cap) : m_frame_cap(frame_cap)
{
    ;
    m_qrcode = new qrcode::WhoQRCodeTerm("QRCode", frame_cap->get_last_node());
    WhoApp::add_task_group(m_frame_cap);
    WhoApp::add_task(m_qrcode);
}

bool WhoQRCodeAppTerm::run()
{
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_qrcode->run(25600, 2, 1);
    return ret;
}
} // namespace app
} // namespace who
