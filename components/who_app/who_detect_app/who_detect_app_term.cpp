#include "who_detect_app_term.hpp"
#include "who_detect_result_handle.hpp"
#include "who_yield2idle.hpp"

namespace who {
namespace app {
WhoDetectAppTerm::WhoDetectAppTerm(frame_cap::WhoFrameCap *frame_cap) : WhoDetectAppBase(frame_cap)
{
    m_detect->set_detect_result_cb(std::bind(&WhoDetectAppTerm::detect_result_cb, this, std::placeholders::_1));
}

bool WhoDetectAppTerm::run()
{
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_detect->run(2560, 2, 1);
    return ret;
}

void WhoDetectAppTerm::detect_result_cb(const detect::WhoDetect::result_t &result)
{
    detect::print_detect_results(result.det_res);
}
} // namespace app
} // namespace who
