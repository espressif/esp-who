#include "who_task.hpp"
#include "who_yield2idle.hpp"
#include <algorithm>
#include <esp_log.h>

static const char *TAG = "WhoTask";

namespace who {
namespace task {
bool WhoTaskBase::run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID)
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    EventBits_t event_bits = xEventGroupGetBits(m_event_group);
    if (event_bits & TASK_STOPPED) {
        if (xTaskCreatePinnedToCore(task, m_name.c_str(), uxStackDepth, this, uxPriority, &m_task_handle, xCoreID) ==
            pdPASS) {
            xEventGroupClearBits(m_event_group, TASK_STOPPED);
            xSemaphoreGive(m_mutex);
            return true;
        } else {
            ESP_LOGE(TAG, "Failed to create task.\n");
        }
    }
    xSemaphoreGive(m_mutex);
    return false;
}

bool WhoTaskBase::stop()
{
    if (stop_async()) {
        wait_for_stopped(portMAX_DELAY);
        cleanup_for_stopped();
        return true;
    }
    return false;
}

bool WhoTaskBase::stop_async()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    EventBits_t event_bits = xEventGroupGetBits(m_event_group);
    if (!(event_bits & TASK_STOPPED) && !(event_bits & TASK_STOP)) {
        xEventGroupSetBits(m_event_group, TASK_STOP);
        xSemaphoreGive(m_mutex);
        return true;
    }
    xSemaphoreGive(m_mutex);
    return false;
}

void WhoTaskBase::wait_for_stopped(TickType_t timeout)
{
    xEventGroupWaitBits(m_event_group, TASK_STOPPED, pdFALSE, pdFALSE, timeout);
}

bool WhoTaskBase::resume()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    EventBits_t event_bits = xEventGroupGetBits(m_event_group);
    if (event_bits & TASK_PAUSED) {
        xEventGroupSetBits(m_event_group, TASK_RESUME);
        xEventGroupClearBits(m_event_group, TASK_PAUSED);
        xSemaphoreGive(m_mutex);
        return true;
    }
    xSemaphoreGive(m_mutex);
    return false;
}

bool WhoTaskBase::pause()
{
    if (pause_async()) {
        wait_for_paused(portMAX_DELAY);
        cleanup_for_paused();
        return true;
    }
    return false;
}

bool WhoTaskBase::pause_async()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    EventBits_t event_bits = xEventGroupGetBits(m_event_group);
    if (!(event_bits & TASK_STOPPED) && !(event_bits & TASK_PAUSED) && !(event_bits & TASK_STOP) &&
        !(event_bits & TASK_PAUSE)) {
        xEventGroupSetBits(m_event_group, TASK_PAUSE);
        xSemaphoreGive(m_mutex);
        return true;
    }
    xSemaphoreGive(m_mutex);
    return false;
}

void WhoTaskBase::wait_for_paused(TickType_t timeout)
{
    xEventGroupWaitBits(m_event_group, TASK_PAUSED, pdFALSE, pdFALSE, timeout);
}

void WhoTaskBase::cleanup_for_paused()
{
    xEventGroupClearBits(m_event_group, ALL_EVENT_BITS & ~TASK_PAUSED);
    cleanup();
}

void WhoTaskBase::cleanup_for_stopped()
{
    xEventGroupClearBits(m_event_group, ALL_EVENT_BITS & ~TASK_STOPPED);
    cleanup();
}

bool WhoTaskBase::is_active()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    EventBits_t event_bits = xEventGroupGetBits(m_event_group);
    if (!(event_bits & TASK_STOPPED) && !(event_bits & TASK_PAUSED) && !(event_bits & TASK_STOP) &&
        !(event_bits & TASK_PAUSE)) {
        xSemaphoreGive(m_mutex);
        return true;
    }
    xSemaphoreGive(m_mutex);
    return false;
}

void WhoTaskBase::task(void *args)
{
    WhoTaskBase *self = static_cast<WhoTaskBase *>(args);
    self->task();
}

