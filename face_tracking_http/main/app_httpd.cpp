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
#include "driver/ledc.h"
#include "sdkconfig.h"
#include "app_mdns.h"
#include "app_camera.h"
#include "fd_forward.h"
#include "array_real.hpp"
#include "kcf.hpp"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char *TAG = "camera_httpd";
#endif

#define FACE_COLOR_WHITE 0x00FFFFFF
#define FACE_COLOR_BLACK 0x00000000
#define FACE_COLOR_RED 0x000000FF
#define FACE_COLOR_GREEN 0x0000FF00
#define FACE_COLOR_BLUE 0x00FF0000
#define FACE_COLOR_YELLOW (FACE_COLOR_RED | FACE_COLOR_GREEN)
#define FACE_COLOR_CYAN (FACE_COLOR_BLUE | FACE_COLOR_GREEN)
#define FACE_COLOR_PURPLE (FACE_COLOR_BLUE | FACE_COLOR_RED)

typedef struct
{
    httpd_req_t *req;
    size_t len;
} jpg_chunking_t;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

static mtmn_config_t mtmn_config = {0};
static bool is_enable_tracker = false;

static void rgb_print(ArrayReal<uint8_t, float, 3> &image, uint32_t color, const char *str)
{
    fb_data_t fb;
    fb.height = image.get_shape()[0];
    fb.width = image.get_shape()[1];
    fb.data = image.get_item();
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    fb_gfx_print(&fb, 5, 5, color, str);
}

static int rgb_printf(ArrayReal<uint8_t, float, 3> &image, uint32_t color, const char *format, ...)
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
    rgb_print(image, color, temp);
    if (len > 64)
    {
        free(temp);
    }
    return len;
}

static void draw_target(ArrayReal<uint8_t, float, 3> &image, struct rectangle_side_t<int> box)
{
    fb_data_t fb;

    fb.height = image.get_shape()[0];
    fb.width = image.get_shape()[1];
    fb.data = image.get_item();
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;

    int right_down_x = box.x + box.width - 1;
    int right_down_y = box.y + box.height - 1;

    fb_gfx_drawFastHLine(&fb, box.x, box.y, box.width, FACE_COLOR_YELLOW);
    fb_gfx_drawFastHLine(&fb, box.x, right_down_y, box.width, FACE_COLOR_YELLOW);
    fb_gfx_drawFastVLine(&fb, box.x, box.y, box.height, FACE_COLOR_YELLOW);
    fb_gfx_drawFastVLine(&fb, right_down_x, box.y, box.height, FACE_COLOR_YELLOW);
}

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

