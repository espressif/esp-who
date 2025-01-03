#include "dl_detect_base.hpp"

namespace dl {
namespace detect {

DetectImpl::~DetectImpl()
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

std::list<dl::detect::result_t> &DetectImpl::run(const dl::image::img_t &img)
{
    dl::tool::Latency latency[3] = {dl::tool::Latency(), dl::tool::Latency(), dl::tool::Latency()};
    latency[0].start();
    m_image_preprocessor->preprocess(img);
    latency[0].end();

    latency[1].start();
    m_model->run();
    latency[1].end();

    latency[2].start();
    m_postprocessor->clear_result();
    m_postprocessor->set_resize_scale_x(m_image_preprocessor->get_resize_scale_x());
    m_postprocessor->set_resize_scale_y(m_image_preprocessor->get_resize_scale_y());
    m_postprocessor->postprocess();
    std::list<dl::detect::result_t> &result = m_postprocessor->get_result(img.width, img.height);
    latency[2].end();

    latency[0].print("detect", "preprocess");
    latency[1].print("detect", "forward");
    latency[2].print("detect", "postprocess");

    return result;
}

} // namespace detect
} // namespace dl
