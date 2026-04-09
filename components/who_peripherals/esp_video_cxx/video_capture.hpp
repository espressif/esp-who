/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: ESPRESSIF MIT
 */

#pragma once

#include "v4l2_device.hpp"
#include <optional>
#include <string>
#include <vector>
#include "freertos/portmacro.h"
#include "esp_video_device.h"

/**
 * @brief VideoCapture class for capturing video frames from camera devices
 *
 * This class provides a C++ wrapper around the V4L2 video capture interface,
 * supporting MIPI-CSI, DVP, SPI, and USB UVC camera devices.
 *
 * Usage example:
 * @code
 * VideoCapture capture;
 * auto cfg = VideoCapture::Config(ESP_VIDEO_MIPI_CSI_DEVICE_NAME,
 *                                  V4L2_PIX_FMT_YUV420, 4);
 *
 * capture.init(cfg);
 * capture.start();
 *
 * VideoCapture::Frame *frame;
 * if (capture.get_frame(&frame) == ESP_OK) {
 *     // Process frame->data, frame->size
 *     capture.return_frame(frame);
 * }
 *
 * capture.stop();
 * capture.deinit();
 * @endcode
 *
 * UVC device example:
 * @code
 * auto cfg = VideoCapture::Config(ESP_VIDEO_USB_UVC_NAME(0),
 *                                  V4L2_PIX_FMT_MJPEG, 4)
 *     .set_uvc_config({1280, 720, 30.0f});
 * @endcode
 */
class VideoCapture {
public:
    /**
     * @brief Configuration structure for VideoCapture
     */
    struct Config {
        /**
         * @brief UVC device specific configuration
         */
        struct UVCConfig {
            uint32_t width;     /*!< Image width in pixels */
            uint32_t height;    /*!< Image height in pixels */
            float fps;          /*!< Frames per second */
        };

        /**
         * @brief Construct a Config with required parameters
         * @param device_name Device name, e.g., ESP_VIDEO_MIPI_CSI_DEVICE_NAME
         * @param pixel_format Pixel format, e.g., V4L2_PIX_FMT_YUV420
         * @param buffer_count Number of buffers to allocate
         */
        Config(const char *device_name, uint32_t pixel_format, uint32_t buffer_count)
            : device_name(device_name)
            , pixel_format(pixel_format)
            , buffer_count(buffer_count) {}

        /**
         * @brief Set buffer memory type
         * @param mem_type Memory type (default: V4L2_MEMORY_MMAP)
         * @return Config&
         */
        Config &set_buffer_mem_type(enum v4l2_memory mem_type) {
            buffer_mem_type = mem_type;
            return *this;
        }

        /**
         * @brief Enable vertical flip
         * @param enable Enable/disable vertical flip (default: true)
         * @return Config&
         */
        Config &set_vflip(bool enable = true) {
            vflip = enable;
            return *this;
        }

        /**
         * @brief Enable horizontal flip
         * @param enable Enable/disable horizontal flip (default: true)
         * @return Config&
         */
        Config &set_hflip(bool enable = true) {
            hflip = enable;
            return *this;
        }

        /**
         * @brief Set UVC configuration
         * @param cfg UVC configuration
         * @return Config&
         */
        Config &set_uvc_config(const UVCConfig &cfg) {
            uvc_cfg = cfg;
            return *this;
        }

        /**
         * @brief Validate the configuration
         * @return esp_err_t
         *      - ESP_OK: Valid configuration
         *      - ESP_ERR_INVALID_ARG: Invalid configuration
         */
        esp_err_t validate() const;

        // Config members
        const char *device_name;                            /*!< Device name (required) */
        uint32_t pixel_format;                              /*!< Pixel format (required) */
        uint32_t buffer_count;                              /*!< Number of buffers (required) */
        enum v4l2_memory buffer_mem_type = V4L2_MEMORY_MMAP;/*!< Buffer memory type */
        bool vflip = false;                                 /*!< Vertical flip */
        bool hflip = false;                                 /*!< Horizontal flip */
        std::optional<UVCConfig> uvc_cfg;                   /*!< UVC configuration (required for UVC devices) */
    };

    /**
     * @brief Frame data structure
     */
    struct Frame {
        void *data;                 /*!< Pointer to frame data */
        size_t size;                /*!< Valid data size in bytes */
        uint32_t width;             /*!< Frame width in pixels */
        uint32_t height;            /*!< Frame height in pixels */
        uint32_t pixel_format;      /*!< Pixel format */
        int64_t timestamp;         /*!< Capture timestamp */
    };

    /**
     * @brief Frame parameters structure
     */
    struct FrameParams {
        uint32_t pixel_format;
        std::string pixel_format_desc;
        uint32_t width;
        uint32_t height;
        float fps;

