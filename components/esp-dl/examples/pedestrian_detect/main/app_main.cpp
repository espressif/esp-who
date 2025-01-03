#include "esp_log.h"
#include "pedestrian_detect.hpp"
#include "bsp/esp-bsp.h"

extern const uint8_t pedestrian_jpg_start[] asm("_binary_pedestrian_jpg_start");
extern const uint8_t pedestrian_jpg_end[] asm("_binary_pedestrian_jpg_end");
const char *TAG = "pedestrian_detect";

extern "C" void app_main(void)
{
#if CONFIG_PEDESTRIAN_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_mount());
#endif

    dl::image::jpeg_img_t jpeg_img = {
        .data = (uint8_t *)pedestrian_jpg_start,
        .width = 640,
        .height = 480,
        .data_size = (uint32_t)(pedestrian_jpg_end - pedestrian_jpg_start),
    };
    dl::image::img_t img;
    img.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888;
    sw_decode_jpeg(jpeg_img, img, true);

#if !CONFIG_PEDESTRIAN_DETECT_MODEL_IN_SDCARD
    PedestrianDetect *detect = new PedestrianDetect();
#else
    char dir[64];
#if CONFIG_IDF_TARGET_ESP32P4
    snprintf(dir, sizeof(dir), "%s/espdl_models/p4", CONFIG_BSP_SD_MOUNT_POINT);
#elif CONFIG_IDF_TARGET_ESP32S3
    snprintf(dir, sizeof(dir), "%s/espdl_models/s3", CONFIG_BSP_SD_MOUNT_POINT);
#endif
    PedestrianDetect *detect = new PedestrianDetect(dir);
#endif

    auto &detect_results = detect->run(img);
    for (const auto &res : detect_results) {
        ESP_LOGI(TAG,
                 "[score: %f, x1: %d, y1: %d, x2: %d, y2: %d]\n",
                 res.score,
                 res.box[0],
                 res.box[1],
                 res.box[2],
                 res.box[3]);
    }
    delete detect;
    heap_caps_free(img.data);

#if CONFIG_PEDESTRIAN_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_unmount());
#endif
}
