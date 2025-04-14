#include "who_p4_ppa_cam.hpp"
#include "esp_timer.h"

static const char *TAG = "WhoP4PPACam";

namespace who {
namespace cam {

bool WhoP4PPACam::ppa_trans_done_cb(ppa_client_handle_t ppa_client, ppa_event_data_t *event_data, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    SemaphoreHandle_t sem = (SemaphoreHandle_t)user_data;
    xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
    return (xHigherPriorityTaskWoken == pdTRUE);
}

WhoP4PPACam::WhoP4PPACam(const video_pix_fmt_t video_pix_fmt,
                         const uint8_t fb_count,
                         const v4l2_memory fb_mem_type,
                         int ppa_resized_w,
                         int ppa_resized_h,
                         bool horizontal_flip) :
    ESPVideo(video_pix_fmt, fb_count, fb_mem_type, horizontal_flip),
    m_mutex(xSemaphoreCreateMutex()),
    m_ppa_sem1(xSemaphoreCreateBinary()),
    m_ppa_sem2(xSemaphoreCreateBinary()),
    m_task_finish(xSemaphoreCreateBinary()),
    m_cam_fbs(new cam_fb_t[fb_count]),
    m_ppa_cam_fbs(new cam_fb_t[fb_count]),
    m_ppa_resized_w(ppa_resized_w),
    m_ppa_resized_h(ppa_resized_h)
{
    if (fb_count < 4) {
        ESP_LOGE("WhoP4PPACam", "fb_count is at least 4.");
    }
    video_init();
    ppa_client_config_t ppa_client_config;
    memset(&ppa_client_config, 0, sizeof(ppa_client_config_t));
    ppa_client_config.oper_type = PPA_OPERATION_SRM;
    ESP_ERROR_CHECK(ppa_register_client(&ppa_client_config, &m_ppa_srm_handle));
    ppa_event_callbacks_t cbs = {
        .on_trans_done = ppa_trans_done_cb,
    };
    ppa_client_register_event_callbacks(m_ppa_srm_handle, &cbs);
    if (xTaskCreatePinnedToCore(task, "WhoP4PPACam", 2560, this, 2, &m_task_handle, 0) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create WhoP4PPACam task.\n");
    }
    xSemaphoreGive(m_ppa_sem2);
}

WhoP4PPACam::~WhoP4PPACam()
{
    m_task_stop = true;
    xSemaphoreGive(m_ppa_sem1);
    xSemaphoreTake(m_task_finish, portMAX_DELAY);

    video_deinit();

    if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
        for (int i = 0; i < m_fb_count; i++) {
            heap_caps_free(m_cam_fbs[i].buf);
        }
    }
    delete[] m_cam_fbs;

    for (int i = 0; i < m_fb_count; i++) {
        heap_caps_free(m_ppa_cam_fbs[i].buf);
    }
    delete m_ppa_cam_fbs;

