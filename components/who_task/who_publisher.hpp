#pragma once
#include "who_task.hpp"

namespace who {
class WhoSubscriber;
class WhoPublisher : public WhoTask, public WhoContainer<WhoSubscriber *> {
public:
    using WhoTask::WhoTask;
};
} // namespace who
