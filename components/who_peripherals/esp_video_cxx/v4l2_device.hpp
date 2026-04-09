/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: ESPRESSIF MIT
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <sys/time.h>
#include "esp_err.h"
#include "linux/videodev2.h"
#include "esp_cam_sensor_types.h"
#include <vector>
#include <string>

/**
 * @brief V4L2 device class for low-level video device operations
 *
 * This class encapsulates all V4L2 ioctl operations, providing a clean
 * interface for device management, buffer handling, and control operations.
 */
class V4L2Device {
public:
    V4L2Device();
    virtual ~V4L2Device();

    // Disable copy
    V4L2Device(const V4L2Device &) = delete;
    V4L2Device &operator=(const V4L2Device &) = delete;

    //==============================
    // Device Operations
    //==============================

    /**
     * @brief Open a V4L2 device
     *
     * @param device_path Path to the device (e.g., "/dev/video0")
     * @param flags Open flags (e.g., O_RDWR)
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_ARG: Invalid path
     *      - ESP_FAIL: Failed to open device
     */
    esp_err_t open(const char *device_path, int flags);

    /**
     * @brief Close the V4L2 device
     *
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     */
    esp_err_t close();

    /**
     * @brief Check if the device is open
     *
     * @return true Device is open
     * @return false Device is not open
     */
    bool is_open() const;

    /**
     * @brief Get the file descriptor
     *
     * @return int File descriptor, or -1 if not open
     */
    int get_fd() const;

    //==============================
    // Capability Operations
    //==============================

    /**
     * @brief Query device capabilities
     *
     * @param[out] capability Pointer to store capability information
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_ERR_INVALID_ARG: Invalid argument
     *      - ESP_FAIL: Failed to query capabilities
     */
    esp_err_t query_capability(struct v4l2_capability *capability);

    //==============================
    // Format Operations
    //==============================

    /**
     * @brief Set video format
     *
     * @param type Buffer type
     * @param width Frame width
     * @param height Frame height
     * @param pixel_format Pixel format (e.g., V4L2_PIX_FMT_YUV420)
     * @param[out] actual_width Actual width set by driver (optional)
     * @param[out] actual_height Actual height set by driver (optional)
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to set format
     */
    esp_err_t set_format(enum v4l2_buf_type type, uint32_t width, uint32_t height,
                        uint32_t pixel_format, uint32_t *actual_width = nullptr,
                        uint32_t *actual_height = nullptr);

    /**
     * @brief Set video format using v4l2_format structure
     *
     * @param[in,out] format Format structure to set
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to set format
     */
    esp_err_t set_format(struct v4l2_format &format);

    /**
     * @brief Get current video format
     *
     * @param type Buffer type
     * @param[out] format Pointer to store format information
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_ERR_INVALID_ARG: Invalid argument
     *      - ESP_FAIL: Failed to get format
     */
    esp_err_t get_format(enum v4l2_buf_type type, struct v4l2_format *format);

    //==============================
    // Buffer Operations
    //==============================

    /**
     * @brief Request buffers from the device
     *
     * @param type Buffer type
     * @param memory Memory type
     * @param count Number of buffers to request
     * @param[out] actual_count Actual number of buffers allocated (optional)
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to request buffers
     */
    esp_err_t request_buffers(enum v4l2_buf_type type, enum v4l2_memory memory,
                             uint32_t count, uint32_t *actual_count = nullptr);

    /**
     * @brief Query buffer information
     *
     * @param type Buffer type
     * @param index Buffer index
     * @param mem Memory type
     * @param[out] buf Pointer to store buffer information
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_ERR_INVALID_ARG: Invalid argument
     *      - ESP_FAIL: Failed to query buffer
     */
    esp_err_t query_buffer(enum v4l2_buf_type type, uint32_t index, enum v4l2_memory mem, struct v4l2_buffer *buf);

    /**
     * @brief Map buffer memory to user space
     *
     * @param offset Buffer offset
     * @param length Buffer length
     * @param[out] ptr Pointer to store mapped address
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to map buffer
     */
    esp_err_t map_buffer(uint32_t offset, uint32_t length, void **ptr);

    /**
     * @brief Unmap buffer memory
     *
     * @param ptr Mapped address
     * @param length Buffer length
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_FAIL: Failed to unmap buffer
     */
    esp_err_t unmap_buffer(void *ptr, uint32_t length);

    /**
     * @brief Queue a buffer to the device
     *
     * @param type Buffer type
     * @param[in,out] buf Buffer structure to queue
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to queue buffer
     */
    esp_err_t queue_buffer(enum v4l2_buf_type type, struct v4l2_buffer &buf);

    /**
     * @brief Dequeue a buffer from the device
     *
     * @param type Buffer type
     * @param mem Memory type
     * @param[out] buf Pointer to store buffer information
     * @param timeout_ms Timeout in milliseconds
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_ERR_TIMEOUT: Timeout occurred
     *      - ESP_FAIL: Failed to dequeue buffer
     */
    esp_err_t dequeue_buffer(enum v4l2_buf_type type, enum v4l2_memory mem, struct v4l2_buffer *buf,
                                   uint32_t timeout_ms);

    //==============================
    // Stream Operations
    //==============================

    /**
     * @brief Start streaming
     *
     * @param type Buffer type
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to start streaming
     */
    esp_err_t stream_on(enum v4l2_buf_type type);

    /**
     * @brief Stop streaming
     *
     * @param type Buffer type
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to stop streaming
     */
    esp_err_t stream_off(enum v4l2_buf_type type);

    //==============================
    // Control Operations
    //==============================

