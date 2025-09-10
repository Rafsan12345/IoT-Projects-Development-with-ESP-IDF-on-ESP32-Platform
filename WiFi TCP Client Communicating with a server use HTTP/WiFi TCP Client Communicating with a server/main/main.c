#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
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





#define WIFI_SSID "UIU-STUDENT"
#define WIFI_PASS "12345678"

static int sta = 0;
//static const char *TAG = "WiFi_STA";

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            printf("WiFi connecting ...\n");
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_CONNECTED:
            printf("WiFi connected ...\n");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            printf("WiFi lost connection ... Retrying...\n");
            sta = 0;
            esp_wifi_connect();
            break;
        case IP_EVENT_STA_GOT_IP:
            sta = 1;
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            printf("WiFi got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
            break;
        default:
            break;
    }
}

void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("WiFi Station init finished.\n");
}




esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}





static void GET_DATA()
{
    esp_http_client_config_t config_get = {
        .url = "http://api.thingspeak.com/channels/2545108/feeds.json?api_key=Z1GH9F767CX7ZNQD&results=1",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}



void app_main(void)
{
    wifi_init_sta();

    while (sta == 0) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        printf("Please wait... trying to connect to your router [%s]\n", WIFI_SSID);
    }

    printf("WiFi connected successfully!\n");
   
    GET_DATA();
}
