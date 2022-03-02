#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "app_buttom.hpp"
#include "app_speech.hpp"

class LED : public Observer
{
private:
    const gpio_num_t pin;
    AppButtom *key;
    AppSpeech *sr;

public:
    LED(const gpio_num_t pin, AppButtom *key, AppSpeech *sr);
    ~LED();

    void update();
};
