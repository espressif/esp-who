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
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "image_util.h"
#include "esp_timer.h"

void image_zoom_in_twice(uint8_t *dimage,
                         int dw,
                         int dh,
                         int dc,
                         uint8_t *simage,
                         int sw,
                         int sc)
{
    for (int dyi = 0; dyi < dh; dyi++)
    {
        int _di = dyi * dw;

        int _si0 = dyi * 2 * sw;
        int _si1 = _si0 + sw;

        for (int dxi = 0; dxi < dw; dxi++)
        {
            int di = (_di + dxi) * dc;
            int si0 = (_si0 + dxi * 2) * sc;
            int si1 = (_si1 + dxi * 2) * sc;

            if (1 == dc)
            {
                dimage[di] = (uint8_t)((simage[si0] + simage[si0 + 1] + simage[si1] + simage[si1 + 1]) >> 2);
            }
            else if (3 == dc)
            {
                dimage[di] = (uint8_t)((simage[si0] + simage[si0 + 3] + simage[si1] + simage[si1 + 3]) >> 2);
                dimage[di + 1] = (uint8_t)((simage[si0 + 1] + simage[si0 + 4] + simage[si1 + 1] + simage[si1 + 4]) >> 2);
                dimage[di + 2] = (uint8_t)((simage[si0 + 2] + simage[si0 + 5] + simage[si1 + 2] + simage[si1 + 5]) >> 2);
            }
            else
            {
                for (int dci = 0; dci < dc; dci++)
                {
                    dimage[di + dci] = (uint8_t)((simage[si0 + dci] + simage[si0 + 3 + dci] + simage[si1 + dci] + simage[si1 + 3 + dci] + 2) >> 2);
                }
            }
        }
    }
    return;
}

void image_resize_linear(uint8_t *dst_image, uint8_t *src_image, int dst_w, int dst_h, int dst_c, int src_w, int src_h)
{ /*{{{*/
    float scale_x = (float)src_w / dst_w;
    float scale_y = (float)src_h / dst_h;

    int dst_stride = dst_c * dst_w;
    int src_stride = dst_c * src_w;

    if (fabs(scale_x - 2) <= 1e-6 && fabs(scale_y - 2) <= 1e-6)
    {
        image_zoom_in_twice(
            dst_image,
            dst_w,
            dst_h,
            dst_c,
            src_image,
            src_w,
            dst_c);
    }
    else
    {
        for (int y = 0; y < dst_h; y++)
        {
            float fy[2];
            fy[0] = (float)((y + 0.5) * scale_y - 0.5); // y
            int src_y = (int)fy[0];                     // y1
            fy[0] -= src_y;                             // y - y1
            fy[1] = 1 - fy[0];                          // y2 - y
            src_y = DL_IMAGE_MAX(0, src_y);
            src_y = DL_IMAGE_MIN(src_y, src_h - 2);

            for (int x = 0; x < dst_w; x++)
            {
                float fx[2];
                fx[0] = (float)((x + 0.5) * scale_x - 0.5); // x
                int src_x = (int)fx[0];                     // x1
                fx[0] -= src_x;                             // x - x1
                if (src_x < 0)
                {
                    fx[0] = 0;
                    src_x = 0;
                }
                if (src_x > src_w - 2)
                {
                    fx[0] = 0;
                    src_x = src_w - 2;
                }
                fx[1] = 1 - fx[0]; // x2 - x

                for (int c = 0; c < dst_c; c++)
                {
                    dst_image[y * dst_stride + x * dst_c + c] = round(src_image[src_y * src_stride + src_x * dst_c + c] * fx[1] * fy[1] + src_image[src_y * src_stride + (src_x + 1) * dst_c + c] * fx[0] * fy[1] + src_image[(src_y + 1) * src_stride + src_x * dst_c + c] * fx[1] * fy[0] + src_image[(src_y + 1) * src_stride + (src_x + 1) * dst_c + c] * fx[0] * fy[0]);
                }
            }
        }
    }
} /*}}}*/

void image_cropper(uint8_t *rot_data, uint8_t *src_data, int rot_w, int rot_h, int rot_c, int src_w, int src_h, float rotate_angle, float ratio, float *center)
{ /*{{{*/
    int rot_stride = rot_w * rot_c;
    float rot_w_start = 0.5f - (float)rot_w / 2;
    float rot_h_start = 0.5f - (float)rot_h / 2;

    //rotate_angle must be radius
    float si = sin(rotate_angle);
    float co = cos(rotate_angle);

    int src_stride = src_w * rot_c;

    for (int y = 0; y < rot_h; y++)
    {
        for (int x = 0; x < rot_w; x++)
        {
            float xs, ys, xr, yr;
            xs = ratio * (rot_w_start + x);
            ys = ratio * (rot_h_start + y);

            xr = xs * co + ys * si;
            yr = -xs * si + ys * co;

            float fy[2];
            fy[0] = center[1] + yr; // y
            int src_y = (int)fy[0]; // y1
            fy[0] -= src_y;         // y - y1
            fy[1] = 1 - fy[0];      // y2 - y
            src_y = DL_IMAGE_MAX(0, src_y);
            src_y = DL_IMAGE_MIN(src_y, src_h - 2);

            float fx[2];
            fx[0] = center[0] + xr; // x
            int src_x = (int)fx[0]; // x1
            fx[0] -= src_x;         // x - x1
            if (src_x < 0)
            {
                fx[0] = 0;
                src_x = 0;
            }
            if (src_x > src_w - 2)
            {
                fx[0] = 0;
                src_x = src_w - 2;
            }
            fx[1] = 1 - fx[0]; // x2 - x

            for (int c = 0; c < rot_c; c++)
            {
                rot_data[y * rot_stride + x * rot_c + c] = round(src_data[src_y * src_stride + src_x * rot_c + c] * fx[1] * fy[1] + src_data[src_y * src_stride + (src_x + 1) * rot_c + c] * fx[0] * fy[1] + src_data[(src_y + 1) * src_stride + src_x * rot_c + c] * fx[1] * fy[0] + src_data[(src_y + 1) * src_stride + (src_x + 1) * rot_c + c] * fx[0] * fy[0]);
            }
        }
    }
} /*}}}*/

