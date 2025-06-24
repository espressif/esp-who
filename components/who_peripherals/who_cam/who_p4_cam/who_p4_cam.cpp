#include "who_p4_cam.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_video_device.h"
#include "esp_video_init.h"
#include <fcntl.h>

static const char *TAG = "WhoP4Cam";

namespace who {
namespace cam {

WhoP4Cam::WhoP4Cam(const uint32_t v4l2_fmt,
                   const uint8_t fb_count,
                   const v4l2_memory fb_mem_type,
                   bool vertical_flip,
                   bool horizontal_flip) :
    WhoCam(fb_count), m_v4l2_fmt(v4l2_fmt), m_fb_mem_type(fb_mem_type)
{
    if (fb_count < 2) {
        ESP_LOGE(TAG, "fb_count is at least 2.");
    }
    if ((fb_mem_type != V4L2_MEMORY_MMAP) && (fb_mem_type != V4L2_MEMORY_USERPTR)) {
        ESP_LOGE(TAG, "unsupported buffer memory type.");
    }
    video_init(vertical_flip, horizontal_flip);
}

WhoP4Cam::~WhoP4Cam()
{
    video_deinit();
    for (int i = 0; i < m_fb_count; i++) {
        if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
            heap_caps_free(m_cam_fbs[i].buf);
        }
        delete (struct v4l2_buffer *)m_cam_fbs[i].ret;
    }
}

esp_err_t WhoP4Cam::set_flip(bool vertical_flip, bool horizontal_flip)
{
    if (!vertical_flip & !horizontal_flip) {
        return ESP_OK;
    }
    struct v4l2_ext_controls controls;
    controls.ctrl_class = V4L2_CTRL_CLASS_USER;
    struct v4l2_ext_control control[1];
    controls.count = 1;
    controls.controls = control;
    if (vertical_flip) {
        control[0].id = V4L2_CID_VFLIP;
        control[0].value = 1;
        if (ioctl(m_fd, VIDIOC_S_EXT_CTRLS, &controls) != 0) {
            ESP_LOGE(TAG, "Failed to mirror the frame vertically.");
            close(m_fd);
            return ESP_FAIL;
        }
    }
    if (horizontal_flip) {
        control[0].id = V4L2_CID_HFLIP;
        control[0].value = 1;
        if (ioctl(m_fd, VIDIOC_S_EXT_CTRLS, &controls) != 0) {
            ESP_LOGE(TAG, "Failed to mirror the frame horizontally.");
            close(m_fd);
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

void WhoP4Cam::video_init(bool vertical_flip, bool horizontal_flip)
{
    ESP_ERROR_CHECK(bsp_i2c_init());

    static bool once = []() {
        esp_video_init_csi_config_t csi_config = {
            .sccb_config =
                {
                    .init_sccb = false,
                    .i2c_handle = bsp_i2c_get_handle(),
                    .freq = 100000,
                },
            .reset_pin = -1,
            .pwdn_pin = -1,
        };
        esp_video_init_config_t cam_config{};
        cam_config.csi = &csi_config;
        ESP_ERROR_CHECK(esp_video_init(&cam_config));
        return true;
    }();
    (void)once;

    ESP_ERROR_CHECK(open_video_device());
    ESP_ERROR_CHECK(print_info());
    ESP_ERROR_CHECK(set_flip(vertical_flip, horizontal_flip));
    ESP_ERROR_CHECK(set_video_format());
    ESP_ERROR_CHECK(init_fbs());
    ESP_ERROR_CHECK(start_video_stream());
}

void WhoP4Cam::video_deinit()
{
    ESP_ERROR_CHECK(stop_video_stream());
    ESP_ERROR_CHECK(close_video_device());
    ESP_ERROR_CHECK(esp_video_deinit());
}

esp_err_t WhoP4Cam::open_video_device()
{
    m_fd = open(ESP_VIDEO_MIPI_CSI_DEVICE_NAME, O_RDONLY);
    if (m_fd < 0) {
        ESP_LOGE(TAG, "Failed to open device");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t WhoP4Cam::print_info()
{
    struct v4l2_capability capability;
    if (ioctl(m_fd, VIDIOC_QUERYCAP, &capability)) {
        ESP_LOGE(TAG, "Failed to get capability");
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

esp_err_t WhoP4Cam::close_video_device()
{
    close(m_fd);
    return ESP_OK;
}

esp_err_t WhoP4Cam::set_video_format()
{
    if (CONFIG_CAMERA_SC2336_MIPI_IF_FORMAT_INDEX_DAFAULT < 8) {
        ESP_LOGE(TAG, "raw10 cam mode is not supported.");
        close(m_fd);
        return ESP_FAIL;
    }
    struct v4l2_format format = {};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_G_FMT, &format) != 0) {
        ESP_LOGE(TAG, "Failed to get format");
        close(m_fd);
        return ESP_FAIL;
    }
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = m_v4l2_fmt;
    m_fb_width = format.fmt.pix.width;
    m_fb_height = format.fmt.pix.height;
    if (ioctl(m_fd, VIDIOC_S_FMT, &format) != 0) {
        ESP_LOGE(TAG, "Failed to set format.");
        ESP_LOGI(TAG, "Supported format:");
        uint32_t idx = 0;
        while (true) {
            struct v4l2_fmtdesc fmtdesc = {};
            fmtdesc.index = idx++;
            fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (ioctl(m_fd, VIDIOC_ENUM_FMT, &fmtdesc) != 0) {
                break;
            }
            ESP_LOGI(TAG, "\t%s", (char *)fmtdesc.description);
        }
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t WhoP4Cam::init_fbs()
{
    struct v4l2_requestbuffers req = {};
    req.count = m_fb_count;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = m_fb_mem_type;
    if (ioctl(m_fd, VIDIOC_REQBUFS, &req) != 0) {
        ESP_LOGE(TAG, "Failed to require buffer");
        close(m_fd);
        return ESP_FAIL;
    }

    for (size_t i = 0; i < m_fb_count; i++) {
        struct v4l2_buffer *buf = new struct v4l2_buffer();
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = m_fb_mem_type;
        buf->index = i;

        if (ioctl(m_fd, VIDIOC_QUERYBUF, buf) != 0) {
            ESP_LOGE(TAG, "Failed to query buffer");
            close(m_fd);
            return ESP_FAIL;
        }

        cam_fb_t *fb = m_cam_fbs + i;
        if (m_fb_mem_type == V4L2_MEMORY_MMAP) {
            fb->buf = mmap(NULL, buf->length, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, buf->m.offset);
        } else {
            // implicit align to cache line size
            fb->buf = heap_caps_malloc(buf->length, MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
            buf->m.userptr = (unsigned long)fb->buf;
        }
        if (!fb->buf) {
            ESP_LOGE(TAG, "Failed to map/alloc buffer");
            close(m_fd);
            return ESP_FAIL;
        }
        fb->width = m_fb_width;
        fb->height = m_fb_height;
        fb->format = v4l2_fmt2cam_fb_fmt(m_v4l2_fmt);
        fb->ret = (void *)buf;

        if (ioctl(m_fd, VIDIOC_QBUF, buf) != 0) {
            ESP_LOGE(TAG, "Failed to queue video frame");
            close(m_fd);
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t WhoP4Cam::start_video_stream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_STREAMON, &type) != 0) {
        ESP_LOGE(TAG, "Failed to start stream");
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t WhoP4Cam::stop_video_stream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_STREAMOFF, &type) != 0) {
        ESP_LOGE(TAG, "Failed to stop stream");
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

cam_fb_t *WhoP4Cam::cam_fb_get()
{
    struct v4l2_buffer buf = {};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = m_fb_mem_type;
    if (ioctl(m_fd, VIDIOC_DQBUF, &buf) != 0) {
        ESP_LOGE(TAG, "failed to receive video frame");
        return nullptr;
    }
    int64_t us = esp_timer_get_time();
    cam_fb_t *fb = m_cam_fbs + buf.index;
    fb->timestamp.tv_sec = us / 1000000;
    fb->timestamp.tv_usec = us % 1000000;
    fb->len = buf.bytesused;
    return fb;
}

void WhoP4Cam::cam_fb_return(cam_fb_t *fb)
{
    if (ioctl(m_fd, VIDIOC_QBUF, fb->ret) != 0) {
        ESP_LOGE(TAG, "failed to queue video frame");
    }
}

} // namespace cam
} // namespace who
