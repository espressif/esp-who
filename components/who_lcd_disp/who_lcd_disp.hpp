#pragma once
#include "who_frame_cap.hpp"
#include "who_lcd.hpp"
#include <vector>

namespace who {
namespace lcd_disp {
class IWhoLCDDisp;

class WhoLCDDisp : public WhoTask {
public:
    WhoLCDDisp(const std::string &name, frame_cap::WhoFrameCapNode *frame_cap_node, int peek_index = 0);
    ~WhoLCDDisp();
    void add_lcd_display_cb(IWhoLCDDisp *task);
    lcd::WhoLCD *get_lcd() { return m_lcd; }
    frame_cap::WhoFrameCapNode *get_frame_cap_node() { return m_frame_cap_node; }

private:
    void task() override;
    void run_lcd_display_cbs(who::cam::cam_fb_t *fb);
    lcd::WhoLCD *m_lcd;
    frame_cap::WhoFrameCapNode *m_frame_cap_node;
    bool m_peek_index;
    std::vector<IWhoLCDDisp *> m_tasks;
};

class IWhoLCDDisp {
public:
    IWhoLCDDisp(lcd_disp::WhoLCDDisp *lcd_disp, WhoTask *task);
    virtual void lcd_display_cb(who::cam::cam_fb_t *fb) = 0;
    bool is_active() { return m_task->is_active(); }
    lcd::WhoLCD *get_lcd() { return m_lcd_disp->get_lcd(); }

protected:
    lcd_disp::WhoLCDDisp *m_lcd_disp;
    WhoTask *m_task;
};
} // namespace lcd_disp
} // namespace who
