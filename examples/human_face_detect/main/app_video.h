
#pragma once
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/errno.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "linux/videodev2.h"
#include "esp_video_init.h"
#include "bsp/esp-bsp.h"
#include "esp_cache.h"
#include "esp_private/esp_cache_private.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FB_COUNT 5

typedef enum 
{
    VIDEO_FMT_RAW8_BGGR = V4L2_PIX_FMT_SBGGR8,
    VIDEO_FMT_RGB565 = V4L2_PIX_FMT_RGB565,
    VIDEO_FMT_RGB888 = V4L2_PIX_FMT_RGB24,
    VIDEO_FMT_YUV420 = V4L2_PIX_FMT_YUV420,
    VIDEO_FMT_YUV422P = V4L2_PIX_FMT_YUV422P,
} video_format_t;

typedef struct
{
    void *buf;
    size_t len;
    struct timeval timestamp;
} video_fb_t;

#define CSI_CAMERA_DEFAULT_CONFIG           \
{                                           \
    {                                       \
        .sccb_config = {                    \
            .init_sccb = false,             \
            .i2c_config = {                 \
                .port      = BSP_I2C_NUM,   \
                .scl_pin   = BSP_I2C_SCL,   \
                .sda_pin   = BSP_I2C_SDA,   \
            },                              \
            .freq = 100000,                 \
        },                                  \
        .reset_pin = -1,                    \
        .pwdn_pin  = -1,                    \
    },                                      \
}

esp_err_t open_video_device();
esp_err_t close_video_device();
esp_err_t set_video_format(const video_format_t video_format);
esp_err_t set_horizontal_flip();
esp_err_t init_fb(const int fb_count);
esp_err_t start_video_stream();
esp_err_t stop_video_stream();
esp_err_t video_init(const video_format_t video_format, const int fb_count);
esp_err_t video_deinit(const int fb_count);
video_fb_t *video_fb_get();
void video_fb_return();

#ifdef __cplusplus
}
#endif