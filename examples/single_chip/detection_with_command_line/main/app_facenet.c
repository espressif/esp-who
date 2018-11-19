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
#include <string.h>
#include <math.h>
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "app_facenet.h"
#include "sdkconfig.h"
#include "dl_lib.h"

static const char *TAG = "app_process";
extern QueueHandle_t gpst_output_queue;

void *facenet_get_image ()
{
    void *buffer = NULL;
    xQueueReceive(gpst_input_queue, &buffer, portMAX_DELAY);
    return buffer;
}

void facenet_output_image (void *buffer)
{
    //xQueueSend(gpst_output_queue, &buffer, portMAX_DELAY);
    xTaskNotifyGive(gpst_input_task);
}

void task_process (void *arg)
{/*{{{*/
    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, gl_input_image_width, gl_input_image_height, 3);
    size_t frame_num = 0;
    uint16_t *img_buffer = NULL;
    do
    {
        img_buffer = (uint16_t *)facenet_get_image();
        transform_input_image(image_matrix->item, img_buffer, gl_input_image_width * gl_input_image_height);

        box_array_t *net_boxes = face_detect(image_matrix);

        if (net_boxes)
        {
            //draw_rectangle(img_buffer, net_boxes, gl_input_image_width);
            frame_num++;
            ESP_LOGI(TAG, "DETECTED: %d\n", frame_num);
            free(net_boxes->box);
            free(net_boxes);
        }

        facenet_output_image(image_matrix->item);

    } while(1);
    dl_matrix3du_free(image_matrix);
}/*}}}*/

void app_facenet_main ()
{
    xTaskCreatePinnedToCore(task_process, "process", 4*1024, NULL, 5, NULL, 1);
}

