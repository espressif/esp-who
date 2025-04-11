#include "who_detect_lcd.hpp"
#include "who_detect_utils.hpp"
#include "who_lcd.hpp"

static const char *TAG = "WhoDetectLCD";

namespace who {
namespace detect {
WhoDetectLCD::WhoDetectLCD(frame_cap::WhoFrameCap *frame_cap,
                           dl::detect::Detect *detect,
                           const std::string &name,
                           const std::vector<std::vector<uint8_t>> &palette) :
    WhoDetectBase(frame_cap, detect, name), m_res_mutex(xSemaphoreCreateMutex())
{
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
    draw_detect_results_on_canvas(who::lcd::LCD::s_canvas, m_result.det_res, m_palette);
#endif
}
} // namespace detect
} // namespace who
