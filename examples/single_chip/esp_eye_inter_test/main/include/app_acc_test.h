#pragma once

#include "app_main.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include "fr_flash.h"
#include "image_util.h"
#include "fb_gfx.h"
#include <string.h>
#include "dl_lib.h"
#include "modified_func.h"

#define ENROLL_CONFIRM_TIMES 3

void face_acc_test();
void accuracy_process(int mode,int face_num);
void acc_cal(float *dist, uint8_t *issame_list, int pairs_num);