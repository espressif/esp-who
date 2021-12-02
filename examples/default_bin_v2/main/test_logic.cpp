#include "esp_log.h"
#include "driver/uart.h"
#include "esp_vfs_fat.h"
#include "test_logic.h"
#include "who_button.h"
#include "fb_gfx.h"
#include <string.h>
#include "dl_tool.hpp"
#include "logo_en_240x240_lcd.h"
#include "esp_camera.h"
#include "who_lcd.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

#define CONSOLE_PROMPT_MAX_LEN (32)
#define CONSOLE_PATH_MAX_LEN (ESP_VFS_PATH_MAX)
#define CONFIG_EXAMPLE_STORE_HISTORY 0

static QueueHandle_t *queues_tests = NULL;
static QueueHandle_t queue_test_flag = NULL;
static QueueHandle_t queue_test_result = NULL;

static scr_driver_t g_lcd;
static scr_info_t g_lcd_info;

#define RGB565_MASK_RED 0x00F8
#define RGB565_MASK_GREEN 0xE007
#define RGB565_MASK_BLUE 0x1F00
#define RGB565_MASK_YELLOW 0xE0FF

#define RGB565_RED 0xF800
#define RGB565_GREEN 0x07E0
#define RGB565_BLUE 0x001F
#define RGB565_YELLOW 0xFFE0
#define RGB565_WHITE 0xFFFF

