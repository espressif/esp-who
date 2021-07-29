#include "app_camera.hpp"

#include "esp_log.h"
#include "esp_system.h"

#include "app_define.h"
#include "dl_tool.hpp"

static const char *TAG = "app_camera";

void app_camera_init(framesize_t frame_size, uint8_t jpeg_quality, uint8_t fb_count)
{
    ESP_LOGI(TAG, "Camera module is %s", CAMERA_MODULE_NAME);

#if CONFIG_CAMERA_MODEL_ESP_EYE || CONFIG_CAMERA_MODEL_ESP32_CAM_BOARD
    /* IO13, IO14 is designed for JTAG by default,
     * to use it as generalized input,
     * firstly declair it as pullup input */
    gpio_config_t conf;
    conf.mode = GPIO_MODE_INPUT;
    conf.pull_up_en = GPIO_PULLUP_ENABLE;
    conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    conf.intr_type = GPIO_INTR_DISABLE;
    conf.pin_bit_mask = 1LL << 13;
    gpio_config(&conf);
    conf.pin_bit_mask = 1LL << 14;
    gpio_config(&conf);
#endif

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = CAMERA_PIN_D0;
    config.pin_d1 = CAMERA_PIN_D1;
    config.pin_d2 = CAMERA_PIN_D2;
    config.pin_d3 = CAMERA_PIN_D3;
    config.pin_d4 = CAMERA_PIN_D4;
    config.pin_d5 = CAMERA_PIN_D5;
    config.pin_d6 = CAMERA_PIN_D6;
    config.pin_d7 = CAMERA_PIN_D7;
    config.pin_xclk = CAMERA_PIN_XCLK;
    config.pin_pclk = CAMERA_PIN_PCLK;
    config.pin_vsync = CAMERA_PIN_VSYNC;
    config.pin_href = CAMERA_PIN_HREF;
    config.pin_sscb_sda = CAMERA_PIN_SIOD;
    config.pin_sscb_scl = CAMERA_PIN_SIOC;
    config.pin_pwdn = CAMERA_PIN_PWDN;
    config.pin_reset = CAMERA_PIN_RESET;
    config.xclk_freq_hz = XCLK_FREQ_HZ;
#if CONFIG_CAMERA_PIXEL_FORMAT_RGB565
    config.pixel_format = PIXFORMAT_RGB565;
#endif
#if CONFIG_CAMERA_PIXEL_FORMAT_YUV422
    config.pixel_format = PIXFORMAT_YUV422;
#endif
#if CONFIG_CAMERA_PIXEL_FORMAT_GRAYSCALE
    config.pixel_format = PIXFORMAT_GRAYSCALE;
#endif
#if CONFIG_CAMERA_PIXEL_FORMAT_JPEG
    config.pixel_format = PIXFORMAT_JPEG;
#endif
#if CONFIG_CAMERA_PIXEL_FORMAT_RGB888
    config.pixel_format = PIXFORMAT_RGB888;
#endif
#if CONFIG_CAMERA_PIXEL_FORMAT_RAW
    config.pixel_format = PIXFORMAT_RAW;
#endif
#if CONFIG_CAMERA_PIXEL_FORMAT_RGB444
    config.pixel_format = PIXFORMAT_RGB444;
#endif
#if CONFIG_CAMERA_PIXEL_FORMAT_RGB555
    config.pixel_format = PIXFORMAT_RGB555;
#endif
    config.frame_size = frame_size;
    config.jpeg_quality = jpeg_quality;
    config.fb_count = fb_count;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return;
    }

    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 1); //flip it back
    //initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID)
    {
        s->set_brightness(s, 1);  //up the blightness just a bit
        s->set_saturation(s, -2); //lower the saturation
    }
}

bool app_camera_decode(camera_fb_t *fb, uint16_t **image_ptr)
{
    assert(fb->format == PIXFORMAT_RGB565);
    *image_ptr = (uint16_t *)fb->buf;
    return true;
}

bool app_camera_decode(camera_fb_t *fb, uint8_t **image_ptr)
{
    *image_ptr = (uint8_t *)dl::tool::malloc_aligned(fb->height * fb->width * 3, sizeof(uint8_t));
    if (!*image_ptr)
    {
        ESP_LOGE(TAG, "malloc memory for image rgb888 failed");
        return false;
    }

    if (!fmt2rgb888(fb->buf, fb->len, fb->format, *image_ptr))
    {
        ESP_LOGE(TAG, "fmt2rgb888 failed");
        dl::tool::free_aligned(*image_ptr);
        return false;
    }
    return true;
}
