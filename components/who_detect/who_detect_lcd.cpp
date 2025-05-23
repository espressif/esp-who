#include "who_detect_lcd.hpp"
#include "who_detect_utils.hpp"

static const char *TAG = "WhoDetectLCD";

namespace who {
namespace detect {
WhoDetectLCD::WhoDetectLCD(const std::string &name,
                           frame_cap::WhoFrameCapNode *frame_cap_node,
                           lcd_disp::WhoLCDDisp *lcd_disp,
                           dl::detect::Detect *detect,
                           const std::vector<std::vector<uint8_t>> &palette) :
    WhoDetectBase(name, frame_cap_node, detect),
    lcd_disp::IWhoLCDDisp(lcd_disp, this),
    m_res_mutex(xSemaphoreCreateMutex())
{
    auto disp_frame_cap_node = lcd_disp->get_frame_cap_node();
#if CONFIG_SOC_PPA_SUPPORTED
    if (disp_frame_cap_node != frame_cap_node) {
        if (frame_cap_node->get_type() == "PPAResizeNode" && frame_cap_node->get_prev_node() == disp_frame_cap_node) {
            float rescale_x =
                dl::image::get_ppa_scale(disp_frame_cap_node->get_fb_width(), frame_cap_node->get_fb_width());
            float rescale_y =
                dl::image::get_ppa_scale(disp_frame_cap_node->get_fb_height(), frame_cap_node->get_fb_height());
            set_rescale_params(
                rescale_x, rescale_y, disp_frame_cap_node->get_fb_width(), disp_frame_cap_node->get_fb_height());
        } else {
            ESP_LOGE(TAG, "Wrong frame cap node.");
        }
    }
#else
    assert(disp_frame_cap_node == frame_cap_node);
#endif
#if BSP_CONFIG_NO_GRAPHIC_LIB
    m_palette = palette;
#else
    m_palette = cvt_to_lv_palette(palette);
#endif
}

WhoDetectLCD::~WhoDetectLCD()
{
    vSemaphoreDelete(m_res_mutex);
}

void WhoDetectLCD::on_new_detect_result(const result_t &result)
{
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    m_results.push(result);
    xSemaphoreGive(m_res_mutex);
}

void WhoDetectLCD::lcd_display_cb(who::cam::cam_fb_t *fb)
{
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    // Try to sync camera frame and result, skip the future result.
    auto compare_timestamp = [](const struct timeval &t1, const struct timeval &t2) -> bool {
        if (t1.tv_sec == t2.tv_sec) {
            return t1.tv_usec < t2.tv_usec;
        }
        return t1.tv_sec < t2.tv_sec;
    };
    struct timeval t1 = fb->timestamp;
    // If detect fps higher than display fps, the result queue may be more than 1. May happen when using lvgl.
    while (!m_results.empty()) {
        result_t result = m_results.front();
        if (!compare_timestamp(t1, result.timestamp)) {
            m_result = result;
            m_results.pop();
        } else {
            break;
        }
    }
    xSemaphoreGive(m_res_mutex);

#if BSP_CONFIG_NO_GRAPHIC_LIB
    draw_detect_results_on_fb(fb, m_result.det_res, m_palette);
#else
    draw_detect_results_on_canvas(m_lcd_disp->get_lcd()->get_canvas(), m_result.det_res, m_palette);
#endif
}

void WhoDetectLCD::cleanup()
{
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    std::queue<result_t> empty = {};
    m_results.swap(empty);
    m_result = {};
    xSemaphoreGive(m_res_mutex);
}
} // namespace detect
} // namespace who
