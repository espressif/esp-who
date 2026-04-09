/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: ESPRESSIF MIT
 */

#pragma once

#include "v4l2_device.hpp"

/**
 * @brief VideoEncoder class for encoding video frames
 *
 * This class provides a C++ wrapper around the V4L2 M2M encoder interface,
 * supporting JPEG and H.264 hardware encoding on ESP32-P4.
 *
 * Usage example:
 * @code
 * VideoEncoder encoder;
 * VideoEncoder::Config config = {
 *     .type = VideoEncoder::Type::H264,
 *     .width = 1280,
 *     .height = 720,
 *     .input_format = V4L2_PIX_FMT_YUV420,
 *     .buffer_count = 1
 * };
 *
 * encoder.init(config);
 *
 * VideoEncoder::H264Params h264_params = {
 *     .gop = 30,
 *     .bitrate = 1000000,
 *     .min_qp = 25,
 *     .max_qp = 35
 * };
 * encoder.setH264Params(h264_params);
 * encoder.start();
 *
 * VideoEncoder::EncodedData encoded;
 * if (encoder.encode(input_data, input_size, encoded) == ESP_OK) {
 *     // Process encoded.data, encoded.size
 *     encoder.releaseOutput(encoded);
 * }
 *
 * encoder.stop();
 * encoder.deinit();
 * @endcode
 */
class VideoEncoder {
public:
    /**
     * @brief Encoder type enumeration
     */
    enum class Type {
        JPEG,                       /*!< JPEG encoding */
        H264                        /*!< H.264 encoding */
    };

    /**
     * @brief Configuration structure for VideoEncoder
     */
    struct Config {
        Type type;                  /*!< Encoder type (JPEG or H264) */
        uint32_t width;             /*!< Image width in pixels */
        uint32_t height;            /*!< Image height in pixels */
        uint32_t input_format;      /*!< Input pixel format (e.g., V4L2_PIX_FMT_YUV420) */
        uint32_t buffer_count;      /*!< Number of output buffers (default: 1) */
    };

    /**
     * @brief H.264 encoding parameters
     */
    struct H264Params {
        uint32_t gop;               /*!< GOP size (I-frame interval), range: 1-120 */
        uint32_t bitrate;           /*!< Target bitrate in bps, range: 25000-2500000 */
        uint32_t min_qp;            /*!< Minimum QP, range: 0-51 */
        uint32_t max_qp;            /*!< Maximum QP, range: 0-51 (must be >= min_qp) */
    };

    /**
     * @brief Encoded data structure
     */
    struct EncodedData {
        uint8_t *data;              /*!< Pointer to encoded data */
        size_t size;                /*!< Valid data size in bytes */
        uint32_t width;             /*!< Image width in pixels */
        uint32_t height;            /*!< Image height in pixels */
        Type type;                  /*!< Encoder type */
        struct timeval timestamp;   /*!< Encoding timestamp */
    };

    /**
     * @brief Construct a new VideoEncoder object
     */
    VideoEncoder();

    /**
     * @brief Destroy the VideoEncoder object
     *
     * @note Automatically calls deinit() if the device is still open
     */
    ~VideoEncoder();

    // Disable copy
    VideoEncoder(const VideoEncoder &) = delete;
    VideoEncoder &operator=(const VideoEncoder &) = delete;

    /**
     * @brief Initialize the video encoder device
     *
     * This function opens the encoder device, sets input/output formats,
     * and allocates buffers.
     *
     * @param config Configuration parameters
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_ARG: Invalid configuration
     *      - ESP_ERR_NOT_FOUND: Device not found
     *      - ESP_ERR_NO_MEM: Memory allocation failed
     *      - ESP_FAIL: Other failure
     */
    esp_err_t init(const Config &config);

    /**
     * @brief Deinitialize the video encoder device
     *
     * This function stops encoding, frees buffers, and closes the device.
     *
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not initialized
     */
    esp_err_t deinit();

    /**
     * @brief Start the encoder
     *
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not initialized or already started
     */
    esp_err_t start();

