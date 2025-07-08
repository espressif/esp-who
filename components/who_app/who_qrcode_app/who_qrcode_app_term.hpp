#pragma once
#include "who_qrcode_app_base.hpp"

namespace who {
namespace app {
class WhoQRCodeAppTerm : public WhoQRCodeAppBase {
public:
    WhoQRCodeAppTerm(frame_cap::WhoFrameCap *frame_cap);
    bool run() override;

protected:
    virtual void qrcode_result_cb(const std::string &result);
};
} // namespace app
} // namespace who
