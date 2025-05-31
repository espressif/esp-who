

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
// #include "protocol_examples_common.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define BROKER_URI CONFIG_MQTT_HOST
#define TOPIC_FACE_RECOGNITION CONFIG_MQTT_TOPIC_RECOGNITION
#define TOPIC_CAM_COMMAND CONFIG_MQTT_TOPIC_COMMAND
static const char *TAG = "mqtt_example";


static struct
{
    esp_mqtt_client_handle_t client;
    uint8_t connected;
} mqtt;

// static void sendCamCommand(esp_mqtt_event_handle_t event)
// {
//     recognizer_state_t state;
//     state = (recognizer_state_t)atoi(event->data);
//     xQueueSend(xQueueEvent, &state, 0);
// }

/*mqtt event handler*/
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    printf("Event received from MQTT client, event_id=%d\n", event->event_id);
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        mqtt.connected = 1;
        //ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_CAM_COMMAND, 0);
        //ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        mqtt.connected = 0;
        //ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        //ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        //ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
#ifdef APP_DEBUG
        //ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
#endif
        break;
    case MQTT_EVENT_DATA:
        // ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        // sendCamCommand(event);
#ifdef APP_DEBUG
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
#endif
        break;
    case MQTT_EVENT_ERROR:
        // ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        // ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

/* Standard esp event handler that calls the mqtt event handler */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    printf("Event received from event loop base");
    // ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb((esp_mqtt_event_handle_t)event_data);
}

void mqtt_task(void *arg)
{

        // ESP_ERROR_CHECK(esp_event_loop_create_default());
    printf("Starting MQTT client...\n");
    esp_mqtt_client_config_t mqtt_cfg = {}; // Zero-initialize the struct
    mqtt_cfg.broker.address.uri = BROKER_URI;
    mqtt_cfg.credentials.username = "caseta";
    mqtt_cfg.credentials.authentication.password = "apd14505jc";
    printf("BROKER_URI: %s\n", BROKER_URI);
    printf("mqtt_cfg.broker.address.uri: %s\n", mqtt_cfg.broker.address.uri);
    printf("Starting MQTT client...\n");
    mqtt.client = esp_mqtt_client_init(&mqtt_cfg);
    printf("Starting MQTT client...\n");
    esp_mqtt_client_register_event(mqtt.client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, mqtt.client);
    printf("Starting MQTT client...\n");
    esp_mqtt_client_start(mqtt.client);

    // Properly delete the task before exiting
    vTaskDelete(NULL);
}

void mqtt_app_start()
{
    xTaskCreatePinnedToCore(mqtt_task, "mqtt_task", 3 * 1024, NULL, 5, NULL, 1);
}

esp_err_t mqtt_publish(const char *data)
{
    if (mqtt.connected)
    {
    printf("mqttpublish\n");

    int msg_id = esp_mqtt_client_publish(mqtt.client, TOPIC_FACE_RECOGNITION, data, 0, 1, 1);
    if (msg_id < 0)
    {
        // ESP_LOGE(TAG, "Unable to publish topic to %s", BROKER_URI);
        return ESP_FAIL;
    }
        }

    return ESP_OK;
}

