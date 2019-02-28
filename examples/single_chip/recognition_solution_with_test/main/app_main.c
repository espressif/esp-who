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
#include <string.h>


en_fsm_state_test g_state_test = LED_TEST;
uint8_t led_pass = 0;
uint8_t mic_pass = 0;
uint8_t con_recv_flag = 0;
char *line = NULL;
TaskHandle_t consoleHandle_t;

static void initialize_console()
{
    /* Disable buffering on stdin and stdout */
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install(CONFIG_CONSOLE_UART_NUM,
                                         256, 0, 0, NULL, 0) );

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONFIG_CONSOLE_UART_NUM);

    /* Initialize the console */
    esp_console_config_t console_config = {
        .max_cmdline_args = 32,
        .max_cmdline_length = 256,
#if CONFIG_LOG_COLORS
        .hint_color = atoi(LOG_COLOR_CYAN)
#endif
    };
    ESP_ERROR_CHECK( esp_console_init(&console_config) );

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within
     * single line.
     */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback *) &esp_console_get_hint);

    /* Set command history size */
    linenoiseHistorySetMaxLen(100);
}


void console_task(void *arg)
{
    initialize_console();
    const char *prompt = LOG_COLOR_I "ESP_EYE> " LOG_RESET_COLOR;
    int probe_status = linenoiseProbe();
    if (probe_status) { /* zero indicates success */
        printf("\n"
               "Your terminal application does not support escape sequences.\n"
               "Line editing and history features are disabled.\n"
               "On Windows, try using Putty instead.\n");
        linenoiseSetDumbMode(1);
#if CONFIG_LOG_COLORS
        /* Since the terminal doesn't support escape sequences,
         * don't use color codes in the prompt.
         */
        prompt = "ESP_EYE> ";
#endif //CONFIG_LOG_COLORS
    }
    line = linenoise(prompt);
    while(1){
        con_recv_flag = 1;
        vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}

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
    printf("\n");
    ESP_LOGI("esp-eye", "Version "VERSION);
    printf("\n");

    xTaskCreatePinnedToCore(&console_task, "console", 2*1024, NULL, 5, &consoleHandle_t, 1);
    int64_t start_time = esp_timer_get_time();
    while(con_recv_flag == 0){
        int64_t end_time = esp_timer_get_time();
        if(((end_time-start_time)/1000)<5000){
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }else{
            break;
        }
    }
    vTaskDelete(consoleHandle_t);
    printf("\n");

    char sig[80] = "FACTORY_TEST";
    if((line != NULL)&&(strcmp(sig,line)==0)){
        ESP_LOGE("MODE", "PERIPHERAL TEST\n");
        app_wifi_test_init();
        gpio_led_test_init();
        app_speech_wakeup_test_init();
        app_camera_init();
        app_facenet_test_main();
        ESP_LOGI("PERIPHERAL TEST", "Start !\n");
        while(1){
            if(g_state_test == TEST_PASS){
                ESP_LOGI("PERIPHERAL TEST", "PASS !");
                break;
            }
            if(g_state_test == TEST_FAIL){
                ESP_LOGE("PERIPHERAL TEST", "FAIL !");
                break;
            }
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }  
    }else{
        gpio_led_init();
        app_speech_wakeup_init();

        xTaskCreatePinnedToCore(&led_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL, 0);

        g_state = WAIT_FOR_WAKEUP;

        vTaskDelay(30 / portTICK_PERIOD_MS);
        ESP_LOGI("esp-eye", "Please say 'Hi LeXin' to the board");
        while (g_state == WAIT_FOR_WAKEUP)
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        app_wifi_init();
        app_camera_init();
        app_httpserver_init();
        ESP_LOGI("esp-eye", "Version "VERSION" success");
    }
}
