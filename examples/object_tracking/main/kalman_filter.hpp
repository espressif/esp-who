#pragma once

#include "mat.h"
#include <cmath>

namespace who {
namespace filter {

constexpr int kStateDim = 8;
constexpr int kMeasDim = 4;
class KalmanFilterBBox {
public:
    KalmanFilterBBox(float process_noise_pos = 1e-2f,
                     float process_noise_vel = 1e-5f,
                     float measurement_noise = 1e-1f)
        : proc_noise_pos_(process_noise_pos),
          proc_noise_vel_(process_noise_vel),
          meas_noise_(measurement_noise),
          F_(kStateDim, kStateDim),
          H_(kMeasDim, kStateDim),
          Q_(kStateDim, kStateDim),
          R_(kMeasDim, kMeasDim),
          P_(kStateDim, kStateDim),
          state_(kStateDim, 1),
          initialized_(false)
    {
        initMatrices();
    }

    void initMatrices() {
        F_ = dspm::Mat::eye(kStateDim);
        F_(0, 4) = 1.0f;
        F_(1, 5) = 1.0f;
        F_(2, 6) = 1.0f;
        F_(3, 7) = 1.0f;

        H_.clear();
        H_(0, 0) = 1.0f;
        H_(1, 1) = 1.0f;
        H_(2, 2) = 1.0f;
        H_(3, 3) = 1.0f;

        Q_.clear();
        Q_(0, 0) = proc_noise_pos_;
        Q_(1, 1) = proc_noise_pos_;
        Q_(2, 2) = proc_noise_pos_;
        Q_(3, 3) = proc_noise_pos_;
        Q_(4, 4) = proc_noise_vel_;
        Q_(5, 5) = proc_noise_vel_;
        Q_(6, 6) = proc_noise_vel_;
        Q_(7, 7) = proc_noise_vel_;

        R_.clear();
        R_(0, 0) = meas_noise_;
        R_(1, 1) = meas_noise_;
        R_(2, 2) = meas_noise_;
        R_(3, 3) = meas_noise_;
    }

    void initiate(float cx, float cy, float w, float h) {
        state_.clear();
        state_(0, 0) = cx;
        state_(1, 0) = cy;
        state_(2, 0) = w;
        state_(3, 0) = h;

        P_ = dspm::Mat::eye(kStateDim) * 10.0f;
        initialized_ = true;
    }

    void predict() {
        if (!initialized_) return;

        state_ = F_ * state_;

        P_ = F_ * P_ * F_.t() + Q_;
    }

    void update(float cx, float cy, float w, float h) {
        if (!initialized_) {
            initiate(cx, cy, w, h);
            return;
        }

        dspm::Mat z(kMeasDim, 1);
        z(0, 0) = cx;
        z(1, 0) = cy;
        z(2, 0) = w;
        z(3, 0) = h;

        dspm::Mat S = H_ * P_ * H_.t() + R_;

        dspm::Mat Hx = H_ * state_;
        dspm::Mat y = z - Hx;

        dspm::Mat PHt = P_ * H_.t();

        dspm::Mat K(kStateDim, kMeasDim);

        dspm::Mat L = choleskyDecompose(S);  // S = L * L^T

        for (int col = 0; col < kStateDim; col++) {
            dspm::Mat b(kMeasDim, 1);
            for (int i = 0; i < kMeasDim; i++) {
                b(i, 0) = PHt(col, i);
            }

            dspm::Mat y_temp(kMeasDim, 1);
            for (int i = 0; i < kMeasDim; i++) {
                float sum = b(i, 0);
                for (int j = 0; j < i; j++) {
                    sum -= L(i, j) * y_temp(j, 0);
                }
                y_temp(i, 0) = sum / L(i, i);
            }

            dspm::Mat x(kMeasDim, 1);
            for (int i = kMeasDim - 1; i >= 0; i--) {
                float sum = y_temp(i, 0);
                for (int j = i + 1; j < kMeasDim; j++) {
                    sum -= L(j, i) * x(j, 0);
                }
                x(i, 0) = sum / L(i, i);
            }

            for (int i = 0; i < kMeasDim; i++) {
                K(col, i) = x(i, 0);
            }
        }

        state_ = state_ + K * y;

        dspm::Mat I = dspm::Mat::eye(kStateDim);
        P_ = (I - K * H_) * P_;
    }

    void step(float cx, float cy, float w, float h) {
        predict();
        update(cx, cy, w, h);
    }

    float getCx() const { return initialized_ ? state_(0, 0) : 0.0f; }
    float getCy() const { return initialized_ ? state_(1, 0) : 0.0f; }
    float getW() const { return initialized_ ? state_(2, 0) : 0.0f; }
    float getH() const { return initialized_ ? state_(3, 0) : 0.0f; }
    float getVcx() const { return initialized_ ? state_(4, 0) : 0.0f; }
    float getVcy() const { return initialized_ ? state_(5, 0) : 0.0f; }

    bool isInitialized() const { return initialized_; }

    void reset() {
        initialized_ = false;
        state_.clear();
        P_ = dspm::Mat::eye(kStateDim) * 10.0f;
    }

private:
    float proc_noise_pos_;
    float proc_noise_vel_;
    float meas_noise_;

    dspm::Mat F_;
    dspm::Mat H_;
    dspm::Mat Q_;
    dspm::Mat R_;
    dspm::Mat P_;
    dspm::Mat state_;

    bool initialized_;

    dspm::Mat choleskyDecompose(const dspm::Mat& A) {
        dspm::Mat L(A.rows, A.cols);
        L.clear();

        for (int i = 0; i < A.rows; i++) {
            for (int j = 0; j <= i; j++) {
                float sum = A(i, j);

                for (int k = 0; k < j; k++) {
                    sum -= L(i, k) * L(j, k);
                }

                if (i == j) {
                    if (sum <= 0) {
                        sum = 1e-6f;
                    }
                    L(i, j) = std::sqrt(sum);
                } else {
                    L(i, j) = sum / L(j, j);
                }
            }
        }

        return L;
    }
};

} // namespace filter
} // namespace who
