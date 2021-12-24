/*
  * ESPRESSIF MIT License
  *
  * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
  *
  * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
  * it is free of charge, to any person obtaining a copy of this software and associated
  * documentation files (the "Software"), to deal in the Software without restriction, including
  * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
  * to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all copies or
  * substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  */
#ifndef _APP_CAMERA_H_
#define _APP_CAMERA_H_

#if CONFIG_CAMERA_MODEL_WROVER_KIT
#define CAM_BOARD       "WROVER-KIT"
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27

#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

#elif CONFIG_CAMERA_MODEL_ESP32_CAM_BOARD
// The 18 pin header on the board has Y5 and Y3 swapped
#define USE_BOARD_HEADER 0

#define CAM_BOARD       "ESP-DEVCAM"
#define PWDN_GPIO_NUM    32
#define RESET_GPIO_NUM   33
#define XCLK_GPIO_NUM     4
#define SIOD_GPIO_NUM    18
#define SIOC_GPIO_NUM    23

#define Y9_GPIO_NUM      36
#define Y8_GPIO_NUM      19
#define Y7_GPIO_NUM      21
#define Y6_GPIO_NUM      39
#if USE_BOARD_HEADER
#define Y5_GPIO_NUM      13
#else
#define Y5_GPIO_NUM      35
#endif
#define Y4_GPIO_NUM      14
#if USE_BOARD_HEADER
#define Y3_GPIO_NUM      35
#else
#define Y3_GPIO_NUM      13
#endif
#define Y2_GPIO_NUM      34
#define VSYNC_GPIO_NUM    5
#define HREF_GPIO_NUM    27
#define PCLK_GPIO_NUM    25

#elif CONFIG_CAMERA_MODEL_ESP_EYE
#define CAM_BOARD        "ESP-EYE"
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

#elif CONFIG_CAMERA_MODEL_M5STACK_PSRAM
#define CAM_BOARD         "M5CAM"
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     25
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       32
#define VSYNC_GPIO_NUM    22
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21

#elif CONFIG_CAMERA_MODEL_M5STACK_WIDE
#define CAM_BOARD         "M5CAMW"
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     22
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       32
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21

#elif CONFIG_CAMERA_MODEL_AI_THINKER
#define CAM_BOARD         "AI-THINKER"
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#elif CONFIG_CAMERA_MODULE_S3_KS_DIY
#define CAMERA_MODULE_NAME "S3_KS_DIY"
#define CAMERA_PIN_PWDN -4
#define CAMERA_PIN_RESET -1
#define CAMERA_PIN_XCLK -1
#define CAMERA_PIN_SIOD 17
#define CAMERA_PIN_SIOC 18

#define CAMERA_PIN_D7 39
#define CAMERA_PIN_D6 41
#define CAMERA_PIN_D5 42
#define CAMERA_PIN_D4 5
#define CAMERA_PIN_D3 40
#define CAMERA_PIN_D2 14
#define CAMERA_PIN_D1 47
#define CAMERA_PIN_D0 45
#define CAMERA_PIN_VSYNC 21
#define CAMERA_PIN_HREF 38
#define CAMERA_PIN_PCLK 48

#elif CONFIG_CAMERA_MODEL_CUSTOM
#define CAM_BOARD         "CUSTOM"
#define PWDN_GPIO_NUM     CONFIG_CAMERA_PIN_PWDN
#define RESET_GPIO_NUM    CONFIG_CAMERA_PIN_RESET
#define XCLK_GPIO_NUM     CONFIG_CAMERA_PIN_XCLK
#define SIOD_GPIO_NUM     CONFIG_CAMERA_PIN_SIOD
#define SIOC_GPIO_NUM     CONFIG_CAMERA_PIN_SIOC

#define Y9_GPIO_NUM       CONFIG_CAMERA_PIN_Y9
#define Y8_GPIO_NUM       CONFIG_CAMERA_PIN_Y8
#define Y7_GPIO_NUM       CONFIG_CAMERA_PIN_Y7
#define Y6_GPIO_NUM       CONFIG_CAMERA_PIN_Y6
#define Y5_GPIO_NUM       CONFIG_CAMERA_PIN_Y5
#define Y4_GPIO_NUM       CONFIG_CAMERA_PIN_Y4
#define Y3_GPIO_NUM       CONFIG_CAMERA_PIN_Y3
#define Y2_GPIO_NUM       CONFIG_CAMERA_PIN_Y2
#define VSYNC_GPIO_NUM    CONFIG_CAMERA_PIN_VSYNC
#define HREF_GPIO_NUM     CONFIG_CAMERA_PIN_HREF
#define PCLK_GPIO_NUM     CONFIG_CAMERA_PIN_PCLK
#endif

#ifdef __cplusplus
extern "C" {
#endif

void app_camera_main();

#ifdef __cplusplus
}
#endif

#endif
