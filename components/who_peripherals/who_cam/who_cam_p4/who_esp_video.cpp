#include "who_esp_video.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_video_device.h"
#include "esp_video_init.h"
#include <fcntl.h>
#include "bsp/esp-bsp.h"

static const char *TAG = "ESPVideo";

namespace who {
namespace cam {

ESPVideo::ESPVideo(const video_pix_fmt_t video_pix_fmt,
                   const uint8_t fb_count,
                   const v4l2_memory fb_mem_type,
                   bool horizontal_flip) :
    WhoCam(fb_count), m_video_pix_fmt(video_pix_fmt), m_fb_mem_type(fb_mem_type), m_horizontal_flip(horizontal_flip)
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
    m_fd = open(ESP_VIDEO_MIPI_CSI_DEVICE_NAME, O_RDONLY);
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

} // namespace cam
} // namespace who