    ESP_ERROR_CHECK(ppa_unregister_client(m_ppa_srm_handle));
    vSemaphoreDelete(m_mutex);
    vSemaphoreDelete(m_ppa_sem1);
    vSemaphoreDelete(m_ppa_sem2);
    vSemaphoreDelete(m_task_finish);
}

cam_fb_t *WhoP4PPACam::cam_fb_get()
{
    xSemaphoreTake(m_ppa_sem2, portMAX_DELAY);
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
    cam_fb_t *ppa_fb = m_ppa_cam_fbs + buf.index;
    fb->timestamp.tv_sec = us / 1000000UL;
    fb->timestamp.tv_usec = us % 1000000UL;
    ppa_fb->timestamp.tv_sec = fb->timestamp.tv_sec;
    ppa_fb->timestamp.tv_usec = fb->timestamp.tv_usec;
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_v4l2_buf_queue.push(buf);
    xSemaphoreGive(m_mutex);
    dl::image::img_t dst_img = fb2img(ppa_fb);
    dl::image::resize_ppa(fb2img(fb),
                          dst_img,
                          m_ppa_srm_handle,
                          ppa_fb->buf,
                          ppa_fb->len,
                          PPA_TRANS_MODE_NON_BLOCKING,
                          (void *)m_ppa_sem1,
                          DL_IMAGE_CAP_PPA | DL_IMAGE_CAP_RGB565_BIG_ENDIAN,
                          nullptr,
                          {},
                          &m_ppa_scale_x,
                          &m_ppa_scale_y);
    return nullptr;
}

cam_fb_t *WhoP4PPACam::cam_fb_peek(bool back)
{
    return cam_fb_peek(back, 1)[0];
}

cam_fb_t *WhoP4PPACam::ppa_cam_fb_peek(bool back)
{
    return ppa_cam_fb_peek(back, 1)[0];
}

std::vector<cam_fb_t *> WhoP4PPACam::cam_fb_peek(bool back, int num)
{
    if (m_buf_queue.size() < num) {
        ESP_LOGW(TAG, "Unable to peek %d frame buffer from a frame buffer queue of size %d .", num, m_buf_queue.size());
        return {};
    }
    std::vector<cam_fb_t *> fbs;
    if (back) {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        for (auto it = m_buf_queue.end() - num; it != m_buf_queue.end(); it++) {
            fbs.emplace_back(it->fb);
        }
        xSemaphoreGive(m_mutex);
    } else {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        for (auto it = m_buf_queue.begin(); it != m_buf_queue.begin() + num; it++) {
            fbs.emplace_back(it->fb);
        }
        xSemaphoreGive(m_mutex);
    }
    return fbs;
}

std::vector<cam_fb_t *> WhoP4PPACam::ppa_cam_fb_peek(bool back, int num)
{
    if (m_buf_queue.size() < num) {
        ESP_LOGW(TAG, "Unable to peek %d frame buffer from a frame buffer queue of size %d .", num, m_buf_queue.size());
        return {};
    }
    std::vector<cam_fb_t *> fbs;
    if (back) {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        for (auto it = m_buf_queue.end() - num; it != m_buf_queue.end(); it++) {
            fbs.emplace_back(it->ppa_fb);
        }
        xSemaphoreGive(m_mutex);
    } else {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        for (auto it = m_buf_queue.begin(); it != m_buf_queue.begin() + num; it++) {
            fbs.emplace_back(it->ppa_fb);
        }
        xSemaphoreGive(m_mutex);
    }
    return fbs;
}

void WhoP4PPACam::cam_fb_return()
{
    if (m_buf_queue.empty()) {
        ESP_LOGW(TAG, "Can not return more frame buffer.");
        return;
    }
    struct v4l2_buffer buf = m_v4l2_buf_queue.front();
    if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
        buf.m.userptr = (unsigned long)m_cam_fbs[buf.index].buf;
        buf.length = m_cam_fbs[buf.index].len;
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

esp_err_t WhoP4PPACam::init_fbs()
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
    size_t ppa_buffer_size = m_ppa_resized_h * m_ppa_resized_w * 3;
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
        cam_fb_t *ppa_fb = m_ppa_cam_fbs + buf.index;
        fb->len = buf.length;
        if (m_fb_mem_type == V4L2_MEMORY_MMAP) {
            fb->buf = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, buf.m.offset);
        } else {
            fb->buf = heap_caps_malloc(buf.length, MALLOC_CAP_SPIRAM | MALLOC_CAP_CACHE_ALIGNED);
            buf.m.userptr = (unsigned long)fb->buf;
        }
        fb->width = m_width;
        fb->height = m_height;
        fb->format = m_video_pix_fmt;

        ppa_fb->width = m_ppa_resized_w;
        ppa_fb->height = m_ppa_resized_h;
        ppa_fb->format = VIDEO_PIX_FMT_RGB888;
        ppa_fb->buf = heap_caps_calloc(1, ppa_buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_CACHE_ALIGNED);
        ppa_fb->len = ppa_buffer_size;

        if (!fb->buf || !ppa_fb->buf) {
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

void WhoP4PPACam::task(void *args)
{
    WhoP4PPACam *self = (WhoP4PPACam *)args;
    while (true) {
        xSemaphoreTake(self->m_ppa_sem1, portMAX_DELAY);
        if (self->m_task_stop) {
            break;
        }
        xSemaphoreTake(self->m_mutex, portMAX_DELAY);
        struct v4l2_buffer buf = self->m_v4l2_buf_queue.back();
        self->m_buf_queue.push_back({self->m_cam_fbs + buf.index, self->m_ppa_cam_fbs + buf.index});
        xSemaphoreGive(self->m_mutex);
        xSemaphoreGive(self->m_ppa_sem2);
    }
    xSemaphoreGive(self->m_task_finish);
    vTaskDelete(NULL);
}

} // namespace cam
} // namespace who
