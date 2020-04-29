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
#include "lssh.h"

    /**
     * @brief Get the min_face, score_threshold, nms_threshold and shape of input image
     * 
     * @param min_face 
     * @param score_threshold 
     * @param nms_threshold 
     * @param image_height      Input image height
     * @param image_width       Input image width
     * @return lssh_config_t 
     */
    lssh_config_t lssh_get_config(fptp_t min_face, fptp_t score_threshold, fptp_t nms_threshold, int image_height, int image_width);

    /**
     * @brief Update config once the input image shape is changeed
     * 
     * @param config 
     * @param min_face 
     * @param image_height 
     * @param image_width 
     */
    void lssh_update_config(lssh_config_t *config, fptp_t min_face, int image_height, int image_width);

    /**
     * @brief 
     * 
     * @param image         Input image
     * @param config        
     * @return box_array_t* 
     */
    box_array_t *lssh_detect_object(dl_matrix3du_t *image, lssh_config_t config);

#if __cplusplus
}
#endif
