#pragma once
#include "who_cam_define.hpp"
#include "who_task.hpp"

namespace who {
class WhoSubscriber : public WhoTask {
public:
    using WhoTask::WhoTask;
    virtual void lcd_display_cb(who::cam::cam_fb_t *fb) {};
};
} // namespace who
