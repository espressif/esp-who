#include "who_qrcode_lcd.hpp"
#include "who_lvgl_utils.hpp"
#if CONFIG_IDF_TARGET_ESP32P4
#define WHO_QRCODE_RES_SHOW_N_FRAMES (60)
#elif CONFIG_IDF_TARGET_ESP32S3
#define WHO_QRCODE_RES_SHOW_N_FRAMES (30)
#endif
LV_FONT_DECLARE(montserrat_bold_26);
LV_FONT_DECLARE(montserrat_bold_20);

static const char *TAG = "WhoQRCode";

namespace who {
namespace qrcode {
void WhoQRCodeLCD::lcd_display_cb(who::cam::cam_fb_t *fb)
{
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    static int cnt = WHO_QRCODE_RES_SHOW_N_FRAMES;
    if (!m_results.empty()) {
        lv_label_set_text(m_label, m_results.back().c_str());
        m_results.clear();
        cnt = 0;
    }
    if (cnt < WHO_QRCODE_RES_SHOW_N_FRAMES && ++cnt == WHO_QRCODE_RES_SHOW_N_FRAMES) {
        lv_label_set_text(m_label, "");
    }
    xSemaphoreGive(m_res_mutex);
}

void WhoQRCodeLCD::task()
{
    create_label();
    WhoQRCodeBase::task();
}

void WhoQRCodeLCD::create_label()
{
    bsp_display_lock(0);
#if CONFIG_IDF_TARGET_ESP32S3
    m_label = create_lvgl_label("", &montserrat_bold_20);
#elif CONFIG_IDF_TARGET_ESP32P4
    m_label = create_lvgl_label("", &montserrat_bold_26);
#endif
    const lv_font_t *font = lv_obj_get_style_text_font(m_label, LV_PART_MAIN);
    lv_obj_set_width(m_label, BSP_LCD_H_RES - 2 * font->line_height);
    lv_obj_align(m_label, LV_ALIGN_TOP_LEFT, font->line_height, font->line_height);
    lv_label_set_long_mode(m_label, LV_LABEL_LONG_WRAP);
    bsp_display_unlock();
}

void WhoQRCodeLCD::on_new_qrcode_result(const char *result)
{
    ESP_LOGI(TAG, "%s", result);
    xSemaphoreTake(m_res_mutex, portMAX_DELAY);
    m_results.emplace_back(std::string(result));
    xSemaphoreGive(m_res_mutex);
}
} // namespace qrcode
} // namespace who
