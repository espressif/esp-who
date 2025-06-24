#pragma once
#include "who_task.hpp"
#include <vector>

namespace who {
class WhoTask;
class WhoYield2IdleTaskGroup : public WhoTaskGroup {
public:
    void select_tasks(bool yield2idle0, bool yield2idle1);
    void pause_selected_tasks();
    void resume_selected_tasks();
    void lock_selected_tasks();
    void unlock_selected_tasks();
    std::vector<WhoTask *> get_tasks_by_coreid(BaseType_t coreid);

private:
    std::vector<WhoTask *> m_selected_tasks;
};
class WhoYield2Idle : public WhoTaskBase {
public:
    static WhoYield2Idle *get_instance();
    using WhoTaskBase::run;
    bool run();
    void start_monitor(WhoTask *task);
    void end_monitor(WhoTask *task);
    bool stop_async() override;
    bool pause_async() override;

private:
    WhoYield2Idle(const std::string &name) : WhoTaskBase(name) {};
    WhoYield2Idle(const WhoYield2Idle &) = delete;
    WhoYield2Idle &operator=(const WhoYield2Idle &) = delete;
    void task() override;
    WhoYield2IdleTaskGroup m_task_group;
    static int s_idle0_cnt;
    static int s_idle1_cnt;
    static bool idle0_cb(void);
    static bool idle1_cb(void);
    static void reset_idle0_cnt();
    static void reset_idle1_cnt();
};
} // namespace who
