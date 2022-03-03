#pragma once

#include "sdkconfig.h"

#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#include "face_recognition_tool.hpp"
#if CONFIG_MFN_V1
#if CONFIG_S8
#include "face_recognition_112_v1_s8.hpp"
#elif CONFIG_S16
#include "face_recognition_112_v1_s16.hpp"
#endif
#endif

#include "__base__.hpp"
#include "app_camera.hpp"
#include "app_button.hpp"
#include "app_speech.hpp"

typedef enum
{
    FACE_IDLE = 0,
    FACE_ENROLL = 1,
    FACE_RECOGNIZE = 2,
    FACE_DELETE = 3,
} face_action_t;

class AppFace : public Observer, public Frame
{
private:
    AppButton *key;
    AppSpeech *speech;

public:
    HumanFaceDetectMSR01 detector;
    HumanFaceDetectMNP01 detector2;

#if CONFIG_MFN_V1
#if CONFIG_S8
    FaceRecognition112V1S8 *recognizer;
#elif CONFIG_S16
    FaceRecognition112V1S16 *recognizer;
#endif
#endif

    face_info_t recognize_result;
    face_action_t state;
    face_action_t state_previous;

    bool switch_on;

    uint8_t frame_count;

    AppFace(AppButton *key,
            AppSpeech *speech,
            QueueHandle_t queue_i = nullptr,
            QueueHandle_t queue_o = nullptr,
            void (*callback)(camera_fb_t *) = esp_camera_fb_return);
    ~AppFace();

    void update();
    void run();
};
