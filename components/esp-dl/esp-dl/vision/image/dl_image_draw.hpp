#pragma once
#include "dl_image_color.hpp"
#include "dl_image_define.hpp"

namespace dl {
namespace image {
template <typename T>
void draw_point(const img_t &img, int x, int y, uint8_t radius, const pix_t &pix);
void draw_point(const img_t &img, int x, int y, const std::vector<uint8_t> &color, uint8_t radius, uint32_t caps = 0);

template <typename T>
void draw_hollow_rectangle(const img_t &img, int x1, int y1, int x2, int y2, uint8_t line_width, const pix_t &pix);
void draw_hollow_rectangle(const img_t &img,
                           int x1,
                           int y1,
                           int x2,
                           int y2,
                           const std::vector<uint8_t> &color,
                           uint8_t line_width,
                           uint32_t caps = 0);

} // namespace image
} // namespace dl
