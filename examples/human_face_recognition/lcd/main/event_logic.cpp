#include <stdio.h>
#include "event_logic.hpp"
#include "who_button.h"
#include "who_human_face_recognition.hpp"

typedef enum
{
    MENU = 1,
    PLAY,
    UP,
    DOWN
}key_name_t;

static QueueHandle_t xQueueKeyStateI = NULL;
static QueueHandle_t xQueueEventO = NULL;
static key_state_t key_state;
static key_name_t adc_button_name;
static recognizer_state_t recognizer_state;

void event_generate(void *arg)
{
    while (1)
    {
        xQueueReceive(xQueueKeyStateI, &key_state, portMAX_DELAY);
        switch (key_state)
        {
        case KEY_SHORT_PRESS:
            recognizer_state = RECOGNIZE;
            break;

        case KEY_LONG_PRESS:
            recognizer_state = ENROLL;
            break;

        case KEY_DOUBLE_CLICK:
            recognizer_state = DELETE;
            break;

        default:
            recognizer_state = DETECT;
            break;
        }
        xQueueSend(xQueueEventO, &recognizer_state, portMAX_DELAY);
    }
}


void event_generate_from_adc_button(void *arg)
{
    while (1)
    {
        xQueueReceive(xQueueKeyStateI, &adc_button_name, portMAX_DELAY);
        switch (adc_button_name)
        {
        case MENU:
            recognizer_state = ENROLL;
            break;

        case PLAY:
            recognizer_state = DELETE;
            break;

        case UP:
            recognizer_state = RECOGNIZE;
            break;

        case DOWN:
            recognizer_state = RECOGNIZE;
            break;

        default:
            recognizer_state = DETECT;
            break;
        }
        xQueueSend(xQueueEventO, &recognizer_state, portMAX_DELAY);
    }
}

void register_event(const QueueHandle_t key_state_i, const QueueHandle_t event_o)
{
    xQueueKeyStateI = key_state_i;
    xQueueEventO = event_o;
    xTaskCreatePinnedToCore(event_generate, "event_logic_task", 1024, NULL, 5, NULL, 0);
}