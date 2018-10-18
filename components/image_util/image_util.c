#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "dl_lib.h"
#include "image_util.h"
#include "freertos/FreeRTOS.h"


void image_resize_linear (uint8_t *dst_image, uint8_t *src_image, int dst_w, int dst_h, int dst_c, int src_w, int src_h)
{/*{{{*/
    float scale_x = (float)src_w / dst_w;
    float scale_y = (float)src_h / dst_h;

    int dst_stride = dst_c * dst_w;
    int src_stride = dst_c * src_w;
    for (int y = 0; y < dst_h; y++)
    {
        float fy[2];
        fy[0] = (float)((y + 0.5) * scale_y - 0.5);  // y
        int src_y = (int)fy[0];    // y1
        fy[0] -= src_y;    // y - y1
        fy[1] = 1 - fy[0];  // y2 - y
        src_y = DL_IMAGE_MAX(0, src_y);
        src_y = DL_IMAGE_MIN(src_y, src_h - 2);

        for (int x = 0; x < dst_w; x++)
        {
            float fx[2];
            fx[0] = (float)((x + 0.5) * scale_x - 0.5); // x
            int src_x = (int)fx[0];     // x1
            fx[0] -= src_x;     // x - x1
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
            fx[1] = 1 - fx[0];  // x2 - x

            for (int c = 0; c < dst_c; c++)
            {
                dst_image[y * dst_stride + x * dst_c + c] = round(src_image[src_y * src_stride + src_x * dst_c + c] * fx[1] * fy[1]
                        + src_image[src_y * src_stride + (src_x + 1) * dst_c + c] * fx[0] * fy[1]
                        + src_image[(src_y + 1) * src_stride +  src_x * dst_c + c] * fx[1] * fy[0]
                        + src_image[(src_y + 1) * src_stride + (src_x + 1) * dst_c + c] * fx[0] * fy[0]);
            }

        }

    }
}/*}}}*/


// TODO: Crop and align the face according to the output size ,angle,ratio,and center. Using the image_resize_linear inside.
// @crop_image: here should be a 56x56x3
// @src_image: input original image.
// @rotate_angle: radius that stands for the rotation. Reverse clock is the positive direction.
// @ratio: the ratio input/output eye distance.
// @center: x,y coordinates that is the rotation center in the input image as well as the cropped image center.
void image_cropper(uint8_t *rot_data, uint8_t *src_data, int rot_w, int rot_h, int rot_c, int src_w, int src_h, float rotate_angle, float ratio, float* center)
{
	float rot_w_start = 0.5f -(float)rot_w/2;
	float rot_h_start = 0.5f -(float)rot_h/2;

	//rotate_angle must be radius
	float si = sin(rotate_angle);
	float co = cos(rotate_angle);

	int rot_stride = rot_w * rot_c;
	int src_stride = rot_c * src_w;

	for (int y = 0; y < rot_h; y++)
	{
		for (int x = 0; x < rot_w; x++)
		{
			float xs,ys,xr,yr;
			xs = ratio * (rot_w_start + x);
			ys = ratio * (rot_h_start + y);

			xr = xs*co + ys*si;
			yr = -xs*si + ys*co;

			float fy[2];
			fy[0] = center[1] + yr; // y
			int src_y = (int)fy[0];    // y1
			fy[0] -= src_y;    // y - y1
			fy[1] = 1 - fy[0];  // y2 - y
			src_y = DL_IMAGE_MAX(0, src_y);
			src_y = DL_IMAGE_MIN(src_y, src_h - 2);

			float fx[2];
			fx[0] = center[0] + xr; // x
			int src_x = (int)fx[0];     // x1
			fx[0] -= src_x;     // x - x1
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
			fx[1] = 1 - fx[0];  // x2 - x

			for (int c = 0; c < rot_c; c++)
			{
				rot_data[y * rot_stride + x * rot_c + c] = round(src_data[src_y * src_stride + src_x * rot_c + c] * fx[1] * fy[1]
						+ src_data[src_y * src_stride + (src_x + 1) * rot_c + c] * fx[0] * fy[1]
						+ src_data[(src_y + 1) * src_stride +  src_x * rot_c + c] * fx[1] * fy[0]
						+ src_data[(src_y + 1) * src_stride + (src_x + 1) * rot_c + c] * fx[0] * fy[0]);
			}

		}

	}

}

void image_sort_insert_by_score (image_list_t *image_sorted_list, const image_list_t *insert_list)
{/*{{{*/
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
}/*}}}*/

