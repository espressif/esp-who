#pragma once
#include "who_task.hpp"
#include <vector>

namespace who {
class WhoYield2Idle : public WhoTaskBase, public WhoContainer<WhoTask *, true> {
public:
    static WhoYield2Idle *get_instance(const BaseType_t xCoreID);
    using WhoTaskBase::run;
    static bool run();
    bool stop() override;
    bool monitor(WhoTask *task);

private:
    WhoYield2Idle(const std::string &name, const BaseType_t xCoreID) : WhoTaskBase(name), m_coreid(xCoreID) {};
    WhoYield2Idle(const WhoYield2Idle &) = delete;
    WhoYield2Idle &operator=(const WhoYield2Idle &) = delete;
    void task() override;
    const BaseType_t m_coreid;
};
} // namespace who
