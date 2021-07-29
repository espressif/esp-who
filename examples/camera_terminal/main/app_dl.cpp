#include "app_dl.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "app_camera.hpp"

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
    IMAGE_T *image_ptr;
    dl::tool::Latency latency_total(24);
    dl::tool::Latency latency_fetch;
    dl::tool::Latency latency_decode;
    dl::tool::Latency latency_detect;
    dl::tool::Latency latency_recognize;

    /* 1. Load configuration for detection */
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

        /* 2. Get one image with camera */
        latency_fetch.start();
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            continue;
        }
        latency_fetch.end();

#if CONFIG_DL_ENABLED
        /* 3. Transform image to RGB */
        latency_decode.start();
        int image_height = fb->height;
        int image_width = fb->width;
        if (app_camera_decode(fb, &image_ptr) == false)
        {
            esp_camera_fb_return(fb);
            continue;
        }
#if !CONFIG_CAMERA_PIXEL_FORMAT_RGB565
        esp_camera_fb_return(fb);
#endif
        latency_decode.end();

        /* 4. Do deep-learning processing */
#if CONFIG_DL_HUMAN_FACE
        latency_detect.start();
#if CONFIG_DL_HUMAN_FACE_DETECTION_S2_ENABLED
        std::list<dl::detect::result_t> &detect_candidates = detector.infer((IMAGE_T *)image_ptr, {(int)image_height, (int)image_width, 3});
        std::list<dl::detect::result_t> &detect_results = detector2.infer((IMAGE_T *)image_ptr, {(int)image_height, (int)image_width, 3}, detect_candidates);
#else
        std::list<dl::detect::result_t> &detect_results = detector.infer((IMAGE_T *)image_ptr, {(int)image_height, (int)image_width, 3});
#endif
        latency_detect.end();

        if (detect_results.size() > 0)
        {
            print_detection_result(detect_results);

#if CONFIG_DL_HUMAN_FACE_RECOGNITION_ENABLED
            latency_recognize.start();
            // TODO: recognize human face
            latency_recognize.end();
#endif
        }
#endif // CONFIG_DL_HUMAN_FACE

#if CONFIG_DL_CAT_FACE
        latency_detect.start();
        std::list<dl::detect::result_t> &detect_results = detector.infer((IMAGE_T *)image_ptr, {(int)image_height, (int)image_width, 3});
        latency_detect.end();
        if (detect_results.size() > 0)
        {
            is_detected = true;
            print_detection_result(image_ptr, fb->height, fb->width, detect_results);
        }
#endif // CONFIG_DL_CAT_FACE

#if CONFIG_DL_HUMAN_HAND
        latency_detect.start();
        // TODO:
        latency_detect.end();
#endif // CONFIG_DL_HUMAN_HAND

#if CONFIG_CAMERA_PIXEL_FORMAT_RGB565
        esp_camera_fb_return(fb);
#else
        dl::tool::free_aligned(image_ptr);
#endif

#else  // not CONFIG_DL_ENABLED
        esp_camera_fb_return(fb);
#endif // CONFIG_DL_ENABLED

        latency_total.end();
        uint32_t frame_latency = latency_total.get_period() / 1000;
        uint32_t average_frame_latency = latency_total.get_average_period() / 1000;
        ESP_LOGI(TAG, "Frame: %4ums (%2.1ffps), Average: %4ums (%2.1ffps) | fetch: %4ums, decode: %4ums, detect: %4ums, recognize: %5ums",
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
