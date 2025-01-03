#include "dl_image_process.hpp"
static const char *TAG = "dl_image_process";

namespace dl {
namespace image {
void bilinear_interpolate_rgb888(
    const img_t &img, float x, float y, pix_t &pix, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area)
{
    if (crop_area.empty()) {
        x = std::max(std::min(x, (float)(img.width - 1)), 0.f);
        y = std::max(std::min(y, (float)(img.height - 1)), 0.f);
    } else {
        x = std::max(std::min(x + crop_area[0], (float)(crop_area[2] - 1)), (float)crop_area[0]);
        y = std::max(std::min(y + crop_area[1], (float)(crop_area[3] - 1)), (float)crop_area[1]);
    }

    int x1 = (int)x;
    int x2 = x1 + 1;

    int y1 = (int)y;
    int y2 = y1 + 1;

    uint8_t *img_data_ptr = (uint8_t *)img.data;
    uint8_t *Q1_ptr = img_data_ptr + 3 * x1 + 3 * img.width * y1;
    uint8_t *Q2_ptr = img_data_ptr + 3 * x2 + 3 * img.width * y1;
    uint8_t *Q3_ptr = img_data_ptr + 3 * x1 + 3 * img.width * y2;
    uint8_t *Q4_ptr = img_data_ptr + 3 * x2 + 3 * img.width * y2;

    float A = (x2 - x) * (y2 - y);
    float B = (x - x1) * (y2 - y);
    float C = (x2 - x) * (y - y1);
    float D = (x - x1) * (y - y1);

    if (pix.type == DL_IMAGE_PIX_TYPE_RGB888) {
        uint8_t *dst_ptr = (uint8_t *)pix.data;
        if (caps & DL_IMAGE_CAP_RGB_SWAP) {
            for (int i = 0; i < 3; i++) {
                dst_ptr[2 - i] = (uint8_t)(A * Q1_ptr[i] + B * Q2_ptr[i] + C * Q3_ptr[i] + D * Q4_ptr[i] + 0.5f);
            }
        } else {
            for (int i = 0; i < 3; i++) {
                dst_ptr[i] = (uint8_t)(A * Q1_ptr[i] + B * Q2_ptr[i] + C * Q3_ptr[i] + D * Q4_ptr[i] + 0.5f);
            }
        }
    } else {
        uint8_t tmp[3];
        for (int i = 0; i < 3; i++) {
            tmp[i] = (uint8_t)(A * Q1_ptr[i] + B * Q2_ptr[i] + C * Q3_ptr[i] + D * Q4_ptr[i] + 0.5f);
        }
        pix_t Q = {.data = tmp, .type = DL_IMAGE_PIX_TYPE_RGB888};
        convert_pixel(Q, pix, caps, norm_lut);
    }
}

void bilinear_interpolate_rgb565(
    const img_t &img, float x, float y, pix_t &pix, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area)
{
    if (crop_area.empty()) {
        x = std::max(std::min(x, (float)(img.width - 1)), 0.f);
        y = std::max(std::min(y, (float)(img.height - 1)), 0.f);
    } else {
        x = std::max(std::min(x + crop_area[0], (float)(crop_area[2] - 1)), (float)crop_area[0]);
        y = std::max(std::min(y + crop_area[1], (float)(crop_area[3] - 1)), (float)crop_area[1]);
    }

    int x1 = (int)x;
    int x2 = x1 + 1;

    int y1 = (int)y;
    int y2 = y1 + 1;

    uint16_t *img_data_ptr = (uint16_t *)img.data;
    pix_t Q1_rgb565 = {.data = (void *)(img_data_ptr + x1 + img.width * y1), .type = DL_IMAGE_PIX_TYPE_RGB565};
    pix_t Q2_rgb565 = {.data = (void *)(img_data_ptr + x2 + img.width * y1), .type = DL_IMAGE_PIX_TYPE_RGB565};
    pix_t Q3_rgb565 = {.data = (void *)(img_data_ptr + x1 + img.width * y2), .type = DL_IMAGE_PIX_TYPE_RGB565};
    pix_t Q4_rgb565 = {.data = (void *)(img_data_ptr + x2 + img.width * y2), .type = DL_IMAGE_PIX_TYPE_RGB565};
    uint8_t tmp[12];
    uint8_t *Q1_ptr = tmp;
    uint8_t *Q2_ptr = tmp + 3;
    uint8_t *Q3_ptr = tmp + 6;
    uint8_t *Q4_ptr = tmp + 9;
    pix_t Q1 = {.data = (void *)Q1_ptr, .type = DL_IMAGE_PIX_TYPE_RGB888};
    pix_t Q2 = {.data = (void *)Q2_ptr, .type = DL_IMAGE_PIX_TYPE_RGB888};
    pix_t Q3 = {.data = (void *)Q3_ptr, .type = DL_IMAGE_PIX_TYPE_RGB888};
    pix_t Q4 = {.data = (void *)Q4_ptr, .type = DL_IMAGE_PIX_TYPE_RGB888};
    convert_pixel(Q1_rgb565, Q1, caps, nullptr);
    convert_pixel(Q2_rgb565, Q2, caps, nullptr);
    convert_pixel(Q3_rgb565, Q3, caps, nullptr);
    convert_pixel(Q4_rgb565, Q4, caps, nullptr);

    float A = (x2 - x) * (y2 - y);
    float B = (x - x1) * (y2 - y);
    float C = (x2 - x) * (y - y1);
    float D = (x - x1) * (y - y1);

    if (pix.type == DL_IMAGE_PIX_TYPE_RGB888) {
        uint8_t *dst_ptr = (uint8_t *)pix.data;
        for (int i = 0; i < 3; i++) {
            dst_ptr[i] = (uint8_t)(A * Q1_ptr[i] + B * Q2_ptr[i] + C * Q3_ptr[i] + D * Q4_ptr[i] + 0.5f);
        }
    } else {
        uint8_t tmp[3];
        for (int i = 0; i < 3; i++) {
            tmp[i] = (uint8_t)(A * Q1_ptr[i] + B * Q2_ptr[i] + C * Q3_ptr[i] + D * Q4_ptr[i] + 0.5f);
        }
        pix_t Q = {.data = tmp, .type = DL_IMAGE_PIX_TYPE_RGB888};
        convert_pixel(Q, pix, 0, norm_lut);
    }
}

void bilinear_interpolate_gray(
    const img_t &img, float x, float y, pix_t &pix, void *norm_lut, const std::vector<int> &crop_area)
{
    if (crop_area.empty()) {
        x = std::max(std::min(x, (float)(img.width - 1)), 0.f);
        y = std::max(std::min(y, (float)(img.height - 1)), 0.f);
    } else {
        x = std::max(std::min(x + crop_area[0], (float)(crop_area[2] - 1)), (float)crop_area[0]);
        y = std::max(std::min(y + crop_area[1], (float)(crop_area[3] - 1)), (float)crop_area[1]);
    }

    int x1 = (int)x;
    int x2 = x1 + 1;

    int y1 = (int)y;
    int y2 = y1 + 1;

    uint8_t *img_data_ptr = (uint8_t *)img.data;
    uint8_t Q1 = img_data_ptr[x1 + img.width * y1];
    uint8_t Q2 = img_data_ptr[x2 + img.width * y1];
    uint8_t Q3 = img_data_ptr[x1 + img.width * y2];
    uint8_t Q4 = img_data_ptr[x2 + img.width * y2];

    float A = (x2 - x) * (y2 - y);
    float B = (x - x1) * (y2 - y);
    float C = (x2 - x) * (y - y1);
    float D = (x - x1) * (y - y1);

    if (pix.type == DL_IMAGE_PIX_TYPE_GRAY) {
        *((uint8_t *)pix.data) = (uint8_t)(A * Q1 + B * Q2 + C * Q3 + D * Q4 + 0.5f);
    } else {
        uint8_t tmp = (uint8_t)(A * Q1 + B * Q2 + C * Q3 + D * Q4 + 0.5f);
        pix_t Q = {.data = &tmp, .type = DL_IMAGE_PIX_TYPE_GRAY};
        convert_pixel(Q, pix, 0, norm_lut);
    }
}

void nearest_interpolate_rgb888(
    const img_t &img, float x, float y, pix_t &pix, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area)
{
    if (crop_area.empty()) {
        x = std::max(std::min(x, (float)(img.width - 1)), 0.f);
        y = std::max(std::min(y, (float)(img.height - 1)), 0.f);
    } else {
        x = std::max(std::min(x + crop_area[0], (float)(crop_area[2] - 1)), (float)crop_area[0]);
        y = std::max(std::min(y + crop_area[1], (float)(crop_area[3] - 1)), (float)crop_area[1]);
    }
    int x1 = (int)(x + 0.5f);
    int y1 = (int)(y + 0.5f);

    pix_t Q = {.data = (void *)((uint8_t *)img.data + 3 * x1 + 3 * img.width * y1), .type = DL_IMAGE_PIX_TYPE_RGB888};
    convert_pixel(Q, pix, caps, norm_lut);
}

void nearest_interpolate_rgb565(
    const img_t &img, float x, float y, pix_t &pix, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area)
{
    if (crop_area.empty()) {
        x = std::max(std::min(x, (float)(img.width - 1)), 0.f);
        y = std::max(std::min(y, (float)(img.height - 1)), 0.f);
    } else {
        x = std::max(std::min(x + crop_area[0], (float)(crop_area[2] - 1)), (float)crop_area[0]);
        y = std::max(std::min(y + crop_area[1], (float)(crop_area[3] - 1)), (float)crop_area[1]);
    }
    int x1 = (int)(x + 0.5f);
    int y1 = (int)(y + 0.5f);

    pix_t Q = {.data = (void *)((uint16_t *)img.data + x1 + img.width * y1), .type = DL_IMAGE_PIX_TYPE_RGB565};
    convert_pixel(Q, pix, caps, norm_lut);
}

void nearest_interpolate_gray(
    const img_t &img, float x, float y, pix_t &pix, void *norm_lut, const std::vector<int> &crop_area)
{
    if (crop_area.empty()) {
        x = std::max(std::min(x, (float)(img.width - 1)), 0.f);
        y = std::max(std::min(y, (float)(img.height - 1)), 0.f);
    } else {
        x = std::max(std::min(x + crop_area[0], (float)(crop_area[2] - 1)), (float)crop_area[0]);
        y = std::max(std::min(y + crop_area[1], (float)(crop_area[3] - 1)), (float)crop_area[1]);
    }
    int x1 = (int)(x + 0.5f);
    int y1 = (int)(y + 0.5f);

    pix_t Q = {.data = (void *)((uint8_t *)img.data + x1 + img.width * y1), .type = DL_IMAGE_PIX_TYPE_GRAY};
    convert_pixel(Q, pix, 0, norm_lut);
}

template <typename T>
void resize_loop(const img_t &src_img,
                 img_t &dst_img,
                 interpolate_type_t interpolate_type,
                 uint32_t caps,
                 void *norm_lut,
                 const std::vector<int> &crop_area,
                 float scale_x,
                 float scale_y)
{
    float x, y;
    T *pix_ptr = (T *)dst_img.data;
    pix_t pix;
    pix.type = dst_img.pix_type;
    int step = DL_IMAGE_IS_PIX_TYPE_RGB888(pix.type) ? 3 : 1;

    float scale_x_inv = 1.f / scale_x;
    float scale_y_inv = 1.f / scale_y;
    switch (interpolate_type) {
    case DL_IMAGE_INTERPOLATE_BILINEAR:
        if (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888) {
            for (int i = 0; i < dst_img.height; i++) {
                y = (i + 0.5f) * scale_y_inv - 0.5f;
                for (int j = 0; j < dst_img.width; j++) {
                    x = (j + 0.5f) * scale_x_inv - 0.5f;
                    pix.data = (void *)pix_ptr;
                    bilinear_interpolate_rgb888(src_img, x, y, pix, caps, norm_lut, crop_area);
                    pix_ptr += step;
                }
            }
        } else if (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565) {
            for (int i = 0; i < dst_img.height; i++) {
                y = (i + 0.5f) * scale_y_inv - 0.5f;
                for (int j = 0; j < dst_img.width; j++) {
                    x = (j + 0.5f) * scale_x_inv - 0.5f;
                    pix.data = (void *)pix_ptr;
                    bilinear_interpolate_rgb565(src_img, x, y, pix, caps, norm_lut, crop_area);
                    pix_ptr += step;
                }
            }
        } else if (src_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY) {
            for (int i = 0; i < dst_img.height; i++) {
                y = (i + 0.5f) * scale_y_inv - 0.5f;
                for (int j = 0; j < dst_img.width; j++) {
                    x = (j + 0.5f) * scale_x_inv - 0.5f;
                    pix.data = (void *)pix_ptr;
                    bilinear_interpolate_gray(src_img, x, y, pix, norm_lut, crop_area);
                    pix_ptr += step;
                }
            }
        } else {
            ESP_LOGE(TAG, "Do not support quant img type.");
        }
        break;
    case DL_IMAGE_INTERPOLATE_NEAREST:
        if (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888) {
            for (int i = 0; i < dst_img.height; i++) {
                y = (i + 0.5f) * scale_y_inv - 0.5f;
                for (int j = 0; j < dst_img.width; j++) {
                    x = (j + 0.5f) * scale_x_inv - 0.5f;
                    pix.data = (void *)pix_ptr;
                    nearest_interpolate_rgb888(src_img, x, y, pix, caps, norm_lut, crop_area);
                    pix_ptr += step;
                }
            }
        } else if (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565) {
            for (int i = 0; i < dst_img.height; i++) {
                y = (i + 0.5f) * scale_y_inv - 0.5f;
                for (int j = 0; j < dst_img.width; j++) {
                    x = (j + 0.5f) * scale_x_inv - 0.5f;
                    pix.data = (void *)pix_ptr;
                    nearest_interpolate_rgb565(src_img, x, y, pix, caps, norm_lut, crop_area);
                    pix_ptr += step;
                }
            }
        } else if (src_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY) {
            for (int i = 0; i < dst_img.height; i++) {
                y = (i + 0.5f) * scale_y_inv - 0.5f;
                for (int j = 0; j < dst_img.width; j++) {
                    x = (j + 0.5f) * scale_x_inv - 0.5f;
                    pix.data = (void *)pix_ptr;
                    nearest_interpolate_gray(src_img, x, y, pix, norm_lut, crop_area);
                    pix_ptr += step;
                }
            }
        } else {
            ESP_LOGE(TAG, "Do not support quant img type");
        }
        break;
    }
}

template void resize_loop<uint8_t>(const img_t &src_img,
                                   img_t &dst_img,
                                   interpolate_type_t interpolate_type,
                                   uint32_t caps,
                                   void *norm_lut,
                                   const std::vector<int> &crop_area,
                                   float scale_x,
                                   float scale_y);
template void resize_loop<int8_t>(const img_t &src_img,
                                  img_t &dst_img,
                                  interpolate_type_t interpolate_type,
                                  uint32_t caps,
                                  void *norm_lut,
                                  const std::vector<int> &crop_area,
                                  float scale_x,
                                  float scale_y);
template void resize_loop<int16_t>(const img_t &src_img,
                                   img_t &dst_img,
                                   interpolate_type_t interpolate_type,
                                   uint32_t caps,
                                   void *norm_lut,
                                   const std::vector<int> &crop_area,
                                   float scale_x,
                                   float scale_y);
template void resize_loop<uint16_t>(const img_t &src_img,
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
            uint32_t caps,
            void *norm_lut,
            const std::vector<int> &crop_area,
            float *scale_x_ret,
            float *scale_y_ret)
{
    assert(src_img.data);
    assert(dst_img.data);
    assert(src_img.height > 0 && src_img.width > 0);
    assert(dst_img.height > 0 && dst_img.width > 0);

    float scale_x, scale_y;
    if (crop_area.empty()) {
        scale_x = (float)dst_img.width / (float)src_img.width;
        scale_y = (float)dst_img.height / (float)src_img.height;
    } else {
        assert(crop_area.size() == 4);
        assert(crop_area[2] > crop_area[0]);
        assert(crop_area[3] > crop_area[1]);
        assert(crop_area[0] < src_img.width && crop_area[0] >= 0);
        assert(crop_area[1] < src_img.height && crop_area[1] >= 0);
        assert(crop_area[2] <= src_img.width && crop_area[2] > 0);
        assert(crop_area[3] <= src_img.height && crop_area[3] > 0);
        scale_x = (float)dst_img.width / (float)(crop_area[2] - crop_area[0]);
        scale_y = (float)dst_img.height / (float)(crop_area[3] - crop_area[1]);
    }
    if (scale_x_ret) {
        *scale_x_ret = scale_x;
    }
    if (scale_y_ret) {
        *scale_y_ret = scale_y;
    }
    if (scale_x == 1 && scale_y == 1) {
        printf("%ld\n", caps);
        convert_img(src_img, dst_img, caps, norm_lut, crop_area);
        return;
    }

    switch (dst_img.pix_type) {
    case DL_IMAGE_PIX_TYPE_RGB888:
    case DL_IMAGE_PIX_TYPE_GRAY:
        resize_loop<uint8_t>(src_img, dst_img, interpolate_type, caps, norm_lut, crop_area, scale_x, scale_y);
        break;
    case DL_IMAGE_PIX_TYPE_RGB888_QINT8:
    case DL_IMAGE_PIX_TYPE_GRAY_QINT8:
        resize_loop<int8_t>(src_img, dst_img, interpolate_type, caps, norm_lut, crop_area, scale_x, scale_y);
        break;
    case DL_IMAGE_PIX_TYPE_RGB888_QINT16:
    case DL_IMAGE_PIX_TYPE_GRAY_QINT16:
        resize_loop<int16_t>(src_img, dst_img, interpolate_type, caps, norm_lut, crop_area, scale_x, scale_y);
        break;
    case DL_IMAGE_PIX_TYPE_RGB565:
        resize_loop<uint16_t>(src_img, dst_img, interpolate_type, caps, norm_lut, crop_area, scale_x, scale_y);
        break;
    }
}

#if CONFIG_IDF_TARGET_ESP32P4
esp_err_t resize_ppa(const img_t &src_img,
                     img_t &dst_img,
                     ppa_client_handle_t ppa_handle,
                     void *ppa_buffer,
                     size_t ppa_buffer_size,
                     ppa_trans_mode_t ppa_mode,
                     void *ppa_user_data,
                     uint32_t caps,
                     void *norm_lut,
                     const std::vector<int> &crop_area,
                     float *scale_x_ret,
                     float *scale_y_ret,
                     float ppa_err_pct_thr)
{
    if (!(caps & DL_IMAGE_CAP_PPA)) {
        return ESP_FAIL;
    }
    ppa_srm_color_mode_t input_srm_color_mode;
    if (convert_pix_type_to_ppa_srm_fmt(src_img.pix_type, &input_srm_color_mode) == ESP_FAIL) {
        return ESP_FAIL;
    }
    assert(ppa_handle);
    assert(ppa_buffer);
    assert(src_img.data);
    assert(dst_img.data);
    assert(src_img.height > 0 && src_img.width > 0);
    assert(dst_img.height > 0 && dst_img.width > 0);

    ppa_srm_oper_config_t srm_oper_config;
    memset(&srm_oper_config, 0, sizeof(ppa_srm_oper_config_t));
    float ppa_scale_x, ppa_scale_y;
    if (crop_area.empty()) {
        float scale_x = (float)dst_img.width / (float)src_img.width;
        float scale_y = (float)dst_img.height / (float)src_img.height;
        if (scale_x == 1 && scale_y == 1) {
            return convert_img_ppa(
                src_img, dst_img, ppa_handle, ppa_buffer, ppa_buffer_size, caps, norm_lut, crop_area);
        }
        // ppa use 8 bit to store int part of scale, 4 bit to store frac part of scale.
        if (!(scale_x >= 0.0625 && scale_x < 256 && scale_y >= 0.0625 && scale_y < 256)) {
            return ESP_FAIL;
        }
        float scale_x_int, scale_y_int;
        float scale_x_frac = modf(scale_x, &scale_x_int);
        float scale_y_frac = modf(scale_y, &scale_y_int);
        scale_x_frac = floorf((scale_x_frac) / 0.0625) * 0.0625;
        scale_y_frac = floorf((scale_y_frac) / 0.0625) * 0.0625;
        ppa_scale_x = scale_x_int + scale_x_frac;
        ppa_scale_y = scale_y_int + scale_y_frac;
        float err_pct_x = ((float)dst_img.width - ppa_scale_x * (float)src_img.width) / (float)dst_img.width;
        float err_pct_y = ((float)dst_img.height - ppa_scale_y * (float)src_img.height) / (float)dst_img.height;
        // black border caused by inacurrate scale, avoid using ppa if it exceeds the threshold.
        if (!(err_pct_x < ppa_err_pct_thr && err_pct_y < ppa_err_pct_thr)) {
            return ESP_FAIL;
        }
        if (scale_x_ret) {
            *scale_x_ret = ppa_scale_x;
        }
        if (scale_y_ret) {
            *scale_y_ret = ppa_scale_y;
        }
        srm_oper_config.in.block_offset_y = 0;
        srm_oper_config.in.block_offset_x = 0;
        srm_oper_config.in.block_h = src_img.height;
        srm_oper_config.in.block_w = src_img.width;
    } else {
        assert(crop_area.size() == 4);
        assert(crop_area[2] > crop_area[0]);
        assert(crop_area[3] > crop_area[1]);
        assert(crop_area[0] < src_img.width && crop_area[0] >= 0);
        assert(crop_area[1] < src_img.height && crop_area[1] >= 0);
        assert(crop_area[2] <= src_img.width && crop_area[2] > 0);
        assert(crop_area[3] <= src_img.height && crop_area[3] > 0);
        float scale_x = (float)dst_img.width / (float)(crop_area[2] - crop_area[0]);
        float scale_y = (float)dst_img.height / (float)(crop_area[3] - crop_area[1]);
        if (scale_x == 1 && scale_y == 1) {
            return convert_img_ppa(
                src_img, dst_img, ppa_handle, ppa_buffer, ppa_buffer_size, caps, norm_lut, crop_area);
        }
        // ppa use 8 bit to store int part of scale, 4 bit to store frac part of scale.
        if (!(scale_x >= 0.0625 && scale_x < 256 && scale_y >= 0.0625 && scale_y < 256)) {
            return ESP_FAIL;
        }
        float scale_x_int, scale_y_int;
        float scale_x_frac = modf(scale_x, &scale_x_int);
        float scale_y_frac = modf(scale_y, &scale_y_int);
        scale_x_frac = floorf((scale_x_frac) / 0.0625) * 0.0625;
        scale_y_frac = floorf((scale_y_frac) / 0.0625) * 0.0625;
        ppa_scale_x = scale_x_int + scale_x_frac;
        ppa_scale_y = scale_y_int + scale_y_frac;
        float err_pct_x =
            ((float)dst_img.width - ppa_scale_x * (float)(crop_area[2] - crop_area[0])) / (float)dst_img.width;
        float err_pct_y =
            ((float)dst_img.height - ppa_scale_y * (float)(crop_area[3] - crop_area[1])) / (float)dst_img.height;
        // black border caused by inacurrate scale, avoid using ppa if it exceeds the threshold.
        if (!(err_pct_x < ppa_err_pct_thr && err_pct_y < ppa_err_pct_thr)) {
            return ESP_FAIL;
        }
        if (scale_x_ret) {
            *scale_x_ret = ppa_scale_x;
        }
        if (scale_y_ret) {
            *scale_y_ret = ppa_scale_y;
        }
        srm_oper_config.in.block_offset_y = crop_area[1];
        srm_oper_config.in.block_offset_x = crop_area[0];
        srm_oper_config.in.block_h = crop_area[3] - crop_area[1];
        srm_oper_config.in.block_w = crop_area[2] - crop_area[0];
    }
    srm_oper_config.in.buffer = (const void *)src_img.data;
    srm_oper_config.in.pic_h = src_img.height;
    srm_oper_config.in.pic_w = src_img.width;
    srm_oper_config.in.srm_cm = input_srm_color_mode;
    srm_oper_config.rgb_swap = caps & DL_IMAGE_CAP_RGB_SWAP;
    srm_oper_config.byte_swap =
        src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565 && !(caps & DL_IMAGE_CAP_RGB565_BIG_ENDIAN);

    srm_oper_config.out.buffer = ppa_buffer;
    srm_oper_config.out.buffer_size = ppa_buffer_size;
    srm_oper_config.out.pic_h = dst_img.height;
    srm_oper_config.out.pic_w = dst_img.width;
    srm_oper_config.out.block_offset_x = 0;
    srm_oper_config.out.block_offset_y = 0;
    bool need_convert = false;
    ppa_srm_color_mode_t output_srm_color_mode;
    if (norm_lut || convert_pix_type_to_ppa_srm_fmt(dst_img.pix_type, &output_srm_color_mode) == ESP_FAIL) {
        output_srm_color_mode = PPA_SRM_COLOR_MODE_RGB888;
        need_convert = true;
    }
    srm_oper_config.out.srm_cm = output_srm_color_mode;
    srm_oper_config.rotation_angle = PPA_SRM_ROTATION_ANGLE_0;

    srm_oper_config.scale_x = ppa_scale_x;
    srm_oper_config.scale_y = ppa_scale_y;
    srm_oper_config.mirror_x = false;
    srm_oper_config.mirror_y = false;
    srm_oper_config.mode = ppa_mode;
    srm_oper_config.user_data = ppa_user_data;
    memset(ppa_buffer, 0, ppa_buffer_size);
    ESP_ERROR_CHECK(ppa_do_scale_rotate_mirror(ppa_handle, &srm_oper_config));
    if (need_convert) {
        img_t ppa_output_img = {
            .data = ppa_buffer, .width = dst_img.width, .height = dst_img.height, .pix_type = DL_IMAGE_PIX_TYPE_RGB888};
        convert_img(ppa_output_img, dst_img, 0, norm_lut);
    } else {
        if (dst_img.data != ppa_buffer) {
            tool::copy_memory(dst_img.data, ppa_buffer, get_img_byte_size(dst_img));
        }
    }
    return ESP_OK;
}
#endif
template <typename T>
void warp_affine_loop(const img_t &src_img,
                      img_t &dst_img,
                      interpolate_type_t interpolate_type,
                      dl::math::Matrix<float> *M_inv,
                      uint32_t caps,
                      void *norm_lut)
{
    float x, y;
    T *pix_ptr = (T *)dst_img.data;
    pix_t pix;
    pix.type = dst_img.pix_type;
    int step = DL_IMAGE_IS_PIX_TYPE_RGB888(pix.type) ? 3 : 1;

    float Ax, Bx, Ay, By;
    switch (interpolate_type) {
    case DL_IMAGE_INTERPOLATE_BILINEAR:
        if (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888) {
            for (int i = 0; i < dst_img.height; i++) {
                Bx = M_inv->array[0][1] * i;
                By = M_inv->array[1][1] * i;
                for (int j = 0; j < dst_img.width; j++) {
                    Ax = M_inv->array[0][0] * j;
                    Ay = M_inv->array[1][0] * j;
                    x = Ax + Bx + M_inv->array[0][2];
                    y = Ay + By + M_inv->array[1][2];
                    pix.data = (void *)pix_ptr;
                    bilinear_interpolate_rgb888(src_img, x, y, pix, caps, norm_lut);
                    pix_ptr += step;
                }
            }
        } else if (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565) {
            for (int i = 0; i < dst_img.height; i++) {
                Bx = M_inv->array[0][1] * i;
                By = M_inv->array[1][1] * i;
                for (int j = 0; j < dst_img.width; j++) {
                    Ax = M_inv->array[0][0] * j;
                    Ay = M_inv->array[1][0] * j;
                    x = Ax + Bx + M_inv->array[0][2];
                    y = Ay + By + M_inv->array[1][2];
                    pix.data = (void *)pix_ptr;
                    bilinear_interpolate_rgb565(src_img, x, y, pix, caps, norm_lut);
                    pix_ptr += step;
                }
            }
        } else if (src_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY) {
            for (int i = 0; i < dst_img.height; i++) {
                Bx = M_inv->array[0][1] * i;
                By = M_inv->array[1][1] * i;
                for (int j = 0; j < dst_img.width; j++) {
                    Ax = M_inv->array[0][0] * j;
                    Ay = M_inv->array[1][0] * j;
                    x = Ax + Bx + M_inv->array[0][2];
                    y = Ay + By + M_inv->array[1][2];
                    pix.data = (void *)pix_ptr;
                    bilinear_interpolate_gray(src_img, x, y, pix, norm_lut);
                    pix_ptr += step;
                }
            }
        } else {
            ESP_LOGE(TAG, "Do not support quant img type.");
        }
        break;
    case DL_IMAGE_INTERPOLATE_NEAREST:
        if (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB888) {
            for (int i = 0; i < dst_img.height; i++) {
                Bx = M_inv->array[0][1] * i;
                By = M_inv->array[1][1] * i;
                for (int j = 0; j < dst_img.width; j++) {
                    Ax = M_inv->array[0][0] * j;
                    Ay = M_inv->array[1][0] * j;
                    x = Ax + Bx + M_inv->array[0][2];
                    y = Ay + By + M_inv->array[1][2];
                    pix.data = (void *)pix_ptr;
                    nearest_interpolate_rgb888(src_img, x, y, pix, caps, norm_lut);
                    pix_ptr += step;
                }
            }
        } else if (src_img.pix_type == DL_IMAGE_PIX_TYPE_RGB565) {
            for (int i = 0; i < dst_img.height; i++) {
                Bx = M_inv->array[0][1] * i;
                By = M_inv->array[1][1] * i;
                for (int j = 0; j < dst_img.width; j++) {
                    Ax = M_inv->array[0][0] * j;
                    Ay = M_inv->array[1][0] * j;
                    x = Ax + Bx + M_inv->array[0][2];
                    y = Ay + By + M_inv->array[1][2];
                    pix.data = (void *)pix_ptr;
                    nearest_interpolate_rgb565(src_img, x, y, pix, caps, norm_lut);
                    pix_ptr += step;
                }
            }
        } else if (src_img.pix_type == DL_IMAGE_PIX_TYPE_GRAY) {
            for (int i = 0; i < dst_img.height; i++) {
                Bx = M_inv->array[0][1] * i;
                By = M_inv->array[1][1] * i;
                for (int j = 0; j < dst_img.width; j++) {
                    Ax = M_inv->array[0][0] * j;
                    Ay = M_inv->array[1][0] * j;
                    x = Ax + Bx + M_inv->array[0][2];
                    y = Ay + By + M_inv->array[1][2];
                    pix.data = (void *)pix_ptr;
                    nearest_interpolate_gray(src_img, x, y, pix, norm_lut);
                    pix_ptr += step;
                }
            }
        } else {
            ESP_LOGE(TAG, "Do not support quant img type");
        }
        break;
    }
}

template void warp_affine_loop<uint8_t>(const img_t &src_img,
                                        img_t &dst_img,
                                        interpolate_type_t interpolate_type,
                                        dl::math::Matrix<float> *M_inv,
                                        uint32_t caps,
                                        void *norm_lut);
template void warp_affine_loop<int8_t>(const img_t &src_img,
                                       img_t &dst_img,
                                       interpolate_type_t interpolate_type,
                                       dl::math::Matrix<float> *M_inv,
                                       uint32_t caps,
                                       void *norm_lut);
template void warp_affine_loop<int16_t>(const img_t &src_img,
                                        img_t &dst_img,
                                        interpolate_type_t interpolate_type,
                                        dl::math::Matrix<float> *M_inv,
                                        uint32_t caps,
                                        void *norm_lut);
template void warp_affine_loop<uint16_t>(const img_t &src_img,
                                         img_t &dst_img,
                                         interpolate_type_t interpolate_type,
                                         dl::math::Matrix<float> *M_inv,
                                         uint32_t caps,
                                         void *norm_lut);

void warp_affine(const img_t &src_img,
                 img_t &dst_img,
                 interpolate_type_t interpolate_type,
                 dl::math::Matrix<float> *M_inv,
                 uint32_t caps,
                 void *norm_lut)
{
    assert(src_img.data);
    assert(dst_img.data);
    assert(src_img.height > 0 && src_img.width > 0);
    assert(dst_img.height > 0 && dst_img.width > 0);

    switch (dst_img.pix_type) {
    case DL_IMAGE_PIX_TYPE_RGB888:
    case DL_IMAGE_PIX_TYPE_GRAY:
        warp_affine_loop<uint8_t>(src_img, dst_img, interpolate_type, M_inv, caps, norm_lut);
        break;
    case DL_IMAGE_PIX_TYPE_RGB888_QINT8:
    case DL_IMAGE_PIX_TYPE_GRAY_QINT8:
        warp_affine_loop<int8_t>(src_img, dst_img, interpolate_type, M_inv, caps, norm_lut);
        break;
    case DL_IMAGE_PIX_TYPE_RGB888_QINT16:
    case DL_IMAGE_PIX_TYPE_GRAY_QINT16:
        warp_affine_loop<int16_t>(src_img, dst_img, interpolate_type, M_inv, caps, norm_lut);
        break;
    case DL_IMAGE_PIX_TYPE_RGB565:
        warp_affine_loop<uint16_t>(src_img, dst_img, interpolate_type, M_inv, caps, norm_lut);
        break;
    }
}
} // namespace image
} // namespace dl
