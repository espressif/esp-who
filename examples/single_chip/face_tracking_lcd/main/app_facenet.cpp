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

#include "fd_forward.h"
#include "kcf.hpp"

static const char *TAG = "app_process";
QueueHandle_t gpst_image_matrix_queue = NULL;
QueueHandle_t gpst_input_queue = NULL;
QueueHandle_t gpst_pnet_queue = NULL;
QueueHandle_t gpst_rnet_queue = NULL;
QueueHandle_t gpst_onet_queue = NULL;
QueueHandle_t gpst_tracker_init_queue = NULL;
QueueHandle_t gpst_tracker_update_queue = NULL;
mtmn_config_t mtmn_config = {0};

bool g_if_tracker_initialized = false;

typedef struct
{
    uint8_t *item;
    box_array_t net_boxes;
} st_net_info;

typedef struct
{
    uint8_t *item;
    struct rectangle_coordinate_t<int> target;
} tracker_info;

void break_point()
{
    while (true)
    {
    }
}

void init_config(mtmn_config_t *mtmn_config)
{
    mtmn_config->type = FAST;
    mtmn_config->min_face = 80;
    mtmn_config->pyramid = 0.707;
    mtmn_config->pyramid_times = 4;
    mtmn_config->p_threshold.score = 0.6;
    mtmn_config->p_threshold.nms = 0.7;
    mtmn_config->p_threshold.candidate_number = 10;
    mtmn_config->r_threshold.score = 0.7;
    mtmn_config->r_threshold.nms = 0.7;
    mtmn_config->r_threshold.candidate_number = 5;
    mtmn_config->o_threshold.score = 0.7;
    mtmn_config->o_threshold.nms = 0.7;
    mtmn_config->o_threshold.candidate_number = 2;
}

template <typename O, typename R>
void draw_rectangle_rgb888_2(ArrayReal<uint8_t, O, 3> &image, struct rectangle_coordinate_t<R> rectangle)
{
#define DRAW_PIXEL_GREEN(BUF, X) \
    do                           \
    {                            \
        BUF[X + 0] = 0;          \
        BUF[X + 1] = 0xFF;       \
        BUF[X + 2] = 0;          \
    } while (0)

    rectangle.x1 = max(rectangle.x1, 0);
    rectangle.y1 = max(rectangle.y1, 0);
    rectangle.x2 = min(rectangle.x2, image.get_shape(1) - 1);
    rectangle.y2 = min(rectangle.y2, image.get_shape(0) - 1);

    // rectangle box
    for (int y = rectangle.y1; y <= rectangle.y2; y++)
    {
        DRAW_PIXEL_GREEN(image.get_item(), image.get_index_3d_y_x(y, rectangle.x1));
        DRAW_PIXEL_GREEN(image.get_item(), image.get_index_3d_y_x(y, rectangle.x2));
    }
    for (int x = rectangle.x1; x <= rectangle.x2; x++)
    {
        DRAW_PIXEL_GREEN(image.get_item(), image.get_index_3d_y_x(rectangle.y1, x));
        DRAW_PIXEL_GREEN(image.get_item(), image.get_index_3d_y_x(rectangle.y2, x));
    }
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

        if (g_if_tracker_initialized)
        {
            if (xQueueSend(gpst_tracker_update_queue, &buf, 0) != pdTRUE)
                ESP_LOGE("Trans", "Send fail");
        }
        else
        {
            if (xQueueSend(gpst_pnet_queue, &buf, 0) != pdTRUE)
                ESP_LOGE("Trans", "Send fail");
        }
        ESP_LOGI(TAG, "Trans: wait fb %lldms, wait matrix %lldms, transform %lldms",
                 (time0[1] - time0[0]) / 1000,
                 (time0[2] - time0[1]) / 1000,
                 (time0[3] - time0[2]) / 1000);
    }
}

