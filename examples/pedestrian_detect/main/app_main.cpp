#include "frame_cap_pipeline.hpp"
#include "pedestrian_detect.hpp"
#include "who_detect_app_lcd.hpp"
#include "who_detect_app_term.hpp"
#include "bsp/esp-bsp.h"

using namespace who::frame_cap;
using namespace who::app;

void run_detect_lcd()
{
    WhoFrameCapNode *lcd_disp_frame_cap_node = nullptr;
#if CONFIG_IDF_TARGET_ESP32S3
    auto frame_cap = get_lcd_dvp_frame_cap_pipeline();
#elif CONFIG_IDF_TARGET_ESP32P4
    auto frame_cap = get_lcd_mipi_csi_frame_cap_pipeline();
    // auto frame_cap = get_lcd_mipi_csi_ppa_frame_cap_pipeline(&lcd_disp_frame_cap_node);
    // auto frame_cap = get_lcd_uvc_frame_cap_pipeline();
#endif
    auto detect_app = new WhoDetectAppLCD({{255, 0, 0}}, frame_cap, lcd_disp_frame_cap_node);
    // create model later to avoid memory fragmentation.
    detect_app->set_model(new PedestrianDetect());
    detect_app->run();
}

void run_detect_term()
{
#if CONFIG_IDF_TARGET_ESP32S3
    auto frame_cap = get_term_dvp_frame_cap_pipeline();
#elif CONFIG_IDF_TARGET_ESP32P4
    auto frame_cap = get_term_mipi_csi_frame_cap_pipeline();
    // auto frame_cap = get_term_mipi_csi_ppa_frame_cap_pipeline();
    // auto frame_cap = get_term_uvc_frame_cap_pipeline();
#endif
    auto detect_app = new WhoDetectAppTerm(frame_cap);
    // create model later to avoid memory fragmentation.
    detect_app->set_model(new PedestrianDetect());
    detect_app->run();
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

    run_detect_lcd();
    // try this if you don't have a lcd.
    // run_detect_term();
}
