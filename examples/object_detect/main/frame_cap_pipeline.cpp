#include "frame_cap_pipeline.hpp"
#include "who_cam.hpp"

using namespace who::cam;
using namespace who::frame_cap;

#if defined(CONFIG_HUMAN_FACE_DETECT_MODEL_LOCATION)
// num of frames the model take to get result
#define MODEL_TIME 2
#define MODEL_INPUT_W 160
#define MODEL_INPUT_H 120
#elif defined(CONFIG_PEDESTRIAN_DETECT_MODEL_LOCATION)
// num of frames the model take to get result
#define MODEL_TIME 3
#define MODEL_INPUT_W 224
#define MODEL_INPUT_H 224
#elif defined(CONFIG_CAT_DETECT_MODEL_LOCATION)
// num of frames the model take to get result
#if defined(CONFIG_ESPDET_PICO_224_224_CAT)
#define MODEL_TIME 3
#define MODEL_INPUT_W 224
#define MODEL_INPUT_H 224
#endif
#if defined(CONFIG_ESPDET_PICO_416_416_CAT)
#define MODEL_TIME 8
#define MODEL_INPUT_W 416
#define MODEL_INPUT_H 416
#endif
#elif defined(CONFIG_DOG_DETECT_MODEL_LOCATION)
// num of frames the model take to get result
#if defined(CONFIG_ESPDET_PICO_224_224_DOG)
#define MODEL_TIME 3
#define MODEL_INPUT_W 224
#define MODEL_INPUT_H 224
#endif
#if defined(CONFIG_ESPDET_PICO_416_416_DOG)
#define MODEL_TIME 8
#define MODEL_INPUT_W 416
#define MODEL_INPUT_H 416
#endif
#endif

// The size of the fb_count and ringbuf_len must be big enough. If you have no idea how to set them, try with 5 and
// larger.
#if CONFIG_IDF_TARGET_ESP32S3
WhoFrameCap *get_lcd_dvp_frame_cap_pipeline()
{
    // The ringbuf_len of FetchNode equals cam_fb_count - 2. The WhoFetchNode fb will display on lcd, if you want to
    // make sure the displayed detection result is synced with the frame, the ringbuf size must be big enough to
    // cover the process time from now to the the detection result is ready. If the ring_buf_len is 3, the frame
    // which the disp task will display is 2 frames before than the frame which feeds into detection task. So the
    // detection task must finish within 2 frames, or the detect result will have a delay compared to the displayed
    // frame.
    framesize_t frame_size = get_cam_frame_size_from_lcd_resolution();
#ifdef BSP_BOARD_ESP32_S3_KORVO_2
    auto cam = new WhoS3Cam(PIXFORMAT_RGB565, frame_size, MODEL_TIME + 3, true, true);
#else
    auto cam = new WhoS3Cam(PIXFORMAT_RGB565, frame_size, MODEL_TIME + 3);
#endif
    auto frame_cap = new WhoFrameCap();
    frame_cap->add_node<WhoFetchNode>("FrameCapFetch", cam);
    return frame_cap;
}

WhoFrameCap *get_term_dvp_frame_cap_pipeline()
{
    // Don't need lcd display, cam_fb_count can decrease 1.
    framesize_t frame_size = get_cam_frame_size_from_lcd_resolution();
#ifdef BSP_BOARD_ESP32_S3_KORVO_2
    auto cam = new WhoS3Cam(PIXFORMAT_RGB565, frame_size, MODEL_TIME + 2, true, true);
#else
    auto cam = new WhoS3Cam(PIXFORMAT_RGB565, frame_size, MODEL_TIME + 2);
#endif
    auto frame_cap = new WhoFrameCap();
    frame_cap->add_node<WhoFetchNode>("FrameCapFetch", cam);
    return frame_cap;
}
#elif CONFIG_IDF_TARGET_ESP32P4
WhoFrameCap *get_lcd_mipi_csi_frame_cap_pipeline()
{
    auto cam = new WhoP4Cam(V4L2_PIX_FMT_RGB565, MODEL_TIME + 3);
    auto frame_cap = new WhoFrameCap();
    frame_cap->add_node<WhoFetchNode>("FrameCapFetch", cam);
    return frame_cap;
}

