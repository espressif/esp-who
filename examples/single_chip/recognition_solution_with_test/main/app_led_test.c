#include "app_led_test.h"

static const char *TAG = "LED TEST";

void led_test_task(void *arg)
{
    while(1)
    {
        uint8_t led_state = 1;
        if(g_state_test == LED_TEST){
            for(int i=0;i<3;i++){
                gpio_set_level(GPIO_LED_WHITE, 1);
                gpio_set_level(GPIO_LED_RED, 1);
                if(gpio_get_level(GPIO_LED_WHITE) == 0 || gpio_get_level(GPIO_LED_RED) == 0){
                    led_state = 0;
                    //ESP_LOGE(TAG,"FAIL\n");
                }
                vTaskDelay(300 / portTICK_PERIOD_MS);
                gpio_set_level(GPIO_LED_WHITE, 0);
                gpio_set_level(GPIO_LED_RED, 0);
                if(gpio_get_level(GPIO_LED_WHITE) == 1 || gpio_get_level(GPIO_LED_RED) == 1){
                    led_state = 0;
                    //ESP_LOGE(TAG,"FAIL\n");
                }
                vTaskDelay(300 / portTICK_PERIOD_MS);
            }
            if(led_state){
                ESP_LOGI(TAG,"PASS\n");
                led_pass = 1;
            }else{
                ESP_LOGE(TAG,"FAIL\n");
                led_state = 1;
            }
            g_state_test = MIC_TEST;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void gpio_led_test_init()
{
    gpio_config_t gpio_conf;
    gpio_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.pin_bit_mask = 1LL << GPIO_LED_RED;
    gpio_config(&gpio_conf);
    gpio_conf.pin_bit_mask = 1LL << GPIO_LED_WHITE;
    gpio_config(&gpio_conf);

    xTaskCreatePinnedToCore(&led_test_task, "led_test_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL, 0);

}