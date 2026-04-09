/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: ESPRESSIF MIT
 */

#include "video_capture.hpp"
#include <fcntl.h>
#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_cam_sensor_types.h"
#include <format>

static const char *TAG = "VideoCapture";

esp_err_t VideoCapture::Config::validate() const
{
    if (buffer_count <= 1) {
        ESP_LOGE(TAG, "Buffer count must be greater than 1");
        return ESP_ERR_INVALID_ARG;
    }

    if (buffer_mem_type != V4L2_MEMORY_MMAP && buffer_mem_type != V4L2_MEMORY_USERPTR) {
        ESP_LOGE(TAG, "Invalid uffer memory type");
        return ESP_ERR_INVALID_ARG;
    }

    auto is_uvc_device = [](const char *name) -> bool {
        // UVC devices are /dev/video40 to /dev/video49
        return (strncmp(name, "/dev/video4", 11) == 0 &&
                name[11] >= '0' && name[11] <= '9');
    };

    bool is_uvc = is_uvc_device(device_name);

    if (is_uvc && !uvc_cfg.has_value()) {
        ESP_LOGE(TAG, "UVC device requires uvc_cfg");
        return ESP_ERR_INVALID_ARG;
    }

    if (!is_uvc && uvc_cfg.has_value()) {
        ESP_LOGE(TAG, "Non-UVC device cannot have uvc_cfg");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

VideoCapture::VideoCapture() : cfg_(nullptr, 0, 0), width_(0), height_(0), is_init_(false), is_start_(false)
{
}

VideoCapture::~VideoCapture()
{
    if (is_init_) {
        deinit();
    }
}

esp_err_t VideoCapture::init(const Config &cfg)
{
    if (is_init_) {
        return ESP_ERR_INVALID_STATE;
    }

    // Validate configuration
    if (auto ret = cfg.validate(); ret != ESP_OK) {
        return ret;
    }

    // Open device
    if (auto ret = device_.open(cfg.device_name, O_RDONLY); ret != ESP_OK) {
        return ret;
    }

    bool is_uvc = cfg.uvc_cfg.has_value();
    // Print device information
    print_device_info(!is_uvc);

    // Set format
    if (is_uvc) {
        auto &uvc_cfg = *cfg.uvc_cfg;
        if (auto ret = device_.set_format(V4L2_BUF_TYPE_VIDEO_CAPTURE, uvc_cfg.width, uvc_cfg.height, cfg.pixel_format); ret != ESP_OK) {
            print_frame_params();
            ESP_ERROR_CHECK(device_.close());
            return ret;
        }
        if (auto ret = device_.set_fps_by_denominator(V4L2_BUF_TYPE_VIDEO_CAPTURE, uvc_cfg.fps, 10000000UL); ret != ESP_OK) {
            print_frame_params();
            ESP_ERROR_CHECK(device_.close());
            return ret;
        }
        width_ = uvc_cfg.width;
        height_ = uvc_cfg.height;
        fps_ = uvc_cfg.fps;
    } else {
        struct v4l2_format format;
        if (auto ret = device_.get_format(V4L2_BUF_TYPE_VIDEO_CAPTURE, &format); ret != ESP_OK) {
            ESP_ERROR_CHECK(device_.close());
            return ret;
        }
        if (cfg.pixel_format != format.fmt.pix.pixelformat) {
            if (auto ret = device_.set_format(V4L2_BUF_TYPE_VIDEO_CAPTURE, format.fmt.pix.width, format.fmt.pix.height, cfg.pixel_format); ret != ESP_OK) {
                print_frame_params();
                ESP_ERROR_CHECK(device_.close());
                return ret;
            }
        }
        width_ = format.fmt.pix.width;
        height_ = format.fmt.pix.height;
        ESP_ERROR_CHECK(device_.get_fps(V4L2_BUF_TYPE_VIDEO_CAPTURE, &fps_));
    }

    // Set flip controls if requested
    if (cfg.hflip) {
        if (auto ret = device_.set_control(V4L2_CTRL_CLASS_USER, V4L2_CID_HFLIP, 1); ret != ESP_OK) {
            ESP_LOGD(TAG, "Failed to set horizontal flip");
            return ret;
        }
    }

    if (cfg.vflip) {
        if (auto ret = device_.set_control(V4L2_CTRL_CLASS_USER, V4L2_CID_VFLIP, 1); ret != ESP_OK) {
            ESP_LOGD(TAG, "Failed to set vertical flip");
            return ret;
        }
    }

    // Request buffers
    if (auto ret = device_.request_buffers(V4L2_BUF_TYPE_VIDEO_CAPTURE,
                                 cfg.buffer_mem_type, cfg.buffer_count); ret != ESP_OK) {
        ESP_LOGD(TAG, "Failed to request buffers");
        device_.close();
        return ret;
    }

    // Query and queue buffers
    for (uint32_t i = 0; i < cfg.buffer_count; i++) {
        struct v4l2_buffer buf;
        if (auto ret = device_.query_buffer(V4L2_BUF_TYPE_VIDEO_CAPTURE, i, cfg.buffer_mem_type, &buf); ret != ESP_OK) {
            ESP_LOGD(TAG, "Failed to query buffer %u", i);
            cleanup(cfg);
            return ret;
        }

        void *buf_addr = nullptr;
        if (cfg.buffer_mem_type == V4L2_MEMORY_MMAP) {
            if (auto ret = device_.map_buffer(buf.m.offset, buf.length, &buf_addr); ret != ESP_OK) {
                ESP_LOGD(TAG, "Failed to map buffer %u", i);
                cleanup(cfg);
                return ret;
            }
        } else if (cfg.buffer_mem_type == V4L2_MEMORY_USERPTR) {
            // implicit align to cache line size
            buf_addr = heap_caps_malloc(buf.length, MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
            if (!buf_addr) {
                ESP_LOGD(TAG, "Failed to alloc buffer %u", i);
                cleanup(cfg);
                return ESP_ERR_NO_MEM;
            }
            buf.m.userptr = (unsigned long)buf_addr;
        } else {
            abort();
        }

        buffers_.emplace_back(buf);
        Frame frame {};
        frame.data = buf_addr;
        frame.width = width_;
        frame.height = height_;
        frame.pixel_format = cfg.pixel_format;
        frames_.emplace_back(frame);

        if (auto ret = device_.queue_buffer(V4L2_BUF_TYPE_VIDEO_CAPTURE, buf); ret != ESP_OK) {
            ESP_LOGD(TAG, "Failed to queue buffer %u", i);
            cleanup(cfg);
            return ret;
        }
    }

    cfg_ = cfg;
    is_init_ = true;
    return ESP_OK;
}

esp_err_t VideoCapture::deinit()
{
    if (!is_init_) {
        return ESP_ERR_INVALID_STATE;
    }

    if (is_start_) {
        stop();
    }

    cleanup(cfg_);

    is_init_ = false;
    return ESP_OK;
}

esp_err_t VideoCapture::start()
{
    if (!is_init_ || is_start_) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (auto ret = device_.stream_on(V4L2_BUF_TYPE_VIDEO_CAPTURE); ret != ESP_OK) {
        return ret;
    }

    is_start_ = true;
    return ESP_OK;
}

esp_err_t VideoCapture::stop()
{
    if (!is_init_ || !is_start_) {
        return ESP_ERR_INVALID_STATE;
    }

    if (auto ret = device_.stream_off(V4L2_BUF_TYPE_VIDEO_CAPTURE); ret != ESP_OK) {
        return ret;
    }

    is_start_ = false;
    return ESP_OK;
}

esp_err_t VideoCapture::get_frame(Frame **frame, uint32_t timeout_ms)
{
    if (!is_init_ || !is_start_) {
        *frame = nullptr;
        return ESP_ERR_INVALID_STATE;
    }

    struct v4l2_buffer buf;

    if (auto ret = device_.dequeue_buffer(V4L2_BUF_TYPE_VIDEO_CAPTURE, cfg_.buffer_mem_type, &buf, timeout_ms); ret != ESP_OK) {
        *frame = nullptr;
        return ret;
    }

    *frame = &frames_[buf.index];
    (*frame)->timestamp = esp_timer_get_time();
    (*frame)->size = buf.bytesused;

    return ESP_OK;
}

esp_err_t VideoCapture::return_frame(Frame *frame)
{
    if (!is_init_ || !is_start_) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!frame || frame < frames_.data() || frame > &frames_[frames_.size() - 1]) {
        ESP_LOGD(TAG, "Invalid frame to return.");
        return ESP_ERR_INVALID_ARG;
    }

    return device_.queue_buffer(V4L2_BUF_TYPE_VIDEO_CAPTURE, buffers_[frame - frames_.data()]);
}

bool VideoCapture::is_init() const
{
    return is_init_;
}

bool VideoCapture::is_start() const
{
    return is_start_;
}

uint32_t VideoCapture::get_width() const
{
    if (!is_init_) {
        ESP_LOGE(TAG, "Device not initialized");
        return 0;
    }
    return width_;
}

uint32_t VideoCapture::get_height() const
{
    if (!is_init_) {
        ESP_LOGE(TAG, "Device not initialized");
        return 0;
    }
    return height_;
}

float VideoCapture::get_fps() const
{
    if (!is_init_) {
        ESP_LOGE(TAG, "Device not initialized");
        return 0;
    }
    return fps_;
}

uint32_t VideoCapture::get_pixel_format() const
{
    if (!is_init_) {
        ESP_LOGE(TAG, "Device not initialized");
        return 0;
    }
    return cfg_.pixel_format;
}

const char *VideoCapture::get_device_name() const
{
    if (!is_init_) {
        ESP_LOGE(TAG, "Device not initialized");
        return nullptr;
    }
    return cfg_.device_name;
}

uint32_t VideoCapture::get_buffer_count() const
{
    if (!is_init_) {
        ESP_LOGE(TAG, "Device not initialized");
        return 0;
    }
    return cfg_.buffer_count;
}

enum v4l2_memory VideoCapture::get_buffer_mem_type() const
{
    if (!is_init_) {
        ESP_LOGE(TAG, "Device not initialized");
        return V4L2_MEMORY_MMAP;
    }
    return cfg_.buffer_mem_type;
}

bool VideoCapture::get_vflip() const
{
    if (!is_init_) {
        ESP_LOGE(TAG, "Device not initialized");
        return false;
    }
    return cfg_.vflip;
}

bool VideoCapture::get_hflip() const
{
    if (!is_init_) {
        ESP_LOGE(TAG, "Device not initialized");
        return false;
    }
    return cfg_.hflip;
}

void VideoCapture::cleanup(const Config &cfg) {
    for (int i = 0; i < frames_.size(); i++) {
        if (cfg.buffer_mem_type == V4L2_MEMORY_MMAP) {
            device_.unmap_buffer(frames_[i].data, buffers_[i].length);
        } else if (cfg.buffer_mem_type == V4L2_MEMORY_USERPTR) {
            heap_caps_free(frames_[i].data);
        } else {
            abort();
        }
    }
    frames_.clear();
    buffers_.clear();
    ESP_ERROR_CHECK(device_.close());
}

void VideoCapture::print_frame_params()
{
    ESP_LOGI(TAG, "Available frame params:");
    auto params = enum_frame_params();
    for (const auto &p : params) {
        std::string line = std::format("{:<30}{:<20}{:<20}",
                                       p.pixel_format_desc,
                                       std::format("{}x{}", p.width, p.height),
                                       std::format("{:.1f} fps", p.fps));
        ESP_LOGI(TAG, "\t%s", line.c_str());
    }
}

void VideoCapture::print_device_info(bool chip_id)
{
    struct v4l2_capability capability;
    if (device_.query_capability(&capability) == ESP_OK) {
        ESP_LOGI(TAG, "version: %d.%d.%d", (uint16_t)(capability.version >> 16),
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
        if (capability.capabilities & V4L2_CAP_TIMEPERFRAME) {
            ESP_LOGI(TAG, "\tTIMEPERFRAME");
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
            if (capability.device_caps & V4L2_CAP_TIMEPERFRAME) {
                ESP_LOGI(TAG, "\tTIMEPERFRAME");
            }
        }
    }

    // Get sensor chip ID
    if (chip_id) {
        esp_cam_sensor_id_t id;
        if (device_.get_chip_id(&id) == ESP_OK) {
            ESP_LOGI(TAG, "chip id: 0x%" PRIx16, id.pid);
        } else {
            ESP_LOGW(TAG, "failed to get chip id");
        }
    }
}

std::vector<VideoCapture::FrameParams> VideoCapture::enum_frame_params()
{
    if (!device_.is_open()) {
        return {};
    }
    std::vector<FrameParams> params;
    for (uint32_t fmt_index = 0;; fmt_index++) {
        struct v4l2_fmtdesc fmtdesc;
        if (device_.enum_format(V4L2_BUF_TYPE_VIDEO_CAPTURE, fmt_index, &fmtdesc) != ESP_OK) {
            break;
        }
        uint32_t fmt_frmsize_index = 0;
        for (;; fmt_frmsize_index++) {
            struct v4l2_frmsizeenum frmsize;
            if (device_.enum_frame_sizes(V4L2_BUF_TYPE_VIDEO_CAPTURE, fmt_frmsize_index, fmtdesc.pixelformat, &frmsize) != ESP_OK) {
                break;
            }
            uint32_t fmt_frmival_index = 0;
            for (;; fmt_frmival_index++) {
                struct v4l2_frmivalenum frmival;
                if (device_.enum_frame_intervals(
                        V4L2_BUF_TYPE_VIDEO_CAPTURE, fmt_frmival_index, fmtdesc.pixelformat,
                        frmsize.discrete.width, frmsize.discrete.height, &frmival) != ESP_OK) {
                    break;
                }
                params.emplace_back(fmtdesc.pixelformat, (char *)fmtdesc.description,
                                    frmsize.discrete.width, frmsize.discrete.height,
                                    (float)frmival.discrete.denominator / frmival.discrete.numerator);
            }
            if (fmt_frmival_index == 0) {
                float fps;
                device_.get_fps(V4L2_BUF_TYPE_VIDEO_CAPTURE, &fps);
                params.emplace_back(fmtdesc.pixelformat, (char *)fmtdesc.description,
                                    frmsize.discrete.width, frmsize.discrete.height, fps);
            }
        }
        if (fmt_frmsize_index == 0) {
            struct v4l2_format v4l2_fmt;
            device_.get_format(V4L2_BUF_TYPE_VIDEO_CAPTURE, &v4l2_fmt);
            float fps;
            device_.get_fps(V4L2_BUF_TYPE_VIDEO_CAPTURE, &fps);
            params.emplace_back(fmtdesc.pixelformat, (char *)fmtdesc.description,
                                v4l2_fmt.fmt.pix.width, v4l2_fmt.fmt.pix.height, fps);
        }
    }
    return params;
}
