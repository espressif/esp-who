#pragma once
#include "who_task.hpp"

namespace who {
namespace app {
class WhoApp {
public:
    virtual ~WhoApp();
    virtual bool run() = 0;
    virtual bool pause();
    virtual bool resume();
    virtual bool stop();

protected:
    void add_task(WhoTask *task) { m_task_group.register_task(task); }
    void add_task_group(WhoTaskGroup *task_group) { m_task_group.register_task_group(task_group); }

private:
    WhoTaskGroup m_task_group;
};
} // namespace app
} // namespace who
