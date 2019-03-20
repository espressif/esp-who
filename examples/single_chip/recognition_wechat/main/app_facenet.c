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

#include "esp_log.h"
#include "app_httpserver.h"
#include "fd_forward.h"
#include "image_util.h"
#include "app_main.h"

static const char *TAG = "app_process";

face_id_name_list st_face_list = {0};
static dl_matrix3du_t *aligned_face = NULL;

extern QueueHandle_t gpst_input_image;
extern QueueHandle_t gpst_output;

static inline mtmn_config_t app_mtmn_config()
{
    mtmn_config_t mtmn_config;
    mtmn_config.min_face = 80;
    mtmn_config.pyramid = 0.7;
    mtmn_config.p_threshold.score = 0.6;
    mtmn_config.p_threshold.nms = 0.7;
    mtmn_config.p_threshold.candidate_number = 100;
    mtmn_config.r_threshold.score = 0.6;
    mtmn_config.r_threshold.nms = 0.7;
    mtmn_config.r_threshold.candidate_number = 4;
    mtmn_config.o_threshold.score = 0.6;
    mtmn_config.o_threshold.nms = 0.4;
    mtmn_config.o_threshold.candidate_number = 1;

    return mtmn_config;
}

static inline box_array_t *do_detection(camera_fb_t *fb, dl_matrix3du_t *image_mat, mtmn_config_t *mtmn_config)
{
    if(!fmt2rgb888(fb->buf, fb->len, fb->format, image_mat->item))
    {
        ESP_LOGW(TAG, "fmt2rgb888 failed");
        return NULL;
    }

    box_array_t *net_boxes = face_detect(image_mat, mtmn_config);
    return net_boxes;
}


void task_process(void *arg)
{
    camera_fb_t * fb = NULL;
    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, 320, 240, 3);
    if (!image_matrix)
    { 
        ESP_LOGE(TAG, "dl_matrix3du_alloc failed");
        return;
    }

    mtmn_config_t mtmn_config = app_mtmn_config();
    http_img_process_result out_res = {0};
    //out_res.lock = xSemaphoreCreateBinary();

    out_res.image = image_matrix->item;

    while (1)
    {
        xQueueReceive(gpst_input_image, &fb, portMAX_DELAY);

        //xSemaphoreTake(out_res.lock, portMAX_DELAY);
        out_res.net_boxes = NULL;
        out_res.face_id = NULL;

        out_res.net_boxes = do_detection(fb, image_matrix, &mtmn_config);

        if (out_res.net_boxes)
        {
            if (g_state == START_DETECT)
            {
                xQueueSend(gpst_output, &out_res, portMAX_DELAY);
                continue;
            }

            // Get face ID
            if (align_face(out_res.net_boxes, image_matrix, aligned_face) == ESP_OK)
            {
                out_res.face_id = get_face_id(aligned_face);
            }
        }
        xQueueSend(gpst_output, &out_res, portMAX_DELAY);
    }

    dl_matrix3du_free(image_matrix);
}

void app_facenet_main()
{
    face_id_name_init(&st_face_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
    aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);
    read_face_id_from_flash_with_name(&st_face_list);

    xTaskCreatePinnedToCore(task_process, "process", 4 * 1024, NULL, 5, NULL, 1);
}
