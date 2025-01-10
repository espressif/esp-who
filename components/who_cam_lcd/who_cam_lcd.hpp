#pragma once
#include "cam.hpp"
#include "lcd.hpp"

namespace who {
namespace app {

class WhoCamLCD {
public:
    WhoCamLCD(who::cam::Cam *cam) : m_cam(cam), m_lcd(new who::lcd::LCD()) {};
    ~WhoCamLCD()
    {
        if (m_lcd) {
            delete m_lcd;
            m_lcd = nullptr;
        }
    };
    void run();
    static void register_task_bit(EventBits_t task_bit) { s_task_bits |= task_bit; }
    who::cam::Cam *m_cam;
    static EventGroupHandle_t s_event_group;

private:
    static void task(void *args);
    who::lcd::LCD *m_lcd;
    static EventBits_t s_task_bits;
};

} // namespace app
} // namespace who
