#pragma once
#include "who_detect_base.hpp"

namespace who {
namespace detect {
class WhoDetectTerminal : public WhoDetectBase {
public:
    WhoDetectTerminal(frame_cap::WhoFrameCap *frame_cap, const std::string &name) :
        WhoDetectTerminal(frame_cap, nullptr, name)
    {
    }
    WhoDetectTerminal(frame_cap::WhoFrameCap *frame_cap, dl::detect::Detect *detect, const std::string &name) :
        WhoDetectBase(frame_cap, detect, name) {};

private:
    void on_new_detect_result(const result_t &result) override;
};
} // namespace detect
} // namespace who
