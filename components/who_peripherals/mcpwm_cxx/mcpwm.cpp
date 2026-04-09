#include "mcpwm.hpp"

static const char *TAG = "MCPWM";

MCPWM::MCPWM(gpio_num_t gen_gpio_num, int group_id) : MCPWM(gen_gpio_num, create_mcpwm_timer(group_id), group_id)
{
    m_extern_timer = false;
    m_timer_mutex = xSemaphoreCreateMutex();
}

MCPWM::MCPWM(gpio_num_t gen_gpio_num, mcpwm_timer_handle_t timer, int group_id) : m_timer(timer), m_group_id(group_id), m_timer_running(false), m_extern_timer(true)
{
    mcpwm_operator_config_t oper_cfg = {};
    oper_cfg.group_id = group_id; // operator must be in the same group to the timer
    ESP_ERROR_CHECK(mcpwm_new_operator(&oper_cfg, &m_oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(m_oper, m_timer));
    mcpwm_comparator_config_t cmpr_cfg = {};
    cmpr_cfg.flags.update_cmp_on_tez = true;
    ESP_ERROR_CHECK(mcpwm_new_comparator(m_oper, &cmpr_cfg, &m_cmpr));
    mcpwm_generator_config_t gen_cfg = {};
    gen_cfg.gen_gpio_num = gen_gpio_num;
    ESP_ERROR_CHECK(mcpwm_new_generator(m_oper, &gen_cfg, &m_gen));
    // go high on counter empty
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
        m_gen,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    // go low on compare threshold
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        m_gen, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, m_cmpr, MCPWM_GEN_ACTION_LOW)));
}

MCPWM::~MCPWM()
{
    ESP_ERROR_CHECK(mcpwm_del_comparator(m_cmpr));
    ESP_ERROR_CHECK(mcpwm_del_generator(m_gen));
    ESP_ERROR_CHECK(mcpwm_del_operator(m_oper));
    if (!m_extern_timer) {
        ESP_ERROR_CHECK(stop_and_disable_timer());
        ESP_ERROR_CHECK(mcpwm_del_timer(m_timer));
        vSemaphoreDelete(m_timer_mutex);
    }
}

mcpwm_timer_handle_t MCPWM::create_mcpwm_timer(int group_id)
{
    mcpwm_timer_handle_t timer = nullptr;
    mcpwm_timer_config_t timer_cfg = {};
    timer_cfg.group_id = group_id;
    timer_cfg.clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
    timer_cfg.resolution_hz = CONFIG_SERVO_TIMEBASE_RESOLUTION_HZ;
    timer_cfg.period_ticks = CONFIG_SERVO_TIMEBASE_PERIOD;
    timer_cfg.count_mode = MCPWM_TIMER_COUNT_MODE_UP;
    if (mcpwm_new_timer(&timer_cfg, &timer) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create mcpwm timer.");
        return nullptr;
    }
    return timer;
}

esp_err_t MCPWM::enable_and_start_timer()
{
    if (m_extern_timer) {
        ESP_LOGE(TAG, "Can not control external timer");
        return ESP_FAIL;
    }
    xSemaphoreTake(m_timer_mutex, portMAX_DELAY);
    if (m_timer_running) {
        xSemaphoreGive(m_timer_mutex);
        return ESP_OK;
    }
    ESP_RETURN_ON_ERROR(mcpwm_timer_enable(m_timer), TAG, "Failed to enable mcpwm timer.");
    ESP_RETURN_ON_ERROR(mcpwm_timer_start_stop(m_timer, MCPWM_TIMER_START_NO_STOP), TAG, "Failed to start mcpwm timer.");
    m_timer_running = true;
    xSemaphoreGive(m_timer_mutex);
    return ESP_OK;
}

esp_err_t MCPWM::stop_and_disable_timer()
{
    if (m_extern_timer) {
        ESP_LOGE(TAG, "Can not control external timer");
        return ESP_FAIL;
    }
    xSemaphoreTake(m_timer_mutex, portMAX_DELAY);
    if (!m_timer_running) {
        xSemaphoreGive(m_timer_mutex);
        return ESP_OK;
    }
    ESP_RETURN_ON_ERROR(mcpwm_timer_start_stop(m_timer, MCPWM_TIMER_STOP_FULL), TAG, "Failed to stop mcpwm timer.");
    ESP_RETURN_ON_ERROR(mcpwm_timer_disable(m_timer), TAG, "Failed to disable mcpwm timer.");
    m_timer_running = false;
    xSemaphoreGive(m_timer_mutex);
    return ESP_OK;
}

esp_err_t MCPWM::set_servo_angle(float angle)
{
    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(m_cmpr, angle_to_cmpr_value(angle)), TAG, "Failed to set servo angle.");
    return ESP_OK;
}

void MCPWM::get_timer(mcpwm_timer_handle_t *timer, int *group_id)
{
    *timer = m_timer;
    *group_id = m_group_id;
}

uint32_t MCPWM::angle_to_cmpr_value(float angle)
{
    return (uint32_t)((angle - CONFIG_SERVO_MIN_DEGREE) *
                      (CONFIG_SERVO_MAX_PULSEWIDTH_US - CONFIG_SERVO_MIN_PULSEWIDTH_US) /
                      (CONFIG_SERVO_MAX_DEGREE - CONFIG_SERVO_MIN_DEGREE)) +
        CONFIG_SERVO_MIN_PULSEWIDTH_US;
}
