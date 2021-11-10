#include "who_led.h"
#include "esp_log.h"

static const char *TAG = "who_led";
static QueueHandle_t xQueueLEDControlI = NULL;
static int gpio_led;

#if CONFIG_LED_ILLUMINATOR_ENABLED
#include "driver/ledc.h"

#if CONFIG_LED_LEDC_LOW_SPEED_MODE
#define CONFIG_LED_LEDC_SPEED_MODE LEDC_LOW_SPEED_MODE
#else
#define CONFIG_LED_LEDC_SPEED_MODE LEDC_HIGH_SPEED_MODE
#endif

void app_led_init()
{
    gpio_set_direction(CONFIG_LED_LEDC_PIN, GPIO_MODE_OUTPUT);
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT, // resolution of PWM duty
        .freq_hz = 1000,                     // frequency of PWM signal
        .speed_mode = LEDC_LOW_SPEED_MODE,   // timer mode
        .timer_num = CONFIG_LED_LEDC_TIMER   // timer index
    };
    ledc_channel_config_t ledc_channel = {
        .channel = CONFIG_LED_LEDC_CHANNEL,
        .duty = 0,
        .gpio_num = CONFIG_LED_LEDC_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = CONFIG_LED_LEDC_TIMER};
#if CONFIG_LED_LEDC_HIGH_SPEED_MODE
    ledc_timer.speed_mode = ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
#endif
    switch (ledc_timer_config(&ledc_timer))
    {
    case ESP_ERR_INVALID_ARG:
        ESP_LOGE(TAG, "ledc_timer_config() parameter error");
        break;
    case ESP_FAIL:
        ESP_LOGE(TAG, "ledc_timer_config() Can not find a proper pre-divider number base on the given frequency and the current duty_resolution");
        break;
    case ESP_OK:
        if (ledc_channel_config(&ledc_channel) == ESP_ERR_INVALID_ARG)
        {
            ESP_LOGE(TAG, "ledc_channel_config() parameter error");
        }
        break;
    default:
        break;
    }

    ESP_LOGE(TAG, "LED has been initialized.");
}

void app_led_duty(int duty)
{ // Turn LED On or Off
    if (duty > CONFIG_LED_MAX_INTENSITY)
        duty = CONFIG_LED_MAX_INTENSITY;

    ledc_set_duty(CONFIG_LED_LEDC_SPEED_MODE, CONFIG_LED_LEDC_CHANNEL, duty);
    ledc_update_duty(CONFIG_LED_LEDC_SPEED_MODE, CONFIG_LED_LEDC_CHANNEL);
    ESP_LOGI(TAG, "Set LED intensity to %d", duty);
}

#endif

static void led_task(void *arg)
{
    led_state_t led_op;
    while (1)
    {
        xQueueReceive(xQueueLEDControlI, &led_op, portMAX_DELAY);

        switch (led_op)
        {
        case LED_ALWAYS_OFF:
            gpio_set_level(gpio_led, 0);
            break;
        case LED_ALWAYS_ON:
            gpio_set_level(gpio_led, 1);
            break;
        case LED_OFF_1S:
            gpio_set_level(gpio_led, 0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gpio_set_level(gpio_led, 1);
            break;
        case LED_OFF_2S:
            gpio_set_level(gpio_led, 0);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            gpio_set_level(gpio_led, 1);
            break;
        case LED_OFF_4S:
            gpio_set_level(gpio_led, 0);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            gpio_set_level(gpio_led, 1);
            break;
        case LED_ON_1S:
            gpio_set_level(gpio_led, 1);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gpio_set_level(gpio_led, 0);
            break;
        case LED_ON_2S:
            gpio_set_level(gpio_led, 1);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            gpio_set_level(gpio_led, 0);
            break;
        case LED_ON_4S:
            gpio_set_level(gpio_led, 1);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            gpio_set_level(gpio_led, 0);
            break;
        case LED_BLINK_1S:
            for (int i = 0; i < 2; ++i)
            {
                gpio_set_level(gpio_led, 1);
                vTaskDelay(250 / portTICK_PERIOD_MS);
                gpio_set_level(gpio_led, 0);
                vTaskDelay(250 / portTICK_PERIOD_MS);
            }
            break;
        case LED_BLINK_2S:
            for (int i = 0; i < 4; ++i)
            {
                gpio_set_level(gpio_led, 1);
                vTaskDelay(250 / portTICK_PERIOD_MS);
                gpio_set_level(gpio_led, 0);
                vTaskDelay(250 / portTICK_PERIOD_MS);
            }
            break;
        case LED_BLINK_4S:
            for (int i = 0; i < 8; ++i)
            {
                gpio_set_level(gpio_led, 1);
                vTaskDelay(250 / portTICK_PERIOD_MS);
                gpio_set_level(gpio_led, 0);
                vTaskDelay(250 / portTICK_PERIOD_MS);
            }
            break;

        default:
            break;
        }
    }
}

void register_led(const gpio_num_t led_io_num, const QueueHandle_t control_i)
{
    xQueueLEDControlI = control_i;
    gpio_led = led_io_num;

    gpio_config_t gpio_conf;
    gpio_conf.mode = GPIO_MODE_OUTPUT_OD;
    gpio_conf.pull_up_en = 1;

    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.pin_bit_mask = 1LL << gpio_led;
    gpio_config(&gpio_conf);

    xTaskCreatePinnedToCore(&led_task, "who_led_task", 1 * 1024, NULL, 5, NULL, 0);
}