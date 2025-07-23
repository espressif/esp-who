#include "who_uvc_cam.hpp"

static const char *TAG = "WhoUVCCam";

namespace who {
namespace cam {
WhoUVCCam::WhoUVCCam(
    const uvc_host_stream_format fmt, uint16_t h_res, uint16_t v_res, float fps, const uint8_t fb_count) :
    WhoCam(fb_count, h_res, v_res), m_frame(xQueueCreate(1, sizeof(uvc_host_frame_t *))), m_format(fmt)
{
    assert(fmt == UVC_VS_FORMAT_MJPEG || fmt == UVC_VS_FORMAT_YUY2);
    size_t frame_size = (fmt == UVC_VS_FORMAT_MJPEG) ? (size_t)(h_res * v_res * 3 / 5.f) : (size_t)(h_res * v_res * 2);
    m_stream_config = {
        .event_cb = &WhoUVCCam::stream_cb,
        .frame_cb = &WhoUVCCam::frame_cb,
        .user_ctx = this,
        .usb =
            {
                .dev_addr = 0,
                .vid = 0,
                .pid = 0,
                .uvc_stream_index = 0,
            },
        .vs_format =
            {
                .h_res = h_res,
                .v_res = v_res,
                .fps = fps,
                .format = fmt,
            },
        .advanced =
            {
                .number_of_frame_buffers = fb_count + 1,
                .frame_size = frame_size,
                .frame_heap_caps = MALLOC_CAP_DEFAULT,
                .number_of_urbs = 3,
                .urb_size = 10 * 1024,
            },
    };
    auto uvc = uvc::WhoUVC::get_instance();
    uvc->run(4096, 16, 0);
    xEventGroupWaitBits(uvc->get_event_group(), uvc::WhoUVC::UVC_HOST_INSTALLED, pdFALSE, pdFALSE, portMAX_DELAY);
    if (uvc_host_stream_open(&m_stream_config, pdMS_TO_TICKS(500), &m_stream) == ESP_OK) {
        ESP_ERROR_CHECK(uvc_host_stream_start(m_stream));
    } else {
        uvc::WhoUVC::get_instance()->print_uvc_devices();
    }
}

cam_fb_t *WhoUVCCam::cam_fb_get()
{
    uvc_host_frame_t *frame;
    xQueueReceive(m_frame, &frame, portMAX_DELAY);
    int i = get_cam_fb_index();
    m_cam_fbs[i] = cam_fb_t(*frame, esp_timer_get_time());
    return &m_cam_fbs[i];
}

void WhoUVCCam::cam_fb_return(cam_fb_t *fb)
{
    uvc_host_frame_return(m_stream, (uvc_host_frame_t *)fb->ret);
}

int WhoUVCCam::get_cam_fb_index()
{
    static int i = 0;
    int index = i;
    i = (i + 1) % m_fb_count;
    return index;
}

void WhoUVCCam::stream_cb(const uvc_host_stream_event_data_t *event, void *user_ctx)
{
    WhoUVCCam *ctx = (WhoUVCCam *)user_ctx;
    ctx->stream_cb(event);
}

void WhoUVCCam::stream_cb(const uvc_host_stream_event_data_t *event)
{
    switch (event->type) {
    case UVC_HOST_TRANSFER_ERROR:
        ESP_LOGE(TAG, "USB error has occurred, err_no = %i", event->transfer_error.error);
        break;
    case UVC_HOST_DEVICE_DISCONNECTED:
        ESP_LOGI(TAG, "Device suddenly disconnected");
        ESP_ERROR_CHECK(uvc_host_stream_close(event->device_disconnected.stream_hdl));
        break;
    case UVC_HOST_FRAME_BUFFER_OVERFLOW:
        ESP_LOGW(TAG, "Frame buffer overflow");
        break;
    case UVC_HOST_FRAME_BUFFER_UNDERFLOW:
        ESP_LOGW(TAG, "Frame buffer underflow");
        break;
    default:
        abort();
        break;
    }
}

bool WhoUVCCam::frame_cb(const uvc_host_frame_t *frame, void *user_ctx)
{
    WhoUVCCam *ctx = (WhoUVCCam *)user_ctx;
    return ctx->frame_cb(frame);
}

bool WhoUVCCam::frame_cb(const uvc_host_frame_t *frame)
{
    if (xQueueSend(m_frame, &frame, 0) != pdTRUE) {
        uvc_host_frame_t *prev_frame;
        if (xQueueReceive(m_frame, &prev_frame, 0) == pdTRUE) {
            uvc_host_frame_return(m_stream, prev_frame);
            xQueueSend(m_frame, &frame, portMAX_DELAY);
        }
    }
    return false;
}
} // namespace cam
} // namespace who
