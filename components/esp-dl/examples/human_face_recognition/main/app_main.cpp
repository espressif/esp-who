#include "human_face_detect.hpp"
#include "human_face_recognition.hpp"
#include "spiflash_fatfs.hpp"
#include "bsp/esp-bsp.h"

extern const uint8_t bill1_jpg_start[] asm("_binary_bill1_jpg_start");
extern const uint8_t bill1_jpg_end[] asm("_binary_bill1_jpg_end");
extern const uint8_t bill2_jpg_start[] asm("_binary_bill2_jpg_start");
extern const uint8_t bill2_jpg_end[] asm("_binary_bill2_jpg_end");
extern const uint8_t musk1_jpg_start[] asm("_binary_musk1_jpg_start");
extern const uint8_t musk1_jpg_end[] asm("_binary_musk1_jpg_end");
extern const uint8_t musk2_jpg_start[] asm("_binary_musk2_jpg_start");
extern const uint8_t musk2_jpg_end[] asm("_binary_musk2_jpg_end");
const char *TAG = "human_face_recognition";

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

    dl::image::jpeg_img_t bill1_jpeg = {.data = (uint8_t *)bill1_jpg_start,
                                        .width = 300,
                                        .height = 300,
                                        .data_size = (uint32_t)(bill1_jpg_end - bill1_jpg_start)};
    dl::image::img_t bill1;
    bill1.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888;
    sw_decode_jpeg(bill1_jpeg, bill1, true);

    dl::image::jpeg_img_t bill2_jpeg = {.data = (uint8_t *)bill2_jpg_start,
                                        .width = 300,
                                        .height = 300,
                                        .data_size = (uint32_t)(bill2_jpg_end - bill2_jpg_start)};
    dl::image::img_t bill2;
    bill2.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888;
    sw_decode_jpeg(bill2_jpeg, bill2, true);

    dl::image::jpeg_img_t musk1_jpeg = {.data = (uint8_t *)musk1_jpg_start,
                                        .width = 300,
                                        .height = 300,
                                        .data_size = (uint32_t)(musk1_jpg_end - musk1_jpg_start)};
    dl::image::img_t musk1;
    musk1.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888;
    sw_decode_jpeg(musk1_jpeg, musk1, true);

    dl::image::jpeg_img_t musk2_jpeg = {.data = (uint8_t *)musk2_jpg_start,
                                        .width = 300,
                                        .height = 300,
                                        .data_size = (uint32_t)(musk2_jpg_end - musk2_jpg_start)};
    dl::image::img_t musk2;
    musk2.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888;
    sw_decode_jpeg(musk2_jpeg, musk2, true);

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

    human_face_recognizer->enroll(bill1, human_face_detect->run(bill1));
    human_face_recognizer->enroll(bill2, human_face_detect->run(bill2));
    human_face_recognizer->enroll(musk1, human_face_detect->run(musk1));

    auto res = human_face_recognizer->recognize(musk2, human_face_detect->run(musk2));
    for (const auto &k : res) {
        printf("id: %d, sim: %f\n", k.id, k.similarity);
    }

    human_face_recognizer->clear_all_feats();

    delete human_face_detect;
    delete human_face_feat;
    delete human_face_recognizer;

    heap_caps_free(bill1.data);
    heap_caps_free(bill2.data);
    heap_caps_free(musk1.data);
    heap_caps_free(musk2.data);

#if CONFIG_DB_FATFS_FLASH
    ESP_ERROR_CHECK(fatfs_flash_unmount());
#endif
#if CONFIG_DB_SPIFFS
    ESP_ERROR_CHECK(bsp_spiffs_unmount());
#endif
#if CONFIG_DB_FATFS_SDCARD || CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD || CONFIG_HUMAN_FACE_FEAT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_unmount());
#endif
}
