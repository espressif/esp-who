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
             bool horizontal_flip);
    ~WhoS3Cam();
    cam_fb_t *cam_fb_get() override;
    cam_fb_t *cam_fb_peek(bool back = true) override;
    std::vector<cam_fb_t *> cam_fb_peek(bool back, int num) override;
    void cam_fb_return() override;
    std::string get_type() override { return "WhoS3Cam"; }

private:
    SemaphoreHandle_t m_mutex;
    esp_err_t set_horizontal_flip() override;
    std::deque<cam_fb_t *> m_buf_queue;
};

} // namespace cam
} // namespace who
