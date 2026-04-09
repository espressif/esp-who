#include "track.hpp"
#include "who_yield2idle.hpp"
#include "who_detect_result_handle.hpp"
#include <cmath>
#include <numbers>
#include <cstdlib>

namespace who {
namespace app {
WhoDetectTrackAppLCD::WhoDetectTrackAppLCD(frame_cap::WhoFrameCap *frame_cap) :
    WhoDetectAppBase(frame_cap),
    m_pid_pan(atof(CONFIG_PAN_KP), atof(CONFIG_PAN_KI), atof(CONFIG_PAN_KD), CONFIG_PAN_INIT_ANGLE, CONFIG_PAN_MIN_ANGLE, CONFIG_PAN_MAX_ANGLE),
    m_pid_tilt(atof(CONFIG_TILT_KP), atof(CONFIG_TILT_KI), atof(CONFIG_TILT_KD), CONFIG_TILT_INIT_ANGLE, CONFIG_TILT_MIN_ANGLE, CONFIG_TILT_MAX_ANGLE),
    m_target_lost(true),
    m_lost_frames(0)
{
    m_detect->set_detect_result_cb(std::bind(&WhoDetectTrackAppLCD::detect_result_cb, this, std::placeholders::_1));
    m_detect->set_cleanup_func(std::bind(&WhoDetectTrackAppLCD::cleanup, this));
    m_half_w = frame_cap->get_last_node()->get_fb_width() / 2.f;
    m_half_h = frame_cap->get_last_node()->get_fb_height() / 2.f;
    m_mcpwm_pan = std::make_unique<MCPWM>((gpio_num_t)CONFIG_PAN_GPIO, 0);
    mcpwm_timer_handle_t timer;
    int group_id;
    m_mcpwm_pan->get_timer(&timer, &group_id);
    m_mcpwm_tilt = std::make_unique<MCPWM>((gpio_num_t)CONFIG_TILT_GPIO, timer, group_id);
    m_mcpwm_pan->enable_and_start_timer();
    m_mcpwm_pan->set_servo_angle(CONFIG_PAN_INIT_ANGLE);
    m_mcpwm_tilt->set_servo_angle(CONFIG_TILT_INIT_ANGLE);
}

bool WhoDetectTrackAppLCD::run()
{
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_detect->run(4096, 2, 1);
    return ret;
}

void WhoDetectTrackAppLCD::detect_result_cb(const detect::WhoDetect::result_t &result)
{
    auto det_res = result.det_res;
    who::detect::draw_detect_results_on_img(result.img, result.det_res, {{0x00, 0xf8}});
    m_lcd.draw_bitmap(result.img.data, (int)result.img.width, (int)result.img.height, 0, 0);

    if (det_res.empty()) {
        //TODO
    } else {
        m_lost_frames = 0;
        m_target_lost = false;

        auto max_det_res = std::max_element(
            det_res.begin(), det_res.end(), [](const dl::detect::result_t &a, const dl::detect::result_t &b) -> bool {
                return a.box_area() < b.box_area();
            });

        float det_cx = (max_det_res->box[0] + max_det_res->box[2]) / 2.0f;
        float det_cy = (max_det_res->box[1] + max_det_res->box[3]) / 2.0f;
        float det_w = max_det_res->box[2] - max_det_res->box[0];
        float det_h = max_det_res->box[3] - max_det_res->box[1];

        m_kf.step(det_cx, det_cy, det_w, det_h);

        float cx = m_kf.getCx();
        float cy = m_kf.getCy();

        float error_x = std::atan2(CONFIG_PAN_DIR * (cx - m_half_w) / 2, atof(CONFIG_CAMERA_FX)) * 180.0 / std::numbers::pi;
        float error_y = std::atan2(CONFIG_TILT_DIR * (cy - m_half_h) / 2, atof(CONFIG_CAMERA_FY)) * 180.0 / std::numbers::pi;

        constexpr float kDeadzone = 0.1f;
        if (std::abs(m_kf.getVcx()) < kDeadzone) error_x = 0;
        if (std::abs(m_kf.getVcy()) < kDeadzone) error_y = 0;

        constexpr float kSpeedThreshold = 1.f;
        constexpr float kMinScale = 0.1f;
        float speed_ratio_x = std::min(std::abs(m_kf.getVcx()) / kSpeedThreshold, 1.0f);
        float scale_x = kMinScale + speed_ratio_x * (1.0f - kMinScale);
        float speed_ratio_y = std::min(std::abs(m_kf.getVcy()) / kSpeedThreshold, 1.0f);
        float scale_y = kMinScale + speed_ratio_y * (1.0f - kMinScale);
        error_x *= scale_x;
        error_y *= scale_y;

        float pan = m_pid_pan.compute(error_x);
        float tilt = m_pid_tilt.compute(error_y);
        m_mcpwm_pan->set_servo_angle(pan);
        m_mcpwm_tilt->set_servo_angle(tilt);
    }
}

void WhoDetectTrackAppLCD::cleanup()
{
    m_pid_pan.reset();
    m_pid_tilt.reset();
    m_kf.reset();
    m_target_lost = true;
    m_lost_frames = 0;
}
}
}