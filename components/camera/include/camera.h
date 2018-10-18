// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "esp_err.h"
#include "driver/ledc.h"
#include "sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define DETECT_BAD_FRAME

typedef enum {
    CAMERA_PF_RGB565 = 0,       //!< RGB, 2 bytes per pixel
    CAMERA_PF_YUV422 = 1,       //!< YUYV, 2 bytes per pixel
    CAMERA_PF_GRAYSCALE = 2,    //!< 1 byte per pixel
    CAMERA_PF_JPEG = 3,         //!< JPEG compressed
    CAMERA_PF_RGB888 = 4,       //!< RGB, 3 bytes per pixel
} camera_pixelformat_t;

typedef enum {
    CAMERA_FS_QQVGA = 4,     //!< 160x120
    CAMERA_FS_QCIF  = 6,     //!< 176x144
    CAMERA_FS_HQVGA = 7,     //!< 240x160
    CAMERA_FS_QVGA = 8,      //!< 320x240
    CAMERA_FS_CIF  = 9,      //!< 400x296

    CAMERA_FS_VGA  = 10,     //!< 640x480
    CAMERA_FS_SVGA = 11,     //!< 800x600

    CAMERA_FS_XGA  = 12,     //!< 1024x768
    CAMERA_FS_SXGA = 13,     //!< 1280x1024
    CAMERA_FS_UXGA = 14,     //!< 1600x1200
} camera_framesize_t;

typedef enum {
    CAMERA_NONE = 0,
    CAMERA_UNKNOWN = 1,
    CAMERA_OV7725 = 7725,
    CAMERA_OV2640 = 2640,
} camera_model_t;

typedef struct {
    int pin_reset;          /*!< GPIO pin for camera reset line */
    int pin_xclk;           /*!< GPIO pin for camera XCLK line */
    int pin_sscb_sda;       /*!< GPIO pin for camera SDA line */
    int pin_sscb_scl;       /*!< GPIO pin for camera SCL line */
    int pin_d7;             /*!< GPIO pin for camera D7 line */
    int pin_d6;             /*!< GPIO pin for camera D6 line */
    int pin_d5;             /*!< GPIO pin for camera D5 line */
    int pin_d4;             /*!< GPIO pin for camera D4 line */
    int pin_d3;             /*!< GPIO pin for camera D3 line */
    int pin_d2;             /*!< GPIO pin for camera D2 line */
    int pin_d1;             /*!< GPIO pin for camera D1 line */
    int pin_d0;             /*!< GPIO pin for camera D0 line */
    int pin_vsync;          /*!< GPIO pin for camera VSYNC line */
    int pin_href;           /*!< GPIO pin for camera HREF line */
    int pin_pclk;           /*!< GPIO pin for camera PCLK line */

    int xclk_freq_hz;       /*!< Frequency of XCLK signal, in Hz. Either 10KHz or 20KHz */

    ledc_timer_t ledc_timer;        /*!< LEDC timer to be used for generating XCLK  */
    ledc_channel_t ledc_channel;    /*!< LEDC channel to be used for generating XCLK  */

    camera_pixelformat_t pixel_format;  /*!< Format of the pixel data: CAMERA_PF_ + YUV422|GRAYSCALE|RGB565|RGB888|JPEG  */
    camera_framesize_t frame_size;      /*!< Size of the output image: CAMERA_FS_ + QVGA|VGA|SVGA|SXGA|UXGA  */

    int jpeg_quality;   /*!< Quality of JPEG output. 0-255 lower means higher quality  */
    size_t fb_count;    /*!< Number of frame buffers to be allocated. If more than one, then each frame will be acquired (double speed)  */
} camera_config_t;

typedef struct {
        uint8_t * buf;
        size_t len;
        size_t width;
        size_t height;
} camera_fb_t;

#define ESP_ERR_CAMERA_BASE 0x20000
#define ESP_ERR_CAMERA_NOT_DETECTED             (ESP_ERR_CAMERA_BASE + 1)
#define ESP_ERR_CAMERA_FAILED_TO_SET_FRAME_SIZE (ESP_ERR_CAMERA_BASE + 2)
#define ESP_ERR_CAMERA_NOT_SUPPORTED            (ESP_ERR_CAMERA_BASE + 3)

