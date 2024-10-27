/*
 * mqtt_client.h
 *
 *  Created on: Sep 08, 2024
 *      Author: bespsm
 */

#ifndef MQTT_ESP_CLIENT_H_
#define MQTT_ESP_CLIENT_H_

#define CONFIG_MQTT_CLIENT_ID "Udemy_ESP32_Thing"
#define CONFIG_MQTT_HOSTNAME  "mqtts://192.168.178.51:8883"
#define CONFIG_MQTT_TOPIC     "/topic/qos1"
/**
 * Starts MQTT task.
 */
void mqtt_esp_client_start(void);

#endif /* MQTT_ESP_CLIENT_H_ */
