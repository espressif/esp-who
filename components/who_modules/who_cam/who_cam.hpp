
#pragma once
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sdkconfig.h"
#include <queue>
#include "bsp/esp-bsp.h"
#if CONFIG_IDF_TARGET_ESP32P4
#include "esp_cache.h"
#include "esp_video_init.h"
#include <fcntl.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/param.h>
#include "esp_private/esp_cache_private.h"
#include "linux/videodev2.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp_camera.h"
#endif
#include "dl_image.hpp"

namespace who {
namespace cam {

typedef enum {
    VIDEO_PIX_FMT_RAW8_BGGR = V4L2_PIX_FMT_SBGGR8,
    VIDEO_PIX_FMT_RGB565 = V4L2_PIX_FMT_RGB565,
    VIDEO_PIX_FMT_RGB888 = V4L2_PIX_FMT_RGB24,
    VIDEO_PIX_FMT_YUV420 = V4L2_PIX_FMT_YUV420,
    VIDEO_PIX_FMT_YUV422P = V4L2_PIX_FMT_YUV422P,
} video_pix_fmt_t;

#if CONFIG_IDF_TARGET_ESP32P4
typedef struct {
    void *buf;
    size_t len;
    int width;
    int height;
    video_pix_fmt_t format;
    struct timeval timestamp;
} video_fb_t;

using cam_fb_t = video_fb_t;

inline dl::image::img_t fb2img(const cam_fb_t *fb)
{
    assert(fb->format == VIDEO_PIX_FMT_RGB565 || fb->format == VIDEO_PIX_FMT_RGB888);
    return {.data = fb->buf,
            .width = fb->width,
            .height = fb->height,
            .pix_type = (fb->format == VIDEO_PIX_FMT_RGB565) ? dl::image::DL_IMAGE_PIX_TYPE_RGB565
                                                             : dl::image::DL_IMAGE_PIX_TYPE_RGB888};
}

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

class Cam {
public:
    Cam(uint8_t fb_count) : m_fb_count(fb_count) {};
    virtual ~Cam() {};
    virtual cam_fb_t *cam_fb_get() = 0;
    virtual cam_fb_t *cam_fb_peek(bool back = true) = 0;
    virtual void cam_fb_return() = 0;
    uint8_t m_fb_count;

private:
    virtual esp_err_t set_horizontal_flip() = 0;
};

#if CONFIG_IDF_TARGET_ESP32P4
class P4Cam : public Cam {
public:
    P4Cam(const video_pix_fmt_t video_pix_fmt,
          const uint8_t fb_count,
          const v4l2_memory fb_mem_type,
          bool horizontal_flip);
    ~P4Cam();
    cam_fb_t *cam_fb_get() override;
    cam_fb_t *cam_fb_peek(bool back = true) override;
    void cam_fb_return() override;
    static SemaphoreHandle_t s_mutex;

private:
    video_pix_fmt_t m_video_pix_fmt;
    v4l2_memory m_fb_mem_type;
    bool m_horizontal_flip;
    int m_fd;
    cam_fb_t *m_cam_fbs;
    std::queue<struct v4l2_buffer> m_buf_queue;
    int m_width;
    int m_height;

    esp_err_t set_horizontal_flip() override;
    esp_err_t video_init();
    esp_err_t video_deinit();
    esp_err_t open_video_device();
    esp_err_t print_info();
    esp_err_t close_video_device();
    esp_err_t set_video_format();
    esp_err_t init_fb();
    esp_err_t start_video_stream();
    esp_err_t stop_video_stream();
};

#elif CONFIG_IDF_TARGET_ESP32S3
using cam_fb_t = camera_fb_t;
class S3Cam : public Cam {};
#endif

class WhoCam {
public:
    WhoCam(Cam *cam) : m_cam(cam) {};
    void run();
    static TaskHandle_t s_task_handle;

private:
    static void task(void *args);
    Cam *m_cam;
};

} // namespace cam
} // namespace who
