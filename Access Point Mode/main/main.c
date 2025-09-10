#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"


#define AP_SSID "ESP32_AP"
#define AP_PASS "12345678"


void wifi_init_softap() {
   esp_netif_init();
   esp_event_loop_create_default();
   esp_netif_create_default_wifi_ap();
   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
   esp_wifi_init(&cfg);
  
   wifi_config_t wifi_config = {
       .ap = {
           .ssid = AP_SSID,
           .password = AP_PASS,
           .max_connection = 4,
           .authmode = WIFI_AUTH_WPA_WPA2_PSK,
       },
   };
   esp_wifi_set_mode(WIFI_MODE_AP);
   esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
   esp_wifi_start();
}
void app_main() {
   nvs_flash_init();
   wifi_init_softap();
}
