#pragma once
#include "dl_detect_postprocessor.hpp"

namespace dl {
namespace detect {
class MSRPostprocessor : public AnchorBoxDetectPostprocessor {
private:
    template <typename T>
    void parse_stage(TensorBase *score, TensorBase *box, const int stage_index);

public:
    void postprocess() override;
    using AnchorBoxDetectPostprocessor::AnchorBoxDetectPostprocessor;
};
} // namespace detect
} // namespace dl
