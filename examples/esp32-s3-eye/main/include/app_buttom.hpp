#pragma once

#include <vector>

#include "__base__.hpp"

typedef enum
{
    _IDLE = 0,
    _MENU,
    _PLAY,
    _UP,
    _DOWN
} _key_name_t;

typedef struct
{
    _key_name_t key; /**< button index on the channel */
    int min;         /**< min voltage in mv corresponding to the button */
    int max;         /**< max voltage in mv corresponding to the button */
} key_config_t;

class AppButtom : public Subject
{
public:
    std::vector<key_config_t> key_configs;
    _key_name_t pressed;

    uint8_t menu;

    AppButtom();
    ~AppButtom();

    void run();
};
