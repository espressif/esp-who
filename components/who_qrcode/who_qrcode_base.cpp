#include "who_qrcode_base.hpp"

namespace who {
namespace qrcode {
void WhoQRCodeBase::task()
{
    while (true) {
        set_and_clear_bits(BLOCKING, RUNNING);
        EventBits_t event_bits =
            xEventGroupWaitBits(m_event_group, NEW_FRAME | PAUSE | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bits & STOP) {
            set_and_clear_bits(TERMINATE, BLOCKING);
            break;
        } else if (event_bits & PAUSE) {
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, RESUME | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & STOP) {
                set_and_clear_bits(TERMINATE, BLOCKING);
                break;
            } else {
                continue;
            }
        }
        set_and_clear_bits(RUNNING, BLOCKING);
        auto fb = m_frame_cap->get_cam()->cam_fb_peek();
        auto img = cam::fb2img(fb);
        quirc_begin(m_qr, nullptr, nullptr);
#if CONFIG_IDF_TARGET_ESP32S3
        uint32_t caps = 0;
#elif CONFIG_IDF_TARGET_ESP32P4
        uint32_t caps = DL_IMAGE_CAP_RGB565_BIG_ENDIAN;
#endif
        dl::image::resize(img, m_input, dl::image::DL_IMAGE_INTERPOLATE_NEAREST, caps);
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
            if (!err) {
                on_new_qrcode_result(reinterpret_cast<const char *>(data.payload));
                break;
            }
        }
    }
    vTaskDelete(NULL);
}
} // namespace qrcode
} // namespace who
