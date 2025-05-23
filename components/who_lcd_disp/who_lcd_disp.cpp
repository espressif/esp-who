#include "who_lcd_disp.hpp"

using namespace who::lcd;

namespace who {
namespace lcd_disp {
WhoLCDDisp::WhoLCDDisp(const std::string &name, frame_cap::WhoFrameCapNode *frame_cap_node, int peek_index) :
    WhoTask(name), m_lcd(new lcd::WhoLCD()), m_frame_cap_node(frame_cap_node), m_peek_index(peek_index)
{
    frame_cap_node->add_new_frame_signal_subscriber(this);
#if !BSP_CONFIG_NO_GRAPHIC_LIB
    m_lcd->create_canvas(frame_cap_node->get_fb_width(), frame_cap_node->get_fb_height());
#endif
}

WhoLCDDisp::~WhoLCDDisp()
{
    delete m_lcd;
    // TODO delete lvgl screen
}

void WhoLCDDisp::add_lcd_display_cb(IWhoLCDDisp *task)
{
    m_tasks.emplace_back(task);
}

void WhoLCDDisp::task()
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
        auto fb = m_frame_cap_node->cam_fb_peek(m_peek_index);
#if BSP_CONFIG_NO_GRAPHIC_LIB
        run_lcd_display_cbs(fb);
        m_lcd->draw_bitmap(fb->buf, (int)fb->width, (int)fb->height, 0, 0);
#else
        bsp_display_lock(0);
        lv_canvas_set_buffer(m_lcd->get_canvas(), fb->buf, fb->width, fb->height, LV_COLOR_FORMAT_NATIVE);
        run_lcd_display_cbs(fb);
        bsp_display_unlock();
#endif
    }
    xEventGroupSetBits(m_event_group, STOPPED);
    vTaskDelete(NULL);
}

void WhoLCDDisp::run_lcd_display_cbs(who::cam::cam_fb_t *fb)
{
    for (const auto &task : m_tasks) {
        if (task->is_active()) {
            task->lcd_display_cb(fb);
        }
    }
}

IWhoLCDDisp::IWhoLCDDisp(lcd_disp::WhoLCDDisp *lcd_disp, WhoTask *task) : m_lcd_disp(lcd_disp), m_task(task)
{
    lcd_disp->add_lcd_display_cb(this);
}
} // namespace lcd_disp
} // namespace who
