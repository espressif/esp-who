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
    camera_fb_t *fb = NULL;
    dl::tool::Latency latency_total(24);
    dl::tool::Latency latency_fetch;
    dl::tool::Latency latency_decode;
    dl::tool::Latency latency_detect;
    dl::tool::Latency latency_recognize;

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

        latency_total.start();

        latency_fetch.start();
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            continue;
        }
        latency_fetch.end();

#if CONFIG_DL_ENABLED
#if CONFIG_DL_HUMAN_FACE
        latency_detect.start();
#if CONFIG_DL_HUMAN_FACE_DETECTION_S2_ENABLED
        std::list<dl::detect::result_t> &detect_candidates = detector.infer((uint16_t *)fb->buf, {(int)fb->height, (int)fb->width, 3});
        std::list<dl::detect::result_t> &detect_results = detector2.infer((uint16_t *)fb->buf, {(int)fb->height, (int)fb->width, 3}, detect_candidates);
#else
        std::list<dl::detect::result_t> &detect_results = detector.infer((uint16_t *)fb->buf, {(int)fb->height, (int)fb->width, 3});
#endif
        latency_detect.end();

        if (detect_results.size() > 0)
        {
            draw_detection_result((uint16_t *)fb->buf, fb->height, fb->width, detect_results);

#if CONFIG_DL_HUMAN_FACE_RECOGNITION_ENABLED
            latency_recognize.start();
            // TODO: recognize human face
            latency_recognize.end();
#endif
        }
#endif // CONFIG_DL_HUMAN_FACE

#if CONFIG_DL_CAT_FACE
        latency_detect.start();
        std::list<dl::detect::result_t> &detect_results = detector.infer((uint16_t *)fb->buf, {(int)fb->height, (int)fb->width, 3});
        latency_detect.end();
        if (detect_results.size() > 0)
        {
            draw_detection_result((uint16_t *)fb->buf, fb->height, fb->width, detect_results);
        }
#endif // CONFIG_DL_CAT_FACE

#if CONFIG_DL_HUMAN_HAND
        latency_detect.start();
        // TODO:
        latency_detect.end();
#endif // CONFIG_DL_HUMAN_HAND
#endif // CONFIG_DL_ENABLED

        app_lcd_draw_bitmap((uint16_t *)fb->buf, fb->height, fb->width);

        esp_camera_fb_return(fb);

        latency_total.end();
        uint32_t frame_latency = latency_total.get_period() / 1000;
        uint32_t average_frame_latency = latency_total.get_average_period() / 1000;
        ESP_LOGI("Frame Latency", "%4ums (%2.1ffps), Average: %4ums (%2.1ffps) | fetch: %4ums, decode: %4ums, detect: %4ums, recognize: %5ums",
                 frame_latency, 1000.0 / frame_latency, average_frame_latency, 1000.0 / average_frame_latency,
                 latency_fetch.get_period() / 1000,
                 latency_decode.get_period() / 1000,
                 latency_detect.get_period() / 1000,
                 latency_recognize.get_period() / 1000);
    }
}

void app_dl_init()
{
    xTaskCreatePinnedToCore(task_dl, "dl", 4 * 1024, NULL, 5, NULL, 1);
}
