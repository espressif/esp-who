#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

typedef enum
{
    COLOR_DETECTION_IDLE = 0,
    OPEN_REGISTER_COLOR_BOX,
    CLOSE_REGISTER_COLOR_BOX,
    REGISTER_COLOR,
    DELETE_COLOR,
    INCREASE_COLOR_AREA,
    DECREASE_COLOR_AREA,
    SWITCH_RESULT,
} color_detection_state_t;

void register_color_detection(QueueHandle_t frame_i,
                                     QueueHandle_t event,
                                     QueueHandle_t result,
                                     QueueHandle_t frame_o = NULL,
                                     const bool camera_fb_return = false);
