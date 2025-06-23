#pragma once
#include "who_cam.hpp"
#include "who_lcd.hpp"
#include "who_publisher.hpp"

namespace who {
namespace frame_cap {
class WhoFrameCap : public WhoPublisher {
public:
    WhoFrameCap(const std::string &name) : WhoFrameCap(nullptr, name) {}
    WhoFrameCap(who::cam::WhoCam *cam, const std::string &name) : WhoPublisher(name), m_cam(cam)
    {
        if (cam) {
            fill_cam_queue();
        }
    }
    void set_cam(who::cam::WhoCam *cam)
    {
        m_cam = cam;
        fill_cam_queue();
    }
    who::cam::WhoCam *get_cam() { return m_cam; }
    bool run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID) override;

protected:
    void set_new_frame_bits();
    who::cam::WhoCam *m_cam;

private:
    void task() override;
    void fill_cam_queue();
    virtual void on_new_frame();
};

class WhoFrameCapLCD : public WhoFrameCap {
public:
    WhoFrameCapLCD(const std::string &name, bool display_back_frame = false) :
        WhoFrameCapLCD(nullptr, nullptr, name, display_back_frame)
    {
    }
    WhoFrameCapLCD(who::cam::WhoCam *cam,
                   who::lcd::WhoLCDiface *lcd,
                   const std::string &name,
                   bool display_back_frame = false) :
        WhoFrameCap(cam, name), m_lcd(lcd), m_display_back_frame(display_back_frame)
    {
    }
    void set_lcd(who::lcd::WhoLCDiface *lcd) { m_lcd = lcd; }
    who::lcd::WhoLCDiface *get_lcd() { return m_lcd; }
    bool run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID) override;

private:
    void run_lcd_display_cbs(who::cam::cam_fb_t *fb);
    void on_new_frame() override;
    who::lcd::WhoLCDiface *m_lcd;
    bool m_display_back_frame;
};
} // namespace frame_cap
} // namespace who
