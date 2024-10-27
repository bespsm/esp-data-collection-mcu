# udemy_esp32

This is a clone of [Udemy IoT Application Development with the ESP32 Using the ESP-IDF Course Repository](https://github.com/kevinudemy/udemy_esp32) with the extensions. Specifically:

- changed PIN configuration for DHT22 sensor
- extended "http_server" module.A dded "DHT22 Sensor Readings" chart to the ESP32 web server with temperature and humidity readings
- exclude from the build "aws_iot" module
- added "mqtt_esp_client" module. It works with the local MQTT server over TLS (tested on Eclipse Mosquitto) (uncomment "mqtt_esp_client_start()" in "main.c" and edit credentials in "mqtt_esp_client.h" to start)
- added "prometheus_pusher" module. It communicates as HTTP client with Prometheus Pushgateway (edit credentials in "prometheus_pusher.h" to start)
- edited "rgb_led" module. The colors are less intensive
- added predefined "sdkconfig" configuration