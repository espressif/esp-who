#include "who_cam.hpp"
#include "who_lcd.hpp"

static const char *TAG = "who::cam";

namespace who {
namespace cam {
SemaphoreHandle_t P4Cam::s_mutex = xSemaphoreCreateMutex();
TaskHandle_t WhoCam::s_task_handle = nullptr;

#if CONFIG_IDF_TARGET_ESP32P4
P4Cam::P4Cam(const video_pix_fmt_t video_pix_fmt,
             const uint8_t fb_count,
             const v4l2_memory fb_mem_type,
             bool horizontal_flip) :
    Cam(fb_count),
    m_video_pix_fmt(video_pix_fmt),
    m_fb_mem_type(fb_mem_type),
    m_horizontal_flip(horizontal_flip),
    m_cam_fbs(new cam_fb_t[fb_count])
{
    if ((fb_mem_type != V4L2_MEMORY_MMAP) && (fb_mem_type != V4L2_MEMORY_USERPTR)) {
        ESP_LOGE(TAG, "unsupported buffer memory type.");
    }
    video_init();
}

P4Cam::~P4Cam()
{
    video_deinit();
    delete[] m_cam_fbs;
}

cam_fb_t *P4Cam::cam_fb_get()
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
    m_cam_fbs[buf.index].timestamp.tv_sec = us / 1000000UL;
    m_cam_fbs[buf.index].timestamp.tv_usec = us % 1000000UL;
    xSemaphoreTake(s_mutex, portMAX_DELAY);
    m_buf_queue.push(buf);
    xSemaphoreGive(s_mutex);
    return &m_cam_fbs[buf.index];
}

cam_fb_t *P4Cam::cam_fb_peek(bool back)
{
    if (back) {
        xSemaphoreTake(s_mutex, portMAX_DELAY);
        struct v4l2_buffer buf = m_buf_queue.back();
        xSemaphoreGive(s_mutex);
        return &m_cam_fbs[buf.index];
    } else {
        xSemaphoreTake(s_mutex, portMAX_DELAY);
        struct v4l2_buffer buf = m_buf_queue.front();
        xSemaphoreGive(s_mutex);
        return &m_cam_fbs[buf.index];
    }
}

void P4Cam::cam_fb_return()
{
    if (m_buf_queue.empty()) {
        ESP_LOGW(TAG, "Can not return more frame buffer.");
        return;
    }
    struct v4l2_buffer buf = m_buf_queue.front();
    if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
        buf.m.userptr = (unsigned long)m_cam_fbs[buf.index].buf;
        buf.length = m_cam_fbs[buf.index].len;
    }
    if (ioctl(m_fd, VIDIOC_QBUF, &buf) != 0) {
        ESP_LOGE(TAG, "failed to queue video frame");
        return;
    }
    xSemaphoreTake(s_mutex, portMAX_DELAY);
    m_buf_queue.pop();
    xSemaphoreGive(s_mutex);
}

