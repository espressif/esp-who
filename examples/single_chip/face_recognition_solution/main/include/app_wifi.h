#ifndef _APP_WIFI_H_
#define _APP_WIFI_H_

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern EventGroupHandle_t g_wifi_event_group;

#ifdef __cplusplus
extern "C" {
#endif

void app_wifi_init(void);

#ifdef __cplusplus
}
#endif

#endif
