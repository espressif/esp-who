#include "who_motion_detection.hpp"

#include "esp_log.h"
#include "esp_camera.h"

#include "dl_image.hpp"

static const char *TAG = "motion_detection";

static QueueHandle_t xQueueFrameI = NULL;
static QueueHandle_t xQueueEvent = NULL;
static QueueHandle_t xQueueFrameO = NULL;
static QueueHandle_t xQueueResult = NULL;

static bool gEvent = true;

static void task_process_handler(void *arg)
{
    camera_fb_t *frame1 = NULL;
    camera_fb_t *frame2 = NULL;

    while (true)
    {
        if (gEvent)
        {
            bool is_moved = false;
            if (xQueueReceive(xQueueFrameI, &(frame1), portMAX_DELAY))
            {
                if (xQueueReceive(xQueueFrameI, &(frame2), portMAX_DELAY))
                {
                    uint32_t moving_point_number = dl::image::get_moving_point_number((uint16_t *)frame1->buf, (uint16_t *)frame2->buf, frame1->height, frame1->width, 8, 15);
                    if (moving_point_number > 50)
                    {
                        ESP_LOGI(TAG, "Something moved!");
                        dl::image::draw_filled_rectangle((uint16_t *)frame2->buf, frame2->height, frame2->width, 0, 0, 20, 20);
                        is_moved = true;
                    }
                }
            }

            if (xQueueFrameO)
            {
                esp_camera_fb_return(frame1);
                xQueueSend(xQueueFrameO, &frame2, portMAX_DELAY);
            }
            else
            {
                esp_camera_fb_return(frame1);
                esp_camera_fb_return(frame2);
            }

            if (xQueueResult)
            {
                xQueueSend(xQueueResult, &is_moved, portMAX_DELAY);
            }
        }
    }
}

static void task_event_handler(void *arg)
{
    while (true)
    {
        xQueueReceive(xQueueEvent, &(gEvent), portMAX_DELAY);
    }
}

void register_motion_detection(QueueHandle_t frame_i, QueueHandle_t event,
                               QueueHandle_t result, QueueHandle_t frame_o)
{
    xQueueFrameI = frame_i;
    xQueueFrameO = frame_o;
    xQueueEvent = event;
    xQueueResult = result;
    xTaskCreatePinnedToCore(task_process_handler, TAG, 4 * 1024, NULL, 5, NULL, 1);
    if (xQueueEvent)
        xTaskCreatePinnedToCore(task_event_handler, TAG, 4 * 1024, NULL, 5, NULL, 1);
}
