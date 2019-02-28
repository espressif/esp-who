#pragma once

#include "app_camera.h"
#include "app_led.h"
#include "app_console.h"
#include "app_acc_test.h"
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define VERSION "0.1.0"

#define GPIO_LED_RED    21
#define GPIO_LED_WHITE  22

/*typedef enum
{
    WAIT_FACE_NUM,
    WAIT_FACE_CAP,
    FACE_CAP,
    FACE_CAP_FINISH,
} en_fsm_state;

extern en_fsm_state g_state;*/