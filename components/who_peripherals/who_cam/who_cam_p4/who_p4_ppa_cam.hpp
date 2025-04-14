#pragma once
#include "who_esp_video.hpp"
#include <deque>
#include <queue>

namespace who {
namespace cam {

class WhoP4PPACam : public ESPVideo {
public:
    typedef struct {
        cam_fb_t *fb;
        cam_fb_t *ppa_fb;
    } ppa_cam_fb_t;
    WhoP4PPACam(const video_pix_fmt_t video_pix_fmt,
                const uint8_t fb_count,
                const v4l2_memory fb_mem_type,
                int ppa_resized_w,
                int ppa_resized_h,
                bool horizontal_flip);
    ~WhoP4PPACam();
    cam_fb_t *cam_fb_get() override;
    cam_fb_t *cam_fb_peek(bool back = true) override;
    std::vector<cam_fb_t *> cam_fb_peek(bool back, int num) override;
    cam_fb_t *ppa_cam_fb_peek(bool back = true);
    std::vector<cam_fb_t *> ppa_cam_fb_peek(bool back, int num);
    void cam_fb_return() override;
    std::string get_type() override { return "WhoP4PPACam"; }
    float m_ppa_scale_x;
    float m_ppa_scale_y;

private:
    esp_err_t init_fbs() override;
    static bool ppa_trans_done_cb(ppa_client_handle_t ppa_client, ppa_event_data_t *event_data, void *user_data);
    static void task(void *args);

    SemaphoreHandle_t m_mutex;
    SemaphoreHandle_t m_ppa_sem1;
    SemaphoreHandle_t m_ppa_sem2;
    SemaphoreHandle_t m_task_finish;
    volatile bool m_task_stop = false;
    TaskHandle_t m_task_handle;
    cam_fb_t *m_cam_fbs;
    cam_fb_t *m_ppa_cam_fbs;
    int m_ppa_resized_w;
    int m_ppa_resized_h;
    ppa_client_handle_t m_ppa_srm_handle;
    std::queue<struct v4l2_buffer> m_v4l2_buf_queue;
    std::deque<ppa_cam_fb_t> m_buf_queue;
};

} // namespace cam
} // namespace who
