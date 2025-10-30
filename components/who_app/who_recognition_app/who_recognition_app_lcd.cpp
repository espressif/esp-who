#include "who_recognition_app_lcd.hpp"
#include "human_face_detect.hpp"
#include "who_lvgl_utils.hpp"
#include "who_yield2idle.hpp"
LV_FONT_DECLARE(montserrat_bold_26);

namespace who {
namespace app {
WhoRecognitionAppLCD::WhoRecognitionAppLCD(frame_cap::WhoFrameCap *frame_cap) :
    WhoRecognitionAppBase(frame_cap),
    m_lcd_disp(new lcd_disp::WhoFrameLCDDisp("LCDDisp", frame_cap->get_last_node(), 1))
{
    WhoApp::add_task(m_lcd_disp);
    m_lcd_disp->set_lcd_disp_cb(std::bind(&WhoRecognitionAppLCD::lcd_disp_cb, this, std::placeholders::_1));

    char db_path[64];
#if CONFIG_DB_FATFS_FLASH
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_SPIFLASH_MOUNT_POINT);
#elif CONFIG_DB_SPIFFS
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_BSP_SPIFFS_MOUNT_POINT);
#else
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_BSP_SD_MOUNT_POINT);
#endif
    m_recognition->set_recognizer(new HumanFaceRecognizer(
        db_path, static_cast<HumanFaceFeat::model_type_t>(CONFIG_DEFAULT_HUMAN_FACE_FEAT_MODEL), false));
    m_recognition->set_detect_model(
        new HumanFaceDetect(static_cast<HumanFaceDetect::model_type_t>(CONFIG_DEFAULT_HUMAN_FACE_DETECT_MODEL), false));

    bsp_display_lock(0);
    m_label = create_lvgl_label("", &montserrat_bold_26);
    const lv_font_t *font = lv_obj_get_style_text_font(m_label, LV_PART_MAIN);
    lv_obj_align(m_label, LV_ALIGN_TOP_MID, 0, font->line_height);
    bsp_display_unlock();

#if CONFIG_IDF_TARGET_ESP32S3
    int disp_n_frames = 60;
#elif CONFIG_IDF_TARGET_ESP32P4
    int disp_n_frames = 30;
#endif

    auto recognition_task = m_recognition->get_recognition_task();
    auto detect_task = m_recognition->get_detect_task();
#if defined(BSP_BOARD_ESP32_S3_EYE) || defined(BSP_BOARD_ESP32_S3_KORVO_2)
    m_recognition_button =
        button::get_recognition_button(button::recognition_button_type_t::PHYSICAL, recognition_task);
#elif defined(BSP_BOARD_ESP32_P4_FUNCTION_EV_BOARD)
    m_recognition_button = button::get_recognition_button(button::recognition_button_type_t::LVGL, recognition_task);
#else
    m_recognition_button =
        button::get_recognition_button(button::recognition_button_type_t::PHYSICAL, recognition_task);
#endif
    m_text_result_lcd_disp = new lcd_disp::WhoTextResultLCDDisp(recognition_task, m_label, disp_n_frames);
    m_detect_result_lcd_disp =
        new lcd_disp::WhoDetectResultLCDDisp(detect_task, m_lcd_disp->get_canvas(), {{255, 0, 0}});
    recognition_task->set_recognition_result_cb(
        std::bind(&WhoRecognitionAppLCD::recognition_result_cb, this, std::placeholders::_1));
    recognition_task->set_detect_result_cb(
        std::bind(&WhoRecognitionAppLCD::detect_result_cb, this, std::placeholders::_1));
    recognition_task->set_cleanup_func(std::bind(&WhoRecognitionAppLCD::recognition_cleanup, this));
    detect_task->set_detect_result_cb(std::bind(&WhoRecognitionAppLCD::detect_result_cb, this, std::placeholders::_1));
    detect_task->set_cleanup_func(std::bind(&WhoRecognitionAppLCD::detect_cleanup, this));
}

WhoRecognitionAppLCD::~WhoRecognitionAppLCD()
{
    delete m_recognition_button;
    delete m_text_result_lcd_disp;
    delete m_detect_result_lcd_disp;
    bsp_display_lock(0);
    lv_obj_del(m_label);
    bsp_display_unlock();
}

bool WhoRecognitionAppLCD::run()
{
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_lcd_disp->run(2560, 2, 0);
    ret &= m_recognition->get_detect_task()->run(3584, 2, 1);
    ret &= m_recognition->get_recognition_task()->run(3584, 2, 1);
    return ret;
}

void WhoRecognitionAppLCD::recognition_result_cb(const std::string &result)
{
    m_text_result_lcd_disp->save_text_result(result);
}

void WhoRecognitionAppLCD::detect_result_cb(const detect::WhoDetect::result_t &result)
{
    m_detect_result_lcd_disp->save_detect_result(result);
}

void WhoRecognitionAppLCD::lcd_disp_cb(who::cam::cam_fb_t *fb)
{
    m_detect_result_lcd_disp->lcd_disp_cb(fb);
    m_text_result_lcd_disp->lcd_disp_cb(fb);
}

void app::WhoRecognitionAppLCD::recognition_cleanup()
{
    m_text_result_lcd_disp->cleanup();
}

void app::WhoRecognitionAppLCD::detect_cleanup()
{
    m_detect_result_lcd_disp->cleanup();
}
} // namespace app
} // namespace who
