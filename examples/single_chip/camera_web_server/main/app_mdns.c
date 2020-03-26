/*
  * ESPRESSIF MIT License
  *
  * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
  *
  * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
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
  *
  */

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_camera.h"
#include "mdns.h"
#include "app_camera.h"

static const char *TAG = "camera mdns";

void app_mdns_update_framesize(int size)
{
	char framesize[4];
    snprintf(framesize, 4, "%d", size);
    if(mdns_service_txt_item_set("_esp-cam", "_tcp", "framesize", (char*)framesize)){
        ESP_LOGE(TAG, "mdns_service_txt_item_set() framesize Failed");
    }
}

void app_mdns_main()
{
	char iname[64];
	char hname[64];
    uint8_t mac[6];
	char framesize[4];
	char pixformat[4];

    if(esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK){
        ESP_LOGE(TAG, "esp_read_mac() Failed");
        return;
    }

    sensor_t * s = esp_camera_sensor_get();
    const char * model;
    switch(s->id.PID){
        case OV2640_PID: model = "OV2640"; break;
        case OV3660_PID: model = "OV3660"; break;
        case OV5640_PID: model = "OV5640"; break;
        case OV7725_PID: model = "OV7725"; break;
        default: model = "UNKNOWN"; break;
    }
    snprintf(iname, 64, "%s-%s-%02X%02X%02X", CAM_BOARD, model, mac[3], mac[4], mac[5]);
    snprintf(framesize, 4, "%d", s->status.framesize);
    snprintf(pixformat, 4, "%d", s->pixformat);

    char * src = iname, * dst = hname, c;
    while (*src) {
    	c = *src++;
    	if (c >= 'A' && c <= 'Z') {
    		c -= 'A';
    	}
    	*dst++ = c;
    }

    if(mdns_init() != ESP_OK){
        ESP_LOGE(TAG, "mdns_init() Failed");
        return;
    }

    if(mdns_hostname_set(hname) != ESP_OK){
        ESP_LOGE(TAG, "mdns_hostname_set(\"%s\") Failed", hname);
        return;
    }

    if(mdns_instance_name_set(iname) != ESP_OK){
        ESP_LOGE(TAG, "mdns_instance_name_set(\"%s\") Failed", iname);
        return;
    }

    if(mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0) != ESP_OK){
        ESP_LOGE(TAG, "mdns_service_add() HTTP Failed");
        return;
    }


    mdns_txt_item_t camera_txt_data[] = {
        {(char*)"board"         ,(char*)CAM_BOARD},
        {(char*)"model"     	,(char*)model},
        {(char*)"stream_port"   ,(char*)"81"},
        {(char*)"framesize"   	,(char*)framesize},
        {(char*)"pixformat"   	,(char*)pixformat}
    };

    if(mdns_service_add(NULL, "_esp-cam", "_tcp", 80, camera_txt_data, 5)) {
        ESP_LOGE(TAG, "mdns_service_add() ESP-CAM Failed");
    }
}