        FrameParams(uint32_t fmt, const std::string &desc, uint32_t w, uint32_t h, float f)
            : pixel_format(fmt), pixel_format_desc(desc), width(w), height(h), fps(f) {}
    };

    /**
     * @brief Construct a new VideoCapture object
     */
    VideoCapture();

    /**
     * @brief Destroy the VideoCapture object
     *
     * @note Automatically calls deinit() if the device is still open
     */
    ~VideoCapture();

    // Disable copy
    VideoCapture(const VideoCapture &) = delete;
    VideoCapture &operator=(const VideoCapture &) = delete;

    /**
     * @brief Initialize the video capture device
     *
     * This function opens the device, sets the format, allocates buffers,
     * and maps them into memory.
     *
     * @param cfg Configuration parameters
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_ARG: Invalid configuration
     *      - ESP_ERR_NOT_FOUND: Device not found
     *      - ESP_ERR_NO_MEM: Memory allocation failed
     *      - ESP_FAIL: Other failure
     */
    esp_err_t init(const Config &cfg);

    /**
     * @brief Deinitialize the video capture device
     *
     * This function stops streaming, unmaps buffers, and closes the device.
     *
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not initialized
     */
    esp_err_t deinit();

    /**
     * @brief Start video streaming
     *
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not initialized or already started
     */
    esp_err_t start();

    /**
     * @brief Stop video streaming
     *
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not started
     */
    esp_err_t stop();

    /**
     * @brief Grab a frame from the capture device
     *
     * This function blocks until a frame is available or timeout occurs.
     * After processing the frame, call return_frame() to return the buffer.
     *
     * @param[out] frame Pointer to frame data structure to fill
     * @param timeout_ms Timeout in milliseconds (default: infinite)
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_TIMEOUT: Timeout occurred
     *      - ESP_ERR_INVALID_STATE: Device not started
     */
    esp_err_t get_frame(Frame **frame, uint32_t timeout_ms = portMAX_DELAY);

    /**
     * @brief Release a frame buffer back to the capture device
     *
     * This function must be called after processing a frame obtained from get_frame().
     *
     * @param frame Frame to release
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_ARG: Invalid frame
     */
    esp_err_t return_frame(Frame *frame);

    /**
     * @brief Check if the device is initialized
     *
     * @return true Device is initialized
     * @return false Device is not initialized
     */
    bool is_init() const;

    /**
     * @brief Check if the device is started
     *
     * @return true Device is started
     * @return false Device is not started
     */
    bool is_start() const;

    /**
     * @brief Get current frame width
     *
     * @return uint32_t Width in pixels
     */
    uint32_t get_width() const;

    /**
     * @brief Get current frame height
     *
     * @return uint32_t Height in pixels
     */
    uint32_t get_height() const;

    /**
     * @brief Get current frame rate
     *
     * @return float Frames per second
     */
    float get_fps() const;

    /**
     * @brief Get current pixel format
     *
     * @return uint32_t Pixel format
     */
    uint32_t get_pixel_format() const;

    /**
     * @brief Get device name
     *
     * @return const char* Device name
     */
    const char *get_device_name() const;

    /**
     * @brief Get buffer count
     *
     * @return uint32_t Number of buffers
     */
    uint32_t get_buffer_count() const;

    /**
     * @brief Get buffer memory type
     *
     * @return enum v4l2_memory Buffer memory type
     */
    enum v4l2_memory get_buffer_mem_type() const;

    /**
     * @brief Get vertical flip setting
     *
     * @return true Vertical flip enabled
     * @return false Vertical flip disabled
     */
    bool get_vflip() const;

    /**
     * @brief Get horizontal flip setting
     *
     * @return true Horizontal flip enabled
     * @return false Horizontal flip disabled
     */
    bool get_hflip() const;

private:
    void cleanup(const Config &cfg);

    /**
     * @brief Print device information and get sensor chip ID
     *
     * This function queries device capabilities and sensor chip ID,
     * then prints the information to the log.
     *
     * @param chip_id Whether to print chip ID information
     */
    void print_device_info(bool chip_id);

    /**
     * @brief Print supported frame parameters
     *
     * This function enumerates and prints all supported frame sizes,
     * pixel formats, and frame rates.
     */
    void print_frame_params();

    /**
     * @brief Enumerate all supported frame parameters
     *
     * @return std::vector<FrameParams> List of supported frame parameters
     */
    std::vector<FrameParams> enum_frame_params();

    V4L2Device device_;         /*!< V4L2 device */
    Config cfg_;
    uint32_t width_;
    uint32_t height_;
    float fps_;
    std::vector<struct v4l2_buffer> buffers_;
    std::vector<Frame> frames_;
    bool is_init_;              /*!< Initialized flag */
    bool is_start_;             /*!< Started flag */
};
