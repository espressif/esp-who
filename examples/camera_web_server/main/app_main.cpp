#include "app_camera.hpp"
#include "app_wifi.h"
#include "app_httpd.hpp"
#include "app_mdns.h"
#if CONFIG_LED_ILLUMINATOR_ENABLED
#include "app_led.h"
#endif

extern "C" void app_main()
{
    app_wifi_main();

#if CONFIG_CAMERA_PIXEL_FORMAT_RGB565
    app_camera_init(FRAMESIZE_QVGA, 12, 2);
#else
    app_camera_init(FRAMESIZE_UXGA, 12, 2);
#endif

#if CONFIG_LED_ILLUMINATOR_ENABLED
    app_led_init();
#endif
    app_httpd_main();
    app_mdns_main();
}
