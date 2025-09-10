#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"

#define LED_PIN 2               // Onboard LED
#define WIFI_SSID "UIU-STUDENT"
#define WIFI_PASS "12345678"

static const char *TAG = "RTOS_BLYNK_HTTP";
int wifi_connected = 0;

// Wi-Fi Event Handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Connecting to WiFi...");
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Retrying...");
        esp_wifi_connect();
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        wifi_connected = 1;
        ESP_LOGI(TAG, "WiFi connected!");
    }
}

// Wi-Fi Initialization
void wifi_init_sta(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
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








// Task 1: LED Blink + Blynk Update
void task_led_blynk(void *pvParameters) {
    gpio_reset_pin(LED_PIN);
gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);


    while (1) {
        gpio_set_level(LED_PIN, 1);
        ESP_LOGI(TAG, "LED ON + Blynk Update");
        vTaskDelay(pdMS_TO_TICKS(500));

        gpio_set_level(LED_PIN, 0);
        ESP_LOGI(TAG, "LED OFF");
        vTaskDelay(pdMS_TO_TICKS(500));

        // TODO: Send virtual pin update to Blynk here
        // Example: blynk_write(V1, value);
    }
}

// Task 2: HTTP GET to ThingSpeak
void task_http_get(void *pvParameters) {
    while (!wifi_connected) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    esp_http_client_config_t config = {
        .url = "http://api.thingspeak.com/channels/2545108/feeds.json?api_key=Z1GH9F767CX7ZNQD&results=1",
        .method = HTTP_METHOD_GET,
        .event_handler = client_event_get_handler
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    while (1) {
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "HTTP GET success, status = %d",
                     esp_http_client_get_status_code(client));
        } else {
            ESP_LOGE(TAG, "HTTP GET failed: %s", esp_err_to_name(err));
        }

        vTaskDelay(pdMS_TO_TICKS(5000));  // 5 sec delay
    }

    esp_http_client_cleanup(client);
}

// Main Application
void app_main(void) {
    nvs_flash_init();
    wifi_init_sta();

    // Create Task 1
    xTaskCreatePinnedToCore(task_led_blynk, "LED_BLYNK", 2048, NULL, 5, NULL, 1);

    // Create Task 2
    xTaskCreatePinnedToCore(task_http_get, "HTTP_GET",4096, NULL, 5, NULL, 1);
}
