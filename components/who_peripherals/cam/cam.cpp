#include "cam.hpp"

static const char *TAG = "who::cam";

namespace who {
namespace cam {

#if CONFIG_IDF_TARGET_ESP32P4

ESPVideo::ESPVideo(const video_pix_fmt_t video_pix_fmt,
                   const uint8_t fb_count,
                   const v4l2_memory fb_mem_type,
                   bool horizontal_flip) :
    Cam(fb_count), m_video_pix_fmt(video_pix_fmt), m_fb_mem_type(fb_mem_type), m_horizontal_flip(horizontal_flip)
{
    if ((fb_mem_type != V4L2_MEMORY_MMAP) && (fb_mem_type != V4L2_MEMORY_USERPTR)) {
        ESP_LOGE(TAG, "unsupported buffer memory type.");
    }
}

esp_err_t ESPVideo::set_horizontal_flip()
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

void ESPVideo::video_init()
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
    ESP_ERROR_CHECK(init_fbs());
    ESP_ERROR_CHECK(start_video_stream());
}

void ESPVideo::video_deinit()
{
    ESP_ERROR_CHECK(stop_video_stream());
    ESP_ERROR_CHECK(close_video_device());
}

esp_err_t ESPVideo::open_video_device()
{
    m_fd = open("/dev/video0", O_RDONLY);
    if (m_fd < 0) {
        ESP_LOGE(TAG, "failed to open device");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t ESPVideo::print_info()
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

esp_err_t ESPVideo::close_video_device()
{
    close(m_fd);
    return ESP_OK;
}

esp_err_t ESPVideo::set_video_format()
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

esp_err_t ESPVideo::start_video_stream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_STREAMON, &type) != 0) {
        ESP_LOGE(TAG, "failed to start stream");
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t ESPVideo::stop_video_stream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_STREAMOFF, &type) != 0) {
        ESP_LOGE(TAG, "failed to stop stream");
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

P4Cam::P4Cam(const video_pix_fmt_t video_pix_fmt,
             const uint8_t fb_count,
             const v4l2_memory fb_mem_type,
             bool horizontal_flip) :
    ESPVideo(video_pix_fmt, fb_count, fb_mem_type, horizontal_flip),
    m_mutex(xSemaphoreCreateMutex()),
    m_cam_fbs(new cam_fb_t[fb_count])
{
    video_init();
}

P4Cam::~P4Cam()
{
    video_deinit();
    if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
        for (int i = 0; i < m_fb_count; i++) {
            heap_caps_free(m_cam_fbs[i].buf);
        }
    }
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
    cam_fb_t *fb = m_cam_fbs + buf.index;
    fb->timestamp.tv_sec = us / 1000000UL;
    fb->timestamp.tv_usec = us % 1000000UL;
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_v4l2_buf_queue.push(buf);
    m_buf_queue.push(fb);
    xSemaphoreGive(m_mutex);
    return fb;
}

cam_fb_t *P4Cam::cam_fb_peek(bool back)
{
    if (m_buf_queue.empty()) {
        ESP_LOGW(TAG, "Can not peek an empty frame buffer queue.");
        return nullptr;
    }
    if (back) {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        cam_fb_t *fb = m_buf_queue.back();
        xSemaphoreGive(m_mutex);
        return fb;
    } else {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        cam_fb_t *fb = m_buf_queue.front();
        xSemaphoreGive(m_mutex);
        return fb;
    }
}

void P4Cam::cam_fb_return()
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
    m_buf_queue.pop();
    xSemaphoreGive(m_mutex);
}

esp_err_t P4Cam::init_fbs()
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

bool PPAP4Cam::ppa_trans_done_cb(ppa_client_handle_t ppa_client, ppa_event_data_t *event_data, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    SemaphoreHandle_t sem = (SemaphoreHandle_t)user_data;
    xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
    return (xHigherPriorityTaskWoken == pdTRUE);
}

PPAP4Cam::PPAP4Cam(const video_pix_fmt_t video_pix_fmt,
                   const uint8_t fb_count,
                   const v4l2_memory fb_mem_type,
                   int ppa_resized_w,
                   int ppa_resized_h,
                   bool horizontal_flip) :
    ESPVideo(video_pix_fmt, fb_count, fb_mem_type, horizontal_flip),
    m_mutex(xSemaphoreCreateMutex()),
    m_ppa_sem1(xSemaphoreCreateBinary()),
    m_ppa_sem2(xSemaphoreCreateBinary()),
    m_cam_fbs(new cam_fb_t[fb_count]),
    m_ppa_cam_fbs(new cam_fb_t[fb_count]),
    m_ppa_resized_w(ppa_resized_w),
    m_ppa_resized_h(ppa_resized_h)
{
    if (fb_count < 4) {
        ESP_LOGE("PPAP4CAM", "fb_count is at least 4.");
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
    if (xTaskCreatePinnedToCore(task, "PPAP4CAM", 2560, this, 2, nullptr, 0) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create PPAP4CAM task.\n");
    }
    xSemaphoreGive(m_ppa_sem2);
}

PPAP4Cam::~PPAP4Cam()
{
    video_deinit();
    // TODO unregister ppa, delete task
    for (int i = 0; i < m_fb_count; i++) {
        if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
            heap_caps_free(m_cam_fbs[i].buf);
        }
        heap_caps_free(m_ppa_cam_fbs[i].buf);
    }
    if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
        delete[] m_cam_fbs;
    }
    delete[] m_ppa_cam_fbs;
}

