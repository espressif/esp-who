#pragma once
#include "who_qrcode_base.hpp"
#include <list>
#include <string>

namespace who {
namespace qrcode {
class WhoQRCodeLCD : public WhoQRCodeBase {
public:
    WhoQRCodeLCD(frame_cap::WhoFrameCap *frame_cap, const std::string &name) :
        WhoQRCodeBase(frame_cap, name), m_res_mutex(xSemaphoreCreateMutex())
    {
    }
    void lcd_display_cb(who::cam::cam_fb_t *fb) override;

private:
    void task() override;
    void create_label();
    void on_new_qrcode_result(const char *result) override;
    SemaphoreHandle_t m_res_mutex;
    std::list<std::string> m_results;
    lv_obj_t *m_label;
};
} // namespace qrcode
} // namespace who