void image_sort_insert_by_score(image_list_t *image_sorted_list, const image_list_t *insert_list)
{ /*{{{*/
    if (insert_list == NULL || insert_list->head == NULL)
        return;
    image_box_t *box = insert_list->head;
    if (NULL == image_sorted_list->head)
    {
        image_sorted_list->head = insert_list->head;
        box = insert_list->head->next;
        image_sorted_list->head->next = NULL;
    }
    image_box_t *head = image_sorted_list->head;

    while (box)
    {
        // insert in head
        if (box->score > head->score)
        {
            image_box_t *tmp = box;
            box = box->next;
            tmp->next = head;
            head = tmp;
        }
        else
        {
            image_box_t *curr = head->next;
            image_box_t *prev = head;
            while (curr)
            {
                if (box->score > curr->score)
                {
                    image_box_t *tmp = box;
                    box = box->next;
                    tmp->next = curr;
                    prev->next = tmp;
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
            // insert in tail
            if (NULL == curr)
            {
                image_box_t *tmp = box;
                box = box->next;
                tmp->next = NULL;
                prev->next = tmp;
            }
        }
    }
    image_sorted_list->head = head;
    image_sorted_list->len += insert_list->len;
} /*}}}*/

image_list_t *image_get_valid_boxes(fptp_t *score,
                                    fptp_t *offset,
                                    fptp_t *landmark,
                                    int width,
                                    int height,
                                    int anchor_number,
                                    int *anchors_size,
                                    fptp_t score_threshold,
                                    int stride,
                                    fptp_t y_resize_scale,
                                    fptp_t x_resize_scale,
                                    bool do_regression)
{ /*{{{*/
    typedef struct
    {
        short x;
        short y;
        int index;
        uc_t anchor_index;
    } valid_index_t;
    valid_index_t *valid_indexes = (valid_index_t *)dl_lib_calloc(width * height, sizeof(valid_index_t), 0);
    int valid_count = 0;
    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (size_t c = 0; c < anchor_number; c++)
            {
                if (score[2 * index + 1] > score_threshold)
                {
                    valid_indexes[valid_count].x = x;
                    valid_indexes[valid_count].y = y;
                    valid_indexes[valid_count].index = index;
                    valid_indexes[valid_count].anchor_index = c;
                    valid_count++;
                }
                index++;
            }
        }
    }

    if (0 == valid_count)
    {
        dl_lib_free(valid_indexes);
        return NULL;
    }

    image_box_t *valid_box = (image_box_t *)dl_lib_calloc(valid_count, sizeof(image_box_t), 0);
    image_list_t *valid_list = (image_list_t *)dl_lib_calloc(1, sizeof(image_list_t), 0);
    valid_list->head = valid_box;
    valid_list->origin_head = valid_box;
    valid_list->len = valid_count;

    if (do_regression)
    {
        for (int i = 0; i < valid_count; i++)
        {
            // TODO: score
            valid_box[i].score = score[2 * valid_indexes[i].index + 1];

            // TODO: box
            int anchor_size = anchors_size[valid_indexes[i].anchor_index];
            int anchor_left_up_x = valid_indexes[i].x * stride;
            int anchor_left_up_y = valid_indexes[i].y * stride;
            

            valid_box[i].box.box_p[0] = (offset[valid_indexes[i].index * 4 + 0] * anchor_size + anchor_left_up_x) / x_resize_scale;
            valid_box[i].box.box_p[1] = (offset[valid_indexes[i].index * 4 + 1] * anchor_size + anchor_left_up_y) / y_resize_scale;
            valid_box[i].box.box_p[2] = (offset[valid_indexes[i].index * 4 + 2] * anchor_size + anchor_left_up_x + anchor_size - 1) / x_resize_scale;
            valid_box[i].box.box_p[3] = (offset[valid_indexes[i].index * 4 + 3] * anchor_size + anchor_left_up_y + anchor_size - 1) / y_resize_scale;

            // TODO: landmark
            if (landmark)
            {
                valid_box[i].landmark.landmark_p[0] = (landmark[valid_indexes[i].index * 10 + 0] * anchor_size + anchor_left_up_x) / x_resize_scale;
                valid_box[i].landmark.landmark_p[1] = (landmark[valid_indexes[i].index * 10 + 1] * anchor_size + anchor_left_up_y) / y_resize_scale;
                valid_box[i].landmark.landmark_p[2] = (landmark[valid_indexes[i].index * 10 + 2] * anchor_size + anchor_left_up_x) / x_resize_scale;
                valid_box[i].landmark.landmark_p[3] = (landmark[valid_indexes[i].index * 10 + 3] * anchor_size + anchor_left_up_y) / y_resize_scale;
                valid_box[i].landmark.landmark_p[4] = (landmark[valid_indexes[i].index * 10 + 4] * anchor_size + anchor_left_up_x) / x_resize_scale;
                valid_box[i].landmark.landmark_p[5] = (landmark[valid_indexes[i].index * 10 + 5] * anchor_size + anchor_left_up_y) / y_resize_scale;
                valid_box[i].landmark.landmark_p[6] = (landmark[valid_indexes[i].index * 10 + 6] * anchor_size + anchor_left_up_x) / x_resize_scale;
                valid_box[i].landmark.landmark_p[7] = (landmark[valid_indexes[i].index * 10 + 7] * anchor_size + anchor_left_up_y) / y_resize_scale;
                valid_box[i].landmark.landmark_p[8] = (landmark[valid_indexes[i].index * 10 + 8] * anchor_size + anchor_left_up_x) / x_resize_scale;
                valid_box[i].landmark.landmark_p[9] = (landmark[valid_indexes[i].index * 10 + 9] * anchor_size + anchor_left_up_y) / y_resize_scale;
            }
            valid_box[i].next = &(valid_box[i + 1]);
        }
    }
    else
    {
        for (int i = 0; i < valid_count; i++)
        {
            valid_box[i].score = score[2 * valid_indexes[i].index + 1];

            int anchor_size = anchors_size[valid_indexes[i].anchor_index];
            valid_box[i].box.box_p[0] = valid_indexes[i].x / x_resize_scale * stride;
            valid_box[i].box.box_p[1] = valid_indexes[i].y / y_resize_scale * stride;
            valid_box[i].box.box_p[2] = valid_box[i].box.box_p[0] + anchor_size / x_resize_scale;
            valid_box[i].box.box_p[3] = valid_box[i].box.box_p[1] + anchor_size / y_resize_scale;

            valid_box[i].offset.box_p[0] = offset[valid_indexes[i].index * 4 + 0];
            valid_box[i].offset.box_p[1] = offset[valid_indexes[i].index * 4 + 1];
            valid_box[i].offset.box_p[2] = offset[valid_indexes[i].index * 4 + 2];
            valid_box[i].offset.box_p[3] = offset[valid_indexes[i].index * 4 + 3];

            if (landmark)
                for (size_t j = 0; j < 10; j++)
                    valid_box[i].landmark.landmark_p[j] = landmark[valid_indexes[i].index * 10 + j];

            valid_box[i].next = &(valid_box[i + 1]);
        }
    }
    valid_box[valid_count - 1].next = NULL;

    dl_lib_free(valid_indexes);

    return valid_list;
} /*}}}*/

