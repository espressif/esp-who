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

void *facenet_get_image()
{
    void *buffer = NULL;
    xQueueReceive(gpst_input_queue, &buffer, portMAX_DELAY);
    return buffer;
}

void facenet_output_image(void *buffer)
{
    //xQueueSend(gpst_output_queue, &buffer, portMAX_DELAY);
    xTaskNotifyGive(gpst_input_task);
}

void task_process(void *arg)
{ /*{{{*/
    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1,
                                                      gl_input_image_width,
                                                      gl_input_image_height,
                                                      3);

    dl_matrix3du_t *aligned_face = dl_matrix3du_alloc(1,
                                                      FACE_WIDTH,
                                                      FACE_HEIGHT,
                                                      3);

    size_t frame_num = 0;
    uint16_t *img_buffer = NULL;

    fptp_t thresh = FACE_REC_THRESHOLD;
    dl_matrix3d_t *id_list[FACE_ID_SAVE_NUMBER] = {0};

    char delay_before_login = 3;
    int is_logging = 1;
    int next_logging_index = 0;

    do
    {
        img_buffer = (uint16_t *)facenet_get_image();
        transform_input_image(image_matrix->item,
                              img_buffer,
                              gl_input_image_width * gl_input_image_height);

        box_array_t *net_boxes = face_detect(image_matrix);

        if (net_boxes)
        {
            //draw_rectangle(img_buffer, net_boxes, gl_input_image_width);
            frame_num++;
            ESP_LOGI(TAG, "Face Detection Count: %d", frame_num);

            if (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK)
            {
                if ((is_logging == 1) && (next_logging_index < FACE_ID_SAVE_NUMBER))
                {
                    // delay
                    if (delay_before_login > 1)
                    {
                        delay_before_login--;
                        ESP_LOGE(TAG, "Login start in %d.", delay_before_login);

                        facenet_output_image(img_buffer);

                        continue;
                    }
                    else if (delay_before_login == 1)
                    {
                        ESP_LOGE(TAG, ">>> Start Face Login <<<");
                        delay_before_login--;
                    }

                    // login
                    if (id_list[next_logging_index] == NULL)
                    {
                        id_list[next_logging_index] = dl_matrix3d_alloc(1, 1, 1, FACE_ID_SIZE);
                    }

                    if (login(aligned_face, id_list[next_logging_index], LOGIN_CONFIRM_TIMES) == ESP_OK)
                    {
                        next_logging_index++;
                        ESP_LOGE(TAG, "Login ID: %d", next_logging_index);

                        if (next_logging_index == FACE_ID_SAVE_NUMBER)
                        {
                            is_logging = 0;
                            ESP_LOGE(TAG, ">>> Start Face Recognition <<<");
                            vTaskDelay(2000 / portTICK_PERIOD_MS);
                        }
                        else
                        {
                            ESP_LOGE(TAG, "Please log in another one.");
                            vTaskDelay(2500 / portTICK_PERIOD_MS);
                        }
                    }
                }
                else
                {
                    int matched_id = recognize_face(aligned_face,
                                                    id_list, thresh,
                                                    next_logging_index);
                    ESP_LOGE(TAG, "Matched ID: %d", matched_id);
                }
            }
            else
            {
                ESP_LOGI(TAG, "Detected face is not proper.");
            }

            free(net_boxes->box);
            free(net_boxes);
        }

        facenet_output_image(image_matrix->item);

    } while (1);
    dl_matrix3du_free(image_matrix);
} /*}}}*/

void app_facenet_main()
{
    xTaskCreatePinnedToCore(task_process, "process", 4 * 1024, NULL, 5, NULL, 1);
}
