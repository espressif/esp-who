#include "who_lvgl_lcd.hpp"
#include "esp_lcd_panel_ops.h"
#if !BSP_CONFIG_NO_GRAPHIC_LIB
namespace who {
namespace lcd {
#if CONFIG_IDF_TARGET_ESP32S3
void WhoLCD::init(const lvgl_port_cfg_t &lvgl_port_cfg)
{
    lvgl_port_init(&lvgl_port_cfg);
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    const bsp_display_config_t bsp_disp_cfg = {
        .max_transfer_sz = BSP_LCD_H_RES * BSP_LCD_V_RES * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(bsp_display_new(&bsp_disp_cfg, &panel_handle, &io_handle));
    esp_lcd_panel_disp_on_off(panel_handle, true);
#ifdef BSP_BOARD_ESP32_S3_KORVO_2
    bool mirror_x = true;
    bool mirror_y = true;
#else
    bool mirror_x = false;
    bool mirror_y = false;
#endif
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        .hres = BSP_LCD_H_RES,
        .vres = BSP_LCD_V_RES,
        .monochrome = false,
        /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
        .rotation =
            {
                .swap_xy = false,
                .mirror_x = mirror_x,
                .mirror_y = mirror_y,
            },
        .flags = {
            .buff_dma = true,
            .buff_spiram = false,
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = false,
#endif
        }};
    m_disp = lvgl_port_add_disp(&disp_cfg);
    ESP_ERROR_CHECK(bsp_display_backlight_on());
}
void WhoLCD::deinit()
{
    // TODO
}
#elif CONFIG_IDF_TARGET_ESP32P4
void WhoLCD::init(const lvgl_port_cfg_t &lvgl_port_cfg)
{
    bsp_display_cfg_t cfg = {.lvgl_port_cfg = lvgl_port_cfg,
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
    m_disp = bsp_display_start_with_config(&cfg);
    ESP_ERROR_CHECK(bsp_display_backlight_on());
}
void WhoLCD::deinit()
{
    bsp_display_stop(m_disp);
    // only a workaround, i2c should not deinitialized by bsp_display.
    ESP_ERROR_CHECK(bsp_i2c_init());
}
#endif
void WhoLCD::create_canvas(uint16_t width, uint16_t height)
{
    bsp_display_lock(0);
    m_canvas = lv_canvas_create(lv_scr_act());
    lv_obj_set_size(m_canvas, width, height);
    bsp_display_unlock();
}
} // namespace lcd
} // namespace who
#endif
