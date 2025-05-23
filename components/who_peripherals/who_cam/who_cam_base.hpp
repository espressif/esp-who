#pragma once
#include "who_cam_define.hpp"

namespace who {
namespace cam {
class WhoCam {
public:
    WhoCam(uint8_t fb_count) : WhoCam(fb_count, 0, 0) {}
    WhoCam(uint8_t fb_count, uint16_t fb_width, uint16_t fb_height) :
        m_fb_count(fb_count), m_cam_fbs(new cam_fb_t[fb_count]), m_fb_width(fb_width), m_fb_height(fb_height)
    {
    }
    virtual ~WhoCam() { delete[] m_cam_fbs; }
    virtual cam_fb_t *cam_fb_get() = 0;
    virtual void cam_fb_return(cam_fb_t *fb) = 0;
    uint16_t get_fb_width() { return m_fb_width; }
    uint16_t get_fb_height() { return m_fb_height; }
    uint8_t get_fb_count() { return m_fb_count; }
    virtual cam_fb_fmt_t get_fb_format() = 0;

protected:
    uint8_t m_fb_count;
    cam_fb_t *m_cam_fbs;
    uint16_t m_fb_width;
    uint16_t m_fb_height;
};

} // namespace cam
} // namespace who
