#include "dl_image_draw.hpp"

namespace dl {
namespace image {
template <typename T>
void draw_point(const img_t &img, int x, int y, uint8_t radius, const pix_t &pix)
{
    T *img_ptr = (T *)img.data;
    T *pix_ptr = (T *)pix.data;

    int step = DL_IMAGE_IS_PIX_TYPE_RGB888(pix.type) ? 3 : 1;
    int radius_pow = radius * radius;

    for (int i = -radius; i < radius + 1; i++) {
        int y_ = DL_CLIP(y + i, 0, img.height - 1);
        int i_pow = i * i;
        for (int j = -radius; j < radius + 1; j++) {
            int x_ = DL_CLIP(x + j, 0, img.width - 1);
            int j_pow = j * j;
            if (i_pow + j_pow <= radius_pow) {
                T *ptr = img_ptr + y_ * img.width * step + x_ * step;
                for (int s = 0; s < step; s++) {
                    *ptr++ = pix_ptr[s];
                }
            }
        }
    }
}

template void draw_point<uint8_t>(const img_t &img, int x, int y, uint8_t radius, const pix_t &pix);
template void draw_point<uint16_t>(const img_t &img, int x, int y, uint8_t radius, const pix_t &pix);

void draw_point(const img_t &img, int x, int y, const std::vector<uint8_t> &color, uint8_t radius, uint32_t caps)
{
    assert(x >= 0 && y >= 0 && x < img.width && y < img.height);

    if (DL_IMAGE_IS_PIX_TYPE_QUANT(img.pix_type)) {
        ESP_LOGE("dl_image_draw", "Can not draw on a quant img.");
        return;
    }

    if (img.pix_type == DL_IMAGE_PIX_TYPE_RGB888) {
        assert(color.size() == 3);
        pix_t pix = {.data = (void *)color.data(), .type = DL_IMAGE_PIX_TYPE_RGB888};
        draw_point<uint8_t>(img, x, y, radius, pix);
    } else if (img.pix_type == DL_IMAGE_PIX_TYPE_RGB565) {
        assert(color.size() == 3);
        pix_t rgb888_pix = {.data = (void *)color.data(), .type = DL_IMAGE_PIX_TYPE_RGB888};
        uint16_t rgb565_pix_data;
        pix_t rgb565_pix = {.data = (void *)&rgb565_pix_data, .type = DL_IMAGE_PIX_TYPE_RGB565};
        convert_pixel(rgb888_pix, rgb565_pix, caps);
        draw_point<uint16_t>(img, x, y, radius, rgb565_pix);
    } else if (img.pix_type == DL_IMAGE_PIX_TYPE_GRAY) {
        assert(color.size() == 1);
        pix_t pix = {.data = (void *)color.data(), .type = DL_IMAGE_PIX_TYPE_GRAY};
        draw_point<uint8_t>(img, x, y, radius, pix);
    }
}

template <typename T>
void draw_hollow_rectangle(const img_t &img, int x1, int y1, int x2, int y2, uint8_t line_width, const pix_t &pix)
{
    T *img_ptr = (T *)img.data;
    T *pix_ptr = (T *)pix.data;

    int step = DL_IMAGE_IS_PIX_TYPE_RGB888(pix.type) ? 3 : 1;

    // draw horizon
    for (int i = -line_width / 2; i < line_width - line_width / 2; i++) {
        int y1_ = DL_CLIP(y1 + i, 0, img.height - 1);
        int y2_ = DL_CLIP(y2 + i, 0, img.height - 1);
        T *row_up = img_ptr + y1_ * img.width * step + x1 * step;
        T *row_down = img_ptr + y2_ * img.width * step + x1 * step;
        for (int x = x1; x <= x2; x++) {
            for (int s = 0; s < step; s++) {
                *row_up++ = pix_ptr[s];
                *row_down++ = pix_ptr[s];
            }
        }
    }

    // draw vertical
    for (int i = -line_width / 2; i < line_width - line_width / 2; i++) {
        int x1_ = DL_CLIP(x1 + i, 0, img.width - 1);
        int x2_ = DL_CLIP(x2 + i, 0, img.width - 1);
        T *colum_left = img_ptr + y1 * img.width * step + x1_ * step;
        T *colum_right = img_ptr + y1 * img.width * step + x2_ * step;
        for (int y = y1; y <= y2; y++) {
            for (int s = 0; s < step; s++) {
                *colum_left++ = pix_ptr[s];
                *colum_right++ = pix_ptr[s];
            }
            colum_left += (img.width - 1) * step;
            colum_right += (img.width - 1) * step;
        }
    }
}

template void draw_hollow_rectangle<uint8_t>(
    const img_t &img, int x1, int y1, int x2, int y2, uint8_t line_width, const pix_t &pix);
template void draw_hollow_rectangle<uint16_t>(
    const img_t &img, int x1, int y1, int x2, int y2, uint8_t line_width, const pix_t &pix);

void draw_hollow_rectangle(const img_t &img,
                           int x1,
                           int y1,
                           int x2,
                           int y2,
                           const std::vector<uint8_t> &color,
                           uint8_t line_width,
                           uint32_t caps)
{
    assert(x2 > x1 && y2 > y1);
    assert(x1 >= 0 && y1 >= 0);
    assert(x2 < img.width && y2 < img.height);

    if (DL_IMAGE_IS_PIX_TYPE_QUANT(img.pix_type)) {
        ESP_LOGE("dl_image_draw", "Can not draw on a quant img.");
        return;
    }

    if (img.pix_type == DL_IMAGE_PIX_TYPE_RGB888) {
        assert(color.size() == 3);
        pix_t pix = {.data = (void *)color.data(), .type = DL_IMAGE_PIX_TYPE_RGB888};
        draw_hollow_rectangle<uint8_t>(img, x1, y1, x2, y2, line_width, pix);
    } else if (img.pix_type == DL_IMAGE_PIX_TYPE_RGB565) {
        assert(color.size() == 3);
        pix_t rgb888_pix = {.data = (void *)color.data(), .type = DL_IMAGE_PIX_TYPE_RGB888};
        uint16_t rgb565_pix_data;
        pix_t rgb565_pix = {.data = (void *)&rgb565_pix_data, .type = DL_IMAGE_PIX_TYPE_RGB565};
        convert_pixel(rgb888_pix, rgb565_pix, caps);
        draw_hollow_rectangle<uint16_t>(img, x1, y1, x2, y2, line_width, rgb565_pix);
    } else if (img.pix_type == DL_IMAGE_PIX_TYPE_GRAY) {
        assert(color.size() == 1);
        pix_t pix = {.data = (void *)color.data(), .type = DL_IMAGE_PIX_TYPE_GRAY};
        draw_hollow_rectangle<uint8_t>(img, x1, y1, x2, y2, line_width, pix);
    }
}
} // namespace image
} // namespace dl
