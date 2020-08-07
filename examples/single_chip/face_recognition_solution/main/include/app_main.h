#include "app_camera.h"
#include "app_httpserver.h"
#include "app_wifi.h"
#include "app_speech_srcif.h"

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
