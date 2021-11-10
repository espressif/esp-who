#include "test_logic.h"

#define GPIO_LED GPIO_NUM_3

static const char *TAG = "LED";

static QueueHandle_t queue_test_result = NULL;
static QueueHandle_t queue_button = NULL;
static QueueHandle_t *queues_tests = NULL;

static void led_test_task(void *arg)
{
    while (1)
    {
        en_test_state g_state_test = TEST_IDLE;
        int button_pressed = 0;
        xQueueReceive(queues_tests[TEST_LED], &g_state_test, portMAX_DELAY);
        bool led_pass = false;
        if (g_state_test == TEST_LED)
        {
            ESP_LOGI("ESP32-S3-EYE", "--------------- Enter LED Test ---------------\n");
            for (int i = 0; i < 3; i++)
            {
                gpio_set_level(GPIO_LED, 1);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                gpio_set_level(GPIO_LED, 0);
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
            xQueueReceive(queue_button, &button_pressed, 60 / portTICK_PERIOD_MS);
            button_pressed = 0;
            ets_printf("Please check if the LED works\n");
            ets_printf("Press UP+ if works, Press MENU if failed\n");
            xQueueReceive(queue_button, &button_pressed, 5000 / portTICK_PERIOD_MS);
            if (button_pressed == 3)
            {
                led_pass = true;
                ESP_LOGI(TAG, "--------------- LED Test PASS ---------------\n");
                xQueueSend(queue_test_result, &led_pass, portMAX_DELAY);
            }
            else
            {
                led_pass = false;
                ESP_LOGE(TAG, "--------------- LED Test FAIL ---------------\n");
                xQueueSend(queue_test_result, &led_pass, portMAX_DELAY);
            }
        }
        else
        {
            ESP_LOGE(TAG, "--------------- Receive Test Code Error ---------------\n");
            bool result = false;
            xQueueSend(queue_test_result, &result, portMAX_DELAY);
        }
    }
}

void register_led_test(QueueHandle_t *test_queues, const QueueHandle_t result_queue, const QueueHandle_t key_state_o)
{
    queue_test_result = result_queue;
    queue_button = key_state_o;
    queues_tests = test_queues;

    gpio_config_t gpio_conf;
    gpio_conf.mode = GPIO_MODE_OUTPUT_OD;
    gpio_conf.pull_up_en = 1;

    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.pin_bit_mask = 1LL << GPIO_LED;
    gpio_config(&gpio_conf);
    xTaskCreatePinnedToCore(&led_test_task, "led_test_task", 3 * 1024, NULL, 5, NULL, 0);
}