#pragma once
#include "human_face_detect.hpp"
#include "human_face_recognition.hpp"
#include "who_cam.hpp"
#include <list>
#include <memory>
#include "bsp/esp-bsp.h"

LV_FONT_DECLARE(montserrat_bold_26);

namespace who {
namespace lcd {
typedef enum {
    FACE_DETECT_RESULT_TYPE,
    FACE_RECOGNITION_RESULT_TYPE,
    PEDESTRIAN_DETECT_RESULT_TYPE,
    RESULT_TYPE_MAX = PEDESTRIAN_DETECT_RESULT_TYPE
} result_type_t;

typedef struct {
    union {
        std::list<dl::detect::result_t> *detect_res;
        char *recognition_res;
    } res;
    struct timeval timestamp;
    result_type_t type;
} result_t;

class LCD {
public:
    LCD();
    ~LCD();
    void set_cam_fb(who::cam::cam_fb_t *fb);
    void init_lvgl_dscs();
    void draw_label(const char *label, lv_area_t coords);
    void draw_rectangle(const std::vector<int> &box, const char *label = nullptr);
    void draw_landmarks(const std::vector<int> &landmarks);
    void draw_detect_result(const std::list<dl::detect::result_t> &detect_res);
    static lv_obj_t *create_btn(const char *text, lv_obj_t *parent = lv_scr_act());
    static lv_obj_t *create_label(const char *text, lv_obj_t *parent = lv_scr_act());
    static lv_color_t cvt_little_endian_color(const lv_color_t &color)
    {
        lv_color_t new_color;
        new_color.red = (color.green & 0x1c) << 3 | (color.blue & 0xc0) >> 3;
        new_color.green = (color.blue & 0x38) << 2 | (color.red & 0xe0) >> 3;
        new_color.blue = (color.red & 0x18) << 3 | (color.green & 0xe0) >> 2;
        return new_color;
    }
    static lv_color_t get_color_by_palette(lv_palette_t palette)
    {
#if CONFIG_IDF_TARGET_ESP32P4
        return lv_palette_main(palette);
#elif CONFIG_IDF_TARGET_ESP32S3
        return cvt_little_endian_color(lv_palette_main(palette));
#endif
    }

private:
    lv_obj_t *m_canvas;
    lv_draw_label_dsc_t m_label_dsc;
    lv_draw_rect_dsc_t m_rect_dsc;
    lv_draw_arc_dsc_t m_arc_dsc;
    lv_layer_t m_layer;
    esp_lcd_panel_handle_t m_panel_handle;
};

class WhoLCD {
public:
    WhoLCD(LCD *lcd, who::cam::Cam *cam, int queue_len) : m_lcd(lcd), m_cam(cam), m_results(RESULT_TYPE_MAX + 1)
    {
        s_queue_handle = xQueueCreate(queue_len, sizeof(result_t));
    };
    void run();
    static TaskHandle_t s_task_handle;
    static QueueHandle_t s_queue_handle;

private:
    void display_result(who::cam::cam_fb_t *fb);
    static void display_task(void *args);
    static void recv_task(void *args);
    LCD *m_lcd;
    who::cam::Cam *m_cam;
    std::vector<std::list<result_t>> m_results;
    static SemaphoreHandle_t s_mutex;
};

} // namespace lcd
} // namespace who
