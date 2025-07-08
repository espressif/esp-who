#include "who_recognition_app_term.hpp"
#include "human_face_detect.hpp"
#include "who_yield2idle.hpp"

namespace who {
namespace app {
WhoRecognitionAppTerm::WhoRecognitionAppTerm(frame_cap::WhoFrameCap *frame_cap) : WhoRecognitionAppBase(frame_cap)
{
    auto recognition_task = m_recognition->get_recognition_task();
    recognition_task->set_recognition_result_cb(
        std::bind(&WhoRecognitionAppTerm::recognition_result_cb, this, std::placeholders::_1));
    char db_path[64];
#if CONFIG_DB_FATFS_FLASH
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_SPIFLASH_MOUNT_POINT);
#elif CONFIG_DB_SPIFFS
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_BSP_SPIFFS_MOUNT_POINT);
#else
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_BSP_SD_MOUNT_POINT);
#endif
    m_recognition->set_recognizer(new HumanFaceRecognizer(db_path));
    m_recognition->set_detect_model(new HumanFaceDetect());
    m_recognition_button =
        button::get_recognition_button(button::recognition_button_type_t::PHYSICAL, recognition_task);
}

WhoRecognitionAppTerm::~WhoRecognitionAppTerm()
{
    delete m_recognition_button;
}

bool WhoRecognitionAppTerm::run()
{
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_recognition->get_detect_task()->run(3584, 2, 1);
    ret &= m_recognition->get_recognition_task()->run(3584, 2, 1);
    return ret;
}

void WhoRecognitionAppTerm::recognition_result_cb(const std::string &result)
{
    ESP_LOGI("WhoRecognition", "%s", result.c_str());
}
} // namespace app
} // namespace who
