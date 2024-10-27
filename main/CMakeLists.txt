# Edit following two lines to set component requirements (see docs)
idf_component_register(SRCS main.c rgb_led.c wifi_app.c http_server.c DHT22.c app_nvs.c wifi_reset_button.c sntp_time_sync.c mqtt_esp_client.c prometheus_pusher.c
						INCLUDE_DIRS "."
						EMBED_FILES webpage/app.css webpage/app.js webpage/favicon.ico webpage/index.html webpage/jquery-3.3.1.min.js webpage/chart.min.js webpage/chartjs-adapter-date-fns.bundle.min.js webpage/date-fns.min.js)

target_add_binary_data(${COMPONENT_TARGET} "../certs/l_ca_crt" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../certs/esp_client_crt" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../certs/l_client_key" TEXT)