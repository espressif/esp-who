#include "who_detect_app_lcd.hpp"
#include "who_yield2idle.hpp"

namespace who {
namespace app {
WhoDetectAppLCD::WhoDetectAppLCD(const std::vector<std::vector<uint8_t>> &palette,
                                 frame_cap::WhoFrameCap *frame_cap,
                                 frame_cap::WhoFrameCapNode *lcd_disp_frame_cap_node) :
    WhoDetectAppBase(frame_cap)
{
    if (!lcd_disp_frame_cap_node) {
        lcd_disp_frame_cap_node = frame_cap->get_last_node();
    }
    m_lcd_disp = new lcd_disp::WhoFrameLCDDisp("LCDDisp", lcd_disp_frame_cap_node);
    WhoApp::add_task(m_lcd_disp);
    m_lcd_disp->set_lcd_disp_cb(std::bind(&WhoDetectAppLCD::lcd_disp_cb, this, std::placeholders::_1));
#if !BSP_CONFIG_NO_GRAPHIC_LIB
    m_result_lcd_disp = new lcd_disp::WhoDetectResultLCDDisp(m_detect, palette, m_lcd_disp->get_canvas());
#else
    m_result_lcd_disp = new lcd_disp::WhoDetectResultLCDDisp(m_detect, palette);
#endif
    m_detect->set_detect_result_cb(std::bind(&WhoDetectAppLCD::detect_result_cb, this, std::placeholders::_1));
    m_detect->set_cleanup_func(std::bind(&WhoDetectAppLCD::cleanup, this));

    auto detect_frame_cap_node = frame_cap->get_last_node();
#if CONFIG_SOC_PPA_SUPPORTED
    if (lcd_disp_frame_cap_node != detect_frame_cap_node) {
        if (detect_frame_cap_node->get_type() == "PPAResizeNode" &&
            detect_frame_cap_node->get_prev_node() == lcd_disp_frame_cap_node) {
            float rescale_x = dl::image::get_ppa_scale(lcd_disp_frame_cap_node->get_fb_width(),
                                                       detect_frame_cap_node->get_fb_width());
            float rescale_y = dl::image::get_ppa_scale(lcd_disp_frame_cap_node->get_fb_height(),
                                                       detect_frame_cap_node->get_fb_height());
            m_detect->set_rescale_params(rescale_x,
                                         rescale_y,
                                         lcd_disp_frame_cap_node->get_fb_width(),
                                         lcd_disp_frame_cap_node->get_fb_height());
        } else {
            ESP_LOGE("WhoDetectAppLCD", "Wrong frame cap node.");
        }
    }
#else
    assert(lcd_disp_frame_cap_node == detect_frame_cap_node);
#endif
}

WhoDetectAppLCD::~WhoDetectAppLCD()
{
    delete m_result_lcd_disp;
}

bool WhoDetectAppLCD::run()
{
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_lcd_disp->run(2560, 2, 0);
    ret &= m_detect->run(2560, 2, 1);
    return ret;
}

void WhoDetectAppLCD::detect_result_cb(const detect::WhoDetect::result_t &result)
{
    m_result_lcd_disp->save_detect_result(result);
}

void WhoDetectAppLCD::lcd_disp_cb(who::cam::cam_fb_t *fb)
{
    m_result_lcd_disp->lcd_disp_cb(fb);
}

void WhoDetectAppLCD::cleanup()
{
    m_result_lcd_disp->cleanup();
}
} // namespace app
} // namespace who
