/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: ESPRESSIF MIT
 */

#include "video_encoder.hpp"
#include <fcntl.h>
#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_video_device.h"

static const char *TAG = "VideoEncoder";

// Default H.264 parameters
static const VideoEncoder::H264Params DEFAULT_H264_PARAMS = {
    .gop = 30,
    .bitrate = 1000000,
    .min_qp = 25,
    .max_qp = 35
};

VideoEncoder::VideoEncoder()
    : output_buffer_(nullptr)
    , output_buffer_size_(0)
    , width_(0)
    , height_(0)
    , input_format_(0)
    , type_(Type::JPEG)
    , started_(false)
    , h264_params_(DEFAULT_H264_PARAMS)
{
}

VideoEncoder::~VideoEncoder()
{
    if (is_opened()) {
        deinit();
    }
}

const char *VideoEncoder::get_device_path(Type type)
{
    switch (type) {
    case Type::JPEG:
        return ESP_VIDEO_JPEG_DEVICE_NAME;
    case Type::H264:
        return ESP_VIDEO_H264_DEVICE_NAME;
    default:
        return nullptr;
    }
}

uint32_t VideoEncoder::get_output_format(Type type)
{
    switch (type) {
    case Type::JPEG:
        return V4L2_PIX_FMT_JPEG;
    case Type::H264:
        return V4L2_PIX_FMT_H264;
    default:
        return 0;
    }
}

esp_err_t VideoEncoder::init(const Config &config)
{
    if (config.width == 0 || config.height == 0) {
        ESP_LOGE(TAG, "Invalid width or height");
        return ESP_ERR_INVALID_ARG;
    }

    const char *device_path = get_device_path(config.type);
    if (device_path == nullptr) {
        ESP_LOGE(TAG, "Invalid encoder type");
        return ESP_ERR_INVALID_ARG;
    }

    // Open device
    esp_err_t ret = device_.open(device_path, O_RDWR);
    if (ret != ESP_OK) {
        return ret;
    }

    type_ = config.type;
    width_ = config.width;
    height_ = config.height;
    input_format_ = config.input_format;

    // Set OUTPUT format (input)
    ret = device_.set_format(V4L2_BUF_TYPE_VIDEO_OUTPUT,
                            config.width, config.height, config.input_format);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set OUTPUT format");
        device_.close();
        return ret;
    }

    // Request OUTPUT buffers (USERPTR mode)
    ret = device_.request_buffers(V4L2_BUF_TYPE_VIDEO_OUTPUT,
                                 V4L2_MEMORY_USERPTR, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to request OUTPUT buffers");
        device_.close();
        return ret;
    }

    // Set CAPTURE format (output)
    ret = device_.set_format(V4L2_BUF_TYPE_VIDEO_CAPTURE,
                            config.width, config.height, get_output_format(config.type));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set CAPTURE format");
        device_.close();
        return ret;
    }

    // Request CAPTURE buffers (MMAP mode)
    uint32_t buffer_count = config.buffer_count > 0 ? config.buffer_count : 1;
    ret = device_.request_buffers(V4L2_BUF_TYPE_VIDEO_CAPTURE,
                                 V4L2_MEMORY_MMAP, buffer_count);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to request CAPTURE buffers");
        device_.close();
        return ret;
    }

    // Query and map CAPTURE buffer
    struct v4l2_buffer buf;
    ret = device_.query_buffer(V4L2_BUF_TYPE_VIDEO_CAPTURE, 0, V4L2_MEMORY_MMAP, &buf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to query CAPTURE buffer");
        device_.close();
        return ret;
    }

    output_buffer_size_ = buf.length;
    ret = device_.map_buffer(buf.m.offset, buf.length, &output_buffer_);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to map CAPTURE buffer");
        device_.close();
        return ESP_ERR_NO_MEM;
    }

    // Queue CAPTURE buffer
    struct v4l2_buffer qbuf;
    memset(&qbuf, 0, sizeof(qbuf));
    qbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    qbuf.memory = V4L2_MEMORY_MMAP;
    qbuf.index = 0;
    ret = device_.queue_buffer(V4L2_BUF_TYPE_VIDEO_CAPTURE, qbuf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to queue CAPTURE buffer");
        device_.unmap_buffer(output_buffer_, output_buffer_size_);
        output_buffer_ = nullptr;
        device_.close();
        return ret;
    }

    // Set default encoder parameters
    if (type_ == Type::H264) {
        set_h264_params(h264_params_);
    }

    const char *type_str = (type_ == Type::JPEG) ? "JPEG" : "H.264";
    ESP_LOGI(TAG, "VideoEncoder initialized: %s %ux%u, input_format=%c%c%c%c",
             type_str, width_, height_,
             (char)(input_format_ & 0xFF),
             (char)((input_format_ >> 8) & 0xFF),
             (char)((input_format_ >> 16) & 0xFF),
             (char)((input_format_ >> 24) & 0xFF));

    return ESP_OK;
}

