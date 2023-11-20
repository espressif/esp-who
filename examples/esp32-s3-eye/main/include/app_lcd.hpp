#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

#include "__base__.hpp"
#include "app_camera.hpp"
#include "app_button.hpp"
#include "app_speech.hpp"

#define BOARD_LCD_MOSI 47
#define BOARD_LCD_MISO -1
#define BOARD_LCD_SCK 21
#define BOARD_LCD_CS 44
#define BOARD_LCD_DC 43
#define BOARD_LCD_RST -1
#define BOARD_LCD_BL 48
#define BOARD_LCD_PIXEL_CLOCK_HZ (40 * 1000 * 1000)
#define BOARD_LCD_BK_LIGHT_ON_LEVEL 0
#define BOARD_LCD_BK_LIGHT_OFF_LEVEL !BOARD_LCD_BK_LIGHT_ON_LEVEL
#define BOARD_LCD_H_RES 240
#define BOARD_LCD_V_RES 240
#define BOARD_LCD_CMD_BITS 8
#define BOARD_LCD_PARAM_BITS 8
// #define LCD_HOST SPI2_HOST

class AppLCD : public Observer, public Frame
{
private:
    AppButton *key;
    AppSpeech *speech;

public:
    esp_lcd_panel_handle_t panel_handle;
    bool switch_on;
    bool paper_drawn;

    AppLCD(AppButton *key,
           AppSpeech *speech,
           QueueHandle_t xQueueFrameI = nullptr,
           QueueHandle_t xQueueFrameO = nullptr,
           void (*callback)(camera_fb_t *) = esp_camera_fb_return);

    void draw_wallpaper();
    void draw_color(int color);

    void update();

    void run();
};
