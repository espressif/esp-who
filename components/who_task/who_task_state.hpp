#pragma once
#include "who_task.hpp"
#include <string>
#include <vector>

namespace who {
namespace task {
class WhoTaskState : public WhoTask {
public:
    WhoTaskState(int interval = 2);
    void task() override;
    bool stop_async() override;
    bool pause_async() override;

private:
    void print_task_status();
    std::vector<std::string> m_task_state;
    int m_interval;
};
} // namespace task
} // namespace who
