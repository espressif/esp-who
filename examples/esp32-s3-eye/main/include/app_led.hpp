#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "app_button.hpp"
#include "app_speech.hpp"

class AppLED : public Observer
{
private:
    const gpio_num_t pin;
    AppButton *key;
    AppSpeech *sr;

public:
    AppLED(const gpio_num_t pin, AppButton *key, AppSpeech *sr);

    void update();
};
