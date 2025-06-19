#pragma once
#include "dl_image.hpp"
#include "usb/uvc_host.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "esp_camera.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "linux/videodev2.h"
#endif
#include "bsp/esp-bsp.h"

namespace who {
namespace cam {
enum class cam_fb_fmt_t { CAM_FB_FMT_RGB565, CAM_FB_FMT_RGB888, CAM_FB_FMT_JPEG, CAM_FB_FMT_UKN };

#if CONFIG_IDF_TARGET_ESP32S3
inline framesize_t get_cam_frame_size_from_lcd_resolution()
{
    for (int i = FRAMESIZE_INVALID - 1; i >= 0; i--) {
        if (resolution[i].width <= BSP_LCD_H_RES && resolution[i].height <= BSP_LCD_V_RES) {
            return (framesize_t)i;
        }
    }
    return FRAMESIZE_INVALID;
}

inline cam_fb_fmt_t pix_fmt2cam_fb_fmt(pixformat_t pix_fmt)
{
    switch (pix_fmt) {
    case PIXFORMAT_RGB565:
        return cam_fb_fmt_t::CAM_FB_FMT_RGB565;
    case PIXFORMAT_RGB888:
        return cam_fb_fmt_t::CAM_FB_FMT_RGB888;
    case PIXFORMAT_JPEG:
        return cam_fb_fmt_t::CAM_FB_FMT_JPEG;
    default:
        return cam_fb_fmt_t::CAM_FB_FMT_UKN;
    }
}
#endif

inline cam_fb_fmt_t dl_pix_fmt2cam_fb_fmt(dl::image::pix_type_t dl_pix_fmt)
{
    switch (dl_pix_fmt) {
    case dl::image::DL_IMAGE_PIX_TYPE_RGB565:
        return cam_fb_fmt_t::CAM_FB_FMT_RGB565;
    case dl::image::DL_IMAGE_PIX_TYPE_RGB888:
        return cam_fb_fmt_t::CAM_FB_FMT_RGB888;
    default:
        return cam_fb_fmt_t::CAM_FB_FMT_UKN;
    }
}

inline cam_fb_fmt_t uvc_fmt2cam_fb_fmt(uvc_host_stream_format uvc_fmt)
{
    switch (uvc_fmt) {
    case UVC_VS_FORMAT_MJPEG:
        return cam_fb_fmt_t::CAM_FB_FMT_JPEG;
    default:
        return cam_fb_fmt_t::CAM_FB_FMT_UKN;
    }
}

#if CONFIG_IDF_TARGET_ESP32P4
inline cam_fb_fmt_t v4l2_fmt2cam_fb_fmt(uint32_t v4l2_fmt)
{
    switch (v4l2_fmt) {
    case V4L2_PIX_FMT_RGB565:
        return cam_fb_fmt_t::CAM_FB_FMT_RGB565;
    case V4L2_PIX_FMT_RGB24:
        return cam_fb_fmt_t::CAM_FB_FMT_RGB888;
    case V4L2_PIX_FMT_MJPEG:
    case V4L2_PIX_FMT_JPEG:
        return cam_fb_fmt_t::CAM_FB_FMT_JPEG;
    default:
        return cam_fb_fmt_t::CAM_FB_FMT_UKN;
    }
}
#endif

typedef struct cam_fb_s {
    void *buf;
    size_t len;
    uint16_t width;
    uint16_t height;
    cam_fb_fmt_t format;
    struct timeval timestamp;
    void *ret;
    cam_fb_s() = default;
#if CONFIG_IDF_TARGET_ESP32S3
    cam_fb_s(const camera_fb_t &fb)
    {
        buf = (void *)fb.buf;
        len = fb.len;
        width = (uint16_t)fb.width;
        height = (uint16_t)fb.height;
        format = pix_fmt2cam_fb_fmt(fb.format);
        timestamp = fb.timestamp;
        ret = (void *)(&fb);
    }
#endif
    cam_fb_s(const uvc_host_frame_t &fb, int64_t cur_time)
    {
        buf = (void *)fb.data;
        len = fb.data_len;
        width = (uint16_t)fb.vs_format.h_res;
        height = (uint16_t)fb.vs_format.v_res;
        format = uvc_fmt2cam_fb_fmt(fb.vs_format.format);
        timestamp.tv_sec = cur_time / 1000000;
        timestamp.tv_usec = cur_time % 1000000;
        ret = (void *)(&fb);
    }
    cam_fb_s(const dl::image::img_t &img, const struct timeval &time)
    {
        buf = img.data;
        len = dl::image::get_img_byte_size(img);
        width = img.width;
        height = img.height;
        format = dl_pix_fmt2cam_fb_fmt(img.pix_type);
        timestamp = time;
        ret = nullptr;
    }
    operator dl::image::img_t() const
    {
        return {.data = buf,
                .width = width,
                .height = height,
                .pix_type = format == who::cam::cam_fb_fmt_t::CAM_FB_FMT_RGB565 ? dl::image::DL_IMAGE_PIX_TYPE_RGB565
                                                                                : dl::image::DL_IMAGE_PIX_TYPE_RGB888};
    }
} cam_fb_t;

} // namespace cam
} // namespace who
