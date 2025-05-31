#include "who_recognition_app.hpp"
#include "who_spiflash_fatfs.hpp"

#include "esp_netif.h"
#include <esp_log.h>
#include <nvs_flash.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>
// #include "protocol_examples_common.h"
// #include "protocol_examples_utils.h"
#include "esp_event.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
// #include "esp_netif.h"
#include "esp_check.h"
#include "esp_tls.h"

#include "nvs_flash.h"
#include <esp_system.h>
#include <esp_wifi.h>
// #include "esp_eth.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "http_lcd.hpp"
#include "mqtt_handler.hpp"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// static const char* html =
//         "<!DOCTYPE html><html><body>"
//         "<h2>Recognition</h2>"
//         "<img src=\"/recognize_stream\" style=\"max-width:100%;height:auto;\"><br><br>"
//         "<form action=\"/recognizebt\" method=\"post\"><button type=\"submit\">Recognize</button></form>"
//         "<form action=\"/enrollbt\" method=\"post\"><button type=\"submit\">Enroll</button></form>"
//         "<form action=\"/deletebt\" method=\"post\"><button type=\"submit\">Delete</button></form>"
//         "</body></html>";

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

#define EXAMPLE_ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY

#if CONFIG_ESP_STATION_EXAMPLE_WPA3_SAE_PWE_HUNT_AND_PECK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#define EXAMPLE_H2E_IDENTIFIER ""
#elif CONFIG_ESP_STATION_EXAMPLE_WPA3_SAE_PWE_HASH_TO_ELEMENT
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HASH_TO_ELEMENT
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#elif CONFIG_ESP_STATION_EXAMPLE_WPA3_SAE_PWE_BOTH
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#endif
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

// static const char *TAG = "wifi station";

static int s_retry_num = 0;

using namespace who::cam;
using namespace who::lcd;
using namespace who::app;

WhoLCDiface* lcd = nullptr; // = new HttpLCD();
WhoRecognitionApp* recognition = nullptr; //  = new WhoRecognitionApp();
static char latest_result[128] = "No result yet";
static httpd_handle_t ws_server_handle = NULL;
static int ws_fd = -1;

// static WhoCam* cam;// = new WhoS3Cam(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, true);
// static WhoLCD* lcd;// = new WhoLCD();
static httpd_handle_t server = NULL;

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN (64)

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

static const char *TAG = "example";

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

esp_err_t stream_httpd_handler(httpd_req_t *req)
{
    esp_err_t res = ESP_OK;
    char part_buf[64];
    static int64_t last_frame = 0;
    if (!last_frame) {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK) {
        return res;
    }

    while (true) {
        // 1. Get the latest LCD buffer
        size_t img_size = 0;
        const uint8_t* img = static_cast<HttpLCD*>(lcd)->get_buffer(img_size);

        if (!img || img_size == 0) {
            ESP_LOGE(TAG, "No LCD buffer available");
            vTaskDelay(10 / portTICK_PERIOD_MS);
            continue;
        }

        // ESP_LOGI(TAG, "buffer available");

        // 2. Wrap the buffer as a camera_fb_t
        camera_fb_t fb;
        fb.buf = (uint8_t*)img;
        fb.len = img_size;
        fb.width = BSP_LCD_H_RES;
        fb.height = BSP_LCD_V_RES;
        fb.format = PIXFORMAT_RGB565;

        // 3. Convert to JPEG
        uint8_t* _jpg_buf = nullptr;
        size_t _jpg_buf_len = 0;
        bool jpeg_converted = frame2jpg(&fb, 80, &_jpg_buf, &_jpg_buf_len);
        if (!jpeg_converted) {
            ESP_LOGE(TAG, "JPEG compression failed");
            res = ESP_FAIL;
            break;
        }

        // 4. Send as multipart JPEG stream
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if (res == ESP_OK) {
            size_t hlen = snprintf(part_buf, sizeof(part_buf), _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, part_buf, hlen);
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char*)_jpg_buf, _jpg_buf_len);
        }

        free(_jpg_buf);

        if (res != ESP_OK) {
            break;
        }

        // int64_t fr_end = esp_timer_get_time();
        // int64_t frame_time = fr_end - last_frame;
        // last_frame = fr_end;
        // frame_time /= 1000;
        // ESP_LOGI(TAG,
        //          "MJPG: %luKB %lums (%.1ffps)",
        //          (uint32_t)(_jpg_buf_len / 1024),
        //          (uint32_t)frame_time,
        //          1000.0 / (uint32_t)frame_time);

        // Optional: add a small delay to control frame rate
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }

    last_frame = 0;
    return res;
}

static const httpd_uri_t stream_uri = {
    .uri = "/stream", .method = HTTP_GET, .handler = stream_httpd_handler, .user_ctx = NULL, .is_websocket = false,
    .handle_ws_control_frames = NULL};


