/*
  * ESPRESSIF MIT License
  *
  * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
  *
  * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
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
  *
  */
#include "esp_system.h"
#include "esp_log.h"

#include "lssh_forward.h"

lssh_config_t lssh_get_config(lssh_model_t model, fptp_t min_face, int score_threshold, fptp_t nms_threshold, int image_height, int image_width, bool free_image, int mode)
{
    lssh_config_t config = {0};

    config.score_threshold = score_threshold;
    config.nms_threshold = nms_threshold;

    config.model = model;
    config.model_config.with_landmark = false;
    config.model_config.free_image = free_image;
    config.model_config.mode = mode;

    lssh_update_config(&config, min_face, image_height, image_width);
    return config;
}

void lssh_update_config(lssh_config_t *config, fptp_t min_face, int image_height, int image_width)
{

    fptp_t rough_resize_scale = config->model.module[0].anchors_shape[0][0] / min_face;

    config->resized_width = round(image_width * rough_resize_scale);
    config->resized_height = round(image_height * rough_resize_scale);

    int short_side = min(config->resized_height, config->resized_width);

    config->model_config.enabled_top_k = 1;
    for (size_t i = 1; i < config->model.modules_total; i++)
    {
        if (short_side >= config->model.module[i].boundary)
            config->model_config.enabled_top_k++;
        else
            break;
    }

    int stride = config->model.module[config->model_config.enabled_top_k - 1].stride;
    // config->resized_height = (int)(round(config->resized_height / stride)) * stride;
    // config->resized_width = (int)(round(config->resized_width / stride)) * stride;
    config->y_resize_scale = (fptp_t)image_height / config->resized_height;
    config->x_resize_scale = (fptp_t)image_width / config->resized_width;
}

image_list_t *__get_boxes_sigmoid(lssh_module_result_t stage, int threshold, int stride, int offset, int **anchors, fptp_t y_resize_scale, fptp_t x_resize_scale)
{
    qtp_t *score = stage.category->item;
    qtp_t *box_offset = stage.box_offset->item;
#if CONFIG_WITH_LANDMARK
    qtp_t *landmark_offset = stage.landmark_offset->item;
#endif
    image_box_t *redundant = (image_box_t *)dl_lib_calloc(stage.category->h * stage.category->w * stage.category->n, sizeof(image_box_t), 0);
    image_list_t *valid_list = (image_list_t *)dl_lib_calloc(1, sizeof(image_list_t), 0);
    int max_score = -SIZE_MAX;
    int max_score_c = 0;

    /*
        score format is (anchor_num, h, w, cls), box is (anchor_num, h, w, 4);
        while in the memory layout is (h, w, anchor, cls) and (h, w, anchor, 4)
    */

    for (size_t y = 0; y < stage.category->h; y++)
    {
        for (size_t x = 0; x < stage.category->w; x++)
        {
            for (size_t a = 0; a < stage.category->n; a++)
            {
                max_score = -SIZE_MAX;
                for (size_t c = 0; c < stage.category->c; c++)
                {
                    if (max_score < *score)
                    {
                        max_score = *score;
                        max_score_c = c;
                    }
                    score++;
                }

                // printf("%f\n", max_score);

                if (max_score > threshold)
                {
                    redundant[valid_list->len].category = max_score_c;
                    redundant[valid_list->len].score = max_score;
                    int center_y = (y * stride + offset) * y_resize_scale;
                    int center_x = (x * stride + offset) * x_resize_scale;
                    int anchor_h = anchors[a][0] * y_resize_scale;
                    int anchor_w = anchors[a][1] * x_resize_scale;
                    int shift = -stage.box_offset->exponent;
                    redundant[valid_list->len].box.box_p[0] = center_x - anchor_w / 2 + ((anchor_w * box_offset[0]) >> shift);
                    redundant[valid_list->len].box.box_p[1] = center_y - anchor_h / 2 + ((anchor_h * box_offset[1]) >> shift);
                    redundant[valid_list->len].box.box_p[2] = center_x + anchor_w / 2 + ((anchor_w * box_offset[2]) >> shift);
                    redundant[valid_list->len].box.box_p[3] = center_y + anchor_h / 2 + ((anchor_h * box_offset[3]) >> shift);
#if CONFIG_WITH_LANDMARK
                    redundant[valid_list->len].landmark.landmark_p[0] = center_x - anchor_w / 2 + ((anchor_w * landmark_offset[0]) >> shift);
                    redundant[valid_list->len].landmark.landmark_p[1] = center_y - anchor_h / 2 + ((anchor_h * landmark_offset[1]) >> shift);
                    redundant[valid_list->len].landmark.landmark_p[2] = center_x - anchor_w / 2 + ((anchor_w * landmark_offset[2]) >> shift);
                    redundant[valid_list->len].landmark.landmark_p[3] = center_y - anchor_h / 2 + ((anchor_h * landmark_offset[3]) >> shift);
                    redundant[valid_list->len].landmark.landmark_p[4] = center_x - anchor_w / 2 + ((anchor_w * landmark_offset[4]) >> shift);
                    redundant[valid_list->len].landmark.landmark_p[5] = center_y - anchor_h / 2 + ((anchor_h * landmark_offset[5]) >> shift);
                    redundant[valid_list->len].landmark.landmark_p[6] = center_x - anchor_w / 2 + ((anchor_w * landmark_offset[6]) >> shift);
                    redundant[valid_list->len].landmark.landmark_p[7] = center_y - anchor_h / 2 + ((anchor_h * landmark_offset[7]) >> shift);
                    redundant[valid_list->len].landmark.landmark_p[8] = center_x - anchor_w / 2 + ((anchor_w * landmark_offset[8]) >> shift);
                    redundant[valid_list->len].landmark.landmark_p[9] = center_y - anchor_h / 2 + ((anchor_h * landmark_offset[9]) >> shift);
#endif
                    valid_list->len++;
                }
                box_offset += 4;
#if CONFIG_WITH_LANDMARK
                landmark_offset += 10;
#endif
            }
        }
    }

    if (0 == valid_list->len)
    {
        dl_lib_free(redundant);
        dl_lib_free(valid_list);
        return NULL;
    }

    image_box_t *compact = (image_box_t *)dl_lib_calloc(valid_list->len, sizeof(image_box_t), 0);
    size_t i = 0;
    for (; i < valid_list->len - 1; i++)
    {
        compact[i] = redundant[i];
        compact[i].next = &(compact[i + 1]);
    }
    compact[i] = redundant[i];
    dl_lib_free(redundant);

    valid_list->head = compact;
    valid_list->origin_head = compact;

    return valid_list;
}

