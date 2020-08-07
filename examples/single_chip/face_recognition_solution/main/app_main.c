/* ESPRESSIF MIT License
 * 
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 * 
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
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
 */

#include "app_main.h"
#include "esp_partition.h"

void gpio_led_init()
{
    gpio_config_t gpio_conf;
    gpio_conf.mode = GPIO_MODE_OUTPUT;
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.pin_bit_mask = 1LL << GPIO_LED_RED;
    gpio_config(&gpio_conf);
    gpio_conf.pin_bit_mask = 1LL << GPIO_LED_WHITE;
    gpio_config(&gpio_conf);

}

void led_task(void *arg)
{
    while(1)
    {
        switch (g_state)
        {
            case WAIT_FOR_WAKEUP:
                gpio_set_level(GPIO_LED_RED, 1);
                gpio_set_level(GPIO_LED_WHITE, 0);
                break;

            case WAIT_FOR_CONNECT:
                gpio_set_level(GPIO_LED_WHITE, 0);
                gpio_set_level(GPIO_LED_RED, 1);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                gpio_set_level(GPIO_LED_RED, 0);
                break;

            case START_DETECT:
            case START_RECOGNITION:
                gpio_set_level(GPIO_LED_WHITE, 1);
                gpio_set_level(GPIO_LED_RED, 0);
                break;

            case START_ENROLL:
                gpio_set_level(GPIO_LED_WHITE, 1);
                gpio_set_level(GPIO_LED_RED, 1);
                break;

            case START_DELETE:
                gpio_set_level(GPIO_LED_WHITE, 1);
                for (int i = 0; i < 3; i++)
                {
                    gpio_set_level(GPIO_LED_RED, 1);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
                    gpio_set_level(GPIO_LED_RED, 0);
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                }
                break;

            default:
                gpio_set_level(GPIO_LED_WHITE, 1);
                gpio_set_level(GPIO_LED_RED, 0);
                break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

en_fsm_state g_state = WAIT_FOR_WAKEUP;
int g_is_enrolling = 0;
int g_is_deleting = 0;

void app_main()
{
    gpio_led_init();
    app_speech_wakeup_init();

    xTaskCreatePinnedToCore(&led_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL, 0);

    g_state = WAIT_FOR_WAKEUP;

    vTaskDelay(30 / portTICK_PERIOD_MS);
    ESP_LOGI("esp-eye", "Please say 'Hi LeXin' to the board");
    ESP_LOGI("esp-eye", "Version "VERSION);
    while (g_state == WAIT_FOR_WAKEUP)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    app_wifi_init();
    app_camera_init();
    app_httpserver_init();
    ESP_LOGI("esp-eye", "Version "VERSION" success");
}
