#pragma once
#include "who_task.hpp"

namespace who {
namespace usb {
class WhoUSB : public task::WhoTaskBase {
public:
    static inline constexpr EventBits_t USB_HOST_INSTALLED = TASK_EVENT_BIT_LAST;

    static WhoUSB *get_instance()
    {
        static WhoUSB instance;
        return &instance;
    }

    bool stop_async() override;

private:
    WhoUSB() : task::WhoTaskBase("USB") {}
    WhoUSB(const WhoUSB &) = delete;
    WhoUSB &operator=(const WhoUSB &) = delete;
    void task() override;
};
} // namespace usb
} // namespace who
