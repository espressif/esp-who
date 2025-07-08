#include "who_yield2idle.hpp"
#include "esp_freertos_hooks.h"
#include <algorithm>
#include <cstring>
#include <esp_log.h>
#include <limits>
static const char *TAG = "WhoYield2Idle";

namespace who {
void WhoYield2IdleTaskGroup::select_tasks(bool yield2idle0, bool yield2idle1)
{
    if (yield2idle0 && yield2idle1) {
        m_selected_tasks = get_all_tasks();
    } else if (yield2idle0) {
        m_selected_tasks = get_tasks_by_coreid(0);
    } else if (yield2idle1) {
        m_selected_tasks = get_tasks_by_coreid(1);
    } else {
        ESP_LOGE(TAG, "No task is selected.");
        m_selected_tasks = {};
    }
}

void WhoYield2IdleTaskGroup::pause_selected_tasks()
{
    std::vector<bool> flags;
    for (const auto &task : m_selected_tasks) {
        flags.emplace_back(task->pause_async());
    }
    for (int i = 0; i < m_selected_tasks.size(); i++) {
        if (flags[i]) {
            m_selected_tasks[i]->wait_for_paused(portMAX_DELAY);
        }
    }
    for (int i = 0; i < m_selected_tasks.size(); i++) {
        if (flags[i]) {
            m_selected_tasks[i]->cleanup_for_paused();
        }
    }
}

void WhoYield2IdleTaskGroup::resume_selected_tasks()
{
    for (const auto &task : m_selected_tasks) {
        task->resume();
    }
}

void WhoYield2IdleTaskGroup::lock_selected_tasks()
{
    for (const auto &task : m_selected_tasks) {
        xSemaphoreTake(task->get_mutex(), portMAX_DELAY);
    }
}

void WhoYield2IdleTaskGroup::unlock_selected_tasks()
{
    for (const auto &task : m_selected_tasks) {
        xSemaphoreGive(task->get_mutex());
    }
}

std::vector<WhoTask *> WhoYield2IdleTaskGroup::get_tasks_by_coreid(BaseType_t coreid)
{
    assert(coreid == 0 || coreid == 1);
    auto all_tasks = get_all_tasks();
    std::vector<WhoTask *> tasks;
    for (const auto &task : all_tasks) {
        if (task->get_coreid() == coreid) {
            tasks.emplace_back(task);
        }
    }
    return tasks;
}

int WhoYield2Idle::s_idle0_cnt = 0;
int WhoYield2Idle::s_idle1_cnt = 0;

WhoYield2Idle *WhoYield2Idle::get_instance()
{
    static WhoYield2Idle yield2idle("Yield2Idle");
    return &yield2idle;
}

bool WhoYield2Idle::run(const configSTACK_DEPTH_TYPE uxStackDepth)
{
    return WhoTaskBase::run(uxStackDepth, configMAX_PRIORITIES - 1, tskNO_AFFINITY);
}

void WhoYield2Idle::start_monitor(WhoTask *task)
{
    m_task_group.register_task(task);
}

void WhoYield2Idle::end_monitor(WhoTask *task)
{
    m_task_group.unregister_task(task);
}

bool WhoYield2Idle::stop_async()
{
    if (WhoTaskBase::stop_async()) {
        xTaskAbortDelay(m_task_handle);
        return true;
    }
    return false;
}

bool WhoYield2Idle::pause_async()
{
    if (WhoTaskBase::pause_async()) {
        xTaskAbortDelay(m_task_handle);
        return true;
    }
    return false;
}

void WhoYield2Idle::task()
{
    const TickType_t interval = pdMS_TO_TICKS((CONFIG_ESP_TASK_WDT_TIMEOUT_S - CONFIG_MAX_TASK_LOOP_TIME) * 1000 / 2);
    if (interval < pdMS_TO_TICKS(1500)) {
        ESP_LOGW(TAG, "Try to increase CONFIG_ESP_TASK_WDT_TIMEOUT_S");
    }
    TickType_t last_wake_time = xTaskGetTickCount();
    esp_register_freertos_idle_hook_for_cpu(idle0_cb, 0);
    esp_register_freertos_idle_hook_for_cpu(idle1_cb, 1);
    bool last_time_yield = true;
    while (true) {
        vTaskDelayUntil(&last_wake_time, interval);
        EventBits_t event_bits = xEventGroupWaitBits(m_event_group, PAUSE | STOP, pdTRUE, pdFALSE, 0);
        if (event_bits & STOP) {
            break;
        } else if (event_bits & PAUSE) {
            xEventGroupSetBits(m_event_group, PAUSED);
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, RESUME | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & STOP) {
                break;
            } else {
                last_wake_time = xTaskGetTickCount();
                continue;
            }
        }
        if (!last_time_yield) {
            bool yield2idle0 = !s_idle0_cnt;
            bool yield2idle1 = !s_idle1_cnt;
            if (yield2idle0 || yield2idle1) {
                // disable resume() and run()
                m_task_group.select_tasks(yield2idle0, yield2idle1);
                m_task_group.lock_selected_tasks();
                m_task_group.pause_selected_tasks();
                vTaskDelay(pdMS_TO_TICKS(10));
                m_task_group.unlock_selected_tasks();
                m_task_group.resume_selected_tasks();
                last_time_yield = true;
                continue;
            }
        }
        reset_idle0_cnt();
        reset_idle1_cnt();
        last_time_yield = false;
    }
    esp_deregister_freertos_idle_hook_for_cpu(idle0_cb, 0);
    esp_deregister_freertos_idle_hook_for_cpu(idle1_cb, 1);
    xEventGroupSetBits(m_event_group, STOPPED);
    vTaskDelete(NULL);
}

bool WhoYield2Idle::idle0_cb(void)
{
    s_idle0_cnt++;
    return true;
}

bool WhoYield2Idle::idle1_cb(void)
{
    s_idle1_cnt++;
    return true;
}

void WhoYield2Idle::reset_idle0_cnt()
{
    s_idle0_cnt = 0;
}

void WhoYield2Idle::reset_idle1_cnt()
{
    s_idle1_cnt = 0;
}
} // namespace who
