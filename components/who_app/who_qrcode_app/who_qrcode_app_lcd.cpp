#include "who_qrcode_app_lcd.hpp"
#include "who_lvgl_utils.hpp"
#include "who_yield2idle.hpp"
LV_FONT_DECLARE(montserrat_bold_20);
LV_FONT_DECLARE(montserrat_bold_26);

namespace who {
namespace app {
WhoQRCodeAppLCD::WhoQRCodeAppLCD(frame_cap::WhoFrameCap *frame_cap) :
    WhoQRCodeAppTerm(frame_cap), m_lcd_disp(new lcd_disp::WhoFrameLCDDisp("LCDDisp", frame_cap->get_last_node()))
{
    WhoApp::add_task(m_lcd_disp);
    m_lcd_disp->set_lcd_disp_cb(std::bind(&WhoQRCodeAppLCD::lcd_disp_cb, this, std::placeholders::_1));

    bsp_display_lock(0);
#if CONFIG_IDF_TARGET_ESP32S3
    m_label = create_lvgl_label("", &montserrat_bold_20);
    int disp_n_frames = 60;
#elif CONFIG_IDF_TARGET_ESP32P4
    m_label = create_lvgl_label("", &montserrat_bold_26);
    int disp_n_frames = 30;
#endif
    const lv_font_t *font = lv_obj_get_style_text_font(m_label, LV_PART_MAIN);
    lv_obj_set_width(m_label, BSP_LCD_H_RES - 2 * font->line_height);
    lv_obj_align(m_label, LV_ALIGN_TOP_LEFT, font->line_height, font->line_height);
    lv_label_set_long_mode(m_label, LV_LABEL_LONG_WRAP);
    bsp_display_unlock();

    m_result_lcd_disp = new lcd_disp::WhoTextResultLCDDisp(m_qrcode, m_label, disp_n_frames);
    m_qrcode->set_cleanup_func(std::bind(&WhoQRCodeAppLCD::cleanup, this));
}

WhoQRCodeAppLCD::~WhoQRCodeAppLCD()
{
    delete m_result_lcd_disp;
    bsp_display_lock(0);
    lv_obj_del(m_label);
    bsp_display_unlock();
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

void WhoQRCodeAppLCD::qrcode_result_cb(const std::string &result)
{
    WhoQRCodeAppTerm::qrcode_result_cb(result);
    m_result_lcd_disp->save_text_result(result);
}

void WhoQRCodeAppLCD::lcd_disp_cb(who::cam::cam_fb_t *fb)
{
    m_result_lcd_disp->lcd_disp_cb(fb);
}

void WhoQRCodeAppLCD::cleanup()
{
    m_result_lcd_disp->cleanup();
}
} // namespace app
} // namespace who
