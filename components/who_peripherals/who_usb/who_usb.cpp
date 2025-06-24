#include "who_usb.hpp"
#include "esp_log.h"
#include "usb/usb_host.h"

static const char *TAG = "WhoUSB";

namespace who {
namespace usb {
void WhoUSB::task()
{
    const usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));
    xEventGroupSetBits(m_event_group, USB_HOST_INSTALLED);
    while (true) {
        EventBits_t event_bits = xEventGroupWaitBits(m_event_group, STOP, pdTRUE, pdFALSE, 0);
        if (event_bits & STOP) {
            break;
        }
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            usb_host_device_free_all();
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            ESP_LOGI(TAG, "USB: All devices freed");
            // Continue handling USB events to allow device reconnection
        }
    }
    ESP_ERROR_CHECK(usb_host_uninstall());
    xEventGroupSetBits(m_event_group, STOPPED);
    vTaskDelete(NULL);
}

bool WhoUSB::stop_async()
{
    if (WhoTaskBase::stop_async()) {
        usb_host_lib_unblock();
        return true;
    }
    return false;
}
} // namespace usb
} // namespace who
