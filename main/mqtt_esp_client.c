/*
 * mqtt_client.h
 *
 *  Created on: Sep 08, 2024
 *      Author: bespsm
 */


#include "esp_log.h"

#include "mqtt_client.h"
#include "mqtt_esp_client.h"
#include "DHT22.h"
#include "tasks_common.h"
#include "wifi_app.h"
#include "sntp_time_sync.h"

// task handle
static TaskHandle_t task_mqtt_client = NULL;


static const char *TAG = "MQTTS";

extern const uint8_t client_cert_pem_start[] asm("_binary_esp_client_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_esp_client_crt_end");
extern const uint8_t client_key_pem_start[] asm("_binary_l_client_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_l_client_key_end");
extern const uint8_t server_cert_pem_start[] asm("_binary_l_ca_crt_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_l_ca_crt_end");

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    char mqttJson[100];
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, CONFIG_MQTT_TOPIC, 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);


        sprintf(mqttJson, "{\"temperature\":%.1f,\"humidity\":%.1f,\"wifi_rssi\":%d,\"ts\":%u}", getTemperature(), getHumidity(), wifi_app_get_rssi(), sntp_time_sync_get_timestamp());
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1", mqttJson, strlen(mqttJson), 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);

        vTaskDelay(10000 / portTICK_PERIOD_MS);

        sprintf(mqttJson, "{\"temperature\":%.1f,\"humidity\":%.1f,\"wifi_rssi\":%d,\"ts\":%u}", getTemperature(), getHumidity(), wifi_app_get_rssi(), sntp_time_sync_get_timestamp());
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1", mqttJson, strlen(mqttJson), 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void *param)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_MQTT_HOSTNAME,
        .client_cert_pem = (const char *)client_cert_pem_start,
        .client_key_pem = (const char *)client_key_pem_start,
        .cert_pem = (const char *)server_cert_pem_start,
    };

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


void mqtt_esp_client_start(void)
{
	if (task_mqtt_client == NULL)
	{
		xTaskCreatePinnedToCore(&mqtt_app_start, "mqtt_client_task", AWS_IOT_TASK_STACK_SIZE, NULL, AWS_IOT_TASK_PRIORITY, &task_mqtt_client, AWS_IOT_TASK_CORE_ID);
	}
}




