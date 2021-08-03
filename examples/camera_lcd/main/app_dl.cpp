#include "app_dl.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "app_camera.hpp"
#include "app_lcd.h"

#include <list>
#include "dl_tool.hpp"
#include "dl_image.hpp"
#include "dl_detect_define.hpp"
#include "app_common.hpp"

#if CONFIG_DL_HUMAN_FACE_DETECTION_S1_MSR01
#include "human_face_detect_msr01.hpp"
#endif

#if CONFIG_DL_HUMAN_FACE_DETECTION_S2_MNP01
#include "human_face_detect_mnp01.hpp"
#endif

#if CONFIG_DL_CAT_FACE_DETECTION_MN03
#include "cat_face_detect_mn03.hpp"
#endif

#if CONFIG_DL_HUMAN_FACE_RECOGNITION_XXX
// TODO: recognize human face
#endif

static const char *TAG = "app_dl";

void task_dl(void *arg)
{
    dl::tool::Latency latency_total(24);
    dl::tool::Latency latency_fetch;
    dl::tool::Latency latency_decode;
    dl::tool::Latency latency_detect;
    dl::tool::Latency latency_recognize;
    dl::tool::Latency latency_moving;

#if CONFIG_DL_HUMAN_FACE
#if CONFIG_DL_HUMAN_FACE_DETECTION_S1_MSR01
    HumanFaceDetectMSR01 detector(0.3F, 0.3F, 10, 0.3F);
#endif

#if CONFIG_DL_HUMAN_FACE_DETECTION_S2_MNP01
    HumanFaceDetectMNP01 detector2(0.4F, 0.3F, 10);
#endif

#if CONFIG_DL_HUMAN_FACE_RECOGNITION_XXX
// TODO: recognize human face
#endif
#endif

#if CONFIG_DL_CAT_FACE
#if CONFIG_DL_CAT_FACE_DETECTION_MN03
    CatFaceDetectMN03 detector(0.4F, 0.3F, 10, 0.3F);
#endif
#endif

    while (true)
    {
        latency_fetch.clear_period();
        latency_decode.clear_period();
        latency_detect.clear_period();
        latency_recognize.clear_period();
        latency_moving.clear_period();

        latency_total.start();

        latency_fetch.start();
        camera_fb_t *frame = esp_camera_fb_get();
        if (!frame)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            continue;
        }
        latency_fetch.end();

#if CONFIG_DL_MOVING_TARGET_DETECTION_ENABLED
        camera_fb_t *frame2 = esp_camera_fb_get();
        if (!frame2)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            continue;
        }
        latency_fetch.end();

        latency_moving.start();
        uint32_t moving_point_number = dl::image::get_moving_point_number((uint16_t *)frame->buf, (uint16_t *)frame2->buf, frame->height, frame->width, 8, 15);
        latency_moving.end();
        if (moving_point_number > 50)
        {
            ESP_LOGI("Moving Target", "Detected.");
            dl::image::draw_filled_rectangle((uint16_t *)frame->buf, frame->height, frame->width, 0, 0, 10, 10);
        }
        esp_camera_fb_return(frame2);
#endif

#if CONFIG_DL_ENABLED
#if CONFIG_DL_HUMAN_FACE
        latency_detect.start();
#if CONFIG_DL_HUMAN_FACE_DETECTION_S2_ENABLED
        std::list<dl::detect::result_t> &detect_candidates = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
        std::list<dl::detect::result_t> &detect_results = detector2.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);
#else
        std::list<dl::detect::result_t> &detect_results = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
#endif
        latency_detect.end();

        if (detect_results.size() > 0)
        {
            draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);

#if CONFIG_DL_HUMAN_FACE_RECOGNITION_ENABLED
            latency_recognize.start();
            // TODO: recognize human face
            latency_recognize.end();
#endif
        }
#endif // CONFIG_DL_HUMAN_FACE

#if CONFIG_DL_CAT_FACE
        latency_detect.start();
        std::list<dl::detect::result_t> &detect_results = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
        latency_detect.end();
        if (detect_results.size() > 0)
        {
            draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
        }
#endif // CONFIG_DL_CAT_FACE

#if CONFIG_DL_HUMAN_HAND
        latency_detect.start();
        // TODO:
        latency_detect.end();
#endif // CONFIG_DL_HUMAN_HAND
#endif // CONFIG_DL_ENABLED

        app_lcd_draw_bitmap((uint16_t *)frame->buf, frame->height, frame->width);

        esp_camera_fb_return(frame);

        latency_total.end();
        uint32_t frame_latency = latency_total.get_period() / 1000;
        uint32_t average_frame_latency = latency_total.get_average_period() / 1000;
        ESP_LOGI("Frame Latency", "%4ums (%2.1ffps), Average: %4ums (%2.1ffps) | fetch: %4ums, "
#if CONFIG_DL_MOVING_TARGET_DETECTION_ENABLED
                                  "moving: %4uus, "
#endif
                                  "decode: %4ums, detect: %4ums, recognize: %5ums",
                 frame_latency, 1000.0 / frame_latency, average_frame_latency, 1000.0 / average_frame_latency,
                 latency_fetch.get_period() / 1000,
#if CONFIG_DL_MOVING_TARGET_DETECTION_ENABLED
                 latency_moving.get_period(),
#endif
                 latency_decode.get_period() / 1000,
                 latency_detect.get_period() / 1000,
                 latency_recognize.get_period() / 1000);
    }
}

void app_dl_init()
{
    xTaskCreatePinnedToCore(task_dl, "dl", 4 * 1024, NULL, 5, NULL, 1);
}
