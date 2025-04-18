#pragma once
#include "who_detect_base.hpp"

namespace who {
namespace detect {
class WhoDetectTerm : public WhoDetectBase {
public:
    using WhoDetectBase::WhoDetectBase;

private:
    void on_new_detect_result(const result_t &result) override;
};
} // namespace detect
} // namespace who
