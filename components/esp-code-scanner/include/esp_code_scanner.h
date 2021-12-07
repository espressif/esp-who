#pragma once

#include "esp_err.h"



struct esp_image_scanner_s;
/** opaque image scanner object. */
typedef struct esp_image_scanner_s esp_image_scanner_t;



typedef enum{
    ESP_CODE_SCANNER_MODE_FAST = 0,
    /* more mode */
}esp_code_scanner_mode_t;

typedef enum{
    ESP_CODE_SCANNER_IMAGE_GRAY = 0,
    ESP_CODE_SCANNER_IMAGE_RGB565,
    ESP_CODE_SCANNER_IMAGE_YUV422,
    /* more image format*/
}esp_code_scanner_image_format_t;

typedef struct
{
    esp_code_scanner_mode_t mode;
    esp_code_scanner_image_format_t fmt;
    uint32_t width;  /*!< iamge width */
    uint32_t height; /*!< iamge height */
    /* more config */
} esp_code_scanner_config_t;

typedef enum{
    ESP_CODE_SCANNER_SYMBOL_CODE39,
    ESP_CODE_SCANNER_SYMBOL_CODE128,
    ESP_CODE_SCANNER_SYMBOL_QR
}esp_code_scanner_symbol_type_t;


typedef struct esp_code_scanner_symbol_t esp_code_scanner_symbol_t;
struct esp_code_scanner_symbol_t
{
    const char *type_name;
    const char *data;
    uint32_t datalen;
    esp_code_scanner_symbol_t* next;
};


/**
 * @brief      Start a code_scanner
 *             This function must be the first function to call,
 *             and it returns a esp_image_scanner_t pointer that you must use as input to other functions in the interface.
 *             This call MUST have a corresponding call to esp_code_scanner_cleanup when the operation is complete.
 *
 * @return
 *     - `esp_image_scanner_t`
 *     - NULL if any errors
 */
esp_image_scanner_t* esp_code_scanner_create();

/**
 * @brief      Set config of code_scanner
 *
 * @param[in]  config   The configurations, see `esp_code_scanner_config_t`
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL if any errors
 */
esp_err_t esp_code_scanner_set_config(esp_image_scanner_t* scanner, const esp_code_scanner_config_t config);

/**
 * @brief      Scan image data
 *
 * @param[in]  scanner  The esp_image_scanner_t
 * @param[in]  image_data     The image data, only supports grayscale images
 *
 * @return
 *  	- number of decoded symbol 
 */
int esp_code_scanner_scan_image(esp_image_scanner_t* scanner, const uint8_t *image_data);

/**
 * @brief      Release resources: including the instance and configuration
 *
 * @param[in]  scanner  The esp_code_scanner_handle_t
 *
 */
void esp_code_scanner_destroy(esp_image_scanner_t* scanner);

/**
 * @brief      Get scan results
 *
 * @param[in]  scanner  The esp_code_scanner_handle_t
 *
 * @return
 *  	- `const esp_code_scanner_symbol_t`
 *  	- Returns NULL if no data is recognized
 */
const esp_code_scanner_symbol_t esp_code_scanner_result(esp_image_scanner_t* scanner);