void task_process(void *arg)
{ /*{{{*/
    size_t frame_num = 0;
    dl_matrix3du_t image_matrix;
    image_matrix.n = 1;
    image_matrix.w = resolution[CAMERA_FRAME_SIZE][0];
    image_matrix.h = resolution[CAMERA_FRAME_SIZE][1];
    image_matrix.c = 3;
    image_matrix.stride = 3 * image_matrix.w;
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
            draw_rectangle_rgb888(image_matrix.item, net_boxes, resolution[CAMERA_FRAME_SIZE][0]);
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
    image_matrix.w = resolution[CAMERA_FRAME_SIZE][0];
    image_matrix.h = resolution[CAMERA_FRAME_SIZE][1];
    image_matrix.c = 3;
    image_matrix.stride = 3 * image_matrix.w;
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
        ESP_LOGI(TAG, "Pnet: wait %lldms, process %lldms",
                 (time0[1] - time0[0]) / 1000,
                 (time0[2] - time0[1]) / 1000);
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
    image_matrix.w = resolution[CAMERA_FRAME_SIZE][0];
    image_matrix.h = resolution[CAMERA_FRAME_SIZE][1];
    image_matrix.c = 3;
    image_matrix.stride = 3 * image_matrix.w;
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
        ESP_LOGI(TAG, "Rnet: wait %lldms, process %lldms",
                 (time0[1] - time0[0]) / 1000,
                 (time0[2] - time0[1]) / 1000);
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
    image_matrix.w = resolution[CAMERA_FRAME_SIZE][0];
    image_matrix.h = resolution[CAMERA_FRAME_SIZE][1];
    image_matrix.c = 3;
    image_matrix.stride = 3 * image_matrix.w;
    image_matrix.item = NULL;
    st_net_info net_info = {0};
    tracker_info tracker_info = {0};
    int64_t time0[4] = {0};

    while (1)
    {
        time0[0] = esp_timer_get_time();
        xQueueReceive(gpst_onet_queue, &net_info, portMAX_DELAY);
        time0[1] = esp_timer_get_time();

        image_matrix.item = net_info.item;
        net_boxes = onet_forward(&image_matrix, &net_info.net_boxes, &net_config);
        time0[2] = esp_timer_get_time();

        free(net_info.net_boxes.box);

        if (net_boxes)
        {
            tracker_info.item = image_matrix.item;
            tracker_info.target.x1 = net_boxes->box[0].box_p[0];
            tracker_info.target.y1 = net_boxes->box[0].box_p[1];
            tracker_info.target.x2 = net_boxes->box[0].box_p[2];
            tracker_info.target.y2 = net_boxes->box[0].box_p[3];

            free(net_boxes->score);
            free(net_boxes->box);
            free(net_boxes->landmark);
            free(net_boxes);

            if (xQueueSend(gpst_tracker_init_queue, &tracker_info, 0) != pdTRUE)
                ESP_LOGE("Onet", "Send fail");
        }
        else
        {
            if (xQueueSend(gpst_output_queue, &(image_matrix.item), 0) != pdTRUE)
                ESP_LOGE("Onet", "Send fail");
        }

        ESP_LOGI(TAG, "Onet: wait %lldms, process %lldms",
                 (time0[1] - time0[0]) / 1000,
                 (time0[2] - time0[1]) / 1000);
    }
}

void task_track(void *arg)
{
    // TODO: init
    tracker_info tracker_info;
    int image_shape[] = {resolution[CAMERA_FRAME_SIZE][1], resolution[CAMERA_FRAME_SIZE][0], 3};
    ArrayReal<uint8_t, float, 3> image(image_shape, NOT_ALLOC);
    KCF<float> tracker;
    struct rectangle_side_t<float> target;
    int64_t time[2];

    while (!g_if_tracker_initialized)
    {
        // TODO: receive image and target bounding box
        xQueueReceive(gpst_tracker_init_queue, &tracker_info, portMAX_DELAY);
        image.set_item(tracker_info.item, false, false);

        // TODO: init tracker
        if (tracker.init(image, tracker_info.target))
            g_if_tracker_initialized = true;

        // TODO: draw bounding box on image
        draw_rectangle_rgb888_2<float, int>(image, tracker_info.target);

        // TODO: send image to lcd
        if (xQueueSend(gpst_output_queue, &(tracker_info.item), 0) != pdTRUE)
            ESP_LOGE("Track", "Send fail");
    }

    while (g_if_tracker_initialized)
    {
        // TODO: receive image
        xQueueReceive(gpst_tracker_update_queue, &(tracker_info.item), portMAX_DELAY);
        image.set_item(tracker_info.item, false, false);

        // TODO: update target bounding box
        target = tracker.update(image);
        tracker_info.target.x1 = target.x;
        tracker_info.target.y1 = target.y;
        tracker_info.target.x2 = target.x + target.width - 1;
        tracker_info.target.y2 = target.y + target.height - 1;

        // TODO: draw bounding box on image
        draw_rectangle_rgb888_2<float, int>(image, tracker_info.target);

        // TODO: send image to lcd
        if (xQueueSend(gpst_output_queue, &(tracker_info.item), 0) != pdTRUE)
            ESP_LOGE("Track", "Send fail");
    }
}

void app_facenet_main()
{
    uint8_t *image_rgb888[3];
    uint8_t *align_face[3];
    gpst_image_matrix_queue = xQueueCreate(3, sizeof(void *));
    gpst_pnet_queue = xQueueCreate(3, sizeof(st_net_info));
    gpst_rnet_queue = xQueueCreate(3, sizeof(st_net_info));
    gpst_onet_queue = xQueueCreate(3, sizeof(st_net_info));
    gpst_tracker_init_queue = xQueueCreate(3, sizeof(st_net_info));
    gpst_tracker_update_queue = xQueueCreate(3, sizeof(void *));

    for (int i = 0; i < 3; i++)
    {
        image_rgb888[i] = (uint8_t *)calloc(resolution[CAMERA_FRAME_SIZE][0] * resolution[CAMERA_FRAME_SIZE][1] * 3, sizeof(uint8_t));
        xQueueSend(gpst_image_matrix_queue, &(image_rgb888[i]), 0);
    }

    /* Load configuration for detection */
    init_config(&mtmn_config);

    xTaskCreatePinnedToCore(task_transform_input, "transform_input", 4 * 1024, NULL, 7, NULL, 0);
    // xTaskCreatePinnedToCore(task_process, "process", 4 * 1024, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_pnet, "detection_pnet", 4 * 1024, &mtmn_config, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_rnet, "detection_rnet", 4 * 1024, &mtmn_config, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_onet, "detection_onet", 4 * 1024, &mtmn_config, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_track, "tracking", 4 * 1024, NULL, 5, NULL, 1);
}
