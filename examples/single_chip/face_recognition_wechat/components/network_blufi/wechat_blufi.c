// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* C Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS Includes */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/* ESP32 Includes */
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "nvs_flash.h"
#include "mdns.h"

/* Bluetooth Includes */
#include "esp_blufi_api.h"
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

/* wechat Includes */
#include "wechat_blufi.h"

static uint8_t ble_service_uuid128[32] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb,
    0x34,
    0x9b,
    0x5f,
    0x80,
    0x00,
    0x00,
    0x80,
    0x00,
    0x10,
    0x00,
    0x00,
    0xFF,
    0xFF,
    0x00,
    0x00,
};

//static uint8_t test_manufacturer[TEST_MANUFACTURER_DATA_LEN] =  {0x12, 0x23, 0x45, 0x56};
static esp_ble_adv_data_t ble_adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x100,
    .max_interval = 0x100,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 16,
    .p_service_uuid = ble_service_uuid128,
    .flag = 0x6,
};

static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min = 0x100,
    .adv_int_max = 0x100,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

wifi_config_t sta_config = {0};
wifi_config_t ap_config = {0};

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

/* store the station info for send back to phone */
static bool gl_sta_connected = false;
static uint8_t gl_sta_bssid[6];
static uint8_t gl_sta_ssid[32];
static int gl_sta_ssid_len;

/* connect infor*/
static uint8_t server_if;
static uint16_t conn_id;

esp_netif_t *AP_netif;
esp_netif_t *STA_netif;

#define SERVER_IP_ADDR      CONFIG_SERVER_IP

static void blufi_event_callback(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param);

void enter_blufi_config_wifi();

void wifi_connect_success();

/**
 * @brief wifi_info_erase
 */
esp_err_t wifi_info_erase(const char *key)
{
    esp_err_t ret    = ESP_OK;
    nvs_handle handle = 0;

    ret = nvs_open(USERDATANAMESPACE, NVS_READWRITE, &handle);
    if (ret != ESP_OK) {
        ESP_LOGE("nvs", "unable to open NVS namespace, errcode is %d\n", ret);
        return ESP_FAIL;
    }

    /**
     * @brief If key is USERDATANAMESPACE, erase all info in USERDATANAMESPACE
     */
    if (!strcmp(key, USERDATANAMESPACE)) {
        ret = nvs_erase_all(handle);
    } else {
        ret = nvs_erase_key(handle, key);
    }

    nvs_commit(handle);
    if (handle > 0) {
        nvs_close(handle);
    }

    return ESP_OK;
}

/**
 * @brief load_info_nvs
 */
esp_err_t load_info_nvs(char *key, void *value, size_t len)
{
    esp_err_t err;
    nvs_handle handle = 0;
    size_t length = len;

    err = nvs_open(USERDATANAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE("nvs", "unable to open NVS namespace, errcode is %d\n", err);
        return ESP_FAIL;
    }
    if (value == NULL) {
        ESP_LOGE("nvs", "value NVS_ERR_PARAMETER_NULL\n");
        return ESP_FAIL;
    }

    err = nvs_get_blob(handle, key, value, &length);
    if (err != ESP_OK) {
        ESP_LOGE("load_info_nvs", "load_info_nvs\n");
        return err;
    }

    if (handle > 0) {
        nvs_close(handle);
    }
    return ESP_OK;
}

/**
 * @brief save_info_nvs
 */
esp_err_t save_info_nvs(char *key, void *value, size_t len)
{
    esp_err_t err;
    nvs_handle handle = 0;
    size_t length = len;

    err = nvs_open(USERDATANAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE("nvs", "unable to open NVS namespace, errcode is %d\n", err);
        return ESP_FAIL;
    }
    if (value == NULL) {
        ESP_LOGE("nvs", "value NVS_ERR_PARAMETER_NULL\n");
        return ESP_FAIL;
    }

    err = nvs_set_blob(handle, key, value, length);
    if (err == ESP_OK) {
        ESP_LOGI("save_info_nvs", "save_info_nvs\n");
    }

    nvs_commit(handle);
    if (handle > 0) {
        nvs_close(handle);
    }
    return err;
}

/**
 * @brief 等待网络连接成功，获取到 IP 地址
 */
