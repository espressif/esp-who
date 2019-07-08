// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _APP_MODEL_TEST_H_
#define _APP_MODEL_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "app_camera.h"

typedef struct
{
    int img_num;
    int face_id_num;
    char **path;
    int *enroll;
    int *img_id;
    int *face_id;
} data_info;


void app_sd_card_init();
void app_model_test();

#ifdef __cplusplus
}
#endif

#endif