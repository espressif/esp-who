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
#include "app_lcd.h"
#include "app_facenet.h"
#include "sdkconfig.h"

static const char *TAG = "app_process";
QueueHandle_t gpst_image_matrix_queue = NULL;
QueueHandle_t gpst_input_queue = NULL;
QueueHandle_t gpst_pnet_queue = NULL;
QueueHandle_t gpst_rnet_queue = NULL;
QueueHandle_t gpst_onet_queue = NULL;
#if FACE_RECOGNITION
QueueHandle_t gpst_aligned_face_queue = NULL;
QueueHandle_t gpst_recog_queue = NULL;
QueueHandle_t gpst_recog_output_queue = NULL;
#endif
mtmn_config_t mtmn_config = {0};

box_array_t *pnet_forward(dl_matrix3du_t *image, fptp_t min_face, fptp_t pyramid, net_config_t *config);

box_array_t *pnet_forward2(dl_matrix3du_t *image, fptp_t min_face, fptp_t pyramid, int pyramid_times, net_config_t *config);

box_array_t *pnet_forward_fast(dl_matrix3du_t *image, fptp_t min_face, int pyramid_times, net_config_t *config);

box_array_t *rnet_forward(dl_matrix3du_t *image, box_array_t *net_boxes, net_config_t *config);

box_array_t *onet_forward(dl_matrix3du_t *image, box_array_t *net_boxes, net_config_t *config);

typedef struct
{
    uint8_t *item;
    box_array_t net_boxes;
} st_net_info;

void init_config(mtmn_config_t *mtmn_config)
{
    mtmn_config->type = FAST;
    mtmn_config->min_face = 80;
    mtmn_config->pyramid = 0.707;
    mtmn_config->pyramid_times = 4;
    mtmn_config->p_threshold.score = 0.6;
    mtmn_config->p_threshold.nms = 0.7;
    mtmn_config->p_threshold.candidate_number = 5;
    mtmn_config->r_threshold.score = 0.7;
    mtmn_config->r_threshold.nms = 0.7;
    mtmn_config->r_threshold.candidate_number = 2;
    mtmn_config->o_threshold.score = 0.7;
    mtmn_config->o_threshold.nms = 0.7;
    mtmn_config->o_threshold.candidate_number = 2;
}

void task_transform_input(void *arg)
{
    camera_fb_t *fb = NULL;
    uint8_t *buf = NULL;

    int64_t time0[4] = {0};
    while (1)
    {
        time0[0] = esp_timer_get_time();
        /* Get one image with camera */
        fb = esp_camera_fb_get();
        time0[1] = esp_timer_get_time();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            continue;
        }

        xQueueReceive(gpst_image_matrix_queue, &buf, portMAX_DELAY);
        time0[2] = esp_timer_get_time();

        /* Transform image to RGB */
        uint32_t res = fmt2rgb888(fb->buf, fb->len, fb->format, buf);
        time0[3] = esp_timer_get_time();
        if (true != res)
        {
            ESP_LOGE(TAG, "fmt2rgb888 failed, fb: %d", fb->len);
            esp_camera_fb_return(fb);
            xQueueSend(gpst_image_matrix_queue, &buf, 0);
            continue;
        }

        esp_camera_fb_return(fb);
        if (xQueueSend(gpst_pnet_queue, &buf, 0) != pdTRUE)
        {
            ESP_LOGE("Trans", "Send fail");
        }
        //ESP_LOGI(TAG, "Trans: wait fb %lldms, wait matrix %lldms, transform %lldms",
        //        (time0[1] - time0[0]) / 1000,
        //        (time0[2] - time0[1]) / 1000,
        //        (time0[3] - time0[2]) / 1000
        //        );
    }
}

