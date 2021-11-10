#include "test_logic.h"
#include "who_lcd.h"

static const char *TAG = "LCD";

static QueueHandle_t queue_test_result = NULL;
static QueueHandle_t queue_button = NULL;
static QueueHandle_t *queues_tests = NULL;

static scr_driver_t g_lcd;
static scr_info_t g_lcd_info;

#define RGB565_MASK_RED 0x00F8
#define RGB565_MASK_GREEN 0xE007
#define RGB565_MASK_BLUE 0x1F00

static void test_lcd_set_color(int color)
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

static esp_err_t lcd_init()
{
    static bool initialized = false;
    if (initialized)
    {
        return ESP_OK;
    }
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
        ESP_LOGE(TAG, "screen find failed");
        return ret;
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
        ESP_LOGE(TAG, "screen initialize failed");
        return ret;
    }

    g_lcd.get_info(&g_lcd_info);
    ESP_LOGI(TAG, "Screen name:%s | width:%d | height:%d", g_lcd_info.name, g_lcd_info.width, g_lcd_info.height);
    initialized = true;

    return ESP_OK;

    // vTaskDelay(pdMS_TO_TICKS(500));
    // app_lcd_draw_wallpaper();
    // vTaskDelay(pdMS_TO_TICKS(500));
}

static void lcd_test_task(void *arg)
{
    while (1)
    {
        en_test_state g_state_test = TEST_IDLE;
        int button_pressed = 0;
        xQueueReceive(queues_tests[TEST_LCD], &g_state_test, portMAX_DELAY);
        bool lcd_pass = false;
        if (g_state_test == TEST_LCD)
        {
            ESP_LOGI("ESP32-S3-EYE", "--------------- Enter LCD Test ---------------\n");

            esp_err_t ret = lcd_init();
            
            if(ESP_OK != ret)
            {
                lcd_pass = false;
                ESP_LOGE(TAG, "--------------- LCD Test FAIL ---------------\n");
                xQueueSend(queue_test_result, &lcd_pass, portMAX_DELAY);
                continue;
            }

            ESP_LOGW(TAG, "Display Red \n");
            test_lcd_set_color(RGB565_MASK_RED);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            ESP_LOGW(TAG, "Display Green \n");
            test_lcd_set_color(RGB565_MASK_GREEN);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            ESP_LOGW(TAG, "Display Blue \n");
            test_lcd_set_color(RGB565_MASK_BLUE);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            ESP_LOGW(TAG, "Display White \n");
            test_lcd_set_color(0xFFFF);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            xQueueReceive(queue_button, &button_pressed, 60 / portTICK_PERIOD_MS);
            button_pressed = 0;
            ets_printf("Please check if the LCD works\n");
            ets_printf("Press UP+ if works, Press MENU if failed\n");
            xQueueReceive(queue_button, &button_pressed, 5000 / portTICK_PERIOD_MS);
            if (button_pressed == 3)
            {
                lcd_pass = true;
                ESP_LOGI(TAG, "--------------- LCD Test PASS ---------------\n");
                xQueueSend(queue_test_result, &lcd_pass, portMAX_DELAY);
            }
            else
            {
                lcd_pass = false;
                ESP_LOGE(TAG, "--------------- LCD Test FAIL ---------------\n");
                xQueueSend(queue_test_result, &lcd_pass, portMAX_DELAY);
            }
            test_lcd_set_color(0x0000);
        }
        else
        {
            ESP_LOGE(TAG, "--------------- Receive Test Code Error ---------------\n");
            bool result = false;
            xQueueSend(queue_test_result, &result, portMAX_DELAY);
        }
    }
}

void register_lcd_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue, const QueueHandle_t key_state_o)
{
    queue_test_result = result_queue;
    queue_button = key_state_o;
    queues_tests = test_queues;

    xTaskCreatePinnedToCore(lcd_test_task, "lcd_test_task", 3 * 1024, NULL, 5, NULL, 1);
}