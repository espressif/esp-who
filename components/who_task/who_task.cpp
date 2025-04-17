#include "who_task.hpp"
#include "who_yield2idle.hpp"
#include <esp_log.h>

static const char *TAG = "WhoTask";

namespace who {
bool WhoTaskBase::run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID)
{
    if (xEventGroupGetBits(m_event_group) & (RUNNING | BLOCKING)) {
        return false;
    }
    set_and_clear_bits(RUNNING, TERMINATE);
    if (xTaskCreatePinnedToCore(task, m_name.c_str(), uxStackDepth, this, uxPriority, &m_task_handle, xCoreID) !=
        pdPASS) {
        ESP_LOGE(TAG, "Failed to create task.\n");
        set_and_clear_bits(TERMINATE, RUNNING);
        return false;
    }
    return true;
}

bool WhoTaskBase::stop()
{
    if (xEventGroupGetBits(m_event_group) & TERMINATE) {
        return false;
    }
    xEventGroupSetBits(m_event_group, STOP);
    xEventGroupWaitBits(m_event_group, TERMINATE, pdFALSE, pdFALSE, portMAX_DELAY);
    return true;
}

bool WhoTaskBase::pause()
{
    if (xEventGroupGetBits(m_event_group) & TERMINATE) {
        return false;
    }
    xEventGroupSetBits(m_event_group, PAUSE);
    return true;
}

bool WhoTaskBase::resume()
{
    if (xEventGroupGetBits(m_event_group) & TERMINATE) {
        return false;
    }
    xEventGroupSetBits(m_event_group, RESUME);
    return true;
}

void WhoTaskBase::set_and_clear_bits(EventBits_t bit_to_set, EventBits_t bit_to_clear)
{
    xEventGroupSetBits(m_event_group, bit_to_set);
    xEventGroupClearBits(m_event_group, bit_to_clear);
}

void WhoTaskBase::task(void *args)
{
    WhoTaskBase *self = static_cast<WhoTaskBase *>(args);
    self->task();
}

bool WhoTask::run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID)
{
    // Never run other tasks when yield2idle running.
    auto yield2idle = WhoYield2Idle::get_instance(xCoreID);
    EventBits_t event_bits =
        xEventGroupWaitBits(yield2idle->get_event_group(), BLOCKING | TERMINATE, pdFALSE, pdFALSE, portMAX_DELAY);
    if (event_bits & BLOCKING) {
        yield2idle->monitor(this);
    }
    return WhoTaskBase::run(uxStackDepth, uxPriority, xCoreID);
}

} // namespace who
