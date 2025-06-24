#pragma once
#include "who_cam_base.hpp"
#include "who_task.hpp"
#include "who_uvc.hpp"
#include <deque>
#include <queue>

namespace who {
namespace cam {
class WhoUVCCam : public WhoCam {
public:
    WhoUVCCam(const uvc_host_stream_format fmt, uint16_t h_res, uint16_t v_res, float fps, const uint8_t fb_count);
    cam_fb_t *cam_fb_get() override;
    void cam_fb_return(cam_fb_t *fb) override;
    cam_fb_fmt_t get_fb_format() override { return uvc_fmt2cam_fb_fmt(m_format); }

private:
    int get_cam_fb_index();
    static void stream_cb(const uvc_host_stream_event_data_t *event, void *user_ctx);
    void stream_cb(const uvc_host_stream_event_data_t *event);
    static bool frame_cb(const uvc_host_frame_t *frame, void *user_ctx);
    bool frame_cb(const uvc_host_frame_t *frame);

    uvc_host_stream_config_t m_stream_config;
    uvc_host_stream_hdl_t m_stream;
    QueueHandle_t m_frame;
    uvc_host_stream_format m_format;
};
} // namespace cam
} // namespace who
