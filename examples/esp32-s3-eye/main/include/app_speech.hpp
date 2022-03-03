#pragma once

#include "esp_afe_sr_iface.h"

#include "__base__.hpp"

class AppSpeech : public Subject
{
public:
    const esp_afe_sr_iface_t *afe_handle;
    esp_afe_sr_data_t *afe_data;
    bool detected;
    command_word_t command;

    AppSpeech();

    void run();
};
