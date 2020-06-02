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

#ifdef __cplusplus
extern "C"
{
#endif
#include "dl_lib_matrix3d.h"
#include "dl_lib_matrix3dq.h"
#include "freertos/FreeRTOS.h"

    typedef struct
    {
        int **anchors_shape;
        int stride;
        int boundary;
        int project_offset;
    } lssh_module_config_t;

    typedef struct
    {
        dl_matrix3dq_t *category;
        dl_matrix3dq_t *box_offset;
        dl_matrix3dq_t *landmark_offset;
    } lssh_module_result_t;

    typedef struct
    {
        bool with_landmark;
        int enabled_top_k;
        bool free_image;
        int mode;
    } lssh_model_config_t;

    typedef struct
    {
        lssh_module_config_t *module;
        int modules_total;
        lssh_module_result_t *(*op)(dl_matrix3dq_t *, lssh_model_config_t);
    } lssh_model_t;

    /**
     * @brief 
     * 
     * @param value 
     */
    void lssh_module_result_free(lssh_module_result_t value);

    /**
     * @brief 
     * 
     * @param values 
     * @param length 
     */
    void lssh_module_results_free(lssh_module_result_t *values, int length);

#ifdef __cplusplus
}
#endif
