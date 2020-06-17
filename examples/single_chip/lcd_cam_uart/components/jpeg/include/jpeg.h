#pragma once
#include <stdint.h>
#include "esp_err.h"
#include "tjpgd.h"

#define JPEG_WORK_BUF_SIZE 3100

uint8_t *jpeg_decode(uint8_t *jpeg, int *w, int* h);