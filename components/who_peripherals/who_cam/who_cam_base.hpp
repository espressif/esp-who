#pragma once
#include "who_cam_define.hpp"

namespace who {
namespace cam {

class WhoCam {
public:
    WhoCam(uint8_t fb_count) : m_fb_count(fb_count) {};
    virtual ~WhoCam() {};
    virtual cam_fb_t *cam_fb_get() = 0;
    virtual cam_fb_t *cam_fb_peek(bool back = true) = 0;
    virtual std::vector<cam_fb_t *> cam_fb_peek(bool back, int num) = 0;
    virtual void cam_fb_return() = 0;
    virtual std::string get_type() = 0;
    uint8_t m_fb_count;

private:
    virtual esp_err_t set_horizontal_flip() = 0;
};

} // namespace cam
} // namespace who
