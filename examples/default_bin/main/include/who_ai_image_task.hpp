#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "test_logic.h"

typedef enum
{
    TASK_IDLE = 0,
    TASK_CAMERA_LCD,
    TASK_CAT_FACE_DETECTION,
    TASK_HUMAN_FACE_RECOGNITION,
    TASK_MOTION_DETECTION,
    TASK_COLOR_DETECTION,
    TASK_HAND_DETECTION,
} ai_image_task_t;

typedef enum
{
    FACE_RECOGNITION_IDLE = 20,
    FACE_RECOGNITION_DETECT,
    FACE_RECOGNITION_ENROLL,
    FACE_RECOGNITION_RECOGNIZE,
    FACE_RECOGNITION_DELETE,
} face_recognition_event_t;

void register_ai_image_task(QueueHandle_t frame_i,
                            QueueHandle_t event,
                            QueueHandle_t result,
                            QueueHandle_t frame_o = NULL,
                            const bool camera_fb_return = false);

// void register_ai_image_task(const QueueHandle_t frame_i,
//                             const QueueHandle_t event,
//                             const QueueHandle_t frame_o);