#include "who_camera.h"
#include "who_color_detection.hpp"
#include "who_lcd.h"
#include "who_button.h"
#include "event_logic.hpp"
#include "who_adc_button.h"

static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueLCDFrame = NULL;
static QueueHandle_t xQueueADCKeyState = NULL;
static QueueHandle_t xQueueGPIOKeyState = NULL;
static QueueHandle_t xQueueEventLogic = NULL;
static button_adc_config_t buttons[4] = {{1, 2800, 3000}, {2, 2250, 2450}, {3, 300, 500}, {4, 850, 1050}};

#define GPIO_BOOT GPIO_NUM_0

extern "C" void app_main()
{
    gpio_config_t gpio_conf;
    gpio_conf.mode = GPIO_MODE_OUTPUT_OD;
    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.pin_bit_mask = 1LL << GPIO_NUM_3;
    gpio_config(&gpio_conf);
    
    xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    xQueueLCDFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    xQueueADCKeyState = xQueueCreate(1, sizeof(int));
    xQueueGPIOKeyState = xQueueCreate(1, sizeof(int));
    xQueueEventLogic = xQueueCreate(1, sizeof(int));

    register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
    register_adc_button(buttons, 4, xQueueADCKeyState);
    register_button(GPIO_NUM_0, xQueueGPIOKeyState);
    register_event(xQueueADCKeyState, xQueueGPIOKeyState, xQueueEventLogic);
    register_color_detection(xQueueAIFrame, xQueueEventLogic, NULL, xQueueLCDFrame, false);
    register_lcd(xQueueLCDFrame, NULL, true);

}
