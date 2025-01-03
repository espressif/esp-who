#pragma once
#include "dl_image_color.hpp"
#include "dl_image_define.hpp"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

namespace dl {
namespace image {
esp_err_t write_bmp(const img_t &img, const char *file_name, uint32_t caps = 0);
esp_err_t read_bmp(img_t &img, const char *file_name);
} // namespace image
} // namespace dl
