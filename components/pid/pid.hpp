#pragma once
#include "esp_timer.h"
#include <algorithm>

class PIDController {
public:
    PIDController(float kp, float ki, float kd, float output_init, float output_min, float output_max);

    float compute(float error);

    void reset();

    void set_gains(float kp, float ki, float kd) {
        kp_ = kp; ki_ = ki; kd_ = kd;
    }

private:
    int64_t nowUs() { return esp_timer_get_time(); }

    float kp_, ki_, kd_;

    float output_;

    float output_min_, output_max_;

    float integral_;
    float last_error_;
    int64_t last_time_us_;
    bool first_update_;
};