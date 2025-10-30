#pragma once
#include "who_frame_cap.hpp"

#if CONFIG_IDF_TARGET_ESP32S3
who::frame_cap::WhoFrameCap *get_lcd_dvp_frame_cap_pipeline();
who::frame_cap::WhoFrameCap *get_term_dvp_frame_cap_pipeline();
#elif CONFIG_IDF_TARGET_ESP32P4
who::frame_cap::WhoFrameCap *get_lcd_mipi_csi_frame_cap_pipeline();
who::frame_cap::WhoFrameCap *get_lcd_mipi_csi_ppa_frame_cap_pipeline(
    who::frame_cap::WhoFrameCapNode **lcd_disp_frame_cap_node);
who::frame_cap::WhoFrameCap *get_lcd_uvc_frame_cap_pipeline();
who::frame_cap::WhoFrameCap *get_term_mipi_csi_frame_cap_pipeline();
who::frame_cap::WhoFrameCap *get_term_mipi_csi_ppa_frame_cap_pipeline();
who::frame_cap::WhoFrameCap *get_term_uvc_frame_cap_pipeline();
#endif
