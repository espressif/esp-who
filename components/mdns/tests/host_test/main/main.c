/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include "mdns.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "mdns-test";

static void query_mdns_host(const char *host_name)
{
    ESP_LOGI(TAG, "Query A: %s.local", host_name);

    struct esp_ip4_addr addr;
    addr.addr = 0;

    esp_err_t err = mdns_query_a(host_name, 2000,  &addr);
    if (err) {
        if (err == ESP_ERR_NOT_FOUND) {
            ESP_LOGW(TAG, "%x: Host was not found!", (err));
            return;
        }
        ESP_LOGE(TAG, "Query Failed: %x", (err));
        return;
    }

    ESP_LOGI(TAG, "Query A: %s.local resolved to: " IPSTR, host_name, IP2STR(&addr));
}

int main(int argc, char *argv[])
{

    setvbuf(stdout, NULL, _IONBF, 0);
    const esp_netif_inherent_config_t base_cg = { .if_key = "WIFI_STA_DEF", .if_desc = CONFIG_TEST_NETIF_NAME };
    esp_netif_config_t cfg = { .base = &base_cg  };
    esp_netif_t *sta = esp_netif_new(&cfg);
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set(CONFIG_TEST_HOSTNAME));
    ESP_LOGI(TAG, "mdns hostname set to: [%s]", CONFIG_TEST_HOSTNAME);
    ESP_ERROR_CHECK(mdns_register_netif(sta));
    ESP_ERROR_CHECK(mdns_netif_action(sta, MDNS_EVENT_ENABLE_IP4 | MDNS_EVENT_IP4_REVERSE_LOOKUP | MDNS_EVENT_IP6_REVERSE_LOOKUP));

#ifdef REGISTER_SERVICE
    //set default mDNS instance name
    mdns_instance_name_set("myesp-inst");
    //structure with TXT records
    mdns_txt_item_t serviceTxtData[3] = {
        {"board", "esp32"},
        {"u", "user"},
        {"p", "password"}
    };
    vTaskDelay(pdMS_TO_TICKS(10000));
    ESP_ERROR_CHECK(mdns_service_add("myesp-service2", "_http", "_tcp", 80, serviceTxtData, 3));
#endif
    vTaskDelay(pdMS_TO_TICKS(10000));
    query_mdns_host("david-work");
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_netif_destroy(sta);
    mdns_free();
    ESP_LOGI(TAG, "Exit");
    return 0;
}
