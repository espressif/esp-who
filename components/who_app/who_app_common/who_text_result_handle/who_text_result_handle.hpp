#pragma once
#include "who_cam_define.hpp"
#include "who_task.hpp"
#include "bsp/esp-bsp.h"

#if !BSP_CONFIG_NO_GRAPHIC_LIB
namespace who {
namespace lcd_disp {
class WhoTextResultLCDDisp {
public:
    WhoTextResultLCDDisp(WhoTask *task, lv_obj_t *label, int disp_n_frames);
    ~WhoTextResultLCDDisp();
    void save_text_result(const std::string &text);
    void lcd_disp_cb(who::cam::cam_fb_t *fb);
    void cleanup();

private:
    WhoTask *m_task;
    int m_disp_n_frames;
    int m_disp_frames_cnt;
    SemaphoreHandle_t m_res_mutex;
    std::string m_result;
    lv_obj_t *m_label;
};
} // namespace lcd_disp
} // namespace who
#endif
