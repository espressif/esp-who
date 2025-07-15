#pragma once
#include "who_frame_cap_node.hpp"

namespace who {
namespace frame_cap {
class WhoFrameCap : public task::WhoTaskGroup {
public:
    ~WhoFrameCap()
    {
        WhoTaskGroup::destroy();
        for (auto queue : m_queues) {
            vQueueDelete(queue);
        }
    }

    template <typename T, typename... Args>
    void add_node(Args &&...args)
    {
        T *node = new T(std::forward<Args>(args)...);
        if (!m_nodes.empty()) {
            auto &prev_node = m_nodes.back();
            QueueHandle_t queue = xQueueCreate(1, sizeof(who::cam::cam_fb_t *));
            node->set_in_queue(queue);
            node->set_prev_node(prev_node);
            prev_node->set_out_queue(queue);
            prev_node->set_next_node(node);
            m_queues.emplace_back(queue);
        }
        m_nodes.emplace_back(node);
        WhoTaskGroup::register_task(node);
    }

    bool run(std::vector<std::tuple<const configSTACK_DEPTH_TYPE, UBaseType_t, const BaseType_t>> args);

    WhoFrameCapNode *get_node(const std::string &name);
    WhoFrameCapNode *get_node(int i);
    WhoFrameCapNode *get_last_node();
    std::vector<WhoFrameCapNode *> get_all_nodes() { return m_nodes; }

private:
    std::vector<WhoFrameCapNode *> m_nodes;
    std::vector<QueueHandle_t> m_queues;
};
} // namespace frame_cap
} // namespace who
