#pragma once
#include "esp_timer.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int button_index; /**< button index on the channel */
        int min;          /**< min voltage in mv corresponding to the button */
        int max;          /**< max voltage in mv corresponding to the button */
    } button_adc_config_t;

    /**
     * @brief initialize adc button
     * 
     * @param buttons_ptr the pointer of adc button configuration
     * @param button_num the numbers of adc buttons
     * @param key_state_o the queue to send which button is pressed
     */
    void register_adc_button(button_adc_config_t *buttons_ptr, int button_num, const QueueHandle_t key_state_o);

#ifdef __cplusplus
}
#endif