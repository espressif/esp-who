#pragma once

#include "esp_log.h"
#include "esp_check.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

class MCPWM {
public:
    MCPWM(gpio_num_t gen_gpio_num, int group_id);
    MCPWM(gpio_num_t gen_gpio_num, mcpwm_timer_handle_t timer, int group_id);
    ~MCPWM();
    static mcpwm_timer_handle_t create_mcpwm_timer(int group_id);
    esp_err_t enable_and_start_timer();
    esp_err_t stop_and_disable_timer();
    esp_err_t set_servo_angle(float angle);
    void get_timer(mcpwm_timer_handle_t *timer, int *group_id);
protected:
    virtual uint32_t angle_to_cmpr_value(float angle);
private:
    mcpwm_timer_handle_t m_timer;
    int m_group_id;
    mcpwm_oper_handle_t m_oper;
    mcpwm_cmpr_handle_t m_cmpr;
    mcpwm_gen_handle_t m_gen;
    SemaphoreHandle_t m_timer_mutex;
    bool m_timer_running;
    bool m_extern_timer;
};