static void test_lcd_draw_wallpaper()
{
    scr_info_t lcd_info;
    g_lcd.get_info(&lcd_info);

    uint16_t *pixels = (uint16_t *)heap_caps_malloc((logo_en_240x240_lcd_width * logo_en_240x240_lcd_height) * sizeof(uint16_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    if (NULL == pixels)
    {
        ESP_LOGE("LCD", "Memory for bitmap is not enough");
        return;
    }
    memcpy(pixels, logo_en_240x240_lcd, (logo_en_240x240_lcd_width * logo_en_240x240_lcd_height) * sizeof(uint16_t));
    g_lcd.draw_bitmap(0, 0, logo_en_240x240_lcd_width, logo_en_240x240_lcd_height, (uint16_t *)pixels);
    heap_caps_free(pixels);
}

static void test_lcd_set_color(int color)
{
    scr_info_t lcd_info;
    g_lcd.get_info(&lcd_info);
    uint16_t *buffer = (uint16_t *)malloc(lcd_info.width * sizeof(uint16_t));
    if (NULL == buffer)
    {
        ESP_LOGE("LCD", "Memory for bitmap is not enough");
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

static esp_err_t lcd_init()
{
    static bool initialized = false;
    if (initialized)
    {
        return ESP_OK;
    }
    spi_config_t bus_conf = {
        .miso_io_num = (gpio_num_t)BOARD_LCD_MISO,
        .mosi_io_num = (gpio_num_t)BOARD_LCD_MOSI,
        .sclk_io_num = (gpio_num_t)BOARD_LCD_SCK,
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
        ESP_LOGE("LCD", "screen find failed");
        return ret;
    }

    scr_controller_config_t lcd_cfg = {
        .interface_drv = iface_drv,
        .pin_num_rst = BOARD_LCD_RST,
        .pin_num_bckl = BOARD_LCD_BL,
        .rst_active_level = 0,
        .bckl_active_level = 0,
        .width = 240,
        .height = 240,
        .offset_hor = 0,
        .offset_ver = 0,
        .rotate = (scr_dir_t)0,
    };
    ret = g_lcd.init(&lcd_cfg);
    if (ESP_OK != ret)
    {
        ESP_LOGE("LCD", "screen initialize failed");
        return ret;
    }

    g_lcd.get_info(&g_lcd_info);
    ESP_LOGI("LCD", "Screen name:%s | width:%d | height:%d", g_lcd_info.name, g_lcd_info.width, g_lcd_info.height);
    initialized = true;

    // vTaskDelay(pdMS_TO_TICKS(100));
    test_lcd_draw_wallpaper();
    vTaskDelay(pdMS_TO_TICKS(200));
    return ESP_OK;
}

static inline int get_test_result(int *test_times, bool *test_reuslts, int num)
{
    for (int i = 0; i < num; ++i)
    {
        if (test_times[i])
        {
            if (!test_reuslts[i])
                return 0;
        }
        else
        {
            return -1;
        }
    }
    return 1;
}

static inline uint16_t get_display_char_color(int *test_times, bool *test_reuslts, int num, int index)
{
    if (index >= 0)
    {
        if (test_times[index] > 0)
        {
            if (test_reuslts[index])
                return RGB565_GREEN;
            else
                return RGB565_RED;
        }
        else
            return RGB565_BLUE;
    }
    else
    {
        int result = get_test_result(test_times, test_reuslts, num);
        return result < 0 ? RGB565_BLUE : (result > 0 ? RGB565_GREEN : RGB565_RED);
    }
}

static int line_gap = 240 / 8;
static int start_line = 5;

static vector<string> test_name = {"SDCard     ",
                                   "IMU        ",
                                   "Button     ",
                                   "LED        ",
                                   "Camera     ",
                                   "MIC        "};

static void draw_mac(camera_fb_t *image, int h, uint16_t bg_color, string mac)
{
    dl::tool::set_value((uint16_t *)image->buf + h * image->width, bg_color, 20 * image->width);
    int start_w = (image->width - (mac.size() * 14)) / 2;
    fb_gfx_print(image, start_w, start_line, 0xF810, mac.c_str());
    fb_gfx_print(image, start_w + 14 * 1, start_line, 0xF800, mac.c_str() + 1);
    fb_gfx_print(image, start_w + 14 * 2, start_line, 0xFC00, mac.c_str() + 2);
    fb_gfx_print(image, start_w + 14 * 3, start_line, 0xFFE0, mac.c_str() + 3);
    fb_gfx_print(image, start_w + 14 * 4, start_line, 0x87E0, mac.c_str() + 4);
    fb_gfx_print(image, start_w + 14 * 5, start_line, 0x07E0, mac.c_str() + 5);
    fb_gfx_print(image, start_w + 14 * 6, start_line, 0x07F0, mac.c_str() + 6);
    fb_gfx_print(image, start_w + 14 * 7, start_line, 0x07FF, mac.c_str() + 7);
    fb_gfx_print(image, start_w + 14 * 8, start_line, 0x041F, mac.c_str() + 8);
    fb_gfx_print(image, start_w + 14 * 9, start_line, 0x001F, mac.c_str() + 9);
    fb_gfx_print(image, start_w + 14 * 10, start_line, 0x801F, mac.c_str() + 10);
    fb_gfx_print(image, start_w + 14 * 11, start_line, 0xF81F, mac.c_str() + 11);
}

static void task_controller_handler(void *arg)
{
    en_test_state g_state_test = TEST_IDLE;
    int test_times[TEST_NUM] = {0};
    bool test_reuslts[TEST_NUM] = {false};
    int test_flag = KEY_SHORT_PRESS;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    uint16_t bg_color = 0x0000;
    uint8_t derived_mac_addr[6] = {0};
    esp_read_mac(derived_mac_addr, ESP_MAC_WIFI_STA);
    std::stringstream stream;
    for (int i = 0; i < 6; ++i)
    {
        stream << std::hex << std::setfill('0') << std::setw(2) << (int)derived_mac_addr[i];
    }
    string mac_name = stream.str();
    string file_name = "/sdcard/" + mac_name;

    while (true)
    {
        if (test_flag == KEY_SHORT_PRESS)
        {
            ESP_LOGI(board_version, "--------------- Test Start ---------------\n");
            esp_err_t ret = lcd_init();
            if (ret != ESP_OK)
            {
                ESP_LOGI(board_version, "*************** Test FAIL ***************\n");
                break;
            }

            uint16_t *lcd_buffer = (uint16_t *)heap_caps_malloc((240 * 240) * sizeof(uint16_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
            assert(lcd_buffer != NULL);
            dl::tool::set_value(lcd_buffer, bg_color, 240 * 240);
            camera_fb_t frame;
            frame.format = PIXFORMAT_RGB565;
            frame.buf = (uint8_t *)lcd_buffer;
            frame.height = 240;
            frame.width = 240;
            frame.len = (240 * 240) * sizeof(uint16_t);
            string str = mac_name;

            // fb_gfx_print(&frame, (frame.width - (strlen(str.c_str()) * 14)) / 2, start_line, RGB565_YELLOW, str.c_str());
            draw_mac(&frame, start_line, bg_color, str);
            for (int i = 0; i < TEST_NUM; ++i)
            {
                str = test_name[i] + (test_times[i] > 0 ? (test_reuslts[i] ? "PASS" : "FAIL") : "  ? ");
                fb_gfx_print(&frame, 15, start_line + line_gap * (i + 1), get_display_char_color(test_times, test_reuslts, TEST_NUM, i), str.c_str());
            }
            int result = get_test_result(test_times, test_reuslts, TEST_NUM);
            str = firmware_version;
            str += "    ?";
            fb_gfx_print(&frame, 15, start_line + line_gap * (TEST_NUM + 1), get_display_char_color(test_times, test_reuslts, TEST_NUM, -1), str.c_str());
            g_lcd.draw_bitmap(0, 0, frame.width, frame.height, lcd_buffer);
            vTaskDelay(100 / portTICK_PERIOD_MS);

            if (!test_reuslts[0])
            {
                g_state_test = (en_test_state)0;
                dl::tool::set_value(lcd_buffer + (start_line + line_gap * (0 + 1)) * 240, bg_color, 20 * 240);
                str = test_name[0] + "  ? *";
                fb_gfx_print(&frame, 15, start_line + line_gap * (0 + 1), RGB565_WHITE, str.c_str());
                g_lcd.draw_bitmap(0, 0, frame.width, frame.height, lcd_buffer);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                xQueueSend(queues_tests[0], &g_state_test, portMAX_DELAY);
                xQueueReceive(queue_test_result, &test_reuslts[0], portMAX_DELAY);
                test_times[0] += 1;
                dl::tool::set_value(lcd_buffer + (start_line + line_gap * (0 + 1)) * 240, bg_color, 20 * 240);
                str = test_name[0] + (test_times[0] > 0 ? (test_reuslts[0] ? "PASS" : "FAIL") : "  ? ");
                fb_gfx_print(&frame, 15, start_line + line_gap * (0 + 1), get_display_char_color(test_times, test_reuslts, TEST_NUM, 0), str.c_str());
                g_lcd.draw_bitmap(0, 0, frame.width, frame.height, lcd_buffer);
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }

            sdmmc_card_t *card = NULL;
            FILE *fp = NULL;

            if (test_reuslts[0])
            {
                static bool wirte_info = true;
                sd_card_mount("/sdcard", &card);
                fp = fopen(file_name.c_str(), "a+");
                if (wirte_info && fp)
                {
                    wirte_info = false;
                    str = "\nBoard:     ";
                    str += board_version;
                    str += "\nFirmware:  ";
                    str += firmware_version;
                    str += "\n";
                    fwrite(str.c_str(), sizeof(char), str.size(), fp);
                }
            }

            for (int i = 1; i < TEST_NUM; ++i)
            {
                if (test_reuslts[i])
                    continue;

                g_state_test = (en_test_state)i;
                dl::tool::set_value(lcd_buffer + (start_line + line_gap * (i + 1)) * 240, bg_color, 20 * 240);
                str = test_name[i] + "  ? *";
                fb_gfx_print(&frame, 15, start_line + line_gap * (i + 1), RGB565_WHITE, str.c_str());
                g_lcd.draw_bitmap(0, 0, frame.width, frame.height, lcd_buffer);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                xQueueSend(queues_tests[i], &g_state_test, portMAX_DELAY);
                xQueueReceive(queue_test_result, &test_reuslts[i], portMAX_DELAY);
                test_times[i] += 1;
                dl::tool::set_value(lcd_buffer + (start_line + line_gap * (i + 1)) * 240, bg_color, 20 * 240);
                str = test_name[i] + (test_times[i] > 0 ? (test_reuslts[i] ? "PASS" : "FAIL") : "  ? ");
                fb_gfx_print(&frame, 15, start_line + line_gap * (i + 1), get_display_char_color(test_times, test_reuslts, TEST_NUM, i), str.c_str());
                g_lcd.draw_bitmap(0, 0, frame.width, frame.height, lcd_buffer);

                if (fp)
                {
                    str = str + "\n";
                    fwrite(str.c_str(), sizeof(char), str.size(), fp);
                }
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }

            bool test_all_done = true;
            for (int i = 0; i < TEST_NUM; ++i)
            {
                test_all_done = test_all_done && (test_times[i] > 0);
            }
            if (test_all_done)
            {
                bool test_all_pass = true;
                for (int i = 0; i < TEST_NUM; ++i)
                {
                    test_all_pass = test_all_pass && test_reuslts[i];
                }
                if (test_all_pass)
                {
                    ESP_LOGI(board_version, "*************** Test PASS ***************\n");
                }
                else
                {
                    ESP_LOGE(board_version, "*************** Test FAIL ***************\n");
                }
            }

            dl::tool::set_value(lcd_buffer + (start_line + line_gap * (TEST_NUM + 1)) * 240, bg_color, 20 * 240);
            result = get_test_result(test_times, test_reuslts, TEST_NUM);
            str = firmware_version;
            str += result < 0 ? "    ?" : (result > 0 ? "  PASS" : "  FAIL");
            fb_gfx_print(&frame, 15, start_line + line_gap * (TEST_NUM + 1), get_display_char_color(test_times, test_reuslts, TEST_NUM, -1), str.c_str());
            g_lcd.draw_bitmap(0, 0, frame.width, frame.height, lcd_buffer);
            if (fp)
            {
                str = str + "\n\n";
                fwrite(str.c_str(), sizeof(char), str.size(), fp);
                fclose(fp);
            }
            if (card)
            {
                esp_vfs_fat_sdcard_unmount("/sdcard", card);
            }

            vTaskDelay(100 / portTICK_PERIOD_MS);
            free(lcd_buffer);
        }
        xQueueReceive(queue_test_flag, &test_flag, 10 / portTICK_PERIOD_MS);
        xQueueReceive(queue_test_flag, &test_flag, portMAX_DELAY);
    }
    ESP_LOGD(board_version, "The End");
    vTaskDelete(NULL);
}

void register_test_controller(QueueHandle_t *console_queues, const QueueHandle_t test_queue, const QueueHandle_t result_queue)
{
    queues_tests = console_queues;
    queue_test_result = result_queue;
    queue_test_flag = test_queue;
    xTaskCreatePinnedToCore(task_controller_handler, "test_console_process", 4 * 1024, NULL, 5, NULL, 1);
}
