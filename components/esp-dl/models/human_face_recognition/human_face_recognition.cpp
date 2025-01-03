#include "human_face_recognition.hpp"

#if CONFIG_HUMAN_FACE_FEAT_MODEL_IN_FLASH_RODATA
extern const uint8_t human_face_feat_espdl[] asm("_binary_human_face_feat_espdl_start");
static const char *path = (const char *)human_face_feat_espdl;
#elif CONFIG_HUMAN_FACE_FEAT_MODEL_IN_FLASH_PARTITION
static const char *path = "human_face_feat";
#endif
namespace human_face_recognition {

MFN::MFN(const char *model_name)
{
#if !CONFIG_HUMAN_FACE_FEAT_MODEL_IN_SDCARD
    m_model =
        new dl::Model(path, model_name, static_cast<fbs::model_location_type_t>(CONFIG_HUMAN_FACE_FEAT_MODEL_LOCATION));
#else
    m_model = new dl::Model(model_name, static_cast<fbs::model_location_type_t>(CONFIG_HUMAN_FACE_FEAT_MODEL_LOCATION));
#endif
#if CONFIG_IDF_TARGET_ESP32P4
    m_image_preprocessor = new dl::image::FeatImagePreprocessor(
        m_model, {127.5, 127.5, 127.5}, {127.5, 127.5, 127.5}, DL_IMAGE_CAP_RGB_SWAP | DL_IMAGE_CAP_RGB565_BIG_ENDIAN);
#elif CONFIG_IDF_TARGET_ESP32S3
    m_image_preprocessor = new dl::image::FeatImagePreprocessor(m_model, {127.5, 127.5, 127.5}, {127.5, 127.5, 127.5});
#endif
    m_postprocessor = new dl::feat::FeatPostprocessor(m_model);
}

} // namespace human_face_recognition

HumanFaceFeat::HumanFaceFeat(const char *sdcard_model_dir, model_type_t model_type)
{
    switch (model_type) {
    case model_type_t::MFN_S8_V1:
#if CONFIG_HUMAN_FACE_FEAT_MFN_S8_V1
#if !CONFIG_HUMAN_FACE_FEAT_MODEL_IN_SDCARD
        m_model = new human_face_recognition::MFN("human_face_feat_mfn_s8_v1.espdl");
#else
        if (sdcard_model_dir) {
            char mfn_dir[128];
            snprintf(mfn_dir, sizeof(mfn_dir), "%s/human_face_feat_mfn_s8_v1.espdl", sdcard_model_dir);
            m_model = new human_face_recognition::MFN(mfn_dir);
        } else {
            ESP_LOGE("human_face_recognition", "please pass sdcard mount point as parameter.");
        }
#endif
#else
        ESP_LOGE("human_face_feat", "human_face_feat_mfn_s8_v1 is not selected in menuconfig.");
#endif
        break;
    case model_type_t::MBF_S8_V1:
#if CONFIG_HUMAN_FACE_FEAT_MBF_S8_V1
#if !CONFIG_HUMAN_FACE_FEAT_MODEL_IN_SDCARD
        m_model = new human_face_recognition::MBF("human_face_feat_mbf_s8_v1.espdl");
#else
        if (sdcard_model_dir) {
            char mbf_dir[128];
            snprintf(mbf_dir, sizeof(mbf_dir), "%s/human_face_feat_mbf_s8_v1.espdl", sdcard_model_dir);
            m_model = new human_face_recognition::MBF(mbf_dir);
        } else {
            ESP_LOGE("human_face_recognition", "please pass sdcard mount point as parameter.");
        }
#endif
#else
        ESP_LOGE("human_face_feat", "human_face_feat_mbf_s8_v1 is not selected in menuconfig.");
#endif
        break;
    }
}

std::vector<dl::recognition::result_t> HumanFaceRecognizer::recognize(const dl::image::img_t &img,
                                                                      std::list<dl::detect::result_t> &detect_res)
{
    std::vector<std::vector<dl::recognition::result_t>> res;
    if (detect_res.empty()) {
        ESP_LOGW("HumanFaceRecognizer", "Failed to recognize. No face detected.");
        return {};
    } else if (detect_res.size() == 1) {
        auto feat = m_feat_extract->run(img, detect_res.back().keypoint);
        return query_feat(feat, m_thr, m_top_k);
    } else {
        auto max_detect_res =
            std::max_element(detect_res.begin(),
                             detect_res.end(),
                             [](const dl::detect::result_t &a, const dl::detect::result_t &b) -> bool {
                                 return a.box_area() > b.box_area();
                             });
        auto feat = m_feat_extract->run(img, max_detect_res->keypoint);
        return query_feat(feat, m_thr, m_top_k);
    }
}

esp_err_t HumanFaceRecognizer::enroll(const dl::image::img_t &img, std::list<dl::detect::result_t> &detect_res)
{
    if (detect_res.empty()) {
        ESP_LOGW("HumanFaceRecognizer", "Failed to enroll. No face detected.");
        return ESP_FAIL;
    } else if (detect_res.size() == 1) {
        auto feat = m_feat_extract->run(img, detect_res.back().keypoint);
        return enroll_feat(feat);
    } else {
        auto max_detect_res =
            std::max_element(detect_res.begin(),
                             detect_res.end(),
                             [](const dl::detect::result_t &a, const dl::detect::result_t &b) -> bool {
                                 return a.box_area() > b.box_area();
                             });
        auto feat = m_feat_extract->run(img, max_detect_res->keypoint);
        return enroll_feat(feat);
    }
}
