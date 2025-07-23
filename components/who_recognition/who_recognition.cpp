#include "who_recognition.hpp"

namespace who {
namespace recognition {
WhoRecognitionCore::WhoRecognitionCore(const std::string &name, detect::WhoDetect *detect) :
    task::WhoTask(name), m_detect(detect)
{
}

WhoRecognitionCore::~WhoRecognitionCore()
{
    delete m_recognizer;
}

void WhoRecognitionCore::set_recognizer(HumanFaceRecognizer *recognizer)
{
    m_recognizer = recognizer;
}

void WhoRecognitionCore::set_recognition_result_cb(const std::function<void(const std::string &)> &result_cb)
{
    m_recognition_result_cb = result_cb;
}

void WhoRecognitionCore::set_detect_result_cb(const std::function<void(const detect::WhoDetect::result_t &)> &result_cb)
{
    m_detect_result_cb = result_cb;
}

void WhoRecognitionCore::set_cleanup_func(const std::function<void()> &cleanup_func)
{
    m_cleanup = cleanup_func;
}

bool WhoRecognitionCore::run(const configSTACK_DEPTH_TYPE uxStackDepth,
                             UBaseType_t uxPriority,
                             const BaseType_t xCoreID)
{
    if (!m_recognizer) {
        ESP_LOGE("WhoRecognitionCore", "recognizer is nullptr, please call set_recognizer() first.");
        return false;
    }
    return task::WhoTask::run(uxStackDepth, uxPriority, xCoreID);
}

void WhoRecognitionCore::task()
{
    while (true) {
        EventBits_t event_bits = xEventGroupWaitBits(
            m_event_group, RECOGNIZE | ENROLL | DELETE | TASK_PAUSE | TASK_STOP, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bits & TASK_STOP) {
            break;
        } else if (event_bits & TASK_PAUSE) {
            xEventGroupSetBits(m_event_group, TASK_PAUSED);
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, TASK_RESUME | TASK_STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & TASK_STOP) {
                break;
            } else {
                continue;
            }
        }
        if (event_bits & RECOGNIZE) {
            auto new_detect_result_cb = [this](const detect::WhoDetect::result_t &result) {
                auto ret = m_recognizer->recognize(result.img, result.det_res);
                if (m_detect_result_cb) {
                    m_detect_result_cb(result);
                }
                if (m_recognition_result_cb) {
                    if (ret.empty()) {
                        m_recognition_result_cb("who?");
                    } else {
                        m_recognition_result_cb(std::format("id: {}, sim: {:.2f}", ret[0].id, ret[0].similarity));
                    }
                }
                m_detect->set_detect_result_cb(m_detect_result_cb);
            };
            m_detect->set_detect_result_cb(new_detect_result_cb);
            continue;
        }
        if (event_bits & ENROLL) {
            auto new_detect_result_cb = [this](const detect::WhoDetect::result_t &result) {
                esp_err_t ret = m_recognizer->enroll(result.img, result.det_res);
                if (m_detect_result_cb) {
                    m_detect_result_cb(result);
                }
                if (m_recognition_result_cb) {
                    if (ret == ESP_FAIL) {
                        m_recognition_result_cb("Failed to enroll.");
                    } else {
                        m_recognition_result_cb(std::format("id: {} enrolled.", m_recognizer->get_num_feats()));
                    }
                }
                m_detect->set_detect_result_cb(m_detect_result_cb);
            };
            m_detect->set_detect_result_cb(new_detect_result_cb);
            continue;
        }
        if (event_bits & DELETE) {
            esp_err_t ret = m_recognizer->delete_last_feat();
            if (m_recognition_result_cb) {
                if (ret == ESP_FAIL) {
                    m_recognition_result_cb("Failed to delete.");
                } else {
                    m_recognition_result_cb(std::format("id: {} deleted.", m_recognizer->get_num_feats() + 1));
                }
            }
        }
    }
    xEventGroupSetBits(m_event_group, TASK_STOPPED);
    vTaskDelete(NULL);
}

void WhoRecognitionCore::cleanup()
{
    if (m_cleanup) {
        m_cleanup();
    }
}

WhoRecognition::WhoRecognition(frame_cap::WhoFrameCapNode *frame_cap_node) :
    m_detect(new detect::WhoDetect("Detect", frame_cap_node)),
    m_recognition(new WhoRecognitionCore("Recognition", m_detect))
{
    WhoTaskGroup::register_task(m_detect);
    WhoTaskGroup::register_task(m_recognition);
}

WhoRecognition::~WhoRecognition()
{
    WhoTaskGroup::destroy();
}

void WhoRecognition::set_detect_model(dl::detect::Detect *model)
{
    m_detect->set_model(model);
}

void WhoRecognition::set_recognizer(HumanFaceRecognizer *recognizer)
{
    m_recognition->set_recognizer(recognizer);
}

detect::WhoDetect *WhoRecognition::get_detect_task()
{
    return m_detect;
}

WhoRecognitionCore *WhoRecognition::get_recognition_task()
{
    return m_recognition;
}
} // namespace recognition
} // namespace who
