#include "pedestrian_detect.hpp"
#include "who_cam.hpp"
#include "who_detect_app.hpp"
#include "who_lcd.hpp"
#include "who_yield2idle.hpp"

using namespace who::cam;
using namespace who::lcd;
using namespace who::app;

extern "C" void app_main(void)
{
#if CONFIG_PEDESTRIAN_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_mount());
#endif

#if CONFIG_IDF_TARGET_ESP32S3
    ESP_ERROR_CHECK(bsp_leds_init());
    ESP_ERROR_CHECK(bsp_led_set(BSP_LED_GREEN, false));
#endif

#if CONFIG_IDF_TARGET_ESP32P4
    auto cam = new WhoP4Cam(VIDEO_PIX_FMT_RGB565, 3, V4L2_MEMORY_USERPTR, true);
    // auto cam = new WhoP4PPACam(VIDEO_PIX_FMT_RGB565, 4, V4L2_MEMORY_USERPTR, 224, 224, true);
#elif CONFIG_IDF_TARGET_ESP32S3
    auto cam = new WhoS3Cam(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, true);
#endif
    auto model = new PedestrianDetect();

    auto detect = new WhoDetectTerminalApp("pedestrian");
    detect->set_cam(cam);
    detect->set_model(model);
    // detect->set_fps(5);
    detect->run();
}
