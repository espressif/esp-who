#include "who_camera.h"
#include "who_human_face_detection.hpp"

static QueueHandle_t xQueueAIFrame = NULL;

extern "C" void app_main()
{
    xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));

    register_camera(PIXFORMAT_RGB565, FRAMESIZE_QVGA, 2, xQueueAIFrame);
    register_human_face_detection(xQueueAIFrame, NULL, NULL, NULL, true);
}
