/* ESPRESSIF MIT License
 * 
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 * 
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * CAMERA + PROCESS + OUTPUT
 *      CONFIG_FACENET_INPUT + CONFIG_FACENET_PROCESS + CONFIG_FACENET_OUTPUT
 *
 * camera_single:
 *      app_main.c
 *      app_camera.c/h
 *      app_facenet.c/h
 */
#include "sdkconfig.h"
#include "app_camera.h"
#include "app_facenet.h"
#include "app_lcd.h"

extern "C" void printTask(void *arg)
{
#define BUF_SIZE 1 * 1024
    char *tasklist = (char *)malloc(BUF_SIZE);
    while (1)
    {
        memset(tasklist, 0, BUF_SIZE);
        vTaskGetRunTimeStats(tasklist);
        printf("Running tasks CPU usage: \n %s\r\n", (char *)tasklist);
        printf("RAM size: %dKB, with PSRAM: %dKB\n", heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL) / 1024, heap_caps_get_free_size(MALLOC_CAP_8BIT));
        vTaskDelay(5000 / portTICK_RATE_MS);
    }

    free(tasklist);
}

extern "C" void app_main()
{
    app_camera_init();
    app_lcd_main();
    app_facenet_main();
    // xTaskCreatePinnedToCore(&printTask, "printTask", 2 * 1024, NULL, 5, NULL, 1);
}
