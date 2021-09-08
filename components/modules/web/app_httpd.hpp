#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

void register_httpd(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb);
