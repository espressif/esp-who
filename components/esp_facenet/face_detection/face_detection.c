#include <string.h>
#include <math.h>
#include "esp_system.h"
#include "face_detection.h"

static const char *TAG = "face_detection";

box_array_t *pnet_forward(dl_matrix3du_t *image)
{/*{{{*/
    mtmn_net_t *out;
    fptp_t scale = 1.0f * MIN_FACE / BOX_LEN;
    fptp_t scale_rev = 1.0f * BOX_LEN / MIN_FACE;
    image_list_t sorted_list[4] = {NULL};
    image_list_t *origin_head[4] = {NULL};   // store original point to free
    image_list_t all_box_list = {NULL};
    box_array_t *pnet_box_list = NULL;
    box_t *pnet_box = NULL;

    int width = round(image->w * scale);
    int height = round(image->h * scale);

    for (int i = 0; i < 4; i++)
    {
        if (DL_IMAGE_MIN(width, height) <= MIN_FACE)
            break;

        dl_matrix3du_t *in = dl_matrix3du_alloc(1, width, height, image->c);
        image_resize_linear(in->item, image->item, width, height, in->c, image->w, image->h);

        out = pnet(in);

        if (out)
        {
            do 
            {
                origin_head[i] = image_get_valid_boxes(out->category->item, out->offset->item, out->category->w, out->category->h, 0.7, scale_rev);

                dl_matrix3d_free(out->category);
                dl_matrix3d_free(out->offset);
                dl_matrix3d_free(out->landmark);
                free(out);

                if (NULL == origin_head[i])
                    break;

                image_sort_insert_by_score(&sorted_list[i], origin_head[i]);

                image_nms_process(&sorted_list[i], 0.5, true);
            } while(0);
        }
        dl_matrix3du_free(in);

        scale *= 0.7;
        scale_rev *= 10.0f/7;
        width = round(image->w * scale);
        height = round(image->h * scale);
    }
    for (int i = 0; i < 4; i++)
        image_sort_insert_by_score(&all_box_list, &sorted_list[i]);

    do
    {
        image_nms_process(&all_box_list, 0.6, false);
        if (0 == all_box_list.len)
            break;

        image_calibrate_by_offset(&all_box_list);

        pnet_box_list = (box_array_t *)calloc(1, sizeof(box_array_t));
        pnet_box = (box_t *)calloc(all_box_list.len, sizeof(box_t));

        image_box_t *t = all_box_list.head;

        // no need to store landmark
        for (int i = 0; i < all_box_list.len; i++, t = t->next)
            pnet_box[i] = t->box;

        pnet_box_list->box = pnet_box;
        pnet_box_list->len = all_box_list.len;

    } while(0);

    for (int i = 0; i < 4; i++)
    {
        if (origin_head[i])
        {
            free(origin_head[i]->origin_head);
            free(origin_head[i]);
        }
    }
    return pnet_box_list;
}/*}}}*/

