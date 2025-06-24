#include "who_detect_app.hpp"
#include "who_yield2idle.hpp"

static const char *TAG = "WhoDetectAppLCD";
namespace who {
namespace app {
WhoDetectAppLCD::WhoDetectAppLCD(frame_cap::WhoFrameCap *frame_cap,
                                 frame_cap::WhoFrameCapNode *lcd_disp_frame_cap_node) :
    m_frame_cap(frame_cap), m_detect(nullptr)
{
    if (!lcd_disp_frame_cap_node) {
        lcd_disp_frame_cap_node = frame_cap->get_last_node();
    }
    m_lcd_disp = new lcd_disp::WhoLCDDisp("LCDDisp", lcd_disp_frame_cap_node);
    WhoApp::add_task(m_lcd_disp);
    WhoApp::add_task_group(m_frame_cap);
}

void WhoDetectAppLCD::set_model(dl::detect::Detect *model, const std::vector<std::vector<uint8_t>> &palette)
{
    m_detect = new detect::WhoDetectLCD("Detect", m_frame_cap->get_last_node(), m_lcd_disp, model, palette);
    WhoApp::add_task(m_detect);
}

bool WhoDetectAppLCD::run()
{
    if (!m_detect) {
        ESP_LOGE(TAG, "Detect model is not set, call set_model() first before run.");
        return false;
    }
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_lcd_disp->run(2560, 2, 0);
    ret &= m_detect->run(2560, 2, 1);
    return ret;
}

WhoDetectAppTerm::WhoDetectAppTerm(frame_cap::WhoFrameCap *frame_cap) : m_frame_cap(frame_cap), m_detect(nullptr)
{
    WhoApp::add_task_group(frame_cap);
}

void WhoDetectAppTerm::set_model(dl::detect::Detect *model)
{
    m_detect = new detect::WhoDetectTerm("Detect", m_frame_cap->get_last_node(), model);
    WhoApp::add_task(m_detect);
}

bool WhoDetectAppTerm::run()
{
    if (!m_detect) {
        ESP_LOGE(TAG, "Detect model is not set, call set_model() first before run.");
        return false;
    }
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_detect->run(2560, 2, 1);
    return ret;
}
} // namespace app
} // namespace who
