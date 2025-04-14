#pragma once
#include "dl_image.hpp"
#include "esp_camera.h"

namespace who {
namespace cam {

using cam_fb_t = camera_fb_t;

inline dl::image::img_t fb2img(const who::cam::cam_fb_t *fb)
{
    assert(fb->format == PIXFORMAT_RGB565 || fb->format == PIXFORMAT_RGB888);
    return {.data = fb->buf,
            .width = (int)fb->width,
            .height = (int)fb->height,
            .pix_type = (fb->format == PIXFORMAT_RGB565) ? dl::image::DL_IMAGE_PIX_TYPE_RGB565
                                                         : dl::image::DL_IMAGE_PIX_TYPE_RGB888};
}

} // namespace cam
} // namespace who
