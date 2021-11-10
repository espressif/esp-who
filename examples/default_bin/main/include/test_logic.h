#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        TEST_SD_CARD = 0,
        TEST_IMU,
        TEST_BUTTON,
        TEST_LED,
        TEST_LCD,
        TEST_CAMERA,
        TEST_MIC,
        TEST_IDLE,
    } en_test_state;

#define TEST_NUM 7

    extern uint8_t mac[6];

    void register_console(QueueHandle_t *console_queues, const QueueHandle_t test_queue, const QueueHandle_t result_queue, TaskHandle_t *console_task);
    void register_test_controller(QueueHandle_t *console_queues, const QueueHandle_t test_queue, const QueueHandle_t result_queue);
    void register_sd_card_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue);
    void register_imu_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue);
    void register_button_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue, const QueueHandle_t key_state_o);
    void register_led_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue, const QueueHandle_t key_state_o);
    void register_lcd_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue, const QueueHandle_t key_state_o);
    void register_camera_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue);
    void register_mic_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue);

    esp_err_t sd_card_mount(const char *mount_point, sdmmc_card_t **card);
    int FatfsComboWrite(const void *buffer, int size, int count, FILE *stream);
    void app_mic_test();

    static void print_memory(const char *tag, int delay_time_ms)
    {
        vTaskDelay(delay_time_ms / portTICK_PERIOD_MS);
        ets_printf("%s: Start free RAM size: %d, INTERNAL: %d, PSRAM: %d\n", tag, heap_caps_get_free_size(MALLOC_CAP_8BIT),
                   heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
                   heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM));
    }
#ifdef __cplusplus
}
#endif