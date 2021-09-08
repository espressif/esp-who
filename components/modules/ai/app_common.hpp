#pragma once

#include <list>
#include "dl_detect_define.hpp"
#include "esp_camera.h"
// #include "who_define.h"

#if CONFIG_CAMERA_PIXEL_FORMAT_RGB565
#define IMAGE_T uint16_t
#define COLOR_RED 0b0000000011111000
#define COLOR_GREEN 0b1110000000000111
#define COLOR_BLUE 0b0001111100000000
#define COLOR_BLACK 0b0000000000000000
#else
#define IMAGE_T uint8_t
#define COLOR_RED 0x0000FF
#define COLOR_GREEN 0x00FF00
#define COLOR_BLUE 0xFF0000
#define COLOR_BLACK 0x000000
#endif

static const char *TAG = "app_common";


/**
 * @brief Draw detection result on RGB565 image.
 * 
 * @param image_ptr     image
 * @param image_height  height of image
 * @param image_width   width of image
 * @param results       detection results
 */
void draw_detection_result(uint16_t *image_ptr, int image_height, int image_width, std::list<dl::detect::result_t> &results);

/**
 * @brief Draw detection result on RGB888 image.
 * 
 * @param image_ptr     image
 * @param image_height  height of image
 * @param image_width   width of image
 * @param results       detection results
 */

void draw_detection_result(uint8_t *image_ptr, int image_height, int image_width, std::list<dl::detect::result_t> &results);

/**
 * @brief Print detection result in terminal
 * 
 * @param results detection results
 */
void print_detection_result(std::list<dl::detect::result_t> &results);

/**
 * @brief Decode fb , 
 *        - if fb->format == PIXFORMAT_RGB565, then return fb->buf
 *        - else, then return a new memory with RGB888, don't forget to free it
 * 
 * @param fb 
 */
void *app_camera_decode(camera_fb_t *fb);
