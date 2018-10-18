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
#include "lwip/api.h"
#include "iot_lcd.h"
#include "iot_wifi_conn.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "freertos/event_groups.h"
#include "lcd_image.h"

#define PROGMEM
#include "FreeSans9pt7b.h"
#include "unity.h"

static const char* TAG = "LCD_REFRESH";
static CEspLcd* tft = NULL;

extern "C" void app_lcd_task(void *pvParameters)
{
    uint8_t i = 0;
    uint32_t time = 0;
    time = xTaskGetTickCount();

    while(1) {
        if((xTaskGetTickCount() - time) > 1000 / portTICK_RATE_MS ) {
           ESP_LOGI(TAG,"refresh %d fps", i);
           time = xTaskGetTickCount();
           i = 0;
        }
        i++;
        tft->drawBitmap(0, 0, i > 150 ? (uint16_t *)pic1_320_240 : (uint16_t *)Status_320_240, 320, 240);
    }
}

extern "C" void app_lcd_init()
{
    lcd_conf_t lcd_pins = {
        .lcd_model = LCD_MOD_AUTO_DET,
        .pin_num_miso = GPIO_NUM_25,
        .pin_num_mosi = GPIO_NUM_23,
        .pin_num_clk  = GPIO_NUM_19,
        .pin_num_cs   = GPIO_NUM_22,
        .pin_num_dc   = GPIO_NUM_21,
        .pin_num_rst  = GPIO_NUM_18,
        .pin_num_bckl = GPIO_NUM_5,
        .clk_freq = 80 * 1000 * 1000,
        .rst_active_level = 0,
        .bckl_active_level = 0,
        .spi_host = HSPI_HOST,
        .init_spi_bus = true,
    };

    /*Initialize SPI Handler*/
    if (tft == NULL) {
        tft = new CEspLcd(&lcd_pins);
    }

    /*screen initialize*/
    tft->invertDisplay(false);
    tft->setRotation(1);
    tft->fillScreen(COLOR_GREEN);
}

TEST_CASE("LCD refresh test", "[lcd_refresh][iot]")
{
    app_lcd_init();

    vTaskDelay(500 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "Free heap: %u", xPortGetFreeHeapSize());

    ESP_LOGI(TAG, "get free size of 32BIT heap : %d\n",
            heap_caps_get_free_size(MALLOC_CAP_32BIT));
    ESP_LOGD(TAG, "Starting app_lcd_task...");
    xTaskCreate(&app_lcd_task, "app_lcd_task", 4096, NULL, 4, NULL);

}
