#include "human_face_detect.hpp"
#include "who_cam_lcd.hpp"
#include "who_detect.hpp"

using namespace who::app;
using namespace who::lcd;
using namespace who::cam;
using namespace dl::detect;
extern "C" void app_main(void)
{
#if CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_mount());
#endif

#if CONFIG_IDF_TARGET_ESP32P4
#if CONFIG_USE_PPA_CAM
    auto cam = new PPAP4Cam(VIDEO_PIX_FMT_RGB565, 6, V4L2_MEMORY_MMAP, 160, 120, true);
#else
    auto cam = new P4Cam(VIDEO_PIX_FMT_RGB565, 5, V4L2_MEMORY_MMAP, true);
#endif
#elif CONFIG_IDF_TARGET_ESP32S3
    auto cam = new S3Cam(PIXFORMAT_RGB565, FRAMESIZE_240X240, 4, true);
#endif
    auto who_cam_lcd = new WhoCamLCD(cam);

#if !CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
    HumanFaceDetect *detect = new HumanFaceDetect();
#else
    char dir[64];
#if CONFIG_IDF_TARGET_ESP32P4
    snprintf(dir, sizeof(dir), "%s/espdl_models/p4", CONFIG_BSP_SD_MOUNT_POINT);
#elif CONFIG_IDF_TARGET_ESP32S3
    snprintf(dir, sizeof(dir), "%s/espdl_models/s3", CONFIG_BSP_SD_MOUNT_POINT);
#endif
    HumanFaceDetect *detect = new HumanFaceDetect(dir);
#endif
    auto who_detect = new WhoDetect(detect, cam, "HumanFaceDet", (1 << 0));

    who_cam_lcd->run();
    who_detect->run();
#if CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_unmount());
#endif
}
