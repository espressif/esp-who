#pragma once
#include "dl_image.hpp"
#include "linux/videodev2.h"

namespace who {
namespace cam {

typedef enum {
    VIDEO_PIX_FMT_RAW8_BGGR = V4L2_PIX_FMT_SBGGR8,
    VIDEO_PIX_FMT_RGB565 = V4L2_PIX_FMT_RGB565,
    VIDEO_PIX_FMT_RGB888 = V4L2_PIX_FMT_RGB24,
    VIDEO_PIX_FMT_YUV420 = V4L2_PIX_FMT_YUV420,
    VIDEO_PIX_FMT_YUV422P = V4L2_PIX_FMT_YUV422P,
} video_pix_fmt_t;

typedef struct {
    void *buf;
    size_t len;
    int width;
    int height;
    video_pix_fmt_t format;
    struct timeval timestamp;
} video_fb_t;

using cam_fb_t = video_fb_t;

inline dl::image::img_t fb2img(const who::cam::cam_fb_t *fb)
{
    assert(fb->format == who::cam::VIDEO_PIX_FMT_RGB565 || fb->format == who::cam::VIDEO_PIX_FMT_RGB888);
    return {.data = fb->buf,
            .width = fb->width,
            .height = fb->height,
            .pix_type = (fb->format == who::cam::VIDEO_PIX_FMT_RGB565) ? dl::image::DL_IMAGE_PIX_TYPE_RGB565
                                                                       : dl::image::DL_IMAGE_PIX_TYPE_RGB888};
}

} // namespace cam
} // namespace who
