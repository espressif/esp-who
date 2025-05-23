#pragma once
#include "who_cam_base.hpp"
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <vector>

namespace who {
namespace cam {

class WhoP4Cam : public WhoCam {
public:
    WhoP4Cam(const uint32_t v4l2_fmt,
             const uint8_t fb_count,
             const v4l2_memory fb_mem_type = V4L2_MEMORY_USERPTR,
             bool vertical_flip = false,
             bool horizontal_flip = true);
    ~WhoP4Cam();
    cam_fb_t *cam_fb_get() override;
    void cam_fb_return(cam_fb_t *fb) override;
    cam_fb_fmt_t get_fb_format() override { return v4l2_fmt2cam_fb_fmt(m_v4l2_fmt); }

private:
    esp_err_t set_flip(bool vertical_flip, bool horizontal_flip);
    esp_err_t open_video_device();
    esp_err_t print_info();
    esp_err_t close_video_device();
    esp_err_t set_video_format();
    esp_err_t init_fbs();
    esp_err_t start_video_stream();
    esp_err_t stop_video_stream();
    void video_init(bool vertical_flip, bool horizontal_flip);
    void video_deinit();
    uint32_t m_v4l2_fmt;
    v4l2_memory m_fb_mem_type;
    int m_fd;
};

} // namespace cam
} // namespace who
