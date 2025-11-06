#pragma once
#include "who_detect_app_lcd.hpp"

namespace who {
namespace app {

class CatDetectApp : public WhoDetectAppLCD {
public:
    CatDetectApp(const std::vector<std::vector<uint8_t>> &palette, frame_cap::WhoFrameCap *frame_cap);
    ~CatDetectApp();

protected:
    void detect_result_cb(const detect::WhoDetect::result_t &result) override;

private:
    bool m_cat_detected;
    TickType_t m_last_cat_time;
    bool m_display_on;
};

} // namespace app
} // namespace who