#include "frame_cap_pipeline.hpp"
#include "cat_detect_app.hpp"
#include "cat_detect.hpp"
#include "bsp/esp-bsp.h"
#include "http_server.hpp"

using namespace who::frame_cap;
using namespace who::app;

dl::detect::Detect *get_detect_model()
{
    return new CatDetect(static_cast<CatDetect::model_type_t>(CONFIG_DEFAULT_CAT_DETECT_MODEL), false);
}

extern "C" void app_main(void)
{
    vTaskPrioritySet(xTaskGetCurrentTaskHandle(), 5);
#if CONFIG_CAT_DETECT_MODEL_IN_SDCARD
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
    auto frame_cap = get_mipi_csi_frame_cap_pipeline();
    // auto frame_cap = get_uvc_frame_cap_pipeline();
#endif
    auto detect_app = new CatDetectApp({{255, 0, 0}}, frame_cap);
    detect_app->set_model(get_detect_model());
    
    // Start HTTP server for video streaming
    start_http_server(get_cam_instance());
    
    detect_app->run();
}
