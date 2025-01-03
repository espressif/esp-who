#pragma once
#include "sdkconfig.h"
#include <cstddef>
#include <cstdint>
#include <string>
#if CONFIG_IDF_TARGET_ESP32P4
#include "driver/ppa.h"
#endif

#define DL_IMAGE_CAP_RGB_SWAP (1 << 0)
#define DL_IMAGE_CAP_RGB565_BYTE_SWAP (1 << 1)
#define DL_IMAGE_CAP_RGB565_BIG_ENDIAN (1 << 2)
#define DL_IMAGE_CAP_PPA (1 << 3)

// gggbbbbb rrrrrggg
#define DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT1(x) ((uint8_t)((x) & 0xF8))
#define DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT2(x) ((uint8_t)((((x) & 0x7) << 5) | (((x) & 0xE000) >> 11)))
#define DL_IMAGE_LITTLE_ENDIAN_RGB565_BIT3(x) ((uint8_t)(((x) & 0x1F00) >> 5))

// rrrrrggg gggbbbbb
#define DL_IMAGE_BIG_ENDIAN_RGB565_BIT1(x) ((uint8_t)(((x) & 0xF800) >> 8))
#define DL_IMAGE_BIG_ENDIAN_RGB565_BIT2(x) ((uint8_t)(((x) & 0x7E0) >> 3))
#define DL_IMAGE_BIG_ENDIAN_RGB565_BIT3(x) ((uint8_t)(((x) & 0x1F) << 3))

#define DL_IMAGE_IS_PIX_TYPE_QUANT(x) \
    ((x) != DL_IMAGE_PIX_TYPE_RGB888 && (x) != DL_IMAGE_PIX_TYPE_RGB565 && (x) != DL_IMAGE_PIX_TYPE_GRAY)
#define DL_IMAGE_IS_PIX_TYPE_RGB888(x) \
    ((x) == DL_IMAGE_PIX_TYPE_RGB888 || (x) == DL_IMAGE_PIX_TYPE_RGB888_QINT8 || (x) == DL_IMAGE_PIX_TYPE_RGB888_QINT16)

#define DL_IMAGE_ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

namespace dl {
namespace image {

typedef enum {
    DL_IMAGE_PIX_TYPE_RGB888 = 0,
    DL_IMAGE_PIX_TYPE_RGB888_QINT8,
    DL_IMAGE_PIX_TYPE_RGB888_QINT16,
    DL_IMAGE_PIX_TYPE_GRAY,
    DL_IMAGE_PIX_TYPE_GRAY_QINT8,
    DL_IMAGE_PIX_TYPE_GRAY_QINT16,
    DL_IMAGE_PIX_TYPE_RGB565
} pix_type_t;

inline std::string pix_type_to_str(pix_type_t type)
{
    switch (type) {
    case DL_IMAGE_PIX_TYPE_RGB888:
        return "DL_IMAGE_PIX_TYPE_RGB888";
    case DL_IMAGE_PIX_TYPE_RGB888_QINT8:
        return "DL_IMAGE_PIX_TYPE_RGB888_QINT8";
    case DL_IMAGE_PIX_TYPE_RGB888_QINT16:
        return "DL_IMAGE_PIX_TYPE_RGB888_QINT16";
    case DL_IMAGE_PIX_TYPE_GRAY:
        return "DL_IMAGE_PIX_TYPE_GRAY";
    case DL_IMAGE_PIX_TYPE_GRAY_QINT8:
        return "DL_IMAGE_PIX_TYPE_GRAY_QINT8";
    case DL_IMAGE_PIX_TYPE_GRAY_QINT16:
        return "DL_IMAGE_PIX_TYPE_GRAY_QINT16";
    case DL_IMAGE_PIX_TYPE_RGB565:
        return "DL_IMAGE_PIX_TYPE_RGB565";
    default:
        return "UNK_PIX_TYPE";
    }
}

typedef struct {
    void *data;
    pix_type_t type;
} pix_t;

typedef struct {
    void *data;
    int width;
    int height;
    pix_type_t pix_type;
} img_t;

inline size_t get_img_byte_size(const img_t &img)
{
    switch (img.pix_type) {
    case DL_IMAGE_PIX_TYPE_RGB888:
    case DL_IMAGE_PIX_TYPE_RGB888_QINT8:
        return img.height * img.width * 3;
    case DL_IMAGE_PIX_TYPE_RGB565:
    case DL_IMAGE_PIX_TYPE_GRAY_QINT16:
        return img.height * img.width * 2;
    case DL_IMAGE_PIX_TYPE_GRAY:
    case DL_IMAGE_PIX_TYPE_GRAY_QINT8:
        return img.height * img.width;
    case DL_IMAGE_PIX_TYPE_RGB888_QINT16:
        return img.height * img.width * 6;
    default:
        return 0;
    }
}

inline int get_img_channel(const img_t &img)
{
    switch (img.pix_type) {
    case DL_IMAGE_PIX_TYPE_RGB888:
    case DL_IMAGE_PIX_TYPE_RGB888_QINT8:
    case DL_IMAGE_PIX_TYPE_RGB888_QINT16:
    case DL_IMAGE_PIX_TYPE_RGB565:
        return 3;
    case DL_IMAGE_PIX_TYPE_GRAY:
    case DL_IMAGE_PIX_TYPE_GRAY_QINT8:
    case DL_IMAGE_PIX_TYPE_GRAY_QINT16:
        return 1;
    default:
        return -1;
    }
}

typedef struct {
    uint8_t *data;
    int width;
    int height;
    uint32_t data_size;
} jpeg_img_t;

typedef enum {
    DL_IMAGE_INTERPOLATE_BILINEAR, /*<! interpolate by taking bilinear of four pixels */
    DL_IMAGE_INTERPOLATE_NEAREST,  /*<! interpolate by taking the nearest pixel */
} interpolate_type_t;

#if CONFIG_IDF_TARGET_ESP32P4
inline esp_err_t convert_pix_type_to_ppa_srm_fmt(pix_type_t type, ppa_srm_color_mode_t *srm_fmt)
{
    if (type == DL_IMAGE_PIX_TYPE_RGB888) {
        *srm_fmt = PPA_SRM_COLOR_MODE_RGB888;
    } else if (type == DL_IMAGE_PIX_TYPE_RGB565) {
        *srm_fmt = PPA_SRM_COLOR_MODE_RGB565;
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
}
#endif

} // namespace image
} // namespace dl
