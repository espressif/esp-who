#pragma once
#include "who_detect_base.hpp"
#include "who_lcd_disp.hpp"
#include "bsp/config.h"
#if !BSP_CONFIG_NO_GRAPHIC_LIB
#include "lvgl.h"
#endif
#include <queue>

namespace who {
namespace detect {
class WhoDetectLCD : public WhoDetectBase, public lcd_disp::IWhoLCDDisp {
public:
    WhoDetectLCD(const std::string &name,
                 frame_cap::WhoFrameCapNode *frame_cap_node,
                 lcd_disp::WhoLCDDisp *lcd_disp,
                 dl::detect::Detect *detect,
                 const std::vector<std::vector<uint8_t>> &palette);
    ~WhoDetectLCD();
    void lcd_display_cb(who::cam::cam_fb_t *fb) override;

protected:
    void on_new_detect_result(const result_t &result) override;
    void cleanup() override;

private:
#if BSP_CONFIG_NO_GRAPHIC_LIB
    std::vector<std::vector<uint8_t>> m_palette;
#else
    std::vector<lv_color_t> m_palette;
#endif
    SemaphoreHandle_t m_res_mutex;

    std::queue<result_t> m_results;
    result_t m_result;
};

} // namespace detect
} // namespace who