void wait_net_connected()
{
    xEventGroupWaitBits(wifi_event_group, 
                WIFI_IP4_CONNECTED_BIT, false, true, portMAX_DELAY);
    // xEventGroupWaitBits(wifi_event_group, WIFI_IP4_CONNECTED_BIT | WIFI_IP6_CONNECTED_BIT, false, true, portMAX_DELAY);
}

/**
 * @brief wifi 事件处理函数
 */
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    wifi_mode_t mode;
    static uint8_t connect_try_count = 0;

    if (event_id == WIFI_EVENT_STA_START) 
    {
        ESP_LOGI("wifi_event_handler", "WIFI_EVENT_STA_START\n");
        if (strlen((char *)sta_config.sta.ssid) != 0 && strlen((char *)sta_config.sta.password) != 0) {
            ESP_ERROR_CHECK(esp_wifi_connect());
        }
        
    } else if (event_id == WIFI_EVENT_STA_STOP) {
        ESP_LOGI("wifi_event_handler", "WIFI_EVENT_STA_STOP\n");
        
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI("wifi_event_handler", "WIFI_EVENT_STA_CONNECTED\n");
        wifi_event_sta_connected_t* event = (wifi_event_sta_connected_t*) event_data;
        gl_sta_connected = true;
        connect_try_count = 0;
        memcpy(gl_sta_bssid, event->bssid, 6);
        memcpy(gl_sta_ssid, event->ssid, event->ssid_len);
        gl_sta_ssid_len = event->ssid_len;

        save_info_nvs(NVS_KEY_WIFI_SSID_PASS, (void *)(sta_config.sta.ssid), WIFI_SSID_PASS_SIZE);
        /* enable ipv6 */
        esp_netif_create_ip6_linklocal(WIFI_IF_STA); 

    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI("wifi_event_handler", "WIFI_EVENT_STA_DISCONNECTED\n");
        /* This is a workaround as ESP32 WiFi libs don't currently auto-reassociate. */
        if (connect_try_count++ > 5 && connect_try_count < 10) {
            // connect_try_count = 0;
            esp_wifi_get_mode(&mode);
            if (esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_FAIL, 0, NULL) != ESP_OK) {
                ESP_LOGI("blufi","esp_fail\n");
            }
        } else {
            gl_sta_connected = false;
            memset(gl_sta_ssid, 0, 32);
            memset(gl_sta_bssid, 0, 6);
            gl_sta_ssid_len = 0;
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, WIFI_IP4_CONNECTED_BIT | WIFI_IP6_CONNECTED_BIT);
        }
        
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        esp_blufi_extra_info_t info;
        ESP_LOGI("wifi_event_handler", "IP_EVENT_STA_GOT_IP\n");
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("wifi_event_handler", "got ip4: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_IP4_CONNECTED_BIT);
        esp_wifi_get_mode(&mode);

        memset(&info, 0, sizeof(esp_blufi_extra_info_t));
        memcpy(info.sta_bssid, gl_sta_bssid, 6);
        info.sta_bssid_set = true;
        info.sta_ssid = gl_sta_ssid;
        info.sta_ssid_len = gl_sta_ssid_len;
        esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, 0, &info);

        wifi_connect_success();

    } else if (event_id == IP_EVENT_GOT_IP6) {
        ESP_LOGI("wifi_event_handler", "IP_EVENT_GOT_IP6\n");
        ip_event_got_ip6_t* event = (ip_event_got_ip6_t*) event_data;
        ESP_LOGI("wifi_event_handler", "got ip6: " IPSTR, IP2STR(&event->ip6_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_IP6_CONNECTED_BIT);
        
    } else if (event_id == WIFI_EVENT_AP_START) {
        ESP_LOGI("wifi_event_handler", "WIFI_EVENT_AP_START\n");
        esp_wifi_get_mode(&mode);

        /* TODO: get config or information of softap, then set to report extra_info */
        if (gl_sta_connected) {
            esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, 0, NULL);
        } else {
            esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_FAIL, 0, NULL);
        }

    } else if (event_id == WIFI_EVENT_AP_STOP) {
        ESP_LOGI("wifi_event_handler", "WIFI_EVENT_AP_STOP\n");

    } else if (event_id == IP_EVENT_AP_STAIPASSIGNED) {
        ESP_LOGI("wifi_event_handler", "IP_EVENT_AP_STAIPASSIGNED\n");
        

    } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        ESP_LOGI("wifi_event_handler", "WIFI_EVENT_AP_STACONNECTED\n");
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI("wifi_event_handler", "station:" MACSTR " join, AID=%d\n",
                 MAC2STR(event->mac),
                 event->aid);
        xEventGroupSetBits(wifi_event_group, WIFI_IP4_CONNECTED_BIT);
        

    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        ESP_LOGI("wifi_event_handler", "WIFI_EVENT_AP_STADISCONNECTED\n");
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI("wifi_event_handler", "station:" MACSTR "leave, AID=%d\n",
                 MAC2STR(event->mac),
                 event->aid);
        xEventGroupClearBits(wifi_event_group, WIFI_IP4_CONNECTED_BIT);
        

    } else if (event_id == WIFI_EVENT_SCAN_DONE) {
        do {
            uint16_t apCount = 0;
            esp_wifi_scan_get_ap_num(&apCount);
            if (apCount == 0) {
                BLUFI_INFO("No AP was found");
                break;
            }
            wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * apCount);
            if (!ap_list) {
                BLUFI_ERROR("malloc error, ap_list is NULL");
                break;
            }
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&apCount, ap_list));
            esp_blufi_ap_record_t *blufi_ap_list = (esp_blufi_ap_record_t *)malloc(apCount * sizeof(esp_blufi_ap_record_t));
            if (!blufi_ap_list) {
                if (ap_list) {
                    free(ap_list);
                }
                BLUFI_ERROR("malloc error, blufi_ap_list is NULL");
                break;
            }
            for (int i = 0; i < apCount; ++i) {
                blufi_ap_list[i].rssi = ap_list[i].rssi;
                memcpy(blufi_ap_list[i].ssid, ap_list[i].ssid, sizeof(ap_list[i].ssid));
            }
            esp_blufi_send_wifi_list(apCount, blufi_ap_list);
            esp_wifi_scan_stop();
            free(ap_list);
            free(blufi_ap_list);
        } while(0);
    }
}

