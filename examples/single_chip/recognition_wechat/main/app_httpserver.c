/* ESPRESSIF MIT License
 * 
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 * 
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "app_httpserver.h"
#include "esp_log.h"
#include "image_util.h"
#include "fb_gfx.h"
#include "app_main.h"

static const char *TAG = "app_httpserver";

#define FACE_COLOR_WHITE  0x00FFFFFF
#define FACE_COLOR_BLACK  0x00000000
#define FACE_COLOR_RED    0x000000FF
#define FACE_COLOR_GREEN  0x0000FF00
#define FACE_COLOR_BLUE   0x00FF0000
#define FACE_COLOR_YELLOW (FACE_COLOR_RED | FACE_COLOR_GREEN)
#define FACE_COLOR_CYAN   (FACE_COLOR_BLUE | FACE_COLOR_GREEN)
#define FACE_COLOR_PURPLE (FACE_COLOR_BLUE | FACE_COLOR_RED)


#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

extern face_id_name_list st_face_list;
static int8_t g_is_enrolling = 0;
static int8_t g_stop_stream = 0;
static int8_t g_detection_enabled = 0;
static int8_t g_recognition_enabled = 0;
typedef struct 
{
    char enroll_name[ENROLL_NAME_LEN];
} httpd_resp_value;

httpd_resp_value st_name;
QueueHandle_t gpst_input_image = NULL;
QueueHandle_t gpst_output = NULL;

// Profiling
static int64_t fr_start = 0;
static int64_t fr_ready = 0;
static int64_t fr_face = 0;
static int64_t fr_recognize = 0;
static int64_t fr_encode = 0;

//static void oneshot_timer_callback(void* arg);
char *number_suffix(int32_t number)
{
    uint8_t n = number % 10;

    if (n == 0)
        return "zero";
    else if (n == 1)
        return "st";
    else if (n == 2)
        return "nd";
    else if (n == 3)
        return "rd";
    else
        return "th";
}


static void rgb_print(void *image, int w, int h, uint32_t color, const char * str){
    fb_data_t fb;
    fb.width = w;
    fb.height = h;
    fb.data = image;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    fb_gfx_print(&fb, (fb.width - (strlen(str) * 14)) / 2, 10, color, str);
}

static int rgb_printf(void *image, int w, int h, uint32_t color, const char *format, ...)
{
    char loc_buf[64];
    char * temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if(len >= sizeof(loc_buf)){
        temp = (char*)malloc(len+1);
        if(temp == NULL) {
            return 0;
        }
    }
    vsnprintf(temp, len+1, format, arg);
    va_end(arg);
    rgb_print(image, w, h, color, temp);
    if(len > 64){
        free(temp);
    }
    return len;
}

static void draw_face_boxes(void *image, int width, int height, box_array_t *boxes){
    int x, y, w, h, i;
    uint32_t color = FACE_COLOR_YELLOW;
    fb_data_t fb;
    fb.width = width;
    fb.height = height;
    fb.data = image;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    for (i = 0; i < boxes->len; i++){
        // rectangle box
        x = (int)boxes->box[i].box_p[0];
        y = (int)boxes->box[i].box_p[1];
        w = (int)boxes->box[i].box_p[2] - x + 1;
        h = (int)boxes->box[i].box_p[3] - y + 1;
        fb_gfx_drawFastHLine(&fb, x, y, w, color);
        fb_gfx_drawFastHLine(&fb, x, y+h-1, w, color);
        fb_gfx_drawFastVLine(&fb, x, y, h, color);
        fb_gfx_drawFastVLine(&fb, x+w-1, y, h, color);
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

static inline int do_enrollment(face_id_name_list *face_list, dl_matrix3d_t *new_id)
{
    ESP_LOGD(TAG, "START ENROLLING");

    int left_sample_face = enroll_face_id_to_flash_with_name(face_list, new_id, st_name.enroll_name);
    ESP_LOGD(TAG, "Face ID %s Enrollment: Taken the %d%s sample",
            face_list->tail->id_name,
            ENROLL_CONFIRM_TIMES - left_sample_face,
            number_suffix(ENROLL_CONFIRM_TIMES - left_sample_face));
    gpio_set_level(GPIO_LED_RED, 0);
    return left_sample_face;
}

esp_err_t facenet_stream_handler(httpd_req_t *req)
{
    esp_err_t res = ESP_OK;
    g_state = START_STREAM;
    g_stop_stream = 0;

    camera_fb_t * fb = NULL;
    size_t _jpg_buf_len = 0;
    uint8_t * _jpg_buf = NULL;
    char * part_buf[64];

    http_img_process_result out_res = {0};

    static int64_t last_frame = 0;
    if(!last_frame) {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    ESP_LOGI(TAG, "Get count %d\n", st_face_list.count);

    while(true)
    {
        // update fsm state
        if (g_stop_stream)
        {
            g_stop_stream = 0;
            httpd_resp_send_404(req);
            break;
        }
        else if (g_is_enrolling)
        {
            g_state = START_ENROLL;
        }
        else if (g_recognition_enabled && (st_face_list.count > 0))
        {
            g_state = START_RECOGNITION;
        }
        else if (g_detection_enabled)
        {
            g_state = START_DETECT;
        }
        else
        {
            g_state = START_STREAM;
        }

        ESP_LOGD(TAG, "State: %d, count:%d", g_state, st_face_list.count);
        // exec event
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
            break;
        }

        if (g_state == START_STREAM)
        {
            _jpg_buf = fb->buf;
            _jpg_buf_len = fb->len;
            goto http_response;
        }

        fr_start = esp_timer_get_time();
        fr_ready = fr_start;
        fr_face = fr_start;
        fr_encode = fr_start;
        fr_recognize = fr_start;

        xQueueSend(gpst_input_image, &fb, portMAX_DELAY);

        xQueueReceive(gpst_output, &out_res, portMAX_DELAY);

        fr_recognize = fr_face;

        if (out_res.net_boxes)
        {
            draw_face_boxes(out_res.image, fb->width, fb->height, out_res.net_boxes);
            free(out_res.net_boxes->box);
            free(out_res.net_boxes->landmark);
            free(out_res.net_boxes);

            if (out_res.face_id)
            {
                if (g_state == START_ENROLL)
                {
                    int left_sample_face = do_enrollment(&st_face_list, out_res.face_id);
                    rgb_print(out_res.image, fb->width, fb->height, FACE_COLOR_YELLOW, "START ENROLLING");
                    rgb_printf(out_res.image, fb->width, fb->height, FACE_COLOR_CYAN, "\nThe %u%s sample",
                            ENROLL_CONFIRM_TIMES - left_sample_face,
                            number_suffix(ENROLL_CONFIRM_TIMES - left_sample_face));
                    if (left_sample_face == 0)
                    {
                        ESP_LOGI(TAG, "Enrolled Face ID: %s", st_face_list.tail->id_name);
                        rgb_printf(out_res.image, fb->width, fb->height, FACE_COLOR_CYAN, "\n\nFace ID: %s", st_face_list.tail->id_name);
                        g_is_enrolling = 0;
                        g_state = START_RECOGNITION;
                    }
                }
                else
                {
                    face_id_node *f = recognize_face_with_name(&st_face_list, out_res.face_id);

                    if (f)
                    {
                        gpio_set_level(GPIO_LED_RED, 1);
                        rgb_printf(out_res.image, fb->width, fb->height, FACE_COLOR_GREEN, "Hello %s", f->id_name);
                    }
                    else
                    {
                        rgb_print(out_res.image, fb->width, fb->height, FACE_COLOR_RED, "\nWHO?");
                    }
                }   // START_ENROLL
                dl_matrix3d_free(out_res.face_id);
            }
            //xSemaphoreGive(out_res.lock);

            if(!fmt2jpg(out_res.image, fb->width*fb->height*3, fb->width, fb->height, PIXFORMAT_RGB888, 90, &_jpg_buf, &_jpg_buf_len))
            {
                ESP_LOGE(TAG, "fmt2jpg failed");
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


http_response:
        if(res == ESP_OK){
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if(fb){
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        } else if(_jpg_buf){
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if(res != ESP_OK){
            break;
        }
        int64_t fr_end = esp_timer_get_time();

        int64_t ready_time = (fr_ready - fr_start)/1000;
        int64_t face_time = (fr_face - fr_ready)/1000;
        int64_t recognize_time = (fr_recognize - fr_face)/1000;
        int64_t encode_time = (fr_encode - fr_recognize)/1000;
        int64_t process_time = (fr_encode - fr_start)/1000;

        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        ESP_LOGI(TAG, "MJPG: %uKB %ums (%.1ffps), %u+%u+%u+%u=%u",
                (uint32_t)(_jpg_buf_len/1024),
                (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time,
                (uint32_t)ready_time, (uint32_t)face_time, (uint32_t)recognize_time, (uint32_t)encode_time, (uint32_t)process_time);
    }

    last_frame = 0;
    g_state = WAIT_FOR_CONNECT;
    return ESP_OK;
}

httpd_uri_t _face_stream_handler = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = facenet_stream_handler,
    .user_ctx  = NULL
};

static esp_err_t cmd_handler(httpd_req_t *req){
    char*  buf;
    size_t buf_len;
    char variable[32] = {0,};
    char value[FACE_ID_SAVE_NUMBER * ENROLL_NAME_LEN] = {0,};

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        if(!buf){
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) == ESP_OK &&
                    httpd_query_key_value(buf, "val", value, sizeof(value)) == ESP_OK) {
            } else {
                free(buf);
                httpd_resp_send_404(req);
                return ESP_FAIL;
            }
        } else {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    } else {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
int val = atoi(value);
    ESP_LOGI(TAG, "%s = %d", variable, val);
    sensor_t * s = esp_camera_sensor_get();
    int res = 0;

    if(!strcmp(variable, "framesize")) {
        if(s->pixformat == PIXFORMAT_JPEG) res = s->set_framesize(s, (framesize_t)val);
    }
    else if(!strcmp(variable, "quality")) res = s->set_quality(s, val);
    else if(!strcmp(variable, "contrast")) res = s->set_contrast(s, val);
    else if(!strcmp(variable, "brightness")) res = s->set_brightness(s, val);
    else if(!strcmp(variable, "saturation")) res = s->set_saturation(s, val);
    else if(!strcmp(variable, "gainceiling")) res = s->set_gainceiling(s, (gainceiling_t)val);
    else if(!strcmp(variable, "colorbar")) res = s->set_colorbar(s, val);
    else if(!strcmp(variable, "awb")) res = s->set_whitebal(s, val);
    else if(!strcmp(variable, "agc")) res = s->set_gain_ctrl(s, val);
    else if(!strcmp(variable, "aec")) res = s->set_exposure_ctrl(s, val);
    else if(!strcmp(variable, "hmirror")) res = s->set_hmirror(s, val);
    else if(!strcmp(variable, "vflip")) res = s->set_vflip(s, val);
    else if(!strcmp(variable, "awb_gain")) res = s->set_awb_gain(s, val);
    else if(!strcmp(variable, "agc_gain")) res = s->set_agc_gain(s, val);
    else if(!strcmp(variable, "aec_value")) res = s->set_aec_value(s, val);
    else if(!strcmp(variable, "aec2")) res = s->set_aec2(s, val);
    else if(!strcmp(variable, "dcw")) res = s->set_dcw(s, val);
    else if(!strcmp(variable, "bpc")) res = s->set_bpc(s, val);
    else if(!strcmp(variable, "wpc")) res = s->set_wpc(s, val);
    else if(!strcmp(variable, "raw_gma")) res = s->set_raw_gma(s, val);
    else if(!strcmp(variable, "lenc")) res = s->set_lenc(s, val);
    else if(!strcmp(variable, "special_effect")) res = s->set_special_effect(s, val);
    else if(!strcmp(variable, "wb_mode")) res = s->set_wb_mode(s, val);
    else if(!strcmp(variable, "ae_level")) res = s->set_ae_level(s, val);

    else if(!strcmp(variable, "stop_stream")) g_stop_stream = 1;
    else if(!strcmp(variable, "face_detect")) {
        g_detection_enabled = val;
        if(!g_detection_enabled) {
            g_recognition_enabled = 0;
            g_is_enrolling = 0;
        }
    }
    else if(!strcmp(variable, "face_enroll")) 
    {
        if (!g_is_enrolling && g_recognition_enabled)
        {
            memcpy(st_name.enroll_name, value, strlen(value) + 1);
            g_is_enrolling = 1;
        }
    }
    else if(!strcmp(variable, "face_delete")) 
    {
        // ["aaa","bbb"]
        char delete_name[ENROLL_NAME_LEN * FACE_ID_SAVE_NUMBER];
        uint8_t len = strlen(value) - strlen("%5b%22") - strlen("%22%5d");
        memcpy(delete_name, value + strlen("%5b%22"), len);
        delete_name[len] = 0;
        char *p = delete_name;
        char *q = NULL;

        do
        {
            q = strstr(p, "%22%2C%22"); 
            if (!q)
                break;
            p[q - p] = 0;
            int8_t left = delete_face_id_in_flash_with_name(&st_face_list, p);
            if (left > 0)
                ESP_LOGW(TAG, "%s ID Delete", p);
            p = q + 9;
            q = strstr(p, "%22%2C%22");
        } while (q);
        int8_t left = delete_face_id_in_flash_with_name(&st_face_list, p);
        if (left > 0)
            ESP_LOGW(TAG, "%s ID Delete", p);
    }
    else if(!strcmp(variable, "delete_all")) 
    {
        delete_face_all_in_flash_with_name(&st_face_list);
    }
    else if(!strcmp(variable, "face_recognize")) {
        g_recognition_enabled = val;
        if(g_recognition_enabled){
            g_detection_enabled = val;
        }
    }
    else if(!strcmp(variable, "get_id")) {
        static char id_json[1024];
        char *p = id_json;
        *p++ = '{';
        p += sprintf(p, "\"len\":%d,", st_face_list.count);
        p += sprintf(p, "\"list\":[");
        face_id_node *head = st_face_list.head;
        for (int i = 0; i < st_face_list.count; i++)
        {
            p += sprintf(p, "\"%s\",", head->id_name);
            head = head->next;
        }
        if (st_face_list.count)
            p--;
        *p++ = ']';
        *p++ = '}';
        *p++ = 0;
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
        return httpd_resp_send(req, id_json, strlen(id_json));
    }
    else {
        res = -1;
    }

    if(res){
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

httpd_uri_t cmd_uri = {
    .uri       = "/control",
    .method    = HTTP_GET,
    .handler   = cmd_handler,
    .user_ctx  = NULL
};
httpd_handle_t camera_httpd = NULL;


static esp_err_t status_handler(httpd_req_t *req){
    static char json_response[1024];

    sensor_t * s = esp_camera_sensor_get();
    char * p = json_response;
    *p++ = '{';

    p+=sprintf(p, "\"framesize\":%u,", s->status.framesize);
    p+=sprintf(p, "\"quality\":%u,", s->status.quality);
    p+=sprintf(p, "\"brightness\":%d,", s->status.brightness);
    p+=sprintf(p, "\"contrast\":%d,", s->status.contrast);
    p+=sprintf(p, "\"saturation\":%d,", s->status.saturation);
    p+=sprintf(p, "\"special_effect\":%u,", s->status.special_effect);
    p+=sprintf(p, "\"wb_mode\":%u,", s->status.wb_mode);
    p+=sprintf(p, "\"awb\":%u,", s->status.awb);
    p+=sprintf(p, "\"awb_gain\":%u,", s->status.awb_gain);
    p+=sprintf(p, "\"aec\":%u,", s->status.aec);
    p+=sprintf(p, "\"aec2\":%u,", s->status.aec2);
    p+=sprintf(p, "\"ae_level\":%d,", s->status.ae_level);
    p+=sprintf(p, "\"aec_value\":%u,", s->status.aec_value);
    p+=sprintf(p, "\"agc\":%u,", s->status.agc);
    p+=sprintf(p, "\"agc_gain\":%u,", s->status.agc_gain);
    p+=sprintf(p, "\"gainceiling\":%u,", s->status.gainceiling);
    p+=sprintf(p, "\"bpc\":%u,", s->status.bpc);
    p+=sprintf(p, "\"wpc\":%u,", s->status.wpc);
    p+=sprintf(p, "\"raw_gma\":%u,", s->status.raw_gma);
    p+=sprintf(p, "\"lenc\":%u,", s->status.lenc);
    p+=sprintf(p, "\"hmirror\":%u,", s->status.hmirror);
    p+=sprintf(p, "\"dcw\":%u,", s->status.dcw);
    p+=sprintf(p, "\"colorbar\":%u", s->status.colorbar);

    p+=sprintf(p, ",\"face_detect\":%u", g_detection_enabled);
    p+=sprintf(p, ",\"face_enroll\":%u,", g_is_enrolling);
    p+=sprintf(p, "\"face_recognize\":%u", g_recognition_enabled);
    *p++ = '}';
    *p++ = 0;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}
httpd_uri_t status_uri = {
    .uri       = "/status",
    .method    = HTTP_GET,
    .handler   = status_handler,
    .user_ctx  = NULL
};
#if 0
const esp_timer_create_args_t oneshot_timer_args = {
    .callback = &oneshot_timer_callback,
    /* argument specified here will be passed to timer callback function */
    .name = "one-shot"
};
esp_timer_handle_t oneshot_timer;

