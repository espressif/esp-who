#include "spiflash_fatfs.hpp"
#include "who_cam_lcd.hpp"
#include "who_recognition.hpp"

using namespace who::app;
using namespace who::cam;
using namespace dl::detect;

extern "C" void app_main(void)
{
#if CONFIG_DB_FATFS_FLASH
    ESP_ERROR_CHECK(fatfs_flash_mount());
#endif
#if CONFIG_DB_SPIFFS
    ESP_ERROR_CHECK(bsp_spiffs_mount());
#endif
#if CONFIG_DB_FATFS_SDCARD || CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD || CONFIG_HUMAN_FACE_FEAT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_mount());
#endif
#if CONFIG_IDF_TARGET_ESP32P4
    auto cam = new P4Cam(VIDEO_PIX_FMT_RGB565, 5, V4L2_MEMORY_MMAP, true);
#elif CONFIG_IDF_TARGET_ESP32S3
    auto cam = new S3Cam(PIXFORMAT_RGB565, FRAMESIZE_240X240, 4, true);
#endif
    auto who_cam_lcd = new WhoCamLCD(cam);
#if CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD || CONFIG_HUMAN_FACE_FEAT_MODEL_IN_SDCARD
    char dir[64];
#if CONFIG_IDF_TARGET_ESP32P4
    snprintf(dir, sizeof(dir), "%s/espdl_models/p4", CONFIG_BSP_SD_MOUNT_POINT);
#elif CONFIG_IDF_TARGET_ESP32S3
    snprintf(dir, sizeof(dir), "%s/espdl_models/s3", CONFIG_BSP_SD_MOUNT_POINT);
#endif
#endif

#if !CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
    HumanFaceDetect *human_face_detect = new HumanFaceDetect();
#else
    HumanFaceDetect *human_face_detect = new HumanFaceDetect(dir);
#endif

#if !CONFIG_HUMAN_FACE_FEAT_MODEL_IN_SDCARD
    HumanFaceFeat *human_face_feat = new HumanFaceFeat();
#else
    HumanFaceFeat *human_face_feat = new HumanFaceFeat(dir);
#endif

    char db_path[64];
#if CONFIG_DB_FATFS_FLASH
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_SPIFLASH_MOUNT_POINT);
#elif CONFIG_DB_SPIFFS
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_BSP_SPIFFS_MOUNT_POINT);
#else
    snprintf(db_path, sizeof(db_path), "%s/face.db", CONFIG_BSP_SD_MOUNT_POINT);
#endif
    auto human_face_recognizer = new HumanFaceRecognizer(
        human_face_feat, db_path, static_cast<dl::recognition::db_type_t>(CONFIG_DB_FILE_SYSTEM));

    auto who_recognition = new WhoHumanFaceRecognition(human_face_detect, human_face_recognizer, cam);
    who_cam_lcd->run();
    who_recognition->run();
#if !CONFIG_DB_FATFS_SDCARD && (CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD || CONFIG_HUMAN_FACE_FEAT_MODEL_IN_SDCARD)
    ESP_ERROR_CHECK(bsp_sdcard_unmount());
#endif
}
