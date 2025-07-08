#pragma once
#include "who_detect.hpp"
#include <queue>
#include "bsp/esp-bsp.h"

namespace who {
namespace detect {
void draw_detect_results_on_img(const dl::image::img_t &img,
                                const std::list<dl::detect::result_t> &detect_res,
                                const std::vector<std::vector<uint8_t>> &palette);

#if !BSP_CONFIG_NO_GRAPHIC_LIB
void draw_detect_results_on_canvas(lv_obj_t *canvas,
                                   const std::list<dl::detect::result_t> &detect_res,
                                   const std::vector<lv_color_t> &palette);
#endif

void print_detect_results(const std::list<dl::detect::result_t> &detect_res);
} // namespace detect

namespace lcd_disp {
class WhoDetectResultLCDDisp {
public:
#if !BSP_CONFIG_NO_GRAPHIC_LIB
    WhoDetectResultLCDDisp(WhoTask *task, lv_obj_t *canvas, const std::vector<std::vector<uint8_t>> &palette);
#else
    WhoDetectResultLCDDisp(WhoTask *task, const std::vector<std::vector<uint8_t>> &palette);
#endif
    ~WhoDetectResultLCDDisp();
    void save_detect_result(const detect::WhoDetect::result_t &result);
    void lcd_disp_cb(who::cam::cam_fb_t *fb);
    void cleanup();

private:
    WhoTask *m_task;
    SemaphoreHandle_t m_res_mutex;
    std::queue<detect::WhoDetect::result_t> m_results;
    detect::WhoDetect::result_t m_result;
#if BSP_CONFIG_NO_GRAPHIC_LIB
    std::vector<std::vector<uint8_t>> m_palette;
#else
    lv_obj_t *m_canvas;
    std::vector<lv_color_t> m_palette;
#endif
};
} // namespace lcd_disp
} // namespace who
