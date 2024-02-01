#include "who_camera.h"
#include "who_color_detection.hpp"
#include "who_lcd.h"
#include "event_logic.hpp"

static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueLCDFrame = NULL;
static QueueHandle_t xQueueEventLogic = NULL;

extern "C" void app_main()
{
    xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    xQueueLCDFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    xQueueEventLogic = xQueueCreate(1, sizeof(int));

    register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
    register_color_detection(xQueueAIFrame, xQueueEventLogic, NULL, xQueueLCDFrame, false);
    register_lcd(xQueueLCDFrame, NULL, true);
    register_button_events(xQueueEventLogic);
}
