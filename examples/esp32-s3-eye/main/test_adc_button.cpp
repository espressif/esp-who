#include "test_logic.h"
#include "driver/adc_common.h"
#include "esp_adc_cal.h"
#include "who_adc_button.h"

static QueueHandle_t gpio_evt_queue = NULL;
static QueueHandle_t adc_evt_queue = NULL;
static QueueHandle_t queue_test_result = NULL;
static QueueHandle_t *queues_tests = NULL;
static const char *TAG = "BUTTON";

bool boot_pass = false;
bool menu_pass = false;
bool play_pass = false;
bool up_pass = false;
bool dn_pass = false;
bool button_pass = false;

void test_adc_button()
{
    int io_num = -1;
    int adc_button_pressed = -1;
    en_test_state g_state_test = TEST_IDLE;

    xQueueReceive(queues_tests[TEST_BUTTON], &g_state_test, portMAX_DELAY);
    if (g_state_test == TEST_BUTTON)
    {
        ESP_LOGI(board_version, "--------------- Enter Button Test ---------------\n");

        // xQueueReceive(gpio_evt_queue, &io_num, 60 / portTICK_PERIOD_MS);
        // io_num = -1;
        // printf("Please press BOOT Button\n");
        // xQueueReceive(gpio_evt_queue, &io_num, 5000 / portTICK_PERIOD_MS);
        // if (io_num != -1)
        // {
        //     ESP_LOGW(TAG, "--------------- BOOT Button Test PASS ---------------\n");
        //     boot_pass = true;
        // }
        // else
        // {
        //     ESP_LOGE(TAG, "--------------- BOOT Button Test FAIL ---------------\n");
        //     boot_pass = false;
        // }

        xQueueReceive(adc_evt_queue, &adc_button_pressed, 60 / portTICK_PERIOD_MS);
        adc_button_pressed = -1;
        printf("Please press MENU Button\n");
        xQueueReceive(adc_evt_queue, &adc_button_pressed, 10000 / portTICK_PERIOD_MS);
        if (adc_button_pressed == MENU)
        {
            ESP_LOGW(TAG, "--------------- MENU Button Test PASS ---------------\n");
            menu_pass = true;
        }
        else
        {
            ESP_LOGE(TAG, "--------------- MENU Button Test FAIL ---------------\n");
            menu_pass = false;
            adc_button_pressed = -1;
        }

        printf("Please press PLAY Button\n");
        xQueueReceive(adc_evt_queue, &adc_button_pressed, 10000 / portTICK_PERIOD_MS);
        if (adc_button_pressed == PLAY)
        {
            ESP_LOGW(TAG, "--------------- PLAY Button Test PASS ---------------\n");
            play_pass = true;
        }
        else
        {
            ESP_LOGE(TAG, "--------------- PLAY Button Test FAIL ---------------\n");
            play_pass = false;
            adc_button_pressed = -1;
        }

        printf("Please press UP+ Button\n");
        xQueueReceive(adc_evt_queue, &adc_button_pressed, 10000 / portTICK_PERIOD_MS);
        if (adc_button_pressed == UP)
        {
            ESP_LOGW(TAG, "--------------- UP+ Button Test PASS ---------------\n");
            up_pass = true;
        }
        else
        {
            ESP_LOGE(TAG, "--------------- UP+ Button Test FAIL ---------------\n");
            up_pass = false;
            adc_button_pressed = -1;
        }

        printf("Please press DN- Button\n");
        xQueueReceive(adc_evt_queue, &adc_button_pressed, 10000 / portTICK_PERIOD_MS);
        if (adc_button_pressed == DOWN)
        {
            ESP_LOGW(TAG, "--------------- DN- Button Test PASS ---------------\n");
            dn_pass = true;
        }
        else
        {
            ESP_LOGE(TAG, "--------------- DN- Button Test FAIL ---------------\n");
            dn_pass = false;
            adc_button_pressed = -1;
        }

        // button_pass = boot_pass && menu_pass && play_pass && up_pass && dn_pass;
        button_pass = menu_pass && play_pass && up_pass && dn_pass;
        if (button_pass)
        {
            ESP_LOGI(TAG, "--------------- Button Test PASS ---------------\n");
        }
        else
        {
            ESP_LOGE(TAG, "--------------- Button Test FAIL ---------------\n");
        }

        xQueueSend(queue_test_result, &button_pass, portMAX_DELAY);
    }
    else
    {
        ESP_LOGE(TAG, "--------------- Receive Test Code Error ---------------\n");
        bool result = false;
        xQueueSend(queue_test_result, &result, portMAX_DELAY);
    }
}



