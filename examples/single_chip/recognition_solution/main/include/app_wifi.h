#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern EventGroupHandle_t g_wifi_event_group;

void app_wifi_init();
