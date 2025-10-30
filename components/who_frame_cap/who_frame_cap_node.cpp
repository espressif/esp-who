#include "who_frame_cap_node.hpp"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"

using namespace who::cam;
static const char *TAG = "WhoFrameCapNode";

namespace who {
namespace frame_cap {
WhoFrameCapNode::WhoFrameCapNode(const std::string &name, uint8_t ringbuf_len, bool out_queue_overwrite) :
    task::WhoTask(name),
    m_out_queue_overwrite(out_queue_overwrite),
    m_out_queue(nullptr),
    m_prev_node(nullptr),
    m_next_node(nullptr),
    m_in_queue(nullptr),
    m_cam_fbs(ringbuf_len),
    m_mutex(xSemaphoreCreateMutex())
{
    // Ensure at least one element in ringbuf.
    assert(ringbuf_len >= 1);
}

WhoFrameCapNode::~WhoFrameCapNode()
{
    vSemaphoreDelete(m_mutex);
}

bool WhoFrameCapNode::pause_async()
{
    if (task::WhoTask::pause_async()) {
        cam_fb_t *fb = nullptr;
        if (m_in_queue) {
            xQueueSend(m_in_queue, &fb, 0);
        }
        return true;
    }
    return false;
}

bool WhoFrameCapNode::stop_async()
{
    if (task::WhoTask::stop_async()) {
        cam_fb_t *fb = nullptr;
        if (m_in_queue) {
            xQueueSend(m_in_queue, &fb, 0);
        }
        return true;
    }
    return false;
}

cam_fb_t *WhoFrameCapNode::cam_fb_peek(int index)
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    if (m_cam_fbs.empty()) {
        ESP_LOGW(TAG, "%s: Unable to peek from an empty frame buffer.", get_name().c_str());
        xSemaphoreGive(m_mutex);
        return nullptr;
    }
    if (index == -1) {
        index = m_cam_fbs.size() - 1;
    }
    if (index < 0 || index > m_cam_fbs.size() - 1) {
        ESP_LOGW(TAG,
                 "%s: Invalid index %d, valid index should be [-1, %d].",
                 get_name().c_str(),
                 index,
                 m_cam_fbs.size() - 1);
        xSemaphoreGive(m_mutex);
        return nullptr;
    }
    cam_fb_t *ret = m_cam_fbs[index];
    xSemaphoreGive(m_mutex);
    return ret;
}

void WhoFrameCapNode::add_new_frame_signal_subscriber(task::WhoTask *task)
{
    m_tasks.emplace_back(task);
}

WhoFrameCapNode *WhoFrameCapNode::get_prev_node()
{
    if (!m_prev_node) {
        ESP_LOGE(TAG, "No prev node.");
    }
    return m_prev_node;
}

WhoFrameCapNode *WhoFrameCapNode::get_next_node()
{
    if (!m_next_node) {
        ESP_LOGE(TAG, "No next node.");
    }
    return m_next_node;
}

void WhoFrameCapNode::task()
{
    while (true) {
        cam_fb_t *in_fb = nullptr;
        if (m_in_queue) {
            xQueueReceive(m_in_queue, &in_fb, portMAX_DELAY);
        }
        EventBits_t event_bits = xEventGroupWaitBits(m_event_group, TASK_PAUSE | TASK_STOP, pdTRUE, pdFALSE, 0);
        if (event_bits & TASK_STOP) {
            break;
        } else if (event_bits & TASK_PAUSE) {
            xEventGroupSetBits(m_event_group, TASK_PAUSED);
            EventBits_t pause_event_bits =
                xEventGroupWaitBits(m_event_group, TASK_RESUME | TASK_STOP, pdTRUE, pdFALSE, portMAX_DELAY);
            if (pause_event_bits & TASK_STOP) {
                break;
            } else {
                continue;
            }
        }
        cam_fb_t *out_fb = process(in_fb);
        // Drop the fb which failed to process.
        if (!out_fb) {
            continue;
        }
        if (m_out_queue) {
            if (m_out_queue_overwrite) {
                xQueueOverwrite(m_out_queue, &out_fb);
            } else {
                xQueueSend(m_out_queue, &out_fb, portMAX_DELAY);
            }
        }
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        update_ringbuf(out_fb);
        bool full_ringbuf = m_cam_fbs.full();
        xSemaphoreGive(m_mutex);
        if (full_ringbuf) {
            for (const auto &task : m_tasks) {
                if (task->is_active()) {
                    xEventGroupSetBits(task->get_event_group(), NEW_FRAME);
                }
            }
        }
    }
    xEventGroupSetBits(m_event_group, TASK_STOPPED);
    vTaskDelete(NULL);
}

WhoFetchNode::~WhoFetchNode()
{
    delete m_cam;
}

void WhoFetchNode::cleanup()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    while (!m_cam_fbs.empty()) {
        auto fb = m_cam_fbs.pop();
        m_cam->cam_fb_return(fb);
    }
    xSemaphoreGive(m_mutex);
}

