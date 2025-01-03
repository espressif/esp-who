#include "dl_cls_postprocessor.hpp"

namespace dl {
namespace cls {
ClsPostprocessor::ClsPostprocessor(
    Model *model, const int top_k, const float score_thr, bool need_softmax, const std::string &output_name) :
    m_top_k(top_k), m_score_thr(score_thr), m_need_softmax(need_softmax)
{
    if (output_name.empty()) {
        std::map<std::string, dl::TensorBase *> model_outputs_map = model->get_outputs();
        assert(model_outputs_map.size() == 1);
        m_model_output = model_outputs_map.begin()->second;
    } else {
        m_model_output = model->get_intermediate(output_name);
    }
    m_output = new dl::TensorBase(m_model_output->shape, nullptr, 0, dl::DATA_TYPE_FLOAT);
    if (need_softmax) {
        m_softmax_module = new dl::module::Softmax(nullptr, -1, dl::MODULE_NON_INPLACE, dl::QUANT_TYPE_SYMM_8BIT);
    }
}

ClsPostprocessor::~ClsPostprocessor()
{
    if (m_output) {
        delete m_output;
        m_output = nullptr;
    }
    if (m_need_softmax && m_softmax_module) {
        delete m_softmax_module;
        m_softmax_module = nullptr;
    }
}

std::vector<dl::cls::result_t> &ClsPostprocessor::postprocess()
{
    if (m_need_softmax) {
        m_softmax_module->run(m_model_output, m_output);
    } else {
        m_output->assign(m_model_output);
    }

    m_cls_result.clear();
    float *output_ptr = (float *)m_output->data;
    for (int i = 0; i < m_output->get_size(); i++) {
        if (*output_ptr > m_score_thr) {
            m_cls_result.emplace_back(m_cat_names[i], *output_ptr);
        }
        output_ptr++;
    }
    std::sort(
        m_cls_result.begin(), m_cls_result.end(), [](result_t &a, result_t &b) -> bool { return a.score > b.score; });
    if (m_cls_result.size() > m_top_k) {
        m_cls_result.resize(m_top_k);
    }
    return m_cls_result;
}

} // namespace cls
} // namespace dl
