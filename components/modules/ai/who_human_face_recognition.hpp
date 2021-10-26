#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

typedef enum
{
    IDLE = 0,
    DETECT,
    ENROLL,
    RECOGNIZE,
    DELETE,
} recognizer_state_t;

void register_human_face_recognition(QueueHandle_t frame_i,
                                     QueueHandle_t event,
                                     QueueHandle_t result,
                                     QueueHandle_t frame_o = NULL,
                                     const bool camera_fb_return = false);
