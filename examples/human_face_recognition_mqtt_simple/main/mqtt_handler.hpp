#pragma once

#include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/queue.h"
// #include "freertos/task.h"
// #include "mqtt_client.h"
// #include "esp_event.h"
// #include "esp_log.h"
// #include "esp_err.h"
// #include "face_recognition_tool.hpp"
// #include "human_face_recognition.hpp"
// #include "led.h"

void mqtt_app_start();
esp_err_t mqtt_publish(const char *data);
