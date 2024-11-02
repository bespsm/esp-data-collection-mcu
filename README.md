# udemy_esp32

This is a clone of [Udemy IoT Application Development with the ESP32 Using the ESP-IDF Course Repository](https://github.com/kevinudemy/udemy_esp32) with the extensions. Specifically:

- changed GPIO configuration for DHT22 sensor
- extended "http_server" module.A dded "DHT22 Sensor Readings" chart to the ESP32 web server with temperature and humidity readings
- exclude from the build "aws_iot" module
- added "mqtt_esp_client" module. It works with the local MQTT server over TLS (tested on Eclipse Mosquitto) 
- added "prometheus_pusher" module. It communicates as HTTP client with Prometheus Pushgateway 
- edited "rgb_led" module. The colors are less intensive
- added predefined "sdkconfig" configuration

### Before flashing, please adapt following files to make the code fit your HW and Ethernet configuration
- edit `DHT_GPIO` macro in [DHT22.h](main/DHT22.h)
- comment/uncomment lines that start [prometheus push](https://github.com/bespsm/esp-data-collection-mcu/blob/9b3a44273e540f64a526f9a8e434dc0f91eec67c/main/main.c#L27) and [MQTT client](https://github.com/bespsm/esp-data-collection-mcu/blob/9b3a44273e540f64a526f9a8e434dc0f91eec67c/main/main.c#L24) tasks in [main.c](main/main.c)
- edit `CONFIG_MQTT_CLIENT_ID`, `CONFIG_MQTT_HOSTNAME`, `CONFIG_MQTT_TOPIC` macros to set up MQTT tasks in [mqtt_esp_client.h](main/mqtt_esp_client.h)

- edit `PROMETHEUS_PUSHGATEWAY_HOST`, `PROMETHEUS_PUSHGATEWAY_PORT`  macros to set up prometheus pusher task in [prometheus_pusher.h](main/prometheus_pusher.h)
- edit `RGB_LED_RED_GPIO`, `RGB_LED_GREEN_GPIO`, `RGB_LED_BLUE_GPIO` macros to adapt your HW configuration to RGB LED in [rgb_led.h](main/rgb_led.h)
- edit `WIFI_*` macros to set upi WI-FI configuration in [wifi_app.h](main/wifi_app.h)
- run `idf.py menuconfig` to adapt further your HW configuration

**This project is developed based on ESP-IDF version 4.4**
