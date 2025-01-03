#pragma once
#include "dl_image_color.hpp"
#include "dl_image_define.hpp"
#include "dl_math_matrix.hpp"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

namespace dl {
namespace image {

void bilinear_interpolate_rgb888(const img_t &img,
                                 float x,
                                 float y,
                                 pix_t &pix,
                                 uint32_t caps,
                                 void *norm_lut,
                                 const std::vector<int> &crop_area = {});
void bilinear_interpolate_rgb565(const img_t &img,
                                 float x,
                                 float y,
                                 pix_t &pix,
                                 uint32_t caps,
                                 void *norm_lut,
                                 const std::vector<int> &crop_area = {});
void bilinear_interpolate_gray(
    const img_t &img, float x, float y, pix_t &pix, void *norm_lut, const std::vector<int> &crop_area = {});
void nearest_interpolate_rgb888(const img_t &img,
                                float x,
                                float y,
                                pix_t &pix,
                                uint32_t caps,
                                void *norm_lut,
                                const std::vector<int> &crop_area = {});
void nearest_interpolate_rgb565(const img_t &img,
                                float x,
                                float y,
                                pix_t &pix,
                                uint32_t caps,
                                void *norm_lut,
                                const std::vector<int> &crop_area = {});
void nearest_interpolate_gray(
    const img_t &img, float x, float y, pix_t &pix, void *norm_lut, const std::vector<int> &crop_area = {});
template <typename T>
void resize_loop(const img_t &src_img,
                 img_t &dst_img,
                 interpolate_type_t interpolate_type,
                 uint32_t caps,
                 void *norm_lut,
                 const std::vector<int> &crop_area,
                 float scale_x,
                 float scale_y);
void resize(const img_t &src_img,
            img_t &dst_img,
            interpolate_type_t interpolate_type,
            uint32_t caps = 0,
            void *norm_lut = nullptr,
            const std::vector<int> &crop_area = {},
            float *scale_x_ret = nullptr,
            float *scale_y_ret = nullptr);
#if CONFIG_IDF_TARGET_ESP32P4
esp_err_t resize_ppa(const img_t &src_img,
                     img_t &dst_img,
                     ppa_client_handle_t ppa_handle,
                     void *ppa_buffer,
                     size_t ppa_buffer_size,
                     ppa_trans_mode_t ppa_mode = PPA_TRANS_MODE_BLOCKING,
                     void *ppa_user_data = nullptr,
                     uint32_t caps = DL_IMAGE_CAP_PPA,
                     void *norm_lut = nullptr,
                     const std::vector<int> &crop_area = {},
                     float *scale_x_ret = nullptr,
                     float *scale_y_ret = nullptr,
                     float ppa_error_thr = 0.3);
#endif
void warp_affine(const img_t &src_img,
                 img_t &dst_img,
                 interpolate_type_t interpolate_type,
                 dl::math::Matrix<float> *M_inv,
                 uint32_t caps = 0,
                 void *norm_lut = nullptr);
} // namespace image
} // namespace dl
