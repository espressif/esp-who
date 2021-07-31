#include "app_lcd.h"
#include <string.h>
#include "wallpaper_128x240_rgb565.h"

static const char *TAG = "app_lcd";

static scr_driver_t g_lcd;
static scr_info_t g_lcd_info;

esp_err_t app_lcd_init()
{
    spi_config_t bus_conf = {
        .miso_io_num = -1,
        .mosi_io_num = 48,
        .sclk_io_num = 21,
        .max_transfer_sz = 2 * 240 * 240 + 10,
    };
    spi_bus_handle_t spi_bus = spi_bus_create(SPI2_HOST, &bus_conf);

    scr_interface_spi_config_t spi_lcd_cfg = {
        .spi_bus = spi_bus,
        .pin_num_cs = 43,
        .pin_num_dc = 47,
        .clk_freq = 40 * 1000000,
        .swap_data = 0,
    };

    scr_interface_driver_t *iface_drv;
    scr_interface_create(SCREEN_IFACE_SPI, &spi_lcd_cfg, &iface_drv);
    esp_err_t ret = scr_find_driver(SCREEN_CONTROLLER_ST7789, &g_lcd);
    if (ESP_OK != ret)
    {
        return ret;
        ESP_LOGE(TAG, "screen find failed");
    }

    scr_controller_config_t lcd_cfg = {
        .interface_drv = iface_drv,
        .pin_num_rst = -1,
        .pin_num_bckl = -1,
        .rst_active_level = 0,
        .bckl_active_level = 1,
        .offset_hor = 0,
        .offset_ver = 0,
        .width = 240,
        .height = 240,
        .rotate = 0,
    };
    ret = g_lcd.init(&lcd_cfg);
    if (ESP_OK != ret)
    {
        return ESP_FAIL;
        ESP_LOGE(TAG, "screen initialize failed");
    }

    g_lcd.get_info(&g_lcd_info);
    ESP_LOGI(TAG, "Screen name:%s | width:%d | height:%d", g_lcd_info.name, g_lcd_info.width, g_lcd_info.height);
    return ESP_OK;
}

void app_lcd_draw_wallpaper()
{
    scr_info_t lcd_info;
    g_lcd.get_info(&lcd_info);

    uint16_t *pixels = (uint16_t *)heap_caps_malloc((wallpaper_128x240_rgb565_width * wallpaper_128x240_rgb565_height) * sizeof(uint16_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    if (NULL == pixels)
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
        return;
    }
    memcpy(pixels, wallpaper_128x240_rgb565_array, (wallpaper_128x240_rgb565_width * wallpaper_128x240_rgb565_height) * sizeof(uint16_t));
    g_lcd.draw_bitmap(0, 0, wallpaper_128x240_rgb565_width, wallpaper_128x240_rgb565_height, (uint16_t *)pixels);
    heap_caps_free(pixels);
}

void app_lcd_set_color(int color)
{
    scr_info_t lcd_info;
    g_lcd.get_info(&lcd_info);
    uint16_t *buffer = (uint16_t *)malloc(lcd_info.width * sizeof(uint16_t));
    if (NULL == buffer)
    {
        for (size_t y = 0; y < lcd_info.height; y++)
        {
            for (size_t x = 0; x < lcd_info.width; x++)
            {
                g_lcd.draw_pixel(x, y, color);
            }
        }
    }
    else
    {
        for (size_t i = 0; i < lcd_info.width; i++)
        {
            buffer[i] = color;
        }

        for (int y = 0; y < lcd_info.height; y++)
        {
            g_lcd.draw_bitmap(0, y, lcd_info.width, 1, buffer);
        }

        free(buffer);
    }
}

void app_lcd_draw_bitmap(uint16_t *image_ptr, const uint16_t image_height, const uint16_t image_width)
{
    g_lcd.draw_bitmap(0, 0, image_width, image_height, image_ptr);
}