esp_err_t VideoEncoder::deinit()
{
    if (!is_opened()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (started_) {
        stop();
    }

    // Unmap output buffer
    if (output_buffer_ != nullptr) {
        device_.unmap_buffer(output_buffer_, output_buffer_size_);
        output_buffer_ = nullptr;
    }

    output_buffer_size_ = 0;

    // Close device
    device_.close();

    width_ = 0;
    height_ = 0;
    input_format_ = 0;

    ESP_LOGI(TAG, "VideoEncoder deinitialized");
    return ESP_OK;
}

esp_err_t VideoEncoder::start()
{
    if (!is_opened()) {
        ESP_LOGE(TAG, "Device not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (started_) {
        ESP_LOGW(TAG, "Already started");
        return ESP_OK;
    }

    // Start CAPTURE queue
    esp_err_t ret = device_.stream_on(V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start CAPTURE queue");
        return ret;
    }

    // Start OUTPUT queue
    ret = device_.stream_on(V4L2_BUF_TYPE_VIDEO_OUTPUT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start OUTPUT queue");
        device_.stream_off(V4L2_BUF_TYPE_VIDEO_CAPTURE);
        return ret;
    }

    started_ = true;
    ESP_LOGI(TAG, "VideoEncoder started");
    return ESP_OK;
}

esp_err_t VideoEncoder::stop()
{
    if (!is_opened()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!started_) {
        return ESP_OK;
    }

    // Stop OUTPUT queue
    device_.stream_off(V4L2_BUF_TYPE_VIDEO_OUTPUT);

    // Stop CAPTURE queue
    device_.stream_off(V4L2_BUF_TYPE_VIDEO_CAPTURE);

    started_ = false;
    ESP_LOGI(TAG, "VideoEncoder stopped");
    return ESP_OK;
}

esp_err_t VideoEncoder::encode(const uint8_t *input_data, size_t input_size,
                               EncodedData &output, uint32_t timeout_ms)
{
    if (!is_opened() || !started_) {
        ESP_LOGE(TAG, "Encoder not started");
        return ESP_ERR_INVALID_STATE;
    }

    if (input_data == nullptr || input_size == 0) {
        ESP_LOGE(TAG, "Invalid input data");
        return ESP_ERR_INVALID_ARG;
    }

    // Queue input buffer (OUTPUT queue, USERPTR mode)
    struct v4l2_buffer in_buf;
    memset(&in_buf, 0, sizeof(in_buf));
    in_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    in_buf.memory = V4L2_MEMORY_USERPTR;
    in_buf.index = 0;
    in_buf.m.userptr = (unsigned long)input_data;
    in_buf.length = input_size;
    esp_err_t ret = device_.queue_buffer(V4L2_BUF_TYPE_VIDEO_OUTPUT, in_buf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to queue input buffer");
        return ret;
    }

    // Dequeue encoded data (CAPTURE queue, MMAP mode)
    struct v4l2_buffer out_buf;
    ret = device_.dequeue_buffer(V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_MEMORY_MMAP, &out_buf, timeout_ms);
    if (ret != ESP_OK) {
        if (ret == ESP_ERR_TIMEOUT) {
            ESP_LOGD(TAG, "Encode timeout");
        } else {
            ESP_LOGE(TAG, "Failed to dequeue encoded buffer");
        }
        // Dequeue the output buffer to maintain state
        struct v4l2_buffer dummy_buf;
        device_.dequeue_buffer(V4L2_BUF_TYPE_VIDEO_OUTPUT, V4L2_MEMORY_USERPTR, &dummy_buf, 0);
        return ret;
    }

    // Dequeue the output buffer
    struct v4l2_buffer dummy_buf;
    device_.dequeue_buffer(V4L2_BUF_TYPE_VIDEO_OUTPUT, V4L2_MEMORY_USERPTR, &dummy_buf, 0);

    // Fill output structure
    output.data = (uint8_t *)output_buffer_;
    output.size = out_buf.bytesused;
    output.width = width_;
    output.height = height_;
    output.type = type_;

    // Get timestamp
    int64_t us = esp_timer_get_time();
    output.timestamp.tv_sec = us / 1000000;
    output.timestamp.tv_usec = us % 1000000;

    return ESP_OK;
}

esp_err_t VideoEncoder::release_output(const EncodedData &output)
{
    if (!is_opened()) {
        return ESP_ERR_INVALID_STATE;
    }

    // Re-queue the CAPTURE buffer
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    return device_.queue_buffer(V4L2_BUF_TYPE_VIDEO_CAPTURE, buf);
}

esp_err_t VideoEncoder::set_quality(uint32_t quality)
{
    if (!is_opened()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (type_ != Type::JPEG) {
        ESP_LOGW(TAG, "set_quality only applies to JPEG encoder");
        return ESP_ERR_INVALID_STATE;
    }

    if (quality < 1 || quality > 100) {
        ESP_LOGE(TAG, "Quality must be between 1 and 100");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = device_.set_control(V4L2_CID_JPEG_CLASS,
                                        V4L2_CID_JPEG_COMPRESSION_QUALITY, quality);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set JPEG quality");
        return ret;
    }

    ESP_LOGI(TAG, "JPEG quality set to %u", quality);
    return ESP_OK;
}

esp_err_t VideoEncoder::set_h264_params(const H264Params &params)
{
    if (!is_opened()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (type_ != Type::H264) {
        ESP_LOGW(TAG, "set_h264_params only applies to H.264 encoder");
        return ESP_ERR_INVALID_STATE;
    }

    if (params.max_qp < params.min_qp) {
        ESP_LOGE(TAG, "max_qp must be >= min_qp");
        return ESP_ERR_INVALID_ARG;
    }

    // Set GOP
    device_.set_control(V4L2_CID_CODEC_CLASS, V4L2_CID_MPEG_VIDEO_H264_I_PERIOD, params.gop);

    // Set bitrate
    device_.set_control(V4L2_CID_CODEC_CLASS, V4L2_CID_MPEG_VIDEO_BITRATE, params.bitrate);

    // Set min QP
    device_.set_control(V4L2_CID_CODEC_CLASS, V4L2_CID_MPEG_VIDEO_H264_MIN_QP, params.min_qp);

    // Set max QP
    device_.set_control(V4L2_CID_CODEC_CLASS, V4L2_CID_MPEG_VIDEO_H264_MAX_QP, params.max_qp);

    h264_params_ = params;
    ESP_LOGI(TAG, "H.264 params set: gop=%u, bitrate=%u, qp=[%u,%u]",
             params.gop, params.bitrate, params.min_qp, params.max_qp);
    return ESP_OK;
}

esp_err_t VideoEncoder::get_h264_params(H264Params &params)
{
    if (!is_opened()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (type_ != Type::H264) {
        ESP_LOGW(TAG, "get_h264_params only applies to H.264 encoder");
        return ESP_ERR_INVALID_STATE;
    }

    params = h264_params_;
    return ESP_OK;
}

esp_err_t VideoEncoder::set_control(uint32_t ctrl_class, uint32_t id, int32_t value)
{
    return device_.set_control(ctrl_class, id, value);
}

esp_err_t VideoEncoder::get_control(uint32_t ctrl_class, uint32_t id, int32_t *value)
{
    return device_.get_control(ctrl_class, id, value);
}

bool VideoEncoder::is_opened() const
{
    return device_.is_open();
}

VideoEncoder::Type VideoEncoder::get_type() const
{
    return type_;
}

uint32_t VideoEncoder::get_width() const
{
    return width_;
}

uint32_t VideoEncoder::get_height() const
{
    return height_;
}

uint32_t VideoEncoder::get_output_buffer_size() const
{
    return output_buffer_size_;
}
