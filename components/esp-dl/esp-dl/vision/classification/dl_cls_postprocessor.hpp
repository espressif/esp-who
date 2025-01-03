#pragma once
#include "dl_cls_define.hpp"
#include "dl_model_base.hpp"
#include "dl_module_softmax.hpp"
#include "dl_tensor_base.hpp"
#include <vector>

namespace dl {
namespace cls {
class ClsPostprocessor {
public:
    ClsPostprocessor(
        Model *model, const int top_k, const float score_thr, bool need_softmax, const std::string &output_name);
    virtual ~ClsPostprocessor();
    virtual std::vector<dl::cls::result_t> &postprocess();

protected:
    const char **m_cat_names;

private:
    TensorBase *m_model_output;
    int m_top_k;
    float m_score_thr;
    bool m_need_softmax;
    dl::module::Softmax *m_softmax_module;
    TensorBase *m_output;
    std::vector<result_t> m_cls_result;
};
} // namespace cls
} // namespace dl
