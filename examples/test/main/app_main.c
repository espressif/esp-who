#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "math.h"
#include "app_camera.h"


static const char *TAG = "APP_CODE_SCANNER";

static void camera_task()
{
    
    app_camera_init();

    scr_driver_t g_lcd;
    app_lcd_init(&g_lcd);

    camera_fb_t *fb = NULL;
    int64_t time1, time2;

    while (1)
    {
        // time1 = esp_timer_get_time();
        fb = esp_camera_fb_get();
        if(fb == NULL)
            printf("camera get failed\n");
        // time2 = esp_timer_get_time();
        // ESP_LOGI(TAG, "Camera get frame time in %lld ms.", (time2 - time1) / 1000);
        
        scr_info_t lcd_info;
        g_lcd.get_info(&lcd_info);
        g_lcd.draw_bitmap(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint16_t *)fb->buf);

        esp_camera_fb_return(fb);
    }
    vTaskDelete(NULL);
}


void app_main()
{
    xTaskCreatePinnedToCore(camera_task, TAG, 4 * 1024, NULL, 6, NULL, 0);
}