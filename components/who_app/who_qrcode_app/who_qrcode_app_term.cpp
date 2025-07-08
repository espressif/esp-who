#include "who_qrcode_app_term.hpp"
#include "who_yield2idle.hpp"

namespace who {
namespace app {
WhoQRCodeAppTerm::WhoQRCodeAppTerm(frame_cap::WhoFrameCap *frame_cap) : WhoQRCodeAppBase(frame_cap)
{
    m_qrcode->set_qrcode_result_cb(std::bind(&WhoQRCodeAppTerm::qrcode_result_cb, this, std::placeholders::_1));
}

bool WhoQRCodeAppTerm::run()
{
    bool ret = WhoYield2Idle::get_instance()->run();
    for (const auto &frame_cap_node : m_frame_cap->get_all_nodes()) {
        ret &= frame_cap_node->run(4096, 2, 0);
    }
    ret &= m_qrcode->run(25600, 2, 1);
    return ret;
}

void WhoQRCodeAppTerm::qrcode_result_cb(const std::string &result)
{
    ESP_LOGI("QRCode", "%s", result.c_str());
}
} // namespace app
} // namespace who
