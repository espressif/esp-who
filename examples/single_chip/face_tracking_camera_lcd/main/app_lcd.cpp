/*
  * ESPRESSIF MIT License
  *
  * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
  *
  * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
  * it is free of charge, to any person obtaining a copy of this software and associated
  * documentation files (the "Software"), to deal in the Software without restriction, including
  * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
  * to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all copies or
  * substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  */
#include "app_lcd.h"
#include "app_facenet.h"
#include "fb_gfx.h"

static const char *TAG = "app_lcd";

TaskHandle_t gpst_output_task = NULL;
QueueHandle_t gpst_output_queue = NULL;
extern QueueHandle_t gpst_image_matrix_queue;
extern QueueHandle_t gpst_recog_output_queue;
CEspLcd *tft = NULL;

static void rgb_print(uint8_t *item, uint32_t color, const char *str)
{
    fb_data_t fb;
    fb.width = 240;
    fb.height = 240;
    fb.data = item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    fb_gfx_print(&fb, (fb.width - (strlen(str) * 14)) / 2, 10, color, str);
}

static int rgb_printf(uint8_t *item, uint32_t color, const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            return 0;
        }
    }
    vsnprintf(temp, len + 1, format, arg);
    va_end(arg);
    rgb_print(item, color, temp);
    if (len > 64)
    {
        free(temp);
    }
    return len;
}

void app_lcd_task(void *pvParameters)
{
    int64_t time1[5] = {0};
    const int x = LCD_WIDTH - DISPLAY_IMAGE_WIDTH;
    const int y = LCD_HEIGHT - DISPLAY_IMAGE_HEIGHT;
    uint8_t *img;
    uint16_t *display_buffer = (uint16_t *)calloc(1, DISPLAY_IMAGE_WIDTH * DISPLAY_IMAGE_HEIGHT * sizeof(uint16_t));
    while (1)
    {
        time1[0] = esp_timer_get_time();
        xQueueReceive(gpst_output_queue, &img, portMAX_DELAY);
        time1[1] = esp_timer_get_time();
#if FACE_RECOGNITION
        int id = -2;
        if (xQueueReceive(gpst_recog_output_queue, &id, 0) == pdTRUE)
        {
            if (id == 0)
                rgb_printf(img, COLOR_RED, "Hello Subject 0");
            else
                rgb_printf(img, COLOR_GREEN, "WHO? %d", id);
        }
#endif
        transform_output_image_adjustable(display_buffer, img, DISPLAY_IMAGE_WIDTH, DISPLAY_IMAGE_HEIGHT, LCD_WIDTH, LCD_HEIGHT);

        time1[2] = esp_timer_get_time();
        char str_buf[50];
        tft->drawBitmap(0, 0, display_buffer, LCD_WIDTH, LCD_HEIGHT);
        time1[3] = esp_timer_get_time();
        if (xQueueSend(gpst_image_matrix_queue, &img, 0) != pdTRUE)
        {
            ESP_LOGE("Output", "Send fail");
        }
        time1[4] = esp_timer_get_time();

#if 0
        ESP_LOGW(TAG, "lcd time consumed: wait %lldms, transform %lldms, output %lldms, send %lldms, all %lld\n",
                (time1[1] - time1[0]) / 1000,
                (time1[2] - time1[1]) / 1000,
                (time1[3] - time1[2]) / 1000,
                (time1[4] - time1[3]) / 1000,
                (time1[4] - time1[0]) / 1000
                );
#endif
    }
    free(display_buffer);
}

void app_lcd_init()
{
    lcd_conf_t lcd_pins;
    lcd_pins.lcd_model = LCD_MOD_ST7789;
    lcd_pins.pin_num_cs = LCD_CS_GPIO;
    lcd_pins.pin_num_rst = LCD_RESET_GPIO;
    lcd_pins.pin_num_dc = LCD_DC_GPIO;
    lcd_pins.pin_num_mosi = LCD_MOSI_GPIO;
    lcd_pins.pin_num_clk = LCD_CLK_GPIO;
    lcd_pins.pin_num_bckl = LCD_LIGHT_GPIO;
    lcd_pins.pin_num_miso = LCD_MISO_GPIO;
    lcd_pins.clk_freq = 26 * 1000 * 1000;
    lcd_pins.rst_active_level = 0;
    lcd_pins.bckl_active_level = 1;
    lcd_pins.spi_host = HSPI_HOST;
    lcd_pins.init_spi_bus = true;

    /*Initialize SPI Handler*/
    if (tft == NULL)
    {
        tft = new CEspLcd(&lcd_pins, LCD_HEIGHT, LCD_WIDTH);
    }

    /*screen initialize*/
    tft->invertDisplay(true);
#if CONFIG_M5STACK
    tft->setRotation(1);
#else
    tft->setRotation(0);
#endif
    tft->fillScreen(COLOR_ESP_BKGD);
    tft->drawBitmap(0, 0, esp_logo, 137, 26);
}

extern "C" void app_lcd_main()
{
    app_lcd_init();
    //tft->setTextSize(2);
    //tft->drawString("Face detection", 0, 100);
    //tft->drawString("3", 116, 120);
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
    //tft->fillRect(0, 120, 240, 20, COLOR_ESP_BKGD);
    //tft->drawString("2", 116, 120);
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
    //tft->fillRect(0, 120, 240, 20, COLOR_ESP_BKGD);
    //tft->drawString("1", 116, 120);
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
    //tft->setTextSize(1);

    gpst_output_queue = xQueueCreate(LCD_CACHE_NUM, sizeof(void *));

    xTaskCreatePinnedToCore(app_lcd_task, "lcd_task", 4096, NULL, 7, &gpst_output_task, 0);
}
