#pragma once
#include "who_cam_base.hpp"
#include <sys/ioctl.h>
#include <sys/mman.h>

namespace who {
namespace cam {

class ESPVideo : public WhoCam {
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

} // namespace cam
} // namespace who
