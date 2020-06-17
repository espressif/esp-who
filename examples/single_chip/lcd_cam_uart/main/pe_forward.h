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
#pragma once

#if __cplusplus
extern "C"
{
#endif

#include "image_util.h"
#include "dl_lib_matrix3d.h"
#include "hd_model.h"
#include "hp_model.h"

#define INPUT_EXPONENT -10
#define SCORE_THRESHOLD 0.5
#define NMS_THRESHOLD 0.45

    /**
     * @brief 
     * 
     * @param image         Input image
     * @param score_threshold
     * @param nms_threshold
     * @param mode          
     * @return od_box_array_t* 
     */

    typedef struct
    {
        int target_size;
        int preprocess_mode;
        int input_w;
        int input_h;
        int free_input;
        fptp_t resize_scale;
        fptp_t score_threshold;
        fptp_t nms_threshold;
        int mode;
    } hd_config_t;
    
    od_box_array_t *hand_detection_forward(dl_matrix3dq_t *image, hd_config_t hd_config);
    dl_matrix3d_t *handpose_estimation_forward(dl_matrix3du_t *image, int target_size, od_box_array_t *od_boxes, int mode);
    dl_matrix3d_t *handpose_estimation_forward2(uint16_t *simage, od_box_array_t *od_boxes, int dw, int sw, int sh, int mode);
    void pe_test();
    void pe_test2();

#if __cplusplus
}
#endif