void image_nms_process(image_list_t *image_list, fptp_t nms_threshold, int same_area)
{ /*{{{*/
    /**** Init ****/
    int num_supressed = 0;
    image_box_t *head = image_list->head;

    /**** Compute Box Area ****/
    fptp_t kept_box_area = 0;
    fptp_t other_box_area = 0;
    if (same_area)
    {
        image_get_area(&(head->box), &kept_box_area);
        other_box_area = kept_box_area;
    }

    /**** Compare IOU ****/
    image_box_t *kept_box = head;
    while (kept_box)
    {
        image_box_t *other_box = kept_box->next;
        image_box_t *prev = kept_box;
        while (other_box)
        {

            box_t inter_box;
            inter_box.box_p[0] = DL_IMAGE_MAX(kept_box->box.box_p[0], other_box->box.box_p[0]);
            inter_box.box_p[1] = DL_IMAGE_MAX(kept_box->box.box_p[1], other_box->box.box_p[1]);
            inter_box.box_p[2] = DL_IMAGE_MIN(kept_box->box.box_p[2], other_box->box.box_p[2]);
            inter_box.box_p[3] = DL_IMAGE_MIN(kept_box->box.box_p[3], other_box->box.box_p[3]);

            fptp_t inter_w, inter_h;
            image_get_width_and_height(&inter_box, &inter_w, &inter_h);

            if (inter_w > 0 && inter_h > 0)
            {
                if (!same_area)
                {
                    image_get_area(&(kept_box->box), &kept_box_area);
                    image_get_area(&(other_box->box), &other_box_area);
                }
                fptp_t inter_area = inter_w * inter_h;
                fptp_t iou = inter_area / (kept_box_area + other_box_area - inter_area);
                if (iou > nms_threshold)
                {
                    num_supressed++;
                    // Delete duplicated box
                    // Here we cannot free a single box, because these boxes are allocated by calloc, we need to free all the calloced memory together.
                    prev->next = other_box->next;
                    other_box = other_box->next;
                    continue;
                }
            }
            prev = other_box;
            other_box = other_box->next;
        }
        kept_box = kept_box->next;
    }

    image_list->len -= num_supressed;
} /*}}}*/

void transform_input_image(uint8_t *m, uint16_t *bmp, int count)
{ /*{{{*/
    uc_t dst[24];
    for (int x = 0; x < count; x += 8)
    {
        rgb565_to_888(*bmp++, dst);
        rgb565_to_888(*bmp++, dst + 3);
        rgb565_to_888(*bmp++, dst + 6);
        rgb565_to_888(*bmp++, dst + 9);
        rgb565_to_888(*bmp++, dst + 12);
        rgb565_to_888(*bmp++, dst + 15);
        rgb565_to_888(*bmp++, dst + 18);
        rgb565_to_888(*bmp++, dst + 21);
        memcpy(m + x * 3, dst, 24 * sizeof(uint8_t));
    }
} /*}}}*/

void transform_output_image(uint16_t *bmp, uint8_t *m, int count)
{ /*{{{*/
    for (int x = 0; x < count; x++)
    {
        rgb888_to_565(bmp, m[2], m[1], m[0]);
        bmp++;
        m += 3;
    }
} /*}}}*/

void transform_output_image_adjustable(uint16_t *bmp, uint8_t *m, int src_w, int src_h, int dst_w, int dst_h)
{ /*{{{*/
    if (src_w == dst_w && src_h == dst_h)
        transform_output_image(bmp, m, src_h*src_w);
    else
    {
        for (int y = 0; y < dst_h; y++)
        {
            m += (src_w - dst_w) / 2 * 3;
            for (int x = 0; x < dst_w; x++)
            {
                rgb888_to_565(bmp, m[2], m[1], m[0]);
                bmp++;
                m += 3;
            }
            m += (src_w - dst_w) / 2 * 3;
        }
    }
} /*}}}*/

void draw_rectangle_rgb565(uint16_t *buf, box_array_t *boxes, int width, bool green)
{ /*{{{*/
    uint16_t p[14];
    for (int i = 0; i < boxes->len; i++)
    {
        // rectangle box
        for (int j = 0; j < 4; j++)
            p[j] = (uint16_t)boxes->box[i].box_p[j];

        // landmark
        if (boxes->landmark)
        {
            for (int j = 0; j < 10; j++)
                p[j + 4] = (uint16_t)boxes->landmark[i].landmark_p[j];
        }

        if ((p[2] < p[0]) || (p[3] < p[1]))
            return;

#define RGB565_GREEN_REVERSE    0xE007
#define RGB565_RED_REVERSE      0xF8
        int color = green ? RGB565_GREEN_REVERSE : RGB565_RED_REVERSE;
        // rectangle box
        for (int w = p[0]; w < p[2] + 1; w++)
        {
            int x1 = (p[1] * width + w);
            int x2 = (p[3] * width + w);
            buf[x1] = color;
            buf[x2] = color;
        }
        for (int h = p[1]; h < p[3] + 1; h++)
        {
            int y1 = (h * width + p[0]);
            int y2 = (h * width + p[2]);
            buf[y1] = color;
            buf[y2] = color;
        }

        // landmark
        if (boxes->landmark)
        {
            for (int j = 0; j < 10; j += 2)
            {
                int x = p[j + 5] * width + p[j + 4];
                buf[x] = RGB565_RED_REVERSE;
                buf[x + 1] = RGB565_RED_REVERSE;
                buf[x + 2] = RGB565_RED_REVERSE;

                buf[width + x] = RGB565_RED_REVERSE;
                buf[width + x + 1] = RGB565_RED_REVERSE;
                buf[width + x + 2] = RGB565_RED_REVERSE;

                buf[2 * width + x] = RGB565_RED_REVERSE;
                buf[2 * width + x + 1] = RGB565_RED_REVERSE;
                buf[2 * width + x + 2] = RGB565_RED_REVERSE;
            }
        }
    }
} /*}}}*/

