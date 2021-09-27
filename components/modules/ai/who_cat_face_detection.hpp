#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

void register_cat_face_detection(QueueHandle_t frame_i,
                                 QueueHandle_t event,
                                 QueueHandle_t result,
                                 QueueHandle_t frame_o,
                                 const bool camera_fb_return);
