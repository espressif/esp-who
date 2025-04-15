#include "who_lcd.hpp"
#include "esp_lcd_panel_ops.h"
#include <string.h>
#if BSP_CONFIG_NO_GRAPHIC_LIB
namespace who {
namespace lcd {
esp_lcd_panel_handle_t WhoLCD::get_lcd_panel_handle()
{
#if CONFIG_IDF_TARGET_ESP32S3
    return m_panel_handle;
#elif CONFIG_IDF_TARGET_ESP32P4
    return m_lcd_handles.panel;
#endif
}

#if CONFIG_IDF_TARGET_ESP32S3
void WhoLCD::init()
{
    const bsp_display_config_t bsp_disp_cfg = {
        .max_transfer_sz = BSP_LCD_H_RES * BSP_LCD_V_RES * (BSP_LCD_BITS_PER_PIXEL / 8),
    };
    ESP_ERROR_CHECK(bsp_display_new(&bsp_disp_cfg, &m_panel_handle, &m_io_handle));
    esp_lcd_panel_disp_on_off(m_panel_handle, true);
    ESP_ERROR_CHECK(bsp_display_backlight_on());
    m_lcd_buffer = heap_caps_malloc(BSP_LCD_H_RES * BSP_LCD_V_RES * (BSP_LCD_BITS_PER_PIXEL / 8),
                                    MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
}

void WhoLCD::draw_full_lcd(const void *data)
{
    // Currently esp-idf doesn't support DMA for PSRAM in esp32s3.
    // Display the fb must copy it from PSRAM to internal RAM.
    // Use a static internal memory chunk to avoid alloc internal memory each time dynamically.
    memcpy(m_lcd_buffer, data, BSP_LCD_H_RES * BSP_LCD_V_RES * (BSP_LCD_BITS_PER_PIXEL / 8));
    esp_lcd_panel_draw_bitmap(m_panel_handle, 0, 0, BSP_LCD_V_RES, BSP_LCD_H_RES, m_lcd_buffer);
}
#elif CONFIG_IDF_TARGET_ESP32P4
void WhoLCD::init()
{
    bsp_display_config_t bsp_disp_cfg = {
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
    bsp_display_new_with_handles(&bsp_disp_cfg, &m_lcd_handles);
    ESP_ERROR_CHECK(bsp_display_brightness_init());
    ESP_ERROR_CHECK(bsp_display_backlight_on());
}

void WhoLCD::draw_full_lcd(const void *data)
{
    esp_lcd_panel_draw_bitmap(m_lcd_handles.panel, 0, 0, BSP_LCD_V_RES, BSP_LCD_H_RES, data);
}
#endif
} // namespace lcd
} // namespace who
#endif
