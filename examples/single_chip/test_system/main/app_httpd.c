// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "app_httpd.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "camera_index.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char *TAG = "camera_httpd";
#endif

#if CONFIG_ESP_FACE_DETECT_ENABLED
#include "fd_forward.h"
#include "dl_lib_matrix3d.h"
#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
#include "fr_forward.h"

#define ENROLL_CONFIRM_TIMES 3
#define FACE_ID_SAVE_NUMBER 250
#endif

#define FACE_COLOR_WHITE 0x00FFFFFF
#define FACE_COLOR_BLACK 0x00000000
#define FACE_COLOR_RED 0x000000FF
#define FACE_COLOR_GREEN 0x0000FF00
#define FACE_COLOR_BLUE 0x00FF0000
#define FACE_COLOR_YELLOW (FACE_COLOR_RED | FACE_COLOR_GREEN)
#define FACE_COLOR_CYAN (FACE_COLOR_BLUE | FACE_COLOR_GREEN)
#define FACE_COLOR_PURPLE (FACE_COLOR_BLUE | FACE_COLOR_RED)
#endif

typedef struct
{
    size_t size;  //number of values used for filtering
    size_t index; //current value index
    size_t count; //value count
    int sum;
    int *values; //array to be filled with values
} ra_filter_t;

typedef struct
{
    httpd_req_t *req;
    size_t len;
} jpg_chunking_t;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static ra_filter_t ra_filter;
httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;
#if CONFIG_ESP_FACE_DETECT_ENABLED
static mtmn_config_t mtmn_config = {0};
static int8_t detection_enabled = 0;
#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
static int8_t recognition_enabled = 0;
static int8_t g_is_enrolling = 0;
face_id_list g_id_list = {0};
#endif
#endif
static ra_filter_t ra_filter;

static ra_filter_t *ra_filter_init(ra_filter_t *filter, size_t sample_size)
{
    memset(filter, 0, sizeof(ra_filter_t));

    filter->values = (int *)malloc(sample_size * sizeof(int));
    if (!filter->values)
    {
        free(filter);
        return NULL;
    }
    memset(filter->values, 0, sample_size * sizeof(int));

    filter->size = sample_size;
    return filter;
}

static int ra_filter_run(ra_filter_t *filter, int value)
{
    if (!filter->values)
    {
        return value;
    }
    filter->sum -= filter->values[filter->index];
    filter->values[filter->index] = value;
    filter->sum += filter->values[filter->index];
    filter->index++;
    filter->index = filter->index % filter->size;
    if (filter->count < filter->size)
    {
        filter->count++;
    }
    return filter->sum / filter->count;
}

#if CONFIG_ESP_FACE_DETECT_ENABLED
#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
static void rgb_print(dl_matrix3du_t *image_matrix, uint32_t color, const char *str)
{
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    fb_gfx_print(&fb, (fb.width - (strlen(str) * 14)) / 2, 10, color, str);
}

static int rgb_printf(dl_matrix3du_t *image_matrix, uint32_t color, const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            return 0;
        }
    }
    vsnprintf(temp, len + 1, format, arg);
    va_end(arg);
    rgb_print(image_matrix, color, temp);
    if (len > 64)
    {
        free(temp);
    }
    return len;
}
#endif
static void draw_face_boxes(dl_matrix3du_t *image_matrix, box_array_t *boxes, int face_id)
{
    int x, y, w, h, i;
    uint32_t color = FACE_COLOR_YELLOW;
    if (face_id < 0)
    {
        color = FACE_COLOR_RED;
    }
    else if (face_id > 0)
    {
        color = FACE_COLOR_GREEN;
    }
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    for (i = 0; i < boxes->len; i++)
    {
        // rectangle box
        x = (int)boxes->box[i].box_p[0];
        y = (int)boxes->box[i].box_p[1];
        w = (int)boxes->box[i].box_p[2] - x + 1;
        h = (int)boxes->box[i].box_p[3] - y + 1;
        fb_gfx_drawFastHLine(&fb, x, y, w, color);
        fb_gfx_drawFastHLine(&fb, x, y + h - 1, w, color);
        fb_gfx_drawFastVLine(&fb, x, y, h, color);
        fb_gfx_drawFastVLine(&fb, x + w - 1, y, h, color);
#if 0
        // landmark
        int x0, y0, j;
        for (j = 0; j < 10; j+=2) {
            x0 = (int)boxes->landmark[i].landmark_p[j];
            y0 = (int)boxes->landmark[i].landmark_p[j+1];
            fb_gfx_fillRect(&fb, x0, y0, 3, 3, color);
        }
#endif
    }
}

#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
static int run_face_recognition(dl_matrix3du_t *image_matrix, box_array_t *net_boxes)
{
    dl_matrix3du_t *aligned_face = NULL;
    int matched_id = 0;

    aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);
    if (!aligned_face)
    {
        ESP_LOGE(TAG, "Could not allocate face recognition buffer");
        return matched_id;
    }

    if (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK)
    {
        if (g_is_enrolling == 1)
        {
            int8_t left_sample_face = enroll_face(&g_id_list, aligned_face);

            if (left_sample_face == (ENROLL_CONFIRM_TIMES - 1))
            {
                ESP_LOGD(TAG, "Enrolling Face ID: %d", g_id_list.tail);
            }
            ESP_LOGD(TAG, "Enrolling Face ID: %d sample %d", g_id_list.tail, ENROLL_CONFIRM_TIMES - left_sample_face);
            rgb_printf(image_matrix, FACE_COLOR_CYAN, "ID[%u] Sample[%u]", g_id_list.tail, ENROLL_CONFIRM_TIMES - left_sample_face);
            if (left_sample_face == 0)
            {
                g_is_enrolling = 0;
                ESP_LOGD(TAG, "Enrolled Face ID: %d", g_id_list.tail);
            }
        }
        else
        {
            matched_id = recognize_face(&g_id_list, aligned_face);
            if (matched_id >= 0)
            {
                ESP_LOGW(TAG, "Match Face ID: %u", matched_id);
                rgb_printf(image_matrix, FACE_COLOR_GREEN, "Hello Subject %u", matched_id);
            }
            else
            {
                ESP_LOGW(TAG, "No Match Found");
                rgb_print(image_matrix, FACE_COLOR_RED, "Intruder Alert!");
                matched_id = -1;
            }
        }
    }
    else
    {
        ESP_LOGW(TAG, "Face Not Aligned");
        //rgb_print(image_matrix, FACE_COLOR_YELLOW, "Human Detected");
    }

    dl_matrix3du_free(aligned_face);
    return matched_id;
}
#endif
#endif
static size_t jpg_encode_stream(void *arg, size_t index, const void *data, size_t len)
{
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if (!index)
    {
        j->len = 0;
    }
    if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK)
    {
        return 0;
    }
    j->len += len;
    return len;
}

