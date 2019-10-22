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

static const char *TAG = "app_process";

#define ENROLL_CONFIRM_TIMES 3
#define FACE_ID_SAVE_NUMBER 1
static face_id_list id_list = {0};
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

mtmn_config_t init_config()
{
    mtmn_config_t mtmn_config = {0};
    mtmn_config.type = FAST;
    mtmn_config.min_face = 80;
    mtmn_config.pyramid = 0.707;
    mtmn_config.pyramid_times = 4;
    mtmn_config.p_threshold.score = 0.6;
    mtmn_config.p_threshold.nms = 0.7;
    mtmn_config.p_threshold.candidate_number = 20;
    mtmn_config.r_threshold.score = 0.7;
    mtmn_config.r_threshold.nms = 0.7;
    mtmn_config.r_threshold.candidate_number = 10;
    mtmn_config.o_threshold.score = 0.7;
    mtmn_config.o_threshold.nms = 0.7;
    mtmn_config.o_threshold.candidate_number = 1;

    return mtmn_config;
}

void task_process(void *arg)
{ /*{{{*/
    size_t frame_num = 0;
    dl_matrix3du_t *image_matrix = NULL;
    camera_fb_t *fb = NULL;

    /* 1. Load configuration for detection */
    mtmn_config_t mtmn_config = init_config();

    /* 2. Preallocate matrix to store aligned face 56x56  */
    dl_matrix3du_t *aligned_face = dl_matrix3du_alloc(1,
                                                      FACE_WIDTH,
                                                      FACE_HEIGHT,
                                                      3);

    int8_t count_down_second = 3; //second
    int8_t is_enrolling = 1;
    int32_t next_enroll_index = 0;
    int8_t left_sample_face;

    do
    {
        int64_t start_time = esp_timer_get_time();
        /* 3. Get one image with camera */
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            continue;
        }
        int64_t fb_get_time = esp_timer_get_time();
        ESP_LOGI(TAG, "Get one frame in %lld ms.", (fb_get_time - start_time) / 1000);

        /* 4. Allocate image matrix to store RGB data */
        image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);

        /* 5. Transform image to RGB */
        uint32_t res = fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
        if (true != res)
        {
            ESP_LOGE(TAG, "fmt2rgb888 failed, fb: %d", fb->len);
            dl_matrix3du_free(image_matrix);
            continue;
        }

        esp_camera_fb_return(fb);

        /* 6. Do face detection */
        box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
        ESP_LOGI(TAG, "Detection time consumption: %lldms", (esp_timer_get_time() - fb_get_time) / 1000);

        if (net_boxes)
        {
            frame_num++;
            //ESP_LOGI(TAG, "Face Detection Count: %d", frame_num);

            /* 5. Do face alignment */
            if (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK)
            {
                //count down
                while (count_down_second > 0)
                {
                    ESP_LOGI(TAG, "Face ID Enrollment Starts in %ds.\n", count_down_second);

                    vTaskDelay(1000 / portTICK_PERIOD_MS);

                    count_down_second--;

                    if (count_down_second == 0)
                        ESP_LOGI(TAG, "\n>>> Face ID Enrollment Starts <<<\n");
                }

                /* 6. Do face enrollment */
                if (is_enrolling == 1)
                {
                    left_sample_face = enroll_face(&id_list, aligned_face);
                    ESP_LOGI(TAG, "Face ID Enrollment: Take the %d%s sample",
                             ENROLL_CONFIRM_TIMES - left_sample_face,
                             number_suffix(ENROLL_CONFIRM_TIMES - left_sample_face));

                    if (left_sample_face == 0)
                    {
                        next_enroll_index++;
                        ESP_LOGI(TAG, "\nEnrolled Face ID: %d", id_list.tail);

                        if (id_list.count == FACE_ID_SAVE_NUMBER)
                        {
                            is_enrolling = 0;
                            ESP_LOGI(TAG, "\n>>> Face Recognition Starts <<<\n");
                            vTaskDelay(2000 / portTICK_PERIOD_MS);
                        }
                        else
                        {
                            ESP_LOGI(TAG, "Please log in another one.");
                            vTaskDelay(2500 / portTICK_PERIOD_MS);
                        }
                    }
                }
                /* 6. Do face recognition */
                else
                {
                    int64_t recog_match_time = esp_timer_get_time();

                    int matched_id = recognize_face(&id_list, aligned_face);
                    if (matched_id >= 0)
                        ESP_LOGI(TAG, "Matched Face ID: %d\n", matched_id);
                    else
                        ESP_LOGI(TAG, "No Matched Face ID\n");

                    ESP_LOGI(TAG, "Recognition time consumption: %lldms",
                             (esp_timer_get_time() - recog_match_time) / 1000);
                }
            }
            else
            {
                ESP_LOGI(TAG, "Detected face is not proper.");
            }

            dl_lib_free(net_boxes->score);
            dl_lib_free(net_boxes->box);
            dl_lib_free(net_boxes->landmark);
            dl_lib_free(net_boxes);
        }

        dl_matrix3du_free(image_matrix);

    } while (1);
} /*}}}*/

void app_facenet_main()
{
    face_id_init(&id_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
    xTaskCreatePinnedToCore(task_process, "process", 4 * 1024, NULL, 5, NULL, 1);
}
