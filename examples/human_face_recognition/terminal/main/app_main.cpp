#include "who_camera.h"
#include "who_human_face_recognition.hpp"
#include "event_logic.hpp"

static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueEventLogic = NULL;

extern "C" void app_main()
{
    xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    xQueueEventLogic = xQueueCreate(1, sizeof(int *));

    register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
    register_button_events(xQueueEventLogic);
    register_human_face_recognition(xQueueAIFrame, xQueueEventLogic, NULL, NULL, true);
}
