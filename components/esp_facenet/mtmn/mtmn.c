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
#include <stdlib.h>
#include "mtmn.h"
#include "pnet_model.h"
#include "rnet_model.h"
#include "onet_model.h"
#include "freertos/FreeRTOS.h"

static int first_time[3] = {0};
mtmn_net_t *pnet (dl_matrix3du_t *in)
{
#define PNET_LAYER_NUM (3)
    static dl_matrix3d_t *filters[PNET_LAYER_NUM * 4] = {0};
    char name[50];

    // Get filters
    if (0 == first_time[0])
    {
        first_time[0] = 1;
        for (int i = 0; i < PNET_LAYER_NUM; i++)
        {
            sprintf(name, "pnet_l%d_dilate", i+1);
            filters[i * 4 + 0] = (dl_matrix3d_t *)get_coeff_pnet_model.getter_3d(name, NULL, 0);
            sprintf(name, "pnet_l%d_depth", i+1);
            filters[i * 4 + 1] = (dl_matrix3d_t *)get_coeff_pnet_model.getter_3d(name, NULL, 0);
            sprintf(name, "pnet_l%d_compress", i+1);
            filters[i * 4 + 2] = (dl_matrix3d_t *)get_coeff_pnet_model.getter_3d(name, NULL, 0);
            sprintf(name, "pnet_l%d_bias", i+1);
            filters[i * 4 + 3] = (dl_matrix3d_t *)get_coeff_pnet_model.getter_3d(name, NULL, 0);
        }
    }
    dl_matrix3d_t **l1_f = &(filters[0]);
    dl_matrix3d_t **l2_f = &(filters[4]);
    dl_matrix3d_t **l3_f = &(filters[8]);

    dl_matrix3d_t *l4_category_conv = (dl_matrix3d_t *)get_coeff_pnet_model.getter_3d("pnet_l4_category_weight", NULL, 0);
    dl_matrix3d_t *l4_category_bias = (dl_matrix3d_t *)get_coeff_pnet_model.getter_3d("pnet_l4_category_bias", NULL, 0);
    dl_matrix3d_t *l4_offset_conv = (dl_matrix3d_t *)get_coeff_pnet_model.getter_3d("pnet_l4_offset_weight", NULL, 0);
    dl_matrix3d_t *l4_offset_bias = (dl_matrix3d_t *)get_coeff_pnet_model.getter_3d("pnet_l4_offset_bias", NULL, 0);

    // Layer 1
    dl_matrix3d_conv_config_t config;
    config.stride_x = 2;
    config.stride_y = 2;
    config.padding = PADDING_VALID;
    config.mode = DL_XTENSA_IMPL;
    config.type = INPUT_UINT8;
    dl_matrix3d_t *l1 = dl_matrix3d_mobilenet(in, l1_f[0], l1_f[1], l1_f[2], l1_f[3], NULL, &config);

    // Layer 2
    config.stride_x = 1;
    config.stride_y = 1;
    config.type = INPUT_FLOAT;
    dl_matrix3d_t *l2 = dl_matrix3d_mobilenet(l1, l2_f[0], l2_f[1], l2_f[2], l2_f[3], NULL, &config);
    
    // Layer 3
    dl_matrix3d_t *l3 = dl_matrix3d_mobilenet(l2, l3_f[0], l3_f[1], l3_f[2], l3_f[3], NULL, &config);

    mtmn_net_t *pnet_o = (mtmn_net_t *)calloc(1, sizeof(mtmn_net_t));
    // Layer 4 category
    pnet_o->category = dl_matrix3d_conv(l3, l4_category_conv, l4_category_bias, 1, 1, 0, 1);
    dl_matrix3d_softmax(pnet_o->category);

    // Layer 4 boxoffset 
    pnet_o->offset = dl_matrix3d_conv(l3, l4_offset_conv, l4_offset_bias, 1, 1, 0, 1);

    dl_matrix3d_free(l1);
    dl_matrix3d_free(l2);
    dl_matrix3d_free(l3);

    return pnet_o;
}



