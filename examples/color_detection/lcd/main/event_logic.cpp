#include <stdio.h>
#include "event_logic.hpp"
#include "who_color_detection.hpp"
#include "bsp/esp-bsp.h"

static QueueHandle_t xQueueEventO = NULL;
static color_detection_state_t detector_state;
static button_handle_t buttons[BSP_BUTTON_NUM];

void button_cb(void *button_handle, void *usr_data)
{
    int button_pressed = reinterpret_cast<int>(usr_data);
    static bool register_mode = false;

    switch (button_pressed) {
    case BSP_BUTTON_MENU: // Menu
        detector_state = register_mode ? CLOSE_REGISTER_COLOR_BOX : OPEN_REGISTER_COLOR_BOX;
        register_mode = !register_mode;
        break;

    case BSP_BUTTON_PLAY: // Play
        detector_state = register_mode ? REGISTER_COLOR : DELETE_COLOR;
        register_mode = false;
        break;

    case BSP_BUTTON_DOWN: // Down
        detector_state = DECREASE_COLOR_AREA;
        break;

    case BSP_BUTTON_UP: // Up
        detector_state = INCREASE_COLOR_AREA;
        break;

    case BSP_BUTTON_BOOT: { // The BOOT button
        color_detection_state_t switch_mode = SWITCH_RESULT;
        xQueueSend(xQueueEventO, &switch_mode, portMAX_DELAY);
        return; // Attention: we return here directly
    }

    default:
        detector_state = COLOR_DETECTION_IDLE;
        break;
    }
    xQueueSend(xQueueEventO, &detector_state, portMAX_DELAY);
}

void register_button_events(const QueueHandle_t event_o)
{
    xQueueEventO = event_o;
    // Create all buttons (including the BOOT button)
    bsp_iot_button_create(buttons, NULL, BSP_BUTTON_NUM);
    for (int i = 0; i < BSP_BUTTON_NUM; i++) {
        iot_button_register_cb(buttons[i], BUTTON_PRESS_DOWN, button_cb, (void *) i);
    }
}