#pragma once

#include "__base__.hpp"
#include "app_camera.hpp"
#include "app_button.hpp"
#include "app_speech.hpp"

class AppMotion : public Observer, public Frame
{
private:
    AppButton *key;
    AppSpeech *speech;

public:
    bool switch_on;

    AppMotion(AppButton *key,
              AppSpeech *speech,
              QueueHandle_t queue_i = nullptr,
              QueueHandle_t queue_o = nullptr,
              void (*callback)(camera_fb_t *) = esp_camera_fb_return);

    void update();

    void run();
};