mtmn_net_t *rnet_with_score_verify(dl_matrix3du_t *in, float score_threshold)
{
#define RNET_LAYER_NUM (4)
    static dl_matrix3d_t *filters[RNET_LAYER_NUM * 4];
    char name[50];

    // Get filters
    if (0 == first_time[1])
    {
        first_time[1] = 1;
        for (int i = 0; i < RNET_LAYER_NUM; i++)
        {
            sprintf(name, "rnet_l%d_dilate", i+1);        
            filters[i * 4 + 0] = (dl_matrix3d_t *)get_coeff_rnet_model.getter_3d(name, NULL, 0);
            sprintf(name, "rnet_l%d_depth", i+1);
            filters[i * 4 + 1] = (dl_matrix3d_t *)get_coeff_rnet_model.getter_3d(name, NULL, 0);
            sprintf(name, "rnet_l%d_compress", i+1);
            filters[i * 4 + 2] = (dl_matrix3d_t *)get_coeff_rnet_model.getter_3d(name, NULL, 0);
            sprintf(name, "rnet_l%d_bias", i+1);
            filters[i * 4 + 3] = (dl_matrix3d_t *)get_coeff_rnet_model.getter_3d(name, NULL, 0);
        }
    }

    dl_matrix3d_t **l1_f = &(filters[0]);
    dl_matrix3d_t **l2_f = &(filters[4]);
    dl_matrix3d_t **l3_f = &(filters[8]);
    dl_matrix3d_t **l4_f = &(filters[12]);

    dl_matrix3d_t *l5_category_fc = (dl_matrix3d_t *)get_coeff_rnet_model.getter_3d("rnet_l5_category_weight", NULL, 0);
    dl_matrix3d_t *l5_category_bias = (dl_matrix3d_t *)get_coeff_rnet_model.getter_3d("rnet_l5_category_bias", NULL, 0);
    dl_matrix3d_t *l5_offset_fc = (dl_matrix3d_t *)get_coeff_rnet_model.getter_3d("rnet_l5_offset_weight", NULL, 0);
    dl_matrix3d_t *l5_offset_bias = (dl_matrix3d_t *)get_coeff_rnet_model.getter_3d("rnet_l5_offset_bias", NULL, 0);

    // Layer 1
    dl_matrix3d_conv_config_t config;
    config.stride_x = 2;
    config.stride_y = 2;
    config.padding = PADDING_VALID;
    config.mode = DL_XTENSA_IMPL;
    config.type = INPUT_UINT8;
    dl_matrix3d_t *l1 = dl_matrix3d_mobilenet(in, l1_f[0], l1_f[1], l1_f[2], l1_f[3], NULL, &config);

    // Layer 2
    config.type = INPUT_FLOAT;
    dl_matrix3d_t *l2 = dl_matrix3d_mobilenet(l1, l2_f[0], l2_f[1], l2_f[2], l2_f[3], NULL, &config);
    
    // Layer 3
    config.stride_x = 1;
    config.stride_y = 1;
    dl_matrix3d_t *l3 = dl_matrix3d_mobilenet(l2, l3_f[0], l3_f[1], l3_f[2], l3_f[3], NULL, &config);

    // Layer 4
    dl_matrix3d_t *l4 = dl_matrix3d_mobilenet(l3, l4_f[0], l4_f[1], l4_f[2], l4_f[3], NULL, &config);

    mtmn_net_t *rnet_o = (mtmn_net_t *)calloc(1, sizeof(mtmn_net_t));

    // Layer 5 category
    rnet_o->category = dl_matrix3d_fc(l4, l5_category_fc, l5_category_bias);
    dl_matrix3d_softmax(rnet_o->category);

    if (rnet_o->category->item[1] < score_threshold)
    {
        dl_matrix3d_free(rnet_o->category);
        free(rnet_o);
        rnet_o = NULL;
        goto rnet_exit;
    }
    // Layer 5 boxoffset
    rnet_o->offset = dl_matrix3d_fc(l4, l5_offset_fc, l5_offset_bias);

rnet_exit:
    dl_matrix3d_free(l1);
    dl_matrix3d_free(l2);
    dl_matrix3d_free(l3);
    dl_matrix3d_free(l4);

    return rnet_o;
}

