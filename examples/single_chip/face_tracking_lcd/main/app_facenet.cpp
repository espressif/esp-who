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
QueueHandle_t gpst_tracker_init_queue = NULL;

static bool g_if_tracker_initialized = false;
static bool g_if_initialize_tracker = false;

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

template <typename O, typename R>
void draw_rectangle_rgb888_2(ArrayReal<uint8_t, O, 3> &image, struct rectangle_side_t<R> rectangle, int thickness)
{
#define DRAW_PIXEL_GREEN(BUF, X) \
    do                           \
    {                            \
        BUF[X + 0] = 0;          \
        BUF[X + 1] = 0xFF;       \
        BUF[X + 2] = 0;          \
    } while (0)

#define DRAW_PIXEL_RED(BUF, X) \
    do                         \
    {                          \
        BUF[X + 0] = 0;        \
        BUF[X + 1] = 0;        \
        BUF[X + 2] = 0xFF;     \
    } while (0)

    struct rectangle_coordinate_t<int> adjusted_rectangle;

    adjusted_rectangle.x1 = max(rectangle.x, 0);
    adjusted_rectangle.y1 = max(rectangle.y, 0);
    adjusted_rectangle.x2 = min(rectangle.x + rectangle.width - 1, image.get_shape(1) - 1);
    adjusted_rectangle.y2 = min(rectangle.y + rectangle.height - 1, image.get_shape(0) - 1);

    // rectangle box
    if (g_if_tracker_initialized)
    {
        for (int y = adjusted_rectangle.y1; y <= adjusted_rectangle.y2; y++)
        {
            for (size_t i = 0; i < thickness; i++)
            {
                DRAW_PIXEL_GREEN(image.get_item(), image.get_index_3d_y_x(y, adjusted_rectangle.x1 - i));
                DRAW_PIXEL_GREEN(image.get_item(), image.get_index_3d_y_x(y, adjusted_rectangle.x2 - i));
            }
        }
        for (int x = adjusted_rectangle.x1; x <= adjusted_rectangle.x2; x++)
        {
            for (size_t i = 0; i < thickness; i++)
            {
                DRAW_PIXEL_GREEN(image.get_item(), image.get_index_3d_y_x(adjusted_rectangle.y1 - i, x));
                DRAW_PIXEL_GREEN(image.get_item(), image.get_index_3d_y_x(adjusted_rectangle.y2 - i, x));
            }
        }
        for (size_t i = 0; i < thickness; i++)
        {
            for (size_t j = 0; j < thickness; j++)
            {
                DRAW_PIXEL_GREEN(image.get_item(), image.get_index_3d_y_x(adjusted_rectangle.y1 - i, adjusted_rectangle.x1 - j));
                DRAW_PIXEL_GREEN(image.get_item(), image.get_index_3d_y_x(adjusted_rectangle.y1 - i, adjusted_rectangle.x1 - j));
            }
        }
    }
    else
    {
        for (int y = adjusted_rectangle.y1; y <= adjusted_rectangle.y2; y++)
        {
            for (size_t i = 0; i < thickness; i++)
            {

                DRAW_PIXEL_RED(image.get_item(), image.get_index_3d_y_x(y, adjusted_rectangle.x1 - i));
                DRAW_PIXEL_RED(image.get_item(), image.get_index_3d_y_x(y, adjusted_rectangle.x2 - i));
            }
        }
        for (int x = adjusted_rectangle.x1; x <= adjusted_rectangle.x2; x++)
        {
            for (size_t i = 0; i < thickness; i++)
            {

                DRAW_PIXEL_RED(image.get_item(), image.get_index_3d_y_x(adjusted_rectangle.y1 - i, x));
                DRAW_PIXEL_RED(image.get_item(), image.get_index_3d_y_x(adjusted_rectangle.y2 - i, x));
            }
        }
        for (size_t i = 0; i < thickness; i++)
        {
            for (size_t j = 0; j < thickness; j++)
            {
                DRAW_PIXEL_RED(image.get_item(), image.get_index_3d_y_x(adjusted_rectangle.y1 - i, adjusted_rectangle.x1 - j));
                DRAW_PIXEL_RED(image.get_item(), image.get_index_3d_y_x(adjusted_rectangle.y1 - i, adjusted_rectangle.x1 - j));
            }
        }
    }
}

void task_transform_input(void *arg)
{
    camera_fb_t *fb = NULL;
    uint8_t *buf = NULL;
    int frame_counter = 50;

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

        if (xQueueSend(gpst_tracker_init_queue, &buf, 0) != pdTRUE)
            ESP_LOGE("Trans", "Send fail");

        if (frame_counter <= 0)
        {
            g_if_initialize_tracker = true;
        }
        else
        {
            frame_counter--;
            ESP_LOGI(TAG, "Tracking start in %d frame.", frame_counter);
        }

        ESP_LOGI(TAG, "Trans: wait fb %lldms, wait matrix %lldms, transform %lldms",
                 (time0[1] - time0[0]) / 1000,
                 (time0[2] - time0[1]) / 1000,
                 (time0[3] - time0[2]) / 1000);
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
    struct rectangle_side_t<int> default_target;
    default_target.height = 40;
    default_target.width = default_target.height;
    default_target.y = (int)((image.get_shape(0) - default_target.y) / 2);
    default_target.x = (int)((image.get_shape(1) - default_target.x) / 2);
    int64_t time[2];

    while (!g_if_tracker_initialized)
    {
        // TODO: receive image and target bounding box
        xQueueReceive(gpst_tracker_init_queue, &(tracker_info.item), portMAX_DELAY);
        image.set_item(tracker_info.item, false, false);

        // TODO: init tracker
        if (g_if_initialize_tracker && tracker.init(image, default_target))
        {
            g_if_tracker_initialized = true;
            g_if_initialize_tracker = false;
        }

        // TODO: draw bounding box on image
        draw_rectangle_rgb888_2<float, int>(image, default_target, 8);

        // TODO: send image to lcd
        if (xQueueSend(gpst_output_queue, &(tracker_info.item), 0) != pdTRUE)
            ESP_LOGE("Track", "Send fail");
    }

    while (g_if_tracker_initialized)
    {
        // TODO: receive image
        xQueueReceive(gpst_tracker_init_queue, &(tracker_info.item), portMAX_DELAY);
        image.set_item(tracker_info.item, false, false);

        // TODO: update target bounding box
        target = tracker.update(image);

        // TODO: draw bounding box on image
        draw_rectangle_rgb888_2<float, float>(image, target, 8);

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
    gpst_tracker_init_queue = xQueueCreate(3, sizeof(void *));

    for (int i = 0; i < 3; i++)
    {
        image_rgb888[i] = (uint8_t *)calloc(resolution[CAMERA_FRAME_SIZE][0] * resolution[CAMERA_FRAME_SIZE][1] * 3, sizeof(uint8_t));
        xQueueSend(gpst_image_matrix_queue, &(image_rgb888[i]), 0);
    }

    xTaskCreatePinnedToCore(task_transform_input, "transform_input", 4 * 1024, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(task_track, "tracking", 4 * 1024, NULL, 5, NULL, 1);
}
