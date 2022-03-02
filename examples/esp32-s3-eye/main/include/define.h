#pragma once

#include "esp_log.h"

#define FORMAT_MENU(message) "\n" LOG_COLOR(LOG_COLOR_GREEN) "---------- %s ----------\n" LOG_RESET_COLOR, message

#define FORMAT_CASE(message)   \
    LOG_COLOR(LOG_COLOR_GREEN) \
    "%s:\n" LOG_RESET_COLOR, message

#define FORMAT_PASS           \
    LOG_BOLD(LOG_COLOR_GREEN) \
    "pass" LOG_RESET_COLOR "\n"

#define FORMAT_FAIL          \
    LOG_COLOR(LOG_COLOR_RED) \
    "FAIL" LOG_RESET_COLOR "\n"


