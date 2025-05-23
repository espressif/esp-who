#pragma once
#include "who_cam_base.hpp"
#include <deque>

namespace who {
namespace cam {

class WhoS3Cam : public WhoCam {
public:
    WhoS3Cam(const pixformat_t pixel_format,
             const framesize_t frame_size,
             const uint8_t fb_count,
             bool vertical_flip = false,
             bool horizontal_flip = true);
    ~WhoS3Cam();
    cam_fb_t *cam_fb_get() override;
    void cam_fb_return(cam_fb_t *fb) override;
    cam_fb_fmt_t get_fb_format() override { return pix_fmt2cam_fb_fmt(m_format); }

private:
    esp_err_t set_flip(bool vertical_flip, bool horizontal_flip);
    int get_cam_fb_index();
    pixformat_t m_format;
};

} // namespace cam
} // namespace who