/**
 * @brief Probe the camera
 * This function enables LEDC peripheral to generate XCLK signal,
 * detects the camera I2C address and detects camera model.
 *
 * @param config camera configuration parameters
 * @param[out] out_camera_model output, detected camera model
 * @return ESP_OK if camera was detected
 */
esp_err_t camera_probe(const camera_config_t* config, camera_model_t* out_camera_model);

/**
 * @brief Initialize the camera driver
 *
 * @note call camera_probe before calling this function
 *
 * This function configures camera over I2C interface,
 * allocates framebuffer and DMA buffers,
 * initializes parallel I2S input, and sets up DMA descriptors.
 *
 * Currently this function can only be called once and there is
 * no way to de-initialize this module.
 *
 * @param config  Camera configuration parameters
 * @return ESP_OK on success
 */
esp_err_t camera_init(const camera_config_t* config);

/**
 * Deinitialize the camera driver
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if the driver hasn't been initialized yet
 */
esp_err_t camera_deinit();

/**
 * @brief Obtain pointer to a frame buffer.
 *
 * @return pointer to the frame buffer
 */
camera_fb_t* camera_get_fb();

/**
 * @brief Return the frame buffer to be reused again.
 *
 * @param fb    Pointer to the frame buffer
 */
void camera_return_fb(camera_fb_t * fb);

/**
 * @brief Get the current width of the frame, in pixels.
 *
 * @return width of frame, in pixels
 */
int camera_get_width();

/**
 * @brief Get the current height of the frame, in pixels.
 *
 * @return height of frame, in pixels
 */
int camera_get_height();

/**
 * @brief Get a pointer to the image sensor.
 * @return pointer to the sensor
 */
sensor_t * camera_sensor_get();

/*
 * Example Use
 *
    static camera_config_t camera_example_config = {
        .pin_reset      = PIN_RESET,
        .pin_xclk       = PIN_XCLK,
        .pin_sscb_sda   = PIN_SIOD,
        .pin_sscb_scl   = PIN_SIOC,
        .pin_d7         = PIN_D7,
        .pin_d6         = PIN_D6,
        .pin_d5         = PIN_D5,
        .pin_d4         = PIN_D4,
        .pin_d3         = PIN_D3,
        .pin_d2         = PIN_D2,
        .pin_d1         = PIN_D1,
        .pin_d0         = PIN_D0,
        .pin_vsync      = PIN_VSYNC,
        .pin_href       = PIN_HREF,
        .pin_pclk       = PIN_PCLK,

        .xclk_freq_hz   = 20000000,
        .ledc_timer     = LEDC_TIMER_0,
        .ledc_channel   = LEDC_CHANNEL_0,
        .pixel_format   = CAMERA_PF_RGB565,//YUV422,GRAYSCALE,RGB565,JPEG
        .frame_size     = CAMERA_FS_QVGA,
        .jpeg_quality   = 12,
        .fb_count       = 2
    };

    esp_err_t camera_example_init(){
        camera_model_t camera_model = CAMERA_NONE;
        esp_err_t err = camera_probe(&camera_example_config, &camera_model);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Camera probe failed with error 0x%x", err);
            return err;
        }
        if (camera_model == CAMERA_OV7725) {
            ESP_LOGD(TAG, "Detected OV7725 camera");
        } else if (camera_model == CAMERA_OV2640) {
            ESP_LOGD(TAG, "Detected OV2640 camera");
        } else {
            ESP_LOGE(TAG, "Camera not supported");
            return ESP_FAIL;
        }
        err = camera_init(&camera_example_config);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
            return err;
        }
        return ESP_OK;
    }

    esp_err_t camera_example_capture(){
        //capture a frame
        camera_fb_t * fb = camera_get_fb();
        if (!fb) {
            ESP_LOGE(TAG, "Frame buffer could not be acquired");
            return ESP_FAIL;
        }

        //replace this with your own function
        display_image(fb->width, fb->height, fb->buf, fb->len);

        //return the frame buffer back to be reused
        camera_return_fb(fb);

        return ESP_OK;
    }
*/


#ifdef __cplusplus
}
#endif
