#include "human_face_detect.hpp"

#if CONFIG_HUMAN_FACE_DETECT_MODEL_IN_FLASH_RODATA
extern const uint8_t human_face_detect_espdl[] asm("_binary_human_face_detect_espdl_start");
static const char *path = (const char *)human_face_detect_espdl;
#elif CONFIG_HUMAN_FACE_DETECT_MODEL_IN_FLASH_PARTITION
static const char *path = "human_face_det";
#endif
namespace human_face_detect {

MSR::MSR(const char *model_name)
{
#if !CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
    m_model = new dl::Model(
        path, model_name, static_cast<fbs::model_location_type_t>(CONFIG_HUMAN_FACE_DETECT_MODEL_LOCATION));
#else
    m_model =
        new dl::Model(model_name, static_cast<fbs::model_location_type_t>(CONFIG_HUMAN_FACE_DETECT_MODEL_LOCATION));
#endif
#if CONFIG_IDF_TARGET_ESP32P4
    m_image_preprocessor = new dl::image::ImagePreprocessor(
        m_model, {0, 0, 0}, {1, 1, 1}, DL_IMAGE_CAP_RGB_SWAP | DL_IMAGE_CAP_RGB565_BIG_ENDIAN);
#else
    m_image_preprocessor = new dl::image::ImagePreprocessor(m_model, {0, 0, 0}, {1, 1, 1}, DL_IMAGE_CAP_RGB_SWAP);
#endif
    m_postprocessor = new dl::detect::MSRPostprocessor(
        m_model, 0.5, 0.5, 10, {{8, 8, 9, 9, {{16, 16}, {32, 32}}}, {16, 16, 9, 9, {{64, 64}, {128, 128}}}});
}

MNP::MNP(const char *model_name)
{
#if !CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
    m_model = new dl::Model(
        path, model_name, static_cast<fbs::model_location_type_t>(CONFIG_HUMAN_FACE_DETECT_MODEL_LOCATION));
#else
    m_model =
        new dl::Model(model_name, static_cast<fbs::model_location_type_t>(CONFIG_HUMAN_FACE_DETECT_MODEL_LOCATION));
#endif
#if CONFIG_IDF_TARGET_ESP32P4
    m_image_preprocessor = new dl::image::ImagePreprocessor(
        m_model, {0, 0, 0}, {1, 1, 1}, DL_IMAGE_CAP_RGB_SWAP | DL_IMAGE_CAP_RGB565_BIG_ENDIAN);
#else
    m_image_preprocessor = new dl::image::ImagePreprocessor(m_model, {0, 0, 0}, {1, 1, 1}, DL_IMAGE_CAP_RGB_SWAP);
#endif
    m_postprocessor = new dl::detect::MNPPostprocessor(m_model, 0.5, 0.5, 10, {{1, 1, 0, 0, {{48, 48}}}});
}

MNP::~MNP()
{
    if (m_model) {
        delete m_model;
        m_model = nullptr;
    }
    if (m_image_preprocessor) {
        delete m_image_preprocessor;
        m_image_preprocessor = nullptr;
    }
    if (m_postprocessor) {
        delete m_postprocessor;
        m_postprocessor = nullptr;
    }
};

std::list<dl::detect::result_t> &MNP::run(const dl::image::img_t &img, std::list<dl::detect::result_t> &candidates)
{
    dl::tool::Latency latency[3] = {dl::tool::Latency(10), dl::tool::Latency(10), dl::tool::Latency(10)};
    m_postprocessor->clear_result();
    for (auto &candidate : candidates) {
        int center_x = (candidate.box[0] + candidate.box[2]) >> 1;
        int center_y = (candidate.box[1] + candidate.box[3]) >> 1;
        int side = DL_MAX(candidate.box[2] - candidate.box[0], candidate.box[3] - candidate.box[1]);
        candidate.box[0] = center_x - (side >> 1);
        candidate.box[1] = center_y - (side >> 1);
        candidate.box[2] = candidate.box[0] + side;
        candidate.box[3] = candidate.box[1] + side;
        candidate.limit_box(img.width, img.height);

        latency[0].start();
        m_image_preprocessor->preprocess(img, candidate.box);
        latency[0].end();

        latency[1].start();
        m_model->run();
        latency[1].end();

        latency[2].start();
        m_postprocessor->set_resize_scale_x(m_image_preprocessor->get_resize_scale_x());
        m_postprocessor->set_resize_scale_y(m_image_preprocessor->get_resize_scale_y());
        m_postprocessor->set_top_left_x(m_image_preprocessor->get_top_left_x());
        m_postprocessor->set_top_left_y(m_image_preprocessor->get_top_left_y());
        m_postprocessor->postprocess();
        latency[2].end();
    }
    m_postprocessor->nms();
    std::list<dl::detect::result_t> &result = m_postprocessor->get_result(img.width, img.height);
    if (candidates.size() > 0) {
        latency[0].print("detect", "preprocess");
        latency[1].print("detect", "forward");
        latency[2].print("detect", "postprocess");
    }
    return result;
}

MSRMNP::~MSRMNP()
{
    if (m_msr) {
        delete m_msr;
        m_msr = nullptr;
    }
    if (m_mnp) {
        delete m_mnp;
        m_mnp = nullptr;
    }
}

std::list<dl::detect::result_t> &MSRMNP::run(const dl::image::img_t &img)
{
    std::list<dl::detect::result_t> &candidates = m_msr->run(img);
    return m_mnp->run(img, candidates);
}

} // namespace human_face_detect

HumanFaceDetect::HumanFaceDetect(const char *sdcard_model_dir, model_type_t model_type)
{
    switch (model_type) {
    case model_type_t::MSRMNP_S8_V1: {
#if CONFIG_HUMAN_FACE_DETECT_MSRMNP_S8_V1
#if !CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
        m_model =
            new human_face_detect::MSRMNP("human_face_detect_msr_s8_v1.espdl", "human_face_detect_mnp_s8_v1.espdl");
#else
        if (sdcard_model_dir) {
            char msr_dir[128];
            snprintf(msr_dir, sizeof(msr_dir), "%s/human_face_detect_msr_s8_v1.espdl", sdcard_model_dir);
            char mnp_dir[128];
            snprintf(mnp_dir, sizeof(mnp_dir), "%s/human_face_detect_mnp_s8_v1.espdl", sdcard_model_dir);
            m_model = new human_face_detect::MSRMNP(msr_dir, mnp_dir);
        } else {
            ESP_LOGE("human_face_detect", "please pass sdcard mount point as parameter.");
        }
#endif
#else
        ESP_LOGE("human_face_detect", "human_face_detect_msrmnp_s8_v1 is not selected in menuconfig.");
#endif
        break;
    }
    }
}
