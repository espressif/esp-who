
#pragma once
#include "cam_define.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <queue>
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
#endif
#include "dl_image.hpp"

namespace who {
namespace cam {

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
class ESPVideo : public Cam {
public:
    ESPVideo(const video_pix_fmt_t video_pix_fmt,
             const uint8_t fb_count,
             const v4l2_memory fb_mem_type,
             bool horizontal_flip);

protected:
    video_pix_fmt_t m_video_pix_fmt;
    v4l2_memory m_fb_mem_type;
    bool m_horizontal_flip;
    int m_width;
    int m_height;
    int m_fd;
    void video_init();
    void video_deinit();

private:
    esp_err_t set_horizontal_flip() override;
    esp_err_t open_video_device();
    esp_err_t print_info();
    esp_err_t close_video_device();
    esp_err_t set_video_format();
    virtual esp_err_t init_fbs() = 0;
    esp_err_t start_video_stream();
    esp_err_t stop_video_stream();
};

class P4Cam : public ESPVideo {
public:
    P4Cam(const video_pix_fmt_t video_pix_fmt,
          const uint8_t fb_count,
          const v4l2_memory fb_mem_type,
          bool horizontal_flip);
    ~P4Cam();
    cam_fb_t *cam_fb_get() override;
    cam_fb_t *cam_fb_peek(bool back = true) override;
    void cam_fb_return() override;

private:
    SemaphoreHandle_t m_mutex;
    cam_fb_t *m_cam_fbs;
    std::queue<struct v4l2_buffer> m_v4l2_buf_queue;
    std::queue<cam_fb_t *> m_buf_queue;

    esp_err_t init_fbs() override;
};

class PPAP4Cam : public ESPVideo {
public:
    typedef struct {
        cam_fb_t *fb;
        cam_fb_t *ppa_fb;
    } ppa_cam_fb_t;
    PPAP4Cam(const video_pix_fmt_t video_pix_fmt,
             const uint8_t fb_count,
             const v4l2_memory fb_mem_type,
             int ppa_resized_w,
             int ppa_resized_h,
             bool horizontal_flip);
    ~PPAP4Cam();
    cam_fb_t *cam_fb_get() override;
    cam_fb_t *cam_fb_peek(bool back = true) override;
    cam_fb_t *ppa_cam_fb_peek(bool back = true);
    void cam_fb_return() override;
    float m_ppa_scale_x;
    float m_ppa_scale_y;

private:
    esp_err_t init_fbs() override;
    static bool ppa_trans_done_cb(ppa_client_handle_t ppa_client, ppa_event_data_t *event_data, void *user_data);
    static void task(void *args);

    SemaphoreHandle_t m_mutex;
    SemaphoreHandle_t m_ppa_sem1;
    SemaphoreHandle_t m_ppa_sem2;
    cam_fb_t *m_cam_fbs;
    cam_fb_t *m_ppa_cam_fbs;
    int m_ppa_resized_w;
    int m_ppa_resized_h;
    ppa_client_handle_t m_ppa_srm_handle;
    std::queue<struct v4l2_buffer> m_v4l2_buf_queue;
    std::queue<ppa_cam_fb_t> m_buf_queue;
};

inline dl::image::img_t fb2img(const who::cam::cam_fb_t *fb)
{
    assert(fb->format == who::cam::VIDEO_PIX_FMT_RGB565 || fb->format == who::cam::VIDEO_PIX_FMT_RGB888);
    return {.data = fb->buf,
            .width = fb->width,
            .height = fb->height,
            .pix_type = (fb->format == who::cam::VIDEO_PIX_FMT_RGB565) ? dl::image::DL_IMAGE_PIX_TYPE_RGB565
                                                                       : dl::image::DL_IMAGE_PIX_TYPE_RGB888};
}

#elif CONFIG_IDF_TARGET_ESP32S3
class S3Cam : public Cam {
public:
    S3Cam(const pixformat_t pixel_format, const framesize_t frame_size, const uint8_t fb_count, bool horizontal_flip);
    ~S3Cam();
    cam_fb_t *cam_fb_get() override;
    cam_fb_t *cam_fb_peek(bool back = true) override;
    void cam_fb_return() override;

private:
    SemaphoreHandle_t m_mutex;
    esp_err_t set_horizontal_flip() override;
    std::queue<cam_fb_t *> m_buf_queue;
};

inline dl::image::img_t fb2img(const who::cam::cam_fb_t *fb)
{
    assert(fb->format == PIXFORMAT_RGB565 || fb->format == PIXFORMAT_RGB888);
    return {.data = fb->buf,
            .width = (int)fb->width,
            .height = (int)fb->height,
            .pix_type = (fb->format == PIXFORMAT_RGB565) ? dl::image::DL_IMAGE_PIX_TYPE_RGB565
                                                         : dl::image::DL_IMAGE_PIX_TYPE_RGB888};
}
#endif

} // namespace cam
} // namespace who
