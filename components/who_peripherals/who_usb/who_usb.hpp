#pragma once
#include "who_task.hpp"

namespace who {
namespace usb {
class WhoUSB : public WhoTaskBase {
public:
    static WhoUSB *get_instance()
    {
        static WhoUSB instance;
        return &instance;
    }

    typedef enum { USB_HOST_INSTALLED = WHO_TASK_LAST } event_type_t;

    bool stop_async() override;

private:
    WhoUSB() : WhoTaskBase("USB") {}
    WhoUSB(const WhoUSB &) = delete;
    WhoUSB &operator=(const WhoUSB &) = delete;
    void task() override;
};
} // namespace usb
} // namespace who
