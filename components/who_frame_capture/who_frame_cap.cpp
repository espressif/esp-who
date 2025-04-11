#include "who_frame_cap.hpp"
#include "esp_lcd_panel_ops.h"
#include "who_lcd.hpp"
#include "who_subscriber.hpp"
#include "bsp/config.h"

static const char *TAG = "WhoFrameCap";

namespace who {
namespace frame_cap {
void WhoFrameCap::task()
{
    while (true) {
        EventBits_t event_bits = xEventGroupGetBits(m_event_group);
        if (event_bits & STOP) {
            set_and_clear_bits(TERMINATE, RUNNING | STOP);
            break;
        } else if (event_bits & PAUSE) {
            set_and_clear_bits(BLOCKING, RUNNING | PAUSE);
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, RESUME | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & STOP) {
                set_and_clear_bits(TERMINATE, BLOCKING);
                break;
            } else {
                set_and_clear_bits(RUNNING, BLOCKING);
                m_cam->cam_fb_get();
                m_cam->cam_fb_return();
            }
        }
        m_cam->cam_fb_get();
        on_new_frame();
        m_cam->cam_fb_return();
    }
    vTaskDelete(NULL);
}

bool WhoFrameCap::run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID)
{
    if (!m_cam) {
        ESP_LOGE(TAG, "Cam is nullptr, please call set_cam() first.");
        return false;
    }
    return WhoPublisher::run(uxStackDepth, uxPriority, xCoreID);
}

void WhoFrameCap::set_new_frame_bits()
{
    for (const auto &subscriber : m_elements) {
        EventBits_t event_bits = xEventGroupGetBits(m_event_group);
        if (!(event_bits & TERMINATE) && !(event_bits & PAUSE)) {
            xEventGroupSetBits(subscriber->m_event_group, NEW_FRAME);
        }
    }
}

void WhoFrameCap::on_new_frame()
{
    set_new_frame_bits();
}

void WhoFrameCapLCD::run_lcd_display_cbs(who::cam::cam_fb_t *fb)
{
    for (const auto &subscriber : m_elements) {
        EventBits_t event_bits = xEventGroupGetBits(m_event_group);
        if (!(event_bits & TERMINATE) && !(event_bits & PAUSE)) {
            subscriber->lcd_display_cb(fb);
        }
    }
}

void WhoFrameCapLCD::on_new_frame()
{
    set_new_frame_bits();
    auto fb = m_cam->cam_fb_peek(m_display_back_frame);
#if BSP_CONFIG_NO_GRAPHIC_LIB
    run_lcd_display_cbs(fb);
    esp_lcd_panel_draw_bitmap(who::lcd::LCD::s_panel_handle, 0, 0, fb->width, fb->height, fb->buf);
#else
    bsp_display_lock(0);
    lv_canvas_set_buffer(who::lcd::LCD::s_canvas, fb->buf, fb->width, fb->height, LV_COLOR_FORMAT_NATIVE);
    run_lcd_display_cbs(fb);
    bsp_display_unlock();
#endif
}
} // namespace frame_cap
} // namespace who
