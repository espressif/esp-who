#pragma once
#include "who_lcd_disp.hpp"
#include "who_qrcode_base.hpp"
#include <list>
#include <string>

namespace who {
namespace qrcode {
class WhoQRCodeLCD : public WhoQRCodeBase, public lcd_disp::IWhoLCDDisp {
public:
    WhoQRCodeLCD(const std::string &name, frame_cap::WhoFrameCapNode *frame_cap_node, lcd_disp::WhoLCDDisp *lcd_disp);
    ~WhoQRCodeLCD();
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
