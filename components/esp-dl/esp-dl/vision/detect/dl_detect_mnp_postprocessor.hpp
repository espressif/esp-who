#pragma once
#include "dl_detect_postprocessor.hpp"

namespace dl {
namespace detect {
class MNPPostprocessor : public AnchorBoxDetectPostprocessor {
private:
    template <typename T>
    void parse_stage(TensorBase *score, TensorBase *box, TensorBase *landmark, const int stage_index);

public:
    void postprocess() override;
    using AnchorBoxDetectPostprocessor::AnchorBoxDetectPostprocessor;
};
} // namespace detect
} // namespace dl
