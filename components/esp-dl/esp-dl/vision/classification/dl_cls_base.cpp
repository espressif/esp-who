#include "dl_cls_base.hpp"

namespace dl {
namespace cls {

ClsImpl::~ClsImpl()
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

std::vector<dl::cls::result_t> &ClsImpl::run(const dl::image::img_t &img)
{
    dl::tool::Latency latency[3] = {dl::tool::Latency(), dl::tool::Latency(), dl::tool::Latency()};
    latency[0].start();
    m_image_preprocessor->preprocess(img);
    latency[0].end();

    latency[1].start();
    m_model->run();
    latency[1].end();

    latency[2].start();
    std::vector<dl::cls::result_t> &result = m_postprocessor->postprocess();
    latency[2].end();

    latency[0].print("cls", "preprocess");
    latency[1].print("cls", "forward");
    latency[2].print("cls", "postprocess");
    return result;
}

} // namespace cls
} // namespace dl