void draw_rectangle_rgb888(uint8_t *buf, box_array_t *boxes, int width)
{ /*{{{*/
    uint16_t p[14];
    for (int i = 0; i < boxes->len; i++)
    {
        // rectangle box
        for (int j = 0; j < 4; j++)
            p[j] = (uint16_t)boxes->box[i].box_p[j];

        // landmark
        if (boxes->landmark)
        {
            for (int j = 0; j < 10; j++)
                p[j + 4] = (uint16_t)boxes->landmark[i].landmark_p[j];
        }

        if ((p[2] < p[0]) || (p[3] < p[1]))
            return;

#define DRAW_PIXEL_GREEN(BUF, X) \
    do                           \
    {                            \
        BUF[X + 0] = 0;          \
        BUF[X + 1] = 0xFF;       \
        BUF[X + 2] = 0;          \
    } while (0)

        // rectangle box
        for (int w = p[0]; w < p[2] + 1; w++)
        {
            int x1 = (p[1] * width + w) * 3;
            int x2 = (p[3] * width + w) * 3;
            DRAW_PIXEL_GREEN(buf, x1);
            DRAW_PIXEL_GREEN(buf, x2);
        }
        for (int h = p[1]; h < p[3] + 1; h++)
        {
            int y1 = (h * width + p[0]) * 3;
            int y2 = (h * width + p[2]) * 3;
            DRAW_PIXEL_GREEN(buf, y1);
            DRAW_PIXEL_GREEN(buf, y2);
        }

        // landmark
        if (boxes->landmark)
        {
            for (int j = 0; j < 10; j += 2)
            {
                int x = (p[j + 5] * width + p[j + 4]) * 3;
                DRAW_PIXEL_GREEN(buf, x);
                DRAW_PIXEL_GREEN(buf, x + 3);
                DRAW_PIXEL_GREEN(buf, x + 6);

                DRAW_PIXEL_GREEN(buf, width * 3 + x);
                DRAW_PIXEL_GREEN(buf, width * 3 + x + 3);
                DRAW_PIXEL_GREEN(buf, width * 3 + x + 6);

                DRAW_PIXEL_GREEN(buf, width * 6 + x);
                DRAW_PIXEL_GREEN(buf, width * 6 + x + 3);
                DRAW_PIXEL_GREEN(buf, width * 6 + x + 6);
            }
        }
    }
} /*}}}*/

void image_abs_diff(uint8_t *dst, uint8_t *src1, uint8_t *src2, int count)
{
    while (count > 0)
    {
        *dst = (uint8_t)abs((int)*src1 - (int)*src2);
        dst++;
        src1++;
        src2++;
        count--;
    }
}

void image_threshold(uint8_t *dst, uint8_t *src, int threshold, int value, int count, en_threshold_mode mode)
{
    int l_val = 0;
    int r_val = 0;
    switch (mode)
    {
    case BINARY:
        r_val = value;
        break;
    default:
        break;
    }
    while (count > 0)
    {
        *dst = (*src > threshold) ? r_val : l_val;

        dst++;
        src++;
        count--;
    }
}

void image_kernel_get_min(uint8_t *dst, uint8_t *src, int w, int h, int c, int stride)
{
    uint8_t min1 = 255;
    uint8_t min2 = 255;
    uint8_t min3 = 255;

    if (c == 3)
    {
        for (int j = 0; j < h; j++)
        {
            for (int i = 0; i < w; i++)
            {
                if (src[0] < min1)
                    min1 = src[0];
                if (src[1] < min2)
                    min2 = src[1];
                if (src[2] < min3)
                    min3 = src[2];
                src += 3;
            }
            src += stride - w * 3;
        }
        dst[0] = min1;
        dst[1] = min2;
        dst[2] = min3;
    }
    else if (c == 1)
    {
        for (int j = 0; j < h; j++)
        {
            for (int i = 0; i < w; i++)
            {
                if (src[0] < min1)
                    min1 = src[0];
                src += 1;
            }
            src += stride - w;
        }
        dst[0] = min1;
    }
    else
    {
    }
}
/*
 * By default 3x3 Kernel, so padding is 2
 */
void image_erode(uint8_t *dst, uint8_t *src, int src_w, int src_h, int src_c)
{
    int stride = src_w * src_c;

    // 1st row, 1st col
    image_kernel_get_min(dst, src, 2, 2, src_c, stride);
    dst += src_c;

    // 1st row
    for (int i = 1; i < src_w - 1; i++)
    {
        image_kernel_get_min(dst, src, 3, 2, src_c, stride);
        dst += src_c;
        src += src_c;
    }

    // 1st row, last col
    image_kernel_get_min(dst, src, 2, 2, src_c, stride);
    dst += src_c;
    src -= src_c * (src_w - 2);

    for (int j = 1; j < src_h - 1; j++)
    {
        // 1st col
        image_kernel_get_min(dst, src, 2, 3, src_c, stride);
        dst += src_c;

        for (int i = 1; i < src_w - 1; i++)
        {
            image_kernel_get_min(dst, src, 3, 3, src_c, stride);
            dst += src_c;
            src += src_c;
        }

        // last col
        image_kernel_get_min(dst, src, 2, 3, src_c, stride);
        dst += src_c;
        src += src_c * 2;
    }

    // last row
    image_kernel_get_min(dst, src, 2, 2, src_c, stride);
    dst += src_c;

    for (int i = 1; i < src_w - 1; i++)
    {
        image_kernel_get_min(dst, src, 3, 2, src_c, stride);
        dst += src_c;
        src += src_c;
    }

    // last row, last col
    image_kernel_get_min(dst, src, 2, 2, src_c, stride);
}

Matrix *matrix_alloc(int h, int w)
{
    Matrix *r = calloc(1, sizeof(Matrix));
    r->w = w;
    r->h = h;
    r->array = calloc(h, sizeof(matrixType *));
    for (int i = 0; i < h; i++)
    {
        r->array[i] = calloc(w, sizeof(matrixType));
    }
    return r;
}

void matrix_free(Matrix *m)
{
    for (int i = 0; i < m->h; i++)
    {
        free(m->array[i]);
    }
    free(m->array);
    free(m);
    //m = NULL;
}