// wifi initialise for Software AP + Sta
void wifi_init_ap_sta()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_event_group = xEventGroupCreate();

    /* default event loop from esp_event library */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    AP_netif = esp_netif_create_default_wifi_ap();
    assert(AP_netif);

    STA_netif = esp_netif_create_default_wifi_sta();
    assert(STA_netif);

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                   
                                                        NULL));

    wifi_config_t ap_wifi_config = {
        .ap = {
            .ssid = DEFAULT_SSID,
            .ssid_len = 0,
            .max_connection = MAX_STA_CONN,
            .password = DEFAULT_PWD,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    // memcpy(sta_config.sta.ssid, sta_wifi_config.sta.ssid, WIFI_SSID_MAX_SIZE);
    // memcpy(sta_config.sta.password, sta_wifi_config.sta.password, WIFI_SSID_PASS_SIZE);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    // "esp_wifi_set_config" can be called only when specified interface is enabled,
    // otherwise, API fail
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    if (strlen((char *)sta_config.sta.ssid) == 0 && strlen((char *)sta_config.sta.password) == 0) {
        enter_blufi_config_wifi();
    }

    ESP_LOGI("wifi_init_ap_sta", "AP SSID:%s, password:%s; Station SSID:%s, pass:%s\n",
             DEFAULT_SSID, DEFAULT_PWD, sta_config.sta.ssid, sta_config.sta.password);
}

/**
 * @brief blufi 回调函数结构体
 */
static esp_blufi_callbacks_t blufi_callbacks = {
    .event_cb = blufi_event_callback,
    .negotiate_data_handler = blufi_dh_negotiate_data_handler,
    .encrypt_func = blufi_aes_encrypt,
    .decrypt_func = blufi_aes_decrypt,
    .checksum_func = blufi_crc_checksum,
};

/**
 * @brief 蓝牙 blufi 时间处理函数
 */
static void blufi_event_callback(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param)
{
    /* actually, should post to blufi_task handle the procedure,
     * now, as a example, we do it more simply */
    switch (event) {
    case ESP_BLUFI_EVENT_INIT_FINISH:
        BLUFI_INFO("BLUFI init finish\n");
        esp_ble_gap_set_device_name(BLUFI_DEVICE_NAME);
        esp_ble_gap_config_adv_data(&ble_adv_data);
        break;

    case ESP_BLUFI_EVENT_DEINIT_FINISH:
        BLUFI_INFO("BLUFI deinit finish\n");
        break;

    case ESP_BLUFI_EVENT_BLE_CONNECT:
        BLUFI_INFO("BLUFI ble connect\n");
        server_if = param->connect.server_if;
        conn_id = param->connect.conn_id;
        esp_ble_gap_stop_advertising();
        blufi_security_init();
        break;

    case ESP_BLUFI_EVENT_BLE_DISCONNECT:
        BLUFI_INFO("BLUFI ble disconnect\n");
        vTaskDelay(200 / portTICK_PERIOD_MS);
        blufi_security_deinit();
        esp_ble_gap_start_advertising(&ble_adv_params);
        break;

    case ESP_BLUFI_EVENT_SET_WIFI_OPMODE:
        BLUFI_INFO("BLUFI Set WIFI opmode %d\n", param->wifi_mode.op_mode);
        ESP_ERROR_CHECK(esp_wifi_set_mode(param->wifi_mode.op_mode));
        break;

    case ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP:
        BLUFI_INFO("BLUFI requset wifi connect to AP\n");
        /* there is no wifi callback when the device has already connected to this wifi
        so disconnect wifi before connection.
        */
        esp_wifi_disconnect();
        esp_wifi_connect();
        break;

    case ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP:
        BLUFI_INFO("BLUFI requset wifi disconnect from AP\n");
        esp_wifi_disconnect();
        break;

    case ESP_BLUFI_EVENT_REPORT_ERROR:
        BLUFI_ERROR("BLUFI report error, error code %d\n", param->report_error.state);
        esp_blufi_send_error_info(param->report_error.state);
        break;

    case ESP_BLUFI_EVENT_GET_WIFI_STATUS: {
        wifi_mode_t mode;
        esp_blufi_extra_info_t info;

        esp_wifi_get_mode(&mode);

        if (gl_sta_connected) {
            memset(&info, 0, sizeof(esp_blufi_extra_info_t));
            memcpy(info.sta_bssid, gl_sta_bssid, 6);
            info.sta_bssid_set = true;
            info.sta_ssid = gl_sta_ssid;
            info.sta_ssid_len = gl_sta_ssid_len;
            esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, 0, &info);
        } else {
            esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_FAIL, 0, NULL);
        }
        BLUFI_INFO("BLUFI get wifi status from AP\n");

        break;
    }

    case ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE:
        BLUFI_INFO("blufi close a gatt connection");
        vTaskDelay(200 / portTICK_PERIOD_MS);
        //esp_blufi_close(server_if, conn_id);
        break;

    case ESP_BLUFI_EVENT_DEAUTHENTICATE_STA:
        BLUFI_INFO("ESP_BLUFI_EVENT_DEAUTHENTICATE_STA\n");
        /* TODO */
        break;

    case ESP_BLUFI_EVENT_RECV_STA_BSSID:
        memcpy(sta_config.sta.bssid, param->sta_bssid.bssid, 6);
        sta_config.sta.bssid_set = 1;
        esp_wifi_set_config(WIFI_IF_STA, &sta_config);
        BLUFI_INFO("Recv STA BSSID %s\n", sta_config.sta.ssid);
        break;

    case ESP_BLUFI_EVENT_RECV_STA_SSID:
        strncpy((char *)sta_config.sta.ssid, (char *)param->sta_ssid.ssid, param->sta_ssid.ssid_len);
        sta_config.sta.ssid[param->sta_ssid.ssid_len] = '\0';
        esp_wifi_set_config(WIFI_IF_STA, &sta_config);
        BLUFI_INFO("Recv STA SSID %s\n", sta_config.sta.ssid);
        break;

    case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
        strncpy((char *)sta_config.sta.password, (char *)param->sta_passwd.passwd, param->sta_passwd.passwd_len);
        sta_config.sta.password[param->sta_passwd.passwd_len] = '\0';
        esp_wifi_set_config(WIFI_IF_STA, &sta_config);
        BLUFI_INFO("Recv STA PASSWORD %s\n", sta_config.sta.password);
        break;

    case ESP_BLUFI_EVENT_RECV_SOFTAP_SSID:
        strncpy((char *)ap_config.ap.ssid, (char *)param->softap_ssid.ssid, param->softap_ssid.ssid_len);
        ap_config.ap.ssid[param->softap_ssid.ssid_len] = '\0';
        ap_config.ap.ssid_len = param->softap_ssid.ssid_len;
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        BLUFI_INFO("Recv SOFTAP SSID %s, ssid len %d\n", ap_config.ap.ssid, ap_config.ap.ssid_len);
        break;

    case ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
        strncpy((char *)ap_config.ap.password, (char *)param->softap_passwd.passwd, param->softap_passwd.passwd_len);
        ap_config.ap.password[param->softap_passwd.passwd_len] = '\0';
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        BLUFI_INFO("Recv SOFTAP PASSWORD %s len = %d\n", ap_config.ap.password, param->softap_passwd.passwd_len);
        break;

    case ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM:
        if (param->softap_max_conn_num.max_conn_num > 4) {
            return;
        }
        ap_config.ap.max_connection = param->softap_max_conn_num.max_conn_num;
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        BLUFI_INFO("Recv SOFTAP MAX CONN NUM %d\n", ap_config.ap.max_connection);
        break;

    case ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE:
        if (param->softap_auth_mode.auth_mode >= WIFI_AUTH_MAX) {
            return;
        }
        ap_config.ap.authmode = param->softap_auth_mode.auth_mode;
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        BLUFI_INFO("Recv SOFTAP AUTH MODE %d\n", ap_config.ap.authmode);
        break;

    case ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL:
        if (param->softap_channel.channel > 13) {
            return;
        }
        ap_config.ap.channel = param->softap_channel.channel;
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        BLUFI_INFO("Recv SOFTAP CHANNEL %d\n", ap_config.ap.channel);
        break;

    case ESP_BLUFI_EVENT_GET_WIFI_LIST: {
        wifi_scan_config_t scanConf = {
            .ssid = NULL,
            .bssid = NULL,
            .channel = 0,
            .show_hidden = false
        };
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scanConf, true));
        BLUFI_INFO("ESP_BLUFI_EVENT_GET_WIFI_LIST\n");
        break;
    }

    case ESP_BLUFI_EVENT_RECV_CUSTOM_DATA:
        BLUFI_INFO("Recv Custom Data %d\n", param->custom_data.data_len);
        esp_log_buffer_hex("Custom Data", param->custom_data.data, param->custom_data.data_len);
        if (strcmp("restart", (char *)param->custom_data.data)) {
            esp_restart();
        }
        break;

    case ESP_BLUFI_EVENT_RECV_USERNAME:
        /* Not handle currently */
        break;
    case ESP_BLUFI_EVENT_RECV_CA_CERT:
        /* Not handle currently */
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
        /* Not handle currently */
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
        /* Not handle currently */
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
        /* Not handle currently */
        break;
        ;
    case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
        /* Not handle currently */
        break;
    default:
        break;
    }
}

