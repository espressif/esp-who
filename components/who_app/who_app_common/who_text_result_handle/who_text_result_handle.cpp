#include "who_text_result_handle.hpp"

#if !BSP_CONFIG_NO_GRAPHIC_LIB
namespace who {
namespace lcd_disp {
WhoTextResultLCDDisp::WhoTextResultLCDDisp(task::WhoTask *task, lv_obj_t *label, int disp_num_frames) :
    m_task(task),
    m_disp_n_frames(disp_num_frames),
    m_disp_frames_cnt(disp_num_frames),
    m_res_mutex(xSemaphoreCreateMutex()),
    m_label(label)
{
}

WhoTextResultLCDDisp::~WhoTextResultLCDDisp()
{
    vSemaphoreDelete(m_res_mutex);
}

void WhoTextResultLCDDisp::save_text_result(const std::string &text)
{
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    m_result = text;
    xSemaphoreGive(m_res_mutex);
}

void WhoTextResultLCDDisp::lcd_disp_cb(who::cam::cam_fb_t *fb)
{
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    if (!m_result.empty()) {
        lv_label_set_text(m_label, m_result.c_str());
        m_result = {};
        m_disp_frames_cnt = 0;
    }
    xSemaphoreGive(m_res_mutex);
    if (m_disp_frames_cnt < m_disp_n_frames && ++m_disp_frames_cnt == m_disp_n_frames) {
        lv_label_set_text(m_label, "");
    }
}

void WhoTextResultLCDDisp::cleanup()
{
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    m_result = {};
    xSemaphoreGive(m_res_mutex);
}

} // namespace lcd_disp
} // namespace who
#endif
