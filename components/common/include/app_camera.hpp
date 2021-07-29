#pragma once

#include "esp_camera.h"
#include "app_define.h"

/**
 * @brief Initialize camera
 * 
 * @param frame_size   One of
 *                     - FRAMESIZE_96X96,    // 96x96
 *                     - FRAMESIZE_QQVGA,    // 160x120
 *                     - FRAMESIZE_QCIF,     // 176x144
 *                     - FRAMESIZE_HQVGA,    // 240x176
 *                     - FRAMESIZE_240X240,  // 240x240
 *                     - FRAMESIZE_QVGA,     // 320x240
 *                     - FRAMESIZE_CIF,      // 400x296
 *                     - FRAMESIZE_HVGA,     // 480x320
 *                     - FRAMESIZE_VGA,      // 640x480
 *                     - FRAMESIZE_SVGA,     // 800x600
 *                     - FRAMESIZE_XGA,      // 1024x768
 *                     - FRAMESIZE_HD,       // 1280x720
 *                     - FRAMESIZE_SXGA,     // 1280x1024
 *                     - FRAMESIZE_UXGA,     // 1600x1200
 *                     - FRAMESIZE_FHD,      // 1920x1080
 *                     - FRAMESIZE_P_HD,     //  720x1280
 *                     - FRAMESIZE_P_3MP,    //  864x1536
 *                     - FRAMESIZE_QXGA,     // 2048x1536
 *                     - FRAMESIZE_QHD,      // 2560x1440
 *                     - FRAMESIZE_WQXGA,    // 2560x1600
 *                     - FRAMESIZE_P_FHD,    // 1080x1920
 *                     - FRAMESIZE_QSXGA,    // 2560x1920
 * @param jpeg_quality Quality of JPEG output. 0-63 lower means higher quality
 * @param fb_count     Number of frame buffers to be allocated. If more than one, then each frame will be acquired (double speed)
 */
void app_camera_init(framesize_t frame_size, uint8_t jpeg_quality, uint8_t fb_count);

/**
 * @brief Decode fb into RGB565
 * 
 * @param fb 
 * @param image_ptr 
 * @return true 
 * @return false 
 */
bool app_camera_decode(camera_fb_t *fb, uint16_t **image_ptr);

/**
 * @brief Decode fb into RGB888
 * 
 * @param fb 
 * @param image_ptr 
 * @return true 
 * @return false 
 */
bool app_camera_decode(camera_fb_t *fb, uint8_t **image_ptr);
