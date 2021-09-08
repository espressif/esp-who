#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#if CONFIG_LED_ILLUMINATOR_ENABLED
    void app_led_init();
    void app_led_duty(int duty)
#endif

#ifdef __cplusplus
}
#endif