/**
 * @brief 蓝牙 gap 事件处理
 */
static void ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&ble_adv_params);
        break;

    default:
        break;
    }
}

/**
 * @brief 蓝牙初始化
 */
static esp_err_t bluetooth_init()
{
    esp_err_t ret;

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        BLUFI_ERROR("%s initialize bt controller failed: %s\n", __func__, esp_err_to_name(ret));
        return ret;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        BLUFI_ERROR("%s enable bt controller failed: %s\n", __func__, esp_err_to_name(ret));
        return ret;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        BLUFI_ERROR("%s init bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return ret;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        BLUFI_ERROR("%s init bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return ret;
    }

    BLUFI_INFO("BD ADDR: " ESP_BD_ADDR_STR "\n", ESP_BD_ADDR_HEX(esp_bt_dev_get_address()));

    BLUFI_INFO("BLUFI VERSION %04x\n", esp_blufi_get_version());

    ret = esp_ble_gap_register_callback(ble_gap_event_handler);
    if (ret) {
        BLUFI_ERROR("%s gap register failed, error code = %x\n", __func__, ret);
        return ret;
    }

    ret = esp_blufi_register_callbacks(&blufi_callbacks);
    if (ret) {
        BLUFI_ERROR("%s blufi register failed, error code = %x\n", __func__, ret);
        return ret;
    }

    ret = esp_blufi_profile_init();
    if (ret) {
        BLUFI_ERROR("%s blufi profile init failed, error code = %x\n", __func__, ret);
        return ret;
    }
    return ret;
}