static void oneshot_timer_callback(void* arg)
{
    if(g_state != START_ENROLL)
        g_is_enrolling = 1;
}


static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    esp_err_t err = esp_timer_start_once(oneshot_timer, 500000);
    if(err == ESP_ERR_INVALID_STATE)
    {
        ESP_ERROR_CHECK(esp_timer_stop(oneshot_timer));
        g_is_enrolling = 0;
        g_is_deleting = 1;
        gpio_set_level(GPIO_LED_WHITE, 0);
    }
}
#endif

void app_httpserver_init ()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 4096 * 2;

#if 0
    ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args, &oneshot_timer));

    gpio_config_t io_conf = {0};
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    io_conf.pin_bit_mask = 1LL << GPIO_BUTTON;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    gpio_isr_handler_add(GPIO_BUTTON, gpio_isr_handler, NULL);
#endif

    gpst_input_image = xQueueCreate(1, sizeof(void *));
    gpst_output = xQueueCreate(1, sizeof(http_img_process_result));


    if (httpd_start(&camera_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(camera_httpd, &cmd_uri);
        httpd_register_uri_handler(camera_httpd, &status_uri);
    }
    config.server_port += 1;
    config.ctrl_port += 1;
    if (httpd_start(&camera_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(camera_httpd, &_face_stream_handler);
    }
}
