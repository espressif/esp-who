#include "who_task_state.hpp"
#include <freertos/FreeRTOS.h>

namespace who {
namespace task {
WhoTaskState::WhoTaskState(int interval) : WhoTask("TaskState"), m_interval(interval)
{
    m_task_state = {"Running", "Ready", "Blocked", "Suspended", "Deleted", "Invalid"};
}

void WhoTaskState::task()
{
    const TickType_t interval = pdMS_TO_TICKS(1000 * m_interval);
    TickType_t last_wake_time = xTaskGetTickCount();
    while (true) {
        vTaskDelayUntil(&last_wake_time, interval);
        EventBits_t event_bits = xEventGroupWaitBits(m_event_group, TASK_PAUSE | TASK_STOP, pdTRUE, pdFALSE, 0);
        if (event_bits & TASK_STOP) {
            break;
        } else if (event_bits & TASK_PAUSE) {
            xEventGroupSetBits(m_event_group, TASK_PAUSED);
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, TASK_RESUME | TASK_STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & TASK_STOP) {
                break;
            } else {
                last_wake_time = xTaskGetTickCount();
                continue;
            }
        }
        print_task_status();
    }
    xEventGroupSetBits(m_event_group, TASK_STOPPED);
    vTaskDelete(NULL);
}

bool WhoTaskState::stop_async()
{
    if (WhoTask::stop_async()) {
        xTaskAbortDelay(m_task_handle);
        return true;
    }
    return false;
}

bool WhoTaskState::pause_async()
{
    if (WhoTask::pause_async()) {
        xTaskAbortDelay(m_task_handle);
        return true;
    }
    return false;
}

void WhoTaskState::print_task_status()
{
    configRUN_TIME_COUNTER_TYPE total_run_time;
    UBaseType_t num_tasks = uxTaskGetNumberOfTasks();
    TaskStatus_t *task_status_array =
        (TaskStatus_t *)heap_caps_malloc(num_tasks * sizeof(TaskStatus_t), MALLOC_CAP_DEFAULT);
    uxTaskGetSystemState(task_status_array, num_tasks, &total_run_time);

    printf("\nTask Name       |  coreid  |   State   | Priority |  Stack HWM  |   Run Time  | Run Time Per |\n");
    printf("----------------------------------------------------------------------------------------------\n");

    for (UBaseType_t i = 0; i < num_tasks; i++) {
#if CONFIG_FREERTOS_RUN_TIME_COUNTER_TYPE_U32
        printf("%-15s | %-8x | %-9s | %-8u | %-11lu | %-11lu | %-12lu |\n",
#else
        printf("%-15s | %-8x | %-9s | %-8u | %-11lu | %-11llu | %-12llu |\n",
#endif
               task_status_array[i].pcTaskName,
               task_status_array[i].xCoreID,
               m_task_state[task_status_array[i].eCurrentState].c_str(),
               task_status_array[i].uxCurrentPriority,
               task_status_array[i].usStackHighWaterMark,
               task_status_array[i].ulRunTimeCounter,
               task_status_array[i].ulRunTimeCounter * 100 / total_run_time);
    }
    printf("\n");
    heap_caps_free(task_status_array);
}
} // namespace task
} // namespace who