WhoTask::WhoTask(const std::string &name) :
    WhoTaskBase(name), m_coreid(tskNO_AFFINITY), m_mutex(xSemaphoreCreateMutex())
{
    WhoYield2Idle::get_instance()->start_monitor(this);
}

WhoTask::~WhoTask()
{
    WhoYield2Idle::get_instance()->end_monitor(this);
    vSemaphoreDelete(m_mutex);
}

bool WhoTask::run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID)
{
    assert(xCoreID != tskNO_AFFINITY);
    // never run when yielding2idle.
    if (xSemaphoreTake(m_mutex, 0) == pdTRUE) {
        bool ret = WhoTaskBase::run(uxStackDepth, uxPriority, xCoreID);
        m_coreid = xCoreID;
        xSemaphoreGive(m_mutex);
        return ret;
    }
    return false;
}

bool WhoTask::resume()
{
    // never resume when yielding2idle.
    if (xSemaphoreTake(m_mutex, 0) == pdTRUE) {
        bool ret = WhoTaskBase::resume();
        xSemaphoreGive(m_mutex);
        return ret;
    }
    return false;
}

void WhoTaskGroup::register_task(WhoTask *task)
{
    assert(!WhoYield2Idle::get_instance()->is_active());
    m_tasks.emplace_back(task);
}

void WhoTaskGroup::unregister_task(WhoTask *task)
{
    assert(!WhoYield2Idle::get_instance()->is_active());
    m_tasks.erase(std::remove(m_tasks.begin(), m_tasks.end(), task), m_tasks.end());
}

void WhoTaskGroup::register_task_group(WhoTaskGroup *task_group)
{
    assert(!WhoYield2Idle::get_instance()->is_active());
    m_task_groups.emplace_back(task_group);
}

void WhoTaskGroup::unregister_task_group(WhoTaskGroup *task_group)
{
    assert(!WhoYield2Idle::get_instance()->is_active());
    m_task_groups.erase(std::remove(m_task_groups.begin(), m_task_groups.end(), task_group), m_task_groups.end());
}

std::vector<WhoTask *> WhoTaskGroup::get_all_tasks()
{
    if (m_task_groups.empty()) {
        return m_tasks;
    }
    auto all_tasks = m_tasks;
    for (const auto &task_group : m_task_groups) {
        auto task_group_tasks = task_group->get_all_tasks();
        all_tasks.insert(all_tasks.end(), task_group_tasks.begin(), task_group_tasks.end());
    }
    return all_tasks;
}

void WhoTaskGroup::stop()
{
    auto tasks = get_all_tasks();
    std::vector<bool> flags;
    for (const auto &task : tasks) {
        flags.emplace_back(task->stop_async());
    }
    for (int i = 0; i < tasks.size(); i++) {
        if (flags[i]) {
            tasks[i]->wait_for_stopped(portMAX_DELAY);
        }
    }
    for (int i = 0; i < tasks.size(); i++) {
        if (flags[i]) {
            tasks[i]->cleanup_for_stopped();
        }
    }
}

void WhoTaskGroup::resume()
{
    auto tasks = get_all_tasks();
    for (const auto &task : tasks) {
        task->resume();
    }
}

void WhoTaskGroup::pause()
{
    auto tasks = get_all_tasks();
    std::vector<bool> flags;
    for (const auto &task : tasks) {
        flags.emplace_back(task->pause_async());
    }
    for (int i = 0; i < tasks.size(); i++) {
        if (flags[i]) {
            tasks[i]->wait_for_paused(portMAX_DELAY);
        }
    }
    for (int i = 0; i < tasks.size(); i++) {
        if (flags[i]) {
            tasks[i]->cleanup_for_paused();
        }
    }
}

void WhoTaskGroup::destroy()
{
    auto tasks = m_tasks;
    for (const auto &task : tasks) {
        delete task;
    }
    auto task_groups = m_task_groups;
    for (const auto &task_group : m_task_groups) {
        delete task_group;
    }
}
} // namespace task
} // namespace who
