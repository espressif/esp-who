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
#include "wechat_blufi.h"

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

            default:
                gpio_set_level(GPIO_LED_WHITE, 1);
                gpio_set_level(GPIO_LED_RED, 0);
                break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

en_fsm_state g_state = WAIT_FOR_WAKEUP;

static void restart_count_erase_timercb(void *timer)
{
    if (!xTimerStop(timer, portMAX_DELAY)) {
        ESP_LOGD("esp-eye", "xTimerStop timer: %p", timer);
    }

    if (!xTimerDelete(timer, portMAX_DELAY)) {
        ESP_LOGD("esp-eye", "xTimerDelete timer: %p", timer);
    }
    
    wifi_info_erase(NVS_KEY_RESTART_COUNT);

    ESP_LOGI("esp-eye", "Erase restart count");
}

static int restart_count_get()
{
    esp_err_t ret          = ESP_OK;
    TimerHandle_t timer    = NULL;
    uint32_t restart_count = 0;

    /**< If the device restarts within the instruction time,
         the event_mdoe value will be incremented by one */
    load_info_nvs(NVS_KEY_RESTART_COUNT, &restart_count, sizeof(uint32_t));
    restart_count++;
    ret = save_info_nvs(NVS_KEY_RESTART_COUNT, &restart_count, sizeof(uint32_t));
    if (ret != ESP_OK) {
        ESP_LOGE("size_info_nvs", "size_info_nvs\n");
        return ret;
    }

    timer = xTimerCreate("restart_count_erase", RESTART_TIMEOUT_MS / portTICK_RATE_MS,
                         false, NULL, restart_count_erase_timercb);

    xTimerStart(timer, 0);

    return restart_count;
}


void printTask(void *arg)
{
    #define BUF_SIZE 1 * 1024
    char *tasklist = malloc(BUF_SIZE);
    while (1) {
            memset(tasklist, 0, BUF_SIZE);
            vTaskGetRunTimeStats(tasklist);
            printf("Running tasks CPU usage: \n %s\r\n", (char *)tasklist);
            printf("RAM size: %dKB, with PSRAM: %dKB\n", heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL) / 1024, heap_caps_get_free_size(MALLOC_CAP_8BIT));
            vTaskDelay(5000 / portTICK_RATE_MS);
        }

    free(tasklist);
}

void app_facenet_main();
void app_main()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Compatible V3.1. NVS format has been changed between v3.1 and v3.2 to support longer blob values.
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    if (restart_count_get() > 3) {
        ESP_LOGI("esp-eye", "Erase information saved in flash");
        wifi_info_erase(USERDATANAMESPACE);
    }

    gpio_led_init();
    app_camera_init();

    xTaskCreatePinnedToCore(&led_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL, 0);

    g_state = WAIT_FOR_CONNECT;

    ESP_LOGI("esp-eye", "Version "VERSION);

    // app_wifi_init();

    // wifi init and blufi 
    blufi_main();
    
    // wait for network connected    
    wait_net_connected();

    app_facenet_main();
    app_httpserver_init();
    printf("Mem availabe after: %d\n", heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL));
    ESP_LOGI("esp-eye", "Version "VERSION" success");
    //xTaskCreatePinnedToCore(&printTask, "printTask", 2*1024, NULL, 5, NULL, 1);
}
