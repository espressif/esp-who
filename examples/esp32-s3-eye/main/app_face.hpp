#pragma once

#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#include "face_recognition_tool.hpp"
// #if CONFIG_MFN_V1_Q8
// #include "face_recognition_112_v1_s8.hpp"
// #elif CONFIG_MFN_V1_Q16
#include "face_recognition_112_v1_s16.hpp"
// #endif

#include "__base__.hpp"
#include "app_camera.hpp"
#include "app_buttom.hpp"
#include "app_speech.hpp"

typedef enum
{
    IDLE = 0,
    ENROLL = 1,
    RECOGNIZE = 2,
    DELETE = 3,
} recognizer_state_t;

class AppFace : public Observer, public Frame
{
private:
    AppButtom *key;
    AppSpeech *speech;

public:
    HumanFaceDetectMSR01 detector;
    HumanFaceDetectMNP01 detector2;

    // #if CONFIG_MFN_V1_Q8
    // FaceRecognition112V1S8 *recognizer;
    // #elif CONFIG_MFN_V1_Q16
    FaceRecognition112V1S16 *recognizer;
    // #endif

    face_info_t recognize_result;
    recognizer_state_t state;
    recognizer_state_t state_previous;

    bool switch_on;

    uint8_t frame_count;

    AppFace(AppButtom *key,
                      AppSpeech *speech,
                      QueueHandle_t queue_i = nullptr,
                      QueueHandle_t queue_o = nullptr,
                      void (*callback)(camera_fb_t *) = esp_camera_fb_return);
    ~AppFace();

    void update();
    void run();
};
