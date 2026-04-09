#pragma once
#include "video_capture.hpp"
#include "who_ringbuf.hpp"
#include "who_task.hpp"
#include "dl_image.hpp"

inline dl::image::pix_type_t v4l2_fmt2dl_pix_type(uint32_t v4l2_fmt) {
    dl::image::pix_type_t pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888;
    switch (v4l2_fmt) {
        case V4L2_PIX_FMT_RGB24:
            pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888;
            break;
        case V4L2_PIX_FMT_BGR32:
            pix_type = dl::image::DL_IMAGE_PIX_TYPE_BGR888;
            break;
        case V4L2_PIX_FMT_RGB565:
            pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB565LE;
            break;
        case V4L2_PIX_FMT_RGB565X:
            pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB565BE;
            break;
        case V4L2_PIX_FMT_YUYV:
            pix_type = dl::image::DL_IMAGE_PIX_TYPE_YUYV;
            break;
        case V4L2_PIX_FMT_UYVY:
            pix_type = dl::image::DL_IMAGE_PIX_TYPE_UYVY;
            break;
        case V4L2_PIX_FMT_GREY:
            pix_type = dl::image::DL_IMAGE_PIX_TYPE_GRAY;
            break;
        default:
            ESP_LOGE("v4l2_fmt2dl_pix_type", "Unsupported format.");
    }
    return pix_type;
}

inline uint32_t dl_pix_type2v4l2_fmt(dl::image::pix_type_t pix_type) {
    uint32_t v4l2_fmt = 0;
    switch (pix_type) {
        case dl::image::DL_IMAGE_PIX_TYPE_RGB888:
            v4l2_fmt = V4L2_PIX_FMT_RGB24;
            break;
        case dl::image::DL_IMAGE_PIX_TYPE_BGR888:
            v4l2_fmt = V4L2_PIX_FMT_BGR32;
            break;
        case dl::image::DL_IMAGE_PIX_TYPE_RGB565LE:
            v4l2_fmt = V4L2_PIX_FMT_RGB565;
            break;
        case dl::image::DL_IMAGE_PIX_TYPE_RGB565BE:
            v4l2_fmt = V4L2_PIX_FMT_RGB565X;
            break;
        case dl::image::DL_IMAGE_PIX_TYPE_YUYV:
            v4l2_fmt = V4L2_PIX_FMT_YUYV;
            break;
        case dl::image::DL_IMAGE_PIX_TYPE_UYVY:
            v4l2_fmt = V4L2_PIX_FMT_UYVY;
            break;
        case dl::image::DL_IMAGE_PIX_TYPE_GRAY:
            v4l2_fmt = V4L2_PIX_FMT_GREY;
            break;
        default:
            ESP_LOGE("dl_pix_type2v4l2_fmt", "Unsupported format.");
    }
    return v4l2_fmt;
}

inline dl::image::img_t frame2img(VideoCapture::Frame *frame) {
    dl::image::pix_type_t pix_type = v4l2_fmt2dl_pix_type(frame->pixel_format);
    return dl::image::img_t {frame->data, (uint16_t) frame->width, (uint16_t) frame->height, pix_type};
}

namespace who {
namespace frame_cap {
class WhoFrameCapNode : public task::WhoTask {
public:
    static inline constexpr EventBits_t NEW_FRAME = TASK_EVENT_BIT_LAST;

    WhoFrameCapNode(const std::string &name, uint8_t ringbuf_len, bool out_queue_overwrite = true);
    ~WhoFrameCapNode();
    bool stop_async() override;
    bool pause_async() override;
    void set_in_queue(QueueHandle_t in_queue) { m_in_queue = in_queue; }
    void set_out_queue(QueueHandle_t out_queue) { m_out_queue = out_queue; }
    void set_prev_node(WhoFrameCapNode *node) { m_prev_node = node; }
    void set_next_node(WhoFrameCapNode *node) { m_next_node = node; }
    VideoCapture::Frame *cam_fb_peek(int index = -1);
    void add_new_frame_signal_subscriber(task::WhoTask *task);
    WhoFrameCapNode *get_prev_node();
    WhoFrameCapNode *get_next_node();
    virtual uint16_t get_fb_width() = 0;
    virtual uint16_t get_fb_height() = 0;
    virtual std::string get_type() = 0;

private:
    void task() override;
    virtual VideoCapture::Frame *process(VideoCapture::Frame *fb) = 0;
    virtual void update_ringbuf(VideoCapture::Frame *fb) = 0;
    bool m_out_queue_overwrite;
    QueueHandle_t m_out_queue;
    WhoFrameCapNode *m_prev_node;
    WhoFrameCapNode *m_next_node;
    std::vector<task::WhoTask *> m_tasks;

protected:
    QueueHandle_t m_in_queue;
    RingBuf<VideoCapture::Frame *> m_cam_fbs;
    SemaphoreHandle_t m_mutex;
};

class WhoFetchNode : public WhoFrameCapNode {
public:
    WhoFetchNode(const std::string &name, VideoCapture *cap, bool out_queue_overwrite = true) :
        WhoFrameCapNode(name, cap->get_buffer_count() - 2, out_queue_overwrite), m_cap(cap)
    {
    }
    ~WhoFetchNode();
    uint16_t get_fb_width() override { return m_cap->get_width(); }
    uint16_t get_fb_height() override { return m_cap->get_height(); }
    std::string get_type() override { return "FetchNode"; }

private:
    void cleanup() override;
    VideoCapture::Frame *process(VideoCapture::Frame *fb) override;
    void update_ringbuf(VideoCapture::Frame *fb) override;
    VideoCapture *m_cap;
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
    VideoCapture::Frame *process(VideoCapture::Frame *fb) override;
    void update_ringbuf(VideoCapture::Frame *fb) override;
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
    VideoCapture::Frame *process(VideoCapture::Frame *fb) override;
    void update_ringbuf(VideoCapture::Frame *fb) override;
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
