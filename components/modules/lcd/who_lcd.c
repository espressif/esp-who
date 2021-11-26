#include "who_lcd.h"
#include "esp_camera.h"
#include <string.h>
#include "logo_en_240x240_lcd.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

static const char *TAG = "who_lcd";

static scr_driver_t g_lcd;
static scr_info_t g_lcd_info;

static QueueHandle_t xQueueFrameI = NULL;
static QueueHandle_t xQueueFrameO = NULL;
static SemaphoreHandle_t xMutexLCD = NULL;

static bool gReturnFB = true;
#define MAX_LCD_LINE 40 

static void task_process_handler(void *arg)
{
    camera_fb_t *frame = NULL;

    while (true)
    {
        if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
        {
            xSemaphoreTake(xMutexLCD, portMAX_DELAY);
            g_lcd.draw_bitmap(0, 0, frame->width, frame->height, (uint16_t *)frame->buf);
            xSemaphoreGive(xMutexLCD);

            if (xQueueFrameO)
            {
                xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
            }
            else if (gReturnFB)
            {
                esp_camera_fb_return(frame);
            }
            else
            {
                free(frame);
            }
        }
    }
}

esp_err_t register_lcd(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb)
{
    xMutexLCD = xSemaphoreCreateMutex();
    spi_config_t bus_conf = {
        .miso_io_num = BOARD_LCD_MISO,
        .mosi_io_num = BOARD_LCD_MOSI,
        .sclk_io_num = BOARD_LCD_SCK,
        .max_transfer_sz = 2 * 240 * 240 + 10,
    };
    spi_bus_handle_t spi_bus = spi_bus_create(SPI2_HOST, &bus_conf);

    scr_interface_spi_config_t spi_lcd_cfg = {
        .spi_bus = spi_bus,
        .pin_num_cs = BOARD_LCD_CS,
        .pin_num_dc = BOARD_LCD_DC,
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
        .pin_num_rst = BOARD_LCD_RST,
        .pin_num_bckl = BOARD_LCD_BL,
        .rst_active_level = 0,
        .bckl_active_level = 0,
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

    app_lcd_set_color(0x000000);
    vTaskDelay(pdMS_TO_TICKS(500));
    app_lcd_draw_wallpaper();
    vTaskDelay(pdMS_TO_TICKS(1000));

    xQueueFrameI = frame_i;
    xQueueFrameO = frame_o;
    gReturnFB = return_fb;
    xTaskCreatePinnedToCore(task_process_handler, TAG, 4 * 1024, NULL, 5, NULL, 0);

    return ESP_OK;
}

void app_lcd_draw_wallpaper()
{
    scr_info_t lcd_info;
    g_lcd.get_info(&lcd_info);

    uint16_t *pixels = (uint16_t *)heap_caps_malloc((logo_en_240x240_lcd_width * logo_en_240x240_lcd_height) * sizeof(uint16_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    if (NULL == pixels)
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
        return;
    }
    memcpy(pixels, logo_en_240x240_lcd, (logo_en_240x240_lcd_width * logo_en_240x240_lcd_height) * sizeof(uint16_t));
    xSemaphoreTake(xMutexLCD, portMAX_DELAY);
    g_lcd.draw_bitmap(0, 0, logo_en_240x240_lcd_width, logo_en_240x240_lcd_height, (uint16_t *)pixels);
    xSemaphoreGive(xMutexLCD);
    heap_caps_free(pixels);
}

void app_lcd_set_color(int color)
{
    scr_info_t lcd_info;
    g_lcd.get_info(&lcd_info);
    uint16_t *buffer = (uint16_t *)malloc(lcd_info.width * sizeof(uint16_t));
    if (NULL == buffer)
    {
        // for (size_t y = 0; y < lcd_info.height; y++)
        // {
        //     for (size_t x = 0; x < lcd_info.width; x++)
        //     {
        //         g_lcd.draw_pixel(x, y, color);
        //     }
        // }
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
    }
    else
    {
        for (size_t i = 0; i < lcd_info.width; i++)
        {
            buffer[i] = color;
        }

        for (int y = 0; y < lcd_info.height; y++)
        {
            xSemaphoreTake(xMutexLCD, portMAX_DELAY);
            g_lcd.draw_bitmap(0, y, lcd_info.width, 1, buffer);
            xSemaphoreGive(xMutexLCD);
        }

        free(buffer);
    }
}

// static esp_lcd_panel_handle_t panel_handle = NULL;

// void app_lcd_draw_wallpaper()
// {
//     if (panel_handle == NULL)
//     {
//         ESP_LOGE(TAG, "The LCD has not been initialized");
//         return;
//     }
//     uint16_t *pixels = (uint16_t *)heap_caps_malloc(MAX_LCD_LINE * logo_en_240x240_lcd_width * sizeof(uint16_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
//     uint16_t *image_ptr = logo_en_240x240_lcd;
//     if (NULL == pixels)
//     {
//         ESP_LOGE(TAG, "Memory for bitmap is not enough");
//         return;
//     }
//     // xSemaphoreTake(xMutexLCD, portMAX_DELAY);
//     for (int i = 0; i < (logo_en_240x240_lcd_height / MAX_LCD_LINE); ++i)
//     {
//         memcpy(pixels, image_ptr, MAX_LCD_LINE * logo_en_240x240_lcd_width * sizeof(uint16_t));
//         image_ptr += (MAX_LCD_LINE * logo_en_240x240_lcd_width);
//         esp_lcd_panel_draw_bitmap(panel_handle, 0, i*MAX_LCD_LINE, logo_en_240x240_lcd_width, (i + 1)*MAX_LCD_LINE, pixels);
//     }
//     // esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, logo_en_240x240_lcd_width, logo_en_240x240_lcd_height, logo_en_240x240_lcd);
//     // xSemaphoreGive(xMutexLCD);
//     heap_caps_free(pixels);
// }

// void app_lcd_set_color(int color)
// {
//     if (panel_handle == NULL)
//     {
//         ESP_LOGE(TAG, "The LCD has not been initialized");
//         return;
//     }
//     uint16_t *buffer = (uint16_t *)malloc(MAX_LCD_LINE * BOARD_LCD_H_RES * sizeof(uint16_t));
//     if (NULL == buffer)
//     {
//         ESP_LOGE(TAG, "Memory for bitmap is not enough");
//     }
//     else
//     {
//         for (size_t i = 0; i < MAX_LCD_LINE * BOARD_LCD_H_RES; i++)
//         {
//             buffer[i] = color;
//         }

//         // xSemaphoreTake(xMutexLCD, portMAX_DELAY);
//         for (int y = 0; y < (BOARD_LCD_V_RES / MAX_LCD_LINE); ++y)
//         {
//             esp_lcd_panel_draw_bitmap(panel_handle, 0, y * MAX_LCD_LINE, BOARD_LCD_H_RES, (y + 1)*MAX_LCD_LINE, buffer);
//         }
//         // xSemaphoreGive(xMutexLCD);

//         free(buffer);
//     }
// }

// static void task_process_handler(void *arg)
// {
//     camera_fb_t *frame = NULL;

//     while (true)
//     {
//         if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
//         {
//             uint16_t *ptr = (uint16_t *)frame->buf;
//             // xSemaphoreTake(xMutexLCD, portMAX_DELAY);
//             for (int i = 0; i < (BOARD_LCD_V_RES / MAX_LCD_LINE); ++i)
//             {
//                 esp_lcd_panel_draw_bitmap(panel_handle, 0, i * MAX_LCD_LINE, BOARD_LCD_H_RES, (i + 1) * MAX_LCD_LINE, ptr);
//                 ptr += (BOARD_LCD_H_RES * MAX_LCD_LINE);
//             }
//             // esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, BOARD_LCD_H_RES, BOARD_LCD_V_RES, (uint16_t *)frame->buf);
//             // xSemaphoreGive(xMutexLCD);

//             if (xQueueFrameO)
//             {
//                 xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
//             }
//             else if (gReturnFB)
//             {
//                 esp_camera_fb_return(frame);
//             }
//             else
//             {
//                 free(frame);
//             }
//         }
//     }
// }

// esp_err_t who_lcd_init()
// {
//     esp_err_t ret;
//     gpio_config_t bk_gpio_config = {
//         .mode = GPIO_MODE_OUTPUT,
//         .pin_bit_mask = 1ULL << BOARD_LCD_BL};
//     ret = gpio_config(&bk_gpio_config);
//     if (ESP_OK != ret)
//     {
//         ESP_LOGE(TAG, "GPIO config failed");
//         return  ret;
//     }

//     spi_bus_config_t buscfg = {
//         .sclk_io_num = BOARD_LCD_SCK,
//         .mosi_io_num = BOARD_LCD_MOSI,
//         .miso_io_num = -1,
//         .quadwp_io_num = -1,
//         .quadhd_io_num = -1,
//         .max_transfer_sz = MAX_LCD_LINE * BOARD_LCD_H_RES * 2 + 8};
//     ret = spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
//     if (ESP_OK != ret)
//     {
//         ESP_LOGE(TAG, "SPI initialize failed");
//         return  ret;
//     }

//     esp_lcd_panel_io_handle_t io_handle = NULL;
//     esp_lcd_panel_io_spi_config_t io_config = {
//         .dc_gpio_num = BOARD_LCD_DC,
//         .cs_gpio_num = BOARD_LCD_CS,
//         .pclk_hz = BOARD_LCD_PIXEL_CLOCK_HZ,
//         .lcd_cmd_bits = BOARD_LCD_CMD_BITS,
//         .lcd_param_bits = BOARD_LCD_PARAM_BITS,
//         .spi_mode = 0,
//         .trans_queue_depth = 2,
//         // .on_color_trans_done = example_notify_lvgl_flush_ready,
//         // .user_ctx = &disp_drv,
//     };
//     ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle);
//     if (ESP_OK != ret)
//     {
//         ESP_LOGE(TAG, "LCD new panel io spi failed");
//         return  ret;
//     }

//     ESP_LOGI(TAG, "Install LCD driver of st7789");
//     // esp_lcd_panel_handle_t panel_handle = NULL;
//     esp_lcd_panel_dev_config_t panel_config = {
//         .reset_gpio_num = BOARD_LCD_RST,
//         .color_space = ESP_LCD_COLOR_SPACE_RGB,
//         .bits_per_pixel = 16,
//     };
//     ret = esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
//     if (ESP_OK != ret)
//     {
//         ESP_LOGE(TAG, "LCD new panel st7789 failed");
//         return  ret;
//     }

//     esp_lcd_panel_reset(panel_handle);
//     esp_lcd_panel_init(panel_handle);
//     esp_lcd_panel_invert_color(panel_handle, true);

//     ESP_LOGI(TAG, "Turn on LCD backlight");
//     gpio_set_level(BOARD_LCD_BL, BOARD_LCD_BK_LIGHT_ON_LEVEL);
//     return ESP_OK;
// }

// esp_err_t register_lcd(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb)
// {
//     // xMutexLCD = xSemaphoreCreateMutex();
//     esp_err_t ret = who_lcd_init();
//     // ESP_LOGI(TAG, "Screen name:%s | width:%d | height:%d", g_lcd_info.name, g_lcd_info.width, g_lcd_info.height);

//     // app_lcd_set_color(0x000000);
//     vTaskDelay(pdMS_TO_TICKS(200));
//     app_lcd_draw_wallpaper();
//     vTaskDelay(pdMS_TO_TICKS(1000));
//     app_lcd_set_color(0x000000);
//     vTaskDelay(pdMS_TO_TICKS(200));

//     xQueueFrameI = frame_i;
//     xQueueFrameO = frame_o;
//     gReturnFB = return_fb;
//     xTaskCreatePinnedToCore(task_process_handler, TAG, 4 * 1024, NULL, 5, NULL, 0);

//     return ret;
// }