mtmn_net_t *onet_with_score_verify(dl_matrix3du_t *in, float score_threshold)
{
#define ONET_LAYER_NUM (5)
#define ONET_COEF_PER_LAYER (5)
    dl_matrix3d_t *filters[ONET_LAYER_NUM * ONET_COEF_PER_LAYER];
    char name[50];

    // Get filters
    for (int i = 0; i < ONET_LAYER_NUM; i++)
    {
        sprintf(name, "onet_l%d_dilate", i+1);
        filters[i * ONET_COEF_PER_LAYER + 0] = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d(name, NULL, 0);
        sprintf(name, "onet_l%d_depth", i+1);
        filters[i * ONET_COEF_PER_LAYER + 1] = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d(name, NULL, 0);
        sprintf(name, "onet_l%d_compress", i+1);
        filters[i * ONET_COEF_PER_LAYER + 2] = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d(name, NULL, 0);
        sprintf(name, "onet_l%d_bias", i+1);
        filters[i * ONET_COEF_PER_LAYER + 3] = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d(name, NULL, 0);
        sprintf(name, "onet_l%d_prelu_alpha", i+1);
        filters[i * ONET_COEF_PER_LAYER + 4] = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d(name, NULL, 0);
    }

    dl_matrix3d_t **l1_f = &(filters[ONET_COEF_PER_LAYER * 0]);
    dl_matrix3d_t **l2_f = &(filters[ONET_COEF_PER_LAYER * 1]);
    dl_matrix3d_t **l3_f = &(filters[ONET_COEF_PER_LAYER * 2]);
    dl_matrix3d_t **l4_f = &(filters[ONET_COEF_PER_LAYER * 3]);
    dl_matrix3d_t **l5_f = &(filters[ONET_COEF_PER_LAYER * 4]);

    dl_matrix3d_t *l6_category_fc = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d("onet_l6_category_weight", NULL, 0);
    dl_matrix3d_t *l6_category_bias = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d("onet_l6_category_bias", NULL, 0);
    dl_matrix3d_t *l6_offset_fc = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d("onet_l6_offset_weight", NULL, 0);
    dl_matrix3d_t *l6_offset_bias = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d("onet_l6_offset_bias", NULL, 0);
    dl_matrix3d_t *l6_landmark_fc = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d("onet_l6_landmark_weight", NULL, 0);
    dl_matrix3d_t *l6_landmark_bias = (dl_matrix3d_t *)get_coeff_onet_model.getter_3d("onet_l6_landmark_bias", NULL, 0);

    // Layer 1
    dl_matrix3d_conv_config_t config;
    config.stride_x = 2;
    config.stride_y = 2;
    config.padding = PADDING_VALID;
    config.mode = DL_C_IMPL;
    config.type = INPUT_UINT8;

    dl_matrix3d_t *l1 = dl_matrix3d_mobilenet(in, l1_f[0], l1_f[1], l1_f[2], l1_f[3], l1_f[4], &config);

    // Layer 2
    config.type = INPUT_FLOAT;
    dl_matrix3d_t *l2 = dl_matrix3d_mobilenet(l1, l2_f[0], l2_f[1], l2_f[2], l2_f[3], l2_f[4], &config);
    
    // Layer 3
    dl_matrix3d_t *l3 = dl_matrix3d_mobilenet(l2, l3_f[0], l3_f[1], l3_f[2], l3_f[3], l3_f[4], &config);

    // Layer 4
    config.stride_x = 1;
    config.stride_y = 1;
    dl_matrix3d_t *l4 = dl_matrix3d_mobilenet(l3, l4_f[0], l4_f[1], l4_f[2], l4_f[3], l4_f[4], &config);

    // Layer 5
    dl_matrix3d_t *l5 = dl_matrix3d_mobilenet(l4, l5_f[0], l5_f[1], l5_f[2], l5_f[3], l5_f[4], &config);

    mtmn_net_t *onet_o = (mtmn_net_t *)calloc(1, sizeof(mtmn_net_t));

    // Layer 6 category
    onet_o->category = dl_matrix3d_fc(l5, l6_category_fc, l6_category_bias);
    dl_matrix3d_softmax(onet_o->category);
    if (onet_o->category->item[1] < score_threshold)
    {
        dl_matrix3d_free(onet_o->category);
        free(onet_o);
        onet_o = NULL;
        goto onet_exit;
    }

    // Layer 6 boxoffset
    onet_o->offset = dl_matrix3d_fc(l5, l6_offset_fc, l6_offset_bias);

    // Layer 6 landmark
    onet_o->landmark = dl_matrix3d_fc(l5, l6_landmark_fc, l6_landmark_bias);


onet_exit:
    dl_matrix3d_free(l1);
    dl_matrix3d_free(l2);
    dl_matrix3d_free(l3);
    dl_matrix3d_free(l4);
    dl_matrix3d_free(l5);

    return onet_o;
}