esp_err_t P4Cam::set_horizontal_flip()
{
    struct v4l2_ext_controls controls;
    struct v4l2_ext_control control[1];
    controls.ctrl_class = V4L2_CTRL_CLASS_USER;
    controls.count = 1;
    controls.controls = control;
    control[0].id = V4L2_CID_HFLIP;
    control[0].value = 1;
    if (ioctl(m_fd, VIDIOC_S_EXT_CTRLS, &controls) != 0) {
        ESP_LOGE(TAG, "failed to mirror the frame horizontally");
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t P4Cam::video_init()
{
    esp_video_init_csi_config_t csi_config[] = CSI_CAMERA_DEFAULT_CONFIG;
    ESP_ERROR_CHECK(bsp_i2c_init());
    csi_config[0].sccb_config.i2c_handle = bsp_i2c_get_handle();
    esp_video_init_config_t cam_config{};
    cam_config.csi = csi_config;

    ESP_ERROR_CHECK(esp_video_init(&cam_config));
    ESP_ERROR_CHECK(open_video_device());
    ESP_ERROR_CHECK(print_info());
    if (m_horizontal_flip) {
        ESP_ERROR_CHECK(set_horizontal_flip());
    }
    ESP_ERROR_CHECK(set_video_format());
    ESP_ERROR_CHECK(init_fb());
    ESP_ERROR_CHECK(start_video_stream());
    return ESP_OK;
}

esp_err_t P4Cam::video_deinit()
{
    ESP_ERROR_CHECK(stop_video_stream());
    ESP_ERROR_CHECK(close_video_device());
    if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
        for (int i = 0; i < m_fb_count; i++) {
            heap_caps_free(m_cam_fbs[i].buf);
        }
    }
    return ESP_OK;
}

esp_err_t P4Cam::open_video_device()
{
    m_fd = open("/dev/video0", O_RDONLY);
    if (m_fd < 0) {
        ESP_LOGE(TAG, "failed to open device");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t P4Cam::print_info()
{
    struct v4l2_capability capability;
    if (ioctl(m_fd, VIDIOC_QUERYCAP, &capability)) {
        ESP_LOGE(TAG, "failed to get capability");
        close(m_fd);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG,
             "version: %d.%d.%d",
             (uint16_t)(capability.version >> 16),
             (uint8_t)(capability.version >> 8),
             (uint8_t)capability.version);
    ESP_LOGI(TAG, "driver:  %s", capability.driver);
    ESP_LOGI(TAG, "card:    %s", capability.card);
    ESP_LOGI(TAG, "bus:     %s", capability.bus_info);
    ESP_LOGI(TAG, "capabilities:");
    if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        ESP_LOGI(TAG, "\tVIDEO_CAPTURE");
    }
    if (capability.capabilities & V4L2_CAP_READWRITE) {
        ESP_LOGI(TAG, "\tREADWRITE");
    }
    if (capability.capabilities & V4L2_CAP_ASYNCIO) {
        ESP_LOGI(TAG, "\tASYNCIO");
    }
    if (capability.capabilities & V4L2_CAP_STREAMING) {
        ESP_LOGI(TAG, "\tSTREAMING");
    }
    if (capability.capabilities & V4L2_CAP_META_OUTPUT) {
        ESP_LOGI(TAG, "\tMETA_OUTPUT");
    }
    if (capability.capabilities & V4L2_CAP_DEVICE_CAPS) {
        ESP_LOGI(TAG, "device capabilities:");
        if (capability.device_caps & V4L2_CAP_VIDEO_CAPTURE) {
            ESP_LOGI(TAG, "\tVIDEO_CAPTURE");
        }
        if (capability.device_caps & V4L2_CAP_READWRITE) {
            ESP_LOGI(TAG, "\tREADWRITE");
        }
        if (capability.device_caps & V4L2_CAP_ASYNCIO) {
            ESP_LOGI(TAG, "\tASYNCIO");
        }
        if (capability.device_caps & V4L2_CAP_STREAMING) {
            ESP_LOGI(TAG, "\tSTREAMING");
        }
        if (capability.device_caps & V4L2_CAP_META_OUTPUT) {
            ESP_LOGI(TAG, "\tMETA_OUTPUT");
        }
    }
    return ESP_OK;
}

esp_err_t P4Cam::close_video_device()
{
    close(m_fd);
    return ESP_OK;
}

esp_err_t P4Cam::set_video_format()
{
    if (CONFIG_CAMERA_SC2336_MIPI_IF_FORMAT_INDEX_DAFAULT < 8) {
        ESP_LOGE(TAG, "raw10 cam mode is not supported.");
        close(m_fd);
        return ESP_FAIL;
    }
    struct v4l2_format format;
    memset(&format, 0, sizeof(struct v4l2_format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_G_FMT, &format) != 0) {
        ESP_LOGE(TAG, "failed to get format");
        close(m_fd);
        return ESP_FAIL;
    }
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = m_video_pix_fmt;
    m_width = format.fmt.pix.width;
    m_height = format.fmt.pix.height;
    if (ioctl(m_fd, VIDIOC_S_FMT, &format) != 0) {
        ESP_LOGE(TAG, "failed to set format");
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t P4Cam::init_fb()
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

        m_cam_fbs[i].len = buf.length;
        if (m_fb_mem_type == V4L2_MEMORY_MMAP) {
            m_cam_fbs[i].buf = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, buf.m.offset);
        } else {
            m_cam_fbs[i].buf = heap_caps_aligned_alloc(cache_line_size, buf.length, MALLOC_CAP_SPIRAM);
            buf.m.userptr = (unsigned long)m_cam_fbs[i].buf;
        }
        m_cam_fbs[i].width = m_width;
        m_cam_fbs[i].height = m_height;
        m_cam_fbs[i].format = m_video_pix_fmt;

        if (!m_cam_fbs[i].buf) {
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

esp_err_t P4Cam::start_video_stream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_STREAMON, &type) != 0) {
        ESP_LOGE(TAG, "failed to start stream");
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t P4Cam::stop_video_stream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_STREAMOFF, &type) != 0) {
        ESP_LOGE(TAG, "failed to stop stream");
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

#elif CONFIG_IDF_TARGET_ESP32S3

#endif

void WhoCam::task(void *args)
{
    WhoCam *self = (WhoCam *)args;
    for (int i = 0; i < self->m_cam->m_fb_count - 2; i++) {
        self->m_cam->cam_fb_get();
    }
    // const TickType_t frame_interval = pdMS_TO_TICKS(66);
    // TickType_t last_wake_time = xTaskGetTickCount();
    while (true) {
        self->m_cam->cam_fb_get();
        xTaskNotifyGive(who::lcd::WhoLCD::s_task_handle);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // vTaskDelayUntil(&last_wake_time, frame_interval);
    }
}

void WhoCam::run()
{
    if (xTaskCreatePinnedToCore(task, "WhoCam", 1536, this, 2, &s_task_handle, 0) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create WhoCam task.\n");
    };
}

} // namespace cam
} // namespace who
