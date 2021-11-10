#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C"
{
#endif

void register_speech_recognition(QueueHandle_t result, QueueHandle_t control_o);

#ifdef __cplusplus
}
#endif