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

char *number_suffix(int32_t number)
{
    uint8_t n = number % 10;

    if (n == 0)
        return "zero";
    else if (n == 1)
        return "st";
    else if (n == 2)
        return "nd";
    else if (n == 3)
        return "rd";
    else
        return "th";
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

    int8_t count_down_second = 3; //second
    int8_t is_enrolling = 1;
    int32_t next_enroll_index = 0;
    int8_t left_sample_face;

    int64_t timestamp = 0;

    do
    {
        img_buffer = (uint16_t *)facenet_get_image();
        transform_input_image(image_matrix->item,
                              img_buffer,
                              gl_input_image_width * gl_input_image_height);

        timestamp = esp_timer_get_time();
        box_array_t *net_boxes = face_detect(image_matrix);
        ESP_LOGI(TAG, "Detection time consumption: %lldms", (esp_timer_get_time() - timestamp) / 1000);

        if (net_boxes)
        {
            //draw_rectangle(img_buffer, net_boxes, gl_input_image_width);
            frame_num++;
            ESP_LOGI(TAG, "Face Detection Count: %d", frame_num);

            if (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK)
            {
                //count down
                while (count_down_second > 0)
                {
                    ESP_LOGE(TAG, "Face ID Enrollment Starts in %ds.", count_down_second);

                    vTaskDelay(1000 / portTICK_PERIOD_MS);

                    count_down_second--;

                    if (count_down_second == 0)
                        ESP_LOGE(TAG, ">>> Face ID Enrollment Starts <<<");
                }

                //enroll
                if ((is_enrolling == 1) && (next_enroll_index < FACE_ID_SAVE_NUMBER))
                {
                    if (id_list[next_enroll_index] == NULL)
                        id_list[next_enroll_index] = dl_matrix3d_alloc(1, 1, 1, FACE_ID_SIZE);

                    left_sample_face = enroll(aligned_face, id_list[next_enroll_index], ENROLL_CONFIRM_TIMES);
                    ESP_LOGE(TAG, "Face ID Enrollment: Take the %d%s sample",
                             ENROLL_CONFIRM_TIMES - left_sample_face,
                             number_suffix(ENROLL_CONFIRM_TIMES - left_sample_face));

                    if (left_sample_face == 0)
                    {
                        next_enroll_index++;
                        ESP_LOGE(TAG, "Enrolled Face ID: %d", next_enroll_index);

                        if (next_enroll_index == FACE_ID_SAVE_NUMBER)
                        {
                            is_enrolling = 0;
                            ESP_LOGE(TAG, ">>> Face Recognition Starts <<<");
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
                    timestamp = esp_timer_get_time();

                    uint16_t matched_id = recognize_face(aligned_face,
                                                         id_list, thresh,
                                                         next_enroll_index);
                    if (matched_id)
                        ESP_LOGE(TAG, "Matched Face ID: %d", matched_id);
                    else
                        ESP_LOGE(TAG, "No Matched Face ID");

                    ESP_LOGI(TAG, "Recognition time consumption: %lldms",
                             (esp_timer_get_time() - timestamp) / 1000);
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
