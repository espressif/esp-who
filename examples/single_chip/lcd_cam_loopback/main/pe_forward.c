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
#include <string.h>
#include <math.h>
#include "esp_system.h"
#include "pe_forward.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define HD_LITE_FEATURE_MAP_NUM 1

static const char *TAG = "pe_forward";
static int line_pairs[20][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 4},         // thumb
                                {0, 5}, {5, 6}, {6, 7}, {7, 8},         // index
                                {0, 9}, {9, 10}, {10, 11}, {11, 12},    // middle
                                {0, 13}, {13, 14}, {14, 15}, {15, 16},  // ring
                                {0, 17}, {17, 18}, {18, 19}, {19, 20}}; // little
static float pi = acosf(-1.0);

od_box_array_t *hand_detection_forward(dl_matrix3dq_t *image, hd_config_t hd_config)
{
    /**
     * @brief resize image
     * 
     */
    //int target_size = 112;
    int preprocess_mode = hd_config.preprocess_mode;
    int target_size = hd_config.target_size;
    fptp_t score_threshold = hd_config.score_threshold;
    fptp_t nms_threshold = hd_config.nms_threshold;
    int mode = hd_config.mode;
    int free_input = hd_config.free_input;
    // dl_matrix3dq_t *hd_image_input = od_image_preporcess(image->item, image->w, image->h, target_size, INPUT_EXPONENT, preprocess_mode);
    // printf("%d,%d,%d,%d %d\n",hd_image_input->n, hd_image_input->h, hd_image_input->w, hd_image_input->c, hd_image_input->exponent);
    // if(free_input){
    //     dl_matrix3du_free(image);
    // }

    /**
     * @brief net operation
     * 
     */
    
    detection_result_t **hd_results = hd_lite1_q(image, mode);

    /**
     * @brief filter by score
     * 
     */
    od_image_list_t **origin_head = (od_image_list_t **)dl_lib_calloc(HD_LITE_FEATURE_MAP_NUM, sizeof(od_image_list_t *), 0);
    od_image_list_t all_box_list = {NULL};

    for (size_t i = 0; i < HD_LITE_FEATURE_MAP_NUM; i++)
    {
        //yolo_result_print(yolo_results[i]);
        fptp_t *cls = hd_results[i]->cls->item;
        fptp_t *score = hd_results[i]->score->item;
        fptp_t *boxes = hd_results[i]->boxes->item;
        // fptp_t resize_scale = (image->w >= image->h)?((float)target_size/image->w):((float)target_size/image->h);
        fptp_t resize_scale=hd_config.resize_scale;
        // int padding_w = (target_size - resize_scale*image->w)/2;
        // int padding_h = (target_size - resize_scale*image->h)/2;

        origin_head[i] = od_image_get_valid_boxes(score,
                                               cls,
                                               boxes,
                                               hd_results[i]->cls->n,
                                               hd_results[i]->cls->h,
                                               hd_results[i]->cls->w,
                                               score_threshold,
                                               resize_scale,
                                               0,
                                               0);
        if (origin_head[i]){
            //printf("od_image_sort_insert_by_score :%d\n",origin_head[i]->len);
            od_image_sort_insert_by_score(&all_box_list, origin_head[i]);
        }

        detection_result_free(hd_results[i]);
    }
    dl_lib_free(hd_results);

    /**
     * @brief nms
     * 
     */
    od_image_nms_process(&all_box_list, nms_threshold, 0);

    /**
     * @brief build up result
     * 
     */
    od_box_array_t *targets_list = NULL;
    if (all_box_list.len)
    {
        // printf("all_box_list：%d \n",all_box_list.len);
        targets_list = (od_box_array_t *)dl_lib_calloc(1, sizeof(od_box_array_t), 0);
        targets_list->len = all_box_list.len;
        targets_list->score = (fptp_t *)dl_lib_calloc(targets_list->len, sizeof(fptp_t), 0);
        targets_list->box = (box_t *)dl_lib_calloc(targets_list->len, sizeof(box_t), 0);
        targets_list->cls = (qtp_t *)dl_lib_calloc(targets_list->len, sizeof(qtp_t), 0);

        od_image_box_t *t = all_box_list.head;
        for (int i = 0; i < all_box_list.len; i++, t = t->next)
        {
            targets_list->box[i] = t->box;
            targets_list->cls[i] = t->cls;
            targets_list->score[i] = t->score;
            // printf("bbox: %f,%f,%f,%f   score: %f, class: %d\n", t->box.box_p[0], t->box.box_p[1],t->box.box_p[2],t->box.box_p[3],t->score, t->cls);
        }
    }

    for (int i = 0; i < HD_LITE_FEATURE_MAP_NUM; i++)
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

static inline dl_matrix3dq_t *dl_matrix3dq_from_3du(dl_matrix3du_t *m, int exponent, int shift_offset)
{
    dl_matrix3dq_t *out = dl_matrix3dq_alloc(m->n, m->w, m->h, m->c, exponent);
    qtp_t *o_item = out->item;
    uint8_t *m_item = m->item;
    int shift = (-exponent)-shift_offset;
    int count=(m->n)*(m->w)*(m->h)*(m->c);
    if(shift>=0){
        for(int i=0;i<count;i++){
            *o_item++ = ((qtp_t)(*(m_item++)))<<shift;
        }
    }else{
        shift = -shift;
        for(int i=0;i<count;i++){
            *o_item++ = ((qtp_t)(*(m_item++)))>>shift;
        }
    }
    return out;
    
}

dl_matrix3d_t *handpose_estimation_forward2(uint16_t *simage, od_box_array_t *od_boxes, int dw, int sw, int sh, int mode)
{
    int hp_exponent = -10;
    int shift = -8 - hp_exponent;
    float scale = 0.0;
    int landmark_num = 21;
    if(od_boxes){
        dl_matrix3d_t *landmarks = dl_matrix3d_alloc(od_boxes->len, 1, landmark_num, 2);
        float dilat_ratio = 1.2;
        for(int i=0; i<od_boxes->len; i++){
            dl_matrix3dq_t *image_input = dl_matrix3dq_alloc(1, dw, dw, 3, hp_exponent);
            float x = od_boxes->box[i].box_p[0];
            float y = od_boxes->box[i].box_p[1];
            float w = od_boxes->box[i].box_p[2] - x + 1;
            float h = od_boxes->box[i].box_p[3] - y + 1;
            float ox = 0.0;
            float oy = 0.0;
            if(w>h){
                oy = (dilat_ratio*w - h)/2.0;
                ox = (dilat_ratio-1)*w/2.0;
            }else{
                ox = (dilat_ratio*h - w)/2.0;
                oy = (dilat_ratio-1)*h/2.0;
            }
            int x1 = (int)(max(0, od_boxes->box[i].box_p[0] - ox));
            int y1 = (int)(max(0, od_boxes->box[i].box_p[1] - oy));
            int x2 = (int)(min(sw, od_boxes->box[i].box_p[2] + ox));
            int y2 = (int)(min(sh, od_boxes->box[i].box_p[3] + oy));
            w = x2 - x1;
            h = y2 - y1;
            scale = (float)(max(w, h))/dw;
            
            image_crop_shift_fast(image_input->item, simage, dw, sw, sh, x1, y1, x2, y2, shift);
            dl_matrix3d_t *landmark = hp_nano1_ls16_q(image_input, mode);
            // ets_printf("x1:%d, y1:%d, x2:%d, y2:%d \n", x1, y1, x2, y2); 
            // printf("scale: %f\n", scale);
            for(int j=0; j<landmark_num; j++){
                landmarks->item[i*(landmark_num*2)+j*2] = ((landmark->item[j*2])*scale) + x1;
                landmarks->item[i*(landmark_num*2)+j*2+1] = ((landmark->item[j*2+1])*scale) + y1;
                // ets_printf("lmks %d: %d, %d\n", j, (int)(landmark->item[j*2]), (int)(landmark->item[j*2+1]));
            }
            dl_matrix3d_free(landmark);
        }
        return landmarks;
    }else{
        dl_matrix3dq_t *image_input = dl_matrix3dq_alloc(1, dw, dw, 3, hp_exponent);
        // scale = (sw >= sh)?((float)dw/sw):((float)dw/sh);
        int tw, th = 0;
        if(sw >= sh){
            scale = (float)sw / dw;
            tw = dw;
            th = (int)(sh / scale);
        }else{
            scale = (float)sh / dw;
            tw = (int)(sw / scale);
            th = dw;
        }
        // dl_matrix3dq_t *hp_input_image = od_image_preporcess(image->item, image->w, image->h, target_size, hp_exponent, 0);
        image_resize_shift_fast(image_input->item, simage, dw, 3, sw, sh, tw, th, shift);
        dl_matrix3d_t *landmarks = hp_nano1_ls16_q(image_input, mode);
        for(int j=0; j<landmark_num; j++){
                landmarks->item[j*2] = (landmarks->item[j*2])*scale;
                landmarks->item[j*2+1] = (landmarks->item[j*2+1])*scale;
                // ets_printf("lmks %d: %d, %d\n", j, (int)(landmarks->item[j*2]), (int)(landmarks->item[j*2+1]));
            }
        return landmarks;
    }
}

dl_matrix3d_t *handpose_estimation_forward(dl_matrix3du_t *image, int target_size, od_box_array_t *od_boxes, int mode)
{
    int hp_exponent = -10;
    int shift_offset = 8;
    float scale = 0.0;
    int landmark_num = 21;
    if(od_boxes){
        dl_matrix3d_t *landmarks = dl_matrix3d_alloc(od_boxes->len, 1, landmark_num, 2);
        float dilat_ratio = 1.2;
        for(int i=0; i<od_boxes->len; i++){
            float x = od_boxes->box[i].box_p[0];
            float y = od_boxes->box[i].box_p[1];
            float w = od_boxes->box[i].box_p[2] - x + 1;
            float h = od_boxes->box[i].box_p[3] - y + 1;
            float ox = 0.0;
            float oy = 0.0;
            if(w>h){
                oy = (dilat_ratio*w - h)/2.0;
                ox = (dilat_ratio-1)*w/2.0;
            }else{
                ox = (dilat_ratio*h - w)/2.0;
                oy = (dilat_ratio-1)*h/2.0;
            }
            int x1 = (int)(max(0, od_boxes->box[i].box_p[0] - ox));
            int y1 = (int)(max(0, od_boxes->box[i].box_p[1] - oy));
            int x2 = (int)(min(image->w, od_boxes->box[i].box_p[2] + ox));
            int y2 = (int)(min(image->h, od_boxes->box[i].box_p[3] + oy));
            w = x2 - x1;
            h = y2 - y1;
            
            int target_w, target_h = 0;
            int dw = 0;
            int dh = 0;
            if(w >= h){
                scale = (float)target_size / w;
                target_w = target_size;
                target_h = (int)(h*scale);
                dh = (target_size - target_h)/2;
            }else{
                scale = (float)target_size / h;
                target_w = (int)(w*scale);
                target_h = target_size;
                dw = (target_size - target_w)/2;
            }
            float srcx[3] = {x1, x2, x2};
            float srcy[3] = {y1, y2, y1};
            float dstx[3] = {dw, dw+target_w, dw+target_w};
            float dsty[3] = {dh, dh+target_h, dh};
            Matrix *M = get_affine_transform(srcx, srcy, dstx, dsty);
            dl_matrix3du_t *hp_input_image_u = dl_matrix3du_alloc(1, target_size, target_size, image->c);
            warp_affine(image, hp_input_image_u, M);
            matrix_free(M);
            dl_matrix3dq_t *hp_input_image = dl_matrix3dq_from_3du(hp_input_image_u, hp_exponent, shift_offset);
            dl_matrix3du_free(hp_input_image_u);
            dl_matrix3d_t *landmark = hp_nano1_ls16_q(hp_input_image, mode);
            // ets_printf("x1:%d, y1:%d, x2:%d, y2:%d \n", x1, y1, x2, y2); 
            for(int j=0; j<landmark_num; j++){
                landmarks->item[i*(landmark_num*2)+j*2] = ((landmark->item[j*2] - dw)/scale) + x1;
                landmarks->item[i*(landmark_num*2)+j*2+1] = ((landmark->item[j*2+1] - dh)/scale) + y1;
                // ets_printf("lmks %d: %d, %d\n", j, (int)(landmarks->item[i*(landmark_num*2)+j*2]), (int)(landmarks->item[i*(landmark_num*2)+j*2+1]));
            }
            dl_matrix3d_free(landmark);
        }
        return landmarks;
    }else{
        scale = (image->w >= image->h)?((float)target_size/image->w):((float)target_size/image->h);
        dl_matrix3dq_t *hp_input_image = od_image_preporcess(image->item, image->w, image->h, target_size, hp_exponent, 0);
        dl_matrix3d_t *landmarks = hp_nano1_ls16_q(hp_input_image, mode);
        for(int j=0; j<landmark_num; j++){
                landmarks->item[j*2] = (landmarks->item[j*2])/scale;
                landmarks->item[j*2+1] = (landmarks->item[j*2+1])/scale;
            }
        return landmarks;
    }
    
}

static inline float get_vector_angle(float x, float y){
    float eps = 1e-6;
    float angle = 0.0;
    float rad_ratio = 180.0/pi;
    angle = rad_ratio*asinf(fabs(y)/sqrtf(x*x + y*y + eps));
    if((x>=0)&&(y>=0)){
        angle = angle;
    }else if ((x<0)&&(y>=0)){
        angle = 180.0 - angle;
    }else if((x<0)&&(y<0)){
        angle = 180.0 + angle;
    }else{
        angle = 360.0 - angle;
    }
    return angle;
}

static inline float get_vector_length(float x, float y){
    float length = sqrtf(x*x + y*y);
    return length;
}

char *pose_estimation_with_lmks(dl_matrix3d_t *landmarks){
    char *pose = malloc(16 * sizeof(char));
    float joints_vector[20][2] = {0};
    float joints_angles[5][4] = {0};
    float joints_length[5][4] = {0};
    float finger_length[5] = {0};
    float finger_angles[5][4] = {0};
    int finger_bent[5] = {0};
    float xmin = landmarks->item[0];
    float xmax = landmarks->item[0];
    float ymin = landmarks->item[1];
    float ymax = landmarks->item[1];
    float finger_angle_thresh = 90.0;
    float thumb_angle_thresh = 70.0;
    int count = 0;
    int ind1, ind2 = 0;
    for(int i=0; i<5; i++){
        for(int j=0; j<4; j++){
            ind1 = line_pairs[count][0];
            ind2 = line_pairs[count][1];
            joints_vector[count][0] = landmarks->item[2*ind2] - landmarks->item[2*ind1];
            joints_vector[count][1] = landmarks->item[2*ind2+1] - landmarks->item[2*ind1+1];
            joints_angles[i][j] = get_vector_angle(joints_vector[count][0], joints_vector[count][1]);
            joints_length[i][j] = get_vector_length(joints_vector[count][0], joints_vector[count][1]);
            count++;
        }
    }
    for(int i=0; i<5; i++){
        for(int j=1; j<4; j++){
            finger_angles[i][j] = fabs(joints_angles[i][j] - joints_angles[i][j-1]);
            if(finger_angles[i][j]>180){
                finger_angles[i][j] = 360.0 - finger_angles[i][j];
            }
        }
        finger_angles[i][0] = finger_angles[i][1] + finger_angles[i][2] + finger_angles[i][3];
        if(finger_angles[i][0]>finger_angle_thresh){
            finger_bent[i] = 1;
        }
    }
    float palm_angle = get_vector_angle((joints_vector[5][0]+joints_vector[9][0]+joints_vector[13][0])/3.0,
                                        (joints_vector[5][1]+joints_vector[9][1]+joints_vector[13][1])/3.0);
    if(palm_angle<=90){
        palm_angle += 90;
    }else{
        palm_angle -= 270;
    }

    for(int i=1;i<21;i++){
        if(xmin>landmarks->item[2*i]){
            xmin = landmarks->item[2*i];
        }
        if(xmax<landmarks->item[2*i]){
            xmax = landmarks->item[2*i];
        }
        if(ymin>landmarks->item[2*i+1]){
            ymin = landmarks->item[2*i+1];
        }
        if(ymax<landmarks->item[2*i+1]){
            ymax = landmarks->item[2*i+1];
        }
    }
    float height = ymax - ymin;
    float width = xmax - xmin;
    if((landmarks->item[8]>xmin) && (landmarks->item[8]<xmax) && (landmarks->item[9]>ymin) && (landmarks->item[9]<ymax)){
        finger_bent[0] = 1;
    }
    if(finger_angles[0][0]>thumb_angle_thresh){
        finger_bent[0] = 1;
    }
//*************************************pose estimation**********************************
    if ((finger_bent[0]==0) && (finger_bent[1]==0) && (finger_bent[2]==0) && (finger_bent[3]==0) && (finger_bent[4]==0)){
        if(abs(palm_angle)<35.0){
            strcpy(pose, "five");
        }else{
            strcpy(pose, "paper");
        }
    }else if((finger_bent[0]==1) && (finger_bent[1]==0) && (finger_bent[2]==0) && (finger_bent[3]==0) && (finger_bent[4]==0)){
        strcpy(pose, "four");
    }else if((finger_bent[0]==0) && (finger_bent[1]==0) && (finger_bent[2]==0) && (finger_bent[3]==1) && (finger_bent[4]==1)){
        strcpy(pose, "three");
    }else if((finger_bent[0]==0) && (finger_bent[1]==0) && (finger_bent[2]==1) && (finger_bent[3]==1) && (finger_bent[4]==1)){
        if(abs(palm_angle)<35.0){
            strcpy(pose, "two");
        }else if(abs(palm_angle)<150.0){
            strcpy(pose, "pa !");
        }else{
            strcpy(pose, "ha ?");
        }
    }else if((finger_bent[0]==1) && (finger_bent[1]==0) && (finger_bent[2]==1) && (finger_bent[3]==1) && (finger_bent[4]==1)){
        if(landmarks->item[17] == ymin){
            strcpy(pose, "one");
        }else if(landmarks->item[17] == ymax){
            strcpy(pose, "down");
        }else if(landmarks->item[16] == xmin){
            strcpy(pose, "left");
        }else if(landmarks->item[16] == xmax){
            strcpy(pose, "right");
        }else{
            strcpy(pose, "ha ?");
        }      
    }else if((finger_bent[0]==1) && (finger_bent[1]==1) && (finger_bent[2]==1) && (finger_bent[3]==1) && (finger_bent[4]==1)){
       if(abs(palm_angle)<45.0){
            strcpy(pose, "fist");
        }else{
            strcpy(pose, "rock");
        }
    }else if((finger_bent[0]==0) && (finger_bent[1]==1) && (finger_bent[2]==1) && (finger_bent[3]==1) && (finger_bent[4]==1) && (landmarks->item[9] == ymin)){
        if(joints_angles[0][3] > 180){
            strcpy(pose, "good");
        }else{
            strcpy(pose, "ha ?");
        }
            
    }else if((finger_bent[0]==0) && (finger_bent[1]==1) && (finger_bent[2]==1) && (finger_bent[3]==1) && (finger_bent[4]==0)){
        strcpy(pose, "six");
    }else if((finger_bent[0]==0) && (finger_bent[1]==0) && (finger_bent[2]==1) && (finger_bent[3]==1) && (finger_bent[4]==0)){
        strcpy(pose, "spiderman");
    }else if((finger_bent[0]==1) && (finger_bent[1]==0) && (finger_bent[2]==1) && (finger_bent[3]==1) && (finger_bent[4]==0)){
        strcpy(pose, "rock u");
    }else if((finger_bent[1]==1) && (finger_bent[2]==0) && (finger_bent[3]==0) && (finger_bent[4]==0)
        && (get_vector_length(landmarks->item[8]-landmarks->item[16], landmarks->item[9]-landmarks->item[17])< (0.2*width))){
        strcpy(pose, "ok");
    }else if((finger_bent[0]==1) && (finger_bent[1]==0) && (finger_bent[2]==0) && (finger_bent[3]==1) && (finger_bent[4]==1)){
        if(abs(palm_angle)<35.0){
            strcpy(pose, "yeah");
        }else{
            strcpy(pose, "scissors");
        }
        
    }else{
        strcpy(pose, "ha ?");
    }
    ets_printf("palm angle: %d, bent: %d, %d, %d, %d, %d pose: %s\n", (int)palm_angle, 
            finger_bent[0], finger_bent[1], finger_bent[2], finger_bent[3], finger_bent[4], pose);

    return pose;
}



// void pe_test()
// {
//     dl_matrix3du_t *hd_image_input = dl_matrix3du_alloc(1,240,320,3);
//     int count = hd_image_input->n*hd_image_input->w*hd_image_input->h*hd_image_input->c;
//     // printf("1 count: %d\n", count);
//     for(int i=0;i<count;i++){
//         hd_image_input->item[i] = image_item[i];
//     }
//     dl_matrix3dq_t *hd_image_resize = od_image_preporcess(hd_image_input->item, 320, 240, 160, -10, 0);
//     printf("input size: %d, %d, %d, %d\n", hd_image_resize->n, hd_image_resize->h,hd_image_resize->w,hd_image_resize->c);
//     dl_matrix3du_free(hd_image_input);
//     hd_config_t hd_config = {0};
//     hd_config.target_size = 160;
//     hd_config.preprocess_mode = 0;
//     hd_config.input_w = 320;
//     hd_config.input_h = 240;                                                                        
//     hd_config.free_input = 1;
//     hd_config.resize_scale = (float)160/320;
//     hd_config.score_threshold = 0.5;
//     hd_config.nms_threshold = 0.45;
//     hd_config.mode = 2;
//     od_box_array_t *hd_boxes = hand_detection_forward(hd_image_resize, hd_config);
//     if (hd_boxes)
//     {
//         //box_array_t *net_boxes = onet_forward(image_mat, lnet_boxes, &onet_config);
//         dl_lib_free(hd_boxes->cls);
//         dl_lib_free(hd_boxes->score);
//         dl_lib_free(hd_boxes->box);
//         dl_lib_free(hd_boxes);
//     }
// }

// void pe_test2()
// {
//     // dl_matrix3dq_t *test_image = dl_matrix3dq_alloc(1,112,112,3,-10);
//     // int count = test_image->n*test_image->w*test_image->h*test_image->c;
//     // printf("count: %d\n", count);
//     // for(int i=0;i<count;i++){
//     //     test_image->item[i] = image_item[i];
//     // }
//     // dl_matrix3d_t *landmarks = hp_nano1_ls16_q(test_image, 2);
//     // for(int i=0; i<21; i++){
//     //     printf("%f, %f\n", landmarks->item[2*i], landmarks->item[2*i+1]);
//     // }
//     hp_test();
// }