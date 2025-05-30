#pragma once
#include "who_recognition_app.hpp"
#include "who_spiflash_fatfs.hpp"

#include "esp_netif.h"
#include <esp_log.h>
#include <nvs_flash.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>
// #include "protocol_examples_common.h"
// #include "protocol_examples_utils.h"
#include "esp_event.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
// #include "esp_netif.h"
#include "esp_check.h"
#include "esp_tls.h"

#include "nvs_flash.h"
#include <esp_system.h>
#include <esp_wifi.h>
// #include "esp_eth.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include <mutex>

// class HttpLCD : public who::lcd::WhoLCD {
class HttpLCD : public who::lcd::WhoLCDiface {
public:
    HttpLCD() : buffer(nullptr), buffer_size(0) {}

    ~HttpLCD() {
        if (buffer) free(buffer);
    }

    void init() override {} // Prevent base class LCD hardware init

    esp_lcd_panel_handle_t get_lcd_panel_handle() override {
        // Return a dummy handle since we don't use the actual LCD hardware
        return nullptr;
    }

    void draw_full_lcd(const void* data) override  {
        std::lock_guard<std::mutex> lock(mutex);
        if (!buffer) {
            buffer_size = BSP_LCD_H_RES * BSP_LCD_V_RES * (BSP_LCD_BITS_PER_PIXEL / 8);
            buffer = (uint8_t*)malloc(buffer_size);
        }
        memcpy(buffer, data, buffer_size);
    }

    // Call this from your HTTP handler to get the latest frame
    const uint8_t* get_buffer(size_t& out_size) {
        std::lock_guard<std::mutex> lock(mutex);
        out_size = buffer_size;
        return buffer;
    }

private:
    uint8_t* buffer;
    size_t buffer_size;
    std::mutex mutex;
};