static esp_err_t recognize_page_handler(httpd_req_t *req)
{
    // char html[1024];
    // snprintf(html, sizeof(html),
    //     "<!DOCTYPE html><html><body>"
    //     "<h2>Recognition</h2>"
    //     "<div><b>Result:</b> %s</div><br>"
    //     "<img id=\"stream\" src=\"/stream\" style=\"max-width:100%%;height:auto;\"><br><br>"
    //     "<form action=\"/recognize_action\" method=\"post\" onsubmit=\"document.getElementById('stream').src='';\">"
    //     "<button type=\"submit\">Recognize</button></form>"
    //     "<form action=\"/enroll_action\" method=\"post\" onsubmit=\"document.getElementById('stream').src='';\">"
    //     "<button type=\"submit\">Enroll</button></form>"
    //     "<form action=\"/delete_action\" method=\"post\" onsubmit=\"document.getElementById('stream').src='';\">"
    //     "<button type=\"submit\">Delete</button></form>"
    //     "</body></html>",
    //     latest_result
    // );

    // httpd_resp_set_type(req, "text/html");
    // httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);

char html[1024];
snprintf(html, sizeof(html),
    "<!DOCTYPE html><html><body>"
    "<h2>Recognition</h2>"
    "<div><b>Result:</b> %s</div><br>"
    "<img id=\"stream\" src=\"/stream\" style=\"max-width:100%%;height:auto;\"><br><br>"
    "<button onclick=\"wsSend('recognize')\">Recognize</button>"
    "<button onclick=\"wsSend('enroll')\">Enroll</button>"
    "<button onclick=\"wsSend('delete')\">Delete</button>"
    "<div id=\"result\"></div>"
    "<script>"
    "var ws = new WebSocket('ws://' + window.location.host + '/ws');"
    "ws.onopen = function() { console.log('WebSocket connected'); };"
    "ws.onmessage = function(evt) { document.getElementById('result').innerHTML = evt.data; };"
    "ws.onerror = function(e) { console.log('WebSocket error', e); };"
    "ws.onclose = function() { console.log('WebSocket closed'); };"
    "function wsSend(cmd) {"
    "  ws.send(cmd);"
    "}"
    "</script>"
    "</body></html>",
    latest_result
);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static const httpd_uri_t recognize_page_uri = {
    .uri = "/recognize",
    .method = HTTP_GET,
    .handler = recognize_page_handler,
    .user_ctx = NULL,
    .is_websocket = false,
    .handle_ws_control_frames = NULL
};


// static esp_err_t recognizebt_post_handler(httpd_req_t *req)
// {
//     ESP_LOGI(TAG, "Recognize button pressed");
//     recognition->recognize();
//     httpd_resp_sendstr(req, "Recognize button pressed!");
//     return ESP_OK;
// }

// static esp_err_t enrollbt_post_handler(httpd_req_t *req)
// {
//     ESP_LOGI(TAG, "Enroll button pressed");
//     recognition->enroll();
//     httpd_resp_sendstr(req, "Enroll button pressed!");
//     return ESP_OK;
// }

// static esp_err_t deletebt_post_handler(httpd_req_t *req)
// {
//     ESP_LOGI(TAG, "Delete button pressed");
//     recognition->delete_face();
//     httpd_resp_sendstr(req, "Delete button pressed!");
//     return ESP_OK;
// }

// static const httpd_uri_t recognizebt_uri = {
//     .uri = "/recognize_action",
//     .method = HTTP_POST,
//     .handler = recognizebt_post_handler,
//     .user_ctx = NULL
// };

// static const httpd_uri_t enrollbt_uri = {
//     .uri = "/enroll_action",
//     .method = HTTP_POST,
//     .handler = enrollbt_post_handler,
//     .user_ctx = NULL
// };

// static const httpd_uri_t deletebt_uri = {
//     .uri = "/delete_action",
//     .method = HTTP_POST,
//     .handler = deletebt_post_handler,
//     .user_ctx = NULL
// };


// WebSocket handler
static esp_err_t ws_handler(httpd_req_t *req)
{
    printf("WebSocket request received: %s\n", req->uri);

    if (req->method == HTTP_GET) {
        // Handshake done, nothing to do here
        ws_server_handle = req->handle;
        ws_fd = httpd_req_to_sockfd(req);
        return ESP_OK;
    }

    printf("2WebSocket request received: %s\n", req->uri);


    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    // Get frame length
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    ws_pkt.payload = (uint8_t*)malloc(ws_pkt.len + 1);
    if (ws_pkt.payload == NULL) {
        return ESP_ERR_NO_MEM;
    }
    ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
    if (ret != ESP_OK) {
        free(ws_pkt.payload);
        return ret;
    }
    ws_pkt.payload[ws_pkt.len] = 0; // Null-terminate

    // Handle commands from client
    if (strcmp((char*)ws_pkt.payload, "recognize") == 0) {
        recognition->recognize();
    } else if (strcmp((char*)ws_pkt.payload, "enroll") == 0) {
        recognition->enroll();
    } else if (strcmp((char*)ws_pkt.payload, "delete") == 0) {
        recognition->delete_face();
    }

    ws_server_handle = req->handle;
    ws_fd = httpd_req_to_sockfd(req);

    free(ws_pkt.payload);
    return ESP_OK;
}

static const httpd_uri_t ws_uri = {
    .uri = "/ws",
    .method = HTTP_GET,
    .handler = ws_handler,
    .user_ctx = NULL,
    .is_websocket = true,
    .handle_ws_control_frames = NULL
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/echo", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

static httpd_handle_t start_webserver(void)
{
    server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // Setting port as 8001 when building for Linux. Port 80 can be used only by a privileged user in linux.
    // So when a unprivileged user tries to run the application, it throws bind error and the server is not started.
    // Port 8001 can be used by an unprivileged user as well. So the application will not throw bind error and the
    // server will be started.
    // config.server_port = 8001;
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        // httpd_register_uri_handler(server, &hello);
        // httpd_register_uri_handler(server, &echo);
        // httpd_register_uri_handler(server, &ctrl);
        // httpd_register_uri_handler(server, &any);
        httpd_register_uri_handler(server, &stream_uri);
        httpd_register_uri_handler(server, &recognize_page_uri);

        // httpd_register_uri_handler(server, &recognizebt_uri);
        // httpd_register_uri_handler(server, &enrollbt_uri);
        // httpd_register_uri_handler(server, &deletebt_uri);
        httpd_register_uri_handler(server, &ws_uri);
        
#if CONFIG_EXAMPLE_BASIC_AUTH
        httpd_register_basic_auth(server);
#endif
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}



void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, EXAMPLE_ESP_WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, EXAMPLE_ESP_WIFI_PASS, sizeof(wifi_config.sta.password));
    wifi_config.sta.threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD;
    wifi_config.sta.sae_pwe_h2e = ESP_WIFI_SAE_MODE;
    strncpy(
        (char *)wifi_config.sta.sae_h2e_identifier, EXAMPLE_H2E_IDENTIFIER, sizeof(wifi_config.sta.sae_h2e_identifier));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits =
        xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

void recognition_result_cb(char *result)
{
    ESP_LOGI(TAG, "Recognition result: %s", result);
    strncpy(latest_result, result, sizeof(latest_result) - 1);
    latest_result[sizeof(latest_result) - 1] = '\0'; // Ensure null-termination

        // Send result to browser via WebSocket
    if (ws_server_handle && ws_fd >= 0) {
        httpd_ws_frame_t ws_pkt = {
            .final = true,
            .fragmented = false,
            .type = HTTPD_WS_TYPE_TEXT,
            .payload = (uint8_t*)result,
            .len = strlen(result)
        };
        httpd_ws_send_frame_async(ws_server_handle, ws_fd, &ws_pkt);
    }

    mqtt_publish(result);
}

extern "C" void app_main(void)
{
#if CONFIG_DB_FATFS_FLASH
    ESP_ERROR_CHECK(fatfs_flash_mount());
#elif CONFIG_DB_SPIFFS
    ESP_ERROR_CHECK(bsp_spiffs_mount());
#endif
#if CONFIG_DB_FATFS_SDCARD || CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD || CONFIG_HUMAN_FACE_FEAT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_mount());
#endif

#if CONFIG_IDF_TARGET_ESP32S3

    ESP_LOGI(TAG, "Staring APP");

    ESP_ERROR_CHECK(bsp_leds_init());
    ESP_ERROR_CHECK(bsp_led_set(BSP_LED_GREEN, false));
#endif

// ===============================================
    ESP_LOGI(TAG, "Staring WhoS3Cam");
    WhoCam* cam = new WhoS3Cam(PIXFORMAT_RGB565, FRAMESIZE_VGA, 2, true);
    
    ESP_LOGI(TAG, "Creating HttpLCD");
    lcd = new HttpLCD();

    ESP_LOGI(TAG, "WhoRecognitionApp");
    recognition = new WhoRecognitionApp();
    recognition->set_cam(cam);
    recognition->set_lcd(lcd);
    recognition->new_result_subscription(recognition_result_cb);

    ESP_LOGI(TAG, "Recognition run");

    recognition->run();


    ESP_LOGI(TAG, "Staring nvs");

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "netif");
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_LOGI(TAG, "Staring event loop");

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(TAG, "wifi_init_sta");

    wifi_init_sta();


    
}


static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");

        if (server) {
            ESP_LOGI(TAG, "Stopping webserver");
            if (stop_webserver(server) == ESP_OK) {
                server = NULL;
            } else {
                ESP_LOGE(TAG, "Failed to stop http server");
            }
        }

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

        if (server == NULL) {
            ESP_LOGI(TAG, "Starting webserver");
            server = start_webserver();
        }
        mqtt_app_start();
    }
}