WhoFrameCap *get_lcd_mipi_csi_ppa_frame_cap_pipeline(WhoFrameCapNode **lcd_disp_frame_cap_node)
{
    // Use ppa to resize the frame into model input shape to avoid doing this in the model inference which can reduce
    // cpu load. Compared to the one without ppa, the cam_fb_count must increase, because the process time since
    // WhoFetchNode equals ppa + detect, while without ppa, the process time equals detect.
    auto cam = new WhoP4Cam(V4L2_PIX_FMT_RGB565, MODEL_TIME + 4);
    auto frame_cap = new WhoFrameCap();
    frame_cap->add_node<WhoFetchNode>("FrameCapFetch", cam);
    frame_cap->add_node<WhoPPAResizeNode>(
        "FrameCapPPAResize", MODEL_INPUT_W, MODEL_INPUT_H, dl::image::DL_IMAGE_PIX_TYPE_RGB565, MODEL_TIME);
    *lcd_disp_frame_cap_node = frame_cap->get_node("FrameCapFetch");
    return frame_cap;
}

WhoFrameCap *get_lcd_uvc_frame_cap_pipeline()
{
    auto cam = new WhoUVCCam(UVC_VS_FORMAT_MJPEG, 640, 480, 30, 4);
    auto frame_cap = new WhoFrameCap();
    // The ringbuf_len of FetchNode equals cam_fb_count - 2, the ringbuf_len of FetchNode should take care of the
    // process time of the following Node. For example, if the DecodeNode takes 2 frame to decode, then the
    // FetchNode ringbuf_len is at least 2, and the fb_count of the cam is at least 4.
    frame_cap->add_node<WhoFetchNode>("FrameCapFetch", cam, false);
    // The DecodeNode ringbuf_len relies on the following PPAResizeNode process time, the time of data transfer.
    frame_cap->add_node<WhoDecodeNode>("FrameCapDecode", dl::image::DL_IMAGE_PIX_TYPE_RGB565, 2, false);
    // The ppa resized fb will display on lcd, if you want to make sure the displayed detection result is synced with
    // the frame, the ringbuf size must be big enough to cover the process time from now to the the detection result is
    // ready.
    frame_cap->add_node<WhoPPAResizeNode>(
        "FrameCapPPAResize", 800, 600, dl::image::DL_IMAGE_PIX_TYPE_RGB565, MODEL_TIME + 1);
    return frame_cap;
}

WhoFrameCap *get_term_mipi_csi_frame_cap_pipeline()
{
    auto cam = new WhoP4Cam(V4L2_PIX_FMT_RGB565, MODEL_TIME + 2);
    auto frame_cap = new WhoFrameCap();
    frame_cap->add_node<WhoFetchNode>("FrameCapFetch", cam);
    return frame_cap;
}

WhoFrameCap *get_term_mipi_csi_ppa_frame_cap_pipeline()
{
    auto cam = new WhoP4Cam(V4L2_PIX_FMT_RGB565, MODEL_TIME + 3);
    auto frame_cap = new WhoFrameCap();
    frame_cap->add_node<WhoFetchNode>("FrameCapFetch", cam);
    frame_cap->add_node<WhoPPAResizeNode>(
        "FrameCapPPAResize", MODEL_INPUT_W, MODEL_INPUT_H, dl::image::DL_IMAGE_PIX_TYPE_RGB565, MODEL_TIME);
    return frame_cap;
}

WhoFrameCap *get_term_uvc_frame_cap_pipeline()
{
    auto cam = new WhoUVCCam(UVC_VS_FORMAT_MJPEG, 640, 480, 30, 4);
    auto frame_cap = new WhoFrameCap();
    frame_cap->add_node<WhoFetchNode>("FrameCapFetch", cam, false);
    frame_cap->add_node<WhoDecodeNode>("FrameCapDecode", dl::image::DL_IMAGE_PIX_TYPE_RGB565, MODEL_TIME);
    return frame_cap;
}
#endif
