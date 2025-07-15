#include "who_uvc.hpp"
#include "who_usb.hpp"
#include "who_uvc_cam.hpp"
#include <unordered_set>

#define UVC_DESC_DWFRAMEINTERVAL_TO_FPS(dwFrameInterval) \
    (((dwFrameInterval) != 0) ? 10000000.0f / ((float)(dwFrameInterval)) : 0)

static const char *TAG = "WhoUVC";

namespace who {
namespace uvc {
void WhoUVC::task()
{
    auto usb = usb::WhoUSB::get_instance();
    usb->run(4096, 15, 0);
    xEventGroupWaitBits(usb->get_event_group(), usb::WhoUSB::USB_HOST_INSTALLED, pdFALSE, pdFALSE, portMAX_DELAY);
    uvc_host_driver_config_t host_config = {};
    host_config.event_cb = &WhoUVC::uvc_host_event_cb;
    ESP_ERROR_CHECK(uvc_host_install(&host_config));
    xEventGroupSetBits(m_event_group, UVC_HOST_INSTALLED);
    while (true) {
        EventBits_t event_bits = xEventGroupWaitBits(m_event_group, TASK_STOP, pdTRUE, pdFALSE, 0);
        if (event_bits & TASK_STOP) {
            break;
        }
        uvc_host_handle_events(portMAX_DELAY);
    }
    xEventGroupSetBits(m_event_group, TASK_STOPPED);
    vTaskDelete(NULL);
}

bool WhoUVC::stop_async()
{
    if (task::WhoTaskBase::stop_async()) {
        // unblock uvc_host_handle_events()
        ESP_ERROR_CHECK(uvc_host_uninstall());
        return true;
    }
    return false;
}

void WhoUVC::print_uvc_devices()
{
    auto uvc_stream_fmt2str = [](enum uvc_host_stream_format fmt) -> const char * {
        switch (fmt) {
        case UVC_VS_FORMAT_MJPEG:
            return "MJPEG";
        case UVC_VS_FORMAT_YUY2:
            return "YUY2";
        case UVC_VS_FORMAT_H264:
            return "H264";
        case UVC_VS_FORMAT_H265:
            return "H265";
        default:
            return "UKN";
        }
    };
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    for (const auto &uvc_dev : m_uvc_devs) {
        ESP_LOGI(TAG, "UVC Device %04x:%04x", uvc_dev.vid, uvc_dev.pid);
        for (const auto &stream_info : uvc_dev.stream_info) {
            ESP_LOGI(TAG, "  Stream %d", stream_info.first);
            for (const auto &frame_info : stream_info.second) {
                ESP_LOGI(TAG,
                         "    %s %dx%d@%2.1fFPS",
                         uvc_stream_fmt2str(frame_info.format),
                         frame_info.h_res,
                         frame_info.v_res,
                         UVC_DESC_DWFRAMEINTERVAL_TO_FPS(frame_info.default_interval));
            }
        }
    }
    xSemaphoreGive(m_mutex);
}

void WhoUVC::uvc_host_event_cb(const uvc_host_driver_event_data_t *event, void *user_ctx)
{
    auto uvc = WhoUVC::get_instance();
    uvc->uvc_host_event_cb(event);
}

void WhoUVC::uvc_host_event_cb(const uvc_host_driver_event_data_t *event)
{
    assert(event->type == UVC_HOST_DRIVER_EVENT_DEVICE_CONNECTED);
    // get frame info of uvc stream
    size_t num_frame_info = event->device_connected.frame_info_num;
    std::vector<uvc_host_frame_info_t> frame_info(num_frame_info);
    ESP_ERROR_CHECK(uvc_host_get_frame_list(event->device_connected.dev_addr,
                                            event->device_connected.uvc_stream_index,
                                            (uvc_host_frame_info_t(*)[])frame_info.data(),
                                            &num_frame_info));
    assert(num_frame_info == event->device_connected.frame_info_num);

    // get connected usb devs
    uint8_t dev_addr_list[20];
    int num_devs;
    ESP_ERROR_CHECK(usb_host_device_addr_list_fill(sizeof(dev_addr_list), dev_addr_list, &num_devs));
    std::unordered_set<uint8_t> dev_addr_set(dev_addr_list, dev_addr_list + num_devs);

    // remove disconnected usb devs from uvc devs list
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    for (auto it = m_uvc_devs.begin(); it != m_uvc_devs.end();) {
        if (dev_addr_set.count(it->dev_addr) == 0) {
            it = m_uvc_devs.erase(it);
        } else {
            it++;
        }
    }
    xSemaphoreGive(m_mutex);

    auto uvc_dev = std::find_if(m_uvc_devs.begin(), m_uvc_devs.end(), [&event](const auto &dev) {
        return dev.dev_addr == event->device_connected.dev_addr;
    });
    if (uvc_dev == m_uvc_devs.end()) {
        // add new uvc dev to uvc devs list
        // get usb dev pid & vid, can not get usb_host_client_hdl from uvc, create a new one.
        usb_host_client_handle_t client_hdl = NULL;
        const usb_host_client_config_t client_config = {
            .is_synchronous = false,
            .max_num_event_msg = 3,
            .async = {.client_event_callback = &WhoUVC::dummy_usb_host_client_event_cb, .callback_arg = NULL}};
        ESP_ERROR_CHECK(usb_host_client_register(&client_config, &client_hdl));
        usb_device_handle_t dev_hdl = NULL;
        ESP_ERROR_CHECK(usb_host_device_open(client_hdl, event->device_connected.dev_addr, &dev_hdl));
        const usb_device_desc_t *dev_desc;
        ESP_ERROR_CHECK(usb_host_get_device_descriptor(dev_hdl, &dev_desc));
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        m_uvc_devs.emplace_back(event->device_connected.dev_addr,
                                dev_desc->idVendor,
                                dev_desc->idProduct,
                                stream_info_t({{event->device_connected.uvc_stream_index, frame_info}}));
        xSemaphoreGive(m_mutex);
        ESP_ERROR_CHECK(usb_host_device_close(client_hdl, dev_hdl));
        ESP_ERROR_CHECK(usb_host_client_deregister(client_hdl));
    } else {
        // add new uvc stream to uvc devs list
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        uvc_dev->stream_info[event->device_connected.uvc_stream_index] = frame_info;
        xSemaphoreGive(m_mutex);
    }
}
} // namespace uvc
} // namespace who