cam_fb_t *PPAP4Cam::cam_fb_get()
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

cam_fb_t *PPAP4Cam::cam_fb_peek(bool back)
{
    if (m_buf_queue.empty()) {
        ESP_LOGW(TAG, "Can not peek an empty frame buffer queue.");
        return nullptr;
    }
    if (back) {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        cam_fb_t *fb = m_buf_queue.back().fb;
        xSemaphoreGive(m_mutex);
        return fb;
    } else {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        cam_fb_t *fb = m_buf_queue.front().fb;
        xSemaphoreGive(m_mutex);
        return fb;
    }
}

cam_fb_t *PPAP4Cam::ppa_cam_fb_peek(bool back)
{
    if (m_buf_queue.empty()) {
        ESP_LOGW(TAG, "Can not peek an empty frame buffer queue.");
        return nullptr;
    }
    if (back) {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        cam_fb_t *fb = m_buf_queue.back().ppa_fb;
        xSemaphoreGive(m_mutex);
        return fb;
    } else {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        cam_fb_t *fb = m_buf_queue.front().ppa_fb;
        xSemaphoreGive(m_mutex);
        return fb;
    }
}

void PPAP4Cam::cam_fb_return()
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
    m_buf_queue.pop();
    xSemaphoreGive(m_mutex);
}

esp_err_t PPAP4Cam::init_fbs()
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
    size_t ppa_buffer_size = DL_IMAGE_ALIGN_UP(m_ppa_resized_h * m_ppa_resized_w * 3, cache_line_size);
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
            fb->buf = heap_caps_aligned_alloc(cache_line_size, buf.length, MALLOC_CAP_SPIRAM);
            buf.m.userptr = (unsigned long)fb->buf;
        }
        fb->width = m_width;
        fb->height = m_height;
        fb->format = m_video_pix_fmt;

        ppa_fb->width = m_ppa_resized_w;
        ppa_fb->height = m_ppa_resized_h;
        ppa_fb->format = VIDEO_PIX_FMT_RGB888;
        ppa_fb->buf = heap_caps_aligned_calloc(
            cache_line_size, ppa_buffer_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
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

void PPAP4Cam::task(void *args)
{
    PPAP4Cam *self = (PPAP4Cam *)args;
    while (true) {
        xSemaphoreTake(self->m_ppa_sem1, portMAX_DELAY);
        xSemaphoreTake(self->m_mutex, portMAX_DELAY);
        struct v4l2_buffer buf = self->m_v4l2_buf_queue.back();
        self->m_buf_queue.push({self->m_cam_fbs + buf.index, self->m_ppa_cam_fbs + buf.index});
        xSemaphoreGive(self->m_mutex);
        xSemaphoreGive(self->m_ppa_sem2);
    }
}

#elif CONFIG_IDF_TARGET_ESP32S3
S3Cam::S3Cam(const pixformat_t pixel_format,
             const framesize_t frame_size,
             const uint8_t fb_count,
             bool horizontal_flip) :
    Cam(fb_count), m_mutex(xSemaphoreCreateMutex())
{
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
    if (horizontal_flip) {
        ESP_ERROR_CHECK(set_horizontal_flip());
    }
}

S3Cam::~S3Cam()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    while (!m_buf_queue.empty()) {
        esp_camera_fb_return(m_buf_queue.front());
        m_buf_queue.pop();
    }
    xSemaphoreGive(m_mutex);
}

cam_fb_t *S3Cam::cam_fb_get()
{
    if (m_buf_queue.size() >= m_fb_count) {
        ESP_LOGW(TAG, "Can not get more frame buffer.");
        return nullptr;
    }
    cam_fb_t *fb = esp_camera_fb_get();
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_buf_queue.push(fb);
    xSemaphoreGive(m_mutex);
    return fb;
}

cam_fb_t *S3Cam::cam_fb_peek(bool back)
{
    if (m_buf_queue.empty()) {
        ESP_LOGW(TAG, "Can not peek an empty frame buffer queue.");
        return nullptr;
    }
    cam_fb_t *fb;
    if (back) {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        fb = m_buf_queue.back();
        xSemaphoreGive(m_mutex);
    } else {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        fb = m_buf_queue.front();
        xSemaphoreGive(m_mutex);
    }
    return fb;
}

void S3Cam::cam_fb_return()
{
    if (m_buf_queue.empty()) {
        ESP_LOGW(TAG, "Can not return more frame buffer.");
        return;
    }
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    esp_camera_fb_return(m_buf_queue.front());
    m_buf_queue.pop();
    xSemaphoreGive(m_mutex);
}

esp_err_t S3Cam::set_horizontal_flip()
{
    sensor_t *s = esp_camera_sensor_get();
    int ret = s->set_hmirror(s, 1);
    return (ret == 0) ? ESP_OK : ESP_FAIL;
}

#endif

} // namespace cam
} // namespace who