    /**
     * @brief Stop the encoder
     *
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Device not started
     */
    esp_err_t stop();

    /**
     * @brief Encode a frame
     *
     * This function encodes raw input data and returns the encoded output.
     * After processing the output, call releaseOutput() to return the buffer.
     *
     * @param input_data Pointer to raw input data
     * @param input_size Input data size in bytes
     * @param[out] output Encoded data structure to fill
     * @param timeout_ms Timeout in milliseconds (default: infinite)
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_TIMEOUT: Timeout occurred
     *      - ESP_ERR_INVALID_STATE: Encoder not started
     *      - ESP_ERR_INVALID_ARG: Invalid input
     */
    esp_err_t encode(const uint8_t *input_data, size_t input_size,
                     EncodedData &output, uint32_t timeout_ms = UINT32_MAX);

    /**
     * @brief Release an output buffer back to the encoder
     *
     * This function must be called after processing encoded data from encode().
     *
     * @param output Encoded data to release
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_ARG: Invalid output
     */
    esp_err_t release_output(const EncodedData &output);

    /**
     * @brief Set JPEG encoding quality
     *
     * @param quality Quality value (1-100, higher is better)
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Encoder is not JPEG type
     */
    esp_err_t set_quality(uint32_t quality);

    /**
     * @brief Set H.264 encoding parameters
     *
     * @param params H.264 parameters
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Encoder is not H.264 type
     *      - ESP_ERR_INVALID_ARG: Invalid parameters
     */
    esp_err_t set_h264_params(const H264Params &params);

    /**
     * @brief Get H.264 encoding parameters
     *
     * @param[out] params H.264 parameters
     * @return esp_err_t
     *      - ESP_OK: Success
     *      - ESP_ERR_INVALID_STATE: Encoder is not H.264 type
     */
    esp_err_t get_h264_params(H264Params &params);

    /**
     * @brief Set a control value
     *
     * @param ctrl_class Control class (e.g., V4L2_CTRL_CLASS_MPEG)
     * @param id Control ID
     * @param value Control value
     * @return esp_err_t
     *      - ESP_OK: Success
     */
    esp_err_t set_control(uint32_t ctrl_class, uint32_t id, int32_t value);

    /**
     * @brief Get a control value
     *
     * @param ctrl_class Control class
     * @param id Control ID
     * @param[out] value Control value
     * @return esp_err_t
     *      - ESP_OK: Success
     */
    esp_err_t get_control(uint32_t ctrl_class, uint32_t id, int32_t *value);

    /**
     * @brief Check if the device is opened
     *
     * @return true Device is opened
     * @return false Device is closed
     */
    bool is_opened() const;

    /**
     * @brief Get encoder type
     *
     * @return Type Encoder type
     */
    Type get_type() const;

    /**
     * @brief Get current width
     *
     * @return uint32_t Width in pixels
     */
    uint32_t get_width() const;

    /**
     * @brief Get current height
     *
     * @return uint32_t Height in pixels
     */
    uint32_t get_height() const;

    /**
     * @brief Get output buffer size
     *
     * @return uint32_t Buffer size in bytes
     */
    uint32_t get_output_buffer_size() const;

private:
    /**
     * @brief Get device path for encoder type
     *
     * @param type Encoder type
     * @return const char* Device path
     */
    static const char *get_device_path(Type type);

    /**
     * @brief Get output pixel format for encoder type
     *
     * @param type Encoder type
     * @return uint32_t Pixel format
     */
    static uint32_t get_output_format(Type type);

    V4L2Device device_;             /*!< V4L2 device */
    void *output_buffer_;           /*!< Output buffer (CAPTURE queue) */
    uint32_t output_buffer_size_;   /*!< Output buffer size */
    uint32_t width_;                /*!< Current width */
    uint32_t height_;               /*!< Current height */
    uint32_t input_format_;         /*!< Input pixel format */
    Type type_;                     /*!< Encoder type */
    bool started_;                  /*!< Encoder started flag */

    // H.264 parameters cache
    H264Params h264_params_;
};
