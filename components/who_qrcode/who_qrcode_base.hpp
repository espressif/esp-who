#pragma once
#include "quirc.h"
#include "who_cam_define.hpp"
#include "who_frame_cap.hpp"
#include "who_subscriber.hpp"

namespace who {
namespace qrcode {
class WhoQRCodeBase : public WhoSubscriber {
public:
    WhoQRCodeBase(frame_cap::WhoFrameCap *frame_cap, const std::string &name) :
        WhoSubscriber(name), m_frame_cap(frame_cap), m_qr(quirc_new())
    {
        frame_cap->add_element(this);
#if CONFIG_IDF_TARGET_ESP32S3
        int w = BSP_LCD_H_RES, h = BSP_LCD_V_RES;
#elif CONFIG_IDF_TARGET_ESP32P4
        int w = BSP_LCD_H_RES / 2, h = BSP_LCD_V_RES / 2;
#endif
        quirc_resize(m_qr, w, h);
        uint8_t *data = quirc_begin(m_qr, nullptr, nullptr);
        m_input = {.data = data, .width = w, .height = h, .pix_type = dl::image::DL_IMAGE_PIX_TYPE_GRAY};
    }

protected:
    void task() override;

private:
    virtual void on_new_qrcode_result(const char *result) = 0;
    frame_cap::WhoFrameCap *m_frame_cap;
    struct quirc *m_qr;
    dl::image::img_t m_input;
};
} // namespace qrcode
} // namespace who
