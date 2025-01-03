#pragma once
#include "dl_image_define.hpp"
#include "dl_tool.hpp"
#include "esp_log.h"
#include <algorithm>
#include <cstring>
#include <vector>

namespace dl {
namespace image {
inline void convert_pixel_from_rgb565_to_rgb888(uint16_t *src_ptr, uint8_t *dst_ptr, uint32_t caps)
{
    if (caps & DL_IMAGE_CAP_RGB565_BIG_ENDIAN) {
        if (caps & DL_IMAGE_CAP_RGB_SWAP) {
            dst_ptr[2] = DL_IMAGE_BIG_ENDIAN_RGB565_BIT1(*src_ptr);
            dst_ptr[1] = DL_IMAGE_BIG_ENDIAN_RGB565_BIT2(*src_ptr);
            dst_ptr[0] = DL_IMAGE_BIG_ENDIAN_RGB565_BIT3(*src_ptr);
        } else {
            dst_ptr[0] = DL_IMAGE_BIG_ENDIAN_RGB565_BIT1(*src_ptr);
            dst_ptr[1] = DL_IMAGE_BIG_ENDIAN_RGB565_BIT2(*src_ptr);
            dst_ptr[2] = DL_IMAGE_BIG_ENDIAN_RGB565_BIT3(*src_ptr);
        }
    } else {
        if (caps & DL_IMAGE_CAP_RGB_SWAP) {
            dst_ptr[2] = DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT1(*src_ptr);
            dst_ptr[1] = DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT2(*src_ptr);
            dst_ptr[0] = DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT3(*src_ptr);
        } else {
            dst_ptr[0] = DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT1(*src_ptr);
            dst_ptr[1] = DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT2(*src_ptr);
            dst_ptr[2] = DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT3(*src_ptr);
        }
    }
}
template <typename T>
inline void convert_pixel_from_rgb565_to_rgb888_quant(uint16_t *src_ptr, T *dst_ptr, uint32_t caps, T *norm_lut)
{
    assert(norm_lut);
    if (caps & DL_IMAGE_CAP_RGB565_BIG_ENDIAN) {
        if (caps & DL_IMAGE_CAP_RGB_SWAP) {
            dst_ptr[2] = norm_lut[DL_IMAGE_BIG_ENDIAN_RGB565_BIT1(*src_ptr)];
            dst_ptr[1] = (norm_lut + 256)[DL_IMAGE_BIG_ENDIAN_RGB565_BIT2(*src_ptr)];
            dst_ptr[0] = (norm_lut + 512)[DL_IMAGE_BIG_ENDIAN_RGB565_BIT3(*src_ptr)];
        } else {
            dst_ptr[0] = norm_lut[DL_IMAGE_BIG_ENDIAN_RGB565_BIT1(*src_ptr)];
            dst_ptr[1] = (norm_lut + 256)[DL_IMAGE_BIG_ENDIAN_RGB565_BIT2(*src_ptr)];
            dst_ptr[2] = (norm_lut + 512)[DL_IMAGE_BIG_ENDIAN_RGB565_BIT3(*src_ptr)];
        }
    } else {
        if (caps & DL_IMAGE_CAP_RGB_SWAP) {
            dst_ptr[2] = norm_lut[DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT1(*src_ptr)];
            dst_ptr[1] = (norm_lut + 256)[DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT2(*src_ptr)];
            dst_ptr[0] = (norm_lut + 512)[DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT3(*src_ptr)];
        } else {
            dst_ptr[0] = norm_lut[DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT1(*src_ptr)];
            dst_ptr[1] = (norm_lut + 256)[DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT2(*src_ptr)];
            dst_ptr[2] = (norm_lut + 512)[DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT3(*src_ptr)];
        }
    }
}
inline void convert_pixel_from_rgb888_to_rgb565(uint8_t *src_ptr, uint16_t *dst_ptr, uint32_t caps)
{
    if (caps & DL_IMAGE_CAP_RGB565_BIG_ENDIAN) {
        if (caps & DL_IMAGE_CAP_RGB_SWAP) {
            *dst_ptr = ((src_ptr[2] >> 3) << 11) | ((src_ptr[1] >> 2) << 5) | (src_ptr[0] >> 3);
        } else {
            *dst_ptr = ((src_ptr[0] >> 3) << 11) | ((src_ptr[1] >> 2) << 5) | (src_ptr[2] >> 3);
        }
    } else {
        if (caps & DL_IMAGE_CAP_RGB_SWAP) {
            *dst_ptr =
                ((src_ptr[1] & 0x1c) << 11) | ((src_ptr[0] >> 3) << 8) | ((src_ptr[2] >> 3) << 3) | (src_ptr[1] >> 5);
        } else {
            *dst_ptr =
                ((src_ptr[1] & 0x1c) << 11) | ((src_ptr[2] >> 3) << 8) | ((src_ptr[0] >> 3) << 3) | (src_ptr[1] >> 5);
        }
    }
}
template <typename T>
inline void convert_pixel_from_rgb888_to_rgb888_quant(uint8_t *src_ptr, T *dst_ptr, uint32_t caps, T *norm_lut)
{
    if (caps & DL_IMAGE_CAP_RGB_SWAP) {
        for (int i = 0; i < 3; i++) {
            dst_ptr[2 - i] = norm_lut[src_ptr[i]];
        }
    } else {
        for (int i = 0; i < 3; i++) {
            dst_ptr[i] = norm_lut[src_ptr[i]];
        }
    }
}
inline void convert_pixel_from_rgb888_to_gray(uint8_t *src_ptr, uint8_t *dst_ptr, uint32_t caps)
{
    if (caps & DL_IMAGE_CAP_RGB_SWAP) {
        uint8_t tmp = (src_ptr[0] * 38 + src_ptr[1] * 75 + src_ptr[2] * 15) >> 7;
        *dst_ptr = std::max(std::min(tmp, (uint8_t)255), (uint8_t)0);
    } else {
        uint8_t tmp = (src_ptr[2] * 38 + src_ptr[1] * 75 + src_ptr[0] * 15) >> 7;
        *dst_ptr = std::max(std::min(tmp, (uint8_t)255), (uint8_t)0);
    }
}
template <typename T>
inline void convert_pixel_from_rgb888_to_gray_quant(uint8_t *src_ptr, T *dst_ptr, uint32_t caps, T *norm_lut)
{
    assert(norm_lut);
    if (caps & DL_IMAGE_CAP_RGB_SWAP) {
        T tmp = norm_lut[(src_ptr[0] * 38 + src_ptr[1] * 75 + src_ptr[2] * 15) >> 7];
        *dst_ptr = std::max(std::min(tmp, (T)255), (T)0);
    } else {
        T tmp = norm_lut[(src_ptr[2] * 38 + src_ptr[1] * 75 + src_ptr[1] * 15) >> 7];
        *dst_ptr = std::max(std::min(tmp, (T)255), (T)0);
    }
}
inline void convert_pixel_from_rgb565_to_gray(uint16_t *src_ptr, uint8_t *dst_ptr, uint32_t caps)
{
    uint8_t tmp[3];
    convert_pixel_from_rgb565_to_rgb888(src_ptr, tmp, caps);
    convert_pixel_from_rgb888_to_gray(tmp, dst_ptr, 0);
}
template <typename T>
inline void convert_pixel_from_rgb565_to_gray_quant(uint16_t *src_ptr, T *dst_ptr, uint32_t caps, T *norm_lut)
{
    uint8_t tmp[3];
    convert_pixel_from_rgb565_to_rgb888(src_ptr, tmp, caps);
    convert_pixel_from_rgb888_to_gray_quant<T>(tmp, dst_ptr, 0, norm_lut);
}
template <typename T>
inline void convert_pixel_from_gray_to_gray_quant(uint8_t *src_ptr, T *dst_ptr, T *norm_lut)
{
    *dst_ptr = norm_lut[*src_ptr];
}
inline void convert_pixel_from_rgb888_to_rgb888(uint8_t *src_ptr, uint8_t *dst_ptr, uint32_t caps)
{
    if (caps & DL_IMAGE_CAP_RGB_SWAP) {
        if (src_ptr == dst_ptr) {
            std::swap(src_ptr[0], src_ptr[2]);
        } else {
            for (int i = 0; i < 3; i++) {
                dst_ptr[2 - i] = src_ptr[i];
            }
        }
    } else {
        if (src_ptr == dst_ptr)
            return;
        for (int i = 0; i < 3; i++) {
            dst_ptr[i] = src_ptr[i];
        }
    }
}
inline void convert_pixel_from_rgb565_to_rgb565(uint16_t *src_ptr, uint16_t *dst_ptr, uint32_t caps)
{
    if (caps & DL_IMAGE_CAP_RGB565_BYTE_SWAP) {
        if (caps & DL_IMAGE_CAP_RGB_SWAP) {
            if (caps & DL_IMAGE_CAP_RGB565_BIG_ENDIAN) {
                // from rrrrrggg gggbbbbb
                // to   gggrrrrr bbbbbggg
                *dst_ptr = (uint16_t)(((*src_ptr & 0xf800) >> 3) | ((*src_ptr & 0x700) >> 8) |
                                      ((*src_ptr & 0xe0) << 8) | ((*src_ptr & 0x1f) << 3));
            } else {
                // from gggbbbbb rrrrrggg
                // to   bbbbbggg gggrrrrr
                *dst_ptr = (uint16_t)(((*src_ptr & 0xf8) >> 3) | ((*src_ptr & 0xe000) >> 8) | ((*src_ptr & 0x7) << 8) |
                                      ((*src_ptr & 0x1f00) << 3));
            }
        } else {
            *dst_ptr = (uint16_t)((*src_ptr << 8) | (*src_ptr >> 8));
        }
    } else {
        if (caps & DL_IMAGE_CAP_RGB_SWAP) {
            if (caps & DL_IMAGE_CAP_RGB565_BIG_ENDIAN) {
                *dst_ptr = (uint16_t)(((*src_ptr & 0xf800) >> 11) | (*src_ptr & 0x7e0) | ((*src_ptr & 0x1f) << 11));
            } else {
                *dst_ptr = (uint16_t)(((*src_ptr & 0xf8) << 5) | (*src_ptr & 0xe007) | ((*src_ptr & 0x1f00) >> 5));
            }
        } else {
            if (src_ptr == dst_ptr)
                return;
            *dst_ptr = *src_ptr;
        }
    }
}
inline void rgb565_pixel_byte_swap(uint8_t *ptr)
{
    *ptr = (uint16_t)((*ptr << 8) | (*ptr >> 8));
}
inline void rgb565_pixel_byte_swap(uint8_t *src_ptr, uint8_t *dst_ptr)
{
    *dst_ptr = (uint16_t)((*src_ptr << 8) | (*src_ptr >> 8));
}
inline void convert_pixel(const pix_t &src_pix, pix_t &dst_pix, uint32_t caps, void *norm_lut = nullptr)
{
    if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB565 && dst_pix.type == DL_IMAGE_PIX_TYPE_RGB565) {
        convert_pixel_from_rgb565_to_rgb565((uint16_t *)src_pix.data, (uint16_t *)dst_pix.data, caps);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB565 && dst_pix.type == DL_IMAGE_PIX_TYPE_RGB888) {
        convert_pixel_from_rgb565_to_rgb888((uint16_t *)src_pix.data, (uint8_t *)dst_pix.data, caps);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB565 && dst_pix.type == DL_IMAGE_PIX_TYPE_RGB888_QINT8) {
        convert_pixel_from_rgb565_to_rgb888_quant<int8_t>(
            (uint16_t *)src_pix.data, (int8_t *)dst_pix.data, caps, (int8_t *)norm_lut);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB565 && dst_pix.type == DL_IMAGE_PIX_TYPE_RGB888_QINT16) {
        convert_pixel_from_rgb565_to_rgb888_quant<int16_t>(
            (uint16_t *)src_pix.data, (int16_t *)dst_pix.data, caps, (int16_t *)norm_lut);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB565 && dst_pix.type == DL_IMAGE_PIX_TYPE_GRAY) {
        convert_pixel_from_rgb565_to_gray((uint16_t *)src_pix.data, (uint8_t *)dst_pix.data, caps);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB565 && dst_pix.type == DL_IMAGE_PIX_TYPE_GRAY_QINT8) {
        convert_pixel_from_rgb565_to_gray_quant<int8_t>(
            (uint16_t *)src_pix.data, (int8_t *)dst_pix.data, caps, (int8_t *)norm_lut);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB565 && dst_pix.type == DL_IMAGE_PIX_TYPE_GRAY_QINT16) {
        convert_pixel_from_rgb565_to_gray_quant<int16_t>(
            (uint16_t *)src_pix.data, (int16_t *)dst_pix.data, caps, (int16_t *)norm_lut);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB888 && dst_pix.type == DL_IMAGE_PIX_TYPE_RGB888) {
        convert_pixel_from_rgb888_to_rgb888((uint8_t *)src_pix.data, (uint8_t *)dst_pix.data, caps);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB888 && dst_pix.type == DL_IMAGE_PIX_TYPE_RGB565) {
        convert_pixel_from_rgb888_to_rgb565((uint8_t *)src_pix.data, (uint16_t *)dst_pix.data, caps);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB888 && dst_pix.type == DL_IMAGE_PIX_TYPE_GRAY) {
        convert_pixel_from_rgb888_to_gray((uint8_t *)src_pix.data, (uint8_t *)dst_pix.data, caps);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB888 && dst_pix.type == DL_IMAGE_PIX_TYPE_GRAY_QINT8) {
        convert_pixel_from_rgb888_to_gray_quant<int8_t>(
            (uint8_t *)src_pix.data, (int8_t *)dst_pix.data, caps, (int8_t *)norm_lut);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB888 && dst_pix.type == DL_IMAGE_PIX_TYPE_GRAY_QINT16) {
        convert_pixel_from_rgb888_to_gray_quant<int16_t>(
            (uint8_t *)src_pix.data, (int16_t *)dst_pix.data, caps, (int16_t *)norm_lut);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB888 && dst_pix.type == DL_IMAGE_PIX_TYPE_RGB888_QINT8) {
        convert_pixel_from_rgb888_to_rgb888_quant<int8_t>(
            (uint8_t *)src_pix.data, (int8_t *)dst_pix.data, caps, (int8_t *)norm_lut);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_RGB888 && dst_pix.type == DL_IMAGE_PIX_TYPE_RGB888_QINT16) {
        convert_pixel_from_rgb888_to_rgb888_quant<int16_t>(
            (uint8_t *)src_pix.data, (int16_t *)dst_pix.data, caps, (int16_t *)norm_lut);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_GRAY && dst_pix.type == DL_IMAGE_PIX_TYPE_GRAY_QINT8) {
        convert_pixel_from_gray_to_gray_quant<int8_t>(
            (uint8_t *)src_pix.data, (int8_t *)dst_pix.data, (int8_t *)norm_lut);
    } else if (src_pix.type == DL_IMAGE_PIX_TYPE_GRAY && dst_pix.type == DL_IMAGE_PIX_TYPE_GRAY_QINT16) {
        convert_pixel_from_gray_to_gray_quant<int16_t>(
            (uint8_t *)src_pix.data, (int16_t *)dst_pix.data, (int16_t *)norm_lut);
    } else {
        ESP_LOGE("dl_image_color",
                 "pixel conversion between fmt %s and %s is not implemented yet.",
                 pix_type_to_str(src_pix.type).c_str(),
                 pix_type_to_str(dst_pix.type).c_str());
    }
}

template <typename T1, typename T2>
void convert_img_loop(
    const img_t &src_img, img_t &dst_img, uint32_t caps, void *norm_lut, const std::vector<int> &crop_area);
void convert_img(const img_t &src_img,
                 img_t &dst_img,
                 uint32_t caps = 0,
                 void *norm_lut = nullptr,
                 const std::vector<int> &crop_area = {});
#if CONFIG_IDF_TARGET_ESP32P4
esp_err_t convert_img_ppa(const img_t &src_img,
                          img_t &dst_img,
                          ppa_client_handle_t ppa_handle,
                          void *ppa_buffer,
                          size_t ppa_buffer_size,
                          uint32_t caps = 0,
                          void *norm_lut = nullptr,
                          const std::vector<int> &crop_area = {});
#endif
} // namespace image
} // namespace dl
