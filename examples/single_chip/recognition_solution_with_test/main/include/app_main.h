#pragma once

#include "app_camera.h"
#include "app_httpserver.h"
#include "app_wifi.h"
#include "app_led_test.h"
#include "app_speech_srcif.h"
#include "app_facenet_test.h"
#include "linenoise/linenoise.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"

#define VERSION "1.0.0"

#define GPIO_LED_RED    21
#define GPIO_LED_WHITE  22
#define GPIO_BUTTON     15
#define GPIO_BOOT       0

typedef enum
{
    WAIT_FOR_WAKEUP,
    WAIT_FOR_CONNECT,
    START_DETECT,
    START_RECOGNITION,
    START_ENROLL,
    START_DELETE,

} en_fsm_state;

extern en_fsm_state g_state;
extern int g_is_enrolling;
extern int g_is_deleting;


typedef enum
{
    LED_TEST,
    MIC_TEST,
    WAIT_KEY_TEST,
    WAIT_CAMERA_TEST,
    CAMERA_TEST,
    TEST_PASS,
    TEST_FAIL
} en_fsm_state_test;

extern en_fsm_state_test g_state_test;
extern uint8_t led_pass;
extern uint8_t mic_pass;

