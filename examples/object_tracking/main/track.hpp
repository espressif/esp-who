#pragma once

#include "who_detect_app_base.hpp"
#include "pid.hpp"
#include "mcpwm.hpp"
#include "kalman_filter.hpp"
#include <memory>
#include "who_lcd.hpp"

namespace who {
namespace app {
class WhoDetectTrackAppLCD : public WhoDetectAppBase {
public:
    WhoDetectTrackAppLCD(frame_cap::WhoFrameCap *frame_cap);
    bool run() override;

protected:
    void detect_result_cb(const detect::WhoDetect::result_t &result);
    void cleanup();
private:
    who::lcd::WhoLCD m_lcd;
    PIDController m_pid_pan;
    PIDController m_pid_tilt;
    std::unique_ptr<MCPWM> m_mcpwm_pan;
    std::unique_ptr<MCPWM> m_mcpwm_tilt;
    float m_pan;
    float m_tilt;
    float m_half_w;
    float m_half_h;

    filter::KalmanFilterBBox m_kf;
    bool m_target_lost;
    int m_lost_frames;
    static constexpr int kMaxLostFrames = 30;
};
}
}