static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    struct timeval _timestamp;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[128];

    int image_shape[] = {240, 320, 3};
    ArrayReal<uint8_t, float, 3> image_array(image_shape, NOT_ALLOC);
    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, image_shape[1], image_shape[0], 3);

    KCF<float> tracker;
    bool is_init_tracker = true;

    struct rectangle_side_t<int> target;
    box_array_t *box = NULL;

    int64_t timestamp_start = 0;
    int64_t timestamp_end = 0;
    int64_t timestamp_track_start = 0;
    int64_t timestamp_track_end = 0;
    int64_t timestamp_proc_end = 0;

    uint32_t duration_total = 0;
    uint32_t duration_jpeg = 0;
    uint32_t duration_track = 0;
    uint32_t duration_wifi = 0;
    uint32_t duration_proc = 0;

    int64_t timestamp = 0;

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
    {
        dl_matrix3du_free(image_matrix);
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "60");

    while (true)
    {
        timestamp_start = esp_timer_get_time();

        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_OK;
        }
        else
        {
            _timestamp.tv_sec = fb->timestamp.tv_sec;
            _timestamp.tv_usec = fb->timestamp.tv_usec;

            timestamp = esp_timer_get_time();
            if (!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item))
            {
                ESP_LOGE(TAG, "fmt2rgb888 failed");
                res = ESP_OK;
            }
            else
            {
                printf("decoding: %ums\n", (uint32_t)(esp_timer_get_time() - timestamp) / 1000);
                timestamp_track_start = esp_timer_get_time();
                if (_jpg_buf)
                {
                    free(_jpg_buf);
                    _jpg_buf = NULL;
                }

                image_array.set_item(image_matrix->item, false, false);

                // TODO: tracking
                if (is_enable_tracker)
                {
                    if (is_init_tracker)
                    {
                        // TODO: get target
                        box = face_detect(image_matrix, &mtmn_config);

                        if (box)
                        {
                            target.width = box->box[0].box_p[2] - box->box[0].box_p[0] + 1;
                            target.height = target.width;

                            target.x = (box->box[0].box_p[2] + box->box[0].box_p[0] - target.width) / 2;
                            target.y = (box->box[0].box_p[3] + box->box[0].box_p[1] - target.height) / 2;

                            dl_lib_free(box->score);
                            dl_lib_free(box->box);
                            dl_lib_free(box->landmark);
                            dl_lib_free(box);

                            // TODO: init tracker
                            tracker.init(image_array, target.x, target.y, target.width, target.height);
                            is_init_tracker = false;

                            // TODO: draw target
                            draw_target(image_array, target);
                        }
                    }
                    else
                    {
                        struct rectangle_side_t<float> rot = tracker.update(image_array);
                        target.x = (int)rot.x;
                        target.y = (int)rot.y;
                        target.width = (int)rot.width;
                        target.height = (int)rot.height;

                        // TODO: draw target
                        draw_target(image_array, target);
                    }
                }
                else // TODO: detecting
                {
                    box = face_detect(image_matrix, &mtmn_config);

                    if (box)
                    {
                        target.x = box->box[0].box_p[0];
                        target.y = box->box[0].box_p[1];
                        target.width = box->box[0].box_p[2] - target.x + 1;
                        target.height = box->box[0].box_p[3] - target.y + 1;

                        dl_lib_free(box->score);
                        dl_lib_free(box->box);
                        dl_lib_free(box->landmark);
                        dl_lib_free(box);

                        draw_target(image_array, target);
                        is_init_tracker = true;
                    }
                }

                timestamp_track_end = esp_timer_get_time();

                // rgb_printf(image_array, FACE_COLOR_CYAN, "Track:%u|JPEG:%u|WiFi:%u", duration_track, duration_jpeg, duration_wifi);
                rgb_printf(image_array, FACE_COLOR_CYAN, "Track:%ums\nJPEG:%ums\nWiFi:%ums", duration_track, duration_jpeg, duration_wifi);
                // rgb_printf(image_array, FACE_COLOR_CYAN, "T:%u", duration_track);

                timestamp = esp_timer_get_time();
                if (!fmt2jpg(image_array.get_item(), image_array.get_length(), image_array.get_shape()[1], image_array.get_shape()[0], PIXFORMAT_RGB888, 90, &_jpg_buf, &_jpg_buf_len))
                    ESP_LOGE(TAG, "fmt2jpg failed");
                printf("encoding: %ums\n", (uint32_t)(esp_timer_get_time() - timestamp) / 1000);
            }
            esp_camera_fb_return(fb);
            fb = NULL;
        }
        timestamp_proc_end = esp_timer_get_time();

        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if (res == ESP_OK)
        {
            size_t hlen = snprintf((char *)part_buf, 128, _STREAM_PART, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }

        if (res != ESP_OK)
        {
            break;
        }

        timestamp_end = esp_timer_get_time();

        duration_total = (timestamp_end - timestamp_start) / 1000;
        duration_proc = (timestamp_proc_end - timestamp_start) / 1000;
        duration_track = (timestamp_track_end - timestamp_track_start) / 1000;
        duration_jpeg = duration_proc - duration_track;
        duration_wifi = duration_total - duration_proc;
        ESP_LOGI(TAG, "%f FPs", 1000000.0 / (float)(timestamp_end - timestamp_start));
    }

    dl_matrix3du_free(image_matrix);

    return res;
}

static esp_err_t parse_get(httpd_req_t *req, char **obuf)
{
    char *buf = NULL;
    size_t buf_len = 0;

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
            *obuf = buf;
            return ESP_OK;
        }
        free(buf);
    }
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

static esp_err_t cmd_handler(httpd_req_t *req)
{
    char *buf = NULL;
    char variable[32];
    char value[32];

    if (parse_get(req, &buf) != ESP_OK ||
        httpd_query_key_value(buf, "var", variable, sizeof(variable)) != ESP_OK ||
        httpd_query_key_value(buf, "val", value, sizeof(value)) != ESP_OK)
    {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int val = atoi(value);
    ESP_LOGI(TAG, "%s = %d", variable, val);
    sensor_t *s = esp_camera_sensor_get();
    int res = 0;

    if (!strcmp(variable, "framesize"))
    {
        if (s->pixformat == PIXFORMAT_JPEG)
        {
            res = s->set_framesize(s, (framesize_t)val);
            if (res == 0)
            {
                app_mdns_update_framesize(val);
            }
        }
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
    else if (!strcmp(variable, "face_detect"))
        is_enable_tracker = val;
    else
        res = -1;

    if (res)
        return httpd_resp_send_500(req);

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static int print_reg(char *p, sensor_t *s, uint16_t reg, uint32_t mask)
{
    return sprintf(p, "\"0x%x\":%u,", reg, s->get_reg(s, reg, mask));
}

static esp_err_t status_handler(httpd_req_t *req)
{
    static char json_response[1024];

    sensor_t *s = esp_camera_sensor_get();
    char *p = json_response;
    *p++ = '{';

    if (s->id.PID == OV5640_PID || s->id.PID == OV3660_PID)
    {
        for (int reg = 0x3400; reg < 0x3406; reg += 2)
        {
            p += print_reg(p, s, reg, 0xFFF); //12 bit
        }
        p += print_reg(p, s, 0x3406, 0xFF);

        p += print_reg(p, s, 0x3500, 0xFFFF0); //16 bit
        p += print_reg(p, s, 0x3503, 0xFF);
        p += print_reg(p, s, 0x350a, 0x3FF);  //10 bit
        p += print_reg(p, s, 0x350c, 0xFFFF); //16 bit

        for (int reg = 0x5480; reg <= 0x5490; reg++)
        {
            p += print_reg(p, s, reg, 0xFF);
        }

        for (int reg = 0x5380; reg <= 0x538b; reg++)
        {
            p += print_reg(p, s, reg, 0xFF);
        }

        for (int reg = 0x5580; reg < 0x558a; reg++)
        {
            p += print_reg(p, s, reg, 0xFF);
        }
        p += print_reg(p, s, 0x558a, 0x1FF); //9 bit
    }
    else
    {
        p += print_reg(p, s, 0xd3, 0xFF);
        p += print_reg(p, s, 0x111, 0xFF);
        p += print_reg(p, s, 0x132, 0xFF);
    }

    p += sprintf(p, "\"board\":\"%s\",", CAM_BOARD);
    p += sprintf(p, "\"xclk\":%u,", s->xclk_freq_hz / 1000000);
    p += sprintf(p, "\"pixformat\":%u,", s->pixformat);
    p += sprintf(p, "\"framesize\":%u,", s->status.framesize);
    p += sprintf(p, "\"quality\":%u,", s->status.quality);
    p += sprintf(p, "\"brightness\":%d,", s->status.brightness);
    p += sprintf(p, "\"contrast\":%d,", s->status.contrast);
    p += sprintf(p, "\"saturation\":%d,", s->status.saturation);
    p += sprintf(p, "\"sharpness\":%d,", s->status.sharpness);
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
    p += sprintf(p, ",\"face_detect\":%u", is_enable_tracker);
    *p++ = '}';
    *p++ = 0;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t mdns_handler(httpd_req_t *req)
{
    size_t json_len = 0;
    const char *json_response = app_mdns_query(&json_len);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, json_len);
}

static esp_err_t xclk_handler(httpd_req_t *req)
{
    char *buf = NULL;
    char _xclk[32];

    if (parse_get(req, &buf) != ESP_OK ||
        httpd_query_key_value(buf, "xclk", _xclk, sizeof(_xclk)) != ESP_OK)
    {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int xclk = atoi(_xclk);
    ESP_LOGI(TAG, "Set XCLK: %d MHz", xclk);

    sensor_t *s = esp_camera_sensor_get();
    int res = s->set_xclk(s, LEDC_TIMER_0, xclk);
    if (res)
    {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t reg_handler(httpd_req_t *req)
{
    char *buf = NULL;
    char _reg[32];
    char _mask[32];
    char _val[32];

    if (parse_get(req, &buf) != ESP_OK ||
        httpd_query_key_value(buf, "reg", _reg, sizeof(_reg)) != ESP_OK ||
        httpd_query_key_value(buf, "mask", _mask, sizeof(_mask)) != ESP_OK ||
        httpd_query_key_value(buf, "val", _val, sizeof(_val)) != ESP_OK)
    {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int reg = atoi(_reg);
    int mask = atoi(_mask);
    int val = atoi(_val);
    ESP_LOGI(TAG, "Set Register: reg: 0x%02x, mask: 0x%02x, value: 0x%02x", reg, mask, val);

    sensor_t *s = esp_camera_sensor_get();
    int res = s->set_reg(s, reg, mask, val);
    if (res)
    {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t greg_handler(httpd_req_t *req)
{
    char *buf = NULL;
    char _reg[32];
    char _mask[32];

    if (parse_get(req, &buf) != ESP_OK ||
        httpd_query_key_value(buf, "reg", _reg, sizeof(_reg)) != ESP_OK ||
        httpd_query_key_value(buf, "mask", _mask, sizeof(_mask)) != ESP_OK)
    {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int reg = atoi(_reg);
    int mask = atoi(_mask);
    sensor_t *s = esp_camera_sensor_get();
    int res = s->get_reg(s, reg, mask);
    if (res < 0)
    {
        return httpd_resp_send_500(req);
    }
    ESP_LOGI(TAG, "Get Register: reg: 0x%02x, mask: 0x%02x, value: 0x%02x", reg, mask, res);

    char buffer[20];
    const char *val = itoa(res, buffer, 10);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, val, strlen(val));
}

static int parse_get_var(char *buf, const char *key, int def)
{
    char _int[16];
    if (httpd_query_key_value(buf, key, _int, sizeof(_int)) != ESP_OK)
    {
        return def;
    }
    return atoi(_int);
}

static esp_err_t pll_handler(httpd_req_t *req)
{
    char *buf = NULL;

    if (parse_get(req, &buf) != ESP_OK)
    {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    int bypass = parse_get_var(buf, "bypass", 0);
    int mul = parse_get_var(buf, "mul", 0);
    int sys = parse_get_var(buf, "sys", 0);
    int root = parse_get_var(buf, "root", 0);
    int pre = parse_get_var(buf, "pre", 0);
    int seld5 = parse_get_var(buf, "seld5", 0);
    int pclken = parse_get_var(buf, "pclken", 0);
    int pclk = parse_get_var(buf, "pclk", 0);
    free(buf);

    ESP_LOGI(TAG, "Set Pll: bypass: %d, mul: %d, sys: %d, root: %d, pre: %d, seld5: %d, pclken: %d, pclk: %d", bypass, mul, sys, root, pre, seld5, pclken, pclk);
    sensor_t *s = esp_camera_sensor_get();
    int res = s->set_pll(s, bypass, mul, sys, root, pre, seld5, pclken, pclk);
    if (res)
    {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t win_handler(httpd_req_t *req)
{
    char *buf = NULL;

    if (parse_get(req, &buf) != ESP_OK)
    {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    int startX = parse_get_var(buf, "sx", 0);
    int startY = parse_get_var(buf, "sy", 0);
    int endX = parse_get_var(buf, "ex", 0);
    int endY = parse_get_var(buf, "ey", 0);
    int offsetX = parse_get_var(buf, "offx", 0);
    int offsetY = parse_get_var(buf, "offy", 0);
    int totalX = parse_get_var(buf, "tx", 0);
    int totalY = parse_get_var(buf, "ty", 0);
    int outputX = parse_get_var(buf, "ox", 0);
    int outputY = parse_get_var(buf, "oy", 0);
    bool scale = parse_get_var(buf, "scale", 0) == 1;
    bool binning = parse_get_var(buf, "binning", 0) == 1;
    free(buf);

    ESP_LOGI(TAG, "Set Window: Start: %d %d, End: %d %d, Offset: %d %d, Total: %d %d, Output: %d %d, Scale: %u, Binning: %u", startX, startY, endX, endY, offsetX, offsetY, totalX, totalY, outputX, outputY, scale, binning);
    sensor_t *s = esp_camera_sensor_get();
    int res = s->set_res_raw(s, startX, startY, endX, endY, offsetX, offsetY, totalX, totalY, outputX, outputY, scale, binning);
    if (res)
    {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t index_handler(httpd_req_t *req)
{
    extern const unsigned char index_ov2640_html_gz_start[] asm("_binary_index_ov2640_html_gz_start");
    extern const unsigned char index_ov2640_html_gz_end[] asm("_binary_index_ov2640_html_gz_end");
    size_t index_ov2640_html_gz_len = index_ov2640_html_gz_end - index_ov2640_html_gz_start;

    extern const unsigned char index_ov3660_html_gz_start[] asm("_binary_index_ov3660_html_gz_start");
    extern const unsigned char index_ov3660_html_gz_end[] asm("_binary_index_ov3660_html_gz_end");
    size_t index_ov3660_html_gz_len = index_ov3660_html_gz_end - index_ov3660_html_gz_start;

    extern const unsigned char index_ov5640_html_gz_start[] asm("_binary_index_ov5640_html_gz_start");
    extern const unsigned char index_ov5640_html_gz_end[] asm("_binary_index_ov5640_html_gz_end");
    size_t index_ov5640_html_gz_len = index_ov5640_html_gz_end - index_ov5640_html_gz_start;

    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL)
    {
        if (s->id.PID == OV3660_PID)
        {
            return httpd_resp_send(req, (const char *)index_ov3660_html_gz_start, index_ov3660_html_gz_len);
        }
        else if (s->id.PID == OV5640_PID)
        {
            return httpd_resp_send(req, (const char *)index_ov5640_html_gz_start, index_ov5640_html_gz_len);
        }
        else
        {
            return httpd_resp_send(req, (const char *)index_ov2640_html_gz_start, index_ov2640_html_gz_len);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Camera sensor not found");
        return httpd_resp_send_500(req);
    }
}

static esp_err_t monitor_handler(httpd_req_t *req)
{
    extern const unsigned char monitor_html_gz_start[] asm("_binary_monitor_html_gz_start");
    extern const unsigned char monitor_html_gz_end[] asm("_binary_monitor_html_gz_end");
    size_t monitor_html_gz_len = monitor_html_gz_end - monitor_html_gz_start;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    return httpd_resp_send(req, (const char *)monitor_html_gz_start, monitor_html_gz_len);
}

void app_httpd_main()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 12;

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

    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL};

    httpd_uri_t xclk_uri = {
        .uri = "/xclk",
        .method = HTTP_GET,
        .handler = xclk_handler,
        .user_ctx = NULL};

    httpd_uri_t reg_uri = {
        .uri = "/reg",
        .method = HTTP_GET,
        .handler = reg_handler,
        .user_ctx = NULL};

    httpd_uri_t greg_uri = {
        .uri = "/greg",
        .method = HTTP_GET,
        .handler = greg_handler,
        .user_ctx = NULL};

    httpd_uri_t pll_uri = {
        .uri = "/pll",
        .method = HTTP_GET,
        .handler = pll_handler,
        .user_ctx = NULL};

    httpd_uri_t win_uri = {
        .uri = "/resolution",
        .method = HTTP_GET,
        .handler = win_handler,
        .user_ctx = NULL};

    httpd_uri_t mdns_uri = {
        .uri = "/mdns",
        .method = HTTP_GET,
        .handler = mdns_handler,
        .user_ctx = NULL};

    httpd_uri_t monitor_uri = {
        .uri = "/monitor",
        .method = HTTP_GET,
        .handler = monitor_handler,
        .user_ctx = NULL};

    mtmn_config.type = FAST;
    mtmn_config.min_face = 80;
    mtmn_config.pyramid = 0.707;
    mtmn_config.pyramid_times = 4;
    mtmn_config.p_threshold.score = 0.6;
    mtmn_config.p_threshold.nms = 0.7;
    mtmn_config.p_threshold.candidate_number = 20;
    mtmn_config.r_threshold.score = 0.7;
    mtmn_config.r_threshold.nms = 0.7;
    mtmn_config.r_threshold.candidate_number = 10;
    mtmn_config.o_threshold.score = 0.7;
    mtmn_config.o_threshold.nms = 0.7;
    mtmn_config.o_threshold.candidate_number = 1;

    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &cmd_uri);
        httpd_register_uri_handler(camera_httpd, &status_uri);
        // httpd_register_uri_handler(camera_httpd, &capture_uri);

        httpd_register_uri_handler(camera_httpd, &xclk_uri);
        httpd_register_uri_handler(camera_httpd, &reg_uri);
        httpd_register_uri_handler(camera_httpd, &greg_uri);
        httpd_register_uri_handler(camera_httpd, &pll_uri);
        httpd_register_uri_handler(camera_httpd, &win_uri);

        httpd_register_uri_handler(camera_httpd, &mdns_uri);
        httpd_register_uri_handler(camera_httpd, &monitor_uri);
    }

    config.server_port += 1;
    config.ctrl_port += 1;
    ESP_LOGI(TAG, "Starting stream server on port: '%d'", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
}