void task_process(void *arg)
{ /*{{{*/
    size_t frame_num = 0;
    dl_matrix3du_t image_matrix;
    image_matrix.n = 1;
    image_matrix.w = DISPLAY_IMAGE_WIDTH;
    image_matrix.h = DISPLAY_IMAGE_HEIGHT;
    image_matrix.c = 3;
    image_matrix.stride = 3 * DISPLAY_IMAGE_WIDTH;
    image_matrix.item = NULL;
    mtmn_config_t mtmn_config;

    int64_t time0[4] = {0};
    do
    {
        time0[0] = esp_timer_get_time();
        xQueueReceive(gpst_input_queue, &(image_matrix.item), portMAX_DELAY);
        time0[1] = esp_timer_get_time();

        /* Do face detection */
        box_array_t *net_boxes = face_detect(&image_matrix, &mtmn_config);
        time0[2] = esp_timer_get_time();
        ESP_LOGI(TAG, "Detection: wait %lldms, process %lldms",
                 (time0[1] - time0[0]) / 1000,
                 (time0[2] - time0[1]) / 1000);

        if (net_boxes)
        {
            frame_num++;
            draw_rectangle_rgb888(image_matrix.item, net_boxes, 240);
            //ESP_LOGI(TAG, "DETECTED: %d\n", frame_num);
            free(net_boxes->score);
            free(net_boxes->box);
            free(net_boxes->landmark);
            free(net_boxes);
        }
        xQueueSend(gpst_output_queue, &(image_matrix.item), 0);
    } while (1);
} /*}}}*/

void task_pnet(void *arg)
{
    mtmn_config_t *config = (mtmn_config_t *)arg;
    net_config_t net_config = {0};
    net_config.w = 12;
    net_config.h = 12;
    net_config.threshold = config->p_threshold;
    box_array_t *net_boxes = NULL;
    dl_matrix3du_t image_matrix;
    image_matrix.n = 1;
    image_matrix.w = DISPLAY_IMAGE_WIDTH;
    image_matrix.h = DISPLAY_IMAGE_HEIGHT;
    image_matrix.c = 3;
    image_matrix.stride = 3 * DISPLAY_IMAGE_WIDTH;
    image_matrix.item = NULL;
    st_net_info net_info = {0};
    int64_t time0[4] = {0};
    while (1)
    {
        time0[0] = esp_timer_get_time();
        xQueueReceive(gpst_pnet_queue, &(image_matrix.item), portMAX_DELAY);
        time0[1] = esp_timer_get_time();
        net_boxes = pnet_forward_fast(&image_matrix,
                                      config->min_face,
                                      config->pyramid_times,
                                      &net_config);
        time0[2] = esp_timer_get_time();
        if (NULL == net_boxes)
        {
            if (xQueueSend(gpst_output_queue, &(image_matrix.item), 0) != pdTRUE)
            {
                ESP_LOGE("Pnet", "Send fail 0");
            }
        }
        else
        {
            net_info.item = image_matrix.item;
            net_info.net_boxes = *net_boxes;
            free(net_boxes);
            if (xQueueSend(gpst_rnet_queue, &net_info, 0) != pdTRUE)
            {
                ESP_LOGE("Pnet", "Send fail 1");
            }
        }
        //ESP_LOGI(TAG, "Pnet: wait %lldms, process %lldms",
        //        (time0[1] - time0[0]) / 1000,
        //        (time0[2] - time0[1]) / 1000
        //        );
    }
}

void task_rnet(void *arg)
{
    mtmn_config_t *config = (mtmn_config_t *)arg;
    net_config_t net_config = {0};
    net_config.w = 24;
    net_config.h = 24;
    net_config.threshold = config->r_threshold;
    box_array_t *net_boxes = NULL;
    dl_matrix3du_t image_matrix;
    image_matrix.n = 1;
    image_matrix.w = DISPLAY_IMAGE_WIDTH;
    image_matrix.h = DISPLAY_IMAGE_HEIGHT;
    image_matrix.c = 3;
    image_matrix.stride = 3 * DISPLAY_IMAGE_WIDTH;
    image_matrix.item = NULL;
    st_net_info net_info = {0};
    int64_t time0[4] = {0};
    while (1)
    {
        time0[0] = esp_timer_get_time();
        xQueueReceive(gpst_rnet_queue, &net_info, portMAX_DELAY);
        time0[1] = esp_timer_get_time();
        image_matrix.item = net_info.item;
        net_boxes = rnet_forward(&image_matrix,
                                 &net_info.net_boxes,
                                 &net_config);
        time0[2] = esp_timer_get_time();
        free(net_info.net_boxes.box);
        if (NULL == net_boxes)
        {
            if (xQueueSend(gpst_output_queue, &(image_matrix.item), 0) != pdTRUE)
            {
                ESP_LOGE("Rnet", "Send fail 0");
            }
        }
        else
        {
            net_info.net_boxes = *net_boxes;
            free(net_boxes);
            if (xQueueSend(gpst_onet_queue, &net_info, 0) != pdTRUE)
            {
                ESP_LOGE("Rnet", "Send fail 1");
            }
        }
        //ESP_LOGI(TAG, "Rnet: wait %lldms, process %lldms",
        //        (time0[1] - time0[0]) / 1000,
        //        (time0[2] - time0[1]) / 1000
        //        );
    }
}

