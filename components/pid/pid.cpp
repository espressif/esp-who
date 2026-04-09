#include "pid.hpp"

PIDController::PIDController(float kp, float ki, float kd, float output_init, float output_min, float output_max) :
    kp_(kp),
    ki_(ki),
    kd_(kd),
    output_(output_init),
    output_min_(output_min),
    output_max_(output_max),
    integral_(0.0),
    last_error_(0.0),
    last_time_us_(nowUs()),
    first_update_(true)
{
}

float PIDController::compute(float error)
{
    int64_t now = nowUs();
    float dt = (now - last_time_us_) / 1e6;
    last_time_us_ = now;

    float proportional = kp_ * error;

    if (first_update_ || dt <= 0 || dt > 1.0) {
        first_update_ = false;
        last_error_ = error;
        integral_ = 0.0;
        output_ += proportional;
        return std::clamp(output_, output_min_, output_max_);
    }

    float integral = error * dt;
    integral_ += integral;

    float derivative = (error - last_error_) / dt;
    last_error_ = error;

    float output_delta = proportional + ki_ * integral_ + kd_ * derivative;
    
    output_ += output_delta;

    float output_clamped = std::clamp(output_, output_min_, output_max_);

    if (output_clamped != output_) {
        integral_ -= integral;
    }

    output_ = output_clamped;

    return output_;
}

void PIDController::reset()
{
    integral_ = 0.0;
    last_error_ = 0.0;
    last_time_us_ = nowUs();
    first_update_ = true;
}