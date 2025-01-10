#include "who_detect.hpp"
#include "display_func_manager.hpp"
#include "helper.hpp"
#include "who_cam_lcd.hpp"

static const char *TAG = "who_detect";

namespace who {
namespace app {

WhoDetect::WhoDetect(dl::detect::Detect *detect, who::cam::Cam *cam, const std::string &name, EventBits_t task_bit) :
    m_mutex(xSemaphoreCreateMutex()), m_detect(detect), m_cam(cam), m_name(name), m_task_bit(task_bit)
{
}

#if !CONFIG_USE_PPA_CAM
void WhoDetect::task(void *args)
{
    WhoDetect *self = (WhoDetect *)args;
    struct timeval timestamp;
    int64_t start = esp_timer_get_time();
    // if detect model takes more than 1 second, task_wt_interval should be ajusted.
    int64_t task_wt_interval = (CONFIG_ESP_TASK_WDT_TIMEOUT_S - 1) * 1000000;
    while (true) {
        if (self->m_task_bit) {
            xEventGroupWaitBits(WhoCamLCD::s_event_group, self->m_task_bit, pdTRUE, pdFALSE, portMAX_DELAY);
        }
        auto fb = self->m_cam->cam_fb_peek();
        timestamp = fb->timestamp;
        auto &res = self->m_detect->run(who::cam::fb2img(fb));
        xSemaphoreTake(self->m_mutex, portMAX_DELAY);
        self->m_results.push({res, timestamp});
        xSemaphoreGive(self->m_mutex);
        int64_t end = esp_timer_get_time();
        if (end - start >= task_wt_interval) {
            vTaskDelay(pdMS_TO_TICKS(10));
            start = esp_timer_get_time();
        }
    }
}
#else
void WhoDetect::task(void *args)
{
    WhoDetect *self = (WhoDetect *)args;
    struct timeval timestamp;
    int64_t start = esp_timer_get_time();
    // if detect model takes more than 1 second, task_wt_interval should be ajusted.
    int64_t task_wt_interval = (CONFIG_ESP_TASK_WDT_TIMEOUT_S - 1) * 1000000;
    auto ppa_cam = static_cast<who::cam::PPAP4Cam *>(self->m_cam);
    float ppa_inv_scale_x = 1.f / ppa_cam->m_ppa_scale_x;
    float ppa_inv_scale_y = 1.f / ppa_cam->m_ppa_scale_y;
    auto fb = ppa_cam->cam_fb_peek();
    int fb_width = fb->width;
    int fb_height = fb->height;
    while (true) {
        if (self->m_task_bit) {
            xEventGroupWaitBits(WhoCamLCD::s_event_group, self->m_task_bit, pdTRUE, pdFALSE, portMAX_DELAY);
        }
        auto ppa_fb = ppa_cam->ppa_cam_fb_peek();
        timestamp = ppa_fb->timestamp;
        auto &res = self->m_detect->run(fb2img(ppa_fb));
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
        xSemaphoreTake(self->m_mutex, portMAX_DELAY);
        self->m_results.push({res, timestamp});
        xSemaphoreGive(self->m_mutex);
        int64_t end = esp_timer_get_time();
        if (end - start >= task_wt_interval) {
            vTaskDelay(pdMS_TO_TICKS(10));
            start = esp_timer_get_time();
        }
    }
}
#endif

void WhoDetect::display(who::cam::cam_fb_t *fb)
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    // Try to sync camera frame and result, skip the future result.
    struct timeval t1 = fb->timestamp;
    bool display = false;
    result_t result;
    while (!m_results.empty()) {
        result = m_results.front();
        if (!compare_timestamp(t1, result.timestamp)) {
            m_results.pop();
            display = true;
        } else {
            break;
        }
    }
    xSemaphoreGive(m_mutex);
    if (display) {
        draw_detect_results(fb, result.det_res);
    }
}

void WhoDetect::run()
{
    auto display_func_manager = DisplayFuncManager::get_instance();
    display_func_manager->register_display_func(m_name, std::bind(&WhoDetect::display, this, std::placeholders::_1));
    if (m_task_bit) {
        WhoCamLCD::register_task_bit(m_task_bit);
    }
    if (xTaskCreatePinnedToCore(task, m_name.c_str(), 2560, this, 2, nullptr, 1) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create WhoDet task.\n");
    }
}

} // namespace app
} // namespace who