void task_onet(void *arg)
{
    mtmn_config_t *config = (mtmn_config_t *)arg;
    net_config_t net_config = {0};
    net_config.w = 48;
    net_config.h = 48;
    net_config.threshold = config->o_threshold;
    box_array_t *net_boxes = NULL;
    dl_matrix3du_t image_matrix;
    image_matrix.n = 1;
    image_matrix.w = DISPLAY_IMAGE_WIDTH;
    image_matrix.h = DISPLAY_IMAGE_HEIGHT;
    image_matrix.c = 3;
    image_matrix.stride = 3 * DISPLAY_IMAGE_WIDTH;
    image_matrix.item = NULL;
    st_net_info net_info = {0};
    int64_t time0[4] = {0};
    dl_matrix3du_t aligned_face;
    aligned_face.n = 1;
    aligned_face.w = 56;
    aligned_face.h = 56;
    aligned_face.c = 3;
    aligned_face.stride = 3 * 56;
    aligned_face.item = NULL;
    while (1)
    {
        time0[0] = esp_timer_get_time();
        xQueueReceive(gpst_onet_queue, &net_info, portMAX_DELAY);
        time0[1] = esp_timer_get_time();
        image_matrix.item = net_info.item;
        net_boxes = onet_forward(&image_matrix,
                                 &net_info.net_boxes,
                                 &net_config);
        time0[2] = esp_timer_get_time();
        free(net_info.net_boxes.box);
        if (net_boxes)
        {
            draw_rectangle_rgb888(image_matrix.item, net_boxes, 240);
#if FACE_RECOGNITION
            if (xQueueReceive(gpst_aligned_face_queue, &(aligned_face.item), 0) == pdTRUE)
            {
                if (align_face(net_boxes, &image_matrix, &aligned_face) == ESP_OK)
                {
                    if (xQueueSend(gpst_recog_queue, &(aligned_face.item), 0) != pdTRUE)
                        ESP_LOGE("Onet", "Recog send fail");
                }
                else
                {
                    if (xQueueSend(gpst_aligned_face_queue, &(aligned_face.item), 0) != pdTRUE)
                        ESP_LOGE("Onet", "Aligned send fail");
                }
            }
#endif
            free(net_boxes->score);
            free(net_boxes->box);
            free(net_boxes->landmark);
            free(net_boxes);
        }
        if (xQueueSend(gpst_output_queue, &(image_matrix.item), 0) != pdTRUE)
            ESP_LOGE("Onet", "Send fail");

        //ESP_LOGI(TAG, "Onet: wait %lldms, process %lldms",
        //        (time0[1] - time0[0]) / 1000,
        //        (time0[2] - time0[1]) / 1000
        //        );
    }
}

