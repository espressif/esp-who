#include "app_dl.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "app_camera.h"

#include <list>
#include "dl_tool.hpp"
#include "dl_image.hpp"
#include "dl_detect_define.hpp"

#if CONFIG_DL_DETECT_HUMAN_FACE
#include "human_face_detect_msr01.hpp"
#endif

#if CONFIG_DL_DETECT_HUMAN_FACE_WITH_KEYPOINT
#include "human_face_detect_mnp01.hpp"
#endif

#if CONFIG_DL_DETECT_CAT_FACE
#include "cat_face_detect_mn03.hpp"
#endif

#if CONFIG_DL_RECOGNIZE_HUMAN_FACE
// TODO: recognize human face
#endif

static const char *TAG = "app_dl";
void task_dl(void *arg)
{
    camera_fb_t *fb = NULL;
    uint8_t *image_rgb888;
    dl::tool::Latency latency_total(24);
    dl::tool::Latency latency_fetch;
    dl::tool::Latency latency_decode;
    dl::tool::Latency latency_detect;

    /* 1. Load configuration for detection */
#if CONFIG_DL_DETECT_HUMAN_FACE
    HumanFaceDetectMSR01 detector(0.3F, 0.3F, 10, 0.3F);
#endif

#if CONFIG_DL_DETECT_HUMAN_FACE_WITH_KEYPOINT
    HumanFaceDetectMNP01 detector2(0.4F, 0.3F, 10);
#endif

#if CONFIG_DL_DETECT_CAT_FACE
    CatFaceDetectMN03 detector(0.4F, 0.3F, 10, 0.3F);
#endif

#if CONFIG_DL_RECOGNIZE_HUMAN_FACE
    dl::tool::Latency latency_recognize;
// TODO: recognize human face
#endif

    while (true)
    {
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

        /* 3. Transform image to RGB */
        latency_decode.start();
        image_rgb888 = (uint8_t *)dl::tool::malloc_aligned(fb->height * fb->width * 3, sizeof(uint8_t));
        bool res = fmt2rgb888(fb->buf, fb->len, fb->format, image_rgb888);
        if (false == res)
        {
            ESP_LOGE(TAG, "fmt2rgb888 failed, fb: %d", fb->len);
            dl::tool::free_aligned(image_rgb888);
            continue;
        }
        int image_height = fb->height;
        int image_width = fb->width;
        esp_camera_fb_return(fb);
        latency_decode.end();

        /* 4. Do deep-learning processing */
        latency_detect.start();
#if CONFIG_DL_DETECT_HUMAN_FACE_WITH_KEYPOINT
        std::list<dl::detect::result_t> &candidates = detector.infer((uint8_t *)image_rgb888, {image_height, image_width, 3});
        std::list<dl::detect::result_t> &results = detector2.infer((uint8_t *)image_rgb888, {image_height, image_width, 3}, candidates);
#else
        std::list<dl::detect::result_t> &results = detector.infer((uint8_t *)image_rgb888, {image_height, image_width, 3});
#endif
        latency_detect.end();

#if CONFIG_DL_RECOGNIZE_HUMAN_FACE
        latency_recognize.start();
#endif
        if (results.size() > 0)
        {
            int i = 0;
            for (std::list<dl::detect::result_t>::iterator prediction = results.begin(); prediction != results.end(); prediction++, i++)
            {
#if CONFIG_DL_DETECT_HUMAN_FACE_WITH_KEYPOINT
                ESP_LOGI(TAG, "[%d]: (%3d, %3d, %3d, %3d) | left eye: (%3d, %3d), right eye: (%3d, %3d), nose: (%3d, %3d), mouth left: (%3d, %3d), mouth right: (%3d, %3d)",
                         i,
                         prediction->box[0], prediction->box[1], prediction->box[2], prediction->box[3],
                         prediction->keypoint[0], prediction->keypoint[1],  // left eye
                         prediction->keypoint[6], prediction->keypoint[7],  // right eye
                         prediction->keypoint[4], prediction->keypoint[5],  // nose
                         prediction->keypoint[2], prediction->keypoint[3],  // mouth left corner
                         prediction->keypoint[8], prediction->keypoint[9]); // mouth right corner
#else
                ESP_LOGI(TAG, "[%d]: (%3d, %3d, %3d, %3d)",
                         i,
                         prediction->box[0], prediction->box[1], prediction->box[2], prediction->box[3]);
#endif
            }

#if CONFIG_DL_RECOGNIZE_HUMAN_FACE
            latency_recognize.start();
#endif
        }
#if CONFIG_DL_RECOGNIZE_HUMAN_FACE
        latency_recognize.end();
#endif
        dl::tool::free_aligned(image_rgb888);
        latency_total.end();

        uint32_t frame_latency = latency_total.get_period() / 1000;
        uint32_t average_frame_latency = latency_total.get_average_period() / 1000;
        ESP_LOGI(TAG, "Frame: %4ums (%.1ffps), Average: %4ums (%.1ffps) | fetch: %4ums, decode: %4ums, detect: %4ums"
#if CONFIG_DL_RECOGNIZE_HUMAN_FACE
                      ", recognize: %5ums"
#endif
                 ,
                 frame_latency, 1000.0 / frame_latency, average_frame_latency, 1000.0 / average_frame_latency,
                 latency_fetch.get_period() / 1000,
                 latency_decode.get_period() / 1000,
                 latency_detect.get_period() / 1000
#if CONFIG_DL_RECOGNIZE_HUMAN_FACE
                 ,
                 latency_recognize.get_period() / 1000
#endif
        );
    }
}

void app_dl_init()
{
    xTaskCreatePinnedToCore(task_dl, "dl", 4 * 1024, NULL, 5, NULL, 1);
}
