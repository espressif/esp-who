#include "http_server.hpp"
#include "esp_http_server.h"
#include "esp_log.h"
#include "frame_cap_pipeline.hpp"
#include "who_cam.hpp"

static const char *TAG = "HTTP_SERVER";
static httpd_handle_t server = NULL;
static who::cam::WhoCam *g_cam = NULL;

static esp_err_t jpg_stream_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "multipart/x-mixed-replace; boundary=frame");
    
    while (true) {
        cam_fb_t *fb = g_cam->cam_fb_get();
        if (fb == NULL) {
            ESP_LOGE(TAG, "Failed to get frame buffer");
            continue;
        }
        
        char *part_buf = (char *)malloc(64 + fb->len);
        if (part_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory");
            g_cam->cam_fb_return(fb);
            continue;
        }
        
        int part_len = snprintf(part_buf, 64 + fb->len,
                               "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %zu\r\n\r\n",
                               fb->len);
        
        memcpy(part_buf + part_len, fb->buf, fb->len);
        part_len += fb->len;
        
        esp_err_t ret = httpd_resp_send_chunk(req, part_buf, part_len);
        free(part_buf);
        g_cam->cam_fb_return(fb);
        
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to send frame: %s", esp_err_to_name(ret));
            break;
        }
        
        // Yield to other tasks
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    return ESP_OK;
}

static const httpd_uri_t jpg_uri = {
    .uri = "/jpg",
    .method = HTTP_GET,
    .handler = jpg_stream_handler,
    .user_ctx = NULL
};

esp_err_t start_http_server(who::cam::WhoCam *cam)
{
    g_cam = cam;
    
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.max_uri_handlers = 2;
    
    ESP_LOGI(TAG, "Starting HTTP server on port %d", config.server_port);
    
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return ESP_FAIL;
    }
    
    httpd_register_uri_handler(server, &jpg_uri);
    
    return ESP_OK;
}

esp_err_t stop_http_server(void)
{
    if (server) {
        httpd_stop(server);
        server = NULL;
    }
    return ESP_OK;
}