#include "dl_feat_base.hpp"

namespace dl {
namespace feat {

FeatImpl::~FeatImpl()
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
}

TensorBase *FeatImpl::run(const dl::image::img_t &img, const std::vector<int> &landmarks)
{
    dl::tool::Latency latency[3] = {dl::tool::Latency(), dl::tool::Latency(), dl::tool::Latency()};
    latency[0].start();
    m_image_preprocessor->preprocess(img, landmarks);
    latency[0].end();

    latency[1].start();
    m_model->run();
    latency[1].end();

    latency[2].start();
    dl::TensorBase *feat = m_postprocessor->postprocess();
    latency[2].end();

    latency[0].print("feat", "preprocess");
    latency[1].print("feat", "forward");
    latency[2].print("feat", "postprocess");
    return feat;
}

} // namespace feat
} // namespace dl