/**
 * @brief 蓝牙配网析构
 */
static bool blufi_init = true;
static void blufi_deinitialize()
{
    if (blufi_init) {
        blufi_init = false;
        ESP_ERROR_CHECK(esp_blufi_profile_deinit());
        ESP_ERROR_CHECK(esp_bluedroid_disable());
        ESP_ERROR_CHECK(esp_bluedroid_deinit());
        ESP_ERROR_CHECK(esp_bt_controller_disable());
        ESP_ERROR_CHECK(esp_bt_controller_deinit());
        BLUFI_INFO("blufi_deinitialize\n");
    }
    // ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BTDM));
}

/**
 * @brief 蓝牙配网初始化
 */
static void blufi_initialize()
{
    if (!blufi_init) {
        blufi_init = true;
        // initialise blutooth
        esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
        ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
        ESP_ERROR_CHECK(esp_bluedroid_init());
        ESP_ERROR_CHECK(esp_bluedroid_enable());
        ESP_ERROR_CHECK(esp_blufi_profile_init());
        BLUFI_INFO("blufi_initialize\n");
    }
}

// wifi config timer
TimerHandle_t wifi_config_timer;
static bool wifi_configing = false;

/**
 * @brief wifi 连接成功
 */
void wifi_connect_success()
{

}

