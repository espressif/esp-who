#include "who_cam_lcd.hpp"
#include "display_func_manager.hpp"

static const char *TAG = "WhoCamLCD";

namespace who {
namespace app {
EventGroupHandle_t WhoCamLCD::s_event_group = xEventGroupCreate();
EventBits_t WhoCamLCD::s_task_bits = 0;

void WhoCamLCD::task(void *args)
{
    WhoCamLCD *self = (WhoCamLCD *)args;
    auto display_func_manager = DisplayFuncManager::get_instance();

#if CONFIG_IDF_TARGET_ESP32P4
    int n = self->m_cam->m_fb_count - 2;
#elif CONFIG_IDF_TARGET_ESP32S3
    int n = self->m_cam->m_fb_count - 1;
#endif
    for (int i = 0; i < n; i++) {
        self->m_cam->cam_fb_get();
    }
    while (true) {
        if (s_task_bits) {
            xEventGroupSetBits(s_event_group, s_task_bits);
        }
        self->m_cam->cam_fb_get();
        who::cam::cam_fb_t *fb = self->m_cam->cam_fb_peek(false);
        display_func_manager->display(fb);
        self->m_cam->cam_fb_return();
    }
}

void WhoCamLCD::run()
{
    if (xTaskCreatePinnedToCore(task, "CamLCD", 4096, this, 2, nullptr, 0) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create CamLCD task.\n");
    };
}

} // namespace app
} // namespace who