cam_fb_t *WhoFetchNode::process(who::cam::cam_fb_t *fb)
{
    return m_cam->cam_fb_get();
}

void WhoFetchNode::update_ringbuf(who::cam::cam_fb_t *fb)
{
    if (m_cam_fbs.full()) {
        auto fb_prev = m_cam_fbs.pop();
        m_cam->cam_fb_return(fb_prev);
    }
    m_cam_fbs.push(fb);
}

void WhoDecodeNode::cleanup()
{
    while (uxQueueMessagesWaiting(m_in_queue) > 0) {
        cam_fb_t *tmp;
        xQueueReceive(m_in_queue, &tmp, 0);
    }
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    while (!m_cam_fbs.empty()) {
        auto fb = m_cam_fbs.pop();
        heap_caps_free(fb->buf);
        delete fb;
    }
    xSemaphoreGive(m_mutex);
}

cam_fb_t *WhoDecodeNode::process(who::cam::cam_fb_t *fb)
{
    auto timestamp = fb->timestamp;
#if CONFIG_IDF_TARGET_ESP32P4
    uint32_t caps = 0;
#else
    uint32_t caps = dl::image::DL_IMAGE_CAP_RGB565_BIG_ENDIAN;
#endif
#if CONFIG_SOC_JPEG_CODEC_SUPPORTED
    auto img = hw_decode_jpeg({fb->buf, fb->len}, m_pix_type, caps);
#else
    auto img = sw_decode_jpeg({fb->buf, fb->len}, m_pix_type, caps);
#endif
    // Sometimes may fail to decode a corrupted frame.
    if (!img.data) {
        return nullptr;
    }
    return new cam_fb_t(img, timestamp);
}

void WhoDecodeNode::update_ringbuf(who::cam::cam_fb_t *fb)
{
    if (m_cam_fbs.full()) {
        auto fb_prev = m_cam_fbs.pop();
        heap_caps_free(fb_prev->buf);
        delete fb_prev;
    }
    m_cam_fbs.push(fb);
}

#if CONFIG_SOC_PPA_SUPPORTED
WhoPPAResizeNode::WhoPPAResizeNode(const std::string &name,
                                   uint16_t dst_w,
                                   uint16_t dst_h,
                                   dl::image::pix_type_t dst_pix_type,
                                   uint8_t ringbuf_len,
                                   bool out_queue_overwrite) :
    WhoFrameCapNode(name, ringbuf_len, out_queue_overwrite),
    m_dst_w(dst_w),
    m_dst_h(dst_h),
    m_dst_imgs(ringbuf_len + 1),
    m_img_idx(0)
{
    ppa_client_config_t ppa_client_config = {};
    ppa_client_config.oper_type = PPA_OPERATION_SRM;
    ESP_ERROR_CHECK(ppa_register_client(&ppa_client_config, &m_ppa_srm_handle));
    size_t align = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    for (int i = 0; i < m_dst_imgs.size(); i++) {
        auto &img = m_dst_imgs[i];
        img.width = dst_w;
        img.height = dst_h;
        img.pix_type = dst_pix_type;
        if (i == 0) {
            m_buf_size = dl::image::align_up(dl::image::get_img_byte_size(img), align);
        }
        img.data = heap_caps_aligned_calloc(align, 1, m_buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    }
}

WhoPPAResizeNode::~WhoPPAResizeNode()
{
    ESP_ERROR_CHECK(ppa_unregister_client(m_ppa_srm_handle));
    for (int i = 0; i < m_dst_imgs.size(); i++) {
        heap_caps_free(m_dst_imgs[i].data);
    }
}

void WhoPPAResizeNode::cleanup()
{
    while (uxQueueMessagesWaiting(m_in_queue) > 0) {
        cam_fb_t *tmp;
        xQueueReceive(m_in_queue, &tmp, 0);
    }
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    while (!m_cam_fbs.empty()) {
        auto fb = m_cam_fbs.pop();
        delete fb;
    }
    xSemaphoreGive(m_mutex);
}

cam_fb_t *WhoPPAResizeNode::process(who::cam::cam_fb_t *fb)
{
    auto timestamp = fb->timestamp;
    auto dst_img = get_dst_img();
    dl::image::resize_ppa(*fb, dst_img, m_ppa_srm_handle);
    return new cam_fb_t(dst_img, timestamp);
}

void WhoPPAResizeNode::update_ringbuf(who::cam::cam_fb_t *fb)
{
    if (m_cam_fbs.full()) {
        auto fb_prev = m_cam_fbs.pop();
        delete fb_prev;
    }
    m_cam_fbs.push(fb);
}

dl::image::img_t WhoPPAResizeNode::get_dst_img()
{
    auto &img = m_dst_imgs[m_img_idx];
    m_img_idx = (m_img_idx + 1) % m_dst_imgs.size();
    return img;
}
#endif
} // namespace frame_cap
} // namespace who
