#include "app_peripherals.h"
#include "esp_log.h"
#include "esp_system.h"
#include "st7789.h"

static const char *TAG = "app_peripherals";

esp_err_t app_camera_init()
{
    ESP_LOGI(TAG, "Camera module is %s", CAMERA_MODULE_NAME);

#if CONFIG_CAMERA_MODULE_ESP_EYE || CONFIG_CAMERA_MODULE_ESP32_CAM_BOARD
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
    config.pixel_format = CAMERA_PIXFORMAT;
    config.frame_size = CAMERA_FRAME_SIZE;
    config.jpeg_quality = 12;
    config.fb_count = CAMERA_FB_COUNT;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return ESP_FAIL;
    }

    sensor_t *s = esp_camera_sensor_get();
    if (s->id.PID == OV3660_PID || s->id.PID == OV2640_PID)
        s->set_vflip(s, 1); //flip it back
    else if (s->id.PID == GC0308_PID){
        s->set_hmirror(s, 0);
    }
    else if (s->id.PID == GC032A_PID){
        s->set_vflip(s, 1);
        // s->set_hmirror(s, 0); //something wrong
    }

    if (s->id.PID == OV3660_PID)
    {
        s->set_brightness(s, 2);
        s->set_contrast(s, 3);
    }

    return ESP_OK;
}


#ifdef LCD_CONTROLLER
esp_err_t app_lcd_init(scr_driver_t *g_lcd){
    static scr_info_t g_lcd_info;

    spi_config_t bus_conf = {
        .miso_io_num = LCD_MISO,
        .mosi_io_num = LCD_MOSI,
        .sclk_io_num = LCD_SCLK,
        .max_transfer_sz = 2 * LCD_WIDTH * LCD_HEIGHT + 10,
    };
    spi_bus_handle_t spi_bus = spi_bus_create(SPI2_HOST, &bus_conf);

    scr_interface_spi_config_t spi_lcd_cfg = {
        .spi_bus = spi_bus,
        .pin_num_cs = LCD_CS,
        .pin_num_dc = LCD_DC,
        .clk_freq = 40 * 1000000,
        .swap_data = 0,
    };

    scr_interface_driver_t *iface_drv;
    scr_interface_create(SCREEN_IFACE_SPI, &spi_lcd_cfg, &iface_drv);
    esp_err_t ret = scr_find_driver(LCD_CONTROLLER, g_lcd);
    if (ESP_OK != ret)
    {
        ESP_LOGE(TAG, "screen find failed");
        return ESP_FAIL;
    }

    scr_controller_config_t lcd_cfg = {
        .interface_drv = iface_drv,
        .pin_num_rst = LCD_RST,
        .pin_num_bckl = LCD_BCKL,
        .rst_active_level = 0,
        .bckl_active_level = 0,
        .offset_hor = 0,
        .offset_ver = 0,
        .width = LCD_WIDTH,
        .height = LCD_HEIGHT,
        .rotate = LCD_ROTATE,
    };
    ret = g_lcd->init(&lcd_cfg);
    if (ESP_OK != ret)
    {
        ESP_LOGE(TAG, "screen initialize failed");
        return ESP_FAIL;
    }
    
    #if CONFIG_CAMERA_MODULE_ESP_S2_KALUGA
    lcd_st7789_set_invert(false);
    #endif

    g_lcd->get_info(&g_lcd_info);
    ESP_LOGI(TAG, "Screen name:%s | width:%d | height:%d", g_lcd_info.name, g_lcd_info.width, g_lcd_info.height);

    return ESP_OK;
}
#endif