#pragma once
#include "dl_detect_base.hpp"
#include "who_cam_define.hpp"
#include "who_frame_cap.hpp"
#include "who_subscriber.hpp"
#include <list>

namespace who {
namespace detect {
class WhoDetectBase : public WhoSubscriber {
public:
    typedef struct {
        std::list<dl::detect::result_t> det_res;
        struct timeval timestamp;
        who::cam::cam_fb_t *fb;
    } result_t;

    WhoDetectBase(frame_cap::WhoFrameCap *frame_cap, dl::detect::Detect *detect, const std::string &name) :
        WhoSubscriber(name), m_frame_cap(frame_cap), m_detect(detect), m_interval(0)
    {
        frame_cap->add_element(this);
    }
    void set_model(dl::detect::Detect *model) { m_detect = model; }
    void set_fps(float fps)
    {
        if (fps > 0) {
            m_interval = pdMS_TO_TICKS((int)(1000.f / fps));
        }
    }
    bool run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID) override;
    bool stop() override;

protected:
    frame_cap::WhoFrameCap *m_frame_cap;

private:
    void detect_loop();
#if CONFIG_IDF_TARGET_ESP32P4
    void ppa_detect_loop();
#endif
    void task() override;
    virtual void on_new_detect_result(const result_t &result) = 0;
    dl::detect::Detect *m_detect;
    TickType_t m_interval;
};
} // namespace detect
} // namespace who
