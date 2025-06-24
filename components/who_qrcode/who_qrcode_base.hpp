#pragma once
// #include "who_cam_define.hpp"
#include "who_frame_cap.hpp"
#include "who_lcd_disp.hpp"

struct quirc;
namespace who {
namespace qrcode {
class WhoQRCodeBase : public WhoTask {
public:
    WhoQRCodeBase(const std::string &name, frame_cap::WhoFrameCapNode *frame_cap_node);
    ~WhoQRCodeBase();

protected:
    void task() override;

private:
    virtual void on_new_qrcode_result(const char *result) = 0;
    frame_cap::WhoFrameCapNode *m_frame_cap_node;
    struct quirc *m_qr;
    dl::image::img_t m_input;
};
} // namespace qrcode
} // namespace who
