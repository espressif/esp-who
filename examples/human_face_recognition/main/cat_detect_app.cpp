#include "cat_detect_app.hpp"
#include "driver/gpio.h"
#include "esp_pm.h"
#include "bsp/display.h"

namespace who {
namespace app {

#define GPIO_CAT_DETECT 3
#define CAT_DETECT_TIMEOUT 30000 // 30 seconds

CatDetectApp::CatDetectApp(const std::vector<std::vector<uint8_t>> &palette, frame_cap::WhoFrameCap *frame_cap)
    : WhoDetectAppLCD(palette, frame_cap)
    , m_cat_detected(false)
    , m_last_cat_time(0)
{
    // Initialize GPIO
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_CAT_DETECT);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(GPIO_CAT_DETECT, 0);

    // Initialize power management
    esp_pm_config_esp32s3_t pm_config = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 40,
        .light_sleep_enable = true
    };
    esp_pm_configure(&pm_config);

    // Initialize display
    m_display_on = true;
}

CatDetectApp::~CatDetectApp()
{
    gpio_set_level(GPIO_CAT_DETECT, 0);
}

void CatDetectApp::detect_result_cb(const detect::WhoDetect::result_t &result)
{
    WhoDetectAppLCD::detect_result_cb(result);

    bool cat_detected_now = !result.det_res.empty();
    TickType_t current_time = xTaskGetTickCount();

    if (cat_detected_now) {
        m_cat_detected = true;
        m_last_cat_time = current_time;
        gpio_set_level(GPIO_CAT_DETECT, 1);
        
        // Turn on display if it's off
        if (!m_display_on) {
            bsp_display_on();
            m_display_on = true;
        }
    } else {
        if (m_cat_detected && (current_time - m_last_cat_time) > pdMS_TO_TICKS(CAT_DETECT_TIMEOUT)) {
            m_cat_detected = false;
            gpio_set_level(GPIO_CAT_DETECT, 0);
        }
        
        // Turn off display if no cat detected for a while
        if (m_display_on && (current_time - m_last_cat_time) > pdMS_TO_TICKS(10000)) { // 10 seconds
            bsp_display_off();
            m_display_on = false;
        }
    }
}

} // namespace app
} // namespace who