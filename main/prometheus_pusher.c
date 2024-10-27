/*
 * prometheus_pusher.c
 *
 *  Created on: Oct 25, 2024
 *      Author: bespsm
 */

#include "esp_log.h"
#include "esp_http_client.h"

#include "prometheus_pusher.h"
#include "DHT22.h"
#include "tasks_common.h"
#include "wifi_app.h"


#define MAX_HTTP_OUTPUT_BUFFER 128

// task handle
static TaskHandle_t task_prom_pusher = NULL;
static const char *TAG = "PROM";


esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                ESP_LOGD(TAG, "HTTP response: %s\n", output_buffer);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}

void prometheus_pusher_task(void *param)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    // mac job id unique based on esp32 MAC
    uint8_t mac[8];
    esp_efuse_mac_get_default(mac);
    char pushgateway_path[32] = {0};
    sprintf(pushgateway_path, "/metrics/job/%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3]);
    ESP_LOGI(TAG, "URL path %s\n", pushgateway_path);
    // prepare template for metrics
    char post_data_tmplt[128]  = "# TYPE temperature gauge\ntemperature %.1f\n# TYPE humidity gauge\nhumidity %.1f\n# TYPE RSSI counter\nRSSI %d\n";
    char post_data[128] = {0};

    esp_http_client_config_t config = {
        .host = PROMETHEUS_PUSHGATEWAY_HOST,
        .port = PROMETHEUS_PUSHGATEWAY_PORT,
        .path = pushgateway_path,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
        .disable_auto_redirect = false,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");

    while (1)
    {
        sprintf(post_data, post_data_tmplt, getTemperature(), getHumidity(), wifi_app_get_rssi());
        // ESP_LOGI(TAG, "POST DATA %s\n", post_data);
        esp_http_client_set_post_field(client, post_data, strlen(post_data));
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
        } else {
            ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}


void prometheus_pusher_start(void) {

    if (task_prom_pusher == NULL)
    {
        xTaskCreatePinnedToCore(&prometheus_pusher_task, "prom_pusher_task", PROMETHEUS_PUSHER_TASK_STACK_SIZE, NULL, PROMETHEUS_PUSHER_TASK_PRIORITY, &task_prom_pusher, PROMETHEUS_PUSHER_TASK_CORE_ID);
    }
}
