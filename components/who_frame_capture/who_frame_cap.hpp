#pragma once
#include "who_cam.hpp"
#include "who_publisher.hpp"

namespace who {
namespace frame_cap {
class WhoFrameCap : public WhoPublisher {
public:
    WhoFrameCap(const std::string &name) : WhoFrameCap(nullptr, name) {}
    WhoFrameCap(who::cam::WhoCam *cam, const std::string &name) : WhoPublisher(name), m_cam(cam)
    {
        if (cam) {
#if CONFIG_IDF_TARGET_ESP32P4
            int n = m_cam->m_fb_count - 2;
#elif CONFIG_IDF_TARGET_ESP32S3
            int n = m_cam->m_fb_count - 1;
#endif
            for (int i = 0; i < n; i++) {
                m_cam->cam_fb_get();
            }
        }
    }
    void set_cam(who::cam::WhoCam *cam)
    {
        m_cam = cam;
#if CONFIG_IDF_TARGET_ESP32P4
        int n = m_cam->m_fb_count - 2;
#elif CONFIG_IDF_TARGET_ESP32S3
        int n = m_cam->m_fb_count - 1;
#endif
        for (int i = 0; i < n; i++) {
            m_cam->cam_fb_get();
        }
    }
    bool run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID) override;
    who::cam::WhoCam *m_cam;

protected:
    void set_new_frame_bits();

private:
    void task() override;
    virtual void on_new_frame();
};

class WhoFrameCapLCD : public WhoFrameCap {
public:
    WhoFrameCapLCD(const std::string &name, bool display_back_frame = false) :
        WhoFrameCapLCD(nullptr, name, display_back_frame)
    {
    }
    WhoFrameCapLCD(who::cam::WhoCam *cam, const std::string &name, bool display_back_frame = false) :
        WhoFrameCap(cam, name), m_display_back_frame(display_back_frame)
    {
    }

private:
    void run_lcd_display_cbs(who::cam::cam_fb_t *fb);
    void on_new_frame() override;
    bool m_display_back_frame;
};
} // namespace frame_cap
} // namespace who
