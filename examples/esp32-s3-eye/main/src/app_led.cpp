#include "app_led.hpp"

#include "esp_log.h"

const static char TAG[] = "App/LED";

typedef enum
{
    LED_ALWAYS_OFF = 0,
    LED_ALWAYS_ON = 1,
    LED_OFF_1S = 2,
    LED_OFF_2S = 3,
    LED_OFF_4S = 4,
    LED_ON_1S = 5,
    LED_ON_2S = 6,
    LED_ON_4S = 7,
    LED_BLINK_1S = 8,
    LED_BLINK_2S = 9,
    LED_BLINK_4S = 10,
} led_mode_t;

LED::LED(const gpio_num_t pin, AppButtom *key, AppSpeech *sr) : pin(pin), key(key), sr(sr)
{
    // initialize GPIO
    gpio_config_t gpio_conf;
    gpio_conf.mode = GPIO_MODE_OUTPUT_OD;
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.pin_bit_mask = 1LL << this->pin;
    gpio_config(&gpio_conf);

    gpio_set_level(this->pin, 0);
}

void LED::update()
{
    led_mode_t mode = LED_ALWAYS_OFF;

    // parse key
    if (this->key->pressed)
    {
        mode = LED_BLINK_2S;
    }
    // parse speech recognition
    else if (this->sr->detected)
    {
        if (this->sr->command > COMMAND_NOT_DETECTED)
            mode = LED_BLINK_1S;
        else
            mode = LED_ALWAYS_ON;
    }

    // do
    switch (mode)
    {
    case LED_ALWAYS_OFF:
        gpio_set_level(this->pin, 0);
        break;
    case LED_ALWAYS_ON:
        gpio_set_level(this->pin, 1);
        break;
    case LED_OFF_1S:
        gpio_set_level(this->pin, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 1);
        break;
    case LED_OFF_2S:
        gpio_set_level(this->pin, 0);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 1);
        break;
    case LED_OFF_4S:
        gpio_set_level(this->pin, 0);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 1);
        break;
    case LED_ON_1S:
        gpio_set_level(this->pin, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 0);
        break;
    case LED_ON_2S:
        gpio_set_level(this->pin, 1);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 0);
        break;
    case LED_ON_4S:
        gpio_set_level(this->pin, 1);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 0);
        break;
    case LED_BLINK_1S:
        for (int i = 0; i < 2; ++i)
        {
            gpio_set_level(this->pin, 1);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            gpio_set_level(this->pin, 0);
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }
        break;
    case LED_BLINK_2S:
        for (int i = 0; i < 4; ++i)
        {
            gpio_set_level(this->pin, 1);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            gpio_set_level(this->pin, 0);
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }
        break;
    case LED_BLINK_4S:
        for (int i = 0; i < 8; ++i)
        {
            gpio_set_level(this->pin, 1);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            gpio_set_level(this->pin, 0);
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }
        break;

    default:
        break;
    }
}