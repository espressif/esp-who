/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief ESP BSP: S3-EYE
 */

#pragma once

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "esp_codec_dev.h"
#include "iot_button.h"
#include "bsp/config.h"
#include "bsp/display.h"
#include "driver/i2s_std.h"
#include "driver/i2c_master.h"
#include "esp_adc/adc_oneshot.h"

#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
#include "lvgl.h"
#include "esp_lvgl_port.h"
#endif // BSP_CONFIG_NO_GRAPHIC_LIB == 0

/**************************************************************************************************
 *  BSP Capabilities
 **************************************************************************************************/

#define BSP_CAPS_DISPLAY        1
#define BSP_CAPS_TOUCH          0
#define BSP_CAPS_BUTTONS        1
#define BSP_CAPS_AUDIO          1
#define BSP_CAPS_AUDIO_SPEAKER  0
#define BSP_CAPS_AUDIO_MIC      1
#define BSP_CAPS_SDCARD         1
#define BSP_CAPS_IMU            1
#define BSP_CAPS_CAMERA         1

/**************************************************************************************************
 * ESP32-S3-EYE pinout
 **************************************************************************************************/

/* I2C */
#define BSP_I2C_SCL           (GPIO_NUM_5)
#define BSP_I2C_SDA           (GPIO_NUM_4)

/* Audio */
#define BSP_I2S_SCLK          (GPIO_NUM_41)
#define BSP_I2S_LCLK          (GPIO_NUM_42)
#define BSP_I2S_DIN           (GPIO_NUM_2)

/* Display */
#define BSP_LCD_SPI_MOSI      (GPIO_NUM_47)
#define BSP_LCD_SPI_CLK       (GPIO_NUM_21)
#define BSP_LCD_SPI_CS        (GPIO_NUM_44)
#define BSP_LCD_DC            (GPIO_NUM_43)
#define BSP_LCD_RST           (GPIO_NUM_NC)
#define BSP_LCD_BACKLIGHT     (GPIO_NUM_48)

/* Camera */
#define BSP_CAMERA_XCLK      (GPIO_NUM_15)
#define BSP_CAMERA_PCLK      (GPIO_NUM_13)
#define BSP_CAMERA_VSYNC     (GPIO_NUM_6)
#define BSP_CAMERA_HSYNC     (GPIO_NUM_7)
#define BSP_CAMERA_D0        (GPIO_NUM_11)
#define BSP_CAMERA_D1        (GPIO_NUM_9)
#define BSP_CAMERA_D2        (GPIO_NUM_8)
#define BSP_CAMERA_D3        (GPIO_NUM_10)
#define BSP_CAMERA_D4        (GPIO_NUM_12)
#define BSP_CAMERA_D5        (GPIO_NUM_18)
#define BSP_CAMERA_D6        (GPIO_NUM_17)
#define BSP_CAMERA_D7        (GPIO_NUM_16)

/* uSD card MMC */
#define BSP_SD_D0            (GPIO_NUM_40)
#define BSP_SD_CMD           (GPIO_NUM_38)
#define BSP_SD_CLK           (GPIO_NUM_39)

/* Buttons */
#define BSP_BUTTON_BOOT_IO   (GPIO_NUM_0)
#define BSP_BUTTONS_IO       (GPIO_NUM_1) // All 4 buttons mapped to this GPIO
typedef enum bsp_led_t {
    BSP_LED_GREEN = GPIO_NUM_3,
} bsp_led_t;

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *
 * Buttons interface
 *
 * Example configuration:
 * \code{.c}
 * button_handle_t btns[BSP_BUTTON_NUM];
 * bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM);
 * iot_button_register_cb(btns[0], ...
 * \endcode
 **************************************************************************************************/
typedef enum {
    BSP_BUTTON_MENU = 0,
    BSP_BUTTON_PLAY,
    // BSP_BUTTON_DOWN,
    // BSP_BUTTON_UP,
    BSP_BUTTON_BOOT,
    BSP_BUTTON_NUM
} bsp_button_t;

/**
 * @brief Initialize all buttons
 *
 * Returned button handlers must be used with espressif/button component API
 *
 * @note For LCD panel button which is defined as BSP_BUTTON_MAIN, bsp_display_start should
 *       be called before call this function.
 *
 * @param[out] btn_array      Output button array
 * @param[out] btn_cnt        Number of button handlers saved to btn_array, can be NULL
 * @param[in]  btn_array_size Size of output button array. Must be at least BSP_BUTTON_NUM
 * @return
 *     - ESP_OK               All buttons initialized
 *     - ESP_ERR_INVALID_ARG  btn_array is too small or NULL
 *     - ESP_FAIL             Underlaying iot_button_create failed
 */
esp_err_t bsp_iot_button_create(button_handle_t btn_array[], int *btn_cnt, int btn_array_size);

/**************************************************************************************************
 *
 * I2C interface
 *
 * There are two devices connected to I2C peripheral:
 *  - QMA7981 or QMA6100P Inertial measurement unit
 *  - OV2640 Camera module
 **************************************************************************************************/
#define BSP_I2C_NUM     CONFIG_BSP_I2C_NUM

/**
 * @brief Init I2C driver
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   I2C parameter error
 *      - ESP_FAIL              I2C driver installation error
 *
 */
esp_err_t bsp_i2c_init(void);

/**
 * @brief Deinit I2C driver and free its resources
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   I2C parameter error
 *
 */
esp_err_t bsp_i2c_deinit(void);

/**
 * @brief Get I2C driver handle
 *
 * @return
 *      - I2C handle
 */
i2c_master_bus_handle_t bsp_i2c_get_handle(void);

/**************************************************************************************************
 *
 * Camera interface
 *
 * ESP32-S3-EYE is shipped with OV2640 camera module.
 * As a camera driver, esp32-camera component is used.
 *
 * Example configuration:
 * \code{.c}
 * const camera_config_t camera_config = BSP_CAMERA_DEFAULT_CONFIG;
 * esp_err_t err = esp_camera_init(&camera_config);
 * \endcode
 **************************************************************************************************/
/**
 * @brief ESP32-S3-EYE camera default configuration
 *
 * In this configuration we select RGB565 color format and 240x240 image size - matching the display.
 * We use double-buffering for the best performance.
 * Since we don't want to waste internal SRAM, we allocate the framebuffers in external PSRAM.
 * By setting XCLK to 16MHz, we configure the esp32-camera driver to use EDMA when accessing the PSRAM.
 *
 * @attention I2C must be enabled by bsp_i2c_init(), before camera is initialized
 */
#define BSP_CAMERA_DEFAULT_CONFIG         \
    {                                     \
        .pin_pwdn = GPIO_NUM_NC,          \
        .pin_reset = GPIO_NUM_NC,         \
        .pin_xclk = BSP_CAMERA_XCLK,      \
        .pin_sccb_sda = GPIO_NUM_NC,      \
        .pin_sccb_scl = GPIO_NUM_NC,      \
        .pin_d7 = BSP_CAMERA_D7,          \
        .pin_d6 = BSP_CAMERA_D6,          \
        .pin_d5 = BSP_CAMERA_D5,          \
        .pin_d4 = BSP_CAMERA_D4,          \
        .pin_d3 = BSP_CAMERA_D3,          \
        .pin_d2 = BSP_CAMERA_D2,          \
        .pin_d1 = BSP_CAMERA_D1,          \
        .pin_d0 = BSP_CAMERA_D0,          \
        .pin_vsync = BSP_CAMERA_VSYNC,    \
        .pin_href = BSP_CAMERA_HSYNC,     \
        .pin_pclk = BSP_CAMERA_PCLK,      \
        .xclk_freq_hz = 16000000,         \
        .ledc_timer = LEDC_TIMER_0,       \
        .ledc_channel = LEDC_CHANNEL_0,   \
        .pixel_format = PIXFORMAT_RGB565, \
        .frame_size = FRAMESIZE_240X240,  \
        .jpeg_quality = 12,               \
        .fb_count = 2,                    \
        .fb_location = CAMERA_FB_IN_PSRAM,\
        .sccb_i2c_port = BSP_I2C_NUM,     \
    }

#define BSP_CAMERA_VFLIP        1
#define BSP_CAMERA_HMIRROR      0

/**************************************************************************************************
 *
 * SPIFFS
 *
 * After mounting the SPIFFS, it can be accessed with stdio functions ie.:
 * \code{.c}
 * FILE* f = fopen(BSP_SPIFFS_MOUNT_POINT"/hello.txt", "w");
 * fprintf(f, "Hello World!\n");
 * fclose(f);
 * \endcode
 **************************************************************************************************/
#define BSP_SPIFFS_MOUNT_POINT      CONFIG_BSP_SPIFFS_MOUNT_POINT

/**
 * @brief Mount SPIFFS to virtual file system
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_spiffs_register was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes
 */
esp_err_t bsp_spiffs_mount(void);

/**
 * @brief Unmount SPIFFS from virtual file system
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if already unmounted
 */
esp_err_t bsp_spiffs_unmount(void);

/**************************************************************************************************
 *
 * uSD card
 *
 * After mounting the uSD card, it can be accessed with stdio functions ie.:
 * \code{.c}
 * FILE* f = fopen(BSP_MOUNT_POINT"/hello.txt", "w");
 * fprintf(f, "Hello %s!\n", bsp_sdcard->cid.name);
 * fclose(f);
 * \endcode
 **************************************************************************************************/
#define BSP_SD_MOUNT_POINT      CONFIG_BSP_SD_MOUNT_POINT
#define BSP_SDSPI_HOST          (SPI3_HOST)

typedef struct {
    const esp_vfs_fat_sdmmc_mount_config_t *mount;
    sdmmc_host_t *host;
    union {
        const sdmmc_slot_config_t   *sdmmc;
        const sdspi_device_config_t *sdspi;
    } slot;
} bsp_sdcard_cfg_t;

/**
 * @brief Mount microSD card to virtual file system
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
 *      - ESP_ERR_NO_MEM if memory cannot be allocated
 *      - ESP_FAIL if partition cannot be mounted
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t bsp_sdcard_mount(void);

/**
 * @brief Unmount microSD card from virtual file system
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_FOUND if the partition table does not contain FATFS partition with given label
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_spiflash_mount was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from wear levelling library, SPI flash driver, or FATFS drivers
 */
esp_err_t bsp_sdcard_unmount(void);

/**
 * @brief Get SD card handle
 *
 * @return SD card handle
 */
sdmmc_card_t *bsp_sdcard_get_handle(void);

/**
 * @brief Get SD card MMC host config
 *
 * @param slot SD card slot
 * @param config Structure which will be filled
 */
void bsp_sdcard_get_sdmmc_host(const int slot, sdmmc_host_t *config);

/**
 * @brief Get SD card SPI host config
 *
 * @param slot SD card slot
 * @param config Structure which will be filled
 */
void bsp_sdcard_get_sdspi_host(const int slot, sdmmc_host_t *config);

/**
 * @brief Get SD card MMC slot config
 *
 * @param slot SD card slot
 * @param config Structure which will be filled
 */
void bsp_sdcard_sdmmc_get_slot(const int slot, sdmmc_slot_config_t *config);

/**
 * @brief Get SD card SPI slot config
 *
 * @param spi_host SPI host ID
 * @param config Structure which will be filled
 */
void bsp_sdcard_sdspi_get_slot(const spi_host_device_t spi_host, sdspi_device_config_t *config);

/**
 * @brief Mount microSD card to virtual file system (MMC mode)
 *
 * @param cfg SD card configuration
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
 *      - ESP_ERR_NO_MEM if memory cannot be allocated
 *      - ESP_FAIL if partition cannot be mounted
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t bsp_sdcard_sdmmc_mount(bsp_sdcard_cfg_t *cfg);

/**
 * @brief Mount microSD card to virtual file system (SPI mode)
 *
 * @param cfg SD card configuration
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
 *      - ESP_ERR_NO_MEM if memory cannot be allocated
 *      - ESP_FAIL if partition cannot be mounted
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t bsp_sdcard_sdspi_mount(bsp_sdcard_cfg_t *cfg);

/**************************************************************************************************
 *
 * LCD interface
 *
 * ESP32-S3-EYE is shipped with 1.3inch ST7789 display controller.
 * It features 16-bit colors and 240x240 resolution.
 *
 * LVGL is used as graphics library. LVGL is NOT thread safe, therefore the user must take LVGL mutex
 * by calling bsp_display_lock() before calling any LVGL API (lv_...) and then give the mutex with
 * bsp_display_unlock().
 *
 * If you want to use the display without LVGL, see bsp/display.h API and use BSP version with 'noglib' suffix.
 **************************************************************************************************/
#define BSP_LCD_PIXEL_CLOCK_HZ     (80 * 1000 * 1000)
#define BSP_LCD_SPI_NUM            (SPI3_HOST)

#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
#define BSP_LCD_DRAW_BUFF_SIZE     (BSP_LCD_H_RES * BSP_LCD_V_RES)
#define BSP_LCD_DRAW_BUFF_DOUBLE   (0)

/**
 * @brief BSP display configuration structure
 */
typedef struct {
    lvgl_port_cfg_t lvgl_port_cfg;  /*!< LVGL port configuration */
    uint32_t        buffer_size;    /*!< Size of the buffer for the screen in pixels */
    bool            double_buffer;  /*!< True, if should be allocated two buffers */
    struct {
        unsigned int buff_dma: 1;    /*!< Allocated LVGL buffer will be DMA capable */
        unsigned int buff_spiram: 1; /*!< Allocated LVGL buffer will be in PSRAM */
    } flags;
} bsp_display_cfg_t;

/**
 * @brief Initialize display
 *
 * This function initializes SPI, display controller and starts LVGL handling task.
 *
 * @return Pointer to LVGL display or NULL when error occurred
 */
lv_display_t *bsp_display_start(void);

/**
 * @brief Initialize display
 *
 * This function initializes SPI, display controller and starts LVGL handling task.
 * LCD backlight must be enabled separately by calling bsp_display_brightness_set()
 *
 * @param cfg display configuration
 *
 * @return Pointer to LVGL display or NULL when error occurred
 */
lv_display_t *bsp_display_start_with_config(const bsp_display_cfg_t *cfg);

/**
 * @brief Get pointer to input device (touch, buttons, ...)
 *
 * @note The LVGL input device is initialized in bsp_display_start() function.
 *
 * @return Pointer to LVGL input device or NULL when not initialized
 */
lv_indev_t *bsp_display_get_input_dev(void);

/**
 * @brief Take LVGL mutex
 *
 * @param timeout_ms Timeout in [ms]. 0 will block indefinitely.
 * @return true  Mutex was taken
 * @return false Mutex was NOT taken
 */
bool bsp_display_lock(uint32_t timeout_ms);

/**
 * @brief Give LVGL mutex
 *
 */
void bsp_display_unlock(void);

/**
 * @brief Rotate screen
 *
 * Display must be already initialized by calling bsp_display_start()
 *
 * @param[in] disp Pointer to LVGL display
 * @param[in] rotation Angle of the display rotation
 */
void bsp_display_rotate(lv_display_t *disp, lv_disp_rotation_t rotation);
#endif // BSP_CONFIG_NO_GRAPHIC_LIB == 0

/**************************************************************************************************
 *
 * LEDs
 *
 **************************************************************************************************/

/**
 * @brief Set LED's GPIOs as output push-pull
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t bsp_leds_init(void);

/**
 * @brief Turn LED on/off
 *
 * @param led_io LED io
 * @param on Switch LED on/off
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t bsp_led_set(const bsp_led_t led_io, const bool on);

/**************************************************************************************************
 *
 * I2S audio interface
 *
 * There is only one device connected to the I2S peripheral
 *  - MEMSensing Microsystems MSM261S4030H0: 48kHz, 24bit mono digital microphone
 *
 * For microphone initialization use bsp_audio_codec_microphone_init() which is inside initialize I2S with bsp_audio_init().
 * After microphone initialization, use functions from esp_codec_dev for record audio.
 * Example audio play:
 * \code{.c}
 * esp_codec_dev_set_out_vol(spk_codec_dev, DEFAULT_VOLUME);
 * esp_codec_dev_open(spk_codec_dev, &fs);
 * esp_codec_dev_write(spk_codec_dev, wav_bytes, bytes_read_from_spiffs);
 * esp_codec_dev_close(spk_codec_dev);
 * \endcode
 **************************************************************************************************/

/**
 * @brief Init audio
 *
 * @note There is no deinit audio function. Users can free audio resources by calling i2s_del_channel()
 * @warning The type of i2s_config param is depending on IDF version.
 * @param[in]  i2s_config I2S configuration. Pass NULL to use default values (Mono, duplex, 16bit, 22050 Hz)
 * @param[out] tx_channel I2S TX channel
 * @param[out] rx_channel I2S RX channel
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_NOT_SUPPORTED The communication mode is not supported on the current chip
 *      - ESP_ERR_INVALID_ARG   NULL pointer or invalid configuration
 *      - ESP_ERR_NOT_FOUND     No available I2S channel found
 *      - ESP_ERR_NO_MEM        No memory for storing the channel information
 *      - ESP_ERR_INVALID_STATE This channel has not initialized or already started
 */
esp_err_t bsp_audio_init(const i2s_std_config_t *i2s_config);

/**
 * @brief Get codec I2S interface (initialized in bsp_audio_init)
 *
 * @return
 *      - Pointer to codec I2S interface handle or NULL when error occurred
 */
const audio_codec_data_if_t *bsp_audio_get_codec_itf(void);

/**
 * @brief Initialize microphone codec device
 *
 * @return Pointer to codec device handle or NULL when error occurred
 */
esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void);

/**************************************************************************************************
 *
 * ADC interface
 *
 * There are multiple devices connected to ADC peripheral:
 *  - Buttons
 *
 * After initialization of ADC, use adc_handle when using ADC driver.
 **************************************************************************************************/

#define BSP_ADC_UNIT     ADC_UNIT_1

/**
 * @brief Initialize ADC
 *
 * The ADC can be initialized inside BSP, when needed.
 *
 * @param[out] adc_handle Returned ADC handle
 */
esp_err_t bsp_adc_initialize(void);

/**
 * @brief Get ADC handle
 *
 * @return ADC handle
 */
adc_oneshot_unit_handle_t bsp_adc_get_handle(void);

#ifdef __cplusplus
}
#endif
