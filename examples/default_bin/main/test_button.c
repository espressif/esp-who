#include "test_logic.h"
#include "driver/adc_common.h"
#include "esp_adc_cal.h"
#include "who_adc_button.h"

static QueueHandle_t gpio_evt_queue = NULL;
static QueueHandle_t adc_evt_queue = NULL;
static QueueHandle_t queue_test_result = NULL;
static QueueHandle_t *queues_tests = NULL;
static const char *TAG = "BUTTON";

//ADC Channels
#define ADC1_EXAMPLE_CHAN0 ADC1_CHANNEL_0
//ADC Attenuation
#define ADC_EXAMPLE_ATTEN ADC_ATTEN_DB_11
//ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

#define PRESS_INTERVAL 500000
// #define ADC_WIDTH_BIT_DEFAULT

static uint32_t voltage = 0;

static esp_adc_cal_characteristics_t adc1_chars;

int adc_button_num = 4;
button_adc_config_t adc_buttons[4] = {{1, 2800, 3000}, {2, 2250, 2450}, {3, 300, 500}, {4, 850, 1050}};

bool boot_pass = false;
bool menu_pass = false;
bool play_pass = false;
bool up_pass = false;
bool dn_pass = false;
bool button_pass = false;

typedef enum
{
    MENU = 1,
    PLAY,
    UP,
    DN
}key_name_t;

static void IRAM_ATTR gpio_isr_handler_key(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueOverwriteFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_key_init(gpio_num_t gpio_num)
{
    gpio_config_t io_conf = {0};
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = 1LL << gpio_num;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    gpio_evt_queue = xQueueCreate(1, sizeof(uint32_t));
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, gpio_isr_handler_key, (void *)gpio_num);
}


static void adc_button_task(void *arg)
{
    int last_button_pressed = -1;
    int button_pressed = -1;
    int64_t backup_time = esp_timer_get_time();
    int64_t last_time = esp_timer_get_time();

    //ADC1 config
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_EXAMPLE_CHAN0, ADC_EXAMPLE_ATTEN));

    while (1)
    {
        voltage = adc1_get_raw(ADC1_EXAMPLE_CHAN0);
        backup_time = esp_timer_get_time();
        for (int i = 0; i < adc_button_num; ++i)
        {
            if ((voltage >= adc_buttons[i].min) && (voltage <= adc_buttons[i].max))
            {
                button_pressed = adc_buttons[i].button_index;
                if ((button_pressed != last_button_pressed) || ((backup_time - last_time) > PRESS_INTERVAL))
                {
                    last_button_pressed = button_pressed;
                    last_time = backup_time;
                    xQueueOverwrite(adc_evt_queue, &button_pressed);
                    break;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void button_test_task(void *arg)
{
    while (1)
    {
        int io_num = -1;
        int adc_button_pressed = -1;
        en_test_state g_state_test = TEST_IDLE;
        xQueueReceive(queues_tests[TEST_BUTTON], &g_state_test, portMAX_DELAY);
        if (g_state_test == TEST_BUTTON)
        {
            ESP_LOGI("ESP32-S3-EYE", "--------------- Enter Button Test ---------------\n");
            
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
            if (adc_button_pressed == DN)
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
            if(button_pass)
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
}

void register_button_test(QueueHandle_t *test_queues,const QueueHandle_t result_queue, const QueueHandle_t key_state_o)
{
    adc_evt_queue = key_state_o;
    queue_test_result = result_queue;
    queues_tests = test_queues;
    // gpio_key_init(GPIO_NUM_0);
    xTaskCreatePinnedToCore(adc_button_task, "adc_button_scan_task", 3*1024, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(button_test_task, "button_test_task", 3*1024, NULL, 5, NULL, 0);
}