#pragma once
#include "who_esp_video.hpp"
#include <deque>
#include <queue>

namespace who {
namespace cam {

class WhoP4Cam : public ESPVideo {
public:
    WhoP4Cam(const video_pix_fmt_t video_pix_fmt,
             const uint8_t fb_count,
             const v4l2_memory fb_mem_type,
             bool horizontal_flip);
    ~WhoP4Cam();
    cam_fb_t *cam_fb_get() override;
    cam_fb_t *cam_fb_peek(bool back = true) override;
    std::vector<cam_fb_t *> cam_fb_peek(bool back, int num) override;
    void cam_fb_return() override;
    std::string get_type() override { return "WhoP4Cam"; }

private:
    SemaphoreHandle_t m_mutex;
    cam_fb_t *m_cam_fbs;
    std::queue<struct v4l2_buffer> m_v4l2_buf_queue;
    std::deque<cam_fb_t *> m_buf_queue;

    esp_err_t init_fbs() override;
};

} // namespace cam
} // namespace who
