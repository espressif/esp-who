#pragma once
#include "dl_detect_base.hpp"
#include "who_frame_cap.hpp"

namespace who {
namespace detect {
class WhoDetect : public task::WhoTask {
public:
    static inline constexpr EventBits_t NEW_FRAME = frame_cap::WhoFrameCapNode::NEW_FRAME;

    typedef struct {
        std::list<dl::detect::result_t> det_res;
        struct timeval timestamp;
        dl::image::img_t img;
    } result_t;

    WhoDetect(const std::string &name, frame_cap::WhoFrameCapNode *frame_cap_node);
    ~WhoDetect();
    void set_model(dl::detect::Detect *model);
    void set_rescale_params(float rescale_x, float rescale_y, uint16_t rescale_max_w, uint16_t rescale_max_h);
    void set_fps(float fps);
    void set_detect_result_cb(const std::function<void(const result_t &)> &result_cb);
    void set_cleanup_func(const std::function<void()> &cleanup_func);
    bool run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID) override;
    bool stop_async() override;
    bool pause_async() override;

private:
    void task() override;
    void cleanup() override;
    void rescale_detect_result(std::list<dl::detect::result_t> &result);

    frame_cap::WhoFrameCapNode *m_frame_cap_node;
    dl::detect::Detect *m_model;
    TickType_t m_interval;
    float m_inv_rescale_x;
    float m_inv_rescale_y;
    uint16_t m_rescale_max_w;
    uint16_t m_rescale_max_h;
    std::function<void(const result_t &)> m_result_cb;
    std::function<void()> m_cleanup;
    SemaphoreHandle_t m_result_cb_mutex;
};
} // namespace detect
} // namespace who
