#pragma once

#include "__base__.hpp"
#include "app_camera.hpp"
#include "app_buttom.hpp"
#include "app_speech.hpp"

class AppMotion : public Observer, public Frame
{
private:
    AppButtom *key;
    AppSpeech *speech;

public:
    bool switch_on;

    AppMotion(AppButtom *key,
                    AppSpeech *speech,
                    QueueHandle_t queue_i = nullptr,
                    QueueHandle_t queue_o = nullptr,
                    void (*callback)(camera_fb_t *) = esp_camera_fb_return);

    void update();

    void run();
};