    /**
     * @brief Set a control value
     *
     * @param ctrl_class Control class
     * @param id Control ID
     * @param value Control value
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to set control
     */
    esp_err_t set_control(uint32_t ctrl_class, uint32_t id, int32_t value);

    /**
     * @brief Get a control value
     *
     * @param ctrl_class Control class
     * @param id Control ID
     * @param[out] value Pointer to store control value
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to get control
     */
    esp_err_t get_control(uint32_t ctrl_class, uint32_t id, int32_t *value);

    /**
     * @brief Set multiple controls
     *
     * @param ctrls Controls structure to set
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to set controls
     */
    esp_err_t set_controls(struct v4l2_ext_controls &ctrls);

    /**
     * @brief Get multiple controls
     *
     * @param[in,out] ctrls Pointer to controls structure
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to get controls
     */
    esp_err_t get_controls(struct v4l2_ext_controls *ctrls);

    /**
     * @brief Query control information
     *
     * @param[in,out] qctrl Pointer to query structure
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to query control
     */
    esp_err_t query_control(struct v4l2_query_ext_ctrl *qctrl);

    /**
     * @brief Get camera sensor chip ID
     *
     * @param[out] chip_id Pointer to store the chip ID information
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to get chip ID
     */
    esp_err_t get_chip_id(esp_cam_sensor_id_t *chip_id);

    //==============================
    // Selection Operations
    //==============================

    /**
     * @brief Set selection (crop/compose)
     *
     * @param[in,out] selection Selection structure to set
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to set selection
     */
    esp_err_t set_selection(struct v4l2_selection &selection);

    /**
     * @brief Get selection (crop/compose)
     *
     * @param[in,out] selection Pointer to store selection information
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to get selection
     */
    esp_err_t get_selection(struct v4l2_selection *selection);

    //==============================
    // Parameter Operations
    //==============================

    /**
     * @brief Set stream parameters
     *
     * @param[in,out] parm Parameter structure to set
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to set parameters
     */
    esp_err_t set_parm(struct v4l2_streamparm &parm);

    /**
     * @brief Get stream parameters
     *
     * @param[out] parm Pointer to store parameter information
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to get parameters
     */
    esp_err_t get_parm(struct v4l2_streamparm *parm);

    /**
     * @brief Set frame rate with fixed numerator
     *
     * @param type Buffer type
     * @param fps Target frame rate
     * @param numerator Fixed numerator for timeperframe
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_ERR_INVALID_ARG: Invalid FPS
     *      - ESP_FAIL: Failed to set FPS
     */
    esp_err_t set_fps_by_numerator(enum v4l2_buf_type type, float fps, uint32_t numerator);

    /**
     * @brief Set frame rate with fixed denominator
     *
     * @param type Buffer type
     * @param fps Target frame rate
     * @param denominator Fixed denominator for timeperframe
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_ERR_INVALID_ARG: Invalid FPS
     *      - ESP_FAIL: Failed to set FPS
     */
    esp_err_t set_fps_by_denominator(enum v4l2_buf_type type, float fps, uint32_t denominator);

    /**
     * @brief Get current frame rate
     *
     * @param type Buffer type
     * @param[out] fps Pointer to store frame rate
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to get FPS
     */
    esp_err_t get_fps(enum v4l2_buf_type type, float *fps);

    //==============================
    // Timeout Operations
    //==============================

    /**
     * @brief Set dequeue buffer timeout in milliseconds
     *
     * @param timeout_ms Timeout in milliseconds
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to set timeout
     */
    esp_err_t set_dqbuf_timeout(uint32_t timeout_ms);

    /**
     * @brief Set dequeue buffer timeout using timeval
     *
     * @param timeout Timeout value
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to set timeout
     */
    esp_err_t set_dqbuf_timeout(const struct timeval &timeout);

    /**
     * @brief Get dequeue buffer timeout
     *
     * @param[out] timeout Pointer to store timeout value
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_FAIL: Failed to get timeout
     */
    esp_err_t get_dqbuf_timeout(struct timeval *timeout);

    //==============================
    // Enumeration Operations
    //==============================

    /**
     * @brief Enumerate supported pixel formats
     *
     * @param type Buffer type
     * @param index Format index
     * @param[out] fmtdesc Pointer to store format description
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_ERR_INVALID_ARG: Invalid argument
     *      - ESP_FAIL: No more formats
     */
    esp_err_t enum_format(enum v4l2_buf_type type, uint32_t index, struct v4l2_fmtdesc *fmtdesc);

    /**
     * @brief Enumerate frame sizes
     *
     * @param type Buffer type
     * @param index Frame size index
     * @param pixel_format Pixel format
     * @param[out] frmsize Pointer to store frame size information
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_ERR_INVALID_ARG: Invalid argument
     *      - ESP_FAIL: No more frame sizes
     */
    esp_err_t enum_frame_sizes(enum v4l2_buf_type type, uint32_t index, uint32_t pixel_format, struct v4l2_frmsizeenum *frmsize);

    /**
     * @brief Enumerate frame intervals
     *
     * @param type Buffer type
     * @param index Frame interval index
     * @param pixel_format Pixel format
     * @param width Frame width
     * @param height Frame height
     * @param[out] frmival Pointer to store frame interval information
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not open
     *      - ESP_ERR_INVALID_ARG: Invalid argument
     *      - ESP_FAIL: No more frame intervals
     */
    esp_err_t enum_frame_intervals(enum v4l2_buf_type type, uint32_t index, uint32_t pixel_format, uint32_t width, uint32_t height, struct v4l2_frmivalenum *frmival);

protected:
    int fd_ = -1;
};
