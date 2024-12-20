#include "who_app.hpp"
#include "who_cam.hpp"
#include "who_lcd.hpp"

extern "C" void app_main(void)
{
#if CONFIG_PEDESTRIAN_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_mount());
#endif
#if CONFIG_IDF_TARGET_ESP32P4
    who::cam::Cam *cam = new who::cam::P4Cam(who::cam::VIDEO_PIX_FMT_RGB565, 5, V4L2_MEMORY_MMAP, true);
#elif CONFIG_IDF_TARGET_ESP32S3
    who::cam::Cam *cam = new who::cam::S3Cam(PIXFORMAT_RGB565, FRAMESIZE_240X240, 5, true);
#endif

    who::lcd::LCD *lcd = new who::lcd::LCD();
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

    who::cam::WhoCam *who_cam = new who::cam::WhoCam(cam);
    who::lcd::WhoLCD *who_lcd = new who::lcd::WhoLCD(lcd, cam, 1);
    who::app::WhoDetect *who_detect = new who::app::WhoDetect(detect, cam, who::lcd::FACE_DETECT_RESULT_TYPE);
    who_lcd->run();
    who_cam->run();
    who_detect->run();
#if CONFIG_PEDESTRIAN_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_unmount());
#endif
}
