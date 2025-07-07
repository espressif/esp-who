#include "who_detect.hpp"

namespace who {
namespace detect {
WhoDetect::WhoDetect(const std::string &name, frame_cap::WhoFrameCapNode *frame_cap_node) :
    WhoTask(name),
    m_frame_cap_node(frame_cap_node),
    m_model(nullptr),
    m_interval(0),
    m_inv_rescale_x(0),
    m_inv_rescale_y(0),
    m_rescale_max_w(0),
    m_rescale_max_h(0),
    m_result_cb_mutex(xSemaphoreCreateRecursiveMutex())
{
    frame_cap_node->add_new_frame_signal_subscriber(this);
}

WhoDetect::~WhoDetect()
{
    vSemaphoreDelete(m_result_cb_mutex);
    if (m_model) {
        delete m_model;
    }
}

void WhoDetect::set_model(dl::detect::Detect *model)
{
    m_model = model;
}

void WhoDetect::set_rescale_params(float rescale_x, float rescale_y, uint16_t rescale_max_w, uint16_t rescale_max_h)
{
    m_inv_rescale_x = 1.f / rescale_x;
    m_inv_rescale_y = 1.f / rescale_y;
    m_rescale_max_w = rescale_max_w;
    m_rescale_max_h = rescale_max_h;
}

void WhoDetect::set_fps(float fps)
{
    if (fps > 0) {
        m_interval = pdMS_TO_TICKS((int)(1000.f / fps));
    }
}

void WhoDetect::set_detect_result_cb(const std::function<void(const result_t &result)> &result_cb)
{
    xSemaphoreTakeRecursive(m_result_cb_mutex, portMAX_DELAY);
    m_result_cb = result_cb;
    xSemaphoreGiveRecursive(m_result_cb_mutex);
}

void WhoDetect::set_cleanup_func(const std::function<void()> &cleanup_func)
{
    m_cleanup = cleanup_func;
}

void WhoDetect::task()
{
    TickType_t last_wake_time = xTaskGetTickCount();
    while (true) {
        EventBits_t event_bits =
            xEventGroupWaitBits(m_event_group, NEW_FRAME | PAUSE | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bits & STOP) {
            break;
        } else if (event_bits & PAUSE) {
            xEventGroupSetBits(m_event_group, PAUSED);
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, RESUME | STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & STOP) {
                break;
            } else {
                last_wake_time = xTaskGetTickCount();
                continue;
            }
        }
        auto fb = m_frame_cap_node->cam_fb_peek();
        struct timeval timestamp = fb->timestamp;
        dl::image::img_t img = static_cast<dl::image::img_t>(*fb);
        auto &res = m_model->run(img);
        if (m_inv_rescale_x && m_inv_rescale_y && m_rescale_max_w && m_rescale_max_h) {
            rescale_detect_result(res);
        }
        if (m_result_cb) {
            xSemaphoreTakeRecursive(m_result_cb_mutex, portMAX_DELAY);
            m_result_cb({res, timestamp, img});
            xSemaphoreGiveRecursive(m_result_cb_mutex);
        }
        if (m_interval) {
            vTaskDelayUntil(&last_wake_time, m_interval);
        }
    }
    xEventGroupSetBits(m_event_group, STOPPED);
    vTaskDelete(NULL);
}

void WhoDetect::rescale_detect_result(std::list<dl::detect::result_t> &result)
{
    for (auto &r : result) {
        r.box[0] *= m_inv_rescale_x;
        r.box[1] *= m_inv_rescale_y;
        r.box[2] *= m_inv_rescale_x;
        r.box[3] *= m_inv_rescale_y;
        r.limit_box(m_rescale_max_w, m_rescale_max_h);
        if (!r.keypoint.empty()) {
            assert(r.keypoint.size() == 10);
            for (int i = 0; i < 5; i++) {
                r.keypoint[2 * i] *= m_inv_rescale_x;
                r.keypoint[2 * i + 1] *= m_inv_rescale_y;
            }
            r.limit_keypoint(m_rescale_max_w, m_rescale_max_h);
        }
    }
}

bool WhoDetect::run(const configSTACK_DEPTH_TYPE uxStackDepth, UBaseType_t uxPriority, const BaseType_t xCoreID)
{
    if (!m_model) {
        ESP_LOGE("WhoDetect", "detect model is nullptr, please call set_model() first.");
        return false;
    }
    return WhoTask::run(uxStackDepth, uxPriority, xCoreID);
}

bool WhoDetect::stop_async()
{
    if (WhoTask::stop_async()) {
        xTaskAbortDelay(m_task_handle);
        return true;
    }
    return false;
}

bool WhoDetect::pause_async()
{
    if (WhoTask::pause_async()) {
        xTaskAbortDelay(m_task_handle);
        return true;
    }
    return false;
}

void WhoDetect::cleanup()
{
    if (m_cleanup) {
        m_cleanup();
    }
}
} // namespace detect
} // namespace who
