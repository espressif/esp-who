#ifndef _APP_CAMERA_H_
#define _APP_CAMERA_H_
#include "esp_camera.h"

#define CAMERA_FRAME_SIZE FRAMESIZE_VGA

#define CAMERA_PIXEL_FORMAT PIXFORMAT_JPEG

// This is esp_eye pins.
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    4
#define SIOD_GPIO_NUM    18
#define SIOC_GPIO_NUM    23

#define Y9_GPIO_NUM      36
#define Y8_GPIO_NUM      37
#define Y7_GPIO_NUM      38
#define Y6_GPIO_NUM      39
#define Y5_GPIO_NUM      35
#define Y4_GPIO_NUM      14
#define Y3_GPIO_NUM      13
#define Y2_GPIO_NUM      34
#define VSYNC_GPIO_NUM   5
#define HREF_GPIO_NUM    27
#define PCLK_GPIO_NUM    25

#define XCLK_FREQ       20000000

#ifdef __cplusplus
extern "C" {
#endif

void app_camera_main();

#ifdef __cplusplus
}
#endif

#endif