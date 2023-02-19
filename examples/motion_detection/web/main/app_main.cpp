#include "who_camera.h"
#include "who_motion_detection.hpp"
#include "app_wifi.h"
#include "app_httpd.hpp"
#include "app_mdns.h"

static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueHttpFrame = NULL;

extern "C" void app_main()
{
    app_wifi_main();

    xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    xQueueHttpFrame = xQueueCreate(2, sizeof(camera_fb_t *));

    register_camera(PIXFORMAT_RGB565, FRAMESIZE_QVGA, 2, xQueueAIFrame);
    register_motion_detection(xQueueAIFrame, NULL, NULL, xQueueHttpFrame);
    register_httpd(xQueueHttpFrame, NULL, true);

    // Must be done after camera initialization because it calls 'esp_camera_sensor_get'
    app_mdns_main();
}
