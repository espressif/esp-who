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

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "sdkconfig.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "app_wifi.h"

/* The examples use WiFi configuration that you can set via 'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define ESP_WIFI_SSID "mywifissid"
*/
#define ESP_WIFI_MODE_AP    1 //TRUE:AP FALSE:STA

#define ESP_WIFI_SSID       CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS       CONFIG_ESP_WIFI_PASSWORD

#define MAX_STA_CONN        CONFIG_MAX_STA_CONN
#define SERVER_IP_ADDR      CONFIG_SERVER_IP
#define ESP_WIFI_AP_CHANNEL CONFIG_ESP_WIFI_AP_CHANNEL

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event,
 * For STA events handling, we only care about two events:
 * - we are connected to the AP with an IP
 */
#define WIFI_CONNECTED_BIT BIT0

static const char *TAG = "App_Wifi";

esp_netif_t *AP_netif;
esp_netif_t *STA_netif;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_ERROR_CHECK(esp_wifi_connect());
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station: " MACSTR "join, AID=%d",
                 MAC2STR(event->mac), 
                 event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station:" MACSTR "leave, AID=%d",
                 MAC2STR(event->mac), 
                 event->aid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
}

void wifi_init_softap()
{
    /* Always use WIFI_INIT_CONFIG_DEFAULT macro to init the config to default values, 
     * this can guarantee all the fields got correct value when more fields are added 
     * into wifi_init_config_t in future release. */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    /* esp_wifi_init API must be called before all other WiFi API can be called */
    ESP_LOGI(TAG, "Initializing ESP Wifi");
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* default event loop from esp_event library */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    AP_netif = esp_netif_create_default_wifi_ap();
    assert(AP_netif);

    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_AP, mac));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    
    int a, b, c, d;
    sscanf(SERVER_IP_ADDR, "%d.%d.%d.%d", &a, &b, &c, &d);
    esp_netif_ip_info_t ip_info;
    esp_netif_set_ip4_addr(&ip_info.ip, a, b, c, d);
    esp_netif_set_ip4_addr(&ip_info.gw, a, b, c, d);
    esp_netif_set_ip4_addr(&ip_info.netmask, 255, 255, 255, 0);
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(AP_netif));
    ESP_ERROR_CHECK(esp_netif_set_ip_info(AP_netif, &ip_info));
    ESP_ERROR_CHECK(esp_netif_dhcps_start(AP_netif));

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));

    if (strlen(ESP_WIFI_SSID) == 0)
    {
        snprintf((char *)wifi_config.ap.ssid, 32, "esp-camera-%x%x", mac[4], mac[5]);
    }
    else
    {
        snprintf((char*)wifi_config.ap.ssid, 32, "%s", ESP_WIFI_SSID);
    }
    
    snprintf((char*)wifi_config.ap.password, 64, "%s", ESP_WIFI_PASS);
    wifi_config.ap.ssid_len = strlen((char*)wifi_config.ap.ssid);
    
    wifi_config.ap.max_connection = MAX_STA_CONN;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    
    if (strlen(ESP_WIFI_AP_CHANNEL)) {
        int channel;
        sscanf(ESP_WIFI_AP_CHANNEL, "%d", &channel);
        wifi_config.ap.channel = channel;
    }

    esp_wifi_set_ps(WIFI_PS_NONE);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    // "esp_wifi_set_config" can be called only when specified interface is enabled, otherwise, API fail
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
            wifi_config.ap.ssid, ESP_WIFI_PASS);

    char buf[80];
    sprintf(buf, "SSID:%s", wifi_config.ap.ssid);
    sprintf(buf, "PASSWORD:%s", wifi_config.ap.password);
}

void wifi_init_sta()
{
    /* Always use WIFI_INIT_CONFIG_DEFAULT macro to init the config to default values, 
     * this can guarantee all the fields got correct value when more fields are added 
     * into wifi_init_config_t in future release. */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    /* esp_wifi_init API must be called before all other WiFi API can be called */
    ESP_LOGI(TAG, "Initializing ESP Wifi");
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    STA_netif = esp_netif_create_default_wifi_sta();
    assert(STA_netif);

    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    
    snprintf((char*)wifi_config.sta.ssid, 32, "%s", ESP_WIFI_SSID);
    snprintf((char*)wifi_config.sta.password, 64, "%s", ESP_WIFI_PASS);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    // "esp_wifi_set_config" can be called only when specified interface is enabled, otherwise, API fail
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_STA finished.");

    // Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
    // number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above)
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) 
    {
        wifi_ap_record_t AP_info;
        ESP_ERROR_CHECK(esp_wifi_sta_get_ap_info(&AP_info));
        ESP_LOGI(TAG, "connected to AP, SSID : %s Channel : %d Strength : %d Authmode : %d",
                 AP_info.ssid, AP_info.primary, AP_info.rssi, AP_info.authmode);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    // The event will not be processed after unregister
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));

    vEventGroupDelete(s_wifi_event_group);
}

void app_wifi_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

#if EXAMPLE_ESP_WIFI_MODE_AP
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();
#else
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
#endif /*EXAMPLE_ESP_WIFI_MODE_AP*/
}

