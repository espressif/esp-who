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

#ifndef _WECHAT_BLUFI_H_
#define _WECHAT_BLUFI_H_

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

/* nvs typedef */
#define USERDATANAMESPACE "userdata"
#define NVS_KEY_WIFI_SSID_PASS "ssid_pass"
#define NVS_KEY_RESTART_COUNT "restart_cou"
#define WIFI_SSID_PASS_SIZE (32+64)

#define BLUFI_EXAMPLE_TAG "ESP32_BLUFI"
#define BLUFI_DEVICE_NAME "BLUFI_DEVICE_ESP_WHO1"

#define DEFAULT_SSID "ESP_Who_Wechat"
#define DEFAULT_PWD "12345678"
#define MAX_STA_CONN 2

#define NOTICE_MDNS_HOSTNAME "notice-mDNS"
#define NOTICE_MDNS_INSTANCE "notice with ESP-WHO"
#define NOTICE_MDNS_SERVICE_TYPE "_notice_service"
#define NOTICE_MDNS_PROTO "_tcp"

#define TCP_DEFAULT_PORT 8899
#define UDP_NOTICE_PORT 7899
#define DEFAULT_PKTSIZE 135

#define WIFI_IP4_CONNECTED_BIT BIT0
#define WIFI_IP6_CONNECTED_BIT BIT1
#define ESPTOUCH_DONE_BIT BIT2

#define WIFI_LIST_NUM 10
#define RESTART_TIMEOUT_MS      (5000)
#define WIFI_CONFIG_TIMEOUT (3 * 60 * 1000)

#define NOTICE_UDP_BUF_SIZE         (64)
#define NOTICE_UDP_RETRY_COUNT      (3)

#define BLUFI_INFO(fmt, ...) ESP_LOGI(BLUFI_EXAMPLE_TAG, fmt, ##__VA_ARGS__)
#define BLUFI_ERROR(fmt, ...) ESP_LOGE(BLUFI_EXAMPLE_TAG, fmt, ##__VA_ARGS__)

/**
 * @brief wifi_info_erase
 */
esp_err_t wifi_info_erase(const char *key);

/**
 * @brief load_info_nvs
 */
esp_err_t load_info_nvs(char *key, void *value, size_t len);

/**
 * @brief save_info_nvs
 */
esp_err_t save_info_nvs(char *key, void *value, size_t len);

/**
 * @brief 蓝牙、wifi 初始化、mdns 初始化、udp 发现初始化
 */
void blufi_main();

/**
 * @brief wifi initialise for Software AP + Sta
 */
void wifi_init_ap_sta();

/**
 * @brief 等待网络连接成功，获取到 IP 地址
 */
void wait_net_connected();

/**
 * @brief 进入 蓝牙配网模式
 */
void enter_blufi_config_wifi();

void blufi_dh_negotiate_data_handler(uint8_t *data, int len, uint8_t **output_data, int *output_len, bool *need_free);

int blufi_aes_encrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len);

int blufi_aes_decrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len);

uint16_t blufi_crc_checksum(uint8_t iv8, uint8_t *data, int len);

int blufi_security_init(void);

void blufi_security_deinit(void);

#endif
