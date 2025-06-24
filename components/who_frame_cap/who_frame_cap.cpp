#include "who_frame_cap.hpp"

static const char *TAG = "WhoFrameCap";

namespace who {
namespace frame_cap {
bool WhoFrameCap::run(std::vector<std::tuple<const configSTACK_DEPTH_TYPE, UBaseType_t, const BaseType_t>> args)
{
    bool ret = true;
    assert(args.size() == m_nodes.size());
    for (int i = 0; i < m_nodes.size(); i++) {
        ret &= m_nodes[i]->run(std::get<0>(args[i]), std::get<1>(args[i]), std::get<2>(args[i]));
    }
    return ret;
}

WhoFrameCapNode *WhoFrameCap::get_node(const std::string &name)
{
    auto it = std::find_if(
        m_nodes.begin(), m_nodes.end(), [&name](const auto &node) -> bool { return node->get_name() == name; });
    if (it != m_nodes.end()) {
        return *it;
    }
    ESP_LOGE(TAG, "Frame node %s not found.", name.c_str());
    return nullptr;
}

WhoFrameCapNode *WhoFrameCap::get_node(int i)
{
    if (i >= 0 && i < m_nodes.size()) {
        return m_nodes[i];
    }
    ESP_LOGE(TAG, "Invalid frame node index %d.", i);
    return nullptr;
}

WhoFrameCapNode *WhoFrameCap::get_last_node()
{
    if (m_nodes.empty()) {
        return nullptr;
    }
    return get_node(m_nodes.size() - 1);
}
} // namespace frame_cap
} // namespace who
