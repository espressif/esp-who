#pragma once
#include "who_cam_base.hpp"
#include "who_ringbuf.hpp"
#include "who_task.hpp"

namespace who {
namespace frame_cap {
class WhoFrameCapNode : public WhoTask {
public:
    WhoFrameCapNode(const std::string &name, uint8_t ringbuf_len, bool out_queue_overwrite = true);
    ~WhoFrameCapNode();
    bool stop_async() override;
    bool pause_async() override;
    void set_in_queue(QueueHandle_t in_queue) { m_in_queue = in_queue; }
    void set_out_queue(QueueHandle_t out_queue) { m_out_queue = out_queue; }
    void set_prev_node(WhoFrameCapNode *node) { m_prev_node = node; }
    void set_next_node(WhoFrameCapNode *node) { m_next_node = node; }
    who::cam::cam_fb_t *cam_fb_peek(int index = -1);
    void add_new_frame_signal_subscriber(WhoTask *task);
    WhoFrameCapNode *get_prev_node();
    WhoFrameCapNode *get_next_node();
    virtual uint16_t get_fb_width() = 0;
    virtual uint16_t get_fb_height() = 0;
    virtual std::string get_type() = 0;

private:
    void task() override;
    virtual who::cam::cam_fb_t *process(who::cam::cam_fb_t *fb) = 0;
    virtual void update_ringbuf(who::cam::cam_fb_t *fb) = 0;
    bool m_out_queue_overwrite;
    QueueHandle_t m_out_queue;
    WhoFrameCapNode *m_prev_node;
    WhoFrameCapNode *m_next_node;
    std::vector<WhoTask *> m_tasks;

protected:
    QueueHandle_t m_in_queue;
    RingBuf<who::cam::cam_fb_t *> m_cam_fbs;
    SemaphoreHandle_t m_mutex;
};

class WhoFetchNode : public WhoFrameCapNode {
public:
    WhoFetchNode(const std::string &name, who::cam::WhoCam *cam, bool out_queue_overwrite = true) :
        WhoFrameCapNode(name, cam->get_fb_count() - 2, out_queue_overwrite), m_cam(cam)
    {
    }
    ~WhoFetchNode();
    uint16_t get_fb_width() override { return m_cam->get_fb_width(); }
    uint16_t get_fb_height() override { return m_cam->get_fb_height(); }
    std::string get_type() override { return "FetchNode"; }

private:
    void cleanup() override;
    who::cam::cam_fb_t *process(who::cam::cam_fb_t *fb) override;
    void update_ringbuf(who::cam::cam_fb_t *fb) override;
    who::cam::WhoCam *m_cam;
};

class WhoDecodeNode : public WhoFrameCapNode {
public:
    WhoDecodeNode(const std::string &name,
                  dl::image::pix_type_t pix_type,
                  uint8_t ringbuf_len,
                  bool out_queue_overwrite = true) :
        WhoFrameCapNode(name, ringbuf_len, out_queue_overwrite), m_pix_type(pix_type)
    {
    }
    uint16_t get_fb_width() override { return get_prev_node()->get_fb_width(); }
    uint16_t get_fb_height() override { return get_prev_node()->get_fb_height(); }
    std::string get_type() override { return "DecodeNode"; }

private:
    void cleanup() override;
    who::cam::cam_fb_t *process(who::cam::cam_fb_t *fb) override;
    void update_ringbuf(who::cam::cam_fb_t *fb) override;
    dl::image::pix_type_t m_pix_type;
};

#if CONFIG_SOC_PPA_SUPPORTED
class WhoPPAResizeNode : public WhoFrameCapNode {
public:
    WhoPPAResizeNode(const std::string &name,
                     uint16_t dst_w,
                     uint16_t dst_h,
                     dl::image::pix_type_t dst_pix_type,
                     uint8_t ringbuf_len,
                     bool out_queue_overwrite = true);
    ~WhoPPAResizeNode();
    uint16_t get_fb_width() override { return m_dst_w; }
    uint16_t get_fb_height() override { return m_dst_h; }
    std::string get_type() override { return "PPAResizeNode"; }

private:
    void cleanup() override;
    who::cam::cam_fb_t *process(who::cam::cam_fb_t *fb) override;
    void update_ringbuf(who::cam::cam_fb_t *fb) override;
    dl::image::img_t get_dst_img();

    uint16_t m_dst_w;
    uint16_t m_dst_h;
    ppa_client_handle_t m_ppa_srm_handle;
    std::vector<dl::image::img_t> m_dst_imgs;
    size_t m_buf_size;
    int m_img_idx;
};
#endif
} // namespace frame_cap
} // namespace who
