#pragma once
#include "who_task.hpp"
#include <list>
#include <unordered_map>
#include <vector>
#include "usb/uvc_host.h"

namespace who {
namespace cam {
class WhoUVCCam;
}
namespace uvc {
class WhoUVC : public task::WhoTaskBase {
public:
    static inline constexpr EventBits_t UVC_HOST_INSTALLED = TASK_EVENT_BIT_LAST;
    using stream_info_t = std::unordered_map<uint8_t, std::vector<uvc_host_frame_info_t>>;
    typedef struct {
        uint8_t dev_addr;
        uint16_t vid;
        uint16_t pid;
        stream_info_t stream_info;
    } uvc_dev_t;

    static WhoUVC *get_instance()
    {
        static WhoUVC instance;
        return &instance;
    }

    bool stop_async() override;
    void print_uvc_devices();

private:
    WhoUVC() : task::WhoTaskBase("UVC"), m_mutex(xSemaphoreCreateMutex()) {}
    WhoUVC(const WhoUVC &) = delete;
    WhoUVC &operator=(const WhoUVC &) = delete;
    void task() override;
    static void uvc_host_event_cb(const uvc_host_driver_event_data_t *event, void *user_ctx);
    void uvc_host_event_cb(const uvc_host_driver_event_data_t *event);
    static void dummy_usb_host_client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg) {}
    std::list<uvc_dev_t> m_uvc_devs;
    SemaphoreHandle_t m_mutex;
};
} // namespace uvc
} // namespace who