box_array_t *lssh_detect_object(dl_matrix3dq_t *image, lssh_config_t config)
{
    /**
     * @brief resize image
     * 
     */
    //dl_matrix3dq_t *resized_image = dl_matrix3dq_alloc(1, config.resized_width, config.resized_height, image->c, 0);
    // image_resize_linear(resized_image->item, image->item, resized_image->w, resized_image->h, resized_image->c, image->w, image->h);
    //assert(image->h == 240);
    //assert(image->w == 320);
    //assert(config.resized_height == 60);
    //assert(config.resized_width == 80);
    //image_resize_4(resized_image->item, image->item, resized_image->w, resized_image->h, resized_image->c, image->w, image->c);
    //image_zoom_in_twice(resized_image->item, resized_image->w, resized_image->h, resized_image->c, image->item, image->w, image->c);
    //printf("resize: %d, %d\n", resized_image->w, resized_image->h);

    /**
     * @brief net operation
     * 
     */
    lssh_module_result_t *module_result = config.model.op(image, config.model_config);

    /**
     * @brief filter by score
     * 
     */
    image_list_t **origin_head = (image_list_t **)dl_lib_calloc(config.model_config.enabled_top_k, sizeof(image_list_t *), 0);
    image_list_t all_box_list = {NULL};

    for (size_t i = 0; i < config.model_config.enabled_top_k; i++)
    {
        origin_head[i] = __get_boxes_sigmoid(module_result[i],
                                             config.score_threshold,
                                             config.model.module[i].stride,
                                             config.model.module[i].project_offset,
                                             config.model.module[i].anchors_shape,
                                             config.y_resize_scale,
                                             config.x_resize_scale);

        if (origin_head[i])
            image_sort_insert_by_score(&all_box_list, origin_head[i]);

        lssh_module_result_free(module_result[i]);
    }
    dl_lib_free(module_result);

    /**
     * @brief nms
     * 
     */
    image_nms_process(&all_box_list, config.nms_threshold, false);

    /**
     * @brief build up result
     * 
     */
    box_array_t *targets_list = NULL;
    if (all_box_list.len)
    {
        targets_list = (box_array_t *)dl_lib_calloc(1, sizeof(box_array_t), 0);
        targets_list->len = all_box_list.len;
        targets_list->category = (int *)dl_lib_calloc(targets_list->len, sizeof(int), 0);
        targets_list->score = (int *)dl_lib_calloc(targets_list->len, sizeof(int), 0);
        targets_list->box = (box_t *)dl_lib_calloc(targets_list->len, sizeof(box_t), 0);
#if CONFIG_WITH_LANDMARK
        targets_list->landmark = (landmark_t *)dl_lib_calloc(targets_list->len, sizeof(landmark_t), 0);
#endif

        image_box_t *t = all_box_list.head;
        for (int i = 0; i < all_box_list.len; i++, t = t->next)
        {
            targets_list->category[i] = t->category;
            targets_list->score[i] = t->score;
            targets_list->box[i] = t->box;
#if CONFIG_WITH_LANDMARK
            targets_list->landmark[i] = t->landmark;
#endif
        }
    }

    for (int i = 0; i < config.model_config.enabled_top_k; i++)
    {
        if (origin_head[i])
        {
            dl_lib_free(origin_head[i]->origin_head);
            dl_lib_free(origin_head[i]);
        }
    }
    dl_lib_free(origin_head);

    return targets_list;
}