/**
 * @brief 蓝牙配网超时
 */
void wifi_config_timer_callback(void *arg)
{
    if (wifi_configing == true){
        wifi_configing = false;
        // if (connnected) {
        // } else {
        // }
        BLUFI_ERROR("wifi_config_timer_callback out\n");

        // de-initialise blufi
        blufi_deinitialize();

        xTimerDelete(wifi_config_timer, portMAX_DELAY);
    }
}

/**
 * @brief 进入 蓝牙配网模式
 */
void enter_blufi_config_wifi()
{
    if (wifi_configing == false) {
        wifi_configing = true;

         // initialise blufi
        blufi_initialize();

        // start timer for 1 minute
        wifi_config_timer = xTimerCreate(
                                "wifi_config_timer",
                                WIFI_CONFIG_TIMEOUT / portTICK_PERIOD_MS, //period time
                                pdFALSE,                                  //auto load
                                (void *)NULL,                             //timer parameter
                                wifi_config_timer_callback);              //timer callback

        xTimerStart(wifi_config_timer, portMAX_DELAY);
    } else {
        xTimerReset(wifi_config_timer, portMAX_DELAY);
    }
}

/**
 * @brief udp 发现 server 初始化
 */
static int notice_udp_server_create()
{
    esp_err_t ret = ESP_OK;
    int sockfd = 0;
    struct sockaddr_in server_addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(UDP_NOTICE_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

retry:
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        goto ERR_EXIT;
    }

    ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sockfd < 0) {
        goto ERR_EXIT;
    }

    struct timeval socket_timeout = {0, 100 * 1000};
    ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &socket_timeout, sizeof(struct timeval));
    if (sockfd < 0) {
        goto ERR_EXIT;
    }

    ESP_LOGD("udp notice", "create udp server, port: %d, sockfd: %d", UDP_NOTICE_PORT, sockfd);

    return sockfd;

ERR_EXIT:

    if (sockfd != -1) {
        ret = close(sockfd);

        if (ret != ESP_OK) {
            ESP_LOGD("udp notice", "close fail, ret: %d", ret);
        }
    }
    goto retry;

    return -1;
}

/**
 * @brief udp 发现初始化
 */