void matrix_print(Matrix *m)
{
    printf("Matrix: %dx%d\n", m->h, m->w);
    for (int i = 0; i < m->h; i++)
    {
        for (int j = 0; j < m->w; j++)
        {
            printf("%f ", m->array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

Matrix *malloc_rand_matrix(int h, int w, int thresh)
{
    Matrix *m = matrix_alloc(h, w);
    unsigned int seed = esp_timer_get_time();
    srand(seed);
    for (int i = 0; i < m->h; i++)
    {
        for (int j = 0; j < m->w; j++)
        {
            m->array[i][j] = rand() % thresh;
        }
    }
    return m;
}

Matrix *get_affine_transform(float *srcx, float *srcy, float *dstx, float *dsty)
{
    Matrix *m = matrix_alloc(2, 3);
    float A[3][2] = {0};
    float Ainv[3][3] = {0};
    for (int i = 0; i < 3; i++)
    {
        A[i][0] = srcx[i];
        A[i][1] = srcy[i];
    }
    float Adet = (A[0][0] * A[1][1] + A[0][1] * A[2][0] + A[1][0] * A[2][1]) - (A[2][0] * A[1][1] + A[1][0] * A[0][1] + A[0][0] * A[2][1]);
    if (Adet == 0)
    {
        printf("the src is linearly dependent\n");
        return NULL;
    }
    Ainv[0][0] = (A[1][1] - A[2][1]) / Adet;
    Ainv[0][1] = (A[2][1] - A[0][1]) / Adet;
    Ainv[0][2] = (A[0][1] - A[1][1]) / Adet;
    Ainv[1][0] = (A[2][0] - A[1][0]) / Adet;
    Ainv[1][1] = (A[0][0] - A[2][0]) / Adet;
    Ainv[1][2] = (A[1][0] - A[0][0]) / Adet;
    Ainv[2][0] = (A[1][0] * A[2][1] - A[2][0] * A[1][1]) / Adet;
    Ainv[2][1] = (A[2][0] * A[0][1] - A[0][0] * A[2][1]) / Adet;
    Ainv[2][2] = (A[0][0] * A[1][1] - A[0][1] * A[1][0]) / Adet;

    for (int i = 0; i < 3; i++)
    {
        m->array[0][i] = Ainv[i][0] * dstx[0] + Ainv[i][1] * dstx[1] + Ainv[i][2] * dstx[2];
        m->array[1][i] = Ainv[i][0] * dsty[0] + Ainv[i][1] * dsty[1] + Ainv[i][2] * dsty[2];
    }
    return m;
}

Matrix *get_inv_affine_matrix(Matrix *m)
{
    Matrix *minv = matrix_alloc(2, 3);
    float mdet = (m->array[0][0]) * (m->array[1][1]) - (m->array[1][0]) * (m->array[0][1]);
    if (mdet == 0)
    {
        printf("the matrix m is wrong !\n");
        return NULL;
    }

    minv->array[0][0] = m->array[1][1] / mdet;
    minv->array[0][1] = -(m->array[0][1] / mdet);
    minv->array[0][2] = ((m->array[0][1]) * (m->array[1][2]) - (m->array[0][2]) * (m->array[1][1])) / mdet;
    minv->array[1][0] = -(m->array[1][0]) / mdet;
    minv->array[1][1] = (m->array[0][0]) / mdet;
    minv->array[1][2] = ((m->array[0][2]) * (m->array[1][0]) - (m->array[0][0]) * (m->array[1][2])) / mdet;
    return minv;
}

Matrix *get_inverse_matrix(Matrix *m)
{
    if (m->w != m->h)
    {
        printf("the input is not a square matrix !\n");
        return NULL;
    }

    Matrix *matw = matrix_alloc(m->h, 2 * (m->w));
    Matrix *inv = matrix_alloc(m->h, m->w);
    matrixType **w = matw->array;
    float eps = 1e-6;

    for (int i = 0; i < matw->h; i++)
    {
        for (int j = 0; j < m->w; j++)
        {
            w[i][j] = m->array[i][j];
        }
        w[i][(m->w) + i] = 1;
    }

    for (int i = 0; i < matw->h; i++)
    {
        if (fabs(w[i][i]) < eps)
        {
            int j;
            for (j = i + 1; j < matw->h; j++)
            {
                if (fabs(w[j][i]) > eps)
                    break;
            }
            if (j == matw->h)
            {
                printf("This matrix is irreversible!\n");
                return NULL;
            }
            for (int k = i; k < matw->w; k++)
            {
                w[i][k] += w[j][k];
            }
        }
        float factor = w[i][i];
        for (int k = i; k < matw->w; k++)
        {
            w[i][k] /= factor;
        }
        for (int k = i + 1; k < matw->h; k++)
        {
            factor = -w[k][i];
            for (int l = i; l < matw->w; l++)
            {
                w[k][l] += (factor * w[i][l]);
            }
        }
    }
    for (int i = (matw->h) - 1; i > 0; i--)
    {
        for (int j = i - 1; j >= 0; j--)
        {
            float factor = -w[j][i];
            for (int k = i; k < matw->w; k++)
            {
                w[j][k] += (factor * w[i][k]);
            }
        }
    }
    for (int i = 0; i < m->h; i++)
    {
        for (int j = 0; j < m->w; j++)
        {
            inv->array[i][j] = w[i][(m->h) + j];
        }
    }
    return inv;
}

Matrix *get_perspective_transform(float *srcx, float *srcy, float *dstx, float *dsty)
{
    Matrix *m = matrix_alloc(3, 3);
    Matrix *A = matrix_alloc(8, 8);

    for (int i = 0; i < 4; i++)
    {
        A->array[i][0] = srcx[i];
        A->array[i][1] = srcy[i];
        A->array[i][2] = 1;
        A->array[i][3] = 0;
        A->array[i][4] = 0;
        A->array[i][5] = 0;
        A->array[i][6] = -dstx[i] * srcx[i];
        A->array[i][7] = -dstx[i] * srcy[i];
    }
    for (int i = 4; i < 8; i++)
    {
        A->array[i][0] = 0;
        A->array[i][1] = 0;
        A->array[i][2] = 0;
        A->array[i][3] = srcx[i - 4];
        A->array[i][4] = srcy[i - 4];
        A->array[i][5] = 1;
        A->array[i][6] = -dsty[i - 4] * srcx[i - 4];
        A->array[i][7] = -dsty[i - 4] * srcy[i - 4];
    }
    Matrix *Ainv = get_inverse_matrix(A);
    for (int i = 0; i < 8; i++)
    {
        m->array[i / 3][i % 3] = (((Ainv->array[i][0]) * dstx[0]) + ((Ainv->array[i][1]) * dstx[1]) + ((Ainv->array[i][2]) * dstx[2]) + ((Ainv->array[i][3]) * dstx[3]) +
                                  ((Ainv->array[i][4]) * dsty[0]) + ((Ainv->array[i][5]) * dsty[1]) + ((Ainv->array[i][6]) * dsty[2]) + ((Ainv->array[i][7]) * dsty[3]));
    }
    m->array[2][2] = 1;
    return m;
}

uint8_t get_otsu_thresh(dl_matrix3du_t *img)
{
    int numPixels = img->w * img->h;

    const int HISTOGRAM_SIZE = 256;
    unsigned int histogram[HISTOGRAM_SIZE];
    memset(histogram, 0, (HISTOGRAM_SIZE) * sizeof(unsigned int));
    uint8_t *ptr = img->item;
    int length = numPixels;
    while (length--)
    {
        uint8_t value = *ptr++;
        histogram[value]++;
    }

    int sum = 0;
    for (int i = 0; i < HISTOGRAM_SIZE; ++i)
    {
        sum += i * histogram[i];
    }

    int sumB = 0;
    int q1 = 0;
    double max = 0;
    uint8_t threshold = 0;
    for (int i = 0; i < HISTOGRAM_SIZE; ++i)
    {
        q1 += histogram[i];
        if (q1 == 0)
            continue;

        const int q2 = numPixels - q1;
        if (q2 == 0)
            break;

        sumB += i * histogram[i];
        const double m1 = (double)sumB / q1;
        const double m2 = ((double)sum - sumB) / q2;
        const double m1m2 = m1 - m2;
        const double variance = m1m2 * m1m2 * q1 * q2;
        if (variance > max)
        {
            threshold = i;
            max = variance;
        }
    }

    return threshold;
}

dl_matrix3du_t *rgb2gray(dl_matrix3du_t *img)
{
    assert(img->c == 3);
    dl_matrix3du_t *gray = dl_matrix3du_alloc(1, img->w, img->h, 1);
    int count = (img->w) * (img->h);
    uint8_t *r = img->item;
    uint8_t *g = r + 1;
    uint8_t *b = r + 2;

    uint8_t *pgray = gray->item;
    int x = 0;
    for (int i = 0; i < count; i++)
    {
        x = (19595 * (*r) + 38469 * (*g) + 7472 * (*b)) >> 16; //fast algorithm
        //Gray = R*0.299 + G*0.587 + B*0.114
        //Gray = (R*30 + G*59 + B*11 + 50) / 100
        //Gray = (R*38 + G*75 + B*15) >> 7

        *(pgray++) = (uint8_t)x;

        r += 3;
        g += 3;
        b += 3;
    }
    return gray;
}

dl_matrix3du_t *rgb2lab(dl_matrix3du_t *img)
{
    assert(img->c == 3);
    dl_matrix3du_t *lab = dl_matrix3du_alloc(1, img->w, img->h, img->c);
    int count = (img->w) * (img->h);
    uint8_t *r = img->item;
    uint8_t *g = r + 1;
    uint8_t *b = r + 2;
    float x, y, z;
    uint8_t *plab = lab->item;
    for (int i = 0; i < count; i++)
    {
        x = (0.433953 * (*r) + 0.376219 * (*g) + 0.189828 * (*b)) / 255;
        y = (0.212671 * (*r) + 0.715160 * (*g) + 0.072169 * (*b)) / 255;
        z = (0.017758 * (*r) + 0.109476 * (*g) + 0.872766 * (*b)) / 255;

        x = (x > 0.008856) ? pow(x, 1.0 / 3) : (7.787037 * x + 0.137931);
        y = (y > 0.008856) ? pow(y, 1.0 / 3) : (7.787037 * y + 0.137931);
        z = (z > 0.008856) ? pow(z, 1.0 / 3) : (7.787037 * z + 0.137931);

        *(plab++) = (uint8_t)(116 * y - 16);
        *(plab++) = (uint8_t)(500 * (x - y) + 128);
        *(plab++) = (uint8_t)(200 * (y - z) + 128);

        r += 3;
        g += 3;
        b += 3;
    }

    return lab;
}

dl_matrix3du_t *rgb2lab_fast(dl_matrix3du_t *img)
{
    assert(img->c == 3);
    dl_matrix3du_t *lab = dl_matrix3du_alloc(1, img->w, img->h, img->c);
    int count = (img->w) * (img->h);
    /*int c1 = 1<<16;
	int c2 = 1<<24;*/
    uint8_t *r = img->item;
    uint8_t *g = r + 1;
    uint8_t *b = r + 2;
    int x, y, z;
    //float temp = 0;
    uint8_t *plab = lab->item;
    for (int i = 0; i < count; i++)
    {
        x = (13933 * (*r) + 46871 * (*g) + 4732 * (*b)) >> 16;
        y = (((5467631 * (*r) - 8376186 * (*g) + 2908178 * (*b))) >> 24) + 128;
        z = (((2043680 * (*r) + 6351200 * (*g) - 8394880 * (*b))) >> 24) + 128;

        *(plab++) = (uint8_t)x;
        *(plab++) = (uint8_t)y;
        *(plab++) = (uint8_t)z;

        r += 3;
        g += 3;
        b += 3;
    }

    return lab;
}

dl_matrix3du_t *gen_binary_img(dl_matrix3du_t *lab, int *thresh)
{
    dl_matrix3du_t *bin = dl_matrix3du_alloc(1, lab->w, lab->h, 1);
    uint8_t *l = lab->item;
    uint8_t *a = l + 1;
    uint8_t *b = l + 2;
    uint8_t *pbin = bin->item;
    int count = (lab->w) * (lab->h);
    int num = 0;
    for (int i = 0; i < count; i++)
    {
        if (((*l) > thresh[0]) && ((*l) < thresh[1]) && ((*a) > thresh[2]) && ((*a) < thresh[3]) && ((*b) > thresh[4]) && ((*b) < thresh[5]))
        {
            *(pbin++) = 255;
            num++;
        }
        else
        {
            *(pbin++) = 0;
        }
        l += 3;
        a += 3;
        b += 3;
    }

    return bin;
}

void img_hist(dl_matrix3du_t *lab, float *rect)
{
    int x = (int)((lab->w) * rect[0]);
    int y = (int)((lab->h) * rect[1]);
    int w = (int)((lab->w) * rect[2]);
    int h = (int)((lab->h) * rect[3]);

    uint8_t *l = (lab->item) + (3 * y * lab->w) + (3 * x);

    int *lHist = calloc(256, sizeof(int));
    int *aHist = calloc(256, sizeof(int));
    int *bHist = calloc(256, sizeof(int));
    int num = w * h;

    for (int i = 0; i < h; i++)
    {
        uint8_t *l_temp = l + 3 * i * lab->w;
        uint8_t *a_temp = l_temp + 1;
        uint8_t *b_temp = l_temp + 2;
        for (int j = 0; j < w; j++)
        {
            lHist[*l_temp] += 1;
            aHist[*a_temp] += 1;
            bHist[*b_temp] += 1;
            l_temp += 3;
            a_temp += 3;
            b_temp += 3;
        }
    }
    int max[3] = {0};
    int maxIndex[3] = {0};
    float mean[3] = {0};
    float std[3] = {0};

    for (int i = 0; i < 256; i++)
    {
        mean[0] += i * lHist[i];
        mean[1] += i * aHist[i];
        mean[2] += i * bHist[i];

        std[0] += i * i * lHist[i];
        std[1] += i * i * aHist[i];
        std[2] += i * i * bHist[i];

        if (lHist[i] > max[0])
        {
            max[0] = lHist[i];
            maxIndex[0] = i;
        }
        if (aHist[i] > max[1])
        {
            max[1] = aHist[i];
            maxIndex[1] = i;
        }
        if (bHist[i] > max[2])
        {
            max[2] = bHist[i];
            maxIndex[2] = i;
        }
    }
    mean[0] /= num;
    mean[1] /= num;
    mean[2] /= num;

    std[0] = sqrt((std[0] / num) - (mean[0] * mean[0]));
    std[1] = sqrt((std[1] / num) - (mean[1] * mean[1]));
    std[2] = sqrt((std[2] / num) - (mean[2] * mean[2]));
    printf("L  \tmax:%d %d \tmean:%d \tstd:%d \n", maxIndex[0], max[0], (int)mean[0], (int)std[0]);
    printf("A  \tmax:%d %d \tmean:%d \tstd:%d \n", maxIndex[1], max[1], (int)mean[1], (int)std[1]);
    printf("B  \tmax:%d %d \tmean:%d \tstd:%d \n", maxIndex[2], max[2], (int)mean[2], (int)std[2]);
    free(lHist);
    free(aHist);
    free(bHist);
}

Matrix *get_similarity_matrix(float *srcx, float *srcy, float *dstx, float *dsty, int num)
{
    int dim = 2;
    double src_mean_x = 0.0;
    double src_mean_y = 0.0;
    double dst_mean_x = 0.0;
    double dst_mean_y = 0.0;

    for (int i = 0; i < num; i++)
    {
        src_mean_x += srcx[i];
        src_mean_y += srcy[i];
        dst_mean_x += dstx[i];
        dst_mean_y += dsty[i];
    }
    src_mean_x /= num;
    src_mean_y /= num;
    dst_mean_x /= num;
    dst_mean_y /= num;

    Matrix *src_demean = matrix_alloc(num, 2);
    Matrix *dst_demean = matrix_alloc(num, 2);
    for (int i = 0; i < num; i++)
    {
        src_demean->array[i][0] = srcx[i] - src_mean_x;
        src_demean->array[i][1] = srcy[i] - src_mean_y;
        dst_demean->array[i][0] = dstx[i] - dst_mean_x;
        dst_demean->array[i][1] = dsty[i] - dst_mean_y;
    }
    double A[2][2] = {0};
    for (int i = 0; i < num; i++)
    {
        A[0][0] += (dst_demean->array[i][0] * src_demean->array[i][0] / num);
        A[0][1] += (dst_demean->array[i][0] * src_demean->array[i][1] / num);
        A[1][0] += (dst_demean->array[i][1] * src_demean->array[i][0] / num);
        A[1][1] += (dst_demean->array[i][1] * src_demean->array[i][1] / num);
    }
    if ((A[0][0] == 0) && (A[0][1] == 0) && (A[1][0] == 0) && (A[1][1] == 0))
    {
        matrix_free(src_demean);
        matrix_free(dst_demean);
        return NULL;
    }

    double d[2] = {1, 1};
    if (((A[0][0] * A[1][1]) - A[0][1] * A[1][0]) < 0)
    {
        d[1] = -1;
    }

    //======================================================================SVD=====================================================================
    double U[2][2] = {0};
    double V[2][2] = {0};
    double S[2] = {0};

    double divide_temp = 0;

    double AAT[2][2] = {0};
    AAT[0][0] = A[0][0] * A[0][0] + A[0][1] * A[0][1];
    AAT[0][1] = A[0][0] * A[1][0] + A[0][1] * A[1][1];
    AAT[1][0] = A[1][0] * A[0][0] + A[1][1] * A[0][1];
    AAT[1][1] = A[1][0] * A[1][0] + A[1][1] * A[1][1];

    double l1 = (AAT[0][0] + AAT[1][1] + sqrt((AAT[0][0] + AAT[1][1]) * (AAT[0][0] + AAT[1][1]) - 4 * ((AAT[0][0] * AAT[1][1]) - (AAT[0][1] * AAT[1][0])))) / 2.0;
    double l2 = (AAT[0][0] + AAT[1][1] - sqrt((AAT[0][0] + AAT[1][1]) * (AAT[0][0] + AAT[1][1]) - 4 * ((AAT[0][0] * AAT[1][1]) - (AAT[0][1] * AAT[1][0])))) / 2.0;
    S[0] = sqrt(l1);
    S[1] = sqrt(l2);

    U[0][0] = 1.0;
    divide_temp = l1 - AAT[1][1];
    if (divide_temp == 0)
    {
        return NULL;
    }
    U[1][0] = AAT[1][0] / divide_temp;
    double norm = sqrt((U[0][0] * U[0][0]) + (U[1][0] * U[1][0]));
    U[0][0] /= norm;
    U[1][0] /= norm;

    U[0][1] = 1.0;
    divide_temp = l2 - AAT[1][1];
    if (divide_temp == 0)
    {
        return NULL;
    }
    U[1][1] = AAT[1][0] / divide_temp;
    norm = sqrt((U[0][1] * U[0][1]) + (U[1][1] * U[1][1]));
    U[0][1] /= norm;
    U[1][1] /= norm;

    if (U[0][1] * U[1][0] < 0)
    {
        U[0][0] = -U[0][0];
        U[1][0] = -U[1][0];
    }

    double ATA[2][2] = {0};
    ATA[0][0] = A[0][0] * A[0][0] + A[1][0] * A[1][0];
    ATA[0][1] = A[0][0] * A[0][1] + A[1][0] * A[1][1];
    ATA[1][0] = A[0][1] * A[0][0] + A[1][1] * A[1][0];
    ATA[1][1] = A[0][1] * A[0][1] + A[1][1] * A[1][1];

    V[0][0] = 1.0;
    divide_temp = l1 - ATA[1][1];
    if (divide_temp == 0)
    {
        return NULL;
    }
    V[0][1] = ATA[1][0] / divide_temp;
    norm = sqrt((V[0][0] * V[0][0]) + (V[0][1] * V[0][1]));
    V[0][0] /= norm;
    V[0][1] /= norm;

    V[1][0] = 1.0;
    divide_temp = l2 - ATA[1][1];
    if (divide_temp == 0)
    {
        return NULL;
    }
    V[1][1] = ATA[1][0] / divide_temp;
    norm = sqrt((V[1][0] * V[1][0]) + (V[1][1] * V[1][1]));
    V[1][0] /= norm;
    V[1][1] /= norm;

    if (V[0][1] * V[1][0] < 0)
    {
        V[0][0] = -V[0][0];
        V[0][1] = -V[0][1];
    }
    if ((S[0] * U[0][0] * V[0][0] + S[1] * U[0][1] * V[1][0]) * A[0][0] < 0)
    {
        U[0][0] = -U[0][0];
        U[0][1] = -U[0][1];
        U[1][0] = -U[1][0];
        U[1][1] = -U[1][1];
    }
    //============================================================================================================================================

    Matrix *T = matrix_alloc(2, 3);
    if (fabs((A[0][0] * A[1][1]) - A[0][1] * A[1][0]) < 1e-8)
    {
        if ((((U[0][0] * U[1][1]) - U[0][1] * U[1][0]) * ((V[0][0] * V[1][1]) - V[0][1] * V[1][0])) > 0)
        {
            T->array[0][0] = U[0][0] * V[0][0] + U[0][1] * V[1][0];
            T->array[0][1] = U[0][0] * V[0][1] + U[0][1] * V[1][1];
            T->array[1][0] = U[1][0] * V[0][0] + U[1][1] * V[1][0];
            T->array[1][1] = U[1][0] * V[0][1] + U[1][1] * V[1][1];
        }
        else
        {
            double s = d[dim - 1];
            d[dim - 1] = -1;
            T->array[0][0] = d[0] * U[0][0] * V[0][0] + d[1] * U[0][1] * V[1][0];
            T->array[0][1] = d[0] * U[0][0] * V[0][1] + d[1] * U[0][1] * V[1][1];
            T->array[1][0] = d[0] * U[1][0] * V[0][0] + d[1] * U[1][1] * V[1][0];
            T->array[1][1] = d[0] * U[1][0] * V[0][1] + d[1] * U[1][1] * V[1][1];
            d[dim - 1] = s;
        }
    }
    else
    {
        T->array[0][0] = d[0] * U[0][0] * V[0][0] + d[1] * U[0][1] * V[1][0];
        T->array[0][1] = d[0] * U[0][0] * V[0][1] + d[1] * U[0][1] * V[1][1];
        T->array[1][0] = d[0] * U[1][0] * V[0][0] + d[1] * U[1][1] * V[1][0];
        T->array[1][1] = d[0] * U[1][0] * V[0][1] + d[1] * U[1][1] * V[1][1];
    }

    double Ex = 0.0;
    double Ex2 = 0.0;
    double Ey = 0.0;
    double Ey2 = 0.0;
    for (int i = 0; i < num; i++)
    {
        Ex += src_demean->array[i][0];
        Ex2 += (src_demean->array[i][0] * src_demean->array[i][0]);
        Ey += src_demean->array[i][1];
        Ey2 += (src_demean->array[i][1] * src_demean->array[i][1]);
    }
    Ex /= num;
    Ex2 /= num;
    Ey /= num;
    Ey2 /= num;
    double var_sum = (Ex2 - Ex * Ex) + (Ey2 - Ey * Ey);
    double scale = (S[0] * d[0] + S[1] * d[1]) / var_sum;

    T->array[0][2] = dst_mean_x - scale * (T->array[0][0] * src_mean_x + T->array[0][1] * src_mean_y);
    T->array[1][2] = dst_mean_y - scale * (T->array[1][0] * src_mean_x + T->array[1][1] * src_mean_y);

    T->array[0][0] *= scale;
    T->array[0][1] *= scale;
    T->array[1][0] *= scale;
    T->array[1][1] *= scale;

    matrix_free(src_demean);
    matrix_free(dst_demean);
    return T;
}

void warp_affine(dl_matrix3du_t *img, dl_matrix3du_t *crop, Matrix *M)
{
    Matrix *M_inv = get_inv_affine_matrix(M);
    uint8_t *dst = crop->item;
    int stride = img->w * img->c;
    int c = img->c;

    float x_src = 0.0;
    float y_src = 0.0;
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;

    for (int i = 0; i < crop->h; i++)
    {
        for (int j = 0; j < crop->w; j++)
        {
            x_src = M_inv->array[0][0] * j + M_inv->array[0][1] * i + M_inv->array[0][2];
            y_src = M_inv->array[1][0] * j + M_inv->array[1][1] * i + M_inv->array[1][2];
            if ((x_src < 0) || (y_src < 0) || (x_src >= (img->w - 1)) || (y_src >= (img->h - 1)))
            {
                for (int k = 0; k < crop->c; k++)
                {
                    *dst++ = 0;
                }
            }
            else
            {
                x1 = floor(x_src);
                x2 = x1 + 1;
                y1 = floor(y_src);
                y2 = y1 + 1;
                for (int k = 0; k < crop->c; k++)
                {
                    *dst++ = (uint8_t)rintf(((img->item[y1 * stride + x1 * c + k]) * (x2 - x_src) * (y2 - y_src)) + ((img->item[y1 * stride + x2 * c + k]) * (x_src - x1) * (y2 - y_src)) + ((img->item[y2 * stride + x1 * c + k]) * (x2 - x_src) * (y_src - y1)) + ((img->item[y2 * stride + x2 * c + k]) * (x_src - x1) * (y_src - y1)));
                }
            }
        }
    }
    matrix_free(M_inv);
}
