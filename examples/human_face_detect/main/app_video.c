#include "app_video.h"

static const char *TAG = "APP_VIDEO";
static video_fb_t buffers[MAX_FB_COUNT];
static struct v4l2_buffer buf;
static int fd;


esp_err_t open_video_device()
{
    fd = open("/dev/video0", O_RDONLY);
    if (fd < 0) {
        ESP_LOGE(TAG, "failed to open device");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t close_video_device()
{
    close(fd);
    return ESP_OK;
}

esp_err_t set_video_format(const video_format_t video_format)
{
    struct v4l2_format format = {
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .fmt.pix.width = 1280,
        .fmt.pix.height = 720,
        .fmt.pix.pixelformat = video_format,
    };
    if (ioctl(fd, VIDIOC_S_FMT, &format) != 0) {
        ESP_LOGE(TAG, "failed to set format");
        close(fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t set_horizontal_flip()
{
    struct v4l2_ext_controls controls;
    struct v4l2_ext_control control[1];
    controls.ctrl_class = V4L2_CTRL_CLASS_USER;
    controls.count      = 1;
    controls.controls   = control;
    control[0].id       = V4L2_CID_HFLIP;
    control[0].value    = 1;
    if (ioctl(fd, VIDIOC_S_EXT_CTRLS, &controls) != 0) {
        ESP_LOGE(TAG, "failed to mirror the frame horizontally");
        close(fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t init_fb(const int fb_count)
{
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = fb_count;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // req.memory = V4L2_MEMORY_MMAP;
    req.memory = V4L2_MEMORY_USERPTR;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) != 0) {
        ESP_LOGE(TAG, "failed to require buffer");
        close(fd);
        return ESP_FAIL;
    }

    for (size_t i = 0; i < fb_count; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        // buf.memory = V4L2_MEMORY_MMAP;
        buf.memory = V4L2_MEMORY_USERPTR;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) != 0) {
            ESP_LOGE(TAG, "failed to query buffer");
            close(fd);
            return ESP_FAIL;
        }

        
        buffers[i].len = buf.length;
        // buffers[i].buf = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        size_t cache_line_size;
        ESP_ERROR_CHECK(esp_cache_get_alignment(MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA, &cache_line_size));
        buffers[i].buf = heap_caps_aligned_alloc(cache_line_size, buf.length, MALLOC_CAP_SPIRAM);
        uint64_t us = (uint64_t)esp_timer_get_time();
        buffers[i].timestamp.tv_sec = us / 1000000UL;
        buffers[i].timestamp.tv_usec = us % 1000000UL;
        buf.m.userptr = (unsigned long)buffers[i].buf;

        if (!buffers[i].buf) {
            ESP_LOGE(TAG, "failed to map buffer");
            close(fd);
            return ESP_FAIL;
        }

        if (ioctl(fd, VIDIOC_QBUF, &buf) != 0) {
            ESP_LOGE(TAG, "failed to queue video frame");
            close(fd);
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t start_video_stream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) != 0) {
        ESP_LOGE(TAG, "failed to start stream");
        close(fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t stop_video_stream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) != 0) {
        ESP_LOGE(TAG, "failed to stop stream");
        close(fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t video_init(const video_format_t video_format, const int fb_count)
{
    assert(fb_count < MAX_FB_COUNT);
    esp_video_init_csi_config_t csi_config[] = CSI_CAMERA_DEFAULT_CONFIG;
    if (!bsp_i2c_get_handle())
    {
        ESP_ERROR_CHECK(bsp_i2c_init());
    }
    csi_config[0].sccb_config.i2c_handle = bsp_i2c_get_handle();
    esp_video_init_config_t cam_config = 
    {
        .csi = csi_config,
    };
    ESP_ERROR_CHECK(esp_video_init(&cam_config));
    ESP_ERROR_CHECK(open_video_device());
    // ESP_ERROR_CHECK(set_horizontal_flip());
    ESP_ERROR_CHECK(set_video_format(video_format));
    ESP_ERROR_CHECK(init_fb(fb_count));
    ESP_ERROR_CHECK(start_video_stream());
    return ESP_OK;
}

esp_err_t video_deinit(const int fb_count)
{
    ESP_ERROR_CHECK(stop_video_stream());
    ESP_ERROR_CHECK(close_video_device());
    for (int i = 0; i < fb_count; i++) {
        heap_caps_free(buffers[i].buf);
    }
    return ESP_OK;
}

video_fb_t *video_fb_get()
{
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // buf.memory = V4L2_MEMORY_MMAP;
    buf.memory = V4L2_MEMORY_USERPTR;
    if (ioctl(fd, VIDIOC_DQBUF, &buf) != 0) {
        ESP_LOGE(TAG, "failed to receive video frame");
    }
    uint64_t us = (uint64_t)esp_timer_get_time();
    buffers[buf.index].timestamp.tv_sec = us / 1000000UL;
    buffers[buf.index].timestamp.tv_usec = us % 1000000UL;
    return &buffers[buf.index];
}

void video_fb_return()
{
    buf.m.userptr =  (unsigned long)buffers[buf.index].buf;
    buf.length = buffers[buf.index].len;
    if (ioctl(fd, VIDIOC_QBUF, &buf) != 0) {
        ESP_LOGE(TAG, "failed to queue video frame");
    }
}