image_list_t *image_get_valid_boxes (fptp_t *score, fptp_t *offset, int width, int height, fptp_t score_threshold, fptp_t scale)
{/*{{{*/
    typedef struct
    {
        short valid_x;
        short valid_y;
        int valid_idx;
    } valid_index_t;
    valid_index_t *valid_indexes = (valid_index_t *)calloc(width * height, sizeof(valid_index_t));
    int valid_count = 0;
    int index = 0;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            if (score[2 * index + 1] > score_threshold)
            {
                valid_indexes[valid_count].valid_x = x;
                valid_indexes[valid_count].valid_y = y;
                valid_indexes[valid_count].valid_idx = index;
                valid_count++;
            }
            index++;
        }

    if (0 == valid_count)
    {
        free(valid_indexes);
        return NULL;
    }

    image_box_t *valid_box = (image_box_t *)calloc(valid_count, sizeof(image_box_t));
    image_list_t *valid_list = (image_list_t *)calloc(1, sizeof(image_list_t));
    valid_list->head = valid_box;
    valid_list->origin_head = valid_box;
    valid_list->len = valid_count;

    for (int i = 0; i < valid_count; i++)
    {
        fptp_t x1 = valid_indexes[i].valid_x * scale * 2.0;
        fptp_t y1 = valid_indexes[i].valid_y * scale * 2.0;
        int valid_i = valid_indexes[i].valid_idx;
        valid_box[i].score = score[2 * valid_i + 1];
        valid_box[i].box.box_p[0] = x1;
        valid_box[i].box.box_p[1] = y1;
        valid_box[i].box.box_p[2] = x1 + MIN_FACE * scale;
        valid_box[i].box.box_p[3] = y1 + MIN_FACE * scale;
        valid_box[i].offset.box_p[0] = offset[valid_i * 4 + 0]; 
        valid_box[i].offset.box_p[1] = offset[valid_i * 4 + 1]; 
        valid_box[i].offset.box_p[2] = offset[valid_i * 4 + 2]; 
        valid_box[i].offset.box_p[3] = offset[valid_i * 4 + 3]; 
        valid_box[i].next = &(valid_box[i + 1]);
    }
    valid_box[valid_count - 1].next = NULL;

    free(valid_indexes);
    
    return valid_list;
}/*}}}*/

void image_nms_process (image_list_t *image_list, fptp_t nms_threshold, int same_area)
{/*{{{*/
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
            // kept_box is contained in other_box
            if (((kept_box->box.box_p[0] > other_box->box.box_p[0])
                && (kept_box->box.box_p[1] > other_box->box.box_p[1])
                && (kept_box->box.box_p[2] < other_box->box.box_p[2])
                && (kept_box->box.box_p[3] < other_box->box.box_p[3]))
            // kept_box contains other_box
                || ((kept_box->box.box_p[0] < other_box->box.box_p[0])
                && (kept_box->box.box_p[1] < other_box->box.box_p[1])
                && (kept_box->box.box_p[2] > other_box->box.box_p[2])
                && (kept_box->box.box_p[3] > other_box->box.box_p[3])))
            // supress
            {
                num_supressed++;
                prev->next = other_box->next;
                other_box = other_box->next;
                continue;
            }


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
}/*}}}*/

void transform_input_image (uint8_t *m, uint16_t *bmp, int count)
{/*{{{*/
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
}/*}}}*/

void transform_output_image (uint16_t *bmp, uint8_t *m, int count)
{/*{{{*/
    for (int x = 0; x < count; x++)
    {
        rgb888_to_565(bmp, m[2], m[1], m[0]);
        bmp++;
        m += 3;
    }
}/*}}}*/

void draw_rectangle(uint16_t *buf, box_array_t *boxes, int width)
{/*{{{*/
    uint16_t p[14];
    for (int i = 0; i < boxes->len; i++)
    {
        // rectangle box
        for (int j = 0; j < 4; j++)
            p[j] = (uint16_t)boxes->box[i].box_p[j];

        // landmark
        for (int j = 0; j < 10; j++)
            p[j + 4] = (uint16_t)boxes->landmark[i].landmark_p[j];

        if ((p[2] < p[0]) || (p[3] < p[1]))
            return;

#define GREEN RGB565_MASK_GREEN
#define RED RGB565_MASK_GREEN
        // rectangle box
        for (int w = p[0]; w < p[2] + 1; w++)
        {
            int x1 = (p[1] * width + w);
            int x2 = (p[3] * width + w);
            buf[x1] = GREEN;   // Green
            buf[x2] = GREEN;   
        }
        for (int h = p[1]; h < p[3] + 1; h++)
        {
            int y1 = (h * width + p[0]);
            int y2 = (h * width + p[2]);
            buf[y1] = GREEN;   // Green
            buf[y2] = GREEN;   
        }

        // landmark
        for (int j = 0; j < 10; j+=2)
        {
            int x = p[j + 5] * width + p[j + 4];
            buf[x] = RED;    // Red
            buf[x + 1] = RED;
            buf[x + 2] = RED;

            buf[width + x] = RED;    // Red
            buf[width + x + 1] = RED;
            buf[width + x + 2] = RED;

            buf[2 * width + x] = RED;    // Red
            buf[2 * width + x + 1] = RED;
            buf[2 * width + x + 2] = RED;
        }
    }
}/*}}}*/
