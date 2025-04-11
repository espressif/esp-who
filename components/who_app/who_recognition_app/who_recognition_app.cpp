#include "who_recognition_app.hpp"

namespace who {
namespace app {
WhoRecognitionApp::WhoRecognitionApp(const std::string &name)
{
    auto detect_model = new HumanFaceDetect();
    auto feat_model = new HumanFaceFeat();
    char db_path[64];
#if CONFIG_DB_FATFS_FLASH
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_SPIFLASH_MOUNT_POINT);
#elif CONFIG_DB_SPIFFS
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_BSP_SPIFFS_MOUNT_POINT);
#else
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_BSP_SD_MOUNT_POINT);
#endif
    auto recognizer = new HumanFaceRecognizer(feat_model, db_path);
    m_frame_cap = new frame_cap::WhoFrameCapLCD(name + "_cap");
    m_detect = new recognition::WhoDetectLCD(m_frame_cap, detect_model, name + "_det", {{255, 0, 0}});
    m_recognition = new recognition::WhoRecognition(m_detect, recognizer, name + "_rec");
    add_element(m_frame_cap);
    add_element(m_detect);
    add_element(m_recognition);
}

bool WhoRecognitionApp::run()
{
    bool ret = m_frame_cap->run(4096, 2, 0);
    ret &= m_detect->run(2560, 2, 1);
    ret &= m_recognition->run(3584, 2, 0);
    return ret;
}
} // namespace app
} // namespace who
