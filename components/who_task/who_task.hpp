#pragma once
#include <freertos/FreeRTOS.h>
#include <string>
#include <vector>

#define ALL_EVENT_BITS ~((EventBits_t)0) & ~(0xff << (sizeof(EventBits_t) - 1) * 8)

namespace who {
typedef enum {
    // WhoTask
    STOPPED = 1 << 0,
    PAUSED = 1 << 1,

    STOP = 1 << 2,
    PAUSE = 1 << 3,
    RESUME = 1 << 4,

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
    WhoTaskBase(const std::string &name) :
        m_name(name), m_event_group(xEventGroupCreate()), m_mutex(xSemaphoreCreateMutex())
    {
        xEventGroupSetBits(m_event_group, STOPPED);
    }
    virtual ~WhoTaskBase()
    {
        vEventGroupDelete(m_event_group);
        vSemaphoreDelete(m_mutex);
    }
    virtual bool run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID);
    virtual bool stop();
    virtual bool stop_async();
    virtual void wait_for_stopped(TickType_t timeout);
    virtual bool resume();
    virtual bool pause();
    virtual bool pause_async();
    virtual void wait_for_paused(TickType_t timeout);
    virtual void cleanup() {}
    virtual void cleanup_for_paused();
    virtual void cleanup_for_stopped();
    bool is_active();
    std::string get_name() { return m_name; }
    EventGroupHandle_t get_event_group() { return m_event_group; }
    TaskHandle_t get_task_handle() { return m_task_handle; }

protected:
    std::string m_name;
    EventGroupHandle_t m_event_group;
    TaskHandle_t m_task_handle;

private:
    virtual void task() = 0;
    static void task(void *args);
    SemaphoreHandle_t m_mutex;
};

class WhoTask : public WhoTaskBase {
public:
    WhoTask(const std::string &name);
    ~WhoTask();
    bool run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID) override;
    bool resume() override;
    BaseType_t get_coreid() { return m_coreid; }
    SemaphoreHandle_t get_mutex() { return m_mutex; }

private:
    BaseType_t m_coreid;
    SemaphoreHandle_t m_mutex;
};

class WhoTaskGroup {
public:
    virtual ~WhoTaskGroup() {}
    void register_task(WhoTask *task);
    void unregister_task(WhoTask *task);
    void register_task_group(WhoTaskGroup *task_group);
    void unregister_task_group(WhoTaskGroup *task_group);
    std::vector<WhoTask *> get_all_tasks();
    void stop();
    void resume();
    void pause();
    void destroy();

private:
    std::vector<WhoTask *> m_tasks;
    std::vector<WhoTaskGroup *> m_task_groups;
};
} // namespace who