#if FACE_RECOGNITION
void task_recognition(void *arg)
{
    int8_t count_down_second = 3; //second
    int8_t is_enrolling = 1;
    int8_t left_sample_face;
    face_id_list id_list = {0};
    dl_matrix3du_t aligned_face;
    aligned_face.n = 1;
    aligned_face.w = 56;
    aligned_face.h = 56;
    aligned_face.c = 3;
    aligned_face.stride = 3 * 56;
    aligned_face.item = NULL;
    face_id_init(&id_list, 1, 3);

    while (1)
    {
        xQueueReceive(gpst_recog_queue, &(aligned_face.item), portMAX_DELAY);

        //count down
        while (count_down_second > 0)
        {
            ESP_LOGW(TAG, "Face ID Enrollment Starts in %ds.\n", count_down_second);

            vTaskDelay(200 / portTICK_PERIOD_MS);

            count_down_second--;

            if (count_down_second == 0)
                ESP_LOGW(TAG, "\n>>> Face ID Enrollment Starts <<<\n");
        }

        if (is_enrolling == 1)
        {
            left_sample_face = enroll_face(&id_list, &aligned_face);
            ESP_LOGW(TAG, "Face ID Enrollment: Take the %d sample",
                     3 - left_sample_face);

            if (left_sample_face == 0)
            {
                ESP_LOGW(TAG, "\nEnrolled Face ID: %d", id_list.tail);

                if (id_list.count == 1)
                {
                    is_enrolling = 0;
                    ESP_LOGW(TAG, "\n>>> Face Recognition Starts <<<\n");
                    vTaskDelay(2000 / portTICK_PERIOD_MS);
                }
                else
                {
                    ESP_LOGI(TAG, "Please log in another one.");
                    vTaskDelay(2500 / portTICK_PERIOD_MS);
                }
            }
        }
        /* Do face recognition */
        else
        {
            int64_t recog_match_time = esp_timer_get_time();

            int matched_id = recognize_face(&id_list, &aligned_face);
            if (matched_id >= 0)
                ESP_LOGW("Recog", "Matched Face ID: %d\n", matched_id);
            else
                ESP_LOGW("Recog", "No Matched Face ID\n");

            ESP_LOGW("Recog", "Recognition time consumption: %lldms",
                     (esp_timer_get_time() - recog_match_time) / 1000);
            if (xQueueSend(gpst_recog_output_queue, &matched_id, 0) != pdTRUE)
                ESP_LOGE("Recog", "Recog send fail");
        }
        if (xQueueSend(gpst_aligned_face_queue, &(aligned_face.item), 0) != pdTRUE)
            ESP_LOGE("Recog", "Aligned send fail");
    }
}
#endif

void app_facenet_main()
{
    uint8_t *image_rgb888[3];
    uint8_t *align_face[3];
    gpst_image_matrix_queue = xQueueCreate(3, sizeof(void *));
    gpst_pnet_queue = xQueueCreate(3, sizeof(st_net_info));
    gpst_rnet_queue = xQueueCreate(3, sizeof(st_net_info));
    gpst_onet_queue = xQueueCreate(3, sizeof(st_net_info));
#if FACE_RECOGNITION
    gpst_aligned_face_queue = xQueueCreate(1, sizeof(void *));
    gpst_recog_queue = xQueueCreate(1, sizeof(void *));
    gpst_recog_output_queue = xQueueCreate(1, sizeof(int));
    align_face[0] = calloc(56 * 56 * 3, sizeof(uint8_t));
    xQueueSend(gpst_aligned_face_queue, &(align_face[0]), 0);
#endif
    for (int i = 0; i < 3; i++)
    {
        image_rgb888[i] = calloc(DISPLAY_IMAGE_WIDTH * DISPLAY_IMAGE_HEIGHT * 3, sizeof(uint8_t));
        xQueueSend(gpst_image_matrix_queue, &(image_rgb888[i]), 0);
    }

    /* Load configuration for detection */
    init_config(&mtmn_config);

    xTaskCreatePinnedToCore(task_transform_input, "transform_input", 4 * 1024, NULL, 7, NULL, 0);
    //xTaskCreatePinnedToCore(task_process, "process", 4 * 1024, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_pnet, "detection_pnet", 4 * 1024, &mtmn_config, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_rnet, "detection_rnet", 4 * 1024, &mtmn_config, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_onet, "detection_onet", 4 * 1024, &mtmn_config, 5, NULL, 1);
#if FACE_RECOGNITION
    xTaskCreatePinnedToCore(task_recognition, "recognition", 4 * 1024, NULL, 5, NULL, 1);
#endif
}
