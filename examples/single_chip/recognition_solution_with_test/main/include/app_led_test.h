#ifndef _APP_LED_H_
#define _APP_LED_H_

#include "app_main.h"
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define GPIO_LED_RED    21
#define GPIO_LED_WHITE  22


void gpio_led_test_init();

#endif