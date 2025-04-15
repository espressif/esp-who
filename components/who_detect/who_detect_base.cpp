#include "who_detect_base.hpp"

namespace who {
namespace detect {
void WhoDetectBase::set_fps(float fps)
{
    if (fps > 0) {
        m_interval = pdMS_TO_TICKS((int)(1000.f / fps));
    }
}

void WhoDetectBase::detect_loop()
{
    TickType_t last_wake_time = xTaskGetTickCount();
    set_and_clear_bits(BLOCKING, RUNNING);
    while (true) {
        EventBits_t event_bits =
            xEventGroupWaitBits(m_event_group, NEW_FRAME | PAUSE | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bits & STOP) {
            set_and_clear_bits(TERMINATE, BLOCKING);
            break;
        } else if (event_bits & PAUSE) {
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, RESUME | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & STOP) {
                set_and_clear_bits(TERMINATE, BLOCKING);
                break;
            } else {
                continue;
            }
        }
        set_and_clear_bits(RUNNING, BLOCKING);
        auto fb = m_frame_cap->get_cam()->cam_fb_peek();
        struct timeval timestamp = fb->timestamp;
        auto &res = m_detect->run(who::cam::fb2img(fb));
        on_new_detect_result({res, timestamp, fb});
        set_and_clear_bits(BLOCKING, RUNNING);
        if (m_interval) {
            vTaskDelayUntil(&last_wake_time, m_interval);
        }
    }
    vTaskDelete(NULL);
}

#if CONFIG_IDF_TARGET_ESP32P4
void WhoDetectBase::ppa_detect_loop()
{
    auto ppa_cam = static_cast<who::cam::WhoP4PPACam *>(m_frame_cap->get_cam());
    float ppa_inv_scale_x = 1.f / ppa_cam->m_ppa_scale_x;
    float ppa_inv_scale_y = 1.f / ppa_cam->m_ppa_scale_y;
    auto fb = ppa_cam->cam_fb_peek();
    int fb_width = fb->width;
    int fb_height = fb->height;
    TickType_t last_wake_time = xTaskGetTickCount();
    set_and_clear_bits(BLOCKING, RUNNING);
    while (true) {
        EventBits_t event_bits =
            xEventGroupWaitBits(m_event_group, NEW_FRAME | PAUSE | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bits & STOP) {
            set_and_clear_bits(TERMINATE, BLOCKING);
            break;
        } else if (event_bits & PAUSE) {
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, RESUME | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & STOP) {
                set_and_clear_bits(TERMINATE, BLOCKING);
                break;
            } else {
                continue;
            }
        }
        set_and_clear_bits(RUNNING, BLOCKING);
        auto ppa_fb = ppa_cam->ppa_cam_fb_peek();
        struct timeval timestamp = ppa_fb->timestamp;
        auto &res = m_detect->run(who::cam::fb2img(ppa_fb));
        for (auto &r : res) {
            r.box[0] = r.box[0] * ppa_inv_scale_x;
            r.box[1] = r.box[1] * ppa_inv_scale_y;
            r.box[2] = r.box[2] * ppa_inv_scale_x;
            r.box[3] = r.box[3] * ppa_inv_scale_y;
            r.limit_box(fb_width, fb_height);
            if (!r.keypoint.empty()) {
                assert(r.keypoint.size() == 10);
                for (int i = 0; i < 5; i++) {
                    r.keypoint[2 * i] *= ppa_inv_scale_x;
                    r.keypoint[2 * i + 1] *= ppa_inv_scale_y;
                }
                r.limit_keypoint(fb_width, fb_height);
            }
        }
        on_new_detect_result({res, timestamp, ppa_fb});
        set_and_clear_bits(BLOCKING, RUNNING);
        if (m_interval) {
            vTaskDelayUntil(&last_wake_time, m_interval);
        }
    }
    vTaskDelete(NULL);
}
#endif

void WhoDetectBase::task()
{
#if CONFIG_IDF_TARGET_ESP32P4
    if (m_frame_cap->get_cam()->get_type() == "WhoP4PPACam") {
        ppa_detect_loop();
    } else {
        detect_loop();
    }
#else
    detect_loop();
#endif
}

bool WhoDetectBase::run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID)
{
    if (!m_detect) {
        ESP_LOGE("WhoDetect", "detect model is nullptr, please call set_model() first.");
        return false;
    }
    return WhoSubscriber::run(uxStackDepth, uxPriority, xCoreID);
}

bool WhoDetectBase::stop()
{
    if (xEventGroupGetBits(m_event_group) & TERMINATE) {
        return false;
    }
    xEventGroupSetBits(m_event_group, STOP);
    xTaskAbortDelay(m_task_handle);
    xEventGroupWaitBits(m_event_group, TERMINATE, pdFALSE, pdFALSE, portMAX_DELAY);
    return true;
}
} // namespace detect
} // namespace who
