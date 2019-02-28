#include "app_led.h"

/*void led_task(void *arg)
{
    while(1)
    {
        uint8_t led_state = 1;
        if(g_state == FACE_CAP){
            gpio_set_level(GPIO_LED_WHITE, 1);
        }
        else{
            gpio_set_level(GPIO_LED_WHITE, 0);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}*/


void gpio_led_init()
{
    gpio_config_t gpio_conf;
    gpio_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.pin_bit_mask = 1LL << GPIO_LED_RED;
    gpio_config(&gpio_conf);
    gpio_conf.pin_bit_mask = 1LL << GPIO_LED_WHITE;
    gpio_config(&gpio_conf);

    //xTaskCreatePinnedToCore(&led_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL, 0);

}