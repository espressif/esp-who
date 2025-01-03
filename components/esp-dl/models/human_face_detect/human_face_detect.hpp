#pragma once

#include "dl_detect_base.hpp"
#include "dl_detect_mnp_postprocessor.hpp"
#include "dl_detect_msr_postprocessor.hpp"
namespace human_face_detect {
class MSR : public dl::detect::DetectImpl {
public:
    MSR(const char *model_name);
};

class MNP {
private:
    dl::Model *m_model;
    dl::image::ImagePreprocessor *m_image_preprocessor;
    dl::detect::MNPPostprocessor *m_postprocessor;

public:
    MNP(const char *model_name);
    ~MNP();
    std::list<dl::detect::result_t> &run(const dl::image::img_t &img, std::list<dl::detect::result_t> &candidates);
};

class MSRMNP : public dl::detect::Detect {
private:
    MSR *m_msr;
    MNP *m_mnp;

public:
    MSRMNP(const char *msr_model_name, const char *mnp_model_name) :
        m_msr(new MSR(msr_model_name)), m_mnp(new MNP(mnp_model_name)) {};
    ~MSRMNP();
    std::list<dl::detect::result_t> &run(const dl::image::img_t &img) override;
};

} // namespace human_face_detect

class HumanFaceDetect : public dl::detect::DetectWrapper {
public:
    typedef enum { MSRMNP_S8_V1 } model_type_t;
    HumanFaceDetect(const char *sdcard_model_dir = nullptr,
                    model_type_t model_type = static_cast<model_type_t>(CONFIG_HUMAN_FACE_DETECT_MODEL_TYPE));
};
