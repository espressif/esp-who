#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_camera.h"
#include "driver/uart.h"

#define ECHO_TEST_TXD  (1)
#define ECHO_TEST_RXD  (3)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)
#define BUF_SIZE (1024)

static char *GET_FLAG = "get";
static char *START = "start";

void task_output(void *pvParameters)
{
    uint64_t start_time;
    uint64_t end_time;
    uint32_t fb_len;
    camera_fb_t *fb;

    uart_config_t uart_config = 
    {
        .baud_rate = 2000000,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    // uart_set_pin(UART_NUM_0, 14, 15, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    //send buffer
    uint8_t *cache = (uint8_t *) malloc(BUF_SIZE * 15);
    //add start flag.
    memcpy(cache, START, 5);

    do
    {   
        start_time = esp_timer_get_time();
        fb = esp_camera_fb_get();
        if (fb == NULL)
        {
            continue;
        }
        end_time = esp_timer_get_time();

        start_time = (end_time - start_time) / 1000;

        fb_len = fb->len;

        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 100 / portTICK_RATE_MS);
        if (len > 0) 
        {
            if (strncmp((char *)data, GET_FLAG, 3) == 0) 
            {
                memcpy(cache + 5, &fb_len, 4);
                memcpy(cache + 5 + 4, fb->buf, fb_len);
                uart_write_bytes(UART_NUM_1, (char *)cache, 5 + 4 + fb_len);
            }
        }
        esp_camera_fb_return(fb);
    } while (1);
   
}

void app_uart_image_main()
{
    xTaskCreatePinnedToCore(task_output, "uart_process", 4 * 1024, NULL, 5, NULL, 1);
}