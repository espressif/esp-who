#include "who_yield2idle.hpp"
#include <esp_log.h>
static const char *TAG = "WhoYield2Idle";

namespace who {
WhoYield2Idle *WhoYield2Idle::get_instance(const BaseType_t xCoreID)
{
    static WhoYield2Idle core0_instance("Yield2Idle0", 0);
    static WhoYield2Idle core1_instance("Yield2Idle1", 1);
    if (xCoreID == 0) {
        return &core0_instance;
    } else if (xCoreID == 1) {
        return &core1_instance;
    } else {
        ESP_LOGE(TAG, "Error Core id, only support 0 or 1");
        return nullptr;
    }
}

bool WhoYield2Idle::run()
{
    bool ret = who::WhoYield2Idle::get_instance(0)->run(1024, 3, 0);
    return ret & who::WhoYield2Idle::get_instance(1)->run(1024, 3, 1);
}

bool WhoYield2Idle::stop()
{
    if (xEventGroupGetBits(m_event_group) & TERMINATE) {
        return false;
    }
    xEventGroupWaitBits(m_event_group, BLOCKING, pdFALSE, pdFALSE, portMAX_DELAY);
    xEventGroupSetBits(m_event_group, STOP);
    xTaskAbortDelay(xTaskGetHandle(m_name.c_str()));
    xEventGroupWaitBits(m_event_group, TERMINATE, pdFALSE, pdFALSE, portMAX_DELAY);
    return true;
}

bool WhoYield2Idle::monitor(WhoTask *task)
{
    if (!add_element(task)) {
        return false;
    }
    WhoYield2Idle *other = (m_coreid == 0) ? get_instance(1) : get_instance(0);
    auto other_it = std::find(other->m_elements.begin(), other->m_elements.end(), task);
    if (other_it != other->m_elements.end()) {
        other->m_elements.erase(other_it);
    }
    return true;
}

void WhoYield2Idle::task()
{
    const TickType_t interval = pdMS_TO_TICKS((CONFIG_ESP_TASK_WDT_TIMEOUT_S - 1) * 1000);
    TickType_t last_wake_time = xTaskGetTickCount();
    while (true) {
        set_and_clear_bits(BLOCKING, RUNNING);
        vTaskDelayUntil(&last_wake_time, interval);
        EventBits_t event_bits = xEventGroupGetBits(m_event_group);
        if (event_bits & STOP) {
            set_and_clear_bits(TERMINATE, BLOCKING | STOP);
            break;
        } else if (event_bits & PAUSE) {
            xEventGroupClearBits(m_event_group, PAUSE);
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, RESUME | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & STOP) {
                set_and_clear_bits(TERMINATE, BLOCKING);
                break;
            } else {
                last_wake_time = xTaskGetTickCount();
                continue;
            }
        }
        set_and_clear_bits(RUNNING, BLOCKING);
        for (const auto &task : m_elements) {
            task->pause();
            xEventGroupWaitBits(task->m_event_group, BLOCKING | TERMINATE, pdFALSE, pdFALSE, portMAX_DELAY);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
        for (const auto &task : m_elements) {
            task->resume();
        }
    }
    vTaskDelete(NULL);
}
} // namespace who
