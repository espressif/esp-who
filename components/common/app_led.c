#include "app_led.h"

#if CONFIG_LED_ILLUMINATOR_ENABLED
#include "esp_log.h"
#include "driver/ledc.h"

static const char *TAG = "app_led";

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