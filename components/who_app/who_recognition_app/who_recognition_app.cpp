#include "who_recognition_app.hpp"
#include "who_yield2idle.hpp"
namespace who {
namespace app {

WhoRecognitionApp::WhoRecognitionApp(frame_cap::WhoFrameCap *frame_cap,
                                     frame_cap::WhoFrameCapNode *lcd_disp_frame_cap_node) :
    m_frame_cap(frame_cap)
{
    if (!lcd_disp_frame_cap_node) {
        lcd_disp_frame_cap_node = frame_cap->get_last_node();
    }
    m_lcd_disp = new lcd_disp::WhoLCDDisp("LCDDisp", lcd_disp_frame_cap_node, 1);
    auto detect_model = new HumanFaceDetect();
    char db_path[64];
#if CONFIG_DB_FATFS_FLASH
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_SPIFLASH_MOUNT_POINT);
#elif CONFIG_DB_SPIFFS
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_BSP_SPIFFS_MOUNT_POINT);
#else
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_BSP_SD_MOUNT_POINT);
#endif
    auto recognizer = new HumanFaceRecognizer(db_path);
    m_detect =
        new recognition::WhoDetectLCD("Detect", frame_cap->get_last_node(), m_lcd_disp, detect_model, {{255, 0, 0}});
    m_recognition = new recognition::WhoRecognition("Recognition", m_lcd_disp, m_detect, recognizer);
    WhoApp::add_task(m_lcd_disp);
    WhoApp::add_task_group(m_frame_cap);
    WhoApp::add_task(m_detect);
    WhoApp::add_task(m_recognition);
}

bool WhoRecognitionApp::run()
{
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_lcd_disp->run(2560, 2, 0);
    ret &= m_detect->run(2560, 2, 1);
    ret &= m_recognition->run(3584, 2, 1);
    return ret;
}
} // namespace app
} // namespace who
