#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    TEST_SD_CARD = 0,
    TEST_LED,
    TEST_LCD,
    TEST_MIC,
    TEST_BUTTON,
    TEST_CAMMERA,
    TEST_IDLE,
} en_test_state;


typedef struct
{
    bool test_sd_card_pass;
    bool test_led_pass;
    bool test_lcd_pass;
    bool test_mic_pass;
    bool test_button_pass;
    bool test_cammera_pass;
} test_result_t;
 

extern test_result_t g_test_result;
extern en_test_state g_state_test;
extern bool auto_test;

void register_console();

#ifdef __cplusplus
}
#endif