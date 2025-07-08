#pragma once
#include "who_app.hpp"
#include "who_qrcode.hpp"

namespace who {
namespace app {
class WhoQRCodeAppBase : public WhoApp {
public:
    WhoQRCodeAppBase(frame_cap::WhoFrameCap *frame_cap);

protected:
    frame_cap::WhoFrameCap *m_frame_cap;
    qrcode::WhoQRCode *m_qrcode;
};
} // namespace app
} // namespace who
