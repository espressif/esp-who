#include "who_p4_cam.hpp"
#include "esp_cache.h"
#include "esp_timer.h"
#include "esp_private/esp_cache_private.h"

static const char *TAG = "WhoP4Cam";

namespace who {
namespace cam {

WhoP4Cam::WhoP4Cam(const video_pix_fmt_t video_pix_fmt,
                   const uint8_t fb_count,
                   const v4l2_memory fb_mem_type,
                   bool horizontal_flip) :
    ESPVideo(video_pix_fmt, fb_count, fb_mem_type, horizontal_flip),
    m_mutex(xSemaphoreCreateMutex()),
    m_cam_fbs(new cam_fb_t[fb_count])
{
    if (fb_count < 3) {
        ESP_LOGE("WhoP4Cam", "fb_count is at least 3.");
    }
    video_init();
}

WhoP4Cam::~WhoP4Cam()
{
    video_deinit();
    if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
        for (int i = 0; i < m_fb_count; i++) {
            heap_caps_free(m_cam_fbs[i].buf);
        }
    }
    delete[] m_cam_fbs;
    vSemaphoreDelete(m_mutex);
}

cam_fb_t *WhoP4Cam::cam_fb_get()
{
    if (m_buf_queue.size() + 1 >= m_fb_count) {
        ESP_LOGW(TAG, "Can not get more frame buffer.");
        return nullptr;
    }
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = m_fb_mem_type;
    if (ioctl(m_fd, VIDIOC_DQBUF, &buf) != 0) {
        ESP_LOGE(TAG, "failed to receive video frame");
        return nullptr;
    }
    int64_t us = esp_timer_get_time();
    cam_fb_t *fb = m_cam_fbs + buf.index;
    fb->timestamp.tv_sec = us / 1000000UL;
    fb->timestamp.tv_usec = us % 1000000UL;
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_v4l2_buf_queue.push(buf);
    m_buf_queue.push_back(fb);
    xSemaphoreGive(m_mutex);
    return fb;
}

cam_fb_t *WhoP4Cam::cam_fb_peek(bool back)
{
    return cam_fb_peek(back, 1)[0];
}

std::vector<cam_fb_t *> WhoP4Cam::cam_fb_peek(bool back, int num)
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

void WhoP4Cam::cam_fb_return()
{
    if (m_buf_queue.empty()) {
        ESP_LOGW(TAG, "Can not return more frame buffer.");
        return;
    }
    struct v4l2_buffer buf = m_v4l2_buf_queue.front();
    cam_fb_t *fb = m_cam_fbs + buf.index;
    if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
        buf.m.userptr = (unsigned long)fb->buf;
        buf.length = fb->len;
    }
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    if (ioctl(m_fd, VIDIOC_QBUF, &buf) != 0) {
        ESP_LOGE(TAG, "failed to queue video frame");
        return;
    }
    m_v4l2_buf_queue.pop();
    m_buf_queue.pop_front();
    xSemaphoreGive(m_mutex);
}

esp_err_t WhoP4Cam::init_fbs()
{
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = m_fb_count;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = m_fb_mem_type;
    if (ioctl(m_fd, VIDIOC_REQBUFS, &req) != 0) {
        ESP_LOGE(TAG, "failed to require buffer");
        close(m_fd);
        return ESP_FAIL;
    }

    struct v4l2_buffer buf;
    size_t cache_line_size;
    ESP_ERROR_CHECK(esp_cache_get_alignment(MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA, &cache_line_size));
    for (size_t i = 0; i < m_fb_count; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = m_fb_mem_type;
        buf.index = i;

        if (ioctl(m_fd, VIDIOC_QUERYBUF, &buf) != 0) {
            ESP_LOGE(TAG, "failed to query buffer");
            close(m_fd);
            return ESP_FAIL;
        }

        cam_fb_t *fb = m_cam_fbs + buf.index;
        fb->len = buf.length;
        if (m_fb_mem_type == V4L2_MEMORY_MMAP) {
            fb->buf = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, buf.m.offset);
        } else {
            fb->buf = heap_caps_aligned_alloc(cache_line_size, buf.length, MALLOC_CAP_SPIRAM);
            buf.m.userptr = (unsigned long)fb->buf;
        }
        fb->width = m_width;
        fb->height = m_height;
        fb->format = m_video_pix_fmt;

        if (!fb->buf) {
            ESP_LOGE(TAG, "failed to map/alloc buffer");
            close(m_fd);
            return ESP_FAIL;
        }

        if (ioctl(m_fd, VIDIOC_QBUF, &buf) != 0) {
            ESP_LOGE(TAG, "failed to queue video frame");
            close(m_fd);
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

} // namespace cam
} // namespace who
