#include "frame_cap_pipeline.hpp"
#include "who_qrcode_app_lcd.hpp"
#include "who_qrcode_app_term.hpp"
#include "bsp/esp-bsp.h"

using namespace who::frame_cap;
using namespace who::app;

extern "C" void app_main(void)
{
    vTaskPrioritySet(xTaskGetCurrentTaskHandle(), 5);
#if CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_mount());
#endif

// close led
#ifdef BSP_BOARD_ESP32_S3_EYE
    ESP_ERROR_CHECK(bsp_leds_init());
    ESP_ERROR_CHECK(bsp_led_set(BSP_LED_GREEN, false));
#endif

#if CONFIG_IDF_TARGET_ESP32S3
    auto frame_cap = get_dvp_frame_cap_pipeline();
#elif CONFIG_IDF_TARGET_ESP32P4
    // auto frame_cap = get_mipi_csi_frame_cap_pipeline();
    auto frame_cap = get_uvc_frame_cap_pipeline();
#endif
    auto qrcode_app = new WhoQRCodeAppLCD(frame_cap);
    // try this if you don't have a lcd.
    // auto qrcode_app = new WhoQRCodeAppTerm(frame_cap);
    qrcode_app->run();
}
