#pragma once
#include "dl_detect_base.hpp"
#include "who_frame_cap.hpp"
#include <list>

namespace who {
namespace detect {
class WhoDetectBase : public WhoTask {
public:
    typedef struct {
        std::list<dl::detect::result_t> det_res;
        struct timeval timestamp;
        who::cam::cam_fb_t *fb;
    } result_t;

    WhoDetectBase(const std::string &name, frame_cap::WhoFrameCapNode *frame_cap_node, dl::detect::Detect *detect);
    ~WhoDetectBase();
    void set_rescale_params(float rescale_x, float rescale_y, uint16_t rescale_max_w, uint16_t rescale_max_h);
    void set_fps(float fps);
    bool run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID) override;
    bool stop_async() override;
    bool pause_async() override;

protected:
    frame_cap::WhoFrameCapNode *m_frame_cap_node;

private:
    void task() override;
    virtual void on_new_detect_result(const result_t &result) = 0;
    dl::detect::Detect *m_detect;
    TickType_t m_interval;
    float m_inv_rescale_x;
    float m_inv_rescale_y;
    uint16_t m_rescale_max_w;
    uint16_t m_rescale_max_h;
    void rescale_detect_result(std::list<dl::detect::result_t> &result);
};
} // namespace detect
} // namespace who
