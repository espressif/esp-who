#pragma once
#include "who_cam_define.hpp"
#include "who_container.hpp"
#include <freertos/FreeRTOS.h>
#include <string>

namespace who {
typedef enum {
    // WhoTask
    RUNNING = 1 << 0,
    BLOCKING = 1 << 1,
    TERMINATE = 1 << 2,
    STOP = 1 << 3,
    PAUSE = 1 << 4,
    RESUME = 1 << 5,
    WHO_TASK_LAST = 1 << 6,

    // WhoFramCap
    NEW_FRAME = 1 << 6,

    // WhoRecognition
    RECOGNIZE = 1 << 7,
    ENROLL = 1 << 8,
    DELETE = 1 << 9,
} event_type_t;

class WhoTaskBase {
public:
    WhoTaskBase(const std::string &name) : m_name(name), m_event_group(xEventGroupCreate())
    {
        xEventGroupSetBits(m_event_group, TERMINATE);
    }
    virtual ~WhoTaskBase() { vEventGroupDelete(m_event_group); }
    virtual bool run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID);
    virtual bool stop();
    virtual bool pause();
    virtual bool resume();
    void set_and_clear_bits(EventBits_t bit_to_clear, EventBits_t bit_to_set);

    std::string m_name;
    EventGroupHandle_t m_event_group;

private:
    virtual void task() = 0;
    static void task(void *args);
};

class WhoTask : public WhoTaskBase {
public:
    using WhoTaskBase::WhoTaskBase;
    virtual bool run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID);
};

class WhoTasks : public WhoContainer<WhoTask *> {
public:
    virtual bool run() = 0;
    virtual bool stop() { return operate_all(&WhoTask::stop); };
    virtual bool pause() { return operate_all(&WhoTask::pause); };
    virtual bool resume() { return operate_all(&WhoTask::resume); };

private:
    template <typename Func>
    bool operate_all(Func func) const
    {
        bool ret = true;
        for (const auto &element : m_elements) {
            ret &= (element->*func)();
        }
        return ret;
    }
};
} // namespace who
