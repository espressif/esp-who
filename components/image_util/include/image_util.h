#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "mtmn.h"

#define BOX_LEN (80)
#define MIN_FACE (12.0)
#define MAX_VALID_COUNT_PER_IMAGE (30)

#define DL_IMAGE_MIN(A, B) ((A) < (B) ? (A) : (B))
#define DL_IMAGE_MAX(A, B) ((A) < (B) ? (B) : (A))

#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240

#define RGB565_MASK_RED        0xF800
#define RGB565_MASK_GREEN      0x07E0
#define RGB565_MASK_BLUE       0x001F

typedef struct
{
    fptp_t landmark_p[10];
} landmark_t;


typedef struct
{
    fptp_t box_p[4];
} box_t;

typedef struct tag_box_list
{
    box_t *box;
    landmark_t *landmark;
    int len;
} box_array_t;

typedef struct tag_image_box
{
    struct tag_image_box *next;
    fptp_t score;
    box_t box;
    box_t offset;
    landmark_t landmark;
} image_box_t;

typedef struct tag_image_list
{
    image_box_t *head;
    image_box_t *origin_head;
    int len;
} image_list_t;


static inline void image_get_width_and_height (box_t *box, float *w, float *h)
{
    *w = box->box_p[2] - box->box_p[0] + 1;
    *h = box->box_p[3] - box->box_p[1] + 1;
}


static inline void image_get_area (box_t *box, float *area)
{
    float w, h;
    image_get_width_and_height(box, &w, &h);
    *area = w * h;
}


static inline void image_calibrate_by_offset (image_list_t *image_list)
{
    for (image_box_t *head = image_list->head; head; head = head->next)
    {
        float w, h;
        image_get_width_and_height(&(head->box), &w, &h);
        head->box.box_p[0] = DL_IMAGE_MAX(0, head->box.box_p[0] + head->offset.box_p[0] * w);
        head->box.box_p[1] = DL_IMAGE_MAX(0, head->box.box_p[1] + head->offset.box_p[1] * w);
        head->box.box_p[2] += head->offset.box_p[2] * w;
        if (head->box.box_p[2] > IMAGE_WIDTH)
        {
            head->box.box_p[2] = IMAGE_WIDTH - 1;
            head->box.box_p[0] = IMAGE_WIDTH - w;
        }
        head->box.box_p[3] += head->offset.box_p[3] * h;
        if (head->box.box_p[3] > IMAGE_HEIGHT)
        {
            head->box.box_p[3] = IMAGE_HEIGHT - 1;
            head->box.box_p[1] = IMAGE_HEIGHT - h;
        }
    }
}

static inline void image_landmark_calibrate (image_list_t *image_list)
{
    for (image_box_t *head = image_list->head; head; head = head->next)
    {
        float w, h;
        image_get_width_and_height(&(head->box), &w, &h);
        head->landmark.landmark_p[0] = head->box.box_p[0] + head->landmark.landmark_p[0] * w;
        head->landmark.landmark_p[1] = head->box.box_p[1] + head->landmark.landmark_p[1] * h;

        head->landmark.landmark_p[2] = head->box.box_p[0] + head->landmark.landmark_p[2] * w;
        head->landmark.landmark_p[3] = head->box.box_p[1] + head->landmark.landmark_p[3] * h;

        head->landmark.landmark_p[4] = head->box.box_p[0] + head->landmark.landmark_p[4] * w;
        head->landmark.landmark_p[5] = head->box.box_p[1] + head->landmark.landmark_p[5] * h;
        
        head->landmark.landmark_p[6] = head->box.box_p[0] + head->landmark.landmark_p[6] * w;
        head->landmark.landmark_p[7] = head->box.box_p[1] + head->landmark.landmark_p[7] * h;

        head->landmark.landmark_p[8] = head->box.box_p[0] + head->landmark.landmark_p[8] * w;
        head->landmark.landmark_p[9] = head->box.box_p[1] + head->landmark.landmark_p[9] * h;
    }

}

static inline void image_rect2sqr (box_array_t *boxes, int width, int height)
{
    for (int i = 0; i < boxes->len; i++)
    {
        box_t *box =&(boxes->box[i]);
        float w, h;
        image_get_width_and_height(box, &w, &h);
        float l = DL_IMAGE_MAX(w, h);

        box->box_p[0] = DL_IMAGE_MAX(0, box->box_p[0] + 0.5 * (w - l));
        box->box_p[1] = DL_IMAGE_MAX(0, box->box_p[1] + 0.5 * (h - l));
        box->box_p[2] = box->box_p[0] + l - 1;
        if (box->box_p[2] > width)
        {
            box->box_p[2] = width - 1;
            box->box_p[0] = width - l;
        }
        box->box_p[3] = box->box_p[1] + l - 1;
        if (box->box_p[3] > height)
        {
            box->box_p[3] = height - 1;
            box->box_p[1] = height - l;
        }
    }
}

static inline void rgb565_to_888(uint16_t in, uint8_t* dst)
{/*{{{*/
    dst[0] = (in & RGB565_MASK_BLUE) << 3; // blue
    dst[1] = (in & RGB565_MASK_GREEN) >> 3; // green
    dst[2] = (in & RGB565_MASK_RED) >> 8; // red
}/*}}}*/

static inline void rgb888_to_565(uint16_t *in, uint8_t r, uint8_t g, uint8_t b)
{/*{{{*/
    uint16_t rgb565=0;
    rgb565 =  ((r >> 3) << 11);
    rgb565 |= ((g >> 2) << 5);
    rgb565 |= (b >> 3);
    *in = rgb565;
}/*}}}*/

image_list_t *image_get_valid_boxes (fptp_t *score, fptp_t *offset, int width, int height, fptp_t score_threshold, fptp_t scale);
void image_sort_insert_by_score (image_list_t *image_sorted_list, const image_list_t *insert_list);
void image_nms_process (image_list_t *image_list, fptp_t nms_threshold, int same_area);
void image_resize_linear (uint8_t *dst_image, uint8_t *src_image, int dst_w, int dst_h, int dst_c, int src_w, int src_h);
void image_cropper(uint8_t *rot_data, uint8_t *src_data, int rot_w, int rot_h, int rot_c, int src_w, int src_h, float rotate_angle, float ratio, float* center);
void transform_input_image (uint8_t *m, uint16_t *bmp, int count);
void transform_output_image (uint16_t *bmp, uint8_t *m, int count);
void draw_rectangle(uint16_t *buf, box_array_t *boxes, int width);
#ifdef __cplusplus
}
#endif
