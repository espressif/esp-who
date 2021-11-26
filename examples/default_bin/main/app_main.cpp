#include "test_logic.h"
#include "who_speech_recognition.h"
#include "who_camera.h"
#include "event_logic.hpp"
#include "who_ai_image_task.hpp"
#include "who_lcd.h"
#include "who_led.h"
#include "who_adc_button.h"
#include "who_button.h"
#include "qma7981.h"
#include <math.h>


static const char *TAG = "ESP32-S3-EYE-V2.1"; 

static QueueHandle_t xQueueTests[TEST_NUM] = {NULL};
static QueueHandle_t xQueueTestFlag = NULL;
static QueueHandle_t xQueueTestResult = NULL;
static QueueHandle_t xQueueKeyResult = NULL;
static TaskHandle_t consoleHandle_t = NULL;

static QueueHandle_t xQueueSRResult = NULL;
static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueLCDFrame = NULL;
static QueueHandle_t xQueueKeyState = NULL;
static QueueHandle_t xQueueBootKey = NULL;
static QueueHandle_t xQueueLEDControl = NULL;
static QueueHandle_t xQueueEventLogic = NULL;
static button_adc_config_t buttons[4] = {{1, 2800, 3000}, {2, 2250, 2450}, {3, 300, 500}, {4, 850, 1050}};

#define GPIO_BOOT GPIO_NUM_0

extern "C" void app_main()
{
    for (int i = 0; i < TEST_NUM; ++i)
    {
        xQueueTests[i] = xQueueCreate(1, sizeof(int));
    }
    xQueueTestFlag = xQueueCreate(1, sizeof(bool));
    xQueueTestResult = xQueueCreate(1, sizeof(bool));
    xQueueKeyResult = xQueueCreate(1, sizeof(int));
    xQueueBootKey = xQueueCreate(1, sizeof(int));
    bool testmode_flag = false;

    uint8_t derived_mac_addr[6] = {0};
    esp_read_mac(derived_mac_addr, ESP_MAC_WIFI_STA);
    ESP_LOGE("ESP_MAC_WIFI_STA ADDRESS", "%x%x%x%x%x%x\n", derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2], derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);

    register_button(GPIO_BOOT, xQueueBootKey);
    int boot_key_state = -1;
    xQueueReceive(xQueueBootKey, &boot_key_state, 3000 / portTICK_PERIOD_MS);
    if(boot_key_state == KEY_SHORT_PRESS)
    {
        testmode_flag = true;
    }


    if (testmode_flag)
    {
        ESP_LOGI(TAG, "--------------- Enter Test Mode ---------------\n");
        ets_printf("ESP32-S3-EYE V2.1 Firmware CN V0.1.3\n");
        register_lcd_test(xQueueTests, xQueueTestResult, xQueueKeyResult);
        register_test_controller(xQueueTests, xQueueBootKey, xQueueTestResult);
        register_imu_test(xQueueTests, xQueueTestResult);
        register_button_test(xQueueTests, xQueueTestResult, xQueueKeyResult);
        register_sd_card_test(xQueueTests, xQueueTestResult);
        register_led_test(xQueueTests, xQueueTestResult, xQueueKeyResult);
        register_camera_test(xQueueTests, xQueueTestResult);
        register_mic_test(xQueueTests, xQueueTestResult);
    }
    else
    {
        // vTaskDelete(consoleHandle_t);
        for (int i = 0; i < 6; ++i)
        {
            vQueueDelete(xQueueTests[i]);
        }
        vQueueDelete(xQueueTestFlag);
        vQueueDelete(xQueueTestResult);
        vQueueDelete(xQueueKeyResult);

        xQueueSRResult = xQueueCreate(1, sizeof(int));
        ets_printf("\n");
        ESP_LOGI(TAG, "Firmware CN V0.1.3");

        xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
        xQueueLCDFrame = xQueueCreate(2, sizeof(camera_fb_t *));
        xQueueKeyState = xQueueCreate(1, sizeof(int));
        xQueueLEDControl = xQueueCreate(1, sizeof(int));
        xQueueEventLogic = xQueueCreate(1, sizeof(int));

        register_lcd(xQueueLCDFrame, NULL, true);
        register_speech_recognition(xQueueSRResult, xQueueLEDControl);
        register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
        register_adc_button(buttons, 4, xQueueKeyState);
        register_event(xQueueSRResult, xQueueKeyState, xQueueEventLogic);
        // register_ai_image_task(xQueueAIFrame, xQueueEventLogic, xQueueLCDFrame);
        register_ai_image_task(xQueueAIFrame, xQueueEventLogic, NULL, xQueueLCDFrame, false);
        register_led(GPIO_NUM_3, xQueueLEDControl);

        print_memory(TAG, 3000);
    }

    // app_mic_test();
}
