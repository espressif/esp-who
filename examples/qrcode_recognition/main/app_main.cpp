#include "who_qrcode_app.hpp"

using namespace who::cam;
using namespace who::lcd;
using namespace who::app;

#define WITH_LCD 1

extern "C" void app_main(void)
{
#if CONFIG_IDF_TARGET_ESP32S3
    ESP_ERROR_CHECK(bsp_leds_init());
    ESP_ERROR_CHECK(bsp_led_set(BSP_LED_GREEN, false));
#endif

#if CONFIG_IDF_TARGET_ESP32P4
    auto cam = new WhoP4Cam(VIDEO_PIX_FMT_RGB565, 3, V4L2_MEMORY_USERPTR, true);
#elif CONFIG_IDF_TARGET_ESP32S3
    auto cam = new WhoS3Cam(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, true);
#endif

#if WITH_LCD
    auto lcd = new WhoLCD();
    auto qrcode = new WhoQRCodeAppLCD();
    qrcode->set_cam(cam);
    qrcode->set_lcd(lcd);
    qrcode->run();
#else
    auto qrcode = new WhoQRCodeAppTerm();
    qrcode->set_cam(cam);
    qrcode->run();
#endif
}
