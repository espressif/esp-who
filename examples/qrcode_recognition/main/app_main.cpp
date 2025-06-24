#include "frame_cap_pipeline.hpp"
#include "who_qrcode_app.hpp"
#include "bsp/esp-bsp.h"

using namespace who::frame_cap;
using namespace who::app;

void run_qrcode_lcd()
{
    WhoFrameCapNode *lcd_disp_frame_cap_node = nullptr;
#if CONFIG_IDF_TARGET_ESP32S3
    auto frame_cap = get_lcd_dvp_frame_cap_pipeline();
#elif CONFIG_IDF_TARGET_ESP32P4
    auto frame_cap = get_lcd_mipi_csi_frame_cap_pipeline();
    // auto frame_cap = get_lcd_uvc_frame_cap_pipeline();
#endif
    auto qrcode_app = new WhoQRCodeAppLCD(frame_cap, lcd_disp_frame_cap_node);
    qrcode_app->run();
}

void run_qrcode_term()
{
#if CONFIG_IDF_TARGET_ESP32S3
    auto frame_cap = get_term_dvp_frame_cap_pipeline();
#elif CONFIG_IDF_TARGET_ESP32P4
    auto frame_cap = get_term_mipi_csi_frame_cap_pipeline();
    // auto frame_cap = get_term_uvc_frame_cap_pipeline();
#endif
    auto qrcode_app = new WhoQRCodeAppTerm(frame_cap);
    qrcode_app->run();
}

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

    run_qrcode_lcd();
    // try this if you don't have a lcd.
    // run_qrcode_term();
}