static esp_err_t capture_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    int64_t fr_start = esp_timer_get_time();

    fb = esp_camera_fb_get();

    if (!fb)
    {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");

#if CONFIG_ESP_FACE_DETECT_ENABLED
    size_t out_len, out_width, out_height;
    uint8_t *out_buf;
    bool s;
    bool detected = false;
    int face_id = 0;
    if (!detection_enabled || fb->width > 400)
    {
#endif
        size_t fb_len = 0;
        if (fb->format == PIXFORMAT_JPEG)
        {
            fb_len = fb->len;
            res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
        }
        else
        {
            jpg_chunking_t jchunk = {req, 0};
            res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk) ? ESP_OK : ESP_FAIL;
            httpd_resp_send_chunk(req, NULL, 0);
            fb_len = jchunk.len;
        }
        esp_camera_fb_return(fb);
        int64_t fr_end = esp_timer_get_time();
        ESP_LOGI(TAG, "JPG: %uB %ums", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
        return res;
#if CONFIG_ESP_FACE_DETECT_ENABLED
    }

    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
    if (!image_matrix)
    {
        esp_camera_fb_return(fb);
        ESP_LOGE(TAG, "dl_matrix3du_alloc failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    out_buf = image_matrix->item;
    out_len = fb->width * fb->height * 3;
    out_width = fb->width;
    out_height = fb->height;

    s = fmt2rgb888(fb->buf, fb->len, fb->format, out_buf);
    esp_camera_fb_return(fb);
    if (!s)
    {
        dl_matrix3du_free(image_matrix);
        ESP_LOGE(TAG, "to rgb888 failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);

    if (net_boxes)
    {
        detected = true;
#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
        if (recognition_enabled)
        {
            face_id = run_face_recognition(image_matrix, net_boxes);
        }
#endif
        draw_face_boxes(image_matrix, net_boxes, face_id);
        free(net_boxes->box);
        free(net_boxes->score);
        free(net_boxes->landmark);
        free(net_boxes);
    }

    jpg_chunking_t jchunk = {req, 0};
    s = fmt2jpg_cb(out_buf, out_len, out_width, out_height, PIXFORMAT_RGB888, 90, jpg_encode_stream, &jchunk);
    dl_matrix3du_free(image_matrix);
    if (!s)
    {
        ESP_LOGE(TAG, "JPEG compression failed");
        return ESP_FAIL;
    }

    int64_t fr_end = esp_timer_get_time();
    ESP_LOGI(TAG, "FACE: %uB %ums %s%d", (uint32_t)(jchunk.len), (uint32_t)((fr_end - fr_start) / 1000), detected ? "DETECTED " : "", face_id);
    return res;
#endif
}

static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[64];
#if CONFIG_ESP_FACE_DETECT_ENABLED
    dl_matrix3du_t *image_matrix = NULL;
    bool detected = false;
    int face_id = 0;
    int64_t fr_start = 0;
    int64_t fr_ready = 0;
    int64_t fr_face = 0;
    int64_t fr_recognize = 0;
    int64_t fr_encode = 0;
#endif

    static int64_t last_frame = 0;
    if (!last_frame)
    {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
    {
        return res;
    }

    while (true)
    {
#if CONFIG_ESP_FACE_DETECT_ENABLED
        detected = false;
        face_id = 0;
#endif
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
        }
        else
        {
#if CONFIG_ESP_FACE_DETECT_ENABLED
            fr_start = esp_timer_get_time();
            fr_ready = fr_start;
            fr_face = fr_start;
            fr_encode = fr_start;
            fr_recognize = fr_start;
            if (!detection_enabled || fb->width > 400)
            {
#endif
                if (fb->format != PIXFORMAT_JPEG)
                {
                    bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                    esp_camera_fb_return(fb);
                    fb = NULL;
                    if (!jpeg_converted)
                    {
                        ESP_LOGE(TAG, "JPEG compression failed");
                        res = ESP_FAIL;
                    }
                }
                else
                {
                    _jpg_buf_len = fb->len;
                    _jpg_buf = fb->buf;
                }
#if CONFIG_ESP_FACE_DETECT_ENABLED
            }
            else
            {

                image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);

                if (!image_matrix)
                {
                    ESP_LOGE(TAG, "dl_matrix3du_alloc failed");
                    res = ESP_FAIL;
                }
                else
                {
                    if (!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item))
                    {
                        ESP_LOGE(TAG, "fmt2rgb888 failed");
                        res = ESP_FAIL;
                    }
                    else
                    {
                        fr_ready = esp_timer_get_time();
                        box_array_t *net_boxes = NULL;
                        if (detection_enabled)
                        {
                            net_boxes = face_detect(image_matrix, &mtmn_config);
                        }
                        fr_face = esp_timer_get_time();
                        fr_recognize = fr_face;
                        if (net_boxes || fb->format != PIXFORMAT_JPEG)
                        {
                            if (net_boxes)
                            {
                                detected = true;
#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
                                if (recognition_enabled)
                                {
                                    face_id = run_face_recognition(image_matrix, net_boxes);
                                }
                                fr_recognize = esp_timer_get_time();
#endif
                                draw_face_boxes(image_matrix, net_boxes, face_id);
                                free(net_boxes->box);
                                free(net_boxes->landmark);
                                free(net_boxes);
                            }
                            if (!fmt2jpg(image_matrix->item, fb->width * fb->height * 3, fb->width, fb->height, PIXFORMAT_RGB888, 90, &_jpg_buf, &_jpg_buf_len))
                            {
                                ESP_LOGE(TAG, "fmt2jpg failed");
                                res = ESP_FAIL;
                            }
                            esp_camera_fb_return(fb);
                            fb = NULL;
                        }
                        else
                        {
                            _jpg_buf = fb->buf;
                            _jpg_buf_len = fb->len;
                        }
                        fr_encode = esp_timer_get_time();
                    }
                    dl_matrix3du_free(image_matrix);
                }
            }
#endif
        }
        if (res == ESP_OK)
        {
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if (fb)
        {
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        }
        else if (_jpg_buf)
        {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if (res != ESP_OK)
        {
            break;
        }
        int64_t fr_end = esp_timer_get_time();

#if CONFIG_ESP_FACE_DETECT_ENABLED
        int64_t ready_time = (fr_ready - fr_start) / 1000;
        int64_t face_time = (fr_face - fr_ready) / 1000;
        int64_t recognize_time = (fr_recognize - fr_face) / 1000;
        int64_t encode_time = (fr_encode - fr_recognize) / 1000;
        int64_t process_time = (fr_encode - fr_start) / 1000;
#endif

        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        uint32_t avg_frame_time = ra_filter_run(&ra_filter, frame_time);
        ESP_LOGI(TAG, "MJPG: %uB %ums (%.1ffps), AVG: %ums (%.1ffps)"
#if CONFIG_ESP_FACE_DETECT_ENABLED
                      ", %u+%u+%u+%u=%u %s%d"
#endif
                 ,
                 (uint32_t)(_jpg_buf_len),
                 (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time,
                 avg_frame_time, 1000.0 / avg_frame_time
#if CONFIG_ESP_FACE_DETECT_ENABLED
                 ,
                 (uint32_t)ready_time, (uint32_t)face_time, (uint32_t)recognize_time, (uint32_t)encode_time, (uint32_t)process_time,
                 (detected) ? "DETECTED " : "", face_id
#endif
        );
    }

    last_frame = 0;
    return res;
}

static esp_err_t cmd_handler(httpd_req_t *req)
{
    char *buf;
    size_t buf_len;
    char variable[32] = {
        0,
    };
    char value[32] = {
        0,
    };

    buf_len = httpd_req_get_url_query_len(req) + 1;

    if (buf_len > 1)
    {
        buf = (char *)malloc(buf_len);

        if (!buf)
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) == ESP_OK &&
                httpd_query_key_value(buf, "val", value, sizeof(value)) == ESP_OK)
            {
            }
            else
            {
                free(buf);
                httpd_resp_send_404(req);
                return ESP_FAIL;
            }
        }
        else
        {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    }
    else
    {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    int val = atoi(value);
    ESP_LOGI(TAG, "%s = %d", variable, val);
    sensor_t *s = esp_camera_sensor_get();
    int res = 0;

    if (!strcmp(variable, "framesize"))
    {
        if (s->pixformat == PIXFORMAT_JPEG)
            res = s->set_framesize(s, (framesize_t)val);
    }
    else if (!strcmp(variable, "quality"))
        res = s->set_quality(s, val);
    else if (!strcmp(variable, "contrast"))
        res = s->set_contrast(s, val);
    else if (!strcmp(variable, "brightness"))
        res = s->set_brightness(s, val);
    else if (!strcmp(variable, "saturation"))
        res = s->set_saturation(s, val);
    else if (!strcmp(variable, "gainceiling"))
        res = s->set_gainceiling(s, (gainceiling_t)val);
    else if (!strcmp(variable, "colorbar"))
        res = s->set_colorbar(s, val);
    else if (!strcmp(variable, "awb"))
        res = s->set_whitebal(s, val);
    else if (!strcmp(variable, "agc"))
        res = s->set_gain_ctrl(s, val);
    else if (!strcmp(variable, "aec"))
        res = s->set_exposure_ctrl(s, val);
    else if (!strcmp(variable, "hmirror"))
        res = s->set_hmirror(s, val);
    else if (!strcmp(variable, "vflip"))
        res = s->set_vflip(s, val);
    else if (!strcmp(variable, "awb_gain"))
        res = s->set_awb_gain(s, val);
    else if (!strcmp(variable, "agc_gain"))
        res = s->set_agc_gain(s, val);
    else if (!strcmp(variable, "aec_value"))
        res = s->set_aec_value(s, val);
    else if (!strcmp(variable, "aec2"))
        res = s->set_aec2(s, val);
    else if (!strcmp(variable, "dcw"))
        res = s->set_dcw(s, val);
    else if (!strcmp(variable, "bpc"))
        res = s->set_bpc(s, val);
    else if (!strcmp(variable, "wpc"))
        res = s->set_wpc(s, val);
    else if (!strcmp(variable, "raw_gma"))
        res = s->set_raw_gma(s, val);
    else if (!strcmp(variable, "lenc"))
        res = s->set_lenc(s, val);
    else if (!strcmp(variable, "special_effect"))
        res = s->set_special_effect(s, val);
    else if (!strcmp(variable, "wb_mode"))
        res = s->set_wb_mode(s, val);
    else if (!strcmp(variable, "ae_level"))
        res = s->set_ae_level(s, val);

#if CONFIG_ESP_FACE_DETECT_ENABLED
    else if (!strcmp(variable, "face_detect"))
    {
        detection_enabled = val;
#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
        if (!detection_enabled)
        {
            recognition_enabled = 0;
        }
#endif
    }
#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
    else if (!strcmp(variable, "face_enroll"))
        g_is_enrolling = val;
    else if (!strcmp(variable, "face_recognize"))
    {
        recognition_enabled = val;
        if (recognition_enabled)
        {
            detection_enabled = val;
        }
    }
#endif
#endif
    else
    {
        res = -1;
    }

    if (res)
    {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t status_handler(httpd_req_t *req)
{
    static char json_response[1024];

    sensor_t *s = esp_camera_sensor_get();
    char *p = json_response;
    *p++ = '{';

    p += sprintf(p, "\"framesize\":%u,", s->status.framesize);
    p += sprintf(p, "\"quality\":%u,", s->status.quality);
    p += sprintf(p, "\"brightness\":%d,", s->status.brightness);
    p += sprintf(p, "\"contrast\":%d,", s->status.contrast);
    p += sprintf(p, "\"saturation\":%d,", s->status.saturation);
    p += sprintf(p, "\"special_effect\":%u,", s->status.special_effect);
    p += sprintf(p, "\"wb_mode\":%u,", s->status.wb_mode);
    p += sprintf(p, "\"awb\":%u,", s->status.awb);
    p += sprintf(p, "\"awb_gain\":%u,", s->status.awb_gain);
    p += sprintf(p, "\"aec\":%u,", s->status.aec);
    p += sprintf(p, "\"aec2\":%u,", s->status.aec2);
    p += sprintf(p, "\"ae_level\":%d,", s->status.ae_level);
    p += sprintf(p, "\"aec_value\":%u,", s->status.aec_value);
    p += sprintf(p, "\"agc\":%u,", s->status.agc);
    p += sprintf(p, "\"agc_gain\":%u,", s->status.agc_gain);
    p += sprintf(p, "\"gainceiling\":%u,", s->status.gainceiling);
    p += sprintf(p, "\"bpc\":%u,", s->status.bpc);
    p += sprintf(p, "\"wpc\":%u,", s->status.wpc);
    p += sprintf(p, "\"raw_gma\":%u,", s->status.raw_gma);
    p += sprintf(p, "\"lenc\":%u,", s->status.lenc);
    p += sprintf(p, "\"hmirror\":%u,", s->status.hmirror);
    p += sprintf(p, "\"dcw\":%u,", s->status.dcw);
    p += sprintf(p, "\"colorbar\":%u", s->status.colorbar);

#if CONFIG_ESP_FACE_DETECT_ENABLED
    p += sprintf(p, ",\"face_detect\":%u", detection_enabled);
#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
    p += sprintf(p, ",\"face_enroll\":%u,", g_is_enrolling);
    p += sprintf(p, "\"face_recognize\":%u", recognition_enabled);
#endif
#endif
    *p++ = '}';
    *p++ = 0;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}

int httpd_req_get_hdr_value_int(httpd_req_t *req, char *key)
{
    int len = httpd_req_get_hdr_value_len(req, key) + 1;

    // char *value = (char *)malloc(len);
    char *value = (char *)heap_caps_malloc(len, MALLOC_CAP_SPIRAM);
    httpd_req_get_hdr_value_str(req, key, value, len);
    int result = atoi(value);

    free(value);

    return result;
}

esp_err_t httpd_req_recv_content_uint8(httpd_req_t *req, uint8_t *content, int max_frame_size)
{
    char tag[] = "httpd_req_recv_content_uint8";
    char *frame = NULL;
    int frame_size = 0;
    int received_frame_len = 0;
    int received_len = 0;

    max_frame_size = (req->content_len > max_frame_size) ? max_frame_size : req->content_len;

    // frame = (char *)malloc(max_frame_size);
    frame = (char *)heap_caps_malloc(max_frame_size, MALLOC_CAP_SPIRAM);
    if (!frame)
    {
        httpd_resp_send_500(req);
        ESP_LOGE(tag, "Could not malloc frame.");

        return ESP_FAIL;
    }

    while (received_len < req->content_len)
    {
        int remain_len = req->content_len - received_len;
        frame_size = (remain_len < max_frame_size) ? remain_len : max_frame_size;

        received_frame_len = httpd_req_recv(req, frame, frame_size);

        switch (received_frame_len)
        {
        case HTTPD_SOCK_ERR_TIMEOUT:
            ESP_LOGW(tag, "Http sock error timeout.");
            break;

        case HTTPD_SOCK_ERR_INVALID:
            ESP_LOGE(tag, "Invalid arguments");
            httpd_resp_send_404(req);

            free(frame);
            return ESP_FAIL;

        case HTTPD_SOCK_ERR_FAIL:
            ESP_LOGE(tag, "Unrecoverable error while calling socket recv()");
            httpd_resp_send_404(req);

            free(frame);
            return ESP_FAIL;

        case 0:
            ESP_LOGE(tag, "Http connection closed by peer.");
            httpd_resp_send_408(req);

            free(frame);
            return ESP_FAIL;

        default:
            if (received_frame_len < 0)
                ESP_LOGE(tag, "Received frame length: %d", received_frame_len);

            memcpy(content + received_len, frame, received_frame_len);
            received_len += received_frame_len;

            break;
        }
    }
    free(frame);

    if (received_len != req->content_len)
        ESP_LOGE(tag, "received_len : req->content_len = %d : %d", received_len, req->content_len);

    return ESP_OK;
}

typedef enum
{
    ENROLL_IDLE = 0,
    ENROLL_FAIL = 1,
    ENROLLING = 2,
    ENROLLED = 3
} enrollment_status_t;

void upload_handler_response(httpd_req_t *req,
                             box_array_t *net_boxes,
                             int8_t *matched_ids,
                             enrollment_status_t enrollment_status,
                             int8_t enrolled_id,
                             uint32_t detection_duration,
                             uint32_t recognition_duration,
                             uint32_t enrollment_duration)
{
    char tag[] = "upload_handler_response";

    char *scores_string = NULL;
    char *boxes_string = NULL;
    char *landmarks_string = NULL;
    char *matched_ids_string = NULL;

    char enrollment_status_string[2] = {0};
    char enrolled_id_string[4] = {0};
    char detection_duration_string[5] = {0};
    char enrollment_duration_string[5] = {0};
    char recognition_duration_string[5] = {0};

    /* net boxes */
    if (net_boxes)
    {
        // scores
        scores_string = (char *)heap_caps_malloc(net_boxes->len * 8 + 2, MALLOC_CAP_SPIRAM);
        char *p_scores_string = scores_string;

        *p_scores_string++ = '(';

        for (size_t i = 0; i < net_boxes->len; i++)
            p_scores_string += sprintf(p_scores_string, "%.5f,", net_boxes->score[i]);

        *p_scores_string++ = ')';
        *p_scores_string = '\0';

        httpd_resp_set_hdr(req, "scores", scores_string);
        ESP_LOGI(tag, "scores_string: %s", scores_string);

        // boxes
        char *boxes_string = (char *)heap_caps_malloc(net_boxes->len * 38 + 2, MALLOC_CAP_SPIRAM);
        char *p_boxes_string = boxes_string;

        *p_boxes_string++ = '(';

        for (size_t i = 0; i < net_boxes->len; i++)
        {
            p_boxes_string += sprintf(p_boxes_string,
                                      "(%.3f,%.3f,%.3f,%.3f),",
                                      net_boxes->box[i].box_p[0],
                                      net_boxes->box[i].box_p[1],
                                      net_boxes->box[i].box_p[2],
                                      net_boxes->box[i].box_p[3]);
        }
        *p_boxes_string++ = ')';
        *p_boxes_string = '\0';

        httpd_resp_set_hdr(req, "boxes", boxes_string);
        ESP_LOGI(tag, "boxes_string: %s", boxes_string);

        // landmarks
        landmarks_string = (char *)heap_caps_malloc(net_boxes->len * 92 + 2, MALLOC_CAP_SPIRAM);
        char *p_landmarks_string = landmarks_string;

        *p_landmarks_string++ = '(';

        for (size_t i = 0; i < net_boxes->len; i++)
        {
            p_landmarks_string += sprintf(p_landmarks_string,
                                          "(%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f),",
                                          net_boxes->landmark[i].landmark_p[0],
                                          net_boxes->landmark[i].landmark_p[1],
                                          net_boxes->landmark[i].landmark_p[2],
                                          net_boxes->landmark[i].landmark_p[3],
                                          net_boxes->landmark[i].landmark_p[4],
                                          net_boxes->landmark[i].landmark_p[5],
                                          net_boxes->landmark[i].landmark_p[6],
                                          net_boxes->landmark[i].landmark_p[7],
                                          net_boxes->landmark[i].landmark_p[8],
                                          net_boxes->landmark[i].landmark_p[9]);
        }
        *p_landmarks_string++ = ')';
        *p_landmarks_string = '\0';

        httpd_resp_set_hdr(req, "landmarks", landmarks_string);
        ESP_LOGI(tag, "landmarks_string: %s", landmarks_string);
    }

    /* matched ids */
    if (matched_ids)
    {
        matched_ids_string = (char *)heap_caps_malloc(net_boxes->len * 5 + 2, MALLOC_CAP_SPIRAM);
        char *p_matched_ids_string = matched_ids_string;

        *p_matched_ids_string++ = '(';

        for (size_t i = 0; i < net_boxes->len; i++)
            p_matched_ids_string += sprintf(p_matched_ids_string, "%d,", matched_ids[i]);

        *p_matched_ids_string++ = ')';
        *p_matched_ids_string = '\0';

        httpd_resp_set_hdr(req, "matched_ids", matched_ids_string);
        ESP_LOGI(tag, "matched_ids_string: %s", matched_ids_string);
    }

    /* enrollment status */
    if (enrollment_status != ENROLL_IDLE)
    {
        sprintf(enrollment_status_string, "%d", enrollment_status);
        httpd_resp_set_hdr(req, "enrollment_status", enrollment_status_string);
        ESP_LOGI(tag, "enrollment_status_string: %s", enrollment_status_string);
    }

    /* enrolled id */
    if (enrolled_id != -1)
    {
        sprintf(enrolled_id_string, "%d", enrolled_id);
        httpd_resp_set_hdr(req, "enrolled_id", enrolled_id_string);
        ESP_LOGI(tag, "enrolled_id_string: %s", enrolled_id_string);
    }

    /* detection duration */
    // if (detection_duration != 0)
    if (1)
    {
        sprintf(detection_duration_string, "%u", detection_duration);
        httpd_resp_set_hdr(req, "detection_duration", detection_duration_string);
        ESP_LOGI(tag, "detection_duration_string: %s", detection_duration_string);
    }

    /* enrollment duration */
    if (enrollment_duration != 0)
    {
        sprintf(enrollment_duration_string, "%u", enrollment_duration);
        httpd_resp_set_hdr(req, "enrollment_duration", enrollment_duration_string);
        ESP_LOGI(tag, "enrollment_duration_string: %s", enrollment_duration_string);
    }

    /* recognition duration */
    if (recognition_duration != 0)
    {
        sprintf(recognition_duration_string, "%u", recognition_duration);
        httpd_resp_set_hdr(req, "recognition_duration", recognition_duration_string);
        ESP_LOGI(tag, "recognition_duration_string: %s", recognition_duration_string);
    }

    httpd_resp_send(req, NULL, 0);

    free(scores_string);
    free(boxes_string);
    free(landmarks_string);
    free(matched_ids_string);

    return;
}

enrollment_status_t upload_handler_enrollment(dl_matrix3du_t *image_matrix, box_array_t *net_boxes, int8_t *enrolled_id, uint32_t *duration)
{
    char tag[] = "upload_handler_enrollment";

    enrollment_status_t enrollment_status = ENROLL_FAIL;

    if (net_boxes->len > 1)
    {
        ESP_LOGE(tag, "One face only, when doing enrollment.");
    }
    else
    {
        dl_matrix3du_t *aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);
        if (!aligned_face)
        {
            ESP_LOGE(tag, "Could not allocate aligned_face");
        }
        else
        {
            if (ESP_FAIL == align_face2(net_boxes->landmark[0].landmark_p, image_matrix, aligned_face))
            {
                ESP_LOGW(tag, "Face is not aligned.");
            }
            else
            {
                ESP_LOGE(tag, "Face is aligned.");

                uint64_t timestamp = esp_timer_get_time();
                int8_t left_sample_face = enroll_face(&g_id_list, aligned_face);
                *duration = (uint32_t)((esp_timer_get_time() - timestamp) / 1000);

                enrollment_status = ENROLLING;

                switch (left_sample_face)
                {
                // Enrolled first sample
                case ENROLL_CONFIRM_TIMES - 1:
                    ESP_LOGW(tag, "Enrolling Face ID: %d", g_id_list.tail);
                    break;
                // Enrolled last sample
                case 0:
                    ESP_LOGW(TAG, "Enrolled Face ID: %d", g_id_list.tail - 1);
                    if (g_id_list.id_list[g_id_list.tail - 1]->c != 512)
                    {
                        ESP_LOGE(TAG, "Enrolled Face ID size: %d", g_id_list.id_list[g_id_list.tail - 1]->c);
                        assert(g_id_list.id_list[g_id_list.tail - 1]->c == 512);
                    }
                    *enrolled_id = g_id_list.tail - 1;
                    enrollment_status = ENROLLED;
                    break;
                // Was enrolling
                default:
                    ESP_LOGW(tag,
                             "Enrolling Face ID: %d sample %d",
                             g_id_list.tail,
                             ENROLL_CONFIRM_TIMES - left_sample_face);
                    break;
                }
            }
            // Free aligned face
            dl_matrix3du_free(aligned_face);
        }
    }
    return enrollment_status;
}

esp_err_t upload_handler_recognition(dl_matrix3du_t *image_matrix, box_array_t *net_boxes, int8_t *matched_ids, uint32_t *duration)
{
    char tag[] = "upload_handler_recognition";

    dl_matrix3du_t *aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);

    if (!aligned_face)
    {
        ESP_LOGE(tag, "Could not allocate aligned_face");
        return ESP_FAIL;
    }
    else
    {
        uint64_t duration_n = 0;
        int aligned_face_counter = 0;

        for (size_t i = 0; i < net_boxes->len; i++)
        {
            // Align face
            if (ESP_FAIL == align_face2(net_boxes->landmark[i].landmark_p, image_matrix, aligned_face))
            {
                ESP_LOGW(TAG, "Face is not aligned.");
                matched_ids[i] = -2;
            }
            else
            {
                ESP_LOGI(tag, "Face is aligned.");

                uint64_t timestamp = esp_timer_get_time();
                matched_ids[i] = recognize_face(&g_id_list, aligned_face);
                duration_n += (esp_timer_get_time() - timestamp);
                aligned_face_counter++;

                ESP_LOGI(tag, "Match Face ID: %u", matched_ids[i]);
            }
        }

        *duration = (aligned_face_counter == 0) ? 0 : (uint32_t)(duration_n / aligned_face_counter / 1000);

        ESP_LOGI(tag, "Recognition duration: %ums", *duration);

        // Free aligned face
        dl_matrix3du_free(aligned_face);
    }

    return ESP_OK;
}

typedef enum
{
    DETECTION = 0,
    RECOGNITION = 1,
    ENROLLMENT = 2
} upload_mission_t;

static esp_err_t upload_handler(httpd_req_t *req)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~start\n");
    printf("RAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("SPIRAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    /* Predefined value */
    char tag[] = "upload_handler";
    int MAX_FRAME_SIZE = 1024;

    /* Declare variables */
    camera_fb_t jpeg;

    box_array_t *net_boxes = NULL;

    uint8_t *request_content = NULL;

    int8_t *matched_ids = NULL;
    enrollment_status_t enrollment_status = ENROLL_IDLE;
    int8_t enrolled_id = -1;

    uint32_t detection_duration = 0;
    uint32_t enrollment_duration = 0;
    uint32_t recognition_duration = 0;

    httpd_resp_set_type(req, "text/plain");

    /* Receive request content */
    printf("request_content 0-RAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("request_content 0-SPIRAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    printf("request_content len: %d\n", req->content_len);
    // request_content = (uint8_t *)malloc(req->content_len * sizeof(uint8_t));
    request_content = (uint8_t *)heap_caps_malloc(req->content_len * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    printf("request_content 1-RAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("request_content 1-SPIRAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    if (!request_content)
    {
        httpd_resp_send_500(req);

        ESP_LOGE(tag, "Can not malloc request_content.");
        return ESP_FAIL;
    }

    if (ESP_FAIL == httpd_req_recv_content_uint8(req, request_content, MAX_FRAME_SIZE))
    {
        free(request_content);
        return ESP_FAIL;
    }

    /* JPEG to Matrix */
    jpeg.len = httpd_req_get_hdr_value_int(req, "size");
    jpeg.width = httpd_req_get_hdr_value_int(req, "width");
    jpeg.height = httpd_req_get_hdr_value_int(req, "height");
    jpeg.format = PIXFORMAT_JPEG;
    jpeg.buf = request_content;
    for (size_t i = 0; i < 3; i++)
        jpeg.buf = memchr(jpeg.buf, '\n', 100) + 1;

    ESP_LOGI(tag, "heigth: %d", jpeg.height);
    ESP_LOGI(tag, "width : %d", jpeg.width);
    ESP_LOGI(tag, "len   : %d", jpeg.len);

    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, jpeg.width, jpeg.height, 3);
    if (!image_matrix)
    {
        httpd_resp_send_500(req);
        ESP_LOGE(tag, "dl_matrix3du_alloc failed");

        free(request_content);

        return ESP_FAIL;
    }

    if (!fmt2rgb888(jpeg.buf, jpeg.len, jpeg.format, image_matrix->item))
    {
        httpd_resp_send_500(req);
        ESP_LOGE(tag, "to rgb888 failed");

        free(request_content);
        dl_matrix3du_free(image_matrix);

        return ESP_FAIL;
    }

    free(request_content);

    /* Handle mission */
    upload_mission_t mission = httpd_req_get_hdr_value_int(req, "mission");
    if (mission == DETECTION || mission == RECOGNITION || mission == ENROLLMENT)
    {
        /* Detection */
        int64_t timestamp = esp_timer_get_time();
        net_boxes = face_detect(image_matrix, &mtmn_config);
        detection_duration = (uint32_t)((esp_timer_get_time() - timestamp) / 1000);
        ESP_LOGI(tag, "Detection duration: %ums", detection_duration);

        if (net_boxes)
        {
            ESP_LOGI(tag, "DETECTED.");

            switch (mission)
            {
            case RECOGNITION:
                // matched_ids = (int8_t *)malloc(net_boxes->len * sizeof(int8_t));
                matched_ids = (int8_t *)heap_caps_malloc(net_boxes->len * sizeof(int8_t), MALLOC_CAP_SPIRAM);
                if (!matched_ids)
                {
                    httpd_resp_send_500(req);
                    ESP_LOGE(tag, "Could not allocate aligned_face");

                    dl_matrix3du_free(image_matrix);
                    return ESP_FAIL;
                }

                if (ESP_FAIL == upload_handler_recognition(image_matrix, net_boxes, matched_ids, &recognition_duration))
                {
                    httpd_resp_send_500(req);
                    ESP_LOGE(tag, "Fail to recognizing");

                    dl_matrix3du_free(image_matrix);
                    free(matched_ids);
                    return ESP_FAIL;
                }
                break;

            case ENROLLMENT:
                enrollment_status = upload_handler_enrollment(image_matrix, net_boxes, &enrolled_id, &enrollment_duration);
                break;

            case DETECTION:
                break;

            default:
                ESP_LOGE(tag, "Out of mission selections.");
                break;
            }
        }
        else
        {
            ESP_LOGW(tag, "Not DETECTED.");
        }
    }
    else
    {
        ESP_LOGE(tag, "Out of mission selections.");
    }

    // Free image
    dl_matrix3du_free(image_matrix);

    /* Send response */
    upload_handler_response(req,
                            net_boxes,
                            matched_ids,
                            enrollment_status,
                            enrolled_id,
                            detection_duration,
                            recognition_duration,
                            enrollment_duration);

    /* Free */
    if (net_boxes)
    {
        free(net_boxes->score);
        free(net_boxes->box);
        free(net_boxes->landmark);
        free(net_boxes);
    }

    if (matched_ids)
        free(matched_ids);

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~end\n");
    printf("RAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("SPIRAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    return ESP_OK;
}

static esp_err_t upload_handler2(httpd_req_t *req)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~start\n");
    printf("RAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("RAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    /* Predefined value */
    char tag[] = "upload_handler2";
    int MAX_FRAME_SIZE = 1024;

    /* Declare variables */
    camera_fb_t jpeg;

    uint8_t *request_content = NULL;

    httpd_resp_set_type(req, "image/jpeg");

    /* Receive request content */
    printf("request_content 0-RAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("request_content 0-SPIRAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    printf("request_content len: %d\n", req->content_len);
    // request_content = (uint8_t *)malloc(req->content_len * sizeof(uint8_t));
    request_content = (uint8_t *)heap_caps_malloc(req->content_len * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    printf("request_content 1-RAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("request_content 1-SPIRAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    if (!request_content)
    {
        httpd_resp_send_500(req);

        ESP_LOGE(tag, "Can not malloc request_content.");
        return ESP_FAIL;
    }

    if (ESP_FAIL == httpd_req_recv_content_uint8(req, request_content, MAX_FRAME_SIZE))
    {
        free(request_content);
        return ESP_FAIL;
    }

    /* JPEG to Matrix */
    jpeg.len = httpd_req_get_hdr_value_int(req, "size");
    jpeg.width = httpd_req_get_hdr_value_int(req, "width");
    jpeg.height = httpd_req_get_hdr_value_int(req, "height");
    jpeg.format = PIXFORMAT_JPEG;
    jpeg.buf = request_content;
    for (size_t i = 0; i < 3; i++)
        jpeg.buf = memchr(jpeg.buf, '\n', 100) + 1;

    ESP_LOGI(tag, "heigth: %d", jpeg.height);
    ESP_LOGI(tag, "width : %d", jpeg.width);
    ESP_LOGI(tag, "len   : %d", jpeg.len);

    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, jpeg.width, jpeg.height, 3);
    if (!image_matrix)
    {
        httpd_resp_send_500(req);
        ESP_LOGE(tag, "dl_matrix3du_alloc failed");

        free(request_content);

        return ESP_FAIL;
    }

    if (!fmt2rgb888(jpeg.buf, jpeg.len, jpeg.format, image_matrix->item))
    {
        httpd_resp_send_500(req);
        ESP_LOGE(tag, "to rgb888 failed");

        free(request_content);
        dl_matrix3du_free(image_matrix);

        return ESP_FAIL;
    }
    free(request_content);

    /* image process */
    // slice
    // I (2934962) r_net_forward: square: (89, 191, 137, 239)
    // I (2934972) r_net_forward: square: (70, 49, 167, 145)
    // I (2934982) r_net_forward: square: (108, 160, 158, 209)
    // I (2934992) r_net_forward: square: (108, 45, 242, 179)
    // I (2935002) r_net_forward: square: (75, 144, 123, 192)
    // I (2935012) r_net_forward: square: (67, 121, 117, 171)
    // I (2935022) r_net_forward: square: (93, 171, 161, 239)
    // I (2935032) r_net_forward: square: (89, 153, 135, 200)
    // I (2935042) r_net_forward: square: (73, 15, 173, 115)
    // I (2935052) r_net_forward: square: (111, 88, 161, 137)
    // I (2935062) r_net_forward: square: (252, 174, 318, 239)
    // I (2935072) r_net_forward: square: (166, 24, 257, 115)
    // I (2935082) r_net_forward: square: (132, 120, 180, 169)
    // I (2935092) r_net_forward: square: (108, 184, 157, 233)
    // I (2935102) r_net_forward: square: (185, 171, 253, 239)
    // I (2935112) r_net_forward: square: (250, 49, 312, 111)
    // I (2935122) r_net_forward: square: (98, 97, 146, 145)
    // I (2935132) r_net_forward: square: (74, 162, 128, 216)
    // I (2935142) r_net_forward: square: (123, 144, 174, 195)
    // I (2935152) r_net_forward: square: (101, 144, 149, 192)
    // int x = 108;
    // int y = 45;
    // int x2 = 242;
    // int y2 = 179;
    // int w = x2 - x + 1;
    // int h = y2 - y + 1;
    // dl_matrix3du_t *sliced_image = dl_matrix3du_alloc(1, w, h, image_matrix->c);
    // dl_matrix3du_slice_copy(sliced_image, image_matrix, x, y, w, h);

    // fmt2jpg(sliced_image->item,
    //         sliced_image->w * sliced_image->h * sliced_image->c,
    //         sliced_image->w,
    //         sliced_image->h,
    //         PIXFORMAT_RGB888,
    //         90,
    //         &jpeg.buf,
    //         &jpeg.len);
    // dl_matrix3du_free(sliced_image);

    // image zoom in twice
    // image_zoom_in_twice(image_matrix->item, 159, 119, image_matrix->c, image_matrix->item, image_matrix->w, image_matrix->c);
    // image_matrix->w = 159;
    // image_matrix->h = 119;
    // jpeg.width = 159;
    // jpeg.height = 119;

    // fmt2jpg(image_matrix->item,
    //         image_matrix->w * image_matrix->h * image_matrix->c,
    //         image_matrix->w,
    //         image_matrix->h,
    //         PIXFORMAT_RGB888,
    //         90,
    //         &jpeg.buf,
    //         &jpeg.len);

    // image resize linear
    int resized_w = 16;
    int resized_h = 12;
    dl_matrix3du_t *resized_image = dl_matrix3du_alloc(1, resized_w, resized_h, image_matrix->c);
    image_resize_linear(resized_image->item, image_matrix->item, resized_w, resized_h, resized_image->c, image_matrix->w, image_matrix->h);
    fmt2jpg(resized_image->item,
            resized_image->w * resized_image->h * resized_image->c,
            resized_image->w,
            resized_image->h,
            PIXFORMAT_RGB888,
            90,
            &jpeg.buf,
            &jpeg.len);
    dl_matrix3du_free(resized_image);

    dl_matrix3du_free(image_matrix);

    httpd_resp_send(req, (const char *)jpeg.buf, jpeg.len);

    if (jpeg.buf)
        free(jpeg.buf);

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~end\n");
    printf("RAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("RAM size: %dB\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    return ESP_OK;
}

static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    return httpd_resp_send(req, (const char *)index_html_gz, index_html_gz_len);
}

void app_httpd_main()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL};

    httpd_uri_t status_uri = {
        .uri = "/status",
        .method = HTTP_GET,
        .handler = status_handler,
        .user_ctx = NULL};

    httpd_uri_t cmd_uri = {
        .uri = "/control",
        .method = HTTP_GET,
        .handler = cmd_handler,
        .user_ctx = NULL};

    httpd_uri_t capture_uri = {
        .uri = "/capture",
        .method = HTTP_GET,
        .handler = capture_handler,
        .user_ctx = NULL};

    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL};

    httpd_uri_t upload_uri = {
        .uri = "/upload",
        .method = HTTP_POST,
        .handler = upload_handler,
        .user_ctx = NULL};

    ra_filter_init(&ra_filter, 20);
#if CONFIG_ESP_FACE_DETECT_ENABLED
    mtmn_config.type = FAST;
    mtmn_config.pyramid = 0.707;/// if mtmn_config.type == FAST, the pyramid is set to sqrt(0.5) in default
    mtmn_config.min_face = 48;
    mtmn_config.pyramid_times = 5;
    mtmn_config.p_threshold.score = 0.6;
    mtmn_config.p_threshold.nms = 0.7;
    mtmn_config.p_threshold.candidate_number = 100;
    mtmn_config.r_threshold.score = 0.7;
    mtmn_config.r_threshold.nms = 0.7;
    mtmn_config.r_threshold.candidate_number = 100;
    mtmn_config.o_threshold.score = 0.7;
    mtmn_config.o_threshold.nms = 0.7;
    mtmn_config.o_threshold.candidate_number = 1;
#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
    face_id_init(&g_id_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
#endif
#endif
    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &cmd_uri);
        httpd_register_uri_handler(camera_httpd, &status_uri);
        httpd_register_uri_handler(camera_httpd, &capture_uri);
        httpd_register_uri_handler(camera_httpd, &upload_uri);
    }

    config.server_port += 1;
    config.ctrl_port += 1;
    ESP_LOGI(TAG, "Starting stream server on port: '%d'", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
}
