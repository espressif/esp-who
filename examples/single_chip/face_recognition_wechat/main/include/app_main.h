#include "app_camera.h"
#include "app_httpserver.h"
#include "app_wifi.h"

#define VERSION "0.9.2"

#define GPIO_LED_RED    21
#define GPIO_LED_WHITE  22
#define GPIO_BUTTON     15

typedef enum
{
    WAIT_FOR_WAKEUP,
    WAIT_FOR_CONNECT,
    START_STREAM,
    START_DETECT,
    START_RECOGNITION,
    START_ENROLL,
} en_fsm_state;

extern en_fsm_state g_state;
