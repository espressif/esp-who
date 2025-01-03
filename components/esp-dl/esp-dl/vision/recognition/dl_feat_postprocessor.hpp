#pragma once
#include "dl_math.hpp"
#include "dl_model_base.hpp"
#include "dl_tensor_base.hpp"
#include <map>

namespace dl {
namespace feat {
class FeatPostprocessor {
private:
    TensorBase *m_model_output;
    TensorBase *m_feat;
    void l2_norm();

public:
    FeatPostprocessor(Model *model, const std::string &output_name = "");
    TensorBase *postprocess();
    ~FeatPostprocessor()
    {
        if (m_feat) {
            delete m_feat;
            m_feat = nullptr;
        }
    }
};
} // namespace feat
} // namespace dl
