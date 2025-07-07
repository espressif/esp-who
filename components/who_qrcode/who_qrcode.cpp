#include "who_qrcode.hpp"
#include "quirc.h"
#include "who_qrcode.hpp"

namespace who {
namespace qrcode {
WhoQRCode::WhoQRCode(const std::string &name, frame_cap::WhoFrameCapNode *frame_cap_node) :
    WhoTask(name), m_frame_cap_node(frame_cap_node), m_qr(quirc_new())
{
    frame_cap_node->add_new_frame_signal_subscriber(this);
#if CONFIG_IDF_TARGET_ESP32S3
    uint16_t w = BSP_LCD_H_RES, h = BSP_LCD_V_RES;
#elif CONFIG_IDF_TARGET_ESP32P4
    uint16_t w = BSP_LCD_H_RES / 2, h = BSP_LCD_V_RES / 2;
#endif
    quirc_resize(m_qr, w, h);
    uint8_t *data = quirc_begin(m_qr, nullptr, nullptr);
    m_input = {.data = data, .width = w, .height = h, .pix_type = dl::image::DL_IMAGE_PIX_TYPE_GRAY};
}

WhoQRCode::~WhoQRCode()
{
    quirc_destroy(m_qr);
}

void WhoQRCode::task()
{
    while (true) {
        EventBits_t event_bits =
            xEventGroupWaitBits(m_event_group, NEW_FRAME | PAUSE | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bits & STOP) {
            break;
        } else if (event_bits & PAUSE) {
            xEventGroupSetBits(m_event_group, PAUSED);
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, RESUME | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & STOP) {
                break;
            } else {
                continue;
            }
        }
        auto fb = m_frame_cap_node->cam_fb_peek();
        quirc_begin(m_qr, nullptr, nullptr);
#if CONFIG_IDF_TARGET_ESP32S3
        uint32_t caps = DL_IMAGE_CAP_RGB565_BIG_ENDIAN;
#elif CONFIG_IDF_TARGET_ESP32P4
        uint32_t caps = 0;
#endif
        dl::image::resize(*fb, m_input, dl::image::DL_IMAGE_INTERPOLATE_NEAREST, caps);
        quirc_end(m_qr);
        int num_codes = quirc_count(m_qr);
        for (int i = 0; i < num_codes; i++) {
            struct quirc_code code;
            struct quirc_data data;
            quirc_decode_error_t err;
            quirc_extract(m_qr, i, &code);
            err = quirc_decode(&code, &data);
            if (err == QUIRC_ERROR_DATA_ECC) {
                quirc_flip(&code);
                err = quirc_decode(&code, &data);
            }
            // Even if there are multiple codes, only process one of them.
            if (!err && m_result_cb) {
                m_result_cb(std::string(reinterpret_cast<const char *>(data.payload)));
                break;
            }
        }
    }
    xEventGroupSetBits(m_event_group, STOPPED);
    vTaskDelete(NULL);
}

void WhoQRCode::set_qrcode_result_cb(const std::function<void(const std::string &)> &result_cb)
{
    m_result_cb = result_cb;
}

void WhoQRCode::set_cleanup_func(const std::function<void()> &cleanup_func)
{
    m_cleanup = cleanup_func;
}

void WhoQRCode::cleanup()
{
    if (m_cleanup) {
        m_cleanup();
    }
}
} // namespace qrcode
} // namespace who