static void notice_udp_task(void *arg)
{
    uint8_t root_mac[6]          = {0};
    char *udp_server_buf         = malloc(NOTICE_UDP_BUF_SIZE);
    struct sockaddr_in from_addr = {0};
    socklen_t from_addr_len      = sizeof(struct sockaddr_in);
    
    // wait network connected
    wait_net_connected();

    int udp_server_sockfd        = notice_udp_server_create();

    if (udp_server_sockfd == -1) {
        ESP_LOGE("notice udp", "Failed to create UDP notification service");

        vTaskDelete(NULL);
        return ;
    }

    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, root_mac));

    while(1){
        memset(udp_server_buf, 0, NOTICE_UDP_BUF_SIZE);
        if (recvfrom(udp_server_sockfd, udp_server_buf, NOTICE_UDP_BUF_SIZE,
                            0, (struct sockaddr *)&from_addr, (socklen_t *)&from_addr_len) > 0) {
            ESP_LOGD("udp notice task", "Mlink notice udp recvfrom, sockfd: %d, port: %d, ip: %s, udp_server_buf: %s",
                     udp_server_sockfd, ntohs(((struct sockaddr_in *)&from_addr)->sin_port),
                     inet_ntoa(((struct sockaddr_in *)&from_addr)->sin_addr), udp_server_buf);

            if (strcmp(udp_server_buf, "Are You ESP_Who_Wechat Device?")) {
                continue;
            }

            sprintf(udp_server_buf, "WeChat MAC:%02x%02x%02x%02x%02x%02x TCP:%d",
                    MAC2STR(root_mac), TCP_DEFAULT_PORT);

            ESP_LOGD("udp notice task", "Mlink notice udp sendto, sockfd: %d, data: %s", udp_server_sockfd, udp_server_buf);

            for (int i = 0, delay_time_ms = 0; i < NOTICE_UDP_RETRY_COUNT; ++i, delay_time_ms += delay_time_ms) {
                vTaskDelay(delay_time_ms);
                delay_time_ms = (i == 0) ? (10 / portTICK_RATE_MS) : delay_time_ms;

                if (sendto(udp_server_sockfd, udp_server_buf, strlen(udp_server_buf),
                           0, (struct sockaddr *)&from_addr, from_addr_len) <= 0) {
                    ESP_LOGW("udp notice task", "Mlink notice udp sendto, errno: %d, errno_str: %s", errno, strerror(errno));
                    break;
                }
            }
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

// mDNS initialise
static void initialise_mdns(void)
{
    int ret = 0;
    uint8_t retry_count = 30;
    uint8_t root_mac[6] = {0};
    char mac_str[16] = {0};

    // wait network connected
    wait_net_connected();

    //initialize mDNS
    ESP_ERROR_CHECK(mdns_init());
    //set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK(mdns_hostname_set(NOTICE_MDNS_HOSTNAME));
    //set default mDNS instance name
    ESP_ERROR_CHECK(mdns_instance_name_set(NOTICE_MDNS_INSTANCE));

    mdns_txt_item_t mdns_txt_data[] = {
        {"esp-who", "WeChat"},
        {"mac", mac_str},
    };

    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, root_mac));
    sprintf(mac_str, "%02x%02x%02x%02x%02x%02x", MAC2STR(root_mac));

    do {
        //initialize MDNS service
        ret = mdns_service_add(NOTICE_MDNS_INSTANCE, NOTICE_MDNS_SERVICE_TYPE, NOTICE_MDNS_PROTO, TCP_DEFAULT_PORT,
                                         mdns_txt_data, sizeof(mdns_txt_data) / sizeof(mdns_txt_item_t));
        
        if (ret != 0) {
            vTaskDelay(100 / portTICK_RATE_MS);
        }
    } while (ret != 0 && retry_count--);
}

/**
 * @brief mdns 初始化、udp 发现初始化
 */
static void initialise_notice()
{
    // initialise mdns
    initialise_mdns();

    // initialise udp notice service
    xTaskCreatePinnedToCore(notice_udp_task, "notice_udp", 3 * 1024, NULL, 4, NULL, 1);
}


/**
 * @brief 蓝牙、wifi 初始化、mdns 初始化、udp 发现初始化
 */
void blufi_main()
{
    // initialise blutooth
    ESP_ERROR_CHECK(bluetooth_init());

    // de-initialise blufi
    blufi_deinitialize();

    // load wifi ssid passwd
    load_info_nvs(NVS_KEY_WIFI_SSID_PASS, (void *)(sta_config.sta.ssid), WIFI_SSID_PASS_SIZE);

    // initialise wifi
    wifi_init_ap_sta();

    // initialise notice service
    initialise_notice();
}