box_array_t *ro_net_forward(dl_matrix3du_t *image, box_array_t *net_boxes, net_config_t *config)
{/*{{{*/
    int valid_count = 0;
    image_list_t valid_list = {NULL};
    image_list_t sorted_list = {NULL};
    dl_matrix3du_t *resize_image[2];
    image_box_t valid_box[4] = {NULL};
    box_t *net_box = NULL;
    landmark_t *net_landmark = NULL;
    box_array_t *net_box_list = NULL;

    if (NULL == net_boxes)
        return NULL;

    image_rect2sqr(net_boxes, image->w, image->h);
    for (int i = 0; i < net_boxes->len; i++)
    {
        int x = round(net_boxes->box[i].box_p[0]);
        int y = round(net_boxes->box[i].box_p[1]);
        int w = round(net_boxes->box[i].box_p[2]) - x + 1;
        int h = round(net_boxes->box[i].box_p[3]) - y + 1;
        resize_image[0] = dl_matrix3du_alloc(1, w, h, image->c);
        resize_image[1] = dl_matrix3du_alloc(1, config->w, config->h, image->c);

        dl_matrix3du_slice_copy(resize_image[0], image, x, y, w, h);

        image_resize_linear(resize_image[1]->item, resize_image[0]->item, config->w, config->h, image->c, w, h);
        mtmn_net_t *out = NULL;
        if (RNET == config->net_type)
        {
            out = rnet_with_score_verify(resize_image[1], config->score_threshold);
        }
        else if (ONET == config->net_type)
        {
            out = onet_with_score_verify(resize_image[1], config->score_threshold);
        }
        if (out)
        {
            assert(out->category->stride == 2);
            assert(out->offset->stride == 4);
            assert(out->offset->c == 4);
            valid_box[valid_count].score = out->category->item[1];
            valid_box[valid_count].box = net_boxes->box[i];
            valid_box[valid_count].offset.box_p[0] = out->offset->item[0];
            valid_box[valid_count].offset.box_p[1] = out->offset->item[1];
            valid_box[valid_count].offset.box_p[2] = out->offset->item[2];
            valid_box[valid_count].offset.box_p[3] = out->offset->item[3];
            if (ONET == config->net_type)
            {
                assert(out->landmark->stride == 10);
                memcpy(&(valid_box[valid_count].landmark), out->landmark->item, sizeof(landmark_t));
            }
            valid_box[valid_count].next = &(valid_box[valid_count + 1]);
            valid_count++;

            dl_matrix3d_free(out->category);
            dl_matrix3d_free(out->offset);
            dl_matrix3d_free(out->landmark);
            free(out);
        }
        dl_matrix3du_free(resize_image[0]);
        dl_matrix3du_free(resize_image[1]);
        if (RNET == config->net_type)
        {
            if (valid_count > 3)
            {
                break;
            }
        }
        else if (ONET == config->net_type)
        {
            if (valid_count > MAX_DETECTION - 1)
                break;
        }
    }
    valid_box[valid_count - 1].next = NULL;
    valid_list.head = valid_box;
    valid_list.len = valid_count;
    image_sort_insert_by_score(&sorted_list, &valid_list);

    do
    {
        image_nms_process(&sorted_list, config->nms_threshold, false);
        if (0 == sorted_list.len)
            break;

        if (ONET == config->net_type)
            image_landmark_calibrate(&sorted_list);
        image_calibrate_by_offset(&sorted_list);


        net_box_list = (box_array_t *)calloc(1, sizeof(box_array_t));
        net_box = (box_t *)calloc(sorted_list.len, sizeof(box_t));
        if (ONET == config->net_type)
            net_landmark = (landmark_t *)calloc(sorted_list.len, sizeof(landmark_t));

        image_box_t *t = sorted_list.head;

        for (int i = 0; i < sorted_list.len; i++, t = t->next)
        {
            net_box[i] = t->box;
            if (ONET == config->net_type)
                net_landmark[i] = t->landmark;
        }

        net_box_list->box = net_box;
        net_box_list->landmark = net_landmark;
        net_box_list->len = sorted_list.len;

    } while(0);
    
    return net_box_list;
}/*}}}*/

box_array_t *face_detect (dl_matrix3du_t *image_matrix)
{/*{{{*/
    box_array_t *pnet_boxes = pnet_forward(image_matrix);
    if (NULL == pnet_boxes)
        return NULL;

    net_config_t rnet_config = {0};
    rnet_config.net_type = RNET;
    rnet_config.file_name = "rnet_in";
    rnet_config.w = 24;
    rnet_config.h = 24;
    rnet_config.score_threshold = 0.7;
    rnet_config.nms_threshold = 0.7;

    box_array_t *rnet_boxes = ro_net_forward(image_matrix, pnet_boxes, &rnet_config);

    free(pnet_boxes->box);
    free(pnet_boxes);

    if (NULL == rnet_boxes)
        return NULL;

    net_config_t onet_config = {0};
    onet_config.net_type = ONET;
    onet_config.file_name = "onet_in";
    onet_config.w = 48;
    onet_config.h = 48;
    onet_config.score_threshold = 0.7;
    onet_config.nms_threshold = 0.4;

    box_array_t *onet_boxes = ro_net_forward(image_matrix, rnet_boxes, &onet_config);

    free(rnet_boxes->box);
    free(rnet_boxes);

    return onet_boxes;

}/*}}}*/


