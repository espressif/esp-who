#include <stdio.h>
#include <stdlib.h>
#include "who_button.h"
static const char *TAG = "WHO BUTTON";
typedef struct
{
    gpio_num_t io_num;
    key_state_t state;
} key_scan_state_t;

#define LONG_PRESS_THRESH 700000
#define DOUBLE_CLICK_THRESH 300000

static QueueHandle_t gpio_evt_queue = NULL;
static QueueHandle_t xQueueKeyStateO = NULL;

static void IRAM_ATTR gpio_isr_handler_key(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

int key_scan(TickType_t ticks_to_wait)
{
    gpio_num_t io_num;
    BaseType_t press_key = pdFALSE;
    BaseType_t lift_key = pdFALSE;
    int64_t backup_time = 0;
    int64_t interval_time = 0;
    static int64_t last_time = 0;

    for (;;)
    {
        xQueueReceive(gpio_evt_queue, &io_num, ticks_to_wait);

        if (gpio_get_level(io_num) == 0)
        {
            press_key = pdTRUE;
            backup_time = esp_timer_get_time();
            interval_time = backup_time - last_time;
        }
        else if (press_key)
        {
            lift_key = pdTRUE;
            last_time = esp_timer_get_time();
            backup_time = last_time - backup_time;
        }

        if (press_key & lift_key)
        {
            press_key = pdFALSE;
            lift_key = pdFALSE;

            if (backup_time > LONG_PRESS_THRESH)
            {
                return KEY_LONG_PRESS;
            }
            else
            {
                if ((interval_time < DOUBLE_CLICK_THRESH) && (interval_time > 0))
                    return KEY_DOUBLE_CLICK;
                else
                    return KEY_SHORT_PRESS;
            }
        }
    }
}

void key_trigger(void *arg)
{
    int ret = 0;

    while (1)
    {
        ret = key_scan(portMAX_DELAY);
        xQueueOverwrite(xQueueKeyStateO, &ret);
    }

    vTaskDelete(NULL);
}

void key_init(gpio_num_t gpio_num)
{
    gpio_config_t io_conf = {0};
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = 1LL << gpio_num;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    gpio_evt_queue = xQueueCreate(5, sizeof(uint32_t));
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, gpio_isr_handler_key, (void *)gpio_num);
}

void register_button(const gpio_num_t key_io_num, const QueueHandle_t key_state_o)
{
    xQueueKeyStateO = key_state_o;
    key_init(key_io_num);
    ESP_LOGI(TAG, "key init done");
    xTaskCreatePinnedToCore(key_trigger, "key_scan_task", 1024, NULL, 5, NULL, 0);
}