#ifndef _APP_LCD_H_
#define _APP_LCD_H_

#if __cplusplus
extern "C" {
#endif

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "iot_lcd.h"
#include "image.h"
#define LCD_CACHE_NUM 4

#define DISPLAY_IMAGE_WIDTH     240
#define DISPLAY_IMAGE_HEIGHT    240

#define LCD_WIDTH       240
#define LCD_HEIGHT      240

#define LCD_CS_GPIO     12
#define LCD_RESET_GPIO  -1
#define LCD_DC_GPIO     15
#define LCD_MOSI_GPIO   19
#define LCD_CLK_GPIO    21
#define LCD_LIGHT_GPIO  2
#define LCD_MISO_GPIO   -1

extern TaskHandle_t gpst_output_task;
extern QueueHandle_t gpst_output_queue;

void app_lcd_main();

#if __cplusplus
}
#endif
#endif
