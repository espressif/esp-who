#pragma once
#include "dl_detect_define.hpp"
#include "who_cam_define.hpp"
#include <list>
#include "bsp/config.h"
#if !BSP_CONFIG_NO_GRAPHIC_LIB
#include "who_lvgl_utils.hpp"
#endif

namespace who {
namespace detect {

void draw_detect_results_on_fb(who::cam::cam_fb_t *fb,
                               const std::list<dl::detect::result_t> &detect_res,
                               const std::vector<std::vector<uint8_t>> &palette);

#if !BSP_CONFIG_NO_GRAPHIC_LIB
void draw_detect_results_on_canvas(lv_obj_t *canvas,
                                   const std::list<dl::detect::result_t> &detect_res,
                                   const std::vector<lv_color_t> &palette);
#endif

void print_detect_results(const std::list<dl::detect::result_t> &detect_res);

} // namespace detect
} // namespace who
