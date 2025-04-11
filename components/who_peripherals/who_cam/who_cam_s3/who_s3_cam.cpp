#include "who_s3_cam.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "bsp/esp-bsp.h"

static const char *TAG = "who_cam_s3";

namespace who {
namespace cam {

WhoS3Cam::WhoS3Cam(const pixformat_t pixel_format,
                   const framesize_t frame_size,
                   const uint8_t fb_count,
                   bool horizontal_flip) :
    WhoCam(fb_count), m_mutex(xSemaphoreCreateMutex())
{
    if (fb_count < 2) {
        ESP_LOGE("WhoS3Cam", "fb_count is at least 2.");
    }
    ESP_ERROR_CHECK(bsp_i2c_init());
    camera_config_t camera_config = BSP_CAMERA_DEFAULT_CONFIG;
    camera_config.pixel_format = pixel_format;
    camera_config.frame_size = frame_size;
    camera_config.fb_count = fb_count;
    if (pixel_format == PIXFORMAT_JPEG) {
        camera_config.xclk_freq_hz = 20000000;
    }
    ESP_ERROR_CHECK(esp_camera_init(&camera_config));
    sensor_t *s = esp_camera_sensor_get();
    if (s->id.PID == OV3660_PID || s->id.PID == OV2640_PID || s->id.PID == GC032A_PID) {
        s->set_vflip(s, 1);
    }
    if (!horizontal_flip) {
        ESP_ERROR_CHECK(set_horizontal_flip());
    }
}

WhoS3Cam::~WhoS3Cam()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    while (!m_buf_queue.empty()) {
        esp_camera_fb_return(m_buf_queue.front());
        m_buf_queue.pop_front();
    }
    xSemaphoreGive(m_mutex);
}

cam_fb_t *WhoS3Cam::cam_fb_get()
{
    if (m_buf_queue.size() >= m_fb_count) {
        ESP_LOGW(TAG, "Can not get more frame buffer.");
        return nullptr;
    }
    cam_fb_t *fb = esp_camera_fb_get();
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_buf_queue.push_back(fb);
    xSemaphoreGive(m_mutex);
    return fb;
}

cam_fb_t *WhoS3Cam::cam_fb_peek(bool back)
{
    return cam_fb_peek(back, 1)[0];
}

std::vector<cam_fb_t *> WhoS3Cam::cam_fb_peek(bool back, int num)
{
    if (m_buf_queue.size() < num) {
        ESP_LOGW(TAG, "Unable to peek %d frame buffer from a frame buffer queue of size %d .", num, m_buf_queue.size());
        return {};
    }
    std::vector<cam_fb_t *> fbs;
    if (back) {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        for (auto it = m_buf_queue.end() - num; it != m_buf_queue.end(); it++) {
            fbs.emplace_back(*it);
        }
        xSemaphoreGive(m_mutex);
    } else {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        for (auto it = m_buf_queue.begin(); it != m_buf_queue.begin() + num; it++) {
            fbs.emplace_back(*it);
        }
        xSemaphoreGive(m_mutex);
    }
    return fbs;
}

void WhoS3Cam::cam_fb_return()
{
    if (m_buf_queue.empty()) {
        ESP_LOGW(TAG, "Can not return more frame buffer.");
        return;
    }
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    esp_camera_fb_return(m_buf_queue.front());
    m_buf_queue.pop_front();
    xSemaphoreGive(m_mutex);
}

esp_err_t WhoS3Cam::set_horizontal_flip()
{
    sensor_t *s = esp_camera_sensor_get();
    int ret = s->set_hmirror(s, 1);
    return (ret == 0) ? ESP_OK : ESP_FAIL;
}

} // namespace cam
} // namespace who
