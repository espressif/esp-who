#pragma once
#include "sdkconfig.h"
#include "bsp/esp-bsp.h"
#if CONFIG_IDF_TARGET_ESP32P4
#include "linux/videodev2.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp_camera.h"
#endif

namespace who {
namespace cam {

#if CONFIG_IDF_TARGET_ESP32P4
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

#define CSI_CAMERA_DEFAULT_CONFIG                   \
    {                                               \
        {                                           \
            .sccb_config =                          \
                {                                   \
                    .init_sccb = false,             \
                    .i2c_config =                   \
                        {                           \
                            .port = BSP_I2C_NUM,    \
                            .scl_pin = BSP_I2C_SCL, \
                            .sda_pin = BSP_I2C_SDA, \
                        },                          \
                    .freq = 100000,                 \
                },                                  \
            .reset_pin = -1,                        \
            .pwdn_pin = -1,                         \
        },                                          \
    }

#elif CONFIG_IDF_TARGET_ESP32S3
using cam_fb_t = camera_fb_t;
#endif

} // namespace cam
} // namespace who
