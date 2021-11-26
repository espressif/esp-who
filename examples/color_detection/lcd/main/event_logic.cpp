#include <stdio.h>
#include "event_logic.hpp"
#include "who_button.h"
#include "who_color_detection.hpp"

typedef enum
{
    MENU = 1,
    PLAY,
    UP,
    DOWN
} key_name_t;

static QueueHandle_t xQueueKeyStateIGPIO = NULL;
static QueueHandle_t xQueueKeyStateIADC = NULL;
static QueueHandle_t xQueueEventO = NULL;
static key_state_t key_state;
static key_name_t adc_button_name;
static color_detection_state_t detector_state;

void event_generate_from_gpio_button(void *arg)
{
    color_detection_state_t switch_mode = SWITCH_RESULT;
    while (1)
    {
        xQueueReceive(xQueueKeyStateIGPIO, &key_state, portMAX_DELAY);
        xQueueSend(xQueueEventO, &switch_mode, portMAX_DELAY);
    }
}

void event_generate_from_adc_button(void *arg)
{
    bool register_mode = false;
    while (1)
    {
        xQueueReceive(xQueueKeyStateIADC, &adc_button_name, portMAX_DELAY);
        switch (adc_button_name)
        {
        case MENU:
            detector_state = register_mode ? CLOSE_REGISTER_COLOR_BOX : OPEN_REGISTER_COLOR_BOX;
            register_mode = !register_mode;
            break;

        case PLAY:
            detector_state = register_mode ? REGISTER_COLOR : DELETE_COLOR;
            register_mode = false;
            break;

        case UP:
            detector_state = INCREASE_COLOR_AREA;
            break;

        case DOWN:
            detector_state = DECREASE_COLOR_AREA;
            break;

        default:
            detector_state = COLOR_DETECTION_IDLE;
            break;
        }
        xQueueSend(xQueueEventO, &detector_state, portMAX_DELAY);
    }
}

void register_event(const QueueHandle_t key_state_i_adc, const QueueHandle_t key_state_i_gpio, const QueueHandle_t event_o)
{
    xQueueKeyStateIADC = key_state_i_adc;
    xQueueKeyStateIGPIO = key_state_i_gpio;
    xQueueEventO = event_o;
    if (xQueueKeyStateIADC != NULL)
        xTaskCreatePinnedToCore(event_generate_from_adc_button, "event_logic_task1", 1024, NULL, 5, NULL, 0);
    if (xQueueKeyStateIGPIO != NULL)
        xTaskCreatePinnedToCore(event_generate_from_gpio_button, "event_logic_task2", 1024, NULL, 5, NULL, 0);
}