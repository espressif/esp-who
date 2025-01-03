#pragma once
#include "dl_image_define.hpp"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32P4
#include "jpeg_decoder.h"
#endif
#if CONFIG_IDF_TARGET_ESP32P4
#include "driver/jpeg_decode.h"
#include "driver/jpeg_encode.h"
#endif

namespace dl {
namespace image {
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32P4
esp_err_t sw_decode_jpeg(const jpeg_img_t &jpeg_img,
                         img_t &decoded_img,
                         bool swap_color_bytes = false,
                         esp_jpeg_image_scale_t scale = JPEG_IMAGE_SCALE_0);
#endif
#if CONFIG_IDF_TARGET_ESP32P4
esp_err_t hw_decode_jpeg(const jpeg_img_t &jpeg_img, img_t &decoded_img, bool swap_color_bytes = false);
esp_err_t hw_encode_jpeg(const img_t &img,
                         jpeg_img_t &encoded_img,
                         jpeg_down_sampling_type_t sub_sample_mode = JPEG_DOWN_SAMPLING_YUV420,
                         uint8_t img_quality = 80);
inline jpeg_dec_output_format_t convert_pix_type_to_dec_output_fmt(pix_type_t type)
{
    if (type == DL_IMAGE_PIX_TYPE_RGB888)
        return JPEG_DECODE_OUT_FORMAT_RGB888;
    else if (type == DL_IMAGE_PIX_TYPE_RGB565)
        return JPEG_DECODE_OUT_FORMAT_RGB565;
    else if (type == DL_IMAGE_PIX_TYPE_GRAY)
        return JPEG_DECODE_OUT_FORMAT_GRAY;
    else {
        ESP_LOGE("dl::image", "Decode output format can not be quanted type.");
        return JPEG_DECODE_OUT_FORMAT_RGB888;
    }
}
inline jpeg_enc_input_format_t convert_pix_type_to_enc_input_fmt(pix_type_t type)
{
    if (type == DL_IMAGE_PIX_TYPE_RGB888)
        return JPEG_ENCODE_IN_FORMAT_RGB888;
    else if (type == DL_IMAGE_PIX_TYPE_RGB565)
        return JPEG_ENCODE_IN_FORMAT_RGB565;
    else if (type == DL_IMAGE_PIX_TYPE_GRAY)
        return JPEG_ENCODE_IN_FORMAT_GRAY;
    else {
        ESP_LOGE("dl::image", "Encode input format can not be quanted type.");
        return JPEG_ENCODE_IN_FORMAT_RGB888;
    }
}
esp_err_t write_jpeg(img_t &img, const char *file_name);
#endif
esp_err_t write_jpeg(jpeg_img_t &img, const char *file_name);
} // namespace image
} // namespace dl
