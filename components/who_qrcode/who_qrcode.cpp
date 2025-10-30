#include "who_qrcode.hpp"
#include "quirc.h"
#include "who_qrcode.hpp"

namespace who {
namespace qrcode {
WhoQRCode::WhoQRCode(const std::string &name, frame_cap::WhoFrameCapNode *frame_cap_node) :
    task::WhoTask(name), m_frame_cap_node(frame_cap_node), m_qr(quirc_new())
{
    frame_cap_node->add_new_frame_signal_subscriber(this);
#if CONFIG_IDF_TARGET_ESP32S3
    uint16_t w = BSP_LCD_H_RES, h = BSP_LCD_V_RES;
    uint32_t caps = dl::image::DL_IMAGE_CAP_RGB565_BIG_ENDIAN;
#elif CONFIG_IDF_TARGET_ESP32P4
    uint16_t w = BSP_LCD_H_RES / 2, h = BSP_LCD_V_RES / 2;
    uint32_t caps = 0;
#endif
    quirc_resize(m_qr, w, h);
    m_image_transformer.set_caps(caps);
}

WhoQRCode::~WhoQRCode()
{
    quirc_destroy(m_qr);
}

void WhoQRCode::task()
{
    while (true) {
        EventBits_t event_bits =
            xEventGroupWaitBits(m_event_group, NEW_FRAME | TASK_PAUSE | TASK_STOP, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bits & TASK_STOP) {
            break;
        } else if (event_bits & TASK_PAUSE) {
            xEventGroupSetBits(m_event_group, TASK_PAUSED);
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, TASK_RESUME | TASK_STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & TASK_STOP) {
                break;
            } else {
                continue;
            }
        }
        auto fb = m_frame_cap_node->cam_fb_peek();
        int w, h;
        uint8_t *data = quirc_begin(m_qr, &w, &h);
        dl::image::img_t dst_img = {
            .data = data, .width = (uint16_t)w, .height = (uint16_t)h, .pix_type = dl::image::DL_IMAGE_PIX_TYPE_GRAY};
        m_image_transformer.set_src_img(*fb).set_dst_img(dst_img).transform();
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
    xEventGroupSetBits(m_event_group, TASK_STOPPED);
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
