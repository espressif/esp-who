#pragma once
#include "who_detect_base.hpp"
#include "bsp/config.h"
#if !BSP_CONFIG_NO_GRAPHIC_LIB
#include "lvgl.h"
#endif
#include <queue>

namespace who {
namespace detect {
class WhoDetectLCD : public WhoDetectBase {
public:
    WhoDetectLCD(frame_cap::WhoFrameCap *frame_cap,
                 dl::detect::Detect *detect,
                 const std::string &name,
                 const std::vector<std::vector<uint8_t>> &palette);
    WhoDetectLCD(frame_cap::WhoFrameCap *frame_cap,
                 const std::string &name,
                 const std::vector<std::vector<uint8_t>> &palette) :
        WhoDetectLCD(frame_cap, nullptr, name, palette)
    {
    }
    ~WhoDetectLCD();
    void lcd_display_cb(who::cam::cam_fb_t *fb) override;

protected:
    void on_new_detect_result(const result_t &result) override;

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
