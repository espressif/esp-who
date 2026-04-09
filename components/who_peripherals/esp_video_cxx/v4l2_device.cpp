/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: ESPRESSIF MIT
 */

#include "v4l2_device.hpp"
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include "esp_log.h"
#include "esp_video_ioctl.h"

static const char *TAG = "V4L2Device";

V4L2Device::V4L2Device()
    : fd_(-1)
{
}

V4L2Device::~V4L2Device()
{
    if (is_open()) {
        close();
    }
}

esp_err_t V4L2Device::open(const char *device_path, int flags)
{
    if (device_path == nullptr) {
        ESP_LOGE(TAG, "Device path is null");
        return ESP_ERR_INVALID_ARG;
    }

    if (is_open()) {
        ESP_LOGW(TAG, "Device already open, closing first");
        close();
    }

    fd_ = ::open(device_path, flags);
    if (fd_ < 0) {
        ESP_LOGE(TAG, "Failed to open device %s: %s", device_path, strerror(errno));
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::close()
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (::close(fd_) != 0) {
        ESP_LOGE(TAG, "Failed to close device: %s", strerror(errno));
        return ESP_FAIL;
    }

    fd_ = -1;
    return ESP_OK;
}

bool V4L2Device::is_open() const
{
    return fd_ >= 0;
}

int V4L2Device::get_fd() const
{
    return fd_;
}

esp_err_t V4L2Device::query_capability(struct v4l2_capability *capability)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (capability == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(capability, 0, sizeof(*capability));
    if (::ioctl(fd_,VIDIOC_QUERYCAP, capability) != 0) {
        ESP_LOGE(TAG, "Failed to query capabilities: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::set_format(enum v4l2_buf_type type, uint32_t width, uint32_t height,
                                uint32_t pixel_format, uint32_t *actual_width,
                                uint32_t *actual_height)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    struct v4l2_format format;
    memset(&format, 0, sizeof(format));
    format.type = type;
    format.fmt.pix.width = width;
    format.fmt.pix.height = height;
    format.fmt.pix.pixelformat = pixel_format;
    format.fmt.pix.field = V4L2_FIELD_NONE;

    esp_err_t ret = set_format(format);
    if (ret != ESP_OK) {
        return ret;
    }

    if (actual_width != nullptr) {
        *actual_width = format.fmt.pix.width;
    }
    if (actual_height != nullptr) {
        *actual_height = format.fmt.pix.height;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::set_format(struct v4l2_format &format)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (::ioctl(fd_,VIDIOC_S_FMT, &format) != 0) {
        ESP_LOGE(TAG, "Failed to set format: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::get_format(enum v4l2_buf_type type, struct v4l2_format *format)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (format == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(format, 0, sizeof(*format));
    format->type = type;

    if (::ioctl(fd_,VIDIOC_G_FMT, format) != 0) {
        ESP_LOGE(TAG, "Failed to get format: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::request_buffers(enum v4l2_buf_type type, enum v4l2_memory memory,
                                     uint32_t count, uint32_t *actual_count)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = count;
    req.type = type;
    req.memory = memory;

    if (::ioctl(fd_,VIDIOC_REQBUFS, &req) != 0) {
        ESP_LOGE(TAG, "Failed to request buffers: %s", strerror(errno));
        return ESP_FAIL;
    }

    if (actual_count != nullptr) {
        *actual_count = req.count;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::query_buffer(enum v4l2_buf_type type, uint32_t index, enum v4l2_memory mem, struct v4l2_buffer *buf)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    memset(buf, 0, sizeof(*buf));
    buf->type = type;
    buf->memory = mem;
    buf->index = index;

    if (::ioctl(fd_, VIDIOC_QUERYBUF, buf) != 0) {
        ESP_LOGE(TAG, "Failed to query buffer %u: %s", index, strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::map_buffer(uint32_t offset, uint32_t length, void **ptr)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (ptr == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    *ptr = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, offset);
    if (*ptr == MAP_FAILED) {
        ESP_LOGE(TAG, "Failed to map buffer");
        *ptr = nullptr;
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::unmap_buffer(void *ptr, uint32_t length)
{
    if (ptr == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (munmap(ptr, length) != 0) {
        ESP_LOGE(TAG, "Failed to unmap buffer: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::queue_buffer(enum v4l2_buf_type type, struct v4l2_buffer &buf)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (::ioctl(fd_,VIDIOC_QBUF, &buf) != 0) {
        ESP_LOGE(TAG, "Failed to queue buffer: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::dequeue_buffer(enum v4l2_buf_type type, enum v4l2_memory mem, struct v4l2_buffer *buf,
                                   uint32_t timeout_ms)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    set_dqbuf_timeout(timeout_ms);

    memset(buf, 0, sizeof(*buf));
    buf->type = type;
    buf->memory = mem;

    if (::ioctl(fd_,VIDIOC_DQBUF, buf) != 0) {
        ESP_LOGE(TAG, "Failed to dequeue mmap buffer: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::stream_on(enum v4l2_buf_type type)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    uint32_t t = type;
    if (::ioctl(fd_,VIDIOC_STREAMON, &t) != 0) {
        ESP_LOGE(TAG, "Failed to start streaming: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::stream_off(enum v4l2_buf_type type)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    uint32_t t = type;
    if (::ioctl(fd_,VIDIOC_STREAMOFF, &t) != 0) {
        ESP_LOGE(TAG, "Failed to stop streaming: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::set_control(uint32_t ctrl_class, uint32_t id, int32_t value)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    struct v4l2_ext_controls controls;
    struct v4l2_ext_control control;

    memset(&controls, 0, sizeof(controls));
    memset(&control, 0, sizeof(control));

    controls.ctrl_class = ctrl_class;
    controls.count = 1;
    controls.controls = &control;

    control.id = id;
    control.value = value;

    return set_controls(controls);
}

esp_err_t V4L2Device::get_control(uint32_t ctrl_class, uint32_t id, int32_t *value)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (value == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    struct v4l2_ext_controls controls;
    struct v4l2_ext_control control;

    memset(&controls, 0, sizeof(controls));
    memset(&control, 0, sizeof(control));

    controls.ctrl_class = ctrl_class;
    controls.count = 1;
    controls.controls = &control;

    control.id = id;

    esp_err_t ret = get_controls(&controls);
    if (ret == ESP_OK) {
        *value = control.value;
    }

    return ret;
}

esp_err_t V4L2Device::set_controls(struct v4l2_ext_controls &ctrls)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (::ioctl(fd_,VIDIOC_S_EXT_CTRLS, &ctrls) != 0) {
        ESP_LOGE(TAG, "Failed to set controls: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::get_controls(struct v4l2_ext_controls *ctrls)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (ctrls == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (::ioctl(fd_,VIDIOC_G_EXT_CTRLS, ctrls) != 0) {
        ESP_LOGE(TAG, "Failed to get controls: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::query_control(struct v4l2_query_ext_ctrl *qctrl)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (qctrl == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (::ioctl(fd_,VIDIOC_QUERY_EXT_CTRL, qctrl) != 0) {
        ESP_LOGE(TAG, "Failed to query control: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::get_chip_id(esp_cam_sensor_id_t *chip_id)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (chip_id == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    struct v4l2_ext_control control;
    struct v4l2_ext_controls controls;

    memset(&controls, 0, sizeof(controls));
    memset(&control, 0, sizeof(control));
    memset(chip_id, 0, sizeof(esp_cam_sensor_id_t));

    controls.ctrl_class = V4L2_CTRL_CLASS_ESP_CAM_IOCTL;
    controls.count = 1;
    controls.controls = &control;
    control.id = ESP_CAM_SENSOR_IOC_G_CHIP_ID;
    control.p_u8 = (uint8_t *)chip_id;
    control.size = sizeof(esp_cam_sensor_id_t);

    esp_err_t ret = get_controls(&controls);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get chip id: %s", strerror(errno));
        return ret;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::set_selection(struct v4l2_selection &selection)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (::ioctl(fd_,VIDIOC_S_SELECTION, &selection) != 0) {
        ESP_LOGE(TAG, "Failed to set selection: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::get_selection(struct v4l2_selection *selection)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (selection == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (::ioctl(fd_,VIDIOC_G_SELECTION, selection) != 0) {
        ESP_LOGE(TAG, "Failed to get selection: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::set_parm(struct v4l2_streamparm &parm)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (::ioctl(fd_,VIDIOC_S_PARM, &parm) != 0) {
        ESP_LOGE(TAG, "Failed to set parameters: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::get_parm(struct v4l2_streamparm *parm)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (parm == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (::ioctl(fd_,VIDIOC_G_PARM, parm) != 0) {
        ESP_LOGE(TAG, "Failed to get parameters: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::set_fps_by_numerator(enum v4l2_buf_type type, float fps, uint32_t numerator)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (fps <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    struct v4l2_streamparm parm;
    memset(&parm, 0, sizeof(parm));
    parm.type = type;
    parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    parm.parm.capture.timeperframe.numerator = numerator;
    parm.parm.capture.timeperframe.denominator = (uint32_t)(fps * numerator + 0.5f);

    return set_parm(parm);
}

esp_err_t V4L2Device::set_fps_by_denominator(enum v4l2_buf_type type, float fps, uint32_t denominator)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (fps <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    struct v4l2_streamparm parm;
    memset(&parm, 0, sizeof(parm));
    parm.type = type;
    parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    parm.parm.capture.timeperframe.numerator = (uint32_t)(denominator / fps + 0.5f);
    parm.parm.capture.timeperframe.denominator = denominator;

    return set_parm(parm);
}

esp_err_t V4L2Device::get_fps(enum v4l2_buf_type type, float *fps)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (fps == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    struct v4l2_streamparm parm;
    memset(&parm, 0, sizeof(parm));
    parm.type = type;
    parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;

    esp_err_t ret = get_parm(&parm);
    if (ret != ESP_OK) {
        return ret;
    }

    if (parm.parm.capture.timeperframe.numerator == 0) {
        return ESP_FAIL;
    }

    *fps = (float)parm.parm.capture.timeperframe.denominator / parm.parm.capture.timeperframe.numerator;
    return ESP_OK;
}

esp_err_t V4L2Device::set_dqbuf_timeout(uint32_t timeout_ms)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    return set_dqbuf_timeout(tv);
}

esp_err_t V4L2Device::set_dqbuf_timeout(const struct timeval &timeout)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    struct timeval tv = timeout;
    if (::ioctl(fd_,VIDIOC_S_DQBUF_TIMEOUT, &tv) != 0) {
        ESP_LOGE(TAG, "Failed to set DQBUF timeout: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::get_dqbuf_timeout(struct timeval *timeout)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (timeout == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (::ioctl(fd_,VIDIOC_G_DQBUF_TIMEOUT, timeout) != 0) {
        ESP_LOGE(TAG, "Failed to get DQBUF timeout: %s", strerror(errno));
        return ESP_FAIL;
    }

    return ESP_OK;
}


esp_err_t V4L2Device::enum_format(enum v4l2_buf_type type, uint32_t index, struct v4l2_fmtdesc *fmtdesc)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (fmtdesc == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(fmtdesc, 0, sizeof(*fmtdesc));
    fmtdesc->index = index;
    fmtdesc->type = type;

    if (::ioctl(fd_, VIDIOC_ENUM_FMT, fmtdesc) != 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::enum_frame_sizes(enum v4l2_buf_type type, uint32_t index, uint32_t pixel_format, struct v4l2_frmsizeenum *frmsize)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (frmsize == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(frmsize, 0, sizeof(*frmsize));
    frmsize->index = index;
    frmsize->pixel_format = pixel_format;
    frmsize->type = type;

    if (::ioctl(fd_, VIDIOC_ENUM_FRAMESIZES, frmsize) != 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t V4L2Device::enum_frame_intervals(enum v4l2_buf_type type, uint32_t index, uint32_t pixel_format, uint32_t width, uint32_t height, struct v4l2_frmivalenum *frmival)
{
    if (!is_open()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (frmival == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(frmival, 0, sizeof(*frmival));
    frmival->index = index;
    frmival->pixel_format = pixel_format;
    frmival->width = width;
    frmival->height = height;
    frmival->type = type;

    if (::ioctl(fd_, VIDIOC_ENUM_FRAMEINTERVALS, frmival) != 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}