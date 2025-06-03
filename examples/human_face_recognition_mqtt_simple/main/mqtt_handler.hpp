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
// #include "mqtt_client.h"

//void mqtt_app_start();
void mqtt_app_start(void (*mqtt_event_cb_)(const char *data, const size_t data_len));
esp_err_t mqtt_publish(const char *data);
