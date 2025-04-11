#include "who_lcd.hpp"
#include "esp_lcd_panel_ops.h"

namespace who {
namespace lcd {

#if BSP_CONFIG_NO_GRAPHIC_LIB
esp_lcd_panel_handle_t LCD::s_panel_handle = nullptr;
LCD::LCD()
{
    esp_lcd_panel_io_handle_t io_handle = nullptr;
#if CONFIG_IDF_TARGET_ESP32P4
    const bsp_display_config_t bsp_disp_cfg = {
#if CONFIG_BSP_LCD_TYPE_HDMI
#if CONFIG_BSP_LCD_HDMI_800x600_60HZ
        .hdmi_resolution = BSP_HDMI_RES_800x600,
#elif CONFIG_BSP_LCD_HDMI_1280x720_60HZ
        .hdmi_resolution = BSP_HDMI_RES_1280x720,
#elif CONFIG_BSP_LCD_HDMI_1280x800_60HZ
        .hdmi_resolution = BSP_HDMI_RES_1280x800,
#elif CONFIG_BSP_LCD_HDMI_1920x1080_30HZ
        .hdmi_resolution = BSP_HDMI_RES_1920x1080,
#endif
#else
        .hdmi_resolution = BSP_HDMI_RES_NONE,
#endif
        .dsi_bus = {
            .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
            .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
        }};
    ESP_ERROR_CHECK(bsp_display_new(&bsp_disp_cfg, &s_panel_handle, &io_handle));
#elif CONFIG_IDF_TARGET_ESP32S3
    const bsp_display_config_t bsp_disp_cfg = {
        .max_transfer_sz = BSP_LCD_H_RES * BSP_LCD_V_RES * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(bsp_display_new(&bsp_disp_cfg, &s_panel_handle, &io_handle));
    esp_lcd_panel_disp_on_off(s_panel_handle, true);
#endif
    ESP_ERROR_CHECK(bsp_display_backlight_on());
}
#else
lv_obj_t *LCD::s_canvas = nullptr;
LCD::LCD()
{
#if CONFIG_IDF_TARGET_ESP32P4
    bsp_display_cfg_t cfg = {.lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
                             .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
                             .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
                             .hw_cfg =
                                 {
#if CONFIG_BSP_LCD_TYPE_HDMI
#if CONFIG_BSP_LCD_HDMI_800x600_60HZ
                                     .hdmi_resolution = BSP_HDMI_RES_800x600,
#elif CONFIG_BSP_LCD_HDMI_1280x720_60HZ
                                     .hdmi_resolution = BSP_HDMI_RES_1280x720,
#elif CONFIG_BSP_LCD_HDMI_1280x800_60HZ
                                     .hdmi_resolution = BSP_HDMI_RES_1280x800,
#elif CONFIG_BSP_LCD_HDMI_1920x1080_30HZ
                                     .hdmi_resolution = BSP_HDMI_RES_1920x1080,
#endif
#else
                                     .hdmi_resolution = BSP_HDMI_RES_NONE,
#endif
                                     .dsi_bus =
                                         {
                                             .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
                                             .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
                                         }},
                             .flags = {
#if CONFIG_BSP_LCD_COLOR_FORMAT_RGB888
                                 .buff_dma = false,
#else
                                 .buff_dma = true,
#endif
                                 .buff_spiram = false,
                                 .sw_rotate = true,
                             }};
    cfg.lvgl_port_cfg.task_affinity = 0;
    bsp_display_start_with_config(&cfg);
#elif CONFIG_IDF_TARGET_ESP32S3
    bsp_display_cfg_t cfg = {.lvgl_port_cfg =
                                 {
                                     .task_priority = 4,
                                     .task_stack = 6144,
                                     .task_affinity = 0,
                                     .task_max_sleep_ms = CONFIG_BSP_DISPLAY_LVGL_MAX_SLEEP,
                                     .timer_period_ms = CONFIG_BSP_DISPLAY_LVGL_TICK,
                                 },
                             .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
                             .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
                             .flags = {
                                 .buff_dma = true,
#if CONFIG_BSP_DISPLAY_LVGL_BUFFER_IN_PSRAM
                                 .buff_spiram = true,
#else
                                 .buff_spiram = false,
#endif
                             }};
    bsp_display_start_with_config(&cfg);
#endif
    ESP_ERROR_CHECK(bsp_display_backlight_on());

    bsp_display_lock(0);
    s_canvas = lv_canvas_create(lv_scr_act());
    lv_obj_set_size(s_canvas, BSP_LCD_H_RES, BSP_LCD_V_RES);
    bsp_display_unlock();
}
#endif

} // namespace lcd
